# C99 Restricted SHA-1 OCSP CertID API Gate

Status: `APPROVED_AND_IMPLEMENTED_P3_10`

## Decision

P3-10 adds one one-shot function in a dedicated misuse-resistant `ac_ocsp_certid_hash.h` public header:

```c
ano_crypto_ocsp_certid_sha1_digest(input, input_len, out, out_len, out_written)
```

The public surface is 18 headers and 85 symbols. The function is the only symbol
on additive `ANOCRYPTO_C_7`; ABI lines 0 through 6 remain unchanged.

## Allowed use

The function exists solely to reproduce the SHA-1 hashes required by the OCSP
CertID interoperability contract:

- `issuerNameHash`: DER encoding of the issuer `Name`;
- `issuerKeyHash`: subjectPublicKey BIT STRING contents, excluding the
  unused-bit-count octet.

AnoPKI owns ASN.1, DER, certificate parsing, OCSP CertID construction, and input
selection. AnoCrypto-C receives only the exact bytes to hash.

## Misuse containment

- No generic SHA-1 hash enum is added.
- No SHA-1 streaming API is added.
- SHA-1 signatures are prohibited.
- SHA-1 HMAC, KDF, password derivation, certificate issuance, and content
  integrity services are prohibited.
- The API makes no collision-resistance or general-security claim.

## Contract

- Digest size is exactly 20 bytes.
- `NULL` input is valid only for zero length.
- Output and `out_written` are required.
- Input/output and metadata overlap are rejected.
- Short output is rejected with output clearing.
- On every failure, `out_written` is zero when addressable and up to the first
  20 output bytes are cleared.
- Module state is enforced before computation.

## Ownership

The primitive contains no PEM, DER, ASN.1, X.509, OCSP, PKCS#8, SPKI, provider,
routing, evidence, or audit logic. Those remain downstream-owned.
