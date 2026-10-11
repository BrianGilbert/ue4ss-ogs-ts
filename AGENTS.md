# ue4ss-ogsa-ts — working conventions

- Motion object is the LOCAL PAWN (PlayerController.Pawn), not the
  camera — KB projects/uevr C-20261010-MBP-08; the engine-agnostic
  publisher + smoke test are shared with uevr-ogsa-ts (BrianGilbert).
- TWO build tracks from the same plugin sources, pairing with different
  UE4SS runtime lineages (CppUserModBase vtables are incompatible
  across them — a DLL only loads into its own lineage):
  - v3.0.1 track (build-mod.cmd): full tree build against reconstructed
    UE4SS v3.0.1 (./prepare-ue4ss.ps1 reconstructs the dep tree at
    ../ue4ss-v3.0.1 — own dir so it never clobbers ../ue4ss). Era-pinned
    toolchain: cmake 3.31 (4.x mislinks corrosion) + rust 1.79 via mise
    (newer rustc confuses the v3.0.1-era corrosion; build-mod also
    strips the phantom /defaultlib defensively). FindRust quirks handled
    in build-mod.cmd: explicit Rust_COMPILER to the ~/.rustup toolchain
    binary + Rust_RUSTUP=NOTFOUND (mise manages toolchains outside
    rustup's registry). In-game validated.
  - experimental track (build-experimental.cmd): thin build against
    plugin/experimental/CppUserModBase.ABI.hpp — an ABI-exact replica of
    main-lineage CppUserModBase (vtable ORDER + member layout pin; the
    runtime calls every virtual unconditionally, so all slots must
    exist) — plus vendored headers (vendor/exp: public main-lineage deps,
    the recoverable d09b7218-era UEPseudo snapshot, fmt 11.2.0, zydis
    4.1.0/zycore 1.5.0). prepare-experimental.ps1 downloads the official
    experimental-latest rolling release UE4SS.dll and generates the
    import lib FROM ITS REAL EXPORT TABLE, so the mod can only link if
    every mangled symbol exists in that runtime; CI re-verifies imports
    vs exports. NOT built from the ue4ss sources: main's UEPseudo dep is
    private (Re-UE4SS/UEPseudo 404s; the exp-latest tag pins 69a66d2,
    unrecoverable — only d09b7218 survives via ashandarei/uepseudo-goa).
    Re-verify the ABI replica against upstream after any runtime bump:
    diff UE4SS/include/Mod/CppUserModBase.hpp (virtual order + members)
    and re-check on_cpp_mods_loaded is still exported (lineage marker).
    In-game validated: Hot Wheels Unleashed (UE 4.26) via Rai Pal's
    bundled runtime, 2026-10-12. Mod install gotcha: UE4SS starts only
    mods listed in mods.txt or carrying enabled.txt in the mod folder
    (Mods/ogsa_telemetry/enabled.txt) — without it the mod is silently
    never scanned (no error in UE4SS.log).
  - ../ue4ss holds a main-lineage checkout (branch main; the
    'experimental-latest' TAG is a stale Dec-2024 snapshot whose vtable
    lacks the appended virtuals — do not build the replica from it).
- Known dep drift handled in CMakeLists/patches: ImGuiColorTextEdit
  re-pinned to the v3.0.1-era commit (upstream pins floating master);
  UEPseudo's pre-STL windows.h assumption satisfied via the
  NOMINMAX-safe /FI shim (patches/winforce.h).
- Test/smoke.cpp is standalone (no UE4SS) and must stay passing; the
  plugin DLL itself needs a game to fully verify.
- Product docs describe current state only; history lives in the
  Open-Game-Standards-Alliance knowledge base.
