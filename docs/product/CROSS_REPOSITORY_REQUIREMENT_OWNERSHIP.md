# Cross-Repository Requirement Ownership

Status: `P3_07_ENTRY_BOUNDARIES_APPROVED`

## Fixed decisions

| Area | Owner |
|---|---|
| Community public API, CLI, format and backend-neutral semantics | corresponding Community repository |
| Enterprise profile, diagnostics, packaging, capability mask and no-fallback adapter | corresponding Enterprise repository |
| Generic hash, MAC, KDF, AEAD, public-key primitive, random, CSP/key lifecycle and stable C ABI | AnoCrypto-C, after an explicit gate |
| `SKT1`, `SKF1`, `SKP1`, file/chunk framing, nonce composition and application output policy | AnoSecureKit |
| ASN.1, PEM/DER, PKCS#10, X.509, CRL, OCSP, chain/EKU/time policy and normalized operation results | AnoPKI |
| key-reference parsing, provider selection, HSM/KMS/PKCS#11 routing, signing evidence, retry/idempotency and audit | Enterprise product |

## Public-key boundary

| Concern | AnoCrypto-C candidate responsibility | Retained downstream responsibility |
|---|---|---|
| key lifecycle | generic in-memory public/private key handle, purpose/algorithm binding, destruction and zeroization | provider routing, authorization, persistence and external non-exportable key custody |
| key ingress | minimal algorithm-native key components after gate | PEM/DER/ASN.1/PKCS#8/SPKI parsing and validation workflow |
| sign/verify | generic RSA/ECDSA primitive after separate implementation gates | CSR/certificate/CRL/OCSP object construction and policy |
| signature encoding | candidate fixed primitive representation only | PKI DER encoding, object embedding and normalized result policy |
| external key reference | not parsed or routed by core | Enterprise HSM/KMS/PKCS#11 selection, retries, evidence and audit |

## P3-07 undecided items

- message-based versus digest-based sign/verify API;
- separate public/private handle types versus a common tagged handle;
- RSA and EC key-component import shape;
- ECDSA raw `r || s` representation versus another primitive-level contract;
- future symbol count and ABI-line assignment.

These items are `TODO` for the P3-07 architecture gate and are not implementation
authorization.

## Boundary consequences

- AnoPKI retains all codec, PKI object, and policy semantics.
- OpenSSL `X509_*`, `OCSP_*`, `PEM_*`, `d2i_*`, and `i2d_*` call sequences are
  not copied into AnoCrypto-C.
- AnoSecureKit v1 bytes remain downstream-owned.
- Provider capability masks remain downstream-owned and must report only real
  linked services.
- No operation may fall back to OpenSSL when an AnoCrypto-C profile is selected.
