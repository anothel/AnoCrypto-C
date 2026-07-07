# C99 Hash/HMAC API Skeleton

## Status

P1 foundation only. SHA-2 and HMAC APIs are declared and state-gated, but algorithm implementations are TODO.

## Goals

- Keep the public ABI C99-only.
- Return `ano_crypto_status_t` from all public functions.
- Keep context layout private by using caller-provided opaque buffers and `*_context_size()` queries.
- Permit size/name query APIs before module initialization.
- Reject cryptographic processing before module initialization and startup self-test success.

## Candidate Algorithms

- SHA-256
- SHA-384
- SHA-512
- HMAC-SHA-256
- HMAC-SHA-384
- HMAC-SHA-512

## TODO(P1)

- Choose implementation strategy.
- Add public-source KAT vector manifest entries.
- Add streaming and one-shot tests.
- Add negative tests for buffer length, NULL pointer, and unsupported algorithms.
- Document input/output aliasing policy.
