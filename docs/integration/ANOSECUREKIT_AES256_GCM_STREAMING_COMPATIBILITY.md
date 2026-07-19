# AnoSecureKit AES-256-GCM Streaming Compatibility Evidence

Status: `SKT1_EXACT_STREAMING_COMPATIBILITY_VERIFIED_LOCAL`

Exact baselines:

- AnoSecureKit Community: `694459ebe497d15ba75ef76a52fa7c36ddd7bcce`
- AnoSecureKit Enterprise: `3a62318fce27985d091bb5445fc59d0fca7033a2`
- dependency identity conflict: `OPEN_DOWNSTREAM_SYNC_DECISION`

The P3-04 test-only compatibility case reproduces the exact `SKT1` v1 primitive
inputs without placing format logic in production code:

- nonce: the 12 bytes from the packet prefix;
- AAD update order: the 5-byte `SKT1` header, then caller AAD;
- plaintext/ciphertext split across multiple irregular updates;
- final 16-byte tag.

Streaming encryption matches the exact known fixture ciphertext/tag. Streaming
decryption and the retained one-shot API cross-consume each other's output.
Wrong key, AAD or tag fails at finalization. Decrypt update bytes remain
unverified and are not released by this evidence harness before final succeeds.

AnoCrypto-C does not parse, serialize, generate or validate `SKT1` packets and
does not implement packet-level buffering, rollback or workflow behavior.
