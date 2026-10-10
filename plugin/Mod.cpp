// Mod.cpp — OGSA-TS telemetry for UE4SS (flatscreen UE4/UE5 games).
//
// Publishes the LOCAL PAWN's world pose (the controlled body — not the
// camera; KB ruling C-20261010-MBP-08) as OGSA-TS motion telemetry over
// LAN-only UDP multicast, from any game UE4SS injects. UE4SS provides
// engine-version-resilient UObject access (incl. the UE4/UE5 FVector
// large-world-coordinates ABI) — this mod only samples and publishes.
//
// Install: <ue4ss>/Mods/ogsa_telemetry/dlls/main.dll (+ optional INI
// beside the DLL — same keys as uevr-ogsa-ts). Distribution lives at
// github.com/BrianGilbert/ue4ss-ogsa-ts.
#ifndef NOMINMAX
#define NOMINMAX // windows.h min/max macros clash with the Unreal SDK's std::min
#endif
#include <winsock2.h>
#include <windows.h>
#include <atomic>
#include <chrono>
#include <fstream>
#include <string>

#include <DynamicOutput/Output.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/AActor.hpp>
#include <Unreal/Hooks.hpp>

#include "ogsa_publisher.h"

namespace ogsa_mod
{
using namespace RC;
using namespace RC::Unreal;

static ogsa::Publisher g_publisher;
static std::chrono::steady_clock::time_point g_lastTick{};
static std::chrono::steady_clock::time_point g_lastSample{};
static double g_sampleInterval = 1.0 / 60.0; // motionHz; set on init from config
static std::atomic<int> g_hookCalls{0};
static UObject* g_cachedController = nullptr;
static std::chrono::steady_clock::time_point g_lastControllerResolve{};
static HMODULE g_module = nullptr;

// UE FRotator::Quaternion() transcription (same as uevr-ogsa-ts)
static ogsa::telemetry::Quat RotatorToQuat(double pitchDeg, double yawDeg,
                                           double rollDeg)
{
    const double half = 3.14159265358979323846 / 180.0 / 2.0;
    const double sp = std::sin(pitchDeg * half), cp = std::cos(pitchDeg * half);
    const double sy = std::sin(yawDeg * half),   cy = std::cos(yawDeg * half);
    const double sr = std::sin(rollDeg * half),  cr = std::cos(rollDeg * half);
    return {
        (float)(cr * sp * sy - sr * cp * cy),  (float)(-cr * sp * cy - sr * cp * sy),
        (float)(cr * cp * sy - sr * sp * cy),  (float)(cr * cp * cy + sr * sp * sy),
    };
}

// Reads controller->Pawn + K2 pose. POD-only args. Isolated so its caller
// can guard it with SEH: during UE5 loading, transient controllers/pawns die
// mid-load and UEPseudo has no IsValid — an AV here just invalidates the cache.
static bool TryReadPose(UObject* controller, ogsa::telemetry::Vec3& posCm,
                        ogsa::telemetry::Quat& quat, UObject** outPawn)
{
    *outPawn = nullptr;
    auto** pawn = controller->GetValuePtrByPropertyNameInChain<UObject*>(STR("Pawn"));
    if (pawn == nullptr || *pawn == nullptr) return false;
    AActor* actor = static_cast<AActor*>(*pawn);
    FVector loc = actor->K2_GetActorLocation();
    FRotator rot = actor->K2_GetActorRotation();
    posCm = { (float)loc.GetX(), (float)loc.GetY(), (float)loc.GetZ() };
    quat = RotatorToQuat(rot.GetPitch(), rot.GetYaw(), rot.GetRoll());
    *outPawn = *pawn;
    return true;
}

// SEH wrapper: must be its own function (no C++ unwinding objects) to allow
// __try. Returns 1 on success, 0 on no-pawn, -1 on guarded access violation.
static int GuardedReadPose(UObject* controller, ogsa::telemetry::Vec3* posCm,
                           ogsa::telemetry::Quat* quat, UObject** outPawn)
{
    __try
    {
        return TryReadPose(controller, *posCm, *quat, outPawn) ? 1 : 0;
    }
    __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION
                  ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
    {
        return -1;
    }
}

static bool SamplePawn(ogsa::telemetry::Vec3& posCm,
                       ogsa::telemetry::Quat& quat,
                       std::wstring* outPawnName = nullptr)
{
    // Controller resolution is expensive (FindAllOf walks the entire object
    // array — 20-40ms on large UE5 games), so it is cached for 1s. A full walk
    // at sampling cadence saturates the game thread and stalls loading on
    // object-heavy games (observed: RoboCop UE 5.2 never finished loading).
    // The pawn is re-read from the cached controller every sample (cheap
    // property read — handovers picked up instantly), and the read is guarded
    // by SEH: if the controller dies mid-load, the fault invalidates the cache
    // and the next sample re-walks. Newest controller wins — the array is
    // append-ordered, so the last with a valid pawn is the active local
    // controller (FindFirstOf pins menu-era ones).
    const auto now = std::chrono::steady_clock::now();
    if (g_cachedController == nullptr ||
        now - g_lastControllerResolve > std::chrono::seconds(1))
    {
        g_lastControllerResolve = now;
        std::vector<UObject*> controllers;
        UObjectGlobals::FindAllOf(STR("PlayerController"), controllers);
        UObject* newest = nullptr;
        for (UObject* controller : controllers)
        {
            auto** pawn = controller->GetValuePtrByPropertyNameInChain<UObject*>(STR("Pawn"));
            if (pawn != nullptr && *pawn != nullptr)
                newest = controller;
        }
        g_cachedController = newest;
    }
    if (g_cachedController == nullptr) return false;

    ogsa::telemetry::Vec3 pos{};
    ogsa::telemetry::Quat q{};
    UObject* pawnUsed = nullptr;
    const int rc = GuardedReadPose(g_cachedController, &pos, &q, &pawnUsed);
    if (rc < 0)
    {
        static std::atomic<int> faults{0};
        const int n = ++faults;
        if (n == 1 || n % 100 == 0)
            RC::Output::send<RC::LogLevel::Warning>(
                STR("[ogsa] stale controller (AV guarded, re-walking); faults={}\n"), n);
        g_cachedController = nullptr;
        return false;
    }
    if (rc == 0 || pawnUsed == nullptr) return false;
    posCm = pos;
    quat = q;
    if (outPawnName != nullptr) *outPawnName = static_cast<AActor*>(pawnUsed)->GetName();
    return true;
}

// Engine:Tick-class events run on the game thread — safe for the K2 calls
// below. The hook source may fire many times per frame (ReceiveTick fires per
// ticking actor), so sampling is cadence-gated to motionHz before any object
// walk (FindFirstOf iterates the entire object array).
// Cadence-gated sampling shared by all tick sources. The hook sources may
// fire many times per frame, so this early-outs on the motionHz interval
// before any object walk (FindAllOf iterates the entire object array).
static void SampleAndPublish()
{
    const auto now = std::chrono::steady_clock::now();
    if (now - g_lastSample < std::chrono::duration<double>(g_sampleInterval)) return;
    const double dt = g_lastSample.time_since_epoch().count() == 0
        ? g_sampleInterval
        : std::chrono::duration<double>(now - g_lastSample).count();
    g_lastSample = now;
    g_lastTick = now;
    ogsa::telemetry::Vec3 pos;
    ogsa::telemetry::Quat quat;
    std::wstring pawnName;
    const bool hasPose = SamplePawn(pos, quat, &pawnName);
    static int samples = 0;
    if (hasPose && (++samples == 1 || samples % 600 == 0))
        RC::Output::send<RC::LogLevel::Normal>(
            STR("[ogsa] sample #{}: pawn '{}' at ({:.1f}, {:.1f}, {:.1f})\n"),
            samples, pawnName, pos.X, pos.Y, pos.Z);
    g_publisher.tick(dt, hasPose, pos, quat);
}

static void OnEngineTickPost(UnrealScriptFunctionCallableContext&, void*)
{
    SampleAndPublish();
}

static void OnEngineTickPre(UnrealScriptFunctionCallableContext&, void*) {}

// Native ProcessEvent detour — fires for every script call, game thread.
// Used as the telemetry heartbeat: UE4SS's by-name hooks only match the
// base-class UFunction node, and every BP-ticked actor executes its own
// subclass node, so name-keyed hooks miss them. Cadence-gating makes this
// cheap: one clock read per call, sampling only at motionHz.
static void OnProcessEvent(UObject* Context, UFunction*, void*)
{
    (void)Context;
    const int calls = ++g_hookCalls;
    if (calls == 1)
        RC::Output::send<RC::LogLevel::Normal>(STR("[ogsa] first ProcessEvent observed\n"));
    SampleAndPublish();
}

static std::string DefaultGameName()
{
    char path[MAX_PATH]{};
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    std::string p(path);
    const auto slash = p.find_last_of("\\/");
    const auto dot = p.find_last_of('.');
    return p.substr(slash + 1, dot - slash - 1);
}

static uint16_t ObjectTypeFromName(const std::string& s)
{
    if (s == "vehicle") return 0;
    if (s == "aircraft") return 1;
    if (s == "watercraft") return 2;
    if (s == "spacecraft") return 3;
    if (s == "camera") return 5;
    return 4; // humanoid
}

static void LoadConfig(ogsa::PublisherConfig& cfg)
{
    char dllPath[MAX_PATH]{};
    GetModuleFileNameA(g_module, dllPath, MAX_PATH);
    std::string p(dllPath);
    const auto slash = p.find_last_of("\\/");
    std::ifstream ini(slash == std::string::npos
        ? std::string("ogsa_telemetry.ini")
        : p.substr(0, slash + 1) + "ogsa_telemetry.ini");
    if (!ini) return;
    std::string line;
    while (std::getline(ini, line))
    {
        const auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        const std::string key = line.substr(0, eq), val = line.substr(eq + 1);
        if (key == "group") cfg.group = _strdup(val.c_str());
        else if (key == "port") cfg.port = atoi(val.c_str());
        else if (key == "ttl") cfg.ttl = atoi(val.c_str());
        else if (key == "motionHz") cfg.motionHz = atof(val.c_str());
        else if (key == "objectName") cfg.objectName = _strdup(val.c_str());
        else if (key == "objectType") cfg.objectType = ObjectTypeFromName(val);
        else if (key == "gameName") cfg.gameName = _strdup(val.c_str());
        // (engine= ABI flag from uevr-ogsa-ts is not needed here: UE4SS
        //  adapts the UE4/UE5 FVector layout itself)
    }
}
} // namespace ogsa_mod

class OgsaTelemetryMod : public RC::CppUserModBase
{
public:
    OgsaTelemetryMod()
    {
        ModName = STR("ogsa_telemetry");
        ModVersion = STR("0.1");
        ModAuthors = STR("BrianGilbert");
        ModDescription = STR("OGSA-TS motion telemetry: publishes the controlled pawn over UDP multicast (LAN only)");
    }

    ~OgsaTelemetryMod() override { ogsa_mod::g_publisher.stop(); }

    auto on_unreal_init() -> void override
    {
        using namespace ogsa_mod;
        ogsa::PublisherConfig cfg;
        cfg.gameName = _strdup(DefaultGameName().c_str());
        LoadConfig(cfg);
        g_sampleInterval = 1.0 / (cfg.motionHz > 0.5 ? cfg.motionHz : 60.0);
        if (!g_publisher.start(cfg))
        {
            RC::Output::send<RC::LogLevel::Error>(
                STR("[ogsa] failed to create multicast socket — mod disabled\n"));
            return;
        }

        // Per-frame hook source. By-name RegisterHook crashes when the
        // UFunction is missing (unchecked null deref in UE4SS v3.0.1) and its
        // name lookup mutates the engine FName table off-thread — so resolve
        // candidates with a read-only object walk and register by pointer.
        // /Script/Engine.Engine:Tick does NOT exist as a UFunction in UE4
        // (it is a C++ virtual); Actor:ReceiveTick is the reflected Tick.
        UFunction* primaryTarget = nullptr;   // native per-frame (detour path)
        UFunction* fallbackTarget = nullptr;   // script tick (exact-name path)
        int found = 0;
        UObjectGlobals::ForEachUObject([&](UObject* Object, int32_t, int32_t) {
            if (found >= 24) return LoopAction::Break;
            auto FullName = Object->GetFullName();
            if (FullName.find(STR("Function ")) != 0) return LoopAction::Continue;
            auto NameNoType = FullName.substr(FullName.find(STR(" ")) + 1);
            bool primary =
                NameNoType == STR("/Script/Engine.Engine:Tick") ||
                NameNoType == STR("/Script/Engine.GameEngine:Tick") ||
                NameNoType == STR("/Script/Engine.GameViewportClient:PostRender") ||
                NameNoType == STR("/Script/Engine.HUD:PostRender") ||
                NameNoType == STR("/Script/Engine.HUD:DrawHUD");
            bool fallback = NameNoType == STR("/Script/Engine.Actor:ReceiveTick");
            if (!primary && !fallback) return LoopAction::Continue;
            ++found;
            RC::Output::send<RC::LogLevel::Normal>(STR("[ogsa] tick candidate: {}{}\n"),
                NameNoType, primary ? STR("") : STR(" (script)"));
            if (primary && (primaryTarget == nullptr ||
                            NameNoType == STR("/Script/Engine.HUD:DrawHUD")))
                primaryTarget = static_cast<UFunction*>(Object);
            if (fallback && fallbackTarget == nullptr)
                fallbackTarget = static_cast<UFunction*>(Object);
            return LoopAction::Continue;
        });
        int hooks = 0;
        if (primaryTarget)
        {
            UObjectGlobals::RegisterHook(primaryTarget, OnEngineTickPre, OnEngineTickPost, nullptr);
            ++hooks;
        }
        if (fallbackTarget && fallbackTarget != primaryTarget)
        {
            UObjectGlobals::RegisterHook(fallbackTarget, OnEngineTickPre, OnEngineTickPost, nullptr);
            ++hooks;
        }
        // Primary heartbeat: fires for every ProcessEvent call on the game
        // thread. By-name hooks only match the base-class UFunction node, but
        // every BP-ticked actor executes its own subclass node — the ProcessEvent
        // detour sees all of them. Cadence-gated in SampleAndPublish.
        Hook::RegisterProcessEventPreCallback(OnProcessEvent);
        RC::Output::send<RC::LogLevel::Normal>(
            STR("[ogsa] {} tick hook(s) + ProcessEvent heartbeat active\n"), hooks);
        RC::Output::send<RC::LogLevel::Normal>(
            STR("[ogsa] OGSA-TS telemetry loaded: pawn tracking -> {}:{} (game={})\n"),
            std::wstring(cfg.group, cfg.group + std::strlen(cfg.group)),
            cfg.port,
            std::wstring(cfg.gameName, cfg.gameName + std::strlen(cfg.gameName)));
    }
};

extern "C"
{
    __declspec(dllexport) RC::CppUserModBase* start_mod() { return new OgsaTelemetryMod(); }
    __declspec(dllexport) void uninstall_mod(RC::CppUserModBase* mod) { delete mod; }
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) ogsa_mod::g_module = module;
    return TRUE;
}
