# P3 Product-Driven Roadmap

Status: `P3_07_COMPLETE_P3_08_SELECTED`

## Completed core slices

- P3-00: fail-closed downstream intake gate.
- P3-01: exact four-source requirement consolidation, ownership, and eight-gap
  register.
- P3-02: RFC 5869 HKDF-SHA-256 on `ANOCRYPTO_C_1`.
- P3-03: AES-256-GCM one-shot on `ANOCRYPTO_C_2`.
- P3-04: AES-256-GCM streaming on `ANOCRYPTO_C_3`.
- P3-05: fixed-profile scrypt on `ANOCRYPTO_C_4`.

## Completed downstream checkpoint

P3-06 connected seven AnoSecureKit capabilities with no OpenSSL fallback. It
changed no AnoCrypto-C public API/ABI and remains an engineering-only,
non-release checkpoint.

## Completed architecture gate

### P3-07 — public-key/key-handle architecture and AnoPKI contract

P3-07 rechecked exact AnoPKI operation and provider evidence and approved:

1. the existing opaque CSP token with internal public/private role, algorithm,
   and purpose binding;
2. normalized RSA and P-256 component import, with all PKI/key-container codecs
   retained in AnoPKI;
3. message-based generic sign/verify;
4. AnoCrypto-C ownership of SHA-2 and RSA DigestInfo;
5. raw 64-byte P-256 `r || s`, with DER retained in AnoPKI;
6. lifecycle, zeroization, stale-handle, failure, and concurrency-lease rules;
7. a smallest reasonable RSA candidate budget of three symbols.

P3-07 changed no public header, symbol, ABI line, or runtime version.

## Selected next core slice

### P3-08 — RSA PKCS#1 v1.5 SHA-2 and RSA key-handle import

Required entry sequence:

1. freeze exact RSA sizes/exponent policy and published KAT sources;
2. freeze the bounded asymmetric storage and operation-lease implementation;
3. review exact public names/prototypes and symbol count;
4. add no more than the approved minimum unless evidence requires it;
5. introduce an additive ABI line only after the API review passes;
6. implement RSA sign/verify using SHA-256/384/512 and canonical DigestInfo;
7. add component, lifecycle, concurrency, negative, differential, consumer,
   install/export, PoC, symbol-version, and exact AnoPKI compatibility evidence.

The candidate surface is one RSA import function plus generic sign and verify.
It is not already ABI-approved.

## Later sequence

1. P-256 ECDSA import and implementation using the shared sign/verify API;
2. restricted SHA-1 OCSP CertID API/policy gate;
3. AnoPKI Enterprise public-key parity and no-fallback checkpoint;
4. separate hosted/provenance/signing/release governance work.

The relative order after P3-08 may be reopened by new exact product evidence.

## Deferred or not approved

- PEM/DER/ASN.1/PKCS#8/SPKI/CSR/X.509/CRL/OCSP implementation in core;
- HSM/KMS/PKCS#11 and `key_ref` routing in core;
- key persistence/export/wrapping or generic key generation;
- digest-based public sign API without a positive requirement;
- Ed25519, RSA-PSS, ECDH, and PQC;
- KCMVP consultation/submission;
- production signing and external release.
