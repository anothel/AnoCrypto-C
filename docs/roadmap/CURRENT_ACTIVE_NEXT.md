# Current / Active / Next

Status: `P3_06_COMPLETE_P3_07_READY`

## DONE

- P0/P1 cryptographic core and release engineering.
- P2 local validation-readiness snapshot.
- P3-00 downstream intake gate.
- P3-01 four-source requirement consolidation and ownership decisions.
- P3-02 minimal HKDF-SHA-256 API and additive `ANOCRYPTO_C_1` evidence.
- P3-03 AES-256-GCM one-shot and additive `ANOCRYPTO_C_2` evidence.
- P3-04 AES-256-GCM streaming and additive `ANOCRYPTO_C_3` evidence.
- P3-05 fixed-profile scrypt, caller-owned resource gate, and additive
  `ANOCRYPTO_C_4` evidence.
- P3-06 AnoSecureKit Enterprise seven-capability parity and no-fallback
  engineering checkpoint. P3-06 changed no AnoCrypto-C public header, symbol,
  or ABI line.

## ACTIVE

`P3-07: Public-key key-handle architecture and AnoPKI contract gate`

P3-07 is design-only. It must review exact AnoPKI requirements, freeze ownership
boundaries, compare handle/import/signature API alternatives, define lifecycle
and failure contracts, produce a future symbol budget, and select at most one
subsequent implementation slice.

P3-07 may not add public code, headers, functions, types, enum values, symbols,
or `ANOCRYPTO_C_5`. It may not implement SHA-1, RSA, ECDSA, PEM, DER, ASN.1,
CSR, X.509, CRL, OCSP, HSM, KMS, or PKCS#11 routing.

## NEXT AFTER THE GATE

`NOT_SELECTED`.

A later task may implement at most one explicitly approved slice after P3-07.
RSA, ECDSA, and restricted SHA-1 do not receive implementation authorization
from this roadmap entry alone.

## DEFERRED / BLOCKED

- KCMVP consultation/submission: `DEFERRED`.
- P1-34 hosted lanes: `NOT_RUN`; review: `UNREVIEWED`.
- Repository signing: `UNSIGNED`.
- Signing handoff: `BLOCKED`.
- External release authorization: `NO`.
- AnoSecureKit content discrepancy: `CLOSED_P3_06`.
- Original upstream Git-tree reproduction: `EXTERNAL_CONFIRMATION_REQUIRED`.
- Trusted release provenance acceptance: `OPEN`.
