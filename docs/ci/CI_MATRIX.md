# AnoCrypto-C CI Matrix Skeleton

Status: P1-18 draft.

## Purpose

This matrix records the first CI coverage skeleton for build, test, public-surface checks, thread-safety policy checks, entropy failure-injection checks, install-package checks, customer PoC package checks, and evidence-draft generation. It should be reviewed and tightened before being used for release-candidate decisions.

## Required checks

| Check | Linux | macOS | Windows | Notes |
|---|---:|---:|---:|---|
| Configure C99 project | Required | Required | Required | `CMAKE_C_STANDARD=99`, extensions off. |
| Build shared library | Required | Required | Required | Candidate module boundary is shared-library first. |
| Strict warnings | Required | Required | Required | MSVC uses `/W4`; GCC/Clang use warning flags from CMake. |
| CTest unit/KAT/negative tests | Required | Required | Required | Output must remain status-only. |
| Build-tree install smoke | Required | Required | Required | Uses linked build target. |
| Build-tree core PoC example | Required | Required | Required | Uses public API and build-tree target. |
| Installed-package smoke | Required | Required | Required | Uses `find_package(AnoCryptoC CONFIG REQUIRED)`. |
| Customer PoC package smoke | Required | Optional | Optional | Stages package, configures packaged example, builds, and runs it. |
| Public surface policy check | Required | Required | Required | Checks naming, claims boundary, headers, package template presence, and Linux exported symbols when available. |
| Thread-safety policy check | Required | Required | Required | Checks support-matrix docs, lock branches, CMake option, concurrent-test guard, evidence hook, and PoC package note. |
| SHA-2 reserved scope policy check | Required | Required | Required | Checks SHA-384/SHA-512 reserved API docs, KAT plan, test, evidence hook, and PoC package note. |
| Random-service boundary policy check | Required | Required | Required | Checks reserved `ac_random.h`, fail-closed placeholders, DRBG/entropy boundary docs, evidence hook, and PoC package note. |
| Entropy/health policy check | Required | Required | Required | Checks test-hook defaults/guards, health skeleton/tests, fatal state wiring, PoC clean-build settings, and evidence records. |
| Entropy operational policy check | Required | Required | Required | Checks source classification, provider health ownership, operational probe, process-change guard, and public/PoC boundaries. |
| Dedicated entropy failure lane | Linux required | Not in default matrix | Not in default matrix | Enables internal hook only for labeled deterministic failure/partial-write tests; not a release binary lane. |
| Evidence draft target | Linux required | Optional | Optional | Includes SBOM/signing placeholder status, evidence manifest, random-boundary records, and entropy/health and entropy operational review records in P1-18. |
| Platform entropy provider branch | Linux provider | Apple provider | Windows provider | Provider tests must not print entropy bytes. |
| Thread-safety ON lane | Required | Required | Required | Uses internal locking where supported. |
| Thread-safety OFF lane | Required | Optional | Optional | Build/test only; not a concurrency-support lane. |

## Suggested matrix axes

| Axis | Values | P1-18 decision |
|---|---|---|
| OS | `ubuntu-latest`, `macos-latest`, `windows-latest` | Required skeleton |
| Build type | `Debug`, `Release` | Start with `Debug`; add `Release` before release-candidate review |
| Library kind | Shared first, static later | Shared required; static TODO |
| Compiler | Default OS compiler first | GCC/Clang/MSVC expansion TODO |
| Entropy provider | Enabled by default | Required, but logs must be status-only |
| Entropy test hook | `OFF` by default; `ON` only in dedicated Linux lane | Must not be enabled in ordinary, release-style, evidence, or PoC SDK builds |
| Thread safety | `ON` default, Linux `OFF` smoke | ON required; OFF confirms explicit caller-serialization configuration |
| Examples | `ANO_CRYPTO_BUILD_EXAMPLES=ON` | Required for PoC smoke |
| Internal symbol hiding | Off for internal test-hook build; on for release-style no-test build | Add dedicated release-style lane before release-candidate review |

## Workflow skeleton

The repository includes `.github/workflows/ci.yml` as a starter workflow. It is intentionally conservative and should be treated as CI plumbing, not release approval.

P1-18 workflow expectations:

- Run default build/test/smoke lanes with `ANO_CRYPTO_ENABLE_THREAD_SAFETY=ON` on Linux, macOS, and Windows.
- Run a Linux `ANO_CRYPTO_ENABLE_THREAD_SAFETY=OFF` lane to keep the non-concurrency-support build path explicit.
- Run `ano_crypto_thread_safety_policy_check` in all default lanes.
- Run `ano_crypto_sha2_scope_policy_check` in all default lanes.
- Run `ano_crypto_random_boundary_policy_check` in all default lanes.
- Run `ano_crypto_entropy_health_policy_check` in all default lanes.
- Run `ano_crypto_entropy_operational_policy_check` in all default lanes.
- Run one Linux-only `entropy-failure-injection` lane with OS entropy disabled and the internal deterministic hook enabled.
- On Windows shared-library test builds, use CMake test-only auto-export so internal-unit tests can link; clean Release/PoC builds keep tests off and do not enable auto-export.
- Generate `THREAD_SAFETY_CHECK.txt`, `SHA2_SCOPE_CHECK.txt`, `RANDOM_BOUNDARY_CHECK.txt`, `ENTROPY_HEALTH_CHECK.txt`, and `ENTROPY_OPERATIONAL_POLICY_CHECK.txt` in the Linux evidence draft.
- Keep customer PoC package smoke on Linux until macOS/Windows package path handling is reviewed.

## Release-candidate TODO

1. Add a second Linux compiler lane.
2. Add a Release build lane with `ANO_CRYPTO_BUILD_TESTS=OFF` and `ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON`.
3. Add customer PoC package smoke on macOS and Windows after path/executable lookup is reviewed.
4. Add Windows-specific concurrent CSP/HMAC stress coverage before widening Windows thread-safety wording.
5. Add static library smoke checks if static packaging becomes a supported deliverable.
6. Add artifact upload only after log redaction rules are reviewed.
7. Replace SBOM/signing placeholders with reviewed generator/signing jobs when the release process is defined.
8. Add ABI baseline comparison after the public ABI is frozen.
9. Add public random-service generation lanes only after P1-16 reserved-boundary criteria, P1-17 health-test constraints, and the P1-18 operational gate are reviewed.
10. Confirm the POSIX fork regression on macOS and deterministic process-marker behavior on Windows hosted runners.
11. Add platform-specific provider fault simulation only when it can remain outside ordinary Release/PoC binaries.

## P1-18 process-boundary lane expectations

- Linux normal lanes run the real OS-provider status path and the POSIX fork regression.
- macOS normal lanes run the Apple provider branch and process-marker tests supported by the runner.
- Windows normal lanes run the Windows provider branch and deterministic process-marker unit coverage.
- The dedicated failure-injection lane retains synthetic provider failures and also runs both entropy policy targets.
- Clean PoC/evidence builds must keep hooks off and internal symbol hiding on.
