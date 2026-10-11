# prepare-experimental.ps1 -- produce the experimental-track build inputs.
#
# 1. Locate an experimental UE4SS.dll (explicit -Ue4ssDll, a -GameDir with
#    UE4SS installed, or download the official 'experimental-latest' rolling
#    release zip -- the line Rai Pal also bundles).
# 2. dumpbin /exports -> UE4SS.def -> lib /def -> build-exp/UE4SS.lib
#    (import lib against the runtime's REAL export table, so the mod can only
#    link if every mangled symbol it needs exists in that runtime).
#
# The def/lib pair is regenerated per build: the rolling release replaces its
# binaries over time, and regenerating keeps the mod pinned to whatever is
# actually shipped. Nothing here is committed to the repo.
param(
    [string]$Ue4ssDll = "",
    [string]$GameDir = "",
    [switch]$VendorHeaders,
    [string]$Ue4ssRepo = "../ue4ss"   # main-lineage checkout (public headers)
)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot

function Find-Dumpbin {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) { throw "vswhere not found (VS installed?)" }
    $install = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $install) { throw "no VS with C++ tools found" }
    $dump = Get-ChildItem "$install\VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe" | Select-Object -First 1
    if (-not $dump) { throw "dumpbin not found under $install" }
    # lib.exe lives next to dumpbin; needs the VC env for link toolchain DLLs,
    # so callers run lib from a vcvars shell. We return paths only.
    return $dump.FullName
}

function Find-Lib {
    $dump = Find-Dumpbin
    return (Join-Path (Split-Path $dump) "lib.exe")
}

if (-not $Ue4ssDll -and $GameDir) {
    $cand = Get-ChildItem -Path $GameDir -Recurse -Filter UE4SS.dll -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($cand) { $Ue4ssDll = $cand.FullName }
}
if (-not $Ue4ssDll) {
    # Official rolling release. The asset name embeds the build commit; query
    # the release for the current UE4SS_*.zip rather than hardcoding it.
    Write-Host "[exp] no -Ue4ssDll/-GameDir; downloading official experimental-latest release"
    $rel = Invoke-RestMethod "https://api.github.com/repos/UE4SS-RE/RE-UE4SS/releases/tags/experimental-latest"
    $asset = $rel.assets | Where-Object { $_.name -match '^UE4SS_v\d' } | Select-Object -First 1
    if (-not $asset) { throw "no UE4SS_*.zip asset on the experimental-latest release" }
    Write-Host "[exp] release asset: $($asset.name)"
    $tmp = Join-Path $env:TEMP "ogsa-exp-runtime"
    Remove-Item -Recurse -Force $tmp -ErrorAction SilentlyContinue
    New-Item -ItemType Directory -Path $tmp | Out-Null
    $zip = Join-Path $tmp $asset.name
    Invoke-WebRequest $asset.browser_download_url -OutFile $zip
    Expand-Archive $zip -DestinationPath $tmp
    $cand = Get-ChildItem $tmp -Recurse -Filter UE4SS.dll | Select-Object -First 1
    if (-not $cand) { throw "UE4SS.dll not found in release zip" }
    $Ue4ssDll = $cand.FullName
}
if (-not (Test-Path $Ue4ssDll)) { throw "UE4SS.dll not found at '$Ue4ssDll'" }
Write-Host "[exp] runtime dll: $Ue4ssDll"

$dumpbin = Find-Dumpbin
$libexe = Find-Lib
$buildDir = Join-Path $root "build-exp"
New-Item -ItemType Directory -Force -Path $buildDir | Out-Null

# --- exports -> def ---
$dumpTxt = Join-Path $buildDir "UE4SS.exports.txt"
& $dumpbin /exports $Ue4ssDll | Out-File $dumpTxt -Encoding ascii
$names = Get-Content $dumpTxt | ForEach-Object {
    if ($_ -match '^\s*\d+\s+[0-9A-F]+\s+[0-9A-F]+\s+(\S+)$') { $Matches[1] }
} | Where-Object { $_ -and $_ -notmatch '^\d+$' }
if ($names.Count -lt 100) { throw "suspiciously few exports ($($names.Count)) from $Ue4ssDll" }
if (-not ($names | Where-Object { $_ -like '*on_cpp_mods_loaded*' })) {
    throw "this UE4SS.dll predates the main-lineage vtable (no on_cpp_mods_loaded export) - it is not an experimental-lineage runtime"
}
$def = Join-Path $buildDir "UE4SS.def"
Set-Content -Path $def -Value (@("LIBRARY UE4SS", "EXPORTS") + $names) -Encoding ascii
Write-Host "[exp] def: $def ($($names.Count) exports)"

# --- def -> import lib (needs VC env for lib.exe's toolchain) ---
$vcvars = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$vcvars = Join-Path $vcvars "VC\Auxiliary\Build\vcvars64.bat"
$libout = Join-Path $buildDir "UE4SS.lib"
cmd /c "call `"$vcvars`" >nul 2>&1 && `"$libexe`" /def:`"$def`" /machine:x64 /out:`"$libout`"" | Out-Null
if (-not (Test-Path $libout)) { throw "lib /def failed to produce $libout" }
Write-Host "[exp] import lib: $libout"

# --- optional: (re)vendor the public headers from a main-lineage checkout ---
if ($VendorHeaders) {
    $ue = Resolve-Path $Ue4ssRepo
    $inc = Join-Path $root "vendor\exp\include"
    $gen = Join-Path $root "vendor\exp\include_gen"
    New-Item -ItemType Directory -Force -Path $inc, $gen | Out-Null
    Remove-Item -Recurse -Force (Join-Path $inc '*'), (Join-Path $gen '*') -ErrorAction SilentlyContinue
    foreach ($m in @(
        "UE4SS\include\Common.hpp",
        "deps\first\String\include\String",
        "deps\first\DynamicOutput\include\DynamicOutput",
        "deps\first\File\include\File",
        "deps\first\Helpers\include\Helpers",
        "deps\first\Constructs\include\Constructs",
        "deps\first\Unreal\include\Unreal"
    )) {
        $s = Join-Path $ue $m
        if (-not (Test-Path $s)) { throw "missing in checkout: $($m.src) (is $Ue4ssRepo on main?)" }
        Copy-Item -Recurse -Force $s $inc
        Write-Host "[exp] vendored $m"
    }
    $gsrc = Join-Path $ue "deps\first\Unreal\generated_include"
    if (-not (Test-Path $gsrc)) { throw "missing UEPseudo generated_include (reconstruct via the v3.0.1 prepare first)" }
    Copy-Item -Recurse -Force "$gsrc\*" $gen
    Write-Host "[exp] vendored UEPseudo generated_include"
    Write-Host "[exp] note: Unreal headers must be the recoverable d09b7218-era tree (see AGENTS.md)"
}
Write-Host "[exp] done -- now run build-experimental.cmd"
