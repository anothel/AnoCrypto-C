# C99 Hash and HMAC API Design

Status: P1-23 engineering draft.

## Implemented services

The existing public API supports SHA-256, SHA-384, and SHA-512 one-shot and
streaming digests; HMAC over those variants in one-shot and streaming form; and
HMAC computation through an in-memory CSP HMAC-key handle.

## ABI boundary

Public context layouts are not exposed. Callers obtain storage requirements
with `ano_crypto_hash_context_size()` and `ano_crypto_hmac_context_size()`.
P1-22 implemented the previously reserved algorithm IDs without adding public
symbols. P1-23 added only test/evidence tooling and left the then-current 53-symbol
ABI-0 export list unchanged. Cross-service context use is rejected.

## State and lifecycle rules

- Query APIs are available before module initialization.
- Processing APIs require the module to be operational.
- Zero-length hash input and zero-length HMAC key/input are accepted.
- Output capacity is validated before output is written.
- A finalized context is cleared and cannot be reused.
- A final call that fails only for short output may be retried as covered by
  regression tests.
- HMAC temporary key blocks, inner digests, and handle snapshots are zeroized.
- Input is treated as bytes and must work at intentionally unaligned addresses.

## Correctness and portability coverage

Startup self-tests and extended public KATs cover all implemented variants.
P1-23 adds a deterministic OpenSSL 3.x test-only differential lane and an
independent portability review for endian metadata, variable chunking, and
unaligned input. The product library does not link to OpenSSL because of these
tests.

## Remaining TODO

- Record input/output aliasing rules per public API.
- Add a controlled internal length-counter overflow seam only after a separate
  release/test-hook boundary review.
- Review compiler output and performance on the final supported toolchain
  matrix.
- Obtain actual hosted 32-bit and big-endian evidence before support wording is
  widened.
- Revisit opaque caller-owned context alignment before public ABI freeze.
