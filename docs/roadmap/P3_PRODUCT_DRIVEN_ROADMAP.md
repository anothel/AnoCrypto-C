# P3 Product-Driven Roadmap

Status: `P3_06_COMPLETE_P3_07_ACTIVE_GATE`

## Completed core slices

### P3-00

Created the fail-closed requirement intake gate.

### P3-01

Imported and verified four downstream source sets, accepted 20 product/contract
rows, assigned ownership, approved eight AnoCrypto-C gaps, and selected HKDF.

### P3-02 — HKDF-SHA-256

Implemented one generic RFC 5869 extract-and-expand function, retained all 69
`ANOCRYPTO_C_0` symbols, added one `ANOCRYPTO_C_1` symbol, and verified exact
AnoSecureKit `SKF1` v1 info-byte compatibility without implementing `SKF1`.

### P3-03 — AES-256-GCM one-shot

Implemented two generic one-shot functions and added `ANOCRYPTO_C_2` without
changing earlier ABI lines. Downstream formats remained outside core.

### P3-04 — AES-256-GCM streaming

Implemented seven caller-owned streaming functions on `ANOCRYPTO_C_3` with
explicit state and unauthenticated-decrypt-output rules.

### P3-05 — fixed-profile scrypt

Implemented one generic fixed-profile function on `ANOCRYPTO_C_4` using
`N=32768`, `r=8`, `p=1`, a 32-byte result, and caller-owned bounded workspace.

## Completed downstream checkpoint

### P3-06 — AnoSecureKit Enterprise parity/no fallback

Connected SHA-256, HMAC-SHA-256, HKDF-SHA-256, random, fixed-profile scrypt,
AES-256-GCM one-shot, and AES-256-GCM streaming through the public P3-05 SDK.
The runtime reports exactly these seven capabilities and does not silently fall
back to OpenSSL. Exact `SKT1`, `SKF1`, and `SKP1` compatibility evidence passes.
P3-06 did not change the AnoCrypto-C public surface and does not authorize a
customer release.

## Active gate

### P3-07 — public-key/key-handle architecture and AnoPKI contract

Design-only work:

1. revalidate exact AnoPKI operation requirements;
2. freeze repository ownership;
3. compare key-handle and key-import alternatives;
4. compare message/digest sign/verify alternatives;
5. define lifecycle, zeroization, concurrency, buffer, and failure rules;
6. estimate the smallest future symbol budget;
7. select at most one later implementation slice.

No public API, ABI line, SHA-1, RSA, or ECDSA implementation is authorized in
P3-07.

## Later implementation sequence

The exact sequence is `NOT_SELECTED`. Candidate slices remain:

- restricted SHA-1 only if the current OCSP CertID requirement is reconfirmed;
- RSA PKCS#1 v1.5 SHA-256/384/512 sign/verify after the key gate;
- ECDSA P-256 SHA-256 sign/verify after the key gate;
- AnoPKI Enterprise parity/no-fallback checkpoint after relevant primitives.

Each implementation requires its own API/ABI gate and one numbered work item.

## Parallel AnoSecureKit Enterprise qualification lane

P3-06 parity is complete, but hosted qualification, Windows/macOS dependency
inspection, trusted provenance, signing/attestation intake, support approval,
and external release authorization remain separate blocked work.

## Deferred or not approved

- high-level PKI APIs in AnoCrypto-C;
- Ed25519, RSA-PSS and PQC without new product evidence;
- key persistence/export/wrapping;
- HSM/KMS/PKCS#11 routing in core;
- KCMVP consultation/submission;
- production signing and external release.
