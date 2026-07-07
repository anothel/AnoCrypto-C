# C99 Platform Entropy Provider Policy Skeleton

Status: P1-7 draft.

## Goal

P1-7 adds a small internal platform entropy provider seam so that future DRBG and random-service work can be reviewed independently from algorithm code.

This document is a design/policy draft. It does not assert formal validation status, external review completion, or suitability for every customer environment.

## Provider selection

The build option `ANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER` defaults to `ON`. When enabled, CMake selects a provider branch by target platform:

- Linux: `getrandom()` with flags set to zero.
- Windows: `BCryptGenRandom()` with the system-preferred RNG flag.
- Apple platforms: `SecRandomCopyBytes()` using the default random source.
- Unsupported platforms: provider name is `unsupported`; requests fail with `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`.

## Failure behavior

- Invalid arguments return `ANO_CRYPTO_ERR_INVALID_ARGUMENT`.
- Unsupported platform/provider configuration returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`.
- Runtime provider failures return `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED`.
- Output buffers are zeroized on failure before returning.
- Entropy bytes are never printed, persisted, or written to evidence logs.

## Current boundary

The provider is internal only:

- No public `ano_crypto_random_*` API is exposed in P1-7.
- Public HMAC_DRBG APIs still accept explicit caller-provided entropy input.
- Prediction-resistance requests still return `ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER`.
- Provider status tests are engineering smoke tests only.

## Evidence expectations

Allowed in evidence logs:

- provider name
- provider availability flag
- status code names
- CTest pass/fail result
- install smoke pass/fail result

Not allowed in evidence logs:

- entropy bytes
- DRBG seed material
- generated random bytes
- key material
- private keys
- tokens
- internal deployment secrets

## Review TODO

1. Add platform CI coverage for all configured provider branches.
2. Decide whether `/dev/urandom` fallback is ever acceptable for specific customer/toolchain targets.
3. Define fork/process-boundary reseed policy.
4. Define health-test requirements and where those results are recorded.
5. Decide whether provider availability should be exposed through a public diagnostic API.
6. Add customer/environment-specific provider review checklist before PoC delivery.
