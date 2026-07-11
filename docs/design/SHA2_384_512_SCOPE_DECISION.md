# SHA-384 / SHA-512 Implementation Decision

Status: P1-23 engineering decision record.

## Decision

P1-22 moves SHA-384, SHA-512, HMAC-SHA-384, and HMAC-SHA-512 from the P1-15 ABI-reserved state into the implemented candidate slice.

The change reuses the existing public algorithm IDs and function symbols. It does not add a public function or expose an internal context layout.

Implemented paths:

- SHA-384 and SHA-512 one-shot digest;
- SHA-384 and SHA-512 streaming init/update/final;
- HMAC-SHA-384 and HMAC-SHA-512 one-shot and streaming processing;
- HMAC key-handle computation for SHA-256, SHA-384, and SHA-512;
- startup self-tests and extended CTest KATs;
- negative tests for size, state, retry, unsupported algorithm, and cross-service context misuse.

## Internal boundary

- The SHA-512-family compression state remains under `src/internal/` and `src/hash/`.
- Public callers continue to request an opaque context size.
- The SHA-384 construction uses the SHA-384 initial state and 48-byte output over the shared SHA-512-family core.
- Internal schedules, HMAC key blocks, inner digests, and finalized contexts are explicitly zeroized.
- Hash digests and MAC values are not written to PoC or evidence logs.

## Evidence boundary

P1-22 engineering evidence includes:

- public-source startup KATs for SHA-384/SHA-512 empty and `abc` inputs;
- public RFC 4231 startup KATs for HMAC-SHA-384/SHA-512 cases 1 and 2;
- extended long-message, million-`a`, and RFC 4231 cases 3, 4, 6, and 7;
- streaming consistency and negative API coverage;
- installed-package and PoC smoke across all three SHA-2 variants;
- a clean Release exported-symbol comparison against the existing 53-symbol ABI-0 list.

This is implementation correctness and engineering readiness evidence. It is not official validation evidence and does not assert KCMVP completion.

## P1-23 hardening

P1-23 adds deterministic OpenSSL 3.x test-only comparisons for SHA-256/384/512 and HMAC variants, intentionally unaligned inputs, a C99 representation compile guard, runtime/CMake endian review, and hosted/32-bit workflow wiring. OpenSSL remains outside the product and installed SDK dependency boundary.

## Remaining TODO

- Review actual hosted Windows, Apple, Linux 32-bit, and any intended big-endian artifacts.
- Review 64-bit performance and compiler output on the selected release toolchains.
- Add a controlled length-counter boundary test seam if required; do not introduce a release test hook without a separate boundary review.
- Extend side-channel and compiler-code-generation review for HMAC key-dependent paths.

## Historical note

P1-15 intentionally kept these services reserved until implementation, KAT, self-test, package, and evidence wiring could be changed together. P1-22 satisfies that entry gate and removes the obsolete reserved-processing regression.
