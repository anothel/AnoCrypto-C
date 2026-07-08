# C99 Thread-Safety Policy

Status: P1-15 draft.

## Goal

This document records the current thread-safety boundary for AnoCrypto-C. It is a design and engineering-test artifact, not a broad concurrency guarantee.

P1-14 adds a public support matrix and CI checklist:

- `docs/design/THREAD_SAFETY_SUPPORT_MATRIX.md`
- `docs/ci/THREAD_SAFETY_CI_CHECKLIST.md`
- `tools/check_thread_safety_policy.cmake`

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
- one-shot hash/HMAC operations with independent caller-owned buffers

Caller-provided operation contexts remain caller-owned. The module does not promise that a single hash, HMAC, or DRBG context can be mutated concurrently by multiple threads.

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
- `ano_crypto_csp_thread_safety_tests`: POSIX-style concurrent HMAC key import/use/destroy loop when thread safety and pthread support are available.

P1-14 adds:

- `ano_crypto_thread_safety_policy_check`: confirms the support matrix, CI checklist, lock sources, build option, test guard, evidence hook, and PoC package note are present.
- `THREAD_SAFETY_CHECK.txt`: generated in release evidence drafts.
- `THREAD_SAFETY_PACKAGE_NOTES.txt`: generated in customer PoC package drafts.

The concurrent test uses public synthetic HMAC fixture bytes only and must not print key material, MAC output, entropy bytes, nonce bytes, DRBG state, generated DRBG bytes, private keys, tokens, or customer data.

## TODO

1. Add Windows-specific concurrent CSP/HMAC coverage.
2. Add sanitizer lanes where supported by the selected compiler/toolchain policy.
3. Review whether module init should block, reject, or otherwise define concurrent startup races more precisely.
4. Add per-header guidance only where wording is stable enough for a public support boundary.
5. Decide if future CSP operations require per-slot locks instead of one global lock.
6. Define behavior for concurrent destroy versus long-running cryptographic operations for future key types.
