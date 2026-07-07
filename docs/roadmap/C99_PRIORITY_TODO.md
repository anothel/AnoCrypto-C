# AnoCrypto-C Priority TODO

Status: P1-3 draft.

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
- engineering release evidence collection script

## Immediate P1 next items

1. Add entropy adapter design and test seam without logging raw entropy.
2. Add CSP/key handle lifecycle behavior beyond skeleton allocation rules.
3. Decide SHA-384/SHA-512 implementation timing.
4. Add source archive SHA-256 generation to the evidence script.
5. Add install-prefix smoke log capture to the evidence script.
6. Add CI matrix and reproducible build notes.
7. Add API/ABI review notes for caller-provided context buffers.
8. Add thread-safety policy and tests.
9. Add side-channel review checklist for key-dependent code paths.
10. Add additional HMAC_DRBG KAT paths: personalization string, no reseed, no additional input, and request-size boundaries.

## P2 candidates

- Block cipher/mode scope selection based on customer and validation-target requirements.
- KDF/MAC expansion.
- Public-key primitive planning.
- AnoCrypto C++ Wrapper boundary draft.
- AnoSecureKit and AnoPKI integration boundary drafts.
