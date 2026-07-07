# C99 Self-test Design

Status: P1-2 draft.

## Startup flow

`ano_crypto_module_init(NULL)` transitions the module to `SELF_TEST_RUNNING`, invokes `ano_crypto_selftest_run()`, and enters `OPERATIONAL` only if all startup checks pass.

Current startup checks:

1. Build identity sanity check.
2. KAT manifest structure check.
3. SHA-256 KATs.
4. HMAC-SHA-256 KATs.

## Failure behavior

A startup self-test failure records the failure status and transitions the module into `ERROR_STATE`. The current policy keeps `ERROR_STATE` sticky for the process.

## Service-gate rule

External cryptographic processing APIs require `OPERATIONAL`. Self-test uses internal helpers to avoid exposing cryptographic services before successful startup self-test completion.

## Current startup KATs

- SHA-256 empty message.
- SHA-256 `abc` message.
- SHA-256 standard multi-block message.
- HMAC-SHA-256 RFC 4231 test case 1.
- HMAC-SHA-256 RFC 4231 test case 2.
- HMAC-SHA-256 RFC 4231 test case 6 for long-key handling.

## Extended non-startup KAT/test coverage

The `ano_crypto_sha256_hmac_extended_kat_tests` executable adds:

- SHA-256 million-`a` vector.
- HMAC-SHA-256 RFC 4231 test cases 3, 4, and 7.
- Chunk-boundary consistency tests for one-shot versus streaming APIs.

## Evidence boundary

These KATs are implementation correctness checks only. They are not official validation evidence.

## TODO

- Add dedicated KAT manifest files with source URL/date/hash metadata.
- Add negative self-test injection cases beyond test hook forced failure.
- Add DRBG instantiate/generate/reseed KAT once DRBG implementation starts.
- Add conditional tests for future key generation and public-key algorithms.
- Add policy for startup self-test duration versus extended evidence-only KATs.
