# C99 Thread-Safety Policy

Status: P1-29 retained and extended draft.

## Goal

This document records the current thread-safety boundary for AnoCrypto-C. It is a design and engineering-test artifact, not a broad concurrency guarantee.

P1-21 retains the public support matrix and adds cross-platform concurrency and sanitizer review records:

- `docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md`
- `docs/ci/THREAD_SAFETY_CI_CHECKLIST.md`
- `tools/check_thread_safety_policy.cmake`
- `docs/ci/CONCURRENCY_SANITIZER_CI_CHECKLIST.md`
- `tools/check_concurrency_sanitizer_policy.cmake`

## Current policy

AnoCrypto-C currently has a small internal process-wide lock seam for selected mutable global state:

- module lifecycle state
- self-test last-status and last-flags bookkeeping
- in-memory CSP handle table and generation counter

The following API groups are intended to tolerate concurrent calls when `ANO_CRYPTO_ENABLE_THREAD_SAFETY=ON` and a reviewed platform lock branch is active:

- version, status, state-name, and build-identity queries
- module state/status queries
- module init/shutdown state transition attempts, with application-level lifecycle ownership still recommended
- self-test last-status/flag queries
- HMAC key import/destroy/state/is-valid operations
- HMAC-SHA-256 compute using an imported HMAC key handle
- AES-CBC and AES-CMAC operations using imported purpose-specific handles and
  independent caller-owned contexts/buffers
- one-shot hash/HMAC/CMAC operations with independent caller-owned buffers

Caller-provided operation contexts remain caller-owned. The module does not promise that a single hash, HMAC, cipher, CMAC, or DRBG context can be mutated concurrently by multiple threads.

## Key handle snapshot rule

`ano_crypto_hmac_compute_with_key()` copies key material from the CSP table under the internal lock, releases the lock, performs HMAC using the local copy, and clears the local copy before returning.

This means a concurrent destroy of the same handle does not retroactively invalidate an operation that already captured a valid key snapshot. A later operation with the stale handle must fail.

## Lock implementation

P1-6 introduced `src/internal/ac_thread_internal.h` and `src/core/thread_lock.c`.

- POSIX-like platforms use `pthread_mutex_t` when `ANO_CRYPTO_ENABLE_THREAD_SAFETY` is enabled.
- Windows uses `INIT_ONCE` plus `CRITICAL_SECTION` when enabled.
- Unsupported platforms currently build with an internal no-op fallback and must be reviewed before any thread-safety support statement.

## Build options

```sh
cmake -S . -B build -DANO_CRYPTO_ENABLE_THREAD_SAFETY=ON
```

The option is ON by default. If thread support is disabled or unavailable, callers must serialize shared module, self-test, CSP, key-handle, and context access themselves.

## Tests and checks

Existing tests:

- `ano_crypto_thread_lock_tests`: basic internal lock seam smoke test.
- `ano_crypto_csp_thread_safety_tests`: Win32 native-thread or POSIX pthread concurrent HMAC/AES/CMAC import/use/destroy and shared-handle compute loops when the reviewed backend is available.

P1-21 evidence wiring includes:

- `ano_crypto_thread_safety_policy_check`: confirms the support matrix, CI checklist, lock sources, build option, test guard, evidence hook, and PoC package note are present.
- `THREAD_SAFETY_CHECK.txt`: generated in release evidence drafts.
- `THREAD_SAFETY_PACKAGE_NOTES.txt`: generated in customer PoC package drafts.

The concurrent test uses public synthetic HMAC/AES/CMAC fixture bytes only and must not print key material, MAC output, entropy bytes, nonce bytes, DRBG state, generated DRBG bytes, private keys, tokens, or customer data.

## TODO

1. Review actual hosted Windows/macOS/Linux thread artifacts before widening support wording.
2. Review Clang sanitizer runtime and ThreadSanitizer separately from the required GCC ASan/UBSan lane.
3. Extend lifecycle review beyond the init/shutdown generation contract below, especially emergency lock failures and caller-owned operation contexts.
4. Add per-header guidance only where wording is stable enough for a public support boundary.
5. Decide if future CSP operations require per-slot locks instead of one global lock.
6. Implement and test the P3-07 operation-lease contract for future public-key types in P3-08.

## P3-07 approved future public-key concurrency contract

P3-07 adds no public-key code. For P3-08, the approved external semantics are:

- import, lookup, destroy, state query, and allocation are serialized when the
  reviewed lock branch is enabled;
- sign/verify acquires an operation lease/pin before leaving the object-table
  critical section; the complete private key is not copied as a snapshot;
- concurrent explicit destroy marks the object closing, rejects new leases,
  waits for existing leases, and then zeroizes; later stale-handle calls fail;
- callers must not use destroy as a cancellation mechanism;
- module shutdown/fatal transition is a barrier: an in-flight operation finishes
  before state visibility or aborts and clears output;
- operation-local private material is cleared before return;
- thread-safety OFF requires caller serialization of all module and public-key
  handle operations.

The bounded lease/pin implementation, active-operation accounting, shutdown
waiting, lock-failure behavior, and concurrency limits are `TODO_P3_08`. These rules do not extend the current runtime support matrix until
P3-08 implementation and tests pass.


## Phase 2 module init/shutdown contract (Issue #8)

With a supported lock backend, orderly shutdown preempts a startup already in
flight: its successful self-test continuation returns
`ANO_CRYPTO_ERR_NOT_INITIALIZED`, without publishing `OPERATIONAL` or clearing
last-error state. A separately requested init after shutdown runs full startup
checks and may succeed, even while the old startup has not yet returned.

The final commit of startup success requires both `SELF_TEST_RUNNING` and the
same private initialization generation captured under the global lock. A state
check alone cannot distinguish a later startup in the same state. Generations
never wrap: after `UINT64_MAX` admitted startups, further startup requests return
`ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED`; an already operational init remains
idempotent. No public types, status values or symbols are added.

Fatal-state and mandatory-self-test failures take precedence over cancellation.
Even an obsolete startup's mandatory failure remains fatal, and shutdown cannot
recover sticky `ERROR_STATE`. Existing module-owned CSP cleanup remains in the
shutdown and fatal-transition paths. Emergency failure to acquire the lock still
has the documented residual cleanup limitation; this change does not add an
unsynchronized table walk or resolve that separate risk.

When thread safety is OFF/unavailable, callers must serialize module lifecycle
and shared-state access; concurrent schedules are not supported in that mode.
This contract does not revoke existing caller-owned contexts or add an operation
barrier for every cryptographic API across shutdown/reinitialization.

`ano_crypto_module_state_tests` retains its CTest index and first runs the
original library-backed tests, then a standalone fixture compiled from the real
`module_state.c` and native lock backend. Only the fixture substitutes startup
work and CSP cleanup, allowing condition-variable handshakes with no sleeps and
no hooks in production binaries. Cases cover shutdown-only cancellation,
old-init/shutdown/new-init with either completion order, external fatal state,
and an older startup failing after a newer startup succeeds. Existing KATs and
CSP tests still validate real startup work and zeroization separately.
