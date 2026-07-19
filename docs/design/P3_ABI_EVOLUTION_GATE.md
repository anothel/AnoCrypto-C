# P3 API and ABI Evolution Gate

Status: `P3_07_NO_CHANGE_P3_08_REVIEW_REQUIRED`

## Current retained public surface

| ABI line | Added symbols | Scope |
|---|---:|---|
| `ANOCRYPTO_C_0` | 69 | retained P1 public surface |
| `ANOCRYPTO_C_1` | 1 | HKDF-SHA-256 |
| `ANOCRYPTO_C_2` | 2 | AES-256-GCM one-shot |
| `ANOCRYPTO_C_3` | 7 | AES-256-GCM streaming |
| `ANOCRYPTO_C_4` | 1 | fixed-profile scrypt |
| total | 80 | current exported symbols |

Public headers remain 16 and the retained compatibility baseline remains P1-29.
Every existing declaration, symbol name, and ABI assignment remains unchanged.

## P3-07 result

P3-07 is design-only and changes none of the following:

- public headers or header bytes;
- public types, enum values, constants, or macros;
- functions or exported symbols;
- symbol-version map or exported-symbol manifests;
- release/runtime version identity;
- CMake targets or installed SDK files;
- P1-29 source/ABI compatibility baseline.

`ANOCRYPTO_C_5` is not introduced or reserved.

## P3-08 candidate budget

The architecture review found that the smallest reasonable RSA/key-handle
candidate is three public functions:

1. one RSA public/private component import selected by the existing key role;
2. one generic message-based signature-sign function;
3. one generic message-based signature-verify function.

Existing `ano_crypto_key_destroy()`, `ano_crypto_key_get_state()`,
`ano_crypto_csp_handle_is_valid()`, and `ano_crypto_csp_handle_get_state()` are
reused. No public asymmetric-metadata query, key-binding query, signature-size,
raw-RSA, key-generation, export, persistence, or provider-routing symbol is
justified by current exact product evidence. Certificate/public-key binding is
performed by supplying the expected public `n/e` together with private
components and validating the complete relationship during import.

This is a candidate budget, not an approved ABI. P3-08 must independently
freeze:

- exact names and prototypes;
- extending `ac_key.h` versus adding one focused public header;
- exact scheme enum names/values and final function/type/constant count;
- RSA component representation, accepted sizes/exponents, and resource bounds;
- message-based sign/verify, SHA-2 ownership, and canonical PKCS#1 v1.5
  `DigestInfo`/EMSA processing;
- null, overflow, alias, overlap, size-query, short-buffer, and output-clearing
  semantics;
- lifecycle, zeroization, module-state, operation-lease, and concurrency rules;
- KAT, negative, boundary, differential, current/retained consumer,
  install/export, PoC, symbol-version, and AnoPKI compatibility evidence;
- one additive ABI line only after the review passes.

No existing symbol may be renamed, removed, moved, or assigned to another ABI
line. Candidate names, final count, header placement, and future ABI-line
identifier remain unapproved until P3-08 closes its API/ABI gate.

## Later additions

A later ECDSA slice may reuse generic sign/verify and add one P-256 import
function after its own API/ABI gate. A restricted SHA-1 slice requires a separate
misuse-resistant digest-only gate. P3-07 pre-approves neither public declaration,
symbol, enum value, nor version line.
