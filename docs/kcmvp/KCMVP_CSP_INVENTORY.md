# KCMVP CSP and Sensitive Intermediate Inventory — P2-05

Status: `FROZEN_INTERNAL_POLICY`; formal CSP classification remains `EXTERNAL_CONFIRMATION_REQUIRED`.

## 1. Controlled identifiers

| Item | Value |
|---|---|
| CSP/zeroization policy | `CZP-P2-05-R1` |
| Validation candidate | `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` |
| Runtime form | Linux x86-64 shared library candidate |
| Public ABI | 14 headers, 69 symbols, `ANOCRYPTO_C_0` |
| Formal institution acceptance | `EXTERNAL_CONFIRMATION_REQUIRED` |
| Exact selected-OE compiler review | `NOT_RUN` |
| P1-34 hosted evidence | `BLOCKED_EXTERNAL: GITHUB_ACTIONS_BILLING` |

This inventory describes the current source and the P2-05 internal candidate policy. It
is not a formal institution-approved CSP list and does not claim residual-data absence
outside the specifically reviewed storage and lifecycle paths.

## 2. Classification rules

- **CSP** means secret or secret-derived state whose disclosure or reuse could affect an
  approved security service.
- **Sensitive intermediate** means transient data requiring clearing even when it is not
  itself a long-lived key.
- **Application data** such as plaintext, ciphertext, IV, digest, and tag is kept out of
  evidence and logs, but is not automatically classified as a module-owned CSP.
- Caller-owned opaque contexts are inside the cryptographic service data path but are not
  retained in a module registry. Their addresses and lifetimes are owned by the caller.
- Reserved public enum values are not evidence that a key type or service is implemented.

## 3. Module-owned CSP inventory

| ID | CSP / storage | Creation and owner | Use and duplication | Destruction / zeroization | Evidence | Status |
|---|---|---|---|---|---|---|
| `CSP-001` | Raw HMAC key in `g_csp_slots[].key_material` | Imported through `ano_crypto_key_import_secret(HMAC)`; module-owned fixed table | Copied under the global lock into a bounded stack snapshot for handle-based HMAC; never exported through public API | Explicit `ano_crypto_key_destroy`; P2-05 orderly shutdown; P2-05 fatal transition; slot reuse clears the entire slot first | key-handle KAT, lifecycle negative tests, P2-05 shutdown/fatal tests, compiler capture | `IMPLEMENTED_INTERNAL` |
| `CSP-002` | Raw AES key for CBC in CSP table | Imported as `BLOCK_CIPHER`, length 16/24/32 | Bounded stack copy initializes caller-owned expanded schedule | Same module-table paths as `CSP-001` | AES KAT/negative/stress, P2-05 tests | `IMPLEMENTED_INTERNAL` |
| `CSP-003` | Raw AES key for CMAC in CSP table | Imported as dedicated `CMAC`, length 16/24/32 | Bounded stack copy initializes CMAC expanded schedule and subkeys | Same module-table paths as `CSP-001` | CMAC KAT/negative/stress, P2-05 tests | `IMPLEMENTED_INTERNAL` |
| `CSP-004` | Future module-integrity HMAC key | Selected P2-03 design only | No runtime parser, tag verifier, key injection, or state exists | Not applicable until implementation gate | P2-03 delta register | `SELECTED_CANDIDATE_NOT_IMPLEMENTED` |

The CSP table capacity is 16 slots and each slot reserves 128 bytes. A handle is a
process-local generation/index token, not a key value and not a persistent identifier.

## 4. Caller-owned CSP and secret-derived context inventory

| ID | State | Owner / storage | Lifetime | Required clearing | Current path | Status |
|---|---|---|---|---|---|---|
| `CSP-101` | Streaming HMAC inner state and outer pad | Caller-owned opaque HMAC context | `hmac_init` to `hmac_final`, failure, reinit, or `hmac_uninit` | Clear full supplied context | init fail-closed, final clears, explicit uninit clears | `IMPLEMENTED`; caller must uninit abandoned contexts |
| `CSP-102` | AES expanded round keys and CBC chaining state | Caller-owned opaque cipher context | `cipher_init` to failure/reinit/`cipher_uninit` | Clear full supplied context | failed reinit clears, operation failure clears, one-shot stack clears, explicit uninit clears | `IMPLEMENTED`; caller must uninit abandoned contexts |
| `CSP-103` | AES-CMAC expanded round keys, K1/K2, chain, pending block | Caller-owned opaque CMAC context | `cmac_init` to final/failure/reinit/`cmac_uninit` | Clear full supplied context | fail-closed init, final/uninit/failure clearing | `IMPLEMENTED`; caller must uninit abandoned contexts |
| `CSP-104` | HMAC_DRBG-SHA-256 `Key`, `V`, reseed counter and process marker | Caller-owned DRBG/random context | instantiate to error/uninstantiate | Clear full supplied context | instantiate failure and uninstantiate clear; error paths documented/tested | `IMPLEMENTED`; caller must uninstantiate abandoned contexts |

`ano_crypto_module_shutdown()` cannot clear these contexts because the module does not
retain their addresses. Shutdown blocks further cryptographic use and clears the
module-owned handle table, but the application must call the matching `*_uninit()` or
clear/deallocate its context storage according to the integration policy.

## 5. Sensitive intermediate inventory

| ID | Intermediate | Code path | Clearing rule | Status / limitation |
|---|---|---|---|---|
| `SI-001` | Stack copy of handle-backed HMAC key | `hmac.c` | Entire 128-byte buffer cleared before and after use and on copy failure | Implemented |
| `SI-002` | Stack copy of handle-backed AES key | `cipher.c` | Entire AES-256-sized buffer cleared before and after schedule initialization and on failure | Implemented |
| `SI-003` | Stack copy of handle-backed CMAC key | `cmac.c` | Entire AES-256-sized buffer cleared before and after initialization and on failure | Implemented |
| `SI-004` | HMAC key block, hashed key, inner digest, outer context | `hmac.c` | Cleared on success and failure paths | Implemented; compiler-specific retention review remains target-specific |
| `SI-005` | AES input/work blocks and CMAC temporary blocks/subkeys | `cipher.c`, `cmac.c`, `aes.c` | Cleared at operation end and failure paths | Implemented source paths; selected-OE machine-code review pending |
| `SI-006` | Entropy input, nonce, DRBG next key/value and provider state | `drbg.c`, entropy/random adapters | Cleared after use and on failure; never written to evidence | Implemented under P2-04 policy |
| `SI-007` | Self-test local keys, contexts and outputs | `selftest.c` | Cleared before return along reviewed branches | Engineering KAT path; selected-OE review pending |
| `SI-008` | Future integrity computed tag, expected tag and key material | Not implemented | Must be cleared on every return path | `DEFERRED_TO_INTEGRITY_IMPLEMENTATION` |

## 6. Handle state and purpose rules

- Implemented import purposes are exactly `HMAC`, `BLOCK_CIPHER`, and `CMAC`.
- HMAC handles cannot initialize AES/CMAC operations; AES and CMAC purposes are separate.
- `EXPORTABLE_CANDIDATE` is rejected.
- Unknown flags and unsupported key types fail closed.
- Live states are internal allocation/initialization/active/restricted states. The current
  import path reaches `ACTIVE` directly after copying.
- Explicit destroy clears key bytes and metadata, then sets `DESTROYED`.
- P2-05 shutdown or fatal transition clears live slot bytes and metadata, then sets
  `ZEROIZED` while preserving generation for a final explicit destroy.
- `ZEROIZED` is invalid for cryptographic use and is available for slot reuse.
- Reuse assigns a new generation, making the old handle `INVALID`.

## 7. Out-of-scope key-management functions

The initial candidate does not implement:

- key export, serialization, or backup;
- key wrapping/unwrapping;
- persistent key storage or database records;
- filesystem, TPM, HSM, secure-element, OS keychain, or cloud-KMS integration;
- inter-process handle sharing;
- key derivation or public-key/private-key management;
- multi-tenant policy, access-control lists, rotation, escrow, or recovery.

These functions must not be inferred from reserved key-type or flag names.

## 8. Open classification and evidence questions

1. Are caller-owned HMAC/AES/CMAC/DRBG opaque contexts classified as CSP storage for the
   selected validation target, and what operator documentation is required?
2. Is the P2-05 `ZEROIZED` tombstone model acceptable, or must shutdown move directly to
   `DESTROYED` or remove query visibility?
3. Is module-owned table wiping on fatal transition sufficient when the global lock itself
   cannot be acquired, given that recovery is process termination and fresh load?
4. What exact optimized compiler-output, debugger/memory observation, or laboratory test
   is expected to support zeroization claims on OE-01?
5. Must stack/register spill, allocator reuse, swap, core dump, hibernation, and OS crash
   policy be included in the module evidence or the operating-environment controls?

All five remain `EXTERNAL_CONFIRMATION_REQUIRED`.
