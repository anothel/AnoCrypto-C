# P3-04 C99 AES-256-GCM Streaming API Gate

Status: `REVIEWED_IMPLEMENTED_LOCAL_EVIDENCE`

## Public API decision

P3-04 extends the existing `ac_aead.h`; no new public header is introduced.
The minimal caller-owned opaque-context surface is exactly seven functions:

1. `ano_crypto_aes256_gcm_stream_context_size`
2. `ano_crypto_aes256_gcm_stream_init`
3. `ano_crypto_aes256_gcm_stream_aad_update`
4. `ano_crypto_aes256_gcm_stream_update`
5. `ano_crypto_aes256_gcm_stream_encrypt_final`
6. `ano_crypto_aes256_gcm_stream_decrypt_final`
7. `ano_crypto_aes256_gcm_stream_uninit`

The context direction is selected at initialization. Separate encrypt and decrypt
operations require separate context instances. The fixed profile remains a
32-byte BLOCK_CIPHER CSP key, 12-byte nonce and 16-byte tag. Initialization
captures an operation-local expanded-key snapshot from a live CSP handle. Later
handle destruction blocks new initialization but does not revoke an already
initialized context; finalization or uninit clears that private snapshot.

The opaque extent must be allocated with `malloc`, `calloc`, `realloc`, or
equivalent storage aligned for any C object. A raw `uint8_t` array is not a
portable context allocation unless the caller independently guarantees the
required alignment. Misaligned storage is rejected before internal access.

## State and output contract

- initialization enters the AAD phase;
- zero-length AAD and payload updates are no-ops and do not close the AAD phase;
- the first non-empty payload update permanently closes the AAD phase;
- AAD after payload is rejected without consuming a valid context;
- payload output length equals input length; exact in-place operation is allowed;
- `out_written` must be disjoint from the context, payload input and payload
  output; structurally valid accepted outputs are cleared and report zero on
  update failure;
- decrypt update output is explicitly unverified until decrypt final succeeds;
- callers must withhold/buffer that output and wipe or discard it on final failure;
- a correct-direction finalization attempt consumes and clears the context on
  success or failure; repeated operations are rejected;
- explicit uninit is allowed outside OPERATIONAL and clears the opaque extent.

Preflight buffer/state failures do not consume a valid context except a
correct-direction finalization attempt, which is terminal. Internal operation
failure invalidates and clears the context. Contexts are caller synchronized and
must not be copied, serialized or persisted.

## ABI decision

- all 69 `ANOCRYPTO_C_0` symbols remain unchanged;
- the HKDF symbol remains the only `ANOCRYPTO_C_1` symbol;
- the two one-shot GCM symbols remain the only `ANOCRYPTO_C_2` symbols;
- the seven streaming functions are the only `ANOCRYPTO_C_3` symbols;
- current surface: 16 public headers and 79 exported symbols.

## Product boundary

This API owns only generic AES-256-GCM streaming mechanics. `SKT1` framing,
header construction, nonce generation, caller-AAD ordering, buffering/rollback,
packet parsing and public product errors remain AnoSecureKit-owned.
