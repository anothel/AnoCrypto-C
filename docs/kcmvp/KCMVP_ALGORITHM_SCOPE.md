# AnoCrypto-C Algorithm Scope

Status: P1-7 draft.

## Implemented candidate slice

| Area | Algorithm | P1-5 status | Notes |
|---|---|---|---|
| Hash | SHA-256 | Implemented | Public-source KATs and extended boundary tests added. |
| MAC | HMAC-SHA-256 | Implemented | Public RFC KATs and long-key path coverage added. |
| DRBG | HMAC_DRBG-SHA-256 | Implemented P1-7 deterministic core | Explicit entropy-input API, startup KAT, public CAVP vector test, and negative tests added. |

## ABI-reserved / not implemented

| Area | Algorithm | Status | Notes |
|---|---|---|---|
| Hash | SHA-384 | Not implemented | Digest-size query only. |
| Hash | SHA-512 | Not implemented | Digest-size query only. |
| MAC | HMAC-SHA-384 | Not implemented | MAC-size query only. |
| MAC | HMAC-SHA-512 | Not implemented | MAC-size query only. |
| DRBG | Hash_DRBG-SHA-256 | Not implemented | Algorithm ID is reserved for API review. |
| CSP/key | HMAC key import/use/destroy | Implemented P1-7 lifecycle slice | In-memory HMAC key handles only; no export, persistence, or keystore. |
| Entropy provider | Internal platform provider seam | Implemented P1-7 internal seam | Linux/Windows/Apple branches selected at build time; no public random service API. |

## Important boundary

This file describes an implementation and evidence-preparation scope. It does not assert official validation status.

KAT and CTest pass logs are engineering correctness evidence only. They do not replace formal validation.

## P1 next candidates

1. Add CI matrix for Linux/Windows/Apple entropy provider branches.
2. Decide SHA-384/SHA-512 implementation timing.
3. Add reproducible build notes.
4. Add public support matrix for thread-safety and platform entropy provider behavior.
5. Add additional HMAC_DRBG KAT paths and request-size boundary tests.
6. Design public random-service API only after validation-boundary review.

## P1-7 CSP/Entropy Notes

- HMAC key handle lifecycle is implemented as an in-memory engineering slice only.
- Internal platform entropy provider seam exists for selected OS branches, but public random service and validation-boundary mapping remain TODO.
- These additions are not formal validation evidence.
- CSP/key negative tests cover fixed-table exhaustion and stale-handle rejection after slot reuse.
- Release evidence tooling captures source archive SHA-256 and installed-package smoke logs as engineering evidence inputs.
