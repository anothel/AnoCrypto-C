# Restricted SHA-1 OCSP CertID Implementation Review

Status: `P3_10_REVIEWED_ENGINEERING_EVIDENCE`

- Public API: one dedicated one-shot OCSP CertID digest function.
- Public header count: 18.
- Exported symbol count: 85.
- ABI: one symbol on `ANOCRYPTO_C_7`; ABI-0 through ABI-6 retained.
- Digest: exactly 20 bytes.
- Inputs: downstream-selected issuer Name DER or subjectPublicKey BIT STRING
  contents.
- No generic SHA-1 enum or streaming API.
- No SHA-1 signature, HMAC, KDF, or password service.
- No collision-resistance or general-purpose SHA-1 security claim.
- No OpenSSL production dependency or downstream PKI codec logic.
- Exact OpenSSL OCSP CertID comparison is test-only.
- Hosted, signing, provenance, external release, and KCMVP blockers remain
  unchanged.
