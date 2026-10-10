# ue4ss-ogsa-ts

[UE4SS](https://github.com/UE4SS-RE/RE-UE4SS) C++ mod that publishes
[OGSA Telemetry Standard (OGSA-TS)](https://github.com/Open-Game-Standards-Alliance/OGSA-Telemetry-Standard)
motion telemetry from **any UE4/UE5 flatscreen game UE4SS injects** —
zero game modification, LAN-only UDP multicast. Motion platforms,
haptic hardware, and dashboards on the local network can follow the
game. Companion plugin: [uevr-ogsa-ts](https://github.com/BrianGilbert/uevr-ogsa-ts) (VR).

Support & discussion: [Discord](https://discord.gg/yyCp5D2Ccp)

Status: **smoke-verified** — standalone wire-path + UE math checks pass
(6/6) and were observed end-to-end on a real receiver; the mod DLL
builds against UE4SS v3.0.1. **In-game testing pending.**

## What it tracks — the pawn, not the camera

The motion object is the **local pawn** (`PlayerController.Pawn`), the
body the player controls. The camera is engine-smoothed and a rig
following it would feed platform motion back into telemetry; the pawn
is the rigid body whose inertial frame a motion rig reproduces.

## Install

1. Requirements: UE4SS installed in the game (see
   [ue4ss.com](https://docs.ue4ss.com)), .NET 6 for the UE4SS UI.
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

## Standalone test (no game)

`cmake --build build-ninja --target ogsa_smoke && build-ninja/ogsa_smoke.exe`
— conversion assertions + 150 synthetic ticks; exit 0 = pass. Run
TelemetryReceive beside it to watch `synthetic_pawn` on the wire.

## Layout

| Path | Contents |
|---|---|
| `plugin/Mod.cpp` | UE4SS glue (pawn lookup, K2 calls, Engine:Tick hook, INI) |
| `plugin/ogsa_publisher.*` | engine-agnostic publisher (shared with uevr-ogsa-ts) |
| `OgsaTelemetry/` | vendored corpus-locked serializer (sync-core.ps1 from ogsa-ts-unreal) |
| `patches/` + `prepare-ue4ss.ps1` | pinned UE4SS v3.0.1 dep tree reconstruction |
| `Test/smoke.cpp` | standalone wire-path + math verification |
