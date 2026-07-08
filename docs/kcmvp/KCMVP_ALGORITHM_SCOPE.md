# AnoCrypto-C Algorithm Scope

Status: P1-11 draft.

## Implemented candidate slice

| Area | Algorithm | Current status | Notes |
|---|---|---|---|
| Hash | SHA-256 | Implemented | Public-source KATs and extended boundary tests added. |
| MAC | HMAC-SHA-256 | Implemented | Public RFC KATs and long-key path coverage added. |
| DRBG | HMAC_DRBG-SHA-256 | Implemented P1-9 engineering slice | Explicit entropy-input public API, startup KAT, public vector test, boundary tests, negative tests, and internal entropy-source instantiate/reseed helper. |
| CSP/key | HMAC key import/use/destroy | Implemented P1-9 lifecycle slice | In-memory HMAC key handles only; no export, persistence, or keystore. |
| Entropy provider | Internal platform provider seam | Implemented P1-9 internal seam | Linux/Windows/Apple branches selected at build time; internal helper can seed/reseed HMAC_DRBG; no public random service API. |

## ABI-reserved / not implemented

| Area | Algorithm | Status | Notes |
|---|---|---|---|
| Hash | SHA-384 | Not implemented | Digest-size query only. |
| Hash | SHA-512 | Not implemented | Digest-size query only. |
| MAC | HMAC-SHA-384 | Not implemented | MAC-size query only. |
| MAC | HMAC-SHA-512 | Not implemented | MAC-size query only. |
| DRBG | Hash_DRBG-SHA-256 | Not implemented | Algorithm ID is reserved for API review. |
| Random service | `ano_crypto_random_*` | Not implemented | Design draft only; no public header or ABI yet. |

## Important boundary

This file describes an implementation and evidence-preparation scope. It does not assert official validation status.

KAT and CTest pass logs are engineering correctness evidence only. They do not replace formal validation.

## P1 next candidates

1. Add additional HMAC_DRBG KAT paths: personalization string, no reseed, no additional input, and request-size boundaries.
2. Add CI matrix for Linux/Windows/Apple entropy provider branches.
3. Decide SHA-384/SHA-512 implementation timing.
4. Add reproducible build notes.
5. Add public support matrix for thread-safety and platform entropy provider behavior.
6. Perform API/ABI review before adding public random-service APIs.

## P1-9 CSP/Entropy/DRBG Notes

- HMAC key handle lifecycle is implemented as an in-memory engineering slice only.
- Internal platform entropy provider seam exists for selected OS branches.
- P1-9 retains the internal DRBG helper that draws entropy/nonce through the adapter and zeroizes local seed material.
- Public random service and validation-boundary mapping remain TODO.
- These additions are not formal validation evidence.
- Release evidence tooling captures source archive SHA-256 and installed-package smoke logs as engineering evidence inputs.
