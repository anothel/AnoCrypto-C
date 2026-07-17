# C99 CSP / Key Lifecycle — P2-05 Current Design

Status: `FROZEN_INTERNAL_POLICY`; formal CSP classification and accepted zeroization evidence remain `EXTERNAL_CONFIRMATION_REQUIRED`.

## Scope

AnoCrypto-C provides a fixed-size, in-process, module-owned CSP handle table for HMAC, block-cipher, and CMAC secret keys. It also exposes caller-owned opaque HMAC, AES-CBC, AES-CMAC, HMAC_DRBG, and random-service contexts. This is not a persistent key store or a general key-management subsystem.

Current public surface remains 14 headers, 69 exported symbols, and `ANOCRYPTO_C_0`.

## Module-owned handle table

- Capacity: 16 slots.
- Maximum secret per slot: 128 bytes.
- Supported purposes: HMAC, block cipher, and CMAC.
- Import validates type, size, flags, and module operational state.
- Exportable-candidate requests are rejected.
- Every allocation receives a generation; stale generations cannot address a reused slot.
- Cryptographic use requires a live purpose-matching state.
- The module copies a bounded key snapshot for an operation and clears that snapshot before return.

## Handle states

```text
INVALID
  -> ALLOCATED
  -> INITIALIZED
  -> ACTIVE
  -> RESTRICTED
  -> ZEROIZED
  -> DESTROYED
```

Not every transitional state is currently externally observable in ordinary flows. `ZEROIZED` is used as a tombstone after module-wide cleanup: it is invalid for cryptographic use but may be explicitly destroyed once. Slot reuse assigns a new generation, so the old handle remains invalid.

## Cleanup rules

### Explicit destroy

`ano_crypto_key_destroy()` is allowed outside `OPERATIONAL`. It clears the complete secret slot and metadata and marks the slot `DESTROYED`. It also accepts a valid-generation `ZEROIZED` tombstone so callers can complete cleanup after shutdown or fatal transition.

### Orderly shutdown

`ano_crypto_module_shutdown()` clears all live module-owned HMAC/AES/CMAC slots under the global lock and changes them to `ZEROIZED`. A later orderly reinitialization is permitted, but old handles remain invalid.

### Fatal transition

When a fatal transition acquires the global lock, it clears every live module-owned slot before exposing sticky `ERROR_STATE`. Shutdown acknowledges the error but does not recover it; a fresh process/load is required. If lock acquisition itself fails, the code latches fatal state without unsafely walking the table. This residual cleanup limitation is explicit and requires P2-06/external review.

### Caller-owned contexts

The module does not retain addresses of caller-owned opaque contexts. Callers must invoke the matching final, uninit, uninstantiate, or random-uninit path. Abandoned contexts, forced process termination, OS copies, swap, core dumps, hibernation, VM snapshots, debugger copies, and hardware remanence are not claimed cleared by module-wide table cleanup.

## Thread/process rules

- The table and module state use the reviewed global-lock abstraction when thread safety is enabled.
- Callers must serialize mutable caller-owned contexts unless the service specification says otherwise.
- Handles are in-process only and are not persistent or transferable across processes.
- VM/container cloning and suspend/resume policy follows the P2-04 entropy/DRBG constraints and does not convert handles into portable credentials.

## Explicit exclusions

- key export, persistence, wrapping, recovery, backup, rotation, platform keystore, and multi-process sharing;
- public/private-key objects;
- integrity-test key material until the P2-03 verifier is implemented;
- universal optimizer, physical-remanence, constant-time, or formal-validation claims.

## Evidence

P2-05 evidence includes explicit-destroy/stale-handle tests, orderly-shutdown and fatal-transition zeroization regressions, a status-only policy review, source/policy checks, local compiler-output capture, clean 69-symbol ABI checks, install/PoC checks, and sensitive-artifact scanning. Exact OE-01 GCC 13.3.0 human review remains `NOT_RUN`.
