# AnoCrypto-C Capability Gap Register

Status: `8_GAPS_APPROVED_HKDF_SATISFIED`

## Retained baseline

The current source exposes 15 headers and 70 symbols. All 69 retained symbols remain on
`ANOCRYPTO_C_0`; the single HKDF symbol is additive on `ANOCRYPTO_C_1`.

| Gap ID | Requirement authority | Minimal capability | API/ABI expectation | Priority/state |
|---|---|---|---|---|
| `AC-GAP-001` | `ASK-004` | RFC 5869 HKDF-SHA-256, 0..8160 output, exact empty-input semantics | new `ac_kdf.h`; additive `ANOCRYPTO_C_1` symbols | `SATISFIED_P3_02` |
| `AC-GAP-002` | `ASK-005` | AES-256-GCM one-shot, 12-byte nonce, 16-byte tag, arbitrary AAD, no unauthenticated plaintext release | new AEAD API, additive ABI | `APPROVED_AFTER_P3_02` |
| `AC-GAP-003` | `ASK-006` | AES-256-GCM incremental encrypt/decrypt with explicit states | opaque context API, additive ABI | `APPROVED_AFTER_ONE_SHOT` |
| `AC-GAP-004` | `ASK-007` | scrypt fixed v1 profile with overflow/resource rejection | KDF API design required | `APPROVED_DESIGN_PENDING` |
| `AC-GAP-005` | `PKI-005` | SHA-1 digest only for OCSP CertID interoperability | restricted algorithm/API policy required | `CONDITIONAL_DEFERRED` |
| `AC-GAP-006` | `PKI-006` | RSA PKCS#1 v1.5 SHA-256/384/512 sign/verify | public-key API and new ABI line | `APPROVED_AFTER_KEY_GATE` |
| `AC-GAP-007` | `PKI-007` | ECDSA P-256 SHA-256 sign/verify with explicit signature encoding | public-key API and new ABI line | `APPROVED_AFTER_KEY_GATE` |
| `AC-GAP-008` | `PKI-008` | key handle/import, public metadata and key/public-material binding | key/CSP architecture gate | `APPROVED_DESIGN_GAP` |

## Explicitly not gaps

- AnoSecureKit formats and file/packet orchestration;
- PKI ASN.1/X.509/CSR/CRL/OCSP codecs;
- Enterprise capability masks, release authorization and artifact approval;
- HSM/KMS routing, key reference syntax, audit and retries;
- Ed25519, RSA-PSS and PQC without new positive product evidence.
