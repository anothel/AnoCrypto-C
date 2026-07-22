# Product Operation to Capability Matrix

Status: `P3_10_ALL_APPROVED_CORE_GAPS_SATISFIED_P3_11_PARITY_NEXT`

| ID | Product operation/contract | Capability | Current result | Owner/decision |
|---|---|---|---|---|
| `ASK-001` | hashing | SHA-256 | satisfied | core + adapter |
| `ASK-002` | HMAC | HMAC-SHA-256 | satisfied | core + adapter |
| `ASK-003` | random/key generation | random service | satisfied | core + adapter |
| `ASK-004` | HKDF/`SKF1` derivation | HKDF-SHA-256 | `SATISFIED_P3_02_P3_06` | core + adapter |
| `ASK-005` | `SKF1`/`SKP1` encryption | GCM one-shot | `SATISFIED_P3_03_P3_06` | core + adapter |
| `ASK-006` | `SKT1` streaming | GCM streaming | `SATISFIED_P3_04_P3_06` | core + adapter |
| `ASK-007` | `SKP1` password derivation | fixed scrypt | `SATISFIED_P3_05_P3_06` | core + adapter |
| `ASK-008` | SDK/archive identity | provenance | content reconciled; provenance open | Enterprise/release |
| `ASK-009` | capability mask/no fallback | profile reporting | `SATISFIED_P3_06_ENGINEERING_CHECKPOINT` | Enterprise |
| `PKI-001` | backend readiness | module identity/state | core satisfied; adapter pending | core + adapter |
| `PKI-002` | signature digests | SHA-256/384/512 | satisfied | core |
| `PKI-003` | serial/key randomness | random | satisfied | operation owner |
| `PKI-004` | PKI codecs | ASN.1/PEM/DER | outside core | AnoPKI |
| `PKI-005` | OCSP CertID issuer-name/key hashes | restricted SHA-1 | `SATISFIED_P3_10_CORE_ADAPTER_PENDING` | core + AnoPKI adapter |
| `PKI-006` | RSA CSR/cert/CRL/OCSP signatures | RSA PKCS#1 v1.5 SHA-2 | `SATISFIED_P3_08_CORE_ADAPTER_PENDING` | core + AnoPKI adapter |
| `PKI-007` | approved ECDSA path | P-256 SHA-256 | `SATISFIED_P3_09_CORE_ADAPTER_PENDING` | core + AnoPKI adapter |
| `PKI-008` | key handle/import/binding | public-key CSP | software RSA/P-256 complete; external providers pending | split |
| `PKI-009` | CSR proof-of-possession | codec + verify | primitives complete; adapter pending | AnoPKI + core |
| `PKI-010` | routing/evidence/retry | orchestration | outside core | Enterprise |
| `PKI-011` | RSA-PSS/Ed25519/PQC | extra algorithms | not approved | deferred |

Accepted product/contract rows: 20. Approved AnoCrypto-C gaps: 8. All eight
approved core/design gaps are now satisfied at the software-core level. The
next work is downstream AnoPKI Enterprise parity, not another primitive.

Retained P3-01 policy token: accepted product/contract rows: 20.
