# C99 Entropy Adapter Design Skeleton

Status: P1-6 draft.

## Purpose

The entropy adapter is a seam between platform entropy sources and DRBG core logic. It is not itself a DRBG and it does not replace the HMAC_DRBG-SHA-256 state machine.

P1-4 added only an internal adapter interface so future OS/platform-specific entropy collection can be implemented and reviewed separately from DRBG logic.

## Implemented P1-5 slice

- Internal entropy source descriptor in `src/internal/ac_entropy_internal.h`
- Callback-based test seam for deterministic unit tests
- Reserved OS entropy source kind that returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`
- Output zeroization on adapter failure paths
- Unit tests for callback success, callback failure, minimum request rejection, and OS-reserved behavior

## Explicitly not implemented yet

- OS entropy collection
- Health tests
- Entropy-quality assessment
- Fork/process-boundary reseed behavior
- Integration with a public random-service API
- Prediction-resistance requests

## Internal API direction

```c
ano_crypto_internal_entropy_source_init_callback(...);
ano_crypto_internal_entropy_source_init_os_reserved(...);
ano_crypto_internal_entropy_source_get(...);
```

The callback seam is for tests and future adapter plumbing. It must not be treated as an operational entropy source without a reviewed provider.

## Sensitive data rules

- Do not log raw entropy, seed material, DRBG internal state, generated secret bytes, or callback outputs.
- On adapter failure, zeroize the output buffer before returning.
- Evidence logs may record status codes and counts only; do not record entropy bytes.

## TODO(P1/P2)

1. Select platform entropy sources per OS/toolchain.
2. Define source failure mapping to `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED`.
3. Define health-test hooks and evidence requirements.
4. Decide how entropy adapter evidence is captured without storing raw entropy.
5. Connect adapter to an approved-candidate random-service API only after review.
