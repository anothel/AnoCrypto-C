# C99 Hash and HMAC API Design

Status: P1-22 engineering draft.

## Implemented services

The existing public API supports:

- SHA-256, SHA-384, and SHA-512 one-shot digests;
- SHA-256, SHA-384, and SHA-512 streaming contexts;
- HMAC over SHA-256, SHA-384, and SHA-512 in one-shot and streaming form;
- HMAC computation through an in-memory CSP HMAC-key handle.

## ABI boundary

Public context layouts are not exposed. Callers obtain storage requirements with:

- `ano_crypto_hash_context_size()`;
- `ano_crypto_hmac_context_size()`.

The existing public function and algorithm identifiers are retained. P1-22 adds implementation behind reserved identifiers without adding a public symbol. Cross-service context use is rejected.

## State and lifecycle rules

- Query APIs are available before module initialization.
- Processing APIs require the module to be operational.
- Zero-length hash input is accepted.
- Zero-length HMAC key and input are accepted.
- Output capacity is validated before output is written.
- A finalized context is cleared and cannot be reused.
- Failed finalization due only to a short output buffer may be retried as documented by the tests.
- HMAC temporary key blocks, inner digests, and handle snapshots are zeroized.

## Self-test and correctness coverage

Startup self-test covers SHA-2 and HMAC public vectors for all implemented variants. Extended CTest coverage includes public long-message and million-`a` digests, RFC 4231 long-key/long-data cases, streaming paths, CSP handle paths, and negative lifecycle checks.

## Remaining TODO

- Record input/output aliasing rules per API.
- Add independent external-provider differential automation.
- Review compiler output and performance on the final supported platform/toolchain matrix.
- Revisit opaque caller-owned context alignment before public ABI freeze.
