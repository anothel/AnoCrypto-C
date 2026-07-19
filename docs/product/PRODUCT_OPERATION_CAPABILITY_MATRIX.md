# Product Operation to Capability Matrix

Status: `P3_07_COMPLETE_RSA_P3_08_SELECTED`

| ID | Product operation/contract | Evidence | Candidate capability | Current result | Owner | Decision |
|---|---|---|---|---|---|---|
| `ASK-001` | public SHA-256 and format hashing | Community provider/tests | SHA-256 | supported and connected | AnoCrypto-C + Enterprise adapter | `SATISFIED` |
| `ASK-002` | public HMAC-SHA-256 | Community provider/tests | HMAC-SHA-256 | supported and connected | AnoCrypto-C + Enterprise adapter | `SATISFIED` |
| `ASK-003` | random bytes and generated key | Community random path; Enterprise adapter | random service | supported and connected | AnoCrypto-C + Enterprise adapter | `SATISFIED` |
| `ASK-004` | public HKDF and `SKF1` file-key derivation | RFC 5869 and exact fixture | HKDF-SHA-256 | P3-02/P3-06 complete | AnoCrypto-C + Enterprise adapter | `SATISFIED_P3_02_P3_06` |
| `ASK-005` | `SKF1`/`SKP1` chunk encryption | one-shot and exact fixtures | AES-256-GCM one-shot | P3-03/P3-06 complete | AnoCrypto-C + Enterprise adapter | `SATISFIED_P3_03_P3_06` |
| `ASK-006` | `SKT1` packet streaming | split-update and exact fixture | AES-256-GCM streaming | P3-04/P3-06 complete | AnoCrypto-C + Enterprise adapter | `SATISFIED_P3_04_P3_06` |
| `ASK-007` | `SKP1` password derivation | fixed profile/resource fixtures | fixed-profile scrypt | P3-05/P3-06 complete | AnoCrypto-C + Enterprise adapter | `SATISFIED_P3_05_P3_06` |
| `ASK-008` | exact SDK/archive identity | P3-06 package/source checks | provenance | content reconciled; provenance open | Enterprise/release | `CONTENT_RECONCILED_PROVENANCE_OPEN` |
| `ASK-009` | explicit mask and no fallback | P3-06 runtime and dependency checks | profile reporting | seven capabilities; no fallback | Enterprise | `SATISFIED_P3_06_ENGINEERING_CHECKPOINT` |
| `PKI-001` | backend identity/readiness/self-test | Community/Enterprise contracts | module identity/state | core supported; adapter integration pending | AnoCrypto-C + adapter | `SATISFIED_CORE_PENDING_ANOPKI_INTEGRATION` |
| `PKI-002` | certificate-signature digest variants | exact issue/provider source | SHA-256/384/512 | supported hash core | AnoCrypto-C | `SATISFIED` |
| `PKI-003` | serial/key randomness | issue path | secure random | supported | operation owner chooses | `SATISFIED` |
| `PKI-004` | CSR/certificate/CRL/OCSP codecs | exact core/OpenSSL paths | ASN.1/PEM/DER/PKI codec | outside core | AnoPKI | `PRODUCT_OWNED_NO_CORE_GAP` |
| `PKI-005` | OCSP CertID interoperability | default normalization and SHA-1 request fixture | restricted SHA-1 digest | exact need reconfirmed; API containment not designed | AnoCrypto-C candidate | `RECONFIRMED_RESTRICTED_DEFERRED` |
| `PKI-006` | CSR/certificate/CRL/OCSP RSA signatures | RSA-2048 positive and negative provider fixtures | RSA PKCS#1 v1.5 SHA-256/384/512 | architecture approved; implementation absent | AnoCrypto-C | `SELECTED_P3_08` |
| `PKI-007` | approved ECDSA path | P-256 certificate/CRL/OCSP success fixtures | ECDSA P-256 SHA-256 | architecture approved; implementation absent | AnoCrypto-C | `REQUIRED_DEFERRED_AFTER_RSA` |
| `PKI-008` | key metadata/handle/binding | move-only provider handle, algorithm/binding negatives | public-key handle/import/binding | common tagged handle and import model approved | AnoCrypto-C + Enterprise provider | `ARCHITECTURE_APPROVED_P3_07` |
| `PKI-009` | CSR proof-of-possession | exact issue-time verify path | codec + signature verify | split; RSA verify selected next | AnoPKI + AnoCrypto-C | `SPLIT_SELECTED_P3_08_RSA` |
| `PKI-010` | provider routing/evidence/retry | Enterprise/Community provider contracts | orchestration | outside module | AnoPKI Enterprise | `ENTERPRISE_OWNED` |
| `PKI-011` | Ed25519/RSA-PSS/PQC | no approved AnoCrypto-C parity requirement | extra algorithms | not approved | future decision | `DEFERRED_NOT_REQUIRED` |

## Acceptance count

- accepted product/contract rows: 20;
- approved AnoCrypto-C gaps: 8;
- completed core capability gaps: 4;
- completed downstream parity checkpoints: 1;
- public-key architecture gates completed: 1 (`P3-07`);
- reconfirmed but deferred gap: 1 (restricted SHA-1);
- selected next implementation slice: 1 (`P3-08` RSA);
- required but not selected implementation: 1 (P-256 ECDSA).
