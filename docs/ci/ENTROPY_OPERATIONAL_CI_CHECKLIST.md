# Entropy Operational Policy CI Checklist

Status: P1-19 engineering checklist.

## Required normal lanes

Linux, macOS, and Windows normal lanes should configure:

- `ANO_CRYPTO_BUILD_TESTS=ON`;
- `ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF`;
- `ANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=ON`;
- strict warnings enabled;
- the platform provider status test;
- `ano_crypto_entropy_operational_policy_tests`;
- `ano_crypto_process_guard_tests`;
- all existing KAT, negative, install-smoke, and policy targets.

## Required checks

1. OS source metadata reports `EXTERNAL_CONDITIONED_RBG` and external provider health ownership.
2. The operational probe accepts only the OS-provider class.
3. Callback and raw-noise engineering classes are rejected by the OS operational probe.
4. OS-provider sources reject attachment of the P1-17 duplicate-block health skeleton.
5. Unclassified callbacks reject module health-test attachment.
6. Explicit internal raw-noise engineering callbacks may attach the skeleton for test-only state-transition coverage.
7. A deterministic process-marker mismatch returns `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED`.
8. Generate output is cleared on a process-marker mismatch.
9. On POSIX lanes, a child process cannot generate from inherited source-backed DRBG state until reseed succeeds.
10. On POSIX lanes, explicit-entropy DRBG state remains caller-managed across process inheritance; this boundary test is not a fork-safety claim.
11. The dedicated failure-injection lane exercises operational-probe success, forced failure, and partial-write failure status paths.
12. Logs contain test names and status only.

## Dedicated entropy failure lane

Retain the P1-17 dedicated failure-injection lane. It must not be used to create Release or PoC binaries.

The lane should continue to verify:

- deterministic success;
- forced source failure;
- partial-write failure;
- operational-probe success/failure/partial-failure status mapping;
- output clearing;
- test hook disabled by default and unavailable in clean builds.

## Release and PoC boundary

Clean Release/PoC configuration must use:

- tests off;
- entropy test hooks off;
- internal symbol hiding on where supported;
- no installed internal entropy or process-guard header;
- no exported `ano_crypto_internal_*` symbol;
- no internal source, hook, health, or process-guard test file in the package.

## Evidence outputs

Review:

- `ENTROPY_OPERATIONAL_POLICY_CHECK.txt`;
- `ENTROPY_OPERATIONAL_REVIEW_NOTES.txt`;
- `ENTROPY_HEALTH_CHECK.txt`;
- `ENTROPY_HEALTH_REVIEW_NOTES.txt`;
- `ctest.log`;
- install smoke logs;
- PoC package smoke logs and policy notes.

Evidence must remain status-only. Do not retain provider output, seed material, nonce material, DRBG state, generated random output, private keys, tokens, or customer data.

## Platform expectations

- Linux: run the real `getrandom()` provider path and POSIX fork regression.
- macOS: run the `SecRandomCopyBytes()` provider path and process-marker regression where supported by the runner.
- Windows: run the `BCryptGenRandom()` provider path and deterministic process-marker unit branch; no POSIX fork test is expected.
- Unsupported targets: provider test may report unavailable/`ANO_CRYPTO_ERR_NOT_IMPLEMENTED`, but the result must be explicit.

## Failure handling

A CI failure blocks the engineering package when it indicates:

- internal policy metadata leaked into public headers;
- a process guard was bypassed for source-backed state;
- failed generation left caller output nonzero;
- an OS-provider source accepted the raw-noise health skeleton;
- test-hook code entered a clean Release/PoC build;
- evidence or package logs contain sensitive material.
