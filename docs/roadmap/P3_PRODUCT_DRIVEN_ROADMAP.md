# P3 Product-Driven Roadmap

Status: `P3_10_CORE_SEQUENCE_COMPLETE_P3_11_ANOPKI_PARITY_NEXT`

## Completed core slices

1. P3-02 HKDF-SHA-256 — `ANOCRYPTO_C_1`;
2. P3-03 AES-256-GCM one-shot — `ANOCRYPTO_C_2`;
3. P3-04 AES-256-GCM streaming — `ANOCRYPTO_C_3`;
4. P3-05 fixed-profile scrypt — `ANOCRYPTO_C_4`;
5. P3-07 public-key architecture gate — no public delta;
6. P3-08 RSA-2048 PKCS#1 v1.5 SHA-2 — `ANOCRYPTO_C_5`;
7. P3-09 ECDSA P-256 SHA-256 component import — `ANOCRYPTO_C_6`;
8. P3-10 restricted SHA-1 for OCSP CertID — `ANOCRYPTO_C_7`.

P3-10 adds no generic SHA-1 algorithm enum. It exposes one dedicated one-shot
service for exact OCSP CertID issuer Name DER and subjectPublicKey BIT STRING
content hashing. SHA-1 signatures, streaming, HMAC, KDF, password use, and
certificate issuance remain prohibited.

## Completed downstream checkpoint

P3-06 connected all seven required AnoSecureKit capabilities with no OpenSSL
fallback. It remains engineering-only and release-blocked.

## Next downstream checkpoint

### P3-11 — AnoPKI Enterprise RSA/ECDSA/OCSP parity and no fallback

1. consume exact P3-10 AnoCrypto-C SDK identity;
2. connect RSA sign/verify and P-256 sign/verify below AnoPKI codecs;
3. connect restricted SHA-1 only for OCSP CertID issuer hashes;
4. preserve DER ECDSA encoding and all PKI object construction in AnoPKI;
5. report truthful capabilities and fail closed for unsupported paths;
6. prove no silent OpenSSL fallback and inspect runtime dependencies;
7. retain hosted, provenance, signing, and release blockers.

## Not approved

PEM/DER/ASN.1/PKCS#8/SPKI/CSR/X.509/CRL/OCSP implementation in core;
HSM/KMS/PKCS#11 or `key_ref` routing in core; key persistence/export/wrapping;
general-purpose SHA-1; RSA-PSS, Ed25519, ECDH, other curves, and PQC without
new evidence; KCMVP submission; production signing; external release.
