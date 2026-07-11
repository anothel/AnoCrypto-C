# C99 CMAC API Gate

Status: P1-27 reviewed draft for a future implementation step. No public ABI is added here.

## Proposed public header

A future implementation may add `include/anocrypto/ac_cmac.h` only after the
API and symbol delta are reviewed. The proposed narrow surface is:

- `ano_crypto_cmac_context_size`;
- `ano_crypto_cmac_mac_size`;
- `ano_crypto_cmac_init`;
- `ano_crypto_cmac_update`;
- `ano_crypto_cmac_final`;
- `ano_crypto_cmac_uninit`;
- `ano_crypto_cmac_compute_with_key`;
- `ano_crypto_cmac_verify_with_key`.

This list is a gate proposal, not a declaration available to callers.

## Context and output rules

- Context storage is caller-owned and opaque.
- Functions return `ano_crypto_status_t` and use output parameters.
- The only output size is 16 bytes.
- Tag truncation is not accepted in the initial slice.
- Empty messages are valid.
- `update` accepts arbitrary byte lengths, including zero-length input where a
  non-NULL pointer is not required.
- Successful `final` clears secret-dependent context state and makes the
  context unusable until reinitialized.
- `uninit` is allowed outside the module operational state for cleanup.
- Short output buffers return `ANO_CRYPTO_ERR_BUFFER_TOO_SMALL` without
  emitting a partial tag.

## Verification rules

The proposed one-shot verification API accepts exactly 16 tag bytes and returns
`ANO_CRYPTO_ERR_AUTHENTICATION_FAILED` for a mismatch. It must use an internal
constant-work comparison candidate and must not reveal a mismatch position.
This is an implementation requirement, not a completed side-channel claim.

## CSP rules

- Only a dedicated CMAC-purpose handle is accepted.
- AES encryption handles are rejected by CMAC services.
- CMAC handles are rejected by CBC services.
- Accepted key lengths are exactly 16, 24, or 32 bytes.
- Exportable-candidate flags remain rejected in the initial slice.
- Destroyed, stale, wrong-purpose, and malformed handles are rejected.

## ABI gate

P1-27 preserves the current `ac_key.h`, public headers, enum values, and
61-symbol ABI-0 baseline. The implementation step must explicitly review:

1. the new key-purpose enum value or equivalent usage mechanism;
2. the exact public function list;
3. umbrella-header and install-package exposure;
4. exported-symbol baseline growth;
5. cross-service key-purpose negative tests;
6. source and binary compatibility notes.
