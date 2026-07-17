# Cross-Repository Requirement Ownership

Status: `P3_01_OWNERSHIP_DECISIONS_APPROVED`

## Fixed decisions

| Area | Owner |
|---|---|
| Community public API, CLI, format and backend-neutral semantics | corresponding Community repository |
| Enterprise profile, diagnostics, packaging, capability mask and no-fallback adapter | corresponding Enterprise repository |
| Generic hash, MAC, KDF, AEAD, public-key primitive, random, CSP/key lifecycle and stable C ABI | AnoCrypto-C, after gate |
| `SKT1`, `SKF1`, `SKP1`, file/chunk framing, nonce composition and application output policy | AnoSecureKit |
| ASN.1, PEM/DER, PKCS#10, X.509, CRL, OCSP, chain/EKU/time policy and normalized operation results | AnoPKI |
| key reference parsing, provider selection, HSM/KMS routing, signing evidence, retry/idempotency and audit | Enterprise product |

## AnoPKI codec decision

P3-01 selects the Enterprise-owned/AnoPKI-owned codec model. AnoCrypto-C will
not add high-level CSR, certificate, CRL or OCSP APIs from this intake.
AnoPKI may consume generic AnoCrypto-C hash, signature, key-handle and random
services through its Enterprise adapter while retaining all PKI object and policy semantics.

AnoPKI retains all codec, PKI object and policy semantics.

## Boundary consequences

- OpenSSL `X509_*`, `OCSP_*`, `PEM_*`, `d2i_*` and `i2d_*` call sequences are not copied into AnoCrypto-C.
- AnoSecureKit v1 bytes are preserved; missing primitives are implemented beneath the provider contract.
- Provider capability masks remain downstream-owned and must report only real linked services.
- No operation may fall back to OpenSSL when the AnoCrypto-C profile is selected.
