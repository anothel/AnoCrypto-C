# AnoCrypto-C Algorithm Scope

Status: P1-6 draft.

## Implemented candidate slice

| Area | Algorithm | P1-5 status | Notes |
|---|---|---|---|
| Hash | SHA-256 | Implemented | Public-source KATs and extended boundary tests added. |
| MAC | HMAC-SHA-256 | Implemented | Public RFC KATs and long-key path coverage added. |
| DRBG | HMAC_DRBG-SHA-256 | Implemented P1-5 deterministic core | Explicit entropy-input API, startup KAT, public CAVP vector test, and negative tests added. |

## ABI-reserved / not implemented

| Area | Algorithm | Status | Notes |
|---|---|---|---|
| Hash | SHA-384 | Not implemented | Digest-size query only. |
| Hash | SHA-512 | Not implemented | Digest-size query only. |
| MAC | HMAC-SHA-384 | Not implemented | MAC-size query only. |
| MAC | HMAC-SHA-512 | Not implemented | MAC-size query only. |
| DRBG | Hash_DRBG-SHA-256 | Not implemented | Algorithm ID is reserved for API review. |
| CSP/key | HMAC key import/use/destroy | Implemented P1-5 lifecycle slice | In-memory HMAC key handles only; no export, persistence, or keystore. |

## Important boundary

This file describes an implementation and evidence-preparation scope. It does not assert official validation status.

KAT and CTest pass logs are engineering correctness evidence only. They do not replace formal validation.

## P1 next candidates

1. Connect reviewed platform entropy sources to the adapter seam.
2. Decide SHA-384/SHA-512 implementation timing.
3. Add CI matrix and reproducible build notes.
4. Add thread-safety policy and tests for CSP table and module global state.
5. Add additional HMAC_DRBG KAT paths and request-size boundary tests.

## P1-5 CSP/Entropy Notes

- HMAC key handle lifecycle is implemented as an in-memory engineering slice only.
- Internal entropy adapter seam exists, but OS entropy collection remains TODO.
- These additions are not formal validation evidence.
- CSP/key negative tests cover fixed-table exhaustion and stale-handle rejection after slot reuse.
- Release evidence tooling captures source archive SHA-256 and installed-package smoke logs as engineering evidence inputs.
