# C99 Entropy Failure Injection and Health-Test Skeleton

Status: P1-19 retained engineering design.

## Scope

This document defines an internal testability boundary for the AnoCrypto-C entropy adapter. It does not add a public random-byte service, expose an entropy provider through the installed C ABI, or select a final noise-source health-test policy.

The P1-17 goals are:

- deterministically exercise provider success, provider failure, and partial-write failure paths;
- reject zero-length and oversized requests before provider use;
- zeroize caller output after any provider or health-test failure that may have written bytes;
- keep test-hook code out of ordinary release and customer PoC builds;
- provide startup and continuous duplicate-block candidate wiring for later review;
- define fail-closed DRBG and module-state behavior for a health-test failure;
- keep entropy material, seed material, and DRBG state out of logs and evidence.

## Internal Failure-Injection Boundary

`ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS` is an internal CMake option. Its default is `OFF`. Configuration fails when the option is enabled while `ANO_CRYPTO_BUILD_TESTS=OFF`.

When enabled for a dedicated test build, the internal platform provider can be set to one of these deterministic modes:

- deterministic sequence;
- deterministic repeated request output;
- failure before output;
- partial output followed by failure.

The declarations exist only in `src/internal/ac_entropy_internal.h` and are guarded by the test-build macro. They are not installed and are not part of `AnoCryptoC::AnoCryptoC` public ABI commitments.

The ordinary test matrix keeps this option off. A separate CI lane enables it only long enough to run the entropy failure-injection tests. Customer PoC staging performs a separate clean build with tests off, entropy hooks off, and internal symbol hiding on.

## Request Contract

Internal entropy requests follow these rules:

| Case | Result | Output rule |
|---|---|---|
| null output or zero length | `ANO_CRYPTO_ERR_INVALID_ARGUMENT` | no provider call |
| request above `ANO_CRYPTO_INTERNAL_ENTROPY_MAX_REQUEST_LEN` | `ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER` | reject before touching output |
| request below source minimum | `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED` | requested output range is zeroized |
| provider failure before output | provider failure status | requested output range is zeroized |
| provider partial write then failure | failure status | full requested output range is zeroized |
| health-test candidate failure | `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED` | full requested output range is zeroized |

The current maximum is an internal engineering ceiling, not a public ABI constant.

## Health-Test Skeleton

P1-17 adds two internal duplicate-block candidates:

1. **Startup duplicate-block candidate**: compares two equal halves of one bounded sample and fails when they are identical.
2. **Continuous duplicate-block candidate**: retains one bounded prior sample and fails when the next sample is exactly identical.

Both candidates are disabled unless an internal entropy source explicitly attaches a health state. The retained prior sample is zeroized when the health state is reset.
The health state is caller-owned internal storage; the internal owner must call `ano_crypto_internal_entropy_health_reset()` during source teardown and error cleanup so retained sample bytes do not outlive the source lifecycle.

These candidates are wiring and state-transition skeletons only. They are not a reviewed repetition-count test, adaptive-proportion test, or noise-source assessment. Applying raw-noise health-test terminology to a conditioned operating-system random API may be inappropriate. Final startup/continuous tests, sample location, cutoffs, restart handling, and conditioning boundary remain **TODO / confirmation required** after the target entropy source and operating environment are selected.

## Failure-State Policy

| Failure | Current P1-17 behavior |
|---|---|
| ordinary provider acquisition failure | current operation fails; output and temporary seed material are zeroized; module-fatal policy remains source-role dependent |
| startup health-test failure with fatal policy | module enters sticky `ERROR_STATE`; initialization must not reach `OPERATIONAL` when this path is wired into startup |
| continuous health-test failure with fatal policy | module enters sticky `ERROR_STATE`; current entropy output is zeroized |
| DRBG instantiate helper health failure | destination DRBG context is zeroized and the call fails |
| DRBG reseed helper fatal health failure | DRBG context is invalidated by zeroization; subsequent cryptographic services are blocked by module `ERROR_STATE` |

Until a dedicated public health-test status is reviewed, health-test failure maps to `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED`. This avoids changing the public status enum solely for an internal candidate skeleton.

Recovery from sticky `ERROR_STATE` remains process restart or a future reviewed recovery design. P1-17 does not add a public reset API.

## Logging and Evidence

Allowed records:

- provider branch or internal test-lane status;
- status code names;
- test case names and pass/fail result;
- request class such as zero-length, partial-failure, or oversized;
- module state name after a synthetic failure.

Prohibited records:

- raw entropy bytes or captured operating-system output;
- deterministic test-hook byte sequences in logs;
- DRBG seed material, nonce material, personalization secrets, or internal DRBG state;
- generated random bytes;
- private keys, tokens, customer data, or internal deployment details.

Tests may compare synthetic buffers in memory. They must clear temporary buffers before exit and must not serialize them into evidence.

## P1-17 Tests

- `tests/negative/entropy_source_negative_tests.c`
- `tests/negative/platform_entropy_failure_injection_tests.c`
- `tests/unit/entropy_health_tests.c`
- `tests/negative/entropy_health_error_state_tests.c`

## TODO / Confirmation Required

- Select the actual entropy/noise source boundary for each supported platform.
- Decide whether operating-system conditioned output needs module-local health tests or only provider-failure handling.
- Define reviewed startup and continuous health tests, sample sizes, and thresholds for the selected source.
- Review fork, VM snapshot/restore, suspend/resume, and provider restart behavior.
- Decide whether an entropy acquisition failure outside startup is operation-local or module-fatal for each future public random service.
- Add platform-specific provider fault simulation where the operating system test environment supports it without production hooks.

## P1-19 placement restriction

P1-19 narrows where this skeleton may be used:

- OS-provider output is classified as external conditioned provider output and must not be passed through this raw-noise candidate skeleton.
- Ordinary/unclassified callbacks are caller-owned and cannot attach the module health state.
- Only `ano_crypto_internal_entropy_source_init_callback_noise_candidate()` creates the explicit internal raw-noise engineering class that may attach the skeleton.
- The duplicate-block logic remains disabled by default and does not define final RCT/APT thresholds, sample representation, or recovery policy.
- `docs/design/C99_ENTROPY_SOURCE_OPERATIONAL_POLICY.md` is the controlling P1-19 source-placement and process-boundary decision.
