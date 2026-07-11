# AnoCrypto-C Service Scope Matrix

Status: P1-23 engineering draft.

This matrix separates implemented core services, internal seams, and deferred product-layer functions. It is a planning and customer-PoC scoping artifact, not official validation evidence.

| Service area | Public surface | Implementation state | Current evidence / boundary |
|---|---|---|---|
| Module lifecycle and self-test | `ano_crypto_module_*`, `ano_crypto_selftest_*` | Implemented skeleton | State/negative tests and startup KAT flow; integrity strategy remains TODO. |
| Build/version identity | `ano_crypto_version_*`, `ano_crypto_build_*` | Implemented | ABI version 1 layout and 53-symbol baseline guarded. |
| SHA-256/384/512 | `ano_crypto_hash_*` | Implemented | Public FIPS vectors, streaming/negative tests, install and PoC smoke. |
| HMAC-SHA-256/384/512 | `ano_crypto_hmac_*` | Implemented | RFC 4231 vectors, streaming tests, and in-memory key-handle path. |
| HMAC_DRBG-SHA-256 | `ano_crypto_drbg_*` | Implemented deterministic caller-entropy core | KAT/boundary/negative tests; explicit-entropy contexts remain caller-managed across process changes. |
| CSP HMAC key lifecycle | `ano_crypto_key_*`, `ano_crypto_csp_*` | Implemented in-memory slice | Import/use/destroy, stale handle, exhaustion, concurrency; no persistence/export/wrapping. |
| Entropy adapter and OS provider | Internal only | Implemented seam | Status-only provider tests and dedicated failure injection; provider bytes never logged. |
| Public random service | `ano_crypto_random_*` | Implemented over platform entropy and HMAC_DRBG-SHA-256 | Lifecycle/failure/fork/stress tests; snapshot/clone/suspend automatic detection remains unsupported. |
| Block cipher, KDF expansion, public key | None | Deferred | Selection gates and planning only. |
| AnoSecureKit / AnoPKI workflows | Outside core | Separate product layers | Must not be mixed into the core module boundary. |

## Service entry rule

A service entering the implemented slice requires a documented parameter/lifecycle boundary, self-test or KAT rationale, negative tests, CSP mapping where secret state exists, install/PoC coverage when customer-facing, and evidence that does not retain secrets or generated cryptographic values.

## P1-22 SHA-512-family transition

P1-15 reserved the SHA-384/SHA-512 identifiers. P1-22 implements the existing identifiers together with startup/extended KATs, negative tests, CSP handle coverage, PoC/install smoke, policy checks, and evidence notes. The public export list remains unchanged. P1-23 adds OpenSSL 3.x test-only differential automation and portability checks while preserving the public export list. Actual hosted 32-bit, Windows, macOS, and big-endian artifact review remains TODO.
