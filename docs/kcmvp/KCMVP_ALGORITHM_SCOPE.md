# AnoCrypto-C Algorithm Scope

Status: P1-1 draft.

## Implemented candidate slice

| Area | Algorithm | P1-1 status | Notes |
|---|---|---|---|
| Hash | SHA-256 | Implemented | Public-source KATs added. |
| MAC | HMAC-SHA-256 | Implemented | Public RFC KATs added. |

## ABI-reserved / not implemented

| Area | Algorithm | Status | Notes |
|---|---|---|---|
| Hash | SHA-384 | Not implemented | Digest-size query only. |
| Hash | SHA-512 | Not implemented | Digest-size query only. |
| MAC | HMAC-SHA-384 | Not implemented | MAC-size query only. |
| MAC | HMAC-SHA-512 | Not implemented | MAC-size query only. |
| DRBG | HMAC_DRBG-SHA-256 | Skeleton only | No entropy, instantiate, generate, reseed implementation yet. |
| CSP/key | HMAC key import | Skeleton only | No operational key handle implementation yet. |

## Important boundary

This file describes an implementation and evidence-preparation scope. It does not assert official validation status.

## P1 next candidates

1. Expand SHA-256/HMAC-SHA-256 KAT coverage.
2. Add local public-vector manifest with source and SHA-256 checksums.
3. Implement HMAC_DRBG-SHA-256 state machine and KATs.
4. Add CSP/key handle lifecycle for HMAC keys.
