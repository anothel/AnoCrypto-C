# C99 Hash and HMAC API Design

Status: P1-9 draft.

## Implemented P1 scope

- `ANO_CRYPTO_HASH_ALG_SHA256`
- `ano_crypto_hash_digest()` one-shot API
- `ano_crypto_hash_init/update/final()` streaming API
- `ano_crypto_hmac_compute()` one-shot API over SHA-256
- `ano_crypto_hmac_init/update/final()` streaming API over SHA-256

## P1-2/P1-4 coverage additions

- Startup self-test now covers:
  - SHA-256 empty message
  - SHA-256 `abc`
  - SHA-256 standard multi-block message
  - HMAC-SHA-256 RFC 4231 test cases 1 and 2
  - HMAC-SHA-256 RFC 4231 test case 6 for the long-key path
- Extended KAT executable covers:
  - SHA-256 standard multi-block message
  - SHA-256 million-`a` vector
  - HMAC-SHA-256 RFC 4231 test cases 3, 4, 6, and 7
- Boundary consistency executable coverage is included through deterministic one-shot versus streaming comparisons.

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
- Finalized public contexts are zeroized and rejected on further use.

## Reserved but not implemented

- SHA-384
- SHA-512
- HMAC-SHA-384
- HMAC-SHA-512

These algorithm IDs remain ABI candidates but processing APIs return `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` for them in P1-5.

## TODO

- Decide whether SHA-384/SHA-512 enter P1 or stay P2.
- Add differential validation strategy against an independently selected reference tool.
- Document input/output aliasing policy per API.
- Add constant-time and side-channel review checklist for HMAC key handling.
- Add generated KAT evidence format if vectors move into separate manifest files.


## P1-4/P1-5 HMAC key handle path

P1-4 adds `ano_crypto_hmac_compute_with_key()` for HMAC-SHA-256 using an imported `ANO_CRYPTO_KEY_TYPE_HMAC` CSP handle. This path is intended to exercise key lifecycle and service-gate behavior without exposing raw key bytes after import.

Current limits:

- HMAC-SHA-256 only.
- In-memory HMAC key handles only.
- No key export, persistence, wrapping, or keystore integration.
- No thread-safety guarantee yet.
- Test keys are public synthetic KAT fixtures only.
