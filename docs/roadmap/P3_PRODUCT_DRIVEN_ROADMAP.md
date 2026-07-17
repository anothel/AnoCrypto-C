# P3 Product-Driven Roadmap

Status: `P3_01_REQUIREMENTS_CONSOLIDATED`

## Completed

### P3-00

Created the fail-closed requirement intake gate.

### P3-01

- imported four source sets;
- verified archive hashes and exact revisions;
- recorded one AnoSecureKit dependency identity conflict and PKI document drift;
- accepted 20 product/contract rows;
- assigned cross-repository ownership;
- approved eight AnoCrypto-C gaps;
- selected HKDF-SHA-256 as the first implementation.

## Approved implementation sequence

### P3-02 — HKDF-SHA-256

Minimal public C API, RFC 5869 evidence, additive ABI line and exact AnoSecureKit compatibility.

### P3-03 — AES-256-GCM one-shot

Authenticated-output-only semantics, KATs, corruption/overlap/zeroization tests and downstream fixture evidence.

### P3-04 — AES-256-GCM streaming

Explicit AAD/data/final states and defined treatment of pre-final decrypted bytes.

### P3-05 — scrypt profile

Resource/overflow-safe design for `SKP1` v1 fixed parameters. Generic API shape requires a separate review.

### P3-06 — AnoSecureKit parity checkpoint

Downstream adapter work: complete the AnoCrypto-C profile, run public API and `SKT1`/`SKF1`/`SKP1` parity, prove no OpenSSL fallback and pin the SDK artifact.

## PKI lane — gated

Before algorithm implementation, approve a public-key key/CSP representation,
key ingress and signature encoding contract. Then schedule restricted SHA-1,
RSA PKCS#1 v1.5 SHA-2 and ECDSA P-256 SHA-256 slices from accepted requirements.
AnoPKI retains all ASN.1/X.509/CSR/CRL/OCSP codecs and product workflows.
