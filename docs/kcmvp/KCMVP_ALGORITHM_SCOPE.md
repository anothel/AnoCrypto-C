# AnoCrypto-C Algorithm Scope

Status: P1-22 engineering draft.

## Implemented candidate slice

| Area | Algorithm/service | Current state | Engineering evidence |
|---|---|---|---|
| Hash | SHA-256 | Implemented | Startup and extended public-source KATs, negative and streaming tests. |
| Hash | SHA-384 | Implemented P1-22 | Startup/extended FIPS vectors, negative tests, install/PoC smoke. |
| Hash | SHA-512 | Implemented P1-22 | Startup/extended FIPS vectors, negative tests, install/PoC smoke. |
| MAC | HMAC-SHA-256 | Implemented | RFC 4231 coverage and CSP handle path. |
| MAC | HMAC-SHA-384 | Implemented P1-22 | RFC 4231 startup/extended coverage and CSP handle path. |
| MAC | HMAC-SHA-512 | Implemented P1-22 | RFC 4231 startup/extended coverage and CSP handle path. |
| DRBG | HMAC_DRBG-SHA-256 | Implemented engineering slice | Startup/public KAT, boundary, negative, reseed, and random-service integration tests. |
| CSP/key | In-memory HMAC key lifecycle | Implemented engineering slice | Import/use/destroy, stale-handle, exhaustion, and concurrency tests. |
| Entropy/random | Platform-provider-backed public random service | Implemented engineering slice | Lifecycle, failure injection, provider-disabled, fork/process, stress, and package smoke. |

## Reserved or deferred

| Area | Service | State |
|---|---|---|
| DRBG | Hash_DRBG-SHA-256 | Algorithm ID placeholder; not implemented. |
| Block cipher/modes | TBD | Deferred pending customer and validation-target review. |
| Additional KDF/MAC | TBD | Deferred. |
| Public-key primitives | TBD | Deferred to later planning. |

## Boundary

This file records implementation and evidence-preparation scope. It does not assert official validation status. Passing KATs and CTest are engineering correctness signals only.

P1-22 does not add a public function or change the stored 53-symbol ABI-0 export list. Independent differential-provider automation and actual hosted Windows/macOS review remain TODO.
