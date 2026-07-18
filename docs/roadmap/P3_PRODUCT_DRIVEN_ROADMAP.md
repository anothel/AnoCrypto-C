# P3 Product-Driven Roadmap

Status: `P3_02_COMPLETE_P3_03_NEXT`

## Completed

### P3-00

Created the fail-closed requirement intake gate.

### P3-01

Imported and verified four downstream source sets, accepted 20 product/contract
rows, assigned ownership, approved eight AnoCrypto-C gaps, and selected HKDF.

### P3-02 — HKDF-SHA-256

Implemented one generic RFC 5869 extract-and-expand function, retained all 69
`ANOCRYPTO_C_0` symbols, added one `ANOCRYPTO_C_1` symbol, and verified exact
AnoSecureKit SKF1 v1 info-byte compatibility without implementing SKF1.

## Approved implementation sequence

### P3-03 — AES-256-GCM one-shot

Authenticated-output-only semantics, KATs, corruption/overlap/zeroization tests
and downstream fixture evidence. Separate API review required.

### P3-04 — AES-256-GCM streaming

Explicit AAD/data/final states and defined treatment of pre-final decrypted bytes.

### P3-05 — scrypt profile

Resource/overflow-safe design for `SKP1` v1 fixed parameters. Generic API shape
requires a separate review.

### P3-06 — AnoSecureKit parity checkpoint

Downstream adapter work: complete the AnoCrypto-C profile, run public API and
`SKT1`/`SKF1`/`SKP1` parity, prove no OpenSSL fallback, resolve exact Community
identity, and pin the SDK artifact.

## PKI lane — gated

Approve public-key key/CSP representation, key ingress, metadata/binding, and
signature encoding before restricted SHA-1, RSA, or ECDSA implementation.
AnoPKI retains all codecs and workflows.
