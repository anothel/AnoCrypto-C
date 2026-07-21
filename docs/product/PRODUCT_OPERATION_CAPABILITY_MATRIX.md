# Product Operation to Capability Matrix

Status: `P3_08_RSA_CORE_COMPLETE_P3_09_ECDSA_NEXT`

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
| `PKI-005` | OCSP CertID | restricted SHA-1 | reconfirmed; deferred gate | split |
| `PKI-006` | RSA CSR/cert/CRL/OCSP signatures | RSA PKCS#1 v1.5 SHA-2 | `SATISFIED_P3_08_CORE_ADAPTER_PENDING` | core + AnoPKI adapter |
| `PKI-007` | approved ECDSA path | P-256 SHA-256 | absent; selected next | `P3_09` |
| `PKI-008` | key handle/import/binding | public-key CSP | RSA portion complete; P-256/external providers pending | split |
| `PKI-009` | CSR proof-of-possession | codec + verify | RSA primitive complete; adapter pending | AnoPKI + core |
| `PKI-010` | routing/evidence/retry | orchestration | outside core | Enterprise |
| `PKI-011` | RSA-PSS/Ed25519/PQC | extra algorithms | not approved | deferred |

accepted product/contract rows: 20. Approved gaps: 8. Completed core capability gaps: 5.
Next implementation gate: P3-09 ECDSA P-256 SHA-256.
