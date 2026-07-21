# Current / Active / Next

Status: `P3_08_COMPLETE_P3_09_ECDSA_GATE_NEXT`

## Retained policy compatibility

Historical P3-01/P3-02 tokens remain only where frozen policy regression tests
require them. Retained token: `P3-02 minimal HKDF-SHA-256 API`. They are not current work.

## DONE

- P0/P1 core and release engineering;
- P2 local validation-readiness snapshot;
- P3-00/P3-01 downstream intake, ownership, and gap consolidation;
- P3-02 HKDF-SHA-256;
- P3-03 AES-256-GCM one-shot;
- P3-04 AES-256-GCM streaming;
- P3-05 fixed-profile scrypt;
- P3-06 AnoSecureKit Enterprise parity/no-fallback checkpoint;
- P3-07 public-key/key-handle architecture gate;
- P3-08 RSA-2048 PKCS#1 v1.5 SHA-256/384/512 and RSA key-handle import.

P3-08 adds one header and three symbols on `ANOCRYPTO_C_5`. The current surface
is 17 headers and 83 symbols. ABI-0 through ABI-4 and P1-29 consumers remain
unchanged.

## ACTIVE

P3-08 local closeout and evidence packaging.

## NEXT

`P3-09: ECDSA P-256 SHA-256 API/ABI and AnoPKI compatibility gate`

P3-09 must independently review exact component import, reuse of the generic
message sign/verify contract, raw 64-byte `r || s`, deterministic/random nonce
policy, side-channel controls, additive ABI impact, and AnoPKI DER conversion.
It must not silently broaden to other curves, digest variants, ECDH, EdDSA, or
PKI codecs.

## LATER

- restricted SHA-1 through a misuse-resistant OCSP CertID-only gate;
- AnoPKI Enterprise RSA/ECDSA adapter parity and no-fallback checkpoint;
- separate hosted/provenance/signing/release qualification.

## DEFERRED / BLOCKED

- KCMVP: `DEFERRED`;
- hosted lanes/review: `NOT_RUN` / `UNREVIEWED`;
- repository signing/handoff: `UNSIGNED` / `BLOCKED`;
- external release authorization: `NO`;
- trusted provenance acceptance: `OPEN`;
- exact unavailable downstream archive recomputation:
  `EXTERNAL_CONFIRMATION_REQUIRED`.
