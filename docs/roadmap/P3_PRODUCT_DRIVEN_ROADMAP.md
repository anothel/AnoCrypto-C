# P3 Product-Driven Roadmap

Status: `P3_08_COMPLETE_P3_09_NEXT`

## Completed core slices

1. P3-02 HKDF-SHA-256 — `ANOCRYPTO_C_1`;
2. P3-03 AES-256-GCM one-shot — `ANOCRYPTO_C_2`;
3. P3-04 AES-256-GCM streaming — `ANOCRYPTO_C_3`;
4. P3-05 fixed-profile scrypt — `ANOCRYPTO_C_4`;
5. P3-07 public-key architecture gate — no public delta;
6. P3-08 RSA-2048 PKCS#1 v1.5 SHA-2 and component import —
   `ANOCRYPTO_C_5`.

P3-08 froze a three-symbol public delta, bounded CSP storage, public/private
role enforcement, message-based SHA-2 signatures, canonical DigestInfo,
CRT exponent blinding, public fault checking, failure clearing, and current/
retained consumer evidence.

## Completed downstream checkpoint

P3-06 connected all seven required AnoSecureKit capabilities with no OpenSSL
fallback. It remains engineering-only and release-blocked.

## Next core slice

### P3-09 — ECDSA P-256 SHA-256

Required entry sequence:

1. freeze P-256 component sizes and key validation;
2. review whether existing import/sign/verify functions can be reused without
   semantic ambiguity;
3. retain raw 64-byte `r || s` in core and DER in AnoPKI;
4. freeze nonce generation and failure policy;
5. add KAT, negative, differential, lifecycle, consumer, ABI, and exact AnoPKI
   compatibility evidence;
6. add an ABI line only if independently reviewed public symbols are required.

## Later

- restricted SHA-1 OCSP CertID gate;
- AnoPKI Enterprise public-key parity/no-fallback checkpoint;
- hosted/provenance/signing/release governance.

## Not approved

PEM/DER/ASN.1/PKCS#8/SPKI/CSR/X.509/CRL/OCSP implementation in core;
HSM/KMS/PKCS#11 or `key_ref` routing in core; key persistence/export/wrapping;
RSA-PSS, Ed25519, ECDH, other curves, and PQC without new evidence; KCMVP
submission; production signing; external release.
