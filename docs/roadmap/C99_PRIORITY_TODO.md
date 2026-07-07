# AnoCrypto-C Priority TODO

Status: P1-2 draft.

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
- engineering release evidence collection script

## Immediate P1 next items

1. Decide SHA-384/SHA-512 implementation timing.
2. Add HMAC_DRBG or Hash_DRBG design-to-code slice.
3. Add public DRBG KAT source manifest entries before implementation.
4. Add CSP/key handle lifecycle behavior beyond skeleton allocation rules.
5. Add install-prefix evidence capture to the evidence script.
6. Add source archive SHA-256 generation to the evidence script.
7. Add CI matrix and reproducible build notes.
8. Add API/ABI review notes for caller-provided context buffers.
9. Add thread-safety policy and tests.
10. Add side-channel review checklist for key-dependent code paths.

## P2 candidates

- Block cipher/mode scope selection based on customer and validation-target requirements.
- KDF/MAC expansion.
- Public-key primitive planning.
- AnoCrypto C++ Wrapper boundary draft.
- AnoSecureKit and AnoPKI integration boundary drafts.
