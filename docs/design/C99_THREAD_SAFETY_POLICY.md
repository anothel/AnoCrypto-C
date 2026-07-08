# C99 Thread-Safety Policy

Status: P1-10 draft.

## Goal

This document records the current thread-safety boundary for anocrypto-c. It is a design and engineering-test artifact, not a broad concurrency guarantee.

## Current Policy

P1-6 introduces a small internal process-wide lock seam for mutable global state:

- module lifecycle state
- self-test last-status and last-flags bookkeeping
- in-memory CSP handle table and generation counter

The following APIs are intended to be safe for concurrent calls under that limited policy:

- module state/status queries
- module init/shutdown state transition attempts
- self-test last-status/flag queries
- HMAC key import/destroy/state/is-valid operations
- HMAC-SHA-256 compute using an imported key handle

Caller-provided operation contexts remain caller-owned. The module does not promise that a single hash, HMAC, or DRBG context can be mutated concurrently by multiple threads.

## Key Handle Snapshot Rule

`ano_crypto_hmac_compute_with_key()` copies key material from the CSP table under the internal lock, releases the lock, performs HMAC using the local copy, and zeroizes the local copy before returning.

This means a concurrent destroy of the same handle does not retroactively invalidate an operation that already captured a valid key snapshot. A later operation with the stale handle must fail.

## Lock Implementation

P1-6 adds `src/internal/ac_thread_internal.h` and `src/core/thread_lock.c`.

- POSIX-like platforms use `pthread_mutex_t` when `ANO_CRYPTO_ENABLE_THREAD_SAFETY` is enabled.
- Windows uses `INIT_ONCE` plus `CRITICAL_SECTION` when enabled.
- Unsupported platforms currently build with an internal no-op fallback and must be reviewed before any thread-safety commitment.

## Build Option

```sh
cmake -S . -B build -DANO_CRYPTO_ENABLE_THREAD_SAFETY=ON
```

The option is ON by default in P1-6 when thread support is available through CMake `Threads`.

## Tests

P1-6 adds:

- `ano_crypto_thread_lock_tests`: basic internal lock seam smoke test.
- `ano_crypto_csp_thread_safety_tests`: POSIX-only concurrent HMAC key import/use/destroy loop.

The concurrent test uses public synthetic HMAC fixture bytes only and must not print key material.

## TODO

1. Decide whether thread safety is part of the public ABI support contract or only a build configuration.
2. Add Windows CI coverage before claiming Windows thread-safety support.
3. Add thread sanitizer jobs where available.
4. Review whether module init should block or reject concurrent init during self-test.
5. Add per-context guidance to each public header.
6. Decide if future CSP operations require per-slot locks instead of one global lock.
7. Define behavior for concurrent destroy versus long-running cryptographic operations.
