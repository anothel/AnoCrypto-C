# Next AnoCrypto-C Work Prompt — P3-10

Continue from `0.1.0-p3-09-ecdsa-p256-sha256`.

Current public surface:

- 17 headers;
- 84 symbols;
- ABI lines `ANOCRYPTO_C_0` through `ANOCRYPTO_C_6`;
- P1-29 retained compatibility baseline.

Task: `P3-10 restricted SHA-1 interoperability API/ABI gate for OCSP CertID`.

Required work:

1. Reconfirm the exact AnoPKI OCSP CertID issuer-name and issuer-key hashing
   byte inputs, default behavior, fixtures, and 20-byte output.
2. Review a dedicated restricted primitive versus extending the generic hash
   enum. Prefer the smallest misuse-resistant contract.
3. Do not authorize SHA-1 certificate, CSR, CRL, or OCSP response signatures.
4. Do not add HMAC-SHA-1, KDF use, password use, collision-resistance claims,
   or general product workflow.
5. Freeze null, zero-length, alias, overlap, overflow, output-clearing,
   module-state, and concurrency behavior before implementation.
6. Preserve every symbol on ABI lines 0 through 6. Do not reserve a new line
   before API review passes.
7. Add public-source SHA-1 KATs, negative/boundary tests, OpenSSL test-only
   differential validation, and exact AnoPKI OCSP CertID compatibility evidence.
8. Update current/retained consumers, install/export, PoC, policy, symbol
   versioning, evidence, roadmap, and public-surface manifests.
9. Keep KCMVP, hosted, signing, provenance, and external release states unchanged.
10. Do not implement PKI codecs, provider routing, RSA-PSS, Ed25519, ECDH, PQC,
    or any high-level OCSP API.

Recommended artifact label: `p3-10-restricted-sha1-ocsp-certid`.

Recommended commit title:

`feat(hash): add restricted SHA-1 for OCSP CertID interoperability`
