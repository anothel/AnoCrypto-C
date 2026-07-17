# Current / Active / Next

Status: `P3_01_COMPLETE_P3_02_SELECTED`

## DONE

- P0/P1 cryptographic core and release engineering.
- P2 local validation-readiness snapshot.
- P3-00 downstream intake gate.
- P3-01 import and analysis of four downstream source sets.
- product/primitive ownership decisions and eight accepted gaps.

## ACTIVE

`P3-02: HKDF-SHA-256 public C API and downstream compatibility evidence`

The implementation has not started in P3-01. P3-02 must design the minimal C99
API, add KAT/negative/differential tests, preserve `ANOCRYPTO_C_0`, introduce
additive `ANOCRYPTO_C_1` symbols and prove the exact AnoSecureKit `SKF1` case.

## NEXT

1. P3-03 AES-256-GCM one-shot.
2. P3-04 AES-256-GCM streaming.
3. P3-05 scrypt fixed-profile API/resource gate.
4. AnoSecureKit Enterprise adapter parity and no-fallback evidence.
5. PKI public-key/key-handle architecture gate.
6. RSA/ECDSA/SHA-1 interoperability slices only after that gate.

## DEFERRED

- KCMVP consultation/submission;
- Ed25519, RSA-PSS, PQC and unsupported algorithm expansion;
- high-level PKI APIs in AnoCrypto-C;
- production signing and external release.

## BLOCKED_EXTERNAL

- P1-34 actual hosted review: GitHub Actions billing;
- hosted lanes `NOT_RUN`, review `UNREVIEWED`;
- repository `UNSIGNED`, external release authorization `NO`.
