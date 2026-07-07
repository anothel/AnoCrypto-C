# AnoCrypto-C Algorithm Scope

Status: P1-3 draft.

## Implemented candidate slice

| Area | Algorithm | P1-3 status | Notes |
|---|---|---|---|
| Hash | SHA-256 | Implemented | Public-source KATs and extended boundary tests added. |
| MAC | HMAC-SHA-256 | Implemented | Public RFC KATs and long-key path coverage added. |
| DRBG | HMAC_DRBG-SHA-256 | Implemented P1-3 deterministic core | Explicit entropy-input API, startup KAT, public CAVP vector test, and negative tests added. |

## ABI-reserved / not implemented

| Area | Algorithm | Status | Notes |
|---|---|---|---|
| Hash | SHA-384 | Not implemented | Digest-size query only. |
| Hash | SHA-512 | Not implemented | Digest-size query only. |
| MAC | HMAC-SHA-384 | Not implemented | MAC-size query only. |
| MAC | HMAC-SHA-512 | Not implemented | MAC-size query only. |
| DRBG | Hash_DRBG-SHA-256 | Not implemented | Algorithm ID is reserved for API review. |
| CSP/key | HMAC key import | Skeleton only | No operational key handle implementation yet. |

## Important boundary

This file describes an implementation and evidence-preparation scope. It does not assert official validation status.

KAT and CTest pass logs are engineering correctness evidence only. They do not replace formal validation.

## P1 next candidates

1. Add entropy adapter design and tests without logging raw entropy.
2. Add CSP/key handle lifecycle behavior for HMAC keys.
3. Decide SHA-384/SHA-512 implementation timing.
4. Add source archive SHA-256 and install smoke log capture to evidence tooling.
5. Add CI matrix and reproducible build notes.
