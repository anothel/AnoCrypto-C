# C99 Priority TODO

Status: `P3_10_COMPLETE_P3_11_ANOPKI_PARITY_NEXT`

## Current core

- version: `0.1.0-p3-10-restricted-sha1-ocsp-certid`;
- public headers: 18;
- exported symbols: 85;
- ABI lines: `ANOCRYPTO_C_0` through `ANOCRYPTO_C_7`;
- P1-29 compatibility retained.

## Next

1. consume P3-10 in AnoPKI Enterprise;
2. connect RSA, P-256 ECDSA, and restricted OCSP CertID SHA-1;
3. preserve downstream codecs, DER signature representation, provider routing,
   evidence, and audit ownership;
4. prove truthful capability reporting and no OpenSSL fallback;
5. keep external release blockers unchanged.

No new AnoCrypto-C algorithm is selected.
