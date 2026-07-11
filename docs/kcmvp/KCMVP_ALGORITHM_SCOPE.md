# AnoCrypto-C Algorithm Scope

Status: P1-27 engineering draft.

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
| CSP/key | In-memory HMAC and AES key lifecycle | Implemented engineering slice | Import/use/destroy, exact AES key lengths, stale-handle, exhaustion, and concurrency tests. |
| Entropy/random | Platform-provider-backed public random service | Implemented engineering slice | Lifecycle, failure injection, provider-disabled, fork/process, stress, and package smoke. |

## Reserved or deferred

| Area | Service | State |
|---|---|---|
| DRBG | Hash_DRBG-SHA-256 | Algorithm ID placeholder; not implemented. |
| Block cipher/modes | AES-128/192/256 + CBC no-padding | Implemented in P1-25 and hardened in P1-26 with startup/extended KAT, negative, expanded differential, lifecycle/portability/concurrency, install/PoC, and evidence checks. |
| Additional MAC | AES-CMAC | Selected by P1-27 entry gate; not implemented. Dedicated key-purpose, full-tag-only API, KAT/evidence, and symbol-delta review required before P1-28. |
| Additional KDF/MAC | Other services | Deferred. |
| Public-key primitives | TBD | Deferred to later planning. |

## Boundary

This file records implementation and evidence-preparation scope. It does not assert official validation status. Passing KATs and CTest are engineering correctness signals only.

P1-22 did not add a public function and P1-23 added only test/evidence tooling, preserving the then-current 53-symbol ABI-0 list. P1-25 added eight reviewed AES/CBC public symbols and updated the stored ABI-0 list to 61 names. P1-26 preserves those functions and that baseline while hardening internal lifecycle, portability, concurrency, differential, and compiler-review workflows. P1-27 selects AES-CMAC as the next candidate but adds no implementation, public API, enum value, or symbol, so the 61-name baseline remains unchanged.

AES-CMAC implementation is gated for P1-28. AEAD, CTR, padding helpers, GMAC, hardware dispatch, and domestic algorithms remain deferred. Actual hosted 32-bit, Windows, macOS, big-endian, manual compiler-output, and target-specific side-channel review remains TODO.
