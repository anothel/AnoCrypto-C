# P3 API and ABI Evolution Gate

Status: `P3_02_ADDITIVE_ABI_IMPLEMENTED`

## Retained baseline

- retained public headers: 14;
- retained exported symbols: 69;
- retained ABI line: `ANOCRYPTO_C_0`;
- retained source/ABI consumer: P1-29.

## P3-02 result

The API review froze exactly one new function in one new header:

- `include/anocrypto/ac_kdf.h`;
- `ano_crypto_hkdf_sha256`;
- `ANOCRYPTO_C_1` inherits `ANOCRYPTO_C_0` and contains only that symbol.

Current public surface is 15 headers and 70 symbols. No `ANOCRYPTO_C_0`
declaration or symbol was removed, renamed, or reassigned.

The detailed signature and null/empty/buffer/overlap/failure rules are frozen in
`docs/design/C99_HKDF_SHA256_API_GATE.md`.

## Evidence

- RFC 5869 cases 1, 2, and 3;
- lengths 0, 1, 32, 8160, and rejected/cleared 8161;
- empty IKM/salt/info;
- null, wrapping range, alias and overlap rejection;
- no partial output and full output clearing on accepted output ranges;
- OpenSSL 3 test-only differential validation;
- exact AnoSecureKit SKF1 v1 info bytes;
- current and retained P1-29 consumers;
- clean Release export-name and ELF symbol-version inspection;
- install/export and PoC smoke.

AEAD, scrypt, SHA-1, RSA, ECDSA, public-key architecture, and high-level product
APIs remain outside this gate.
