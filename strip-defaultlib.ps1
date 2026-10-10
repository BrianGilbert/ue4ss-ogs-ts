# strip-defaultlib.ps1 — the v3.0.1-era corrosion vendored by UE4SS captures
# rustc's "note: native-static-lib: /defaultlib:msvcrt" directive and adds it
# to the link line as if it were a library file; ninja then fails resolving
# it. Era CI built before rustc emitted that note this way. Strip the bogus
# token from the generated ninja rules after configure (idempotent).
$f = "build-ninja/build.ninja"
if (-not (Test-Path $f)) { throw "no build.ninja at $f" }
$lines = Get-Content $f
$clean = $lines | ForEach-Object { $_ -replace '\\defaultlib\$:msvcrt\s*', '' -replace '\\defaultlib:msvcrt\s*', '' }
if ($clean.Count -ne $lines.Count) { throw "line count changed?" }
$clean | Set-Content $f -Encoding ascii
Write-Host "stripped /defaultlib:msvcrt tokens from build.ninja"
