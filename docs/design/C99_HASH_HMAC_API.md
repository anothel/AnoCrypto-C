# C99 Hash and HMAC API Design

Status: P1-1 draft.

## Implemented P1-1 scope

- `ANO_CRYPTO_HASH_ALG_SHA256`
- `ano_crypto_hash_digest()` one-shot API
- `ano_crypto_hash_init/update/final()` streaming API
- `ano_crypto_hmac_compute()` one-shot API over SHA-256
- `ano_crypto_hmac_init/update/final()` streaming API over SHA-256

## ABI boundary

Public context layouts are not exposed. Callers obtain context sizes with:

- `ano_crypto_hash_context_size()`
- `ano_crypto_hmac_context_size()`

The current implementation uses caller-provided opaque buffers. The struct layout remains internal and may change before a public ABI freeze.

## State gate

All processing APIs call `ano_crypto_module_require_operational()` before performing cryptographic processing. Query APIs such as digest size and context size do not open cryptographic services and are allowed before initialization.

Startup self-test uses internal SHA-256/HMAC-SHA-256 helpers so that self-test can run while the module is in `SELF_TEST_RUNNING`, without allowing external cryptographic service calls before the module reaches `OPERATIONAL`.

## Implemented behavior

- SHA-256 supports zero-length input.
- SHA-256 supports streaming updates.
- HMAC-SHA-256 supports zero-length key and zero-length message inputs.
- HMAC temporary key blocks and inner digests are explicitly zeroized after use.
- Output buffer sizes are checked before writing results.

## Reserved but not implemented

- SHA-384
- SHA-512
- HMAC-SHA-384
- HMAC-SHA-512

These algorithm IDs remain ABI candidates but processing APIs return `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` for them in P1-1.

## TODO

- Add larger public KAT corpus and local vector manifest hashes.
- Add long-message tests and incremental chunk-boundary tests.
- Decide whether SHA-384/SHA-512 enter P1 or stay P2.
- Add differential validation strategy against an independently selected reference tool.
- Document input/output aliasing policy per API.
