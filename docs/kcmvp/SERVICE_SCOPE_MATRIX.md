# Service Scope Matrix

Status: P1-29 engineering draft.

| Service | Public surface | Status | Key boundary |
|---|---|---|---|
| Module lifecycle/self-test | `ac_module.h`, `ac_selftest.h` | Implemented skeleton and startup KAT path | Fatal self-test enters sticky error state. |
| Build/version identity | `ano_crypto_version_*`, `ano_crypto_build_*` | Implemented | ABI version 1 layout; overall 69-symbol baseline. |
| SHA-2 | `ac_hash.h` | Implemented | SHA-256/384/512. |
| HMAC | `ac_hmac.h` | Implemented | Raw key and HMAC-purpose CSP handle. |
| HMAC_DRBG | `ac_drbg.h` | Implemented deterministic core | Explicit-entropy API remains caller-managed. |
| Public random | `ac_random.h` | Implemented on reviewed provider/process-marker branches | Source-backed HMAC_DRBG context. |
| AES-CBC | `ac_cipher.h` | Implemented without padding | Block-cipher-purpose CSP handle; caller IV. |
| AES-CMAC | `ac_cmac.h` | Implemented, full tag only | Dedicated CMAC-purpose CSP handle; CBC/CMAC cross-use rejected. |
| Key/CSP lifecycle | `ac_key.h`, `ac_csp.h` | In-memory handles implemented | Export, persistence, wrapping deferred. |
| CTR/GCM/GMAC/padding | None | Deferred | Requires separate policy/API/KAT gate. |
| PKI/SecureKit workflow | None | Outside core | Remains in AnoPKI/AnoSecureKit layers. |

P1-28 CMAC coverage includes startup KAT, all 12 public AES-CMAC examples,
negative lifecycle/authentication tests, install/PoC smoke, and OpenSSL 3.x
test-only differential comparison. Target-specific compiler/timing/cache review
and official validation scope remain TODO.

## P1-29 service-state note

AES-CMAC service availability is unchanged. P1-29 adds fail-closed context
validation, cleanup and aliasing rules, lifecycle/concurrency regression, and
manual compiler-review inputs. No service identifier or public operation was
added.
