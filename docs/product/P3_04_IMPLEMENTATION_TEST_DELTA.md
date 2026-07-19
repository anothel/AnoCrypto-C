# P3-04 Implementation and Test Delta

Version: `0.1.0-p3-04-aes256-gcm-streaming`
Status: `LOCAL_ENGINEERING_COMPLETE_EXTERNAL_RELEASE_NOT_AUTHORIZED`

## Added

- seven-function caller-owned opaque AES-256-GCM streaming API in `ac_aead.h`;
- `malloc`-aligned opaque storage contract with explicit misalignment rejection
  before internal context access;
- internal chunk-boundary independent CTR/GHASH state;
- explicit AAD/payload/final state machine;
- additive `ANOCRYPTO_C_3` symbol-version line;
- NIST chunk-split, state/negative, OpenSSL differential and exact `SKT1` tests;
- negative coverage for ordering, output clearing, overlap, context alignment,
  disjoint `out_written`, short/null tags, authentication failure and shutdown;
- CSP snapshot-lifetime coverage proving an initialized context may finish after
  handle destruction while stale-handle reinitialization is rejected;
- current/retained consumer, install, PoC and evidence updates.

## Preserved

- all 69 `ANOCRYPTO_C_0` symbols;
- HKDF on `ANOCRYPTO_C_1`;
- one-shot AES-256-GCM on `ANOCRYPTO_C_2`;
- P1-29 compatibility baseline;
- AnoSecureKit ownership of `SKT1`/`SKF1`/`SKP1` and workflows;
- KCMVP `DEFERRED`, hosted `NOT_RUN/UNREVIEWED`, repository `UNSIGNED`,
  external authorization `NO`, and downstream identity conflict open.

## Excluded

No scrypt, SHA-1, RSA, ECDSA, public-key architecture, generic nonce/tag
profiles, product packet API, push, tag, signing, timestamp or publication.
