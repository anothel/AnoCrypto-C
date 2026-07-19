# P3 API and ABI Evolution Gate

Status: `P3_05_ADDITIVE_ABI_IMPLEMENTED`

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

## P3-03 result

- `ac_aead.h` adds exactly two generic AES-256-GCM one-shot functions.
- all 69 `ANOCRYPTO_C_0` symbols are unchanged;
- the single `ANOCRYPTO_C_1` HKDF symbol is unchanged;
- `ANOCRYPTO_C_2` inherits `ANOCRYPTO_C_1` and contains only the two GCM symbols;
- current public surface is 16 headers and 72 exported symbols.


## P3-04 result

- `ac_aead.h` is extended without adding another public header.
- Exactly seven generic AES-256-GCM streaming functions are added.
- All 69 `ANOCRYPTO_C_0` symbols, the one `ANOCRYPTO_C_1` HKDF symbol, and the
  two `ANOCRYPTO_C_2` one-shot GCM symbols are unchanged.
- `ANOCRYPTO_C_3` inherits `ANOCRYPTO_C_2` and contains only the seven streaming
  symbols.
- Current public surface is 16 headers and 79 exported symbols.
- The opaque context is caller-owned but its layout is private; callers obtain
  its size at runtime and must not serialize or persist it.
- Decrypt update output is unverified until decrypt final succeeds. Buffering,
  withholding, and failure wiping remain caller/product responsibilities.
- `SKT1` framing, packet policy, nonce composition, and orchestration remain
  AnoSecureKit-owned.


## P3-05 result

- `ac_kdf.h` is extended without adding another public header.
- Exactly one generic fixed-profile scrypt function is added.
- All 69 `ANOCRYPTO_C_0` symbols, the one `ANOCRYPTO_C_1` HKDF symbol, the two `ANOCRYPTO_C_2` one-shot GCM symbols, and the seven `ANOCRYPTO_C_3` streaming symbols are unchanged.
- `ANOCRYPTO_C_4` inherits `ANOCRYPTO_C_3` and contains only `ano_crypto_scrypt_derive_key`.
- Current public surface is 16 headers and 80 exported symbols.
- The fixed resource contract is `N=32768`, `r=8`, `p=1`, 32-byte output, and caller-provided 33,556,544-byte workspace.
- The production implementation performs no heap allocation and has no OpenSSL dependency.
- Exact `SKP1` v1 derivation/decryption is compatibility evidence only. File framing, password policy, prompting, storage, and orchestration remain AnoSecureKit-owned.
