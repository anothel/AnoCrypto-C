# AnoSecureKit AES-256-GCM Compatibility Evidence

Status: `EXACT_FIXTURE_COMPATIBILITY_VERIFIED_LOCAL`

## Exact downstream baselines

- AnoSecureKit Community: `694459ebe497d15ba75ef76a52fa7c36ddd7bcce`
- AnoSecureKit Enterprise: `3a62318fce27985d091bb5445fc59d0fca7033a2`
- dependency identity conflict: `OPEN_DOWNSTREAM_SYNC_DECISION`

## Verified primitive mapping

The P3-03 tests reproduce exact downstream v1 inputs while keeping all format
logic in test code:

- `SKT1`: header plus caller AAD, fixed 12-byte nonce and 16-byte tag
- `SKF1`: exact v1 file fixture, HKDF-derived 32-byte file key, chunk nonce and
  complete header/record/caller-AAD bytes
- `SKP1`: exact v1 GCM chunk fixture using the pinned downstream-derived key;
  no scrypt implementation is introduced by this slice

The generated ciphertext and tag match the exact fixture bytes and decrypt back
to the expected plaintext. Corrupt key, nonce, AAD, ciphertext or tag returns an
authentication error and no plaintext.

## Boundary

AnoCrypto-C does not parse or serialize any AnoSecureKit format, select chunk
indices, build AAD, derive passwords, or execute a product workflow. The tests
are compatibility evidence for the generic primitive only.
