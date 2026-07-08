# C99 Entropy Adapter Design Skeleton

Status: P1-14 draft.

## Purpose

The entropy adapter is a seam between platform entropy sources and DRBG core logic. It is not itself a DRBG and it does not replace the HMAC_DRBG-SHA-256 state machine.

P1-8 keeps the public DRBG API explicit-entropy based, while adding an internal helper that draws entropy and nonce bytes through the adapter before instantiating or reseeding HMAC_DRBG-SHA-256.

## Implemented through P1-8

- Internal entropy source descriptor in `src/internal/ac_entropy_internal.h`
- Callback-based test seam for deterministic unit tests
- Internal OS provider source kind using the platform provider when configured
- `ano_crypto_internal_platform_entropy_provider_name()` for evidence/status-only reporting
- `ano_crypto_internal_platform_entropy_is_available()` for test/evidence branching
- Output zeroization on adapter failure paths
- Internal HMAC_DRBG-SHA-256 instantiate/reseed helper driven by an entropy source
- Unit tests for callback success/failure, minimum request rejection, unsupported provider behavior, platform provider wiring, and DRBG helper integration

## Platform provider mapping

| Platform branch | Internal provider name | API boundary | Notes |
|---|---|---|---|
| Linux | `linux-getrandom` | `getrandom(..., flags=0)` | Used only when `ANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=ON` and CMake configures the Linux branch. |
| Windows | `windows-bcryptgenrandom` | `BCryptGenRandom(..., BCRYPT_USE_SYSTEM_PREFERRED_RNG)` | Build links `bcrypt` when this branch is configured. |
| Apple platforms | `apple-secrandomcopybytes` | `SecRandomCopyBytes(kSecRandomDefault, ...)` | Build links Security framework when this branch is configured. |
| Other/disabled | `unsupported` | none | Returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` and zeroizes caller output. |

## Explicitly not implemented yet

- Public random byte generation API
- Public automatic DRBG instantiation from OS entropy
- Entropy health tests
- Entropy-quality assessment beyond OS-provider boundary selection
- Fork/process-boundary reseed behavior
- Prediction-resistance requests
- Formal mapping to a validation package

## Internal API direction

```c
ano_crypto_internal_entropy_source_init_callback(...);
ano_crypto_internal_entropy_source_init_os(...);
ano_crypto_internal_entropy_source_get(...);
ano_crypto_internal_platform_entropy_get(...);
ano_crypto_internal_hmac_drbg_sha256_instantiate_from_entropy_source(...);
ano_crypto_internal_hmac_drbg_sha256_reseed_from_entropy_source(...);
```

The callback seam is for tests and adapter plumbing. It must not be treated as an operational entropy source. The OS provider seam is an internal platform boundary, not a public random-service contract.

## Failure and zeroization rules

- Adapter read failure zeroizes the requested output buffer.
- DRBG helper instantiation failure zeroizes local entropy/nonce buffers before returning.
- DRBG helper instantiation failure zeroizes caller-provided DRBG context storage after the context length is verified.
- Reseed helper failure zeroizes local entropy input before returning.
- Evidence logs may record provider name, availability, and status only.

## Sensitive data rules

- Do not log raw entropy, seed material, DRBG internal state, generated secret bytes, callback outputs, or test-source byte streams.
- On adapter failure, zeroize the output buffer before returning.
- Evidence logs may record provider name, availability, status codes, and counts only; do not record entropy bytes.

## TODO(P1/P2)

1. Add platform CI jobs for Linux, Windows, and Apple provider branches.
2. Define provider-specific failure mapping and retry policy in more detail.
3. Define health-test hooks and evidence requirements.
4. Decide how entropy adapter evidence is captured without storing raw entropy.
5. Connect adapter to a public random-service API only after API/ABI and validation-boundary review.
6. Decide whether DRBG instantiate-from-provider helper remains internal or becomes part of a public random-service implementation detail.
