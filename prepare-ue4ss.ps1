# prepare-ue4ss.ps1 — build the pinned UE4SS source tree this mod needs.
#
# Why this exists: UE4SS v3.0.1's build deps have moved on —
#   * deps/first/Unreal (UEPseudo) is now a private repo; the public fork
#     ashandarei/uepseudo-goa still carries the exact pinned commit
#     d09b7218 (verified by SHA).
#   * two floating-source incompatibilities are patched (patches/).
# Run from the repo root:
#   ./prepare-ue4ss.ps1 [-Ue4ssDir ..\ue4ss-v3.0.1]
# (own dir so it never clobbers a main-lineage checkout at ../ue4ss, which the
# experimental track vendors headers from — see prepare-experimental.ps1)
param(
    [string]$Ue4ssDir = "../ue4ss-v3.0.1",
    [string]$Tag = "v3.0.1",
    [string]$UnrealSha = "d09b7218bfe7392adeffb500fdeee0b42ca1cd27",
    [string]$UnrealRepo = "https://github.com/ashandarei/uepseudo-goa.git"
)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot

# v3.0.1-era submodule URLs use SSH; rewrite to https (no agent on CI)
$env:GIT_CONFIG_COUNT = 1
$env:GIT_CONFIG_KEY_0 = "url.https://github.com/.insteadOf"
$env:GIT_CONFIG_VALUE_0 = "git@github.com:"

if (-not (Test-Path "$Ue4ssDir/CMakeLists.txt")) {
    git clone --quiet https://github.com/UE4SS-RE/RE-UE4SS.git $Ue4ssDir
    if ($LASTEXITCODE -ne 0) { throw "clone RE-UE4SS failed" }
}
git -C $Ue4ssDir checkout -q $Tag
if ($LASTEXITCODE -ne 0) { throw "checkout $Tag failed" }
git -C $Ue4ssDir submodule update --init --quiet deps/first/patternsleuth
if ($LASTEXITCODE -ne 0) { throw "patternsleuth submodule failed" }

$unreal = "$Ue4ssDir/deps/first/Unreal"
if (-not (Test-Path "$unreal/include/Unreal/AActor.hpp")) {
    git clone --quiet $UnrealRepo $unreal
    if ($LASTEXITCODE -ne 0) { throw "clone UEPseudo fork failed" }
    git -C $unreal checkout -q $UnrealSha
    if (-not (Test-Path "$unreal/.git")) { throw "UEPseudo checkout incomplete" }
    Remove-Item -Recurse -Force "$unreal/.git"   # dep content, not a submodule
}

git -C $unreal apply "$root/patches/uepseudo-msvc-enum-fwddecl.patch"
git -C $unreal apply "$root/patches/uepseudo-find-template-args.patch"
Write-Host "UE4SS $Tag + UEPseudo $UnrealSha (+2 patches) ready at $Ue4ssDir"
exit 0
