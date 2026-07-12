# C99 AES-CMAC API Policy

Status: P1-29 hardened public API policy.

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
- Context, input, output, and output-length storage must obey the documented
  non-overlap rules; rejected preflight does not consume a valid context.
- Failed reinitialization after valid size preflight clears the prior context.
- `uninit` is allowed outside OPERATIONAL for cleanup. A short length clears only
  the supplied extent and returns `BUFFER_TOO_SMALL`; a sufficient oversized
  length clears only the reviewed context extent.
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

P1-28 added one public header, one key-purpose enum value, and eight exported
functions. The clean Release ABI-0 baseline therefore grows from 61 to 69
names with no removal or rename of an existing symbol.

## P1-29 hardening decision

P1-29 does not add a public declaration or symbol. It strengthens private
context validation, cleanup, overlap, lifecycle, concurrency, differential, and
compiler-review evidence while retaining the 69-name ABI-0 baseline.
