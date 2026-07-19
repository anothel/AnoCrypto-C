# P3 Product-Driven Roadmap

Status: `P3_05_COMPLETE_P3_06_NEXT`

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

### P3-03 — AES-256-GCM one-shot

Implemented two generic one-shot functions with a fixed 32-byte key, 12-byte
nonce and 16-byte tag; authenticated before plaintext output; retained ABI-0/1;
added two `ANOCRYPTO_C_2` symbols; verified NIST, negative, differential and exact
downstream v1 primitive fixtures without implementing downstream formats.

### P3-04 — AES-256-GCM streaming

Implemented a seven-function caller-owned opaque context API with explicit
AAD/payload/final states, arbitrary chunk boundaries, exact in-place support,
and additive `ANOCRYPTO_C_3`. Decrypt update output is explicitly unverified
until final authentication; callers must buffer, withhold, and wipe it on
failure. Exact `SKT1` v1 byte compatibility is evidence only.

### P3-05 — scrypt fixed profile

Implemented one generic fixed-profile function using `N=32768`, `r=8`, `p=1`, 32-byte output, caller-owned bounded workspace, full workspace zeroization, and additive `ANOCRYPTO_C_4`. Exact `SKP1` v1 key derivation/decryption is evidence only; no downstream format or password workflow is implemented.

## Approved implementation sequence

### P3-06 — AnoSecureKit parity checkpoint

Downstream adapter work: complete the AnoCrypto-C profile, run public API and
`SKT1`/`SKF1`/`SKP1` parity, prove no OpenSSL fallback, resolve exact Community
identity, and pin the SDK artifact.

## PKI lane — gated

Approve public-key key/CSP representation, key ingress, metadata/binding, and
signature encoding before restricted SHA-1, RSA, or ECDSA implementation.
AnoPKI retains all codecs and workflows.
