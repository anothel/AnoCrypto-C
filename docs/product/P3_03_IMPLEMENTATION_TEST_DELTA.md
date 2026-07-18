# P3-03 Implementation and Test Delta

Version: `0.1.0-p3-03-aes256-gcm-oneshot`

## Added

- minimal public one-shot AES-256-GCM C API
- internal GCM counter, GHASH and authenticated-output-only decrypt path using
  the existing AES block core and CSP handle model
- additive `ANOCRYPTO_C_2` symbol version line
- NIST KAT, empty/AAD/in-place, exact AnoSecureKit fixture, negative and
  OpenSSL test-only differential coverage
- current/retained consumer, install, PoC, public-surface and release evidence
  integration

## Preserved

- all 69 `ANOCRYPTO_C_0` symbols
- the one `ANOCRYPTO_C_1` HKDF symbol
- P1-29 source/ABI consumer baseline
- KCMVP `DEFERRED`, hosted `NOT_RUN/UNREVIEWED`, repository `UNSIGNED`, external
  release authorization `NO`
- downstream Community identity conflict remains open

## Excluded

AES-GCM streaming, scrypt, SHA-1, RSA, ECDSA, high-level product APIs, format
parsing, push, tag, signing, timestamping and external publication.
