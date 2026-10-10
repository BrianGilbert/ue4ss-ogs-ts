@echo off
rem build-mod.cmd — configure (once) + build the UE4SS mod.
rem MSVC + Ninja, UE4SS v3.0.1 triplet config. Era-pinned toolchain:
rem cmake 3.31 + rust 1.79 (see AGENTS.md — newer versions break the
rem v3.0.1-era corrosion in two different ways).
setlocal
rem MSVC env: use an existing dev environment if present (CI's
rem msvc-dev-cmd), else set it up from the local VS install
where cl >nul 2>&1
if errorlevel 1 call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
set PATH=%PATH%;%USERPROFILE%\.cargo\bin
set GIT_CONFIG_COUNT=1
set GIT_CONFIG_KEY_0=url.https://github.com/.insteadOf
set GIT_CONFIG_VALUE_0=git@github.com:
cd /d "%~dp0"
if exist build-ninja\build.ninja goto build

set "RUSTC=%USERPROFILE:\=/%/.rustup/toolchains/1.79.0-x86_64-pc-windows-msvc/bin/rustc.exe"
set "CARGOEXE=%USERPROFILE:\=/%/.rustup/toolchains/1.79.0-x86_64-pc-windows-msvc/bin/cargo.exe"
cmake -G Ninja -B build-ninja -S . ^
  -DCMAKE_BUILD_TYPE=Game__Shipping__Win64 ^
  -DRust_COMPILER="%RUSTC%" ^
  -DRust_CARGO="%CARGOEXE%" ^
  -DRust_RUSTUP=Rust_RUSTUP-NOTFOUND
if errorlevel 1 exit /b 1
powershell -NoProfile -ExecutionPolicy Bypass -File strip-defaultlib.ps1
if errorlevel 1 exit /b 1

:build
cmake --build build-ninja --target ogsa_telemetry
if errorlevel 1 exit /b 1
