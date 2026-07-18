# P3-02 Implementation and Test Delta

Status: `COMPLETE_LOCAL_EVIDENCE`

## Implemented

- one-function minimal C99 HKDF-SHA-256 API;
- RFC 5869 extract-and-expand using the existing internal HMAC-SHA-256 core;
- empty IKM/salt/info and zero-length output semantics;
- 8160-byte maximum and fail-closed 8161-byte rejection;
- non-wrapping range, output/input overlap, and output-clearing rules;
- additive ELF symbol versioning: 69 symbols at `ANOCRYPTO_C_0`, one at
  `ANOCRYPTO_C_1`;
- installed header, umbrella header, current consumer, retained P1-29 consumer,
  install smoke, and customer PoC coverage.

## Evidence

- RFC 5869 cases 1, 2, and 3;
- output lengths 0, 1, 32, 8160, and rejected 8161;
- null, overflow, alias, overlap, module-state, and no-partial-output tests;
- OpenSSL 3 test-only differential validation through 8160 bytes;
- exact AnoSecureKit SKF1 v1 28-byte info compatibility vector;
- clean Release export-name and ELF symbol-version inspection.

## Explicit exclusions

No AES-GCM, scrypt, SHA-1, RSA, ECDSA, high-level product API, SKF1 workflow,
push, tag, signing, timestamping, publication, or external distribution is in
this slice.

## Retained state

- KCMVP: `DEFERRED`
- P1-34 hosted lanes: `NOT_RUN`
- P1-34 review: `UNREVIEWED`
- Repository signing: `UNSIGNED`
- External release authorization: `NO`
- AnoSecureKit Community identity conflict: open
