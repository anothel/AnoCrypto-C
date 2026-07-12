# C99 AES-CMAC API Policy

Status: P1-28 implemented public API policy.

## Public surface

`include/anocrypto/ac_cmac.h` exposes exactly eight functions:

- `ano_crypto_cmac_context_size`;
- `ano_crypto_cmac_mac_size`;
- `ano_crypto_cmac_init`;
- `ano_crypto_cmac_update`;
- `ano_crypto_cmac_final`;
- `ano_crypto_cmac_uninit`;
- `ano_crypto_cmac_compute_with_key`;
- `ano_crypto_cmac_verify_with_key`.

## Context and output rules

- Context storage is caller-owned and opaque.
- Functions return `ano_crypto_status_t` and use output parameters.
- The only accepted tag size is 16 bytes.
- Empty messages are valid.
- `update` accepts arbitrary byte lengths; `NULL` is accepted only with length zero.
- Successful `final` clears secret-dependent context state.
- `uninit` is allowed outside OPERATIONAL for cleanup.
- Short output returns `ANO_CRYPTO_ERR_BUFFER_TOO_SMALL` and no partial tag.

## Verification rules

One-shot verification accepts exactly 16 tag bytes. A mismatch returns
`ANO_CRYPTO_ERR_AUTHENTICATION_FAILED`. The implementation uses an internal
constant-work comparison candidate and does not report a mismatch position.
This is not a universal side-channel claim.

## CSP rules

- Only `ANO_CRYPTO_KEY_TYPE_CMAC` handles are accepted.
- AES encryption handles are rejected by CMAC services.
- CMAC handles are rejected by CBC services.
- Key lengths are exactly 16, 24, or 32 bytes.
- Exportable-candidate flags remain rejected.
- Destroyed, stale, wrong-purpose, and malformed handles are rejected.

## ABI decision

P1-28 adds one public header, one key-purpose enum value, and eight exported
functions. The clean Release ABI-0 baseline therefore grows from 61 to 69
names with no removal or rename of an existing symbol.
