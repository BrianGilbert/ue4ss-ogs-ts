# ue4ss-ogsa-ts

[UE4SS](https://github.com/UE4SS-RE/RE-UE4SS) C++ mod that publishes
[OGSA Telemetry Standard (OGSA-TS)](https://github.com/Open-Game-Standards-Alliance/OGSA-Telemetry-Standard)
motion telemetry from **any UE4/UE5 flatscreen game UE4SS injects** —
zero game modification, LAN-only UDP multicast. Motion platforms,
haptic hardware, and dashboards on the local network can follow the
game. Companion plugin: [uevr-ogsa-ts](https://github.com/BrianGilbert/uevr-ogsa-ts) (VR).

Support & discussion: [Discord](https://discord.gg/yyCp5D2Ccp)

Status: **in-game validated** — live pawn telemetry observed on a real
receiver in *Stray* (UE 4.27) and *The Gunk* (UE 4.26): position/yaw
tracking movement, unit-norm quaternions, pawn handover across
menu↔gameplay and level changes. Smoke suite 6/6. Built against
UE4SS v3.0.1 — **requires the official v3.0.1 runtime** (see Install).

## Supported engines

UE4SS does the engine discovery, so the support band is theirs:
**UE 4.12 – 5.x** on the UE4SS **v3.0.1** this mod builds against
(the upstream project's stated target for that line). The maintained
fork, [RE-UE4SS](https://github.com/UE4SS-RE/RE-UE4SS), now states
**UE 4.7 – 5.8**, so newer/experimental UE4SS builds cover older
engines than our pin.

UE4SS prints the detected version during startup in `UE4SS.log`
(e.g. `EngineVersion 4.11` in the PS scan section).

Two caveats from the field:

- **Below the v3.0.1 floor**: e.g. *Days Gone* is genuinely UE 4.11;
  UE4SS detects the version, then the signature scan times out
  (`GUObjectArray` / `FName_ToString` unresolved, `FName_Constructor`
  ambiguous) and the mod never loads. UE4SS itself failed to
  initialize — nothing downstream can run.
- **In-band games can still fail**: shipping builds sometimes need
  custom AOBs. Symptom: `expected at least one value` or
  `PS scan timed out` in `UE4SS.log`. Fix: place `UE4SS_Signatures/*.lua`
  overrides (e.g. `GUObjectArray.lua`) in the working directory next to
  `ue4ss.dll`, per the
  [Updating AOBs guide](https://github.com/UE4SS/UE4SS/blob/master/Guides/FixingBrokenAOBs.md).

## What it tracks — the pawn, not the camera

The motion object is the **local pawn** (`PlayerController.Pawn`), the
body the player controls. The camera is engine-smoothed and a rig
following it would feed platform motion back into telemetry; the pawn
is the rigid body whose inertial frame a motion rig reproduces.

The mod takes the newest controller with a valid pawn, so menu
spectator pawns (the engine's `DefaultPawn`) hand over to the gameplay
pawn automatically — no restart needed across menus, level loads, or
pawn swaps. Character pawns keep yaw on the actor rotation; pitch/roll
live on the mesh component, so a walking character reports yaw-only
(expected).

Sampling is cadence-gated to `motionHz` inside a **ProcessEvent
detour** — the game-thread heartbeat that sees every script call
(by-name UFunction hooks only match the base-class function node,
which every Blueprint-ticked actor bypasses).

## Install

1. Requirements: **official UE4SS v3.0.1** installed in the game (see
   [ue4ss.com](https://docs.ue4ss.com)), .NET 6 for the UE4SS UI.
   Extract the [v3.0.1 zip](https://github.com/UE4SS-RE/RE-UE4SS/releases/tag/v3.0.1)
   into the game's binary directory (the folder with the game's
   `*-Win64-Shipping.exe`).

   ⚠️ **Do not use the UE4SS build bundled with Rai Pal** — it reports
   "v3.0.1" but is an experimental build 1,100+ commits past the tag,
   with `CppUserModBase` vtable changes that crash v3.0.1-built C++
   mods (verified in the field: access violation in the mod DLL at
   startup). Replace it with the official v3.0.1 runtime.
2. Build (or take CI artifacts):
   ```powershell
   ./prepare-ue4ss.ps1          # reconstructs the pinned UE4SS 3.0.1 dep tree at ../ue4ss
   ./build-mod.cmd              # era-pinned cmake 3.31 + rust 1.79 (see AGENTS.md)
   ```
   → `build-ninja/main.dll`
3. Drop into `<ue4ss>/Mods/ogsa_telemetry/dlls/main.dll`
   (per-game: `<game>/ue4ss/Mods/...`).
4. Optional `ogsa_telemetry.ini` beside the DLL:
   ```ini
   group=239.255.17.17
   port=40123
   motionHz=60
   objectName=player
   objectType=humanoid    ; humanoid|vehicle|aircraft|watercraft|spacecraft|camera
   gameName=              ; default: the game's exe name
   ```
   (UE4/UE5 FVector layout is handled automatically by UE4SS's SDK.)
5. Pair with a receiver — e.g. ogsa-ts-dotnet's
   `dotnet run --project src/TelemetryReceive`.

### Rai Pal deployments

⚠️ Rai Pal's bundled UE4SS is **not compatible with this mod** (see the
warning in Install — experimental runtime, ABI-broken for v3.0.1 C++
mods). To keep using Rai Pal as your launcher, install the **official
UE4SS v3.0.1** into the game's binary directory yourself as above and
launch through Rai Pal or Steam — the official `dwmapi.dll` beside the
game exe wins over anything else.

For reference, the Rai Pal gotchas observed in the field (they apply to
any UE4SS mod under Rai Pal):

- **Proxy placement.** Rai Pal drops its `dwmapi.dll` + `override.txt`
  pair where it thinks the engine binaries live. If the game exe is in
  `<Game>/<Project>/Binaries/Win64/` and the proxy lands in
  `<Game>/Engine/Binaries/Win64/`, the loader never enters the DLL
  search path and UE4SS silently never runs (no `UE4SS.log` anywhere).
  Fix: copy both files next to the game exe.
- **Mod registration.** UE4SS only starts mods listed in `mods.txt` or
  carrying an `enabled.txt` in their mod folder — Rai Pal's lists only
  cover mods it installed itself. Fix: add `ogsa_telemetry : 1` to
  `Mods/mods.txt` (or create `Mods/ogsa_telemetry/enabled.txt`).

Verify with `UE4SS.log` in the install folder: it should show the
scan results, `Starting mod ogsa_telemetry`, the tick candidates the
mod found, and `OGSA-TS telemetry loaded`.

## Standalone test (no game)

`cmake --build build-ninja --target ogsa_smoke && build-ninja/ogsa_smoke.exe`
— conversion assertions + 150 synthetic ticks; exit 0 = pass. Run
TelemetryReceive beside it to watch `synthetic_pawn` on the wire.

## Layout

| Path | Contents |
|---|---|
| `plugin/Mod.cpp` | UE4SS glue (pawn lookup, K2 calls, ProcessEvent heartbeat, INI) |
| `plugin/ogsa_publisher.*` | engine-agnostic publisher (shared with uevr-ogsa-ts) |
| `OgsaTelemetry/` | vendored corpus-locked serializer (sync-core.ps1 from ogsa-ts-unreal) |
| `patches/` + `prepare-ue4ss.ps1` | pinned UE4SS v3.0.1 dep tree reconstruction |
| `Test/smoke.cpp` | standalone wire-path + math verification |
