# Thread-Safety Support Matrix

Status: P2-02 target-bound engineering matrix. OE-01 freezes thread safety ON; other platform support remains engineering-only or deferred.

## Purpose

This matrix turns the earlier internal locking note into a public support-boundary draft. It is an engineering support matrix for the current AnoCrypto-C C99 API surface. It is not a broad concurrency guarantee, and it is not official validation evidence.

## Build configuration boundary

| Configuration | Current position | Notes |
|---|---|---|
| `ANO_CRYPTO_ENABLE_THREAD_SAFETY=ON` | Default intended configuration | Protects selected module, self-test, and CSP global state through an internal process-wide lock when the platform branch is reviewed and compiled. |
| `ANO_CRYPTO_ENABLE_THREAD_SAFETY=OFF` | Buildable developer configuration | The library does not serialize global mutable state. Callers must serialize all module, self-test, CSP, key-handle, and shared-context access. |
| CMake `Threads` unavailable | Unsupported fallback for concurrency | The source can still build, but the no-op fallback is not a thread-safety support commitment. |
| Internal test hooks enabled | Test configuration only | Internal symbols may be visible for unit tests. Do not use this as the release-style symbol surface. |
| Release-style no-test build with symbol hiding | Preferred public-symbol check lane | Should be paired with `ANO_CRYPTO_ENABLE_THREAD_SAFETY=ON` before any customer PoC binary handoff. |
| P2-02 OE-01 validation candidate | Frozen internal candidate | Ubuntu 24.04 x86_64 shared Release build with `ANO_CRYPTO_ENABLE_THREAD_SAFETY=ON`; caller-owned mutable contexts still require caller serialization. |

## Public API support matrix

| API area | Concurrent calls with thread-safety ON | Shared object or context rule | Current evidence | P1-14 support note |
|---|---|---|---|---|
| Version/status/name helpers | Supported as read-only helper calls | Returned strings are static read-only pointers; callers must not modify them. | Build-tree tests and install smoke use these helpers. | Safe for status/name queries. |
| Build identity query | Supported as read-only helper call | `ano_crypto_build_identity_t` is caller-provided output; returned string pointers are module-owned and read-only. | Install smoke, self-test, and P1-21 ABI layout regression. | ABI version 1 field order is recorded for ABI-0; platform type/alignment review remains separate. |
| Module lifecycle | Serialized by the global lock | Module state is process-wide. Callers should initialize once at process startup and avoid concurrent init/shutdown races in application design. | Module state tests and normal CTest startup paths. | Concurrent calls are serialized, but application-level lifecycle ownership is still recommended. |
| Self-test status queries | Serialized by the global lock | Last-status and last-flags are process-wide. | Module/self-test tests. | Query concurrency is supported with thread-safety ON. |
| Explicit self-test run | Partially serialized through state bookkeeping | Startup self-test is normally reached through module init. | Startup self-test flow and KAT tests. | Do not schedule repeated concurrent self-test runs in a customer integration until a dedicated policy is reviewed. |
| One-shot SHA-256 | Supported when buffers are independent | Input/output buffers are caller-owned. Do not mutate the same output buffer from multiple threads. | KAT, extended KAT, build-tree PoC. | Reentrant for independent buffers after module is operational. |
| Streaming SHA-256 context | Separate contexts supported | A single caller-provided context must not be updated or finalized concurrently by multiple threads. | KAT and extended chunk tests. | Caller owns context serialization. |
| One-shot HMAC-SHA-256 with raw key input | Supported when buffers are independent | Key, message, and output buffers are caller-owned. Do not share mutable buffers without external serialization. | KAT and extended KAT. | Reentrant for independent buffers after module is operational. |
| Streaming HMAC context | Separate contexts supported | A single caller-provided HMAC context must not be mutated concurrently. | KAT and negative API tests. | Caller owns context serialization. |
| HMAC key import/destroy/state | Serialized by the global CSP lock | Handles are process-local opaque values. Destroying a handle invalidates later use of that handle. | CSP lifecycle, stale-handle, exhaustion, and cross-platform concurrent smoke tests. | Supported for process-local HMAC key handles under the current capacity limit. |
| HMAC compute with key handle | Supported with snapshot rule | The implementation copies key material under the lock, releases the lock, computes locally, and clears the local copy. | HMAC key handle KAT and cross-platform concurrent smoke test. | A concurrent destroy does not affect an operation that already captured a valid snapshot; later stale-handle calls must fail. |
| AES-CBC and AES-CMAC key handles | Supported for independent operation contexts with snapshot rule | Key material is copied under the CSP lock into operation-local state. A single caller-owned cipher or CMAC context must not be mutated concurrently. | Cross-platform per-thread and shared-handle CSP/HMAC/AES/CMAC concurrency regression. | A captured operation may finish after concurrent destroy; later stale-handle use fails. |
| AES-256-GCM one-shot | Supported for independent calls with snapshot rule | Key material is copied under the CSP lock into operation-local state; nonce, AAD, payload, tag, and output ranges must follow the public alias rules. | NIST KAT, negative, OpenSSL differential, current/retained consumer, install, and PoC regression. | A captured call may finish after concurrent destroy; later stale-handle calls fail. |
| AES-256-GCM streaming context | Separate contexts supported with snapshot rule | Init copies the expanded operation state from a live handle. A single caller-owned context must not be updated, finalized, or uninitialized concurrently. Decrypt output remains unverified until final. | NIST chunk/state KAT, negative lifecycle/overlap tests, OpenSSL streaming differential, exact `SKT1` fixture, current/retained consumer, install, and PoC regression. | A context initialized before concurrent handle destroy may finish; new init with the stale handle fails. Caller owns context serialization and pre-final plaintext withholding. |
| Fixed-profile scrypt | Separate calls supported only with disjoint caller storage | Password and salt are read-only and may overlap each other. Each call requires its own output and 33,556,544-byte workspace; sharing either mutable range across concurrent calls requires caller serialization. | KAT, negative alias/resource tests, OpenSSL differential, exact `SKP1` fixture, current/retained consumer, install, and PoC regression. | No module-global mutable state or production heap allocation is introduced. Caller owns workspace allocation, lifetime, and concurrency. |
| HMAC_DRBG context | Separate contexts supported | A single DRBG context is caller-owned mutable state and must not be generated/reseeded/uninstantiated concurrently. | KAT, boundary tests, negative tests. | Caller owns context serialization. |
| Internal entropy adapter/provider seam | Internal only | Provider tests record status only and must not log entropy bytes. | Provider status tests. | Not a public random-service support commitment. |
| Zeroize helper | Caller-owned buffer only | The same mutable buffer must not be cleared concurrently with other reads/writes. | CSP lifecycle paths and tests. | Caller owns buffer serialization. |

## Platform support notes

| Platform branch | Current implementation | Current CI intent | Notes |
|---|---|---|---|
| Linux / POSIX-like | `pthread_mutex_t` branch | OE-01 selected; ordinary/concurrency engineering tests plus selected-candidate evidence plan | Local GCC engineering lanes are regression evidence only. Exact Ubuntu/GCC13 candidate execution and review remain pending. |
| Apple | `pthread_mutex_t` branch | Deferred engineering portability lane | Outside initial candidate; actual hosted artifact review remains `NOT_RUN`/`UNREVIEWED`. |
| Windows | `INIT_ONCE` and `CRITICAL_SECTION` branch | Deferred engineering portability lane | Outside initial candidate; workflow exists but actual hosted review remains `NOT_RUN`/`UNREVIEWED`. |
| Other platforms | No-op fallback | Build-only until reviewed | Must be listed as unsupported for thread-safety unless a real lock branch is added and tested. |

## Review rules

- Do not claim that caller-owned hash, HMAC, or DRBG contexts are concurrently mutable.
- Do not print keys, entropy input, nonce input, DRBG state, MAC output, generated DRBG bytes, private keys, tokens, or customer data in concurrency tests or logs.
- Keep public thread-safety wording tied to a specific build configuration and platform branch.
- Re-run `ano_crypto_thread_safety_policy_check`, CTest, install smoke, PoC package smoke, and release-style symbol scan after changing synchronization code or public headers.
- Treat sanitizer output, hosted CI logs, and concurrency smoke tests as engineering evidence until the release evidence process is reviewed.

## TODO before support hardening

1. Review and retain actual hosted thread-platform artifacts for Linux, Apple, and Windows.
2. Review Clang sanitizer runtime and ThreadSanitizer before adding required lanes.
3. Decide whether module init should block, reject, or otherwise define concurrent startup races more precisely.
4. Decide whether future CSP operations need per-slot locking instead of one global lock.
5. Add per-header concurrency notes only where they can be kept stable without freezing unnecessary ABI.
