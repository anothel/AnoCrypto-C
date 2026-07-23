# Product Operation to Capability Matrix

Status: `PHASE_1_LOCAL_INTEGRATION_COMPLETE`

## Retained P3-01 policy compatibility

Historical token: `accepted product/contract rows: 20`.

| ID | Product operation/contract | Capability | Phase 1 result | Owner/limit |
|---|---|---|---|---|
| `ASK-001` | hashing | SHA-256 | satisfied and connected | core + adapter |
| `ASK-002` | HMAC | HMAC-SHA-256 | satisfied and connected | core + adapter |
| `ASK-003` | random/key generation | random service | satisfied and connected | core + adapter |
| `ASK-004` | HKDF/`SKF1` derivation | HKDF-SHA-256 | `SATISFIED_P3_02_P3_06` | core + adapter |
| `ASK-005` | `SKF1`/`SKP1` encryption | GCM one-shot | `SATISFIED_P3_03_P3_06` | core + adapter |
| `ASK-006` | `SKT1` streaming | GCM streaming | `SATISFIED_P3_04_P3_06` | core + adapter |
| `ASK-007` | `SKP1` password derivation | fixed scrypt | `SATISFIED_P3_05_P3_06` | core + adapter |
| `ASK-008` | SDK/archive identity | provenance | content reconciled; trusted provenance remains open | release lane |
| `ASK-009` | capability mask/no fallback | profile reporting | `SATISFIED_P3_06_LOCAL` | Enterprise |
| `PKI-001` | backend readiness | module identity/state | `SATISFIED_P3_11_LOCAL` | core + adapter |
| `PKI-002` | signature digests | SHA-256/384/512 | satisfied | core |
| `PKI-003` | serial/key randomness | random | satisfied | operation owner |
| `PKI-004` | PKI codecs | ASN.1/PEM/DER | product-owned and locally exercised | AnoPKI |
| `PKI-005` | OCSP CertID issuer hashes | restricted SHA-1 | `SATISFIED_P3_10_P3_11_LOCAL` | core + adapter |
| `PKI-006` | RSA CSR/cert/CRL/OCSP signatures | RSA PKCS#1 v1.5 SHA-2 | `SATISFIED_P3_08_P3_11_LOCAL` | core + adapter |
| `PKI-007` | approved ECDSA path | P-256 SHA-256 | `SATISFIED_P3_09_P3_11_LOCAL` | core + adapter |
| `PKI-008` | key handle/import/binding | public-key CSP | local software path satisfied; production providers deferred | split |
| `PKI-009` | CSR proof-of-possession | codec + verify | `SATISFIED_P3_11_LOCAL` | AnoPKI + core |
| `PKI-010` | routing/evidence/retry | orchestration | local file-provider/evidence boundary verified; production provider deferred | Enterprise |
| `PKI-011` | RSA-PSS/Ed25519/PQC | extra algorithms | not approved | deferred |

Accepted product/contract rows: 20. Approved AnoCrypto-C gaps: 8. All eight
approved software-core/design gaps are satisfied. The selected AnoSecureKit and
AnoPKI Enterprise paths were locally integrated and verified without fallback.
No additional implementation is selected.
