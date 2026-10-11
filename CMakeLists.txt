cmake_minimum_required(VERSION 3.22)
project(ue4ss_ogsa_ts CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# UE4SS v3.0.1 source tree reconstructed by ./prepare-ue4ss.ps1 (own dir:
# ../ue4ss-v3.0.1 — ../ue4ss may hold a main-lineage checkout for the
# experimental track). Override with -DUE4SS_DIR=
set(UE4SS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../ue4ss-v3.0.1" CACHE PATH
    "UE4SS v3.0.1 source tree (run prepare-ue4ss.ps1 first)")

# Standalone smoke test — same publisher code, no UE4SS coupling
add_executable(ogsa_smoke Test/smoke.cpp plugin/ogsa_publisher.cpp)
target_include_directories(ogsa_smoke PRIVATE vendor plugin .)
target_link_libraries(ogsa_smoke ws2_32)

# The mod DLL (installed as Mods/ogsa_telemetry/dlls/main.dll)
include(FetchContent)

# UE4SS v3.0.1 pins ImGuiColorTextEdit to the floating 'master' branch, which
# has since drifted past the pinned imgui (PushTabStop). Re-pin to the last
# v3.0.1-era commit. First declaration wins — must precede add_subdirectory.
FetchContent_Declare(imguitextedit
    GIT_REPOSITORY https://github.com/UE4SS-RE/ImGuiColorTextEdit.git
    GIT_TAG af7821926251feca84e35f8fa83eee84dae90424)

add_subdirectory(${UE4SS_DIR} ue4ss-build)

# UEPseudo assumes windows.h leaks in via the STL (true on the MSVC that
# built v3.0.1; false today) — force-include our NOMINMAX shim instead of
# raw Windows.h so the min/max macros don't break std::min/std::max.
target_compile_options(Unreal PRIVATE "/FI${CMAKE_SOURCE_DIR}/patches/winforce.h")

add_library(ogsa_telemetry SHARED plugin/Mod.cpp plugin/ogsa_publisher.cpp)
set_target_properties(ogsa_telemetry PROPERTIES OUTPUT_NAME "main")
target_include_directories(ogsa_telemetry PRIVATE plugin .)
target_link_libraries(ogsa_telemetry PRIVATE UE4SS ws2_32)
