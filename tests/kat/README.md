# AnoCrypto-C KAT Tests

Status: P1-23 engineering draft.

Public-source coverage includes:

- SHA-256, SHA-384, and SHA-512 empty, `abc`, standard long-message, and million-`a` vectors.
- HMAC-SHA-256, HMAC-SHA-384, and HMAC-SHA-512 RFC 4231 cases 1, 2, 3, 4, 6, and 7.
- HMAC_DRBG-SHA-256 NIST CAVP no-prediction-resistance/reseed flow.
- HMAC key-handle computation for all three SHA-2 digest variants.
- Deterministic one-shot/streaming consistency and negative API boundaries.

These tests are implementation correctness checks, not official validation evidence. Public HMAC keys and DRBG KAT inputs are synthetic fixtures, not operational secrets. Test output must not print digest values, MAC values, key material, entropy, seed material, DRBG state, or generated random bytes.

Differential tests are separate from KAT provenance and live under `tests/differential/`. They compare values in memory and log only status/provider/count metadata.
