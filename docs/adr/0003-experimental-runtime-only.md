# 3. Build for the experimental UE4SS runtime only

Date: 2026-11-26. Status: accepted.

## Context

The repo built two DLLs: one for UE4SS v3.0.1 and one for the experimental
runtime. The v3.0.1 build reconstructed UE4SS's source tree, taking its
private `UEPseudo` dependency from a public fork,
`ashandarei/uepseudo-goa`. GitHub now answers "not found" for that fork. CI
last cloned it on 2026-11-04 and failed to on 2026-11-12 and 2026-11-26.
Releases waited for that job, so none was published, and the standalone smoke
test ran inside it, so CI didn't run that either.

Rai Pal bundles the experimental runtime.

## Decision

Build and release only the experimental DLL. Remove the v3.0.1 scripts,
patches and CMake project. The smoke test builds from `Test/CMakeLists.txt`,
and a release waits for the conformance job and the experimental build, which
runs the smoke test. The release zip keeps its `experimental/` folder.

The runtime download passes the workflow's token to the GitHub API. An
anonymous request hit the rate limit on a shared build machine on 2026-11-12.

## Consequences

- New releases don't load into UE4SS v3.0.1. Releases up to `v2026.11.04`
  include that DLL.
- The game results for The Gunk, Stray and RoboCop: Rogue City come from
  v3.0.1 builds.
- Bringing v3.0.1 back needs an authorized source of `UEPseudo`.
