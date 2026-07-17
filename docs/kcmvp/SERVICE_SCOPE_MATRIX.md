# Service Scope Matrix

Status: P2-07 current engineering service inventory bound to OE-01 and internal self-test/state, entropy/DRBG, CSP/zeroization, and secure-coding/side-channel policies; module-byte integrity, exact OE-01 review, and formal classification remain pending.

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


## P2-02 boundary note

The service inventory remains 14 public headers and 69 exports on `ANOCRYPTO_C_0`. P2-02 binds the unchanged services to `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` without adding runtime services or public API. Module integrity, final conditional-test classification, entropy-provider responsibility, formal CSP inventory, and target-specific side-channel review remain open P2 items.

## P2-03 self-test and state note

No cryptographic service or public symbol is added. Startup KATs and the sticky service gate remain current behavior. `INT-HMAC-SHA256-SINGLE-SO-R1` is a selected internal design only; no shared-object verifier, integrity key, expected tag, or metadata record is present. Target on-demand self-test state transitions are also pending implementation. The service matrix therefore must not label module integrity as operational.

## P2-04 entropy/DRBG scope note

`EDP-P2-04-R1` freezes Linux `getrandom(flags=0)` as the initial external conditioned provider profile, with no direct noise source and no generic health test applied to conditioned output. This is internal policy only; provider acceptance, entropy credit, formal conditional-test classification, and exact OE-01 execution remain open. Public service scope and ABI are unchanged.
