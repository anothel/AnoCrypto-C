# C99 CSP / Key Lifecycle — Current Design

Status: `FROZEN_INTERNAL_POLICY`; formal CSP classification and accepted zeroization evidence remain `EXTERNAL_CONFIRMATION_REQUIRED`.

## Scope

AnoCrypto-C provides a fixed-size, in-process, module-owned CSP handle table for symmetric keys and reviewed RSA/P-256 public-key objects. It also exposes caller-owned opaque HMAC, AES-CBC, AES-CMAC, AES-256-GCM streaming, HMAC_DRBG, and random-service contexts. This is not a persistent key store or a general key-management subsystem.

The current public surface is 18 headers and 85 exported symbols across retained `ANOCRYPTO_C_0` through `ANOCRYPTO_C_6` and additive `ANOCRYPTO_C_7`. RSA and P-256 public/private keys reuse the same opaque CSP handle with internal role, algorithm, and purpose binding.

## Module-owned handle table

- Capacity: 16 slots shared across symmetric, RSA-2048, and P-256 objects.
- Maximum symmetric secret per slot: 128 bytes. Each slot also has storage for the supported RSA and P-256 representations.
- Supported symmetric purposes: HMAC, block cipher, and CMAC.
- Import validates type, size, flags, and module operational state.
- Exportable-candidate requests are rejected.
- Every allocation receives a generation; stale generations cannot address a reused slot.
- Cryptographic use requires a live purpose-matching state.
- Symmetric services copy a bounded key snapshot for an operation. One-shot paths clear it before return. Caller-owned cipher, CMAC, and AES-256-GCM streaming contexts retain private operation state and follow their service-specific finalization and cleanup rules. RSA and P-256 access the slot under the lock as described below.

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

Only some transitional states are externally observable in ordinary flows. `ZEROIZED` is used as a tombstone after module-wide cleanup. Cryptographic use is rejected, and callers may destroy the tombstone once while its generation still matches. Allocation can reuse the slot and assign a different generation.

## Cleanup rules

### Explicit destroy

`ano_crypto_key_destroy()` is allowed outside `OPERATIONAL`. It clears the complete secret slot and metadata and marks the slot `DESTROYED`. It also accepts a valid-generation `ZEROIZED` tombstone so callers can complete cleanup after shutdown or fatal transition.

### Orderly shutdown

`ano_crypto_module_shutdown()` clears all live module-owned slots under the global lock, including symmetric, RSA, and P-256 slots, and marks them `ZEROIZED`. A later orderly reinitialization permits new imports. It leaves existing tombstones invalid for cryptographic use. See the [module init/shutdown contract](C99_THREAD_SAFETY_POLICY.md#phase-2-module-initshutdown-contract-issue-8) for startup and fatal-state precedence.

### Fatal transition

When a fatal transition acquires the global lock, it clears every live module-owned slot before exposing sticky `ERROR_STATE`. Shutdown acknowledges the error but does not recover it; a fresh process/load is required. If lock acquisition itself fails, the code latches fatal state without unsafely walking the table. This residual cleanup limitation is explicit and requires P2-06/external review.

### Caller-owned contexts

The module does not retain addresses of caller-owned opaque contexts. Coordinate shutdown with users of those contexts, follow the service-specific finalization and cleanup rules, and initialize fresh contexts for the next operational period. A streaming AES-256-GCM context snapshots the expanded key schedule during init. Destroying the originating CSP handle prevents new initialization with that handle while the initialized context retains its snapshot. Module-wide table cleanup covers module-owned slots. Abandoned contexts, forced process termination, OS copies, swap, core dumps, hibernation, VM snapshots, debugger copies, and hardware remanence are outside that boundary. See the [caller-owned context lifecycle](C99_CALLER_OWNED_CONTEXT_LIFECYCLE.md) for per-service cleanup and restart procedures.

## Thread/process rules

- The table and module state use the reviewed global-lock abstraction when thread safety is enabled.
- Callers must serialize mutable caller-owned contexts unless the service specification says otherwise.
- Handles are in-process only and are not persistent or transferable across processes.
- VM/container cloning and suspend/resume policy follows the P2-04 entropy/DRBG constraints and does not convert handles into portable credentials.

## Explicit exclusions

- key export, persistence, wrapping, recovery, backup, rotation, platform keystore, and multi-process sharing;
- additional public-key algorithms, general key generation, key export, or external-provider objects beyond the reviewed RSA-2048 and P-256 profiles;
- integrity-test key material until the P2-03 verifier is implemented;
- universal optimizer, physical-remanence, constant-time, or formal-validation claims.

## Evidence

Retained P2-05 evidence includes explicit-destroy/stale-handle tests, orderly-shutdown and fatal-transition zeroization regressions, a status-only policy review, source/policy checks, local compiler-output capture, install/PoC checks, and sensitive-artifact scanning. The shutdown test checks that an HMAC tombstone remains unusable after reinitialization and can still be explicitly destroyed. P3-04 adds streaming-context final/uninit cleanup and post-handle-snapshot semantics. P3-05 adds caller-workspace scrypt clearing and clean 80-symbol ABI checks. P3-08 adds RSA handle tests. P3-09 adds P-256 handle tests, key binding, deterministic signing, and clean 85-symbol regression. P3-10 adds a restricted one-shot digest without a new CSP object. Exact OE-01 GCC 13.3.0 human review remains `NOT_RUN`.

## P3-08/P3-09 implemented public-key lifecycle

P3-08 and P3-09 use the existing `ano_crypto_csp_handle_t` and share the same table, capacity, and cleanup paths as symmetric handles. Import copies validated components. Public-key handles are verify-only. Private-key handles can sign and verify. Copying a handle value creates an alias to the same object, so destroying the object invalidates every alias. The supported key profiles are recorded in the [RSA](C99_RSA_PKCS1_V15_SHA2_API_GATE.md) and [P-256](C99_ECDSA_P256_SHA256_API_GATE.md) design notes.

With a supported lock backend, sign/verify holds the global lock during access to the key slot and its key-dependent computation. Destroy and lock-protected module cleanup acquire the same lock before clearing slots. The lock protects this part of the operation. Callers that require all API calls to finish before shutdown must quiesce those calls themselves. The [P3-07 architecture gate](C99_PUBLIC_KEY_KEY_HANDLE_ARCHITECTURE_GATE.md) records the original design goals for per-object leases and a full operation barrier. The [thread-safety policy](C99_THREAD_SAFETY_POLICY.md#specialized-security-follow-up) records the outstanding review of that boundary.
