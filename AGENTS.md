# ue4ss-ogsa-ts — working conventions

- Motion object is the LOCAL PAWN (PlayerController.Pawn), not the
  camera — KB projects/uevr C-20261010-MBP-08; the engine-agnostic
  publisher + smoke test are shared with uevr-ogsa-ts (BrianGilbert).
- Builds against UE4SS v3.0.1 (the newest tag whose UEPseudo dependency
  is publicly recoverable). ./prepare-ue4ss.ps1 reconstructs the dep
  tree (fork-pinned UEPseudo @ d09b7218 + 2 patches); ./build-mod.cmd
  builds main.dll (Mods/ogsa_telemetry/dlls/).
- Era-pinned toolchain, both required, neither optional:
    cmake 3.31 (4.x mislinks corrosion's rust static-lib directive),
    rust 1.79 via mise (newer rustc emits a native-static-lib note the
    v3.0.1-era corrosion misparses into a phantom /defaultlib:msvcrt —
    build-mod also strips it defensively via strip-defaultlib.ps1).
  FindRust quirks handled in build-mod.cmd: explicit Rust_COMPILER to
  the ~/.rustup toolchain binary + Rust_RUSTUP=NOTFOUND (mise manages
  toolchains outside rustup's registry).
- Known dep drift handled in CMakeLists/patches: ImGuiColorTextEdit
  re-pinned to the v3.0.1-era commit (upstream pins floating master);
  UEPseudo's pre-STL windows.h assumption satisfied via the
  NOMINMAX-safe /FI shim (patches/winforce.h).
- Test/smoke.cpp is standalone (no UE4SS) and must stay passing; the
  plugin DLL itself needs a game to fully verify.
- Product docs describe current state only; history lives in the
  Open-Game-Standards-Alliance knowledge base.
