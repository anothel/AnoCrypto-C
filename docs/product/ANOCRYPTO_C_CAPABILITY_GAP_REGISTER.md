# AnoCrypto-C Capability Gap Register

Status: `P3_10_ALL_APPROVED_CORE_GAPS_SATISFIED_ADAPTER_PARITY_NEXT`

Retained P3-01/P3-02 policy token: `8_GAPS_APPROVED_HKDF_SATISFIED`.

## Current baseline

The core exposes 18 headers and 85 symbols. ABI-0 through ABI-6 retain their
previous assignments; restricted OCSP CertID SHA-1 is the only name on
`ANOCRYPTO_C_7`.

| Gap ID | Requirement | Current state |
|---|---|---|
| `AC-GAP-001` | RFC 5869 HKDF-SHA-256 | `SATISFIED_P3_02` |
| `AC-GAP-002` | AES-256-GCM one-shot | `SATISFIED_P3_03` |
| `AC-GAP-003` | AES-256-GCM streaming | `SATISFIED_P3_04` |
| `AC-GAP-004` | fixed-profile scrypt | `SATISFIED_P3_05` |
| `AC-GAP-005` | restricted SHA-1 for OCSP CertID | `SATISFIED_P3_10_CORE_ADAPTER_PENDING` |
| `AC-GAP-006` | RSA PKCS#1 v1.5 SHA-256/384/512 | `SATISFIED_P3_08_CORE_ADAPTER_PENDING` |
| `AC-GAP-007` | ECDSA P-256 SHA-256 | `SATISFIED_P3_09_CORE_ADAPTER_PENDING` |
| `AC-GAP-008` | public-key handle/import/metadata/binding | `SATISFIED_SOFTWARE_CORE_EXTERNAL_PROVIDER_PENDING` |

No additional algorithm is selected. External-provider handles, persistence,
export, wrapping, routing, evidence, and audit remain outside AnoCrypto-C.
