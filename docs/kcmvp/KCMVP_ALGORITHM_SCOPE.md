# KCMVP Candidate Algorithm Scope

## Status

Draft. This document is for readiness planning only and does not state completion of any official validation.

## P1 Candidate Scope

| Area | Candidate | Implementation Status | Evidence Status |
|---|---|---:|---:|
| Hash | SHA-256 / SHA-384 / SHA-512 | TODO | TODO |
| MAC | HMAC-SHA-256 / HMAC-SHA-384 / HMAC-SHA-512 | TODO | TODO |
| DRBG | HMAC_DRBG-SHA-256 or Hash_DRBG-SHA-256 | TODO | TODO |
| CSP | key handle / zeroize lifecycle | skeleton | TODO |

## Out of Scope for P1

- Full PKI workflows
- Certificate chain validation
- Application envelope encryption policy
- Platform keystore adapters

## TODO

- Choose the first DRBG candidate.
- Record public KAT vector provenance.
- Define algorithm implementation strategy.
- Add negative test and evidence collection plan.
