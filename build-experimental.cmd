@echo off
rem build-experimental.cmd — thin experimental-runtime build of the mod.
rem
rem Builds main.dll against the ABI replica of the main-lineage CppUserModBase
rem plus the vendored d09b7218-era UEPseudo headers, linked to an import lib
rem generated from a REAL experimental UE4SS.dll (run prepare-experimental.ps1
rem first, or pass -Ue4ssDll). Produces a mod that loads into experimental
rem runtimes (Rai Pal's bundled build, the official 'experimental-latest'
rem rolling release) that the v3.0.1-built DLL crashes (CppUserModBase vtable
rem grew: on_ui_init was inserted mid-vtable after v3.0.1; the main lineage
rem later appended the hook_lua overloads + on_cpp_mods_loaded after
rem render_tab).
setlocal
where cl >nul 2>&1
if errorlevel 1 call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
cd /d "%~dp0"

if not exist vendor\exp\include\Unreal\UObject.hpp (
    echo [exp] vendor\exp is missing UEPseudo headers - see prepare-experimental.ps1 -VendorHeaders
    exit /b 1
)
if not exist build-exp\UE4SS.lib (
    echo [exp] build-exp\UE4SS.lib missing - run: powershell -File prepare-experimental.ps1
    exit /b 1
)

if not exist build-exp mkdir build-exp
cl /nologo /std:c++20 /EHsc /O2 /MD /LD ^
  /DOGS_EXP_ABI /DFMT_HEADER_ONLY=1 /utf-8 /DUE_BUILD_SHIPPING=1 /DUE_GAME /DPLATFORM_WINDOWS=1 /DPLATFORM_MICROSOFT=1 /DOVERRIDE_PLATFORM_HEADER_NAME=Windows /DUBT_COMPILED_PLATFORM=Win64 /DUNICODE /D_UNICODE ^
  /I"%~dp0plugin" /I"%~dp0." ^
  /I"%~dp0vendor\exp\include" ^
  /I"%~dp0vendor\exp\include_gen" ^
  /I"%~dp0vendor\exp\include\Unreal" ^
  /I"%~dp0vendor\exp\include\Unreal\Core" ^
  /I"%~dp0vendor\exp\fmt" ^
  plugin\Mod.cpp plugin\ogs_publisher.cpp ^
  /Fe:build-exp\main.dll ^
  /link build-exp\UE4SS.lib ws2_32.lib
if errorlevel 1 exit /b 1
del /q build-exp\*.obj 2>nul
echo [exp] build-exp\main.dll - pair with an experimental UE4SS runtime ^(not v3.0.1^)
