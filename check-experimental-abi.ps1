# check-experimental-abi.ps1 — guard the CppUserModBase ABI replica.
#
# The dangerous drift is vtable drift: a runtime that gained/reordered
# vtable slots still exports every OLD symbol, so the linker's import
# gate passes and a short/misaligned-vtable mod crashes at runtime
# (the exact crash class that broke v3.0.1 mods on experimental
# runtimes). Export tables cannot distinguish virtual from non-virtual
# members (register_tab etc. are exported too), so the PRIMARY source
# for the virtual set + order is upstream main's CppUserModBase.hpp
# (rolling releases are cut from main); the target runtime's export
# table CONFIRMS those virtuals actually ship.
#
# Checks, in order:
#   A. REPLICA vs UPSTREAM (gate): ordered virtual sequence and
#      overload counts must match exactly. Any add/remove/reorder in
#      main means the replica no longer pins the real layout -> fail
#      with the diff and the fix pointer.
#   B. RUNTIME vs UPSTREAM (gate): every upstream virtual must be
#      exported by the target runtime with >= the header's overload
#      count (catches lineage mismatch, e.g. a stale-tag runtime).
#   C. RUNTIME vs REPLICA subset (fallback): if the upstream fetch
#      fails (offline local run), only the subset check runs, with a
#      warning that the full order check was skipped.
param(
    [string]$ExportsFile = "build-exp/UE4SS.exports.txt",
    [string]$ReplicaHeader = "plugin/experimental/CppUserModBase.ABI.hpp",
    [string]$UpstreamUrl = "https://raw.githubusercontent.com/UE4SS-RE/RE-UE4SS/main/UE4SS/include/Mod/CppUserModBase.hpp"
)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
Set-Location $root

# Extract the ordered virtual-name sequence from a CppUserModBase
# class body (handles RC_UE4SS_API-prefixed and plain virtuals, and the
# virtual dtor). Non-virtual members (register_tab, ...) are ignored.
function Get-VirtualOrder([string[]]$lines) {
    $order = @()
    $inClass = $false
    foreach ($line in $lines) {
        if ($line -match '^\s*class\s+.*CppUserModBase') { $inClass = $true; continue }
        if (-not $inClass) { continue }
        if ($line -match '^\s*};') { break }
        if ($line -match '(?:RC_UE4SS_API\s+)?virtual\s+auto\s+~(\w+)') { $order += ('~' + $Matches[1]); continue }
        if ($line -match '(?:RC_UE4SS_API\s+)?virtual\s+auto\s+(\w+)') { $order += $Matches[1] }
    }
    return $order
}

if (-not (Test-Path $ReplicaHeader)) { throw "replica header not found: $ReplicaHeader" }
$replicaOrder = Get-VirtualOrder (Get-Content $ReplicaHeader)
if ($replicaOrder.Count -eq 0) { throw "no virtuals parsed from $ReplicaHeader -- parser broken?" }

# runtime export map (name -> count), CppUserModBase members only
if (-not (Test-Path $ExportsFile)) { throw "exports file not found: $ExportsFile (run prepare-experimental.ps1 first)" }
$runtimeMap = @{}
foreach ($line in (Get-Content $ExportsFile)) {
    if ($line -match '^\s*\d+\s+[0-9A-F]+\s+[0-9A-F]+\s+\?(\w+)@CppUserModBase@RC@@') {
        $n = $Matches[1]
        if (-not $runtimeMap.ContainsKey($n)) { $runtimeMap[$n] = 0 }
        $runtimeMap[$n]++
    }
}
if ($runtimeMap.Count -eq 0) { throw "no CppUserModBase exports found in $ExportsFile -- wrong runtime lineage?" }

# count replica virtuals per name (skip dtor: not name-exported per overload)
$replicaCounts = @{}
foreach ($v in $replicaOrder) {
    if ($v.StartsWith('~')) { continue }
    if (-not $replicaCounts.ContainsKey($v)) { $replicaCounts[$v] = 0 }
    $replicaCounts[$v]++
}

$fail = @()

# ---- A/B: upstream as primary source ----
$upstreamFetched = $false
try {
    $upstream = (Invoke-WebRequest -Uri $UpstreamUrl -UseBasicParsing -TimeoutSec 30).Content -split "`n"
    $upstreamFetched = $true
    $upstreamOrder = Get-VirtualOrder $upstream

    if (($upstreamOrder -join '|') -ne ($replicaOrder -join '|')) {
        $fail += "virtual sequence differs (upstream vs replica):"
        $fail += "  upstream: $($upstreamOrder -join ' -> ')"
        $fail += "  replica : $($replicaOrder -join ' -> ')"
    }

    $upstreamCounts = @{}
    foreach ($v in $upstreamOrder) {
        if ($v.StartsWith('~')) { continue }
        if (-not $upstreamCounts.ContainsKey($v)) { $upstreamCounts[$v] = 0 }
        $upstreamCounts[$v]++
    }
    foreach ($name in $upstreamCounts.Keys) {
        if (-not $runtimeMap.ContainsKey($name)) { $fail += "upstream virtual '$name' is NOT exported by the target runtime -- lineage mismatch (stale-tag runtime?) or renamed upstream" }
        elseif ($runtimeMap[$name] -lt $upstreamCounts[$name]) { $fail += "upstream virtual '$name' x$($upstreamCounts[$name]) but runtime exports only x$($runtimeMap[$name])" }
    }
} catch {
    Write-Host "::warning::upstream fetch failed ($($_.Exception.Message)) -- running fallback subset check only"
}

# ---- C: runtime subset check (always runs) ----
foreach ($name in $replicaCounts.Keys) {
    if (-not $runtimeMap.ContainsKey($name)) { $fail += "replica virtual '$name' is not exported by the target runtime (removed upstream?)" }
    elseif ($runtimeMap[$name] -lt $replicaCounts[$name]) { $fail += "replica virtual '$name' x$($replicaCounts[$name]) but runtime exports only x$($runtimeMap[$name])" }
}
$knownVirtualExports = ($runtimeMap.Keys | Where-Object { $replicaCounts.ContainsKey($_) }).Count
if ($knownVirtualExports -eq 0) { $fail += "no overlap between runtime exports and replica virtuals -- catastrophic drift" }

if ($fail.Count -gt 0) {
    Write-Host "::error::CppUserModBase ABI replica is out of sync"
    $fail | ForEach-Object { Write-Host "  $_" }
    Write-Host "Fix: update $ReplicaHeader (vtable order + member layout) per AGENTS.md, experimental track."
    exit 1
}
Write-Host "[abi] runtime subset OK: all $($replicaCounts.Count) replica virtuals exported by the target runtime"
if ($upstreamFetched) { Write-Host "[abi] upstream gate OK: replica virtual order matches main ($($replicaOrder.Count) slots incl. dtor)" }
