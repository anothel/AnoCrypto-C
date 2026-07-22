# AnoPKI OCSP CertID Restricted SHA-1 Compatibility

Status: `P3_10_PRIMITIVE_COMPATIBILITY_COMPLETE_ADAPTER_PENDING`

AnoPKI Community uses SHA-1 as the default and interoperable digest for OCSP
CertID issuer-name and issuer-key hashes. P3-10 validates the generic primitive
boundary without implementing OCSP in AnoCrypto-C.

## Exact byte contracts

- issuer name: complete DER encoding produced for the issuer `Name`;
- issuer key: subjectPublicKey BIT STRING contents only, excluding the leading
  unused-bit-count octet;
- output: 20-byte SHA-1 digest.

The OpenSSL test-only differential constructs an issuer certificate, calls
`OCSP_cert_to_id(EVP_sha1(), issuer, issuer)`, extracts the exact OpenSSL hashes,
and compares them with AnoCrypto-C results over the corresponding Name DER and
public-key BIT STRING bytes.

## Evidence

- public KATs for empty input, `abc`, long input, fixed Name DER, and fixed key
  BIT STRING contents;
- negative tests for null, short buffer, overlap, module state, and clearing;
- deterministic OpenSSL differential cases plus exact OCSP CertID comparison;
- production source has no OpenSSL dependency.

## Boundary

AnoPKI remains responsible for certificate parsing, DER production, selecting
the exact issuer fields, constructing and parsing OCSP CertID, and representing
hash algorithm identifiers. P3-10 does not add a generic SHA-1 service.
