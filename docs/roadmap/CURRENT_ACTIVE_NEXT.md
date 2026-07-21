# Current / Active / Next

Status: `P3_09_COMPLETE_P3_10_RESTRICTED_SHA1_GATE_NEXT`

## Retained policy compatibility

Historical P3-01/P3-02 tokens remain only for frozen policy regression tests.
Retained token: `P3-02 minimal HKDF-SHA-256 API`. It is not current work.

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
- P3-08 RSA-2048 PKCS#1 v1.5 SHA-256/384/512;
- P3-09 ECDSA P-256 SHA-256 and component import.

P3-09 adds one symbol on `ANOCRYPTO_C_6` and reuses the generic sign/verify
functions on `ANOCRYPTO_C_5`. The current surface is 17 headers and 84 symbols.
ABI-0 through ABI-5 and P1-29 consumers remain unchanged.

## ACTIVE

P3-09 local closeout, full evidence, and packaging.

## NEXT

`P3-10: restricted SHA-1 interoperability API/ABI gate for OCSP CertID`

The exact AnoPKI operation review confirms that SHA-1 remains needed for legacy
OCSP CertID issuer-name/key hashing. P3-10 must determine a misuse-resistant
primitive boundary without enabling SHA-1 certificate signatures, HMAC-SHA-1,
issuance policy, or general product workflows. No public enum, function, symbol,
or ABI line is pre-approved.

## LATER

- AnoPKI Enterprise RSA/ECDSA adapter parity and no-fallback checkpoint;
- hosted/provenance/signing/release qualification;
- future algorithms only after new product evidence.

## DEFERRED / BLOCKED

- KCMVP: `DEFERRED`;
- hosted lanes/review: `NOT_RUN` / `UNREVIEWED`;
- repository signing/handoff: `UNSIGNED` / `BLOCKED`;
- external release authorization: `NO`;
- trusted provenance acceptance: `OPEN`;
- exact unavailable downstream archive recomputation:
  `EXTERNAL_CONFIRMATION_REQUIRED`.
