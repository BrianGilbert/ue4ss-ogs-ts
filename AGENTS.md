# ue4ss-ogs-ts — working conventions

- Motion object is the LOCAL PAWN (PlayerController.Pawn), not the
  camera — KB projects/uevr C-20261010-MBP-08; the engine-agnostic
  publisher + smoke test are shared with uevr-ogs-ts (BrianGilbert).
- One build, for the experimental UE4SS runtime lineage (Rai Pal's
  bundle and 'experimental-latest'). CppUserModBase vtables differ between
  lineages, so the DLL doesn't load into UE4SS v3.0.1.
  - build-experimental.cmd: thin build against
    plugin/experimental/CppUserModBase.ABI.hpp — an ABI replica of
    main-lineage CppUserModBase (vtable ORDER + member layout pin; the
    runtime calls every virtual unconditionally, so all slots must
    exist) — plus vendored headers (vendor/exp: public main-lineage deps,
    the recoverable d09b7218-era UEPseudo snapshot, `fmt` 11.2.0, `zydis`
    4.1.0/zycore 1.5.0). prepare-experimental.ps1 downloads the official
    experimental-latest rolling release UE4SS.dll and generates the
    import lib FROM ITS REAL EXPORT TABLE, so the mod can only link if
    every mangled symbol exists in that runtime; CI re-verifies imports
    vs exports. NOT built from the ue4ss sources: main's UEPseudo dep is
    private (Re-UE4SS/UEPseudo 404s; the exp-latest tag pins 69a66d2,
    unrecoverable; the d09b7218 snapshot exists here only as vendor/exp).
    Re-verify the ABI replica against upstream after any runtime bump:
    diff UE4SS/include/Mod/CppUserModBase.hpp (virtual order + members)
    and re-check `on_cpp_mods_loaded` is still exported (lineage marker).
    check-experimental-abi.ps1 automates this in CI: it compares the
    replica's ordered virtual sequence against upstream main's
    CppUserModBase.hpp (gate -- add/remove/reorder fails the build with
    a diff), and confirms every upstream virtual is exported by the
    target runtime with matching overload counts (catches lineage
    mismatch). The import-vs-exports link gate alone CANNOT catch
    vtable drift -- a runtime with new/reordered slots still exports
    every old mangled symbol, which is exactly the silent crash class
    this check exists for.
    In-game validated: Hot Wheels Unleashed (UE 4.26) via Rai Pal's
    bundled runtime, 2026-10-12. Mod install gotcha: UE4SS starts only
    mods listed in mods.txt or carrying enabled.txt in the mod folder
    (Mods/ogs_telemetry/enabled.txt) — without it the mod is silently
    never scanned (no error in UE4SS.log).
  - ../ue4ss holds a main-lineage checkout (branch main; the
    'experimental-latest' TAG is a stale Dec-2024 snapshot whose vtable
    lacks the appended virtuals — do not build the replica from it).
- Test/smoke.cpp is standalone (no UE4SS, built by Test/CMakeLists.txt)
  and must stay passing; the plugin DLL itself needs a game to fully verify.
- Product docs describe current state only; history lives in the
  Open-Game-Standards knowledge base.
