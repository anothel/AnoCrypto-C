# P3-01 Downstream Requirement Analysis

Status: `COMPLETE_PRODUCT_DRIVEN_CONSOLIDATION`

## Conclusions

1. All four requested source sets were imported and hash-verified.
2. AnoSecureKit requirements are sufficiently concrete to approve a first implementation sequence.
3. AnoPKI owns ASN.1, PEM/DER, PKCS#10, X.509, CRL and OCSP codecs and product operations.
4. AnoCrypto-C is considered only for generic primitives, key/CSP lifecycle and module contracts.
5. Eight AnoCrypto-C capability gaps are accepted; only HKDF-SHA-256 is selected for immediate implementation.
6. Current public surface remains 14 headers, 69 symbols and `ANOCRYPTO_C_0` until P3-02.

## Selected first implementation

`P3-02: HKDF-SHA-256 public C API, KATs and AnoSecureKit compatibility evidence`

Rationale:

- exact Community public API and CLI require HKDF-SHA-256;
- `SKF1` v1 derives its file key with exact info bytes;
- ownership is unambiguous;
- implementation can reuse the reviewed HMAC-SHA-256 core;
- it is smaller and lower-risk than the AEAD and public-key slices;
- it provides the first controlled exercise of additive P3 ABI evolution.

## Not selected yet

- AES-256-GCM one-shot and streaming are approved gaps but follow HKDF.
- scrypt is approved for `SKP1` compatibility, with API/resource-limit design still required.
- PKI public-key work requires a dedicated key representation and ownership gate.
- restricted SHA-1 is accepted only for OCSP CertID interoperability and requires a weak-algorithm policy gate.
