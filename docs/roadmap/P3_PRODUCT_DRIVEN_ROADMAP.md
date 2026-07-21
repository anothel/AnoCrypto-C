# P3 Product-Driven Roadmap

Status: `P3_09_COMPLETE_P3_10_RESTRICTED_SHA1_GATE_NEXT`

## Completed core slices

1. P3-02 HKDF-SHA-256 — `ANOCRYPTO_C_1`;
2. P3-03 AES-256-GCM one-shot — `ANOCRYPTO_C_2`;
3. P3-04 AES-256-GCM streaming — `ANOCRYPTO_C_3`;
4. P3-05 fixed-profile scrypt — `ANOCRYPTO_C_4`;
5. P3-07 public-key architecture gate — no public delta;
6. P3-08 RSA-2048 PKCS#1 v1.5 SHA-2 — `ANOCRYPTO_C_5`;
7. P3-09 ECDSA P-256 SHA-256 component import — `ANOCRYPTO_C_6`.

P3-09 reuses the generic message sign/verify contract, emits fixed 64-byte raw
`r || s`, uses RFC 6979 HMAC-SHA-256 nonces, validates `d*G == Q`, and leaves
DER conversion and every PKI workflow in AnoPKI.

## Completed downstream checkpoint

P3-06 connected all seven required AnoSecureKit capabilities with no OpenSSL
fallback. It remains engineering-only and release-blocked.

## Next core gate

### P3-10 — restricted SHA-1 for OCSP CertID interoperability

Required sequence:

1. re-freeze the exact AnoPKI OCSP CertID byte inputs and output length;
2. compare a dedicated restricted service with any generic hash-enum extension;
3. prohibit SHA-1 certificate/CRL/OCSP signatures and HMAC-SHA-1;
4. define misuse-resistant naming, null/alias/output-clearing, and module-state rules;
5. add RFC/NIST vectors, OpenSSL test-only differential, exact AnoPKI fixture,
   consumer, install, policy, and ABI evidence;
6. add an ABI line only if independently reviewed symbols require one.

## Later

- AnoPKI Enterprise RSA/ECDSA adapter parity/no-fallback checkpoint;
- hosted/provenance/signing/release governance.

## Not approved

PEM/DER/ASN.1/PKCS#8/SPKI/CSR/X.509/CRL/OCSP implementation in core;
HSM/KMS/PKCS#11 or `key_ref` routing in core; key persistence/export/wrapping;
RSA-PSS, Ed25519, ECDH, other curves, and PQC without new evidence; KCMVP
submission; production signing; external release.
