# ue4ss-ogs-ts

> [!IMPORTANT]
> **In active development.** Validated in The Gunk, Stray, Hot Wheels
> Unleashed and RoboCop: Rogue City, but every Unreal game sets up its
> player differently, so expect the odd title where tracking needs a
> config tweak. Please report what you find on
> [Discord](https://discord.gg/yyCp5D2Ccp). If a game misbehaves and I
> don't already own it, a key for it is the fastest way to get it fixed.
>
> Development is funded by
> [sponsorship](https://github.com/sponsors/BrianGilbert), which moves this
> and the rest of the [OGS](https://github.com/Open-Game-Standards)
> work forward.

[UE4SS](https://github.com/UE4SS-RE/RE-UE4SS) C++ mod that publishes
[OGS Telemetry Standard (OGS-TS)](https://github.com/Open-Game-Standards/OGS-Telemetry-Standard)
motion telemetry from **any UE4/UE5 flatscreen game UE4SS injects** —
zero game modification, LAN-only UDP multicast. Motion platforms,
haptic hardware, and dashboards on the local network can follow the
game. Companion plugin: [uevr-ogs-ts](https://github.com/BrianGilbert/uevr-ogs-ts) (VR).

Support & discussion: [Discord](https://discord.gg/yyCp5D2Ccp)

Status: **in-game validated** on Hot Wheels Unleashed (UE 4.26) through
Rai Pal's bundled runtime. The mod builds against the official
**`experimental-latest` rolling release** of UE4SS, the line Rai Pal
bundles. It doesn't load into UE4SS v3.0.1; releases up to `v2026.11.04`
include a DLL for that runtime.

## Supported engines

UE4SS does the engine discovery, so the support band is theirs. The
maintained fork, [RE-UE4SS](https://github.com/UE4SS-RE/RE-UE4SS), states
**UE 4.7 – 5.8**.

Validated in-game with this mod:

| Game | Engine | Notes |
|---|---|---|
| Hot Wheels Unleashed | UE 4.26 | current build, Rai Pal's bundled runtime |
| The Gunk | UE 4.26 | UE4SS v3.0.1 build, not retested with the current one |
| Stray | UE 4.27 | UE4SS v3.0.1 build, not retested with the current one |
| RoboCop: Rogue City | UE 5.2 | UE4SS v3.0.1 build, not retested with the current one |

UE4SS prints the detected version during startup in `UE4SS.log`
(for example `EngineVersion 4.11` in the PS scan section).

Two caveats from the field:

- **Older engines can fail**: *Days Gone* is UE 4.11. With UE4SS v3.0.1,
  UE4SS detected the version, then the signature scan timed out
  (`GUObjectArray` / `FName_ToString` unresolved, `FName_Constructor`
  ambiguous) and the mod never loaded. When UE4SS fails to initialize,
  nothing downstream can run.
- **In-band games can still fail**: shipping builds sometimes need
  custom AOBs. Symptom: `expected at least one value` or
  `PS scan timed out` in `UE4SS.log`. Fix: place `UE4SS_Signatures/*.lua`
  overrides (for example `GUObjectArray.lua`) in the working directory next to
  `ue4ss.dll`, per the
  [Updating AOBs guide](https://github.com/UE4SS/UE4SS/blob/master/Guides/FixingBrokenAOBs.md).

## What it tracks — the pawn, not the camera

The motion object is the **local pawn** (`PlayerController.Pawn`), the
body the player controls. The camera is engine-smoothed and a rig
following it would feed platform motion back into telemetry; the pawn
is the rigid body whose inertial frame a motion rig reproduces.

The mod takes the newest controller with a valid pawn (cached for 1 s
with an SEH guard — transient controllers die during UE5 load
transitions, and a full object-array walk per sample saturates the
game thread on object-heavy games and stalls loading), so menu
spectator pawns (the engine's `DefaultPawn`) hand over to the gameplay
pawn automatically — no restart needed across menus, level loads, or
pawn swaps. Character pawns keep yaw on the actor rotation; pitch/roll
live on the mesh component, so a walking character reports yaw-only
(expected).

Sampling is cadence-gated to `motionHz` inside a **ProcessEvent
detour** — the game-thread heartbeat that sees every script call
(by-name `UFunction` hooks only match the base-class function node,
which every Blueprint-ticked actor bypasses).

## Install

### With Rai Pal

In [Rai Pal](https://github.com/Raicuparta/rai-pal), select the game and
install **OGS Telemetry (UE4SS)**. It uses Rai Pal's UE4SS, so there's
nothing to download or copy. Then run the game through Rai Pal. The optional
settings and the receiver steps below still apply.

### Without Rai Pal

1. Copy `experimental/ogs_telemetry/` from the release zip into
   `<ue4ss>/Mods/`. That's the whole install. It needs the
   [`experimental-latest`](https://github.com/UE4SS-RE/RE-UE4SS/releases/tag/experimental-latest)
   rolling release of UE4SS. The folder holds
   `dlls/main.dll` plus an `enabled.txt` (UE4SS only starts mods listed in
   `mods.txt` or carrying `enabled.txt`; without it the mod is silently never
   loaded, with no error in `UE4SS.log`).
2. Or build it, with Visual Studio 2022's C++ tools:
   ```powershell
   ./prepare-experimental.ps1   # downloads the official experimental runtime,
                                # generates UE4SS.lib from its real export table
   ./build-experimental.cmd     # ABI-replica thin build (see AGENTS.md)
   # -> build-exp/main.dll
   ```
3. If you built from source instead, drop into
   `<ue4ss>/Mods/ogs_telemetry/dlls/main.dll`
   (per-game: `<game>/ue4ss/Mods/...`) and create an empty
   `<ue4ss>/Mods/ogs_telemetry/enabled.txt` — same layout the release
   zip ships.
4. Optional `ogs_telemetry.ini` beside the DLL:
   ```ini
   group=239.255.17.17
   port=40123
   motionHz=60
   objectName=player
   objectType=humanoid    ; humanoid|vehicle|aircraft|watercraft|spacecraft|camera|creature|other
   dynamics=on           ; on (default) | off — bodyDynamics variant in motion frames
   pause=on              ; on (default) | off — report pauses on session.paused (C-11)
   gameName=             ; default: the game's exe name, pack/build tokens
                         ; stripped ("Game-Win64-Shipping" -> "Game")
   ```
   The tracked pawn's class name is surfaced automatically as the object's
   `typeLabel` in discovery (for example "BP_PlayerCharacter_C"), re-sent when
   possession changes under a new discovery `generation` (the label is
   object identity; motion frames carry the generation too);
   `objectName` stays the stable matching key.
   (UE4/UE5 `FVector` layout is handled automatically by UE4SS's SDK.)

   Motion frames carry pose **plus the `bodyDynamics` variant** — specific
   force (m/s², accelerometer-style: `f = a − g`, +1 g up at rest) and
   angular velocity (rad/s, C-4 body-frame sense), derived per engine tick
   from the pawn pose and the engine's `GetVelocity`. While that reading is
   missing or disagrees with how the pawn moves, velocity comes from position
   deltas, which are noisier. That takes three failed half-second checks in a
   row. A check where the pawn didn't move at all isn't counted, and one where
   it barely moved isn't counted for up to six in a row.
   `ogs_telemetry_debug.log` records each failure and the source in use.
   Movement is timed with the game's time dilation, so slow motion, such as a
   popup that eases the game to a stop, doesn't read as braking.
   Acceleration is low-pass filtered (τ = 0.1 s), teleports
   reset the estimator, and rates come from quaternion deltas — no Euler
   representation-flip artifacts (see GameConn `docs/cueing.md` for the
   receiver-side value).
5. Pair with a receiver, for example ogs-ts-dotnet's
   `dotnet run --project src/TelemetryReceive`.

### If it doesn't load through Rai Pal

- **No `UE4SS.log` anywhere** (observed on The Gunk): Rai Pal can put its
  `dwmapi.dll` and `override.txt` in `Engine/Binaries/Win64/`, where Windows
  never loads them. Move both into the folder with the game's
  `*-Win64-Shipping.exe`.
- **Updates have no effect:** an older manual UE4SS install in the game
  folder, with its own `UE4SS.dll` and `Mods/`, takes the place of Rai Pal's.
  Remove it. The mod rewrites `ogs_telemetry_debug.log` beside its DLL at
  every launch, so the log shows which copy ran.

When the mod loads, `UE4SS.log` shows the scan results,
`Starting mod ogs_telemetry`, the tick candidates the mod found and
`OGS-TS telemetry loaded`.

## Standalone test (no game)

```powershell
cmake -S Test -B Test/build
cmake --build Test/build --config Release --target ogs_smoke
Test/build/Release/ogs_smoke.exe
```

It checks the conversions, the wire path and the derived dynamics with
synthetic ticks; exit 0 means pass. Run TelemetryReceive beside it to watch
the synthetic objects on the wire.

## Layout

| Path | Contents |
|---|---|
| `plugin/Mod.cpp` | UE4SS glue (pawn lookup, K2 calls, ProcessEvent heartbeat, INI) |
| `plugin/experimental/CppUserModBase.ABI.hpp` | ABI replica of the experimental-lineage mod base (vtable + layout pin) |
| `plugin/ogs_publisher.*` | engine-agnostic publisher (shared with uevr-ogs-ts) |
| `OgsTelemetry/` | vendored canonical-encoding serializer (sync-core.ps1 from ogs-ts-unreal), corpus-locked |
| `vendor/exp/` | vendored headers: public main-lineage deps + recoverable UEPseudo snapshot + `fmt` 11.2.0 |
| `prepare-experimental.ps1` + `build-experimental.cmd` | import lib from the real runtime's exports, thin build |
| `Test/` | standalone smoke test (wire path and math) and corpus conformance |
