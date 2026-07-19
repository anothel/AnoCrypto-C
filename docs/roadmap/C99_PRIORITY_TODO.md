# C99 Priority TODO

Status: `P3_07_COMPLETE_P3_08_READY`

## Current public baseline

- runtime version: `0.1.0-p3-05-scrypt-fixed-profile`;
- public headers: 16;
- exported symbols: 80;
- ABI lines: `ANOCRYPTO_C_0` through `ANOCRYPTO_C_4`;
- retained compatibility baseline: P1-29.

## Completed product-driven sequence

- P3-00 downstream intake gate;
- P3-01 exact downstream requirement consolidation;
- P3-02 RFC 5869 HKDF-SHA-256;
- P3-03 AES-256-GCM one-shot;
- P3-04 AES-256-GCM streaming;
- P3-05 fixed-profile scrypt;
- P3-06 AnoSecureKit Enterprise seven-capability parity/no-fallback checkpoint;
- P3-07 public-key key-handle architecture and AnoPKI contract gate.

P3-07 approved a common existing opaque handle, module-owned algorithm-native
components, message-based signature services, module-owned SHA-2 and RSA
DigestInfo/EMSA, and raw P-256 `r || s` for a later gate. It added no public
code or ABI.

## Immediate next work

`P3-08: RSA PKCS#1 v1.5 SHA-256/384/512 and public-key handle foundation`

P3-08 must:

1. review and freeze exact component parameters, prototypes, names, and
   header placement;
2. freeze RSA modulus/exponent bounds, fixed-pool capacity, memory limits,
   blinding, CRT fault checks, and temporary zeroization;
3. preserve all current headers/symbols and add at most the reviewed minimal
   three-symbol candidate surface on one additive ABI line;
4. implement one role-selected RSA public/private component import using
   canonical n/e or n/e/d/p/q with full consistency checks;
5. implement message-based sign/verify with module-owned SHA-256/384/512 and
   canonical DigestInfo/EMSA;
6. define exact null, overflow, alias, overlap, size-query, output-clearing,
   verification, stale-handle, shutdown/fatal, and concurrency behavior;
7. add public KAT, negative, boundary, OpenSSL test-only differential, current
   and retained consumer, install/export, PoC, symbol-version, and exact AnoPKI
   compatibility evidence;
8. keep ECDSA, SHA-1, PKI codecs, external provider routing, and high-level PKI
   workflows out of the slice.

## Later product-driven candidates

- ECDSA P-256 SHA-256, after a separate API/ABI gate;
- restricted SHA-1 for OCSP CertID only, after a separate narrow policy gate;
- AnoPKI Enterprise parity/no-fallback after exact Enterprise source and
  primitive availability.

## P1-34 external blocker — unchanged

```text
Status: BLOCKED_EXTERNAL: GITHUB_ACTIONS_BILLING
Run: 29256531912
Commit: cf0f3c425dd265c7509fb64a6123db97f71055dd
Jobs reaching runner: 0
Artifacts: 0
Actual hosted lanes: NOT_RUN
Actual hosted review: UNREVIEWED
```

Local or synthetic evidence must not be substituted for actual hosted review.

## Deferred / unauthorized

- KCMVP consultation, submission, and validation claims;
- hosted completion claims;
- production signer/TSA provisioning, signing, timestamping, or release;
- PEM/DER/ASN.1/CSR/X.509/CRL/OCSP implementation in core;
- HSM/KMS/PKCS#11/key_ref routing in core;
- key persistence/export/wrapping;
- RSA-PSS, Ed25519, ECDH, PQC, and other algorithms without new exact evidence.
