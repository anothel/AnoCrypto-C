# AnoCrypto-C CI Matrix Skeleton

Status: P1-10 draft.

## Purpose

This matrix records the first CI coverage skeleton for build, test, public-surface checks, install-package checks, and evidence-draft generation. It should be reviewed and tightened before being used for release-candidate decisions.

## Required checks

| Check | Linux | macOS | Windows | Notes |
|---|---:|---:|---:|---|
| Configure C99 project | Required | Required | Required | `CMAKE_C_STANDARD=99`, extensions off. |
| Build shared library | Required | Required | Required | Candidate module boundary is shared-library first. |
| Strict warnings | Required | Required | Required | MSVC uses `/W4`; GCC/Clang use warning flags from CMake. |
| CTest unit/KAT/negative tests | Required | Required | Required | Output must remain status-only. |
| Build-tree install smoke | Required | Required | Required | Uses linked build target. |
| Installed-package smoke | Required | Required | Required | Uses `find_package(AnoCryptoC CONFIG REQUIRED)`. |
| Public surface policy check | Required | Required | Required | Checks naming, claims boundary, headers, package template presence, and Linux exported symbols when available. |
| Evidence draft target | Linux required | Optional | Optional | Cross-platform evidence layout still needs review. |
| Platform entropy provider branch | Linux provider | Apple provider | Windows provider | Provider tests must not print entropy bytes. |

## Suggested matrix axes

| Axis | Values | P1-10 decision |
|---|---|---|
| OS | `ubuntu-latest`, `macos-latest`, `windows-latest` | Required skeleton |
| Build type | `Debug`, `Release` | Start with `Debug`; add `Release` before release-candidate review |
| Library kind | Shared first, static later | Shared required; static TODO |
| Compiler | Default OS compiler first | GCC/Clang/MSVC expansion TODO |
| Entropy provider | Enabled by default | Required, but logs must be status-only |
| Thread safety | Enabled by default | Required for current global-state seam |
| Internal symbol hiding | Off for internal test-hook build; on for release-style no-test build | Add dedicated release-style lane before release-candidate review |

## Workflow skeleton

The repository includes `.github/workflows/ci.yml` as a starter workflow. It is intentionally conservative and should be treated as CI plumbing, not release approval.

## Release-candidate TODO

1. Add a second Linux compiler lane.
2. Add a Release build lane with `ANO_CRYPTO_BUILD_TESTS=OFF` and `ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON`.
3. Add static library smoke checks if static packaging becomes a supported deliverable.
4. Add artifact upload only after log redaction rules are reviewed.
5. Add SBOM/signing jobs when the release process is defined.
6. Add ABI baseline comparison after the public ABI is frozen.
