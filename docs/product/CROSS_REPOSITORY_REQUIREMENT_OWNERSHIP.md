# Cross-Repository Requirement Ownership

Status: `P3_00_INTAKE_RULE`

## Ownership test

| Requirement kind | Default owner |
|---|---|
| Community public API, CLI, format or backend-neutral behavior | corresponding Community repository |
| OpenSSL provider implementation defect | corresponding Community repository |
| Enterprise profile, diagnostics, policy, packaging or proprietary adapter | corresponding Enterprise repository |
| Generic cryptographic primitive, key/CSP lifecycle, random/DRBG, self-test or stable C ABI | AnoCrypto-C candidate, subject to gate |
| X.509, CSR, CRL, OCSP, trust, ASN.1 or PKI workflow | AnoPKI Core/Enterprise, not AnoCrypto-C |
| envelope, packet/file/password format, nonce orchestration or application policy | AnoSecureKit, not AnoCrypto-C |
| HSM/KMS/PKCS#11 routing and provider orchestration | owning Enterprise product unless a generic primitive gap is separately proven |

## Routing questions

1. Is the behavior a product operation or a reusable cryptographic primitive?
2. Can the backend-neutral product contract express it without AnoCrypto-C types?
3. Does the OpenSSL implementation combine primitive work with product policy?
4. Is the primitive already present in AnoCrypto-C?
5. Would adding it alter a Community v1 format or operation contract?
6. What public API, ABI, CSP, self-test, zeroization and evidence impact follows?

## Prohibited shortcuts

- copying Community operation code into AnoCrypto-C;
- treating OpenSSL API calls as a one-to-one AnoCrypto-C API specification;
- modifying a fixed product format to fit missing AnoCrypto-C capability;
- implementing an Enterprise adapter concern inside the cryptographic module;
- using silent OpenSSL fallback as parity evidence;
- starting implementation from an unversioned requirement note.
