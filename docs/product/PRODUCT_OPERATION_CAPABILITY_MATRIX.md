# Product Operation to Capability Matrix

Status: `P3_06_PARITY_COMPLETE_P3_07_GATE_READY`

| ID | Product operation/contract | Evidence | Candidate capability | Current result | Owner | Decision |
|---|---|---|---|---|---|---|
| `ASK-001` | public SHA-256 and format hashing | Community `src/hash.cpp`, provider interface/tests | SHA-256 | supported and connected | AnoCrypto-C primitive + Enterprise adapter | `SATISFIED` |
| `ASK-002` | public HMAC-SHA-256 | Community provider contract and tests | HMAC-SHA-256 | supported and connected | AnoCrypto-C primitive + Enterprise adapter | `SATISFIED` |
| `ASK-003` | random bytes and generated 256-bit key | Community `src/random.cpp`; Enterprise native adapter | random service | supported and connected | AnoCrypto-C primitive + Enterprise adapter | `SATISFIED` |
| `ASK-004` | public HKDF and `SKF1` v1 file-key derivation | RFC 5869 and exact `SKF1` fixture | HKDF-SHA-256 | supported in P3-02 and connected in P3-06 | AnoCrypto-C + Enterprise adapter | `SATISFIED_P3_02_P3_06` |
| `ASK-005` | `SKF1`/`SKP1` chunk encryption | one-shot provider calls and v1 fixtures | AES-256-GCM one-shot | supported in P3-03 and connected in P3-06 | AnoCrypto-C + Enterprise adapter | `SATISFIED_P3_03_P3_06` |
| `ASK-006` | `SKT1` packet streaming and public packet API | split-update and exact fixture tests | AES-256-GCM streaming | supported in P3-04 and connected in P3-06 | AnoCrypto-C + Enterprise adapter | `SATISFIED_P3_04_P3_06` |
| `ASK-007` | `SKP1` v1 password derivation | fixed v1 format and negative/resource fixtures | scrypt `N=32768,r=8,p=1`, 32-byte output | supported in P3-05 and connected in P3-06 | AnoCrypto-C + Enterprise adapter | `SATISFIED_P3_05_P3_06` |
| `ASK-008` | exact SDK/archive identity | P3-06 package gates and source comparison | package digest/provenance | content reconciled; upstream Git-tree reproduction and release provenance acceptance remain open | Enterprise/release lane | `CONTENT_RECONCILED_PROVENANCE_OPEN` |
| `ASK-009` | explicit capability mask and no fallback | P3-06 runtime contract, parity tests, Linux dependency inspection | truthful profile capability reporting | seven capabilities reported; no OpenSSL fallback | Enterprise | `SATISFIED_P3_06_ENGINEERING_CHECKPOINT` |
| `PKI-001` | backend identity/readiness/self-test | Community backend metadata, Enterprise SDK manifest | module version/build/state | core supported; AnoPKI adapter integration pending | AnoCrypto-C + adapter | `SATISFIED_CORE_PENDING_ANOPKI_INTEGRATION` |
| `PKI-002` | certificate-signature digest variants | algorithm policy and OpenSSL paths | SHA-256/384/512 | supported | AnoCrypto-C | `SATISFIED` |
| `PKI-003` | serial/key randomness when selected owner requires it | issue contract | secure random | supported | operation owner chooses | `SATISFIED` |
| `PKI-004` | CSR/certificate/CRL/OCSP parsing and encoding | backend-neutral operations and OpenSSL adapters | ASN.1/PEM/DER/PKI codec | deliberately outside core | AnoPKI | `PRODUCT_OWNED_NO_CORE_GAP` |
| `PKI-005` | OCSP CertID interoperability | issuer-inspection fixture/policy | restricted SHA-1 digest | not implemented; current requirement must be reconfirmed | AnoCrypto-C candidate | `CONDITIONAL_DEFERRED` |
| `PKI-006` | CSR/certificate/CRL/OCSP RSA signatures | positive RSA fixtures and key-provider path | RSA PKCS#1 v1.5 SHA-256/384/512 sign/verify | not implemented | AnoCrypto-C | `BLOCKED_BY_P3_07_KEY_GATE` |
| `PKI-007` | approved ECDSA path | ECDSA P-256 policy/tests | ECDSA P-256 SHA-256 sign/verify | not implemented | AnoCrypto-C | `BLOCKED_BY_P3_07_KEY_GATE` |
| `PKI-008` | key metadata, non-exportable handle use and binding | FileKeyProvider contract and binding tests | public-key handle/import/metadata/binding | design not frozen | AnoCrypto-C primitive + Enterprise provider | `ACTIVE_P3_07_DESIGN_GATE` |
| `PKI-009` | CSR proof-of-possession | CSR inspect contract and invalid-signature tests | codec plus signature verify | primitive portion depends on `PKI-006/007` | AnoPKI + AnoCrypto-C | `SPLIT_OWNERSHIP` |
| `PKI-010` | provider routing, signing evidence and retry policy | ADR and Enterprise product gates | operation orchestration/evidence | outside module | AnoPKI Enterprise | `ENTERPRISE_OWNED` |
| `PKI-011` | Ed25519, RSA-PSS and PQC | no current positive parity requirement | additional public-key algorithms | not approved | future decision | `DEFERRED_NOT_REQUIRED` |

## Acceptance count

- accepted product/contract rows: 20;
- approved AnoCrypto-C gaps: 8;
- completed core capability gaps: 4 (`ASK-004` through `ASK-007`);
- completed downstream parity checkpoints: 1 (`P3-06`);
- remaining public-key/design gaps: 4 (`PKI-005` through `PKI-008`);
- active next gate: `P3-07` public-key/key-handle architecture and AnoPKI contract gate;
- selected next algorithm implementation: `NOT_SELECTED`.
