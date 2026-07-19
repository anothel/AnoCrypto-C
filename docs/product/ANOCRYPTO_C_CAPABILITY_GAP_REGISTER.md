# AnoCrypto-C Capability Gap Register

Status: `8_GAPS_APPROVED_FOUR_CORE_CAPABILITIES_SATISFIED_P3_07_GATE_READY`

Retained P3-01/P3-02 policy token: `8_GAPS_APPROVED_HKDF_SATISFIED`.

## Current baseline

The current core source exposes 16 headers and 80 symbols. All 69 retained
symbols remain on `ANOCRYPTO_C_0`; HKDF remains on `ANOCRYPTO_C_1`; two one-shot
AES-256-GCM symbols remain on `ANOCRYPTO_C_2`; seven streaming symbols remain on
`ANOCRYPTO_C_3`; fixed-profile scrypt remains on `ANOCRYPTO_C_4`.

| Gap ID | Requirement authority | Minimal capability | API/ABI expectation | Current state |
|---|---|---|---|---|
| `AC-GAP-001` | `ASK-004` | RFC 5869 HKDF-SHA-256, 0..8160 output, exact empty-input semantics | `ac_kdf.h`; additive `ANOCRYPTO_C_1` | `SATISFIED_P3_02` |
| `AC-GAP-002` | `ASK-005` | AES-256-GCM one-shot, 12-byte nonce, 16-byte tag, arbitrary AAD, authenticated plaintext release | `ac_aead.h`; additive `ANOCRYPTO_C_2` | `SATISFIED_P3_03` |
| `AC-GAP-003` | `ASK-006` | AES-256-GCM incremental encrypt/decrypt with explicit states | caller-owned opaque context; additive `ANOCRYPTO_C_3` | `SATISFIED_P3_04` |
| `AC-GAP-004` | `ASK-007` | scrypt fixed v1 profile with overflow/resource rejection | `ac_kdf.h`; caller workspace; additive `ANOCRYPTO_C_4` | `SATISFIED_P3_05` |
| `AC-GAP-005` | `PKI-005` | SHA-1 digest only for OCSP CertID interoperability | restricted algorithm/API policy required | `CONDITIONAL_DEFERRED_RECONFIRM_REQUIRED` |
| `AC-GAP-006` | `PKI-006` | RSA PKCS#1 v1.5 SHA-256/384/512 sign/verify | public-key API and later additive ABI | `BLOCKED_BY_P3_07_KEY_GATE` |
| `AC-GAP-007` | `PKI-007` | ECDSA P-256 SHA-256 sign/verify with explicit signature representation | public-key API and later additive ABI | `BLOCKED_BY_P3_07_KEY_GATE` |
| `AC-GAP-008` | `PKI-008` | key handle/import, public metadata and key/public-material binding | key/CSP architecture gate; no public addition in gate | `ACTIVE_P3_07_DESIGN_GATE` |

## P3-07 authorization boundary

P3-07 is a design and contract gate only. It may review alternatives, write
architecture decisions, define candidate API contracts, and select at most one
subsequent implementation slice. It may not add a public header, function,
type, enum value, symbol, or ABI line. `ANOCRYPTO_C_5` is not reserved.

## Explicitly not gaps

- AnoSecureKit formats and file/packet orchestration;
- PKI ASN.1/X.509/CSR/CRL/OCSP codecs;
- Enterprise capability masks, release authorization and artifact approval;
- HSM/KMS/PKCS#11 routing, key-reference syntax, audit and retries;
- Ed25519, RSA-PSS and PQC without new positive product evidence.
