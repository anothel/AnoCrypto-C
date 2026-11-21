# P3 API and ABI Evolution Gate

Status: `P3_10_RESTRICTED_SHA1_ADDITIVE_ABI_APPROVED_IMPLEMENTED`

## Current public surface

| ABI line | Added symbols | Scope |
|---|---:|---|
| `ANOCRYPTO_C_0` | 69 | retained P1 surface |
| `ANOCRYPTO_C_1` | 1 | HKDF-SHA-256 |
| `ANOCRYPTO_C_2` | 2 | AES-256-GCM one-shot |
| `ANOCRYPTO_C_3` | 7 | AES-256-GCM streaming |
| `ANOCRYPTO_C_4` | 1 | fixed-profile scrypt |
| `ANOCRYPTO_C_5` | 3 | RSA import/sign/verify |
| `ANOCRYPTO_C_6` | 1 | P-256 public/private component import |
| `ANOCRYPTO_C_7` | 1 | restricted OCSP CertID SHA-1 digest |
| total | 85 | current exported symbols |

Public headers: 18. Compatibility baseline: P1-29. Every ABI-0 through ABI-6
name and assignment remains unchanged.

## P3-10 approval

P3-10 adds only `ano_crypto_ocsp_certid_sha1_digest`, declared in
`include/anocrypto/ac_ocsp_certid_hash.h`.
The service is one-shot, emits exactly 20 bytes, and accepts only caller-selected
bytes for OCSP CertID issuerNameHash or issuerKeyHash compatibility. It does not
add SHA-1 to the generic hash enum and does not authorize SHA-1 streaming,
signatures, HMAC, KDF, password processing, or PKI workflows.

## Next gate

P3-11 is a downstream adapter parity checkpoint and may not add another
AnoCrypto-C ABI line without a separately approved primitive requirement.
