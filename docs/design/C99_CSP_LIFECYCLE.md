# C99 CSP / Key Lifecycle Skeleton

Status: P1-22 engineering design.

## Implemented lifecycle slice

anocrypto-c currently provides a small in-process CSP handle table for HMAC keys. This is a lifecycle and API-gating slice, not a complete key-management subsystem.

Implemented behavior:

- `ano_crypto_key_import_secret()` imports HMAC keys into an opaque `ano_crypto_csp_handle_t`.
- `ano_crypto_hmac_compute_with_key()` computes HMAC-SHA-256, HMAC-SHA-384, or HMAC-SHA-512 using an imported HMAC key handle.
- `ano_crypto_key_destroy()` zeroizes stored key bytes and marks the handle as destroyed.
- `ano_crypto_key_get_state()` and `ano_crypto_csp_handle_is_valid()` expose state/validity queries without exposing key material.
- Cleanup through `ano_crypto_key_destroy()` is allowed outside `OPERATIONAL` so callers can clear CSP storage during shutdown/error cleanup.
- Negative tests cover fixed-table exhaustion, generation-based stale-handle rejection, slot reuse after destroy, and algorithm/handle misuse.
- Cross-platform Win32/pthread regression tests cover independent handles and supported shared-handle reads when thread safety is enabled.

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
- Thread safety is available only when `ANO_CRYPTO_ENABLE_THREAD_SAFETY=ON` and a reviewed platform lock backend is selected; otherwise callers must serialize CSP access.
- Handle table capacity is intentionally small and fixed for P1 tests.
- Exhaustion currently returns `ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED`.

## Sensitive data rules

- Raw key bytes must not be printed, logged, copied to evidence, or included in diagnostics.
- Test keys are public synthetic KAT inputs only.
- Destroy paths must zeroize key storage before marking the slot destroyed.

## TODO(P1/P2)

1. Define import/export permission matrix by key type and candidate mode.
2. Add internal zeroization evidence strategy for optimized builds.
3. Review long-running handle exhaustion and process lifecycle behavior on selected customer platforms.
4. Decide whether CSP/key lifecycle belongs inside the validation-target boundary for each product package.
