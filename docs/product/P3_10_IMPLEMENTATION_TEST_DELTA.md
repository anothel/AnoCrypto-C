# P3-10 Implementation and Test Delta

Version: `0.1.0-p3-10-restricted-sha1-ocsp-certid`

## Public delta

- added dedicated `include/anocrypto/ac_ocsp_certid_hash.h`;
- retained existing `include/anocrypto/ac_hash.h` unchanged;
- added `ano_crypto_ocsp_certid_sha1_digest`;
- added one symbol on `ANOCRYPTO_C_7`;
- 18 public headers;
- current total: 85 exported symbols;
- retained every symbol and assignment on ABI lines 0 through 6.

## Implementation delta

- added an internal C99 SHA-1 core;
- added a dedicated one-shot OCSP CertID wrapper;
- added startup self-test and KAT manifest entry;
- retained module gating, failure clearing, and overlap policy;
- production code performs no heap allocation and has no OpenSSL dependency.

## Explicit exclusions

- No generic SHA-1 hash enum.
- No streaming SHA-1 API.
- No SHA-1 signature, HMAC, KDF, password, or certificate service.
- No ASN.1, DER, X.509, OCSP, PEM, or provider orchestration.

## Test delta

- restricted SHA-1 KAT and boundary vectors;
- API negative and output-clearing tests;
- OpenSSL EVP differential cases;
- exact OpenSSL OCSP CertID issuer-name and issuer-key compatibility;
- current and retained consumers, symbol-version, install, PoC, and evidence
  regression.
