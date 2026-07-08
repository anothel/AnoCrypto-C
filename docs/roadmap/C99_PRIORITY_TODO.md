# AnoCrypto-C Priority TODO

Status: P1-9 draft.

## Completed P0/P1 foundation items

- C99 repository skeleton
- clean-room source manifest and exclusion log
- public C ABI header skeletons
- module lifecycle/state-machine skeleton
- startup self-test framework skeleton
- build/version identity APIs
- CMake build/install/export skeleton
- unit, negative, KAT, and install smoke test skeletons
- SHA-256 implementation
- HMAC-SHA-256 implementation
- public-source SHA-256/HMAC-SHA-256 KAT coverage
- extended chunk-boundary consistency tests
- HMAC_DRBG-SHA-256 deterministic core implementation
- public-source HMAC_DRBG-SHA-256 KAT coverage
- internal entropy adapter seam and unit tests
- in-memory HMAC CSP/key handle lifecycle slice
- engineering release evidence collection script
- install-prefix smoke log capture in the evidence script
- source archive SHA-256 generation in the evidence script
- CSP handle table exhaustion and stale-handle reuse tests
- limited module/self-test/CSP global-state lock seam and thread-safety policy draft
- POSIX concurrent CSP/HMAC smoke test
- internal platform entropy provider seam for Linux, Windows, and Apple provider branches
- platform entropy provider smoke/status tests
- internal HMAC_DRBG-SHA-256 instantiate/reseed helper driven by entropy source
- random-service public API design draft without exposing public ABI
- HMAC_DRBG-SHA-256 boundary tests for personalization, additional input, reseed-after-generate, max request, and oversized request rejection
- CMake package naming normalized to `AnoCryptoC` / `AnoCryptoC::AnoCryptoC`

## Immediate P1 next items

1. Add CI matrix coverage for Linux, Windows, and Apple entropy provider branches.
2. Add reproducible build notes.
3. Add API/ABI review notes for caller-provided context buffers.
4. Expand thread-safety policy into reviewed public support matrix and CI coverage.
5. Add side-channel review checklist for key-dependent code paths.
6. Add SBOM/signing placeholders to the evidence workflow.
7. Decide SHA-384/SHA-512 implementation timing.
8. Decide whether and when to expose public `ano_crypto_random_*` APIs.
9. Add reproducible source archive rules if archive byte-for-byte reproducibility becomes a release requirement.

## P2 candidates

- Block cipher/mode scope selection based on customer and validation-target requirements.
- KDF/MAC expansion.
- Public-key primitive planning.
- AnoCrypto C++ Wrapper boundary draft.
- AnoSecureKit and AnoPKI integration boundary drafts.
