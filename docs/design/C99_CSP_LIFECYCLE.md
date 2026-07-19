# C99 CSP / Key Lifecycle — Current Design

Status: `FROZEN_INTERNAL_POLICY`; formal CSP classification and accepted zeroization evidence remain `EXTERNAL_CONFIRMATION_REQUIRED`.

## Scope

AnoCrypto-C provides a fixed-size, in-process, module-owned CSP handle table for HMAC, block-cipher, and CMAC secret keys. It also exposes caller-owned opaque HMAC, AES-CBC, AES-CMAC, AES-256-GCM streaming, HMAC_DRBG, and random-service contexts. This is not a persistent key store or a general key-management subsystem.

The current public surface is 16 headers and 80 exported symbols across retained `ANOCRYPTO_C_0`, `ANOCRYPTO_C_1`, `ANOCRYPTO_C_2`, `ANOCRYPTO_C_3`, and additive `ANOCRYPTO_C_4`.

## Module-owned handle table

- Capacity: 16 slots.
- Maximum secret per slot: 128 bytes.
- Supported purposes: HMAC, block cipher, and CMAC.
- Import validates type, size, flags, and module operational state.
- Exportable-candidate requests are rejected.
- Every allocation receives a generation; stale generations cannot address a reused slot.
- Cryptographic use requires a live purpose-matching state.
- The module copies a bounded key snapshot for an operation. One-shot paths clear it before return; caller-owned cipher, CMAC, and AES-256-GCM streaming contexts retain only their private expanded operation state until finalization or explicit uninit clears the reviewed opaque extent.

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

The module does not retain addresses of caller-owned opaque contexts. Callers must invoke the matching final, uninit, uninstantiate, or random-uninit path. A streaming AES-256-GCM context snapshots the expanded key schedule during init, so destroying the originating CSP handle prevents new initialization but does not revoke an already initialized context. Abandoned contexts, forced process termination, OS copies, swap, core dumps, hibernation, VM snapshots, debugger copies, and hardware remanence are not claimed cleared by module-wide table cleanup.

## Thread/process rules

- The table and module state use the reviewed global-lock abstraction when thread safety is enabled.
- Callers must serialize mutable caller-owned contexts unless the service specification says otherwise.
- Handles are in-process only and are not persistent or transferable across processes.
- VM/container cloning and suspend/resume policy follows the P2-04 entropy/DRBG constraints and does not convert handles into portable credentials.

## Explicit exclusions

- key export, persistence, wrapping, recovery, backup, rotation, platform keystore, and multi-process sharing;
- public/private-key objects in the current runtime; P3-07 defines only a future contract;
- integrity-test key material until the P2-03 verifier is implemented;
- universal optimizer, physical-remanence, constant-time, or formal-validation claims.

## Evidence

Retained P2-05 evidence includes explicit-destroy/stale-handle tests, orderly-shutdown and fatal-transition zeroization regressions, a status-only policy review, source/policy checks, local compiler-output capture, install/PoC checks, and sensitive-artifact scanning. P3-04 adds streaming-context final/uninit cleanup and post-handle-snapshot semantics. P3-05 adds caller-workspace scrypt clearing, clean 80-symbol ABI checks, and current/retained consumer regression. Exact OE-01 GCC 13.3.0 human review remains `NOT_RUN`.

## P3-07 approved future public-key lifecycle

P3-07 changes no current CSP implementation. It approves the following contract
for the P3-08 RSA/key-handle foundation:

- reuse `ano_crypto_csp_handle_t` and the existing public/private key-class
  values;
- use a separate bounded module-owned public-key object pool rather than
  silently enlarging every existing 128-byte symmetric slot;
- treat copied handle values as aliases to one object, not independent
  ownership;
- import immutable algorithm-native components and retain no caller pointer;
- accept `ANO_CRYPTO_KEY_FLAG_NONE` only in the first slice;
- make public handles verify-only and private handles sign-only;
- reuse existing destroy/state/is-valid functions;
- invalidate every alias on destroy and reject stale generations;
- clear complete private objects and metadata on destroy, orderly shutdown, and
  fatal transition;
- acquire an operation lease/pin rather than copying a complete private-key
  snapshot; destroy marks the object closing, rejects new leases, waits for
  active leases, and then zeroizes;
- require a module-wide shutdown/fatal barrier so no public-key operation emits
  output after the transition becomes externally visible.

Exact pool capacity, object size, RSA limits, handle-index partition, and
concurrent-operation ceiling remain `TODO_P3_08_RESOURCE_GATE`. The lease/pin
semantics are approved, but no public-key object exists in the P3-07 runtime.
