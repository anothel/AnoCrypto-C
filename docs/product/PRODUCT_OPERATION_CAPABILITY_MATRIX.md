# Product Operation to Capability Matrix

Status: `P3_01_REQUIREMENTS_ACCEPTED`

| ID | Product operation/contract | Evidence | Candidate capability | Current AnoCrypto-C result | Owner | Decision |
|---|---|---|---|---|---|---|
| `ASK-001` | public SHA-256 and format hashing | Community `src/hash.cpp`, provider interface/tests | SHA-256 | supported | AnoCrypto-C primitive | `SATISFIED` |
| `ASK-002` | public HMAC-SHA-256 | Community provider contract and tests | HMAC-SHA-256 | supported | AnoCrypto-C primitive | `SATISFIED` |
| `ASK-003` | random bytes and generated 256-bit key | Community `src/random.cpp`; Enterprise native adapter | random service | supported | AnoCrypto-C primitive + Enterprise adapter | `SATISFIED` |
| `ASK-004` | public HKDF and `SKF1` v1 file-key derivation | `src/file_crypto.cpp`, exact `SKF1` fixture, RFC 5869 CLI test | HKDF-SHA-256 | missing | AnoCrypto-C | `ACCEPTED_GAP_SELECTED_P3_02` |
| `ASK-005` | `SKF1`/`SKP1` chunk encryption | one-shot provider calls and v1 fixtures | AES-256-GCM one-shot | missing | AnoCrypto-C | `ACCEPTED_GAP` |
| `ASK-006` | `SKT1` packet streaming and public packet API | `src/packet_stream.cpp`, split-update/fixture tests | AES-256-GCM streaming | missing | AnoCrypto-C | `ACCEPTED_GAP` |
| `ASK-007` | `SKP1` v1 password derivation | fixed v1 format and negative fixtures | scrypt `N=32768,r=8,p=1`, 32-byte output | missing | AnoCrypto-C | `ACCEPTED_GAP` |
| `ASK-008` | exact SDK/archive identity | Enterprise package gates | package digest/provenance | partial engineering support; approved artifact absent | Enterprise/release lane | `NOT_A_PRIMITIVE_GAP` |
| `ASK-009` | explicit capability mask and no fallback | Enterprise runtime contract/tests | truthful profile capability reporting | Enterprise already owns mask; no runtime fallback | Enterprise | `ADAPTER_OWNED` |
| `PKI-001` | backend identity/readiness/self-test | Community backend metadata, Enterprise SDK manifest | module version/build/state | supported | AnoCrypto-C + adapter | `SATISFIED_PENDING_REAL_INTEGRATION` |
| `PKI-002` | certificate-signature digest variants | algorithm policy and OpenSSL paths | SHA-256/384/512 | supported | AnoCrypto-C | `SATISFIED` |
| `PKI-003` | serial/key randomness when selected owner requires it | issue contract | secure random | supported | owner decided per operation | `SATISFIED` |
| `PKI-004` | CSR/certificate/CRL/OCSP parsing and encoding | backend-neutral operations and OpenSSL adapters | ASN.1/PEM/DER/PKI codec | deliberately outside core | AnoPKI | `PRODUCT_OWNED_NO_CORE_GAP` |
| `PKI-005` | OCSP CertID interoperability | issuer-inspection fixture/policy | restricted SHA-1 digest | missing | AnoCrypto-C candidate | `ACCEPTED_CONDITIONAL_GAP` |
| `PKI-006` | CSR/certificate/CRL/OCSP RSA signatures | positive RSA fixtures and key-provider path | RSA PKCS#1 v1.5 SHA-256/384/512 sign/verify | missing | AnoCrypto-C | `ACCEPTED_GAP_AFTER_KEY_GATE` |
| `PKI-007` | approved ECDSA path | ECDSA P-256 policy/tests | ECDSA P-256 SHA-256 sign/verify | missing | AnoCrypto-C | `ACCEPTED_GAP_AFTER_KEY_GATE` |
| `PKI-008` | key metadata, non-exportable handle use and binding | FileKeyProvider contract and binding tests | public-key handle/import/metadata/binding | missing | AnoCrypto-C primitive + Enterprise provider | `ACCEPTED_DESIGN_GAP` |
| `PKI-009` | CSR proof-of-possession | CSR inspect contract and invalid-signature tests | codec plus signature verify | primitive portion covered by `PKI-006/007` | AnoPKI + AnoCrypto-C | `SPLIT_OWNERSHIP` |
| `PKI-010` | provider routing, signing evidence and retry policy | ADR 0007 and Enterprise product gates | operation orchestration/evidence | outside module | AnoPKI Enterprise | `ENTERPRISE_OWNED` |
| `PKI-011` | Ed25519, RSA-PSS and PQC | no current positive parity requirement | additional public-key algorithms | not assessed | future decision | `DEFERRED_NOT_REQUIRED` |

## Acceptance count

- accepted product/contract rows: 20;
- satisfied or non-core-owned rows: 12;
- accepted AnoCrypto-C gaps: 8;
- selected implementation: 1 (`ASK-004`).
