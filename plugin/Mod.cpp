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
#include <chrono>
#include <fstream>
#include <string>

#include <DynamicOutput/Output.hpp>
#include <Mod/CppUserModBase.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/AActor.hpp>

#include "ogsa_publisher.h"

namespace ogsa_mod
{
using namespace RC;
using namespace RC::Unreal;

static ogsa::Publisher g_publisher;
static std::chrono::steady_clock::time_point g_lastTick{};
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

static bool SamplePawn(ogsa::telemetry::Vec3& posCm,
                       ogsa::telemetry::Quat& quat)
{
    UObject* controller = UObjectGlobals::FindFirstOf(STR("PlayerController"));
    if (controller == nullptr) return false;
    auto** pawn = controller->GetValuePtrByPropertyNameInChain<UObject*>(STR("Pawn"));
    if (pawn == nullptr || *pawn == nullptr) return false;
    AActor* actor = static_cast<AActor*>(*pawn);
    FVector loc = actor->K2_GetActorLocation();
    FRotator rot = actor->K2_GetActorRotation();
    posCm = { (float)loc.GetX(), (float)loc.GetY(), (float)loc.GetZ() };
    quat = RotatorToQuat(rot.GetPitch(), rot.GetYaw(), rot.GetRoll());
    return true;
}

// Engine:Tick runs on the game thread — safe for the K2 calls above.
static void OnEngineTickPost(UnrealScriptFunctionCallableContext&, void*)
{
    const auto now = std::chrono::steady_clock::now();
    const double dt = g_lastTick.time_since_epoch().count() == 0
        ? 1.0 / 60.0
        : std::chrono::duration<double>(now - g_lastTick).count();
    g_lastTick = now;
    ogsa::telemetry::Vec3 pos;
    ogsa::telemetry::Quat quat;
    const bool hasPose = SamplePawn(pos, quat);
    g_publisher.tick(dt, hasPose, pos, quat);
}

static void OnEngineTickPre(UnrealScriptFunctionCallableContext&, void*) {}

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
        if (!g_publisher.start(cfg))
        {
            RC::Output::send<RC::LogLevel::Error>(
                STR("[ogsa] failed to create multicast socket — mod disabled\n"));
            return;
        }
        UObjectGlobals::RegisterHook(STR("/Script/Engine.Engine:Tick"),
                                     OnEngineTickPre, OnEngineTickPost, nullptr);
        RC::Output::send<RC::LogLevel::Normal>(
            STR("[ogsa] OGSA-TS telemetry loaded: pawn tracking -> {}:{}\n"),
            std::wstring(cfg.group, cfg.group + std::strlen(cfg.group)),
            cfg.port);
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
