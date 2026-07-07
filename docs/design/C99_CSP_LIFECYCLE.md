# C99 CSP / Key Lifecycle Skeleton

Status: P1-6 draft.

## Implemented P1-5 slice

AnoCrypto-C currently provides a small in-process CSP handle table for HMAC keys. This is a lifecycle and API-gating slice, not a complete key-management subsystem.

Implemented behavior:

- `ano_crypto_key_import_secret()` imports HMAC keys into an opaque `ano_crypto_csp_handle_t`.
- `ano_crypto_hmac_compute_with_key()` computes HMAC-SHA-256 using an imported HMAC key handle.
- `ano_crypto_key_destroy()` zeroizes stored key bytes and marks the handle as destroyed.
- `ano_crypto_key_get_state()` and `ano_crypto_csp_handle_is_valid()` expose state/validity queries without exposing key material.
- Cleanup through `ano_crypto_key_destroy()` is allowed outside `OPERATIONAL` so callers can clear CSP storage during shutdown/error cleanup.
- P1-5 negative tests cover fixed-table exhaustion, generation-based stale-handle rejection, and slot reuse after destroy.

## Candidate CSP States

```text
INVALID
ALLOCATED
INITIALIZED
ACTIVE
RESTRICTED
ZEROIZED
DESTROYED
```

The current implementation uses `ACTIVE` for imported HMAC keys and `DESTROYED` after destroy. After a destroyed slot is reused with a new generation, the old handle resolves to `INVALID`. Other states are ABI-reserved for later lifecycle work.

## Current constraints

- Only HMAC keys are accepted by `ano_crypto_key_import_secret()`.
- Imported key length must be 1..128 bytes.
- Exportable-key flag is rejected until export policy exists.
- Key material is stored in process memory only.
- No persistence, serialization, export, or keystore integration is implemented.
- No thread-safety guarantee is made yet.
- Handle table capacity is intentionally small and fixed for P1 tests.
- Exhaustion currently returns `ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED`.

## Sensitive data rules

- Raw key bytes must not be printed, logged, copied to evidence, or included in diagnostics.
- Test keys are public synthetic KAT inputs only.
- Destroy paths must zeroize key storage before marking the slot destroyed.

## TODO(P1/P2)

1. Decide handle table ownership, locking, and process model.
2. Define import/export permission matrix by key type and candidate mode.
3. Add internal zeroization evidence strategy for optimized builds.
4. Add thread-safety tests after the locking policy is selected.
5. Decide whether CSP/key lifecycle belongs inside the validation-target boundary for each product package.
