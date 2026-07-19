# AnoCrypto-C Capability Gap Register

Status: `P3_07_ARCHITECTURE_APPROVED_RSA_P3_08_SELECTED`

Retained P3-01/P3-02 policy token: `8_GAPS_APPROVED_HKDF_SATISFIED`.

## Current baseline

The current core source exposes 16 headers and 80 symbols. All 69 retained
symbols remain on `ANOCRYPTO_C_0`; HKDF remains on `ANOCRYPTO_C_1`; two one-shot
AES-256-GCM symbols remain on `ANOCRYPTO_C_2`; seven streaming symbols remain on
`ANOCRYPTO_C_3`; fixed-profile scrypt remains on `ANOCRYPTO_C_4`.

| Gap ID | Requirement authority | Minimal capability | P3-07 result | Current state |
|---|---|---|---|---|
| `AC-GAP-001` | `ASK-004` | RFC 5869 HKDF-SHA-256 | unchanged | `SATISFIED_P3_02` |
| `AC-GAP-002` | `ASK-005` | AES-256-GCM one-shot | unchanged | `SATISFIED_P3_03` |
| `AC-GAP-003` | `ASK-006` | AES-256-GCM streaming | unchanged | `SATISFIED_P3_04` |
| `AC-GAP-004` | `ASK-007` | fixed-profile scrypt | unchanged | `SATISFIED_P3_05` |
| `AC-GAP-005` | `PKI-005` | SHA-1 digest only for OCSP CertID interoperability | exact source and fixture reconfirm default SHA-1; no issuance/signature use approved | `RECONFIRMED_RESTRICTED_DEFERRED_API_GATE_REQUIRED` |
| `AC-GAP-006` | `PKI-006` | RSA PKCS#1 v1.5 SHA-256/384/512 sign/verify | architecture approved; selected as the single next slice | `SELECTED_P3_08_API_ABI_GATE_REQUIRED` |
| `AC-GAP-007` | `PKI-007` | ECDSA P-256 SHA-256 sign/verify | handle/import/signature boundary approved; implementation not selected | `ARCHITECTURE_APPROVED_DEFERRED_AFTER_RSA` |
| `AC-GAP-008` | `PKI-008` | key handle/import/metadata/binding | common tagged handle and normalized import model approved | `ARCHITECTURE_APPROVED_P3_07_IMPLEMENT_WITH_P3_08` |

## P3-08 selected scope

P3-08 may review and, only after that review passes, implement:

- one RSA public/private component import function using the common CSP handle;
- generic message-based signature sign and verify functions;
- RSA PKCS#1 v1.5 with SHA-256/384/512;
- bounded asymmetric key storage, lifecycle, zeroization, and concurrency leases;
- one additive ABI line only after exact names and symbol count are approved.

The candidate budget is three new symbols. It is a target, not pre-approved ABI.
P3-07 does not add or reserve `ANOCRYPTO_C_5`.

## Explicitly not gaps

- AnoSecureKit formats and orchestration;
- PKI ASN.1/X.509/CSR/CRL/OCSP codecs;
- Enterprise capability masks, provider routing, release authorization, and
  artifact approval;
- HSM/KMS/PKCS#11, `key_ref`, audit, and retries;
- key persistence/export/wrapping;
- Ed25519, RSA-PSS, ECDH, and PQC without new positive evidence.
