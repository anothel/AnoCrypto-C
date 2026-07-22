# Current / Active / Next

Status: `P3_10_COMPLETE_P3_11_ANOPKI_PARITY_NEXT`

Retained historical policy token: `P3-02 minimal HKDF-SHA-256 API`. It is not current work.

## DONE

- P3-02 HKDF-SHA-256 (`ANOCRYPTO_C_1`);
- P3-03 AES-256-GCM one-shot (`ANOCRYPTO_C_2`);
- P3-04 AES-256-GCM streaming (`ANOCRYPTO_C_3`);
- P3-05 fixed-profile scrypt (`ANOCRYPTO_C_4`);
- P3-06 AnoSecureKit Enterprise parity/no-fallback checkpoint;
- P3-07 public-key/key-handle architecture gate;
- P3-08 RSA-2048 PKCS#1 v1.5 SHA-256/384/512 (`ANOCRYPTO_C_5`);
- P3-09 ECDSA P-256 SHA-256 component import (`ANOCRYPTO_C_6`);
- P3-10 restricted SHA-1 for OCSP CertID (`ANOCRYPTO_C_7`).

The current core surface is 18 public headers and 85 exported symbols. ABI-0
through ABI-6 remain unchanged; the only ABI-7 symbol is
`ano_crypto_ocsp_certid_sha1_digest`.

## ACTIVE

`P3-11: AnoPKI Enterprise RSA/ECDSA/OCSP primitive parity and no-fallback checkpoint` — `READY_TO_START`

P3-11 is downstream integration work. It must consume the P3-10 public SDK and
connect RSA, P-256 ECDSA, and restricted OCSP CertID SHA-1 without adding a new
AnoCrypto-C primitive or silently falling back to OpenSSL. AnoPKI retains all
ASN.1, PEM/DER, CSR, X.509, CRL, OCSP, signature-DER, provider-routing,
evidence, and audit behavior.

## NEXT

No post-P3-11 core algorithm or release task is selected.

## DEFERRED / BLOCKED

- KCMVP: `DEFERRED`;
- hosted lanes/review: `NOT_RUN` / `UNREVIEWED`;
- repository signing/handoff: `UNSIGNED` / `BLOCKED`;
- external release authorization: `NO`;
- trusted provenance acceptance: `OPEN`.
