# P3-03 C99 AES-256-GCM API Gate

Status: `APPROVED_AND_IMPLEMENTED`

## Reviewed public surface

One header and two functions are sufficient:

- `include/anocrypto/ac_aead.h`
- `ano_crypto_aes256_gcm_encrypt`
- `ano_crypto_aes256_gcm_decrypt`

The API uses an existing purpose-separated `BLOCK_CIPHER` CSP handle. Only a
32-byte key, 12-byte nonce and 16-byte tag are accepted. AAD and payload may be
empty. Ciphertext length is exactly plaintext length.

## Output and buffer rules

Exact in-place payload operation is allowed. Partial payload overlap is rejected.
Writable output may not overlap nonce, AAD or tag. Decryption computes and checks
the authentication tag before any plaintext write. Authentication or any later
failure clears the accepted operation output range and reports zero bytes.

## Ownership and exclusions

This is a generic one-shot primitive. It does not implement `SKT1`, `SKF1`,
`SKP1`, chunk/record framing, nonce composition, streaming, scrypt, key wrapping,
or product orchestration. Those remain downstream-owned.

## ABI result

- retained `ANOCRYPTO_C_0`: 69 symbols unchanged
- retained `ANOCRYPTO_C_1`: one HKDF symbol unchanged
- additive `ANOCRYPTO_C_2`: two AES-256-GCM symbols
- current total: 16 headers and 72 symbols
