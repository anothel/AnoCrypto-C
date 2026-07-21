# C99 Priority TODO

Status: `P3_09_COMPLETE_P3_10_NEXT`

## Current baseline

- runtime version: `0.1.0-p3-09-ecdsa-p256-sha256`;
- public headers: 17;
- exported symbols: 84;
- ABI lines: `ANOCRYPTO_C_0` through `ANOCRYPTO_C_6`;
- retained compatibility: P1-29.

## Completed product-driven sequence

- P3-02 HKDF-SHA-256;
- P3-03 AES-256-GCM one-shot;
- P3-04 AES-256-GCM streaming;
- P3-05 fixed-profile scrypt;
- P3-06 AnoSecureKit parity/no-fallback;
- P3-07 public-key architecture;
- P3-08 RSA PKCS#1 v1.5 SHA-2;
- P3-09 ECDSA P-256 SHA-256.

## Highest priority

`P3-10 restricted SHA-1 interoperability gate for OCSP CertID`.

Do not implement SHA-1 signatures, HMAC-SHA-1, PKI codecs, provider routing,
production signing, KCMVP submission, or external release in that slice.
