# P3 API and ABI Evolution Gate

Status: `P3_09_ECDSA_ADDITIVE_ABI_APPROVED_IMPLEMENTED`

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
| total | 84 | current exported symbols |

Public headers: 17. Compatibility baseline: P1-29. Every ABI-0 through ABI-5
name and assignment remains unchanged.

## P3-08 approval

The independently reviewed minimum RSA surface is:

1. `ano_crypto_rsa_key_import` in `ac_key.h`;
2. `ano_crypto_signature_sign` in new `ac_signature.h`;
3. `ano_crypto_signature_verify` in new `ac_signature.h`.

P3-08 freezes RSA-2048/e=65537, SHA-256/384/512, message-based input,
core-owned DigestInfo/EMSA-PKCS1-v1_5, 256-byte signatures, bounded CSP
storage, role binding, size-query/failure clearing, operation leases, CRT
exponent blinding, and public fault checking.

No public metadata query, raw RSA, key generation, key export, persistence,
provider routing, PKI codec, or format/workflow API is approved.

## P3-09 approval

P3-09 adds only `ano_crypto_ec_p256_key_import` and reuses the P3-08 generic
sign/verify symbols. It freezes 32-byte affine coordinates/scalar, P-256 with
SHA-256, RFC 6979 HMAC-SHA-256 nonce derivation, and 64-byte raw `r || s`. DER
conversion remains AnoPKI-owned.

## Next gate

P3-10 may evaluate restricted SHA-1 interoperability for OCSP CertID only. No
function, enum, symbol count, or ABI line is pre-approved.
