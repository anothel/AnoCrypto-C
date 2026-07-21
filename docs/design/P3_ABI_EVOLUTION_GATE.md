# P3 API and ABI Evolution Gate

Status: `P3_08_RSA_ADDITIVE_ABI_APPROVED_IMPLEMENTED`

## Current public surface

| ABI line | Added symbols | Scope |
|---|---:|---|
| `ANOCRYPTO_C_0` | 69 | retained P1 surface |
| `ANOCRYPTO_C_1` | 1 | HKDF-SHA-256 |
| `ANOCRYPTO_C_2` | 2 | AES-256-GCM one-shot |
| `ANOCRYPTO_C_3` | 7 | AES-256-GCM streaming |
| `ANOCRYPTO_C_4` | 1 | fixed-profile scrypt |
| `ANOCRYPTO_C_5` | 3 | RSA import/sign/verify |
| total | 83 | current exported symbols |

Public headers: 17. Compatibility baseline: P1-29. Every ABI-0 through ABI-4
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

## Next gate

P3-09 may evaluate P-256 import and reuse of generic sign/verify. It must not
change the three P3-08 symbols or infer that another ABI line is required.
ECDSA representation, nonce policy, validation, exact symbol count, and ABI
impact require separate review.
