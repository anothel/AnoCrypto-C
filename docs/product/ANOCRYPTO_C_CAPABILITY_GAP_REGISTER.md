# AnoCrypto-C Capability Gap Register

Status: `PHASE_1_APPROVED_GAPS_SATISFIED`

## Retained P3-01/P3-02 policy compatibility

Historical token: `8_GAPS_APPROVED_HKDF_SATISFIED`.

## Frozen core surface

- version: `0.1.0-p3-10-restricted-sha1-ocsp-certid`;
- source commit: `98f544a574f70b387c300f171243b4e87174937a`;
- public headers: 18;
- exported symbols: 85;
- ABI lines: `ANOCRYPTO_C_0` through `ANOCRYPTO_C_7`.

| Gap ID | Requirement | Phase 1 state |
|---|---|---|
| `AC-GAP-001` | RFC 5869 HKDF-SHA-256 | `SATISFIED_P3_02_P3_06` |
| `AC-GAP-002` | AES-256-GCM one-shot | `SATISFIED_P3_03_P3_06` |
| `AC-GAP-003` | AES-256-GCM streaming | `SATISFIED_P3_04_P3_06` |
| `AC-GAP-004` | fixed-profile scrypt | `SATISFIED_P3_05_P3_06` |
| `AC-GAP-005` | restricted SHA-1 for OCSP CertID | `SATISFIED_P3_10_P3_11` |
| `AC-GAP-006` | RSA PKCS#1 v1.5 SHA-256/384/512 | `SATISFIED_P3_08_P3_11` |
| `AC-GAP-007` | ECDSA P-256 SHA-256 | `SATISFIED_P3_09_P3_11` |
| `AC-GAP-008` | public-key handle/import/metadata/binding | `SATISFIED_LOCAL_SOFTWARE_PATH` |

## Non-gap deferred work

The following are not active AnoCrypto-C gaps and were not required to close
Phase 1:

- production HSM/KMS/PKCS#11 and external-provider lifecycle;
- key persistence/export/wrapping;
- hosted qualification;
- trusted provenance acceptance;
- production signing/timestamping;
- customer distribution and commercialization;
- KCMVP validation;
- additional algorithms without new product evidence.

Active gap: `NONE`.
