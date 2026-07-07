# AnoCrypto-C Algorithm Scope

Status: P1-2 draft.

## Implemented candidate slice

| Area | Algorithm | P1-2 status | Notes |
|---|---|---|---|
| Hash | SHA-256 | Implemented | Public-source KATs and extended boundary tests added. |
| MAC | HMAC-SHA-256 | Implemented | Public RFC KATs and long-key path coverage added. |

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

KAT and CTest pass logs are engineering correctness evidence only. They do not replace formal validation.

## P1 next candidates

1. Add source archive SHA-256 and install smoke log capture to evidence tooling.
2. Add public DRBG KAT manifest entries before DRBG implementation.
3. Implement HMAC_DRBG-SHA-256 state machine and KATs.
4. Add CSP/key handle lifecycle for HMAC keys.
5. Decide SHA-384/SHA-512 implementation timing.
