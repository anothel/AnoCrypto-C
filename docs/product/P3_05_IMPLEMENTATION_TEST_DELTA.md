# P3-05 Implementation and Test Delta

Version: `0.1.0-p3-05-scrypt-fixed-profile`

## Added

- one fixed-profile generic scrypt public function in `ac_kdf.h`;
- caller-owned 33,556,544-byte workspace and 64 MiB resource gate;
- production C99 PBKDF2-HMAC-SHA-256, Salsa20/8, BlockMix and ROMix implementation;
- additive `ANOCRYPTO_C_4` with one symbol;
- fixed-profile KAT, negative/resource/zeroization, OpenSSL test-only differential, and exact downstream compatibility evidence.

## Explicitly not added

No product file format, password workflow, tunable scrypt cost API, SHA-1, RSA, ECDSA, public-key architecture, signing, hosted claim, or external release authorization.
