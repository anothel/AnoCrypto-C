# AnoSecureKit Requirement Trace

## Authority

- Community release source: `694459ebe497d15ba75ef76a52fa7c36ddd7bcce`.
- Enterprise source: `3a62318fce27985d091bb5445fc59d0fca7033a2`.
- Dependency identity conflict: see `DOWNSTREAM_SOURCE_CONFLICTS.md`.

## Trace

| Capability | Contract/source | Positive/compatibility evidence | AnoCrypto-C result |
|---|---|---|---|
| SHA-256 | `src/backend/crypto_backend.hpp`, `src/hash.cpp` | hash/public-header/CLI tests | present |
| HMAC-SHA-256 | provider interface | HMAC and CLI tests | present |
| random/generate key | `src/random.cpp` | random and consumer tests | present |
| HKDF-SHA-256 | public `hash.hpp`, `src/file_crypto.cpp` | RFC 5869 installed CLI vector; exact `SKF1` fixture | present; P3-02 compatibility verified |
| AES-256-GCM one-shot | provider interface, `file_crypto.cpp` | exact `SKT1`/`SKF1`/`SKP1` v1 inputs and outputs plus corruption cases | present; P3-03 compatibility verified |
| AES-256-GCM streaming | `packet_stream.cpp` | packet split/finalize and `SKT1` fixtures | gap approved |
| scrypt fixed profile | `file_crypto.cpp`, format docs | `SKP1` positive/negative fixtures | gap approved |

The Enterprise adapter already implements lifecycle, identity, SHA-256,
HMAC-SHA-256 and random through public AnoCrypto-C APIs. Missing operations fail
closed and do not fall back to OpenSSL.
