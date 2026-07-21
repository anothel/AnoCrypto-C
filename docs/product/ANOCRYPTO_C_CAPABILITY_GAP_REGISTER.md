# AnoCrypto-C Capability Gap Register

Status: `P3_08_RSA_SATISFIED_P3_09_ECDSA_NEXT`

Retained P3-01/P3-02 policy token: `8_GAPS_APPROVED_HKDF_SATISFIED`.

## Current baseline

The core exposes 17 headers and 83 symbols. ABI-0 through ABI-4 retain their
previous assignments; the RSA import/sign/verify symbols are the only names on
`ANOCRYPTO_C_5`.

| Gap ID | Requirement | Current state |
|---|---|---|
| `AC-GAP-001` | RFC 5869 HKDF-SHA-256 | `SATISFIED_P3_02` |
| `AC-GAP-002` | AES-256-GCM one-shot | `SATISFIED_P3_03` |
| `AC-GAP-003` | AES-256-GCM streaming | `SATISFIED_P3_04` |
| `AC-GAP-004` | fixed-profile scrypt | `SATISFIED_P3_05` |
| `AC-GAP-005` | restricted SHA-1 for OCSP CertID | `RECONFIRMED_DEFERRED_SEPARATE_GATE` |
| `AC-GAP-006` | RSA PKCS#1 v1.5 SHA-256/384/512 | `SATISFIED_P3_08_CORE_ADAPTER_PENDING` |
| `AC-GAP-007` | ECDSA P-256 SHA-256 | `SELECTED_P3_09_API_ABI_GATE_REQUIRED` |
| `AC-GAP-008` | public-key handle/import/metadata/binding | `PARTIALLY_SATISFIED_P3_07_P3_08_RSA` |

P3-08 implements only RSA-2048/e=65537 algorithm-native component import and
message sign/verify. P-256 import and ECDSA remain absent. External-provider
handles, persistence, export, wrapping, routing, evidence, and audit remain
outside AnoCrypto-C.
