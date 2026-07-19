# AnoSecureKit Requirement Trace

## Authority

- Community release source: `694459ebe497d15ba75ef76a52fa7c36ddd7bcce`.
- Enterprise intake source: `3a62318fce27985d091bb5445fc59d0fca7033a2`.
- P3-06 Enterprise parity checkpoint: `53824f00ff3460ba05ac866d1dd58d9a0c01ff3c`.
- Current content/provenance disposition: see `DOWNSTREAM_SOURCE_CONFLICTS.md`.

## Trace

| Capability | Contract/source | Positive/compatibility evidence | AnoCrypto-C / Enterprise result |
|---|---|---|---|
| SHA-256 | `src/backend/crypto_backend.hpp`, `src/hash.cpp` | hash/public-header/CLI tests | core present; connected P3-06 |
| HMAC-SHA-256 | provider interface | HMAC and CLI tests | core present; connected P3-06 |
| random/generate key | `src/random.cpp` | random and consumer tests | core present; connected P3-06 |
| HKDF-SHA-256 | public `hash.hpp`, `src/file_crypto.cpp` | RFC 5869; exact `SKF1` fixture | P3-02; connected P3-06 |
| AES-256-GCM one-shot | provider interface, `file_crypto.cpp` | exact `SKT1`/`SKF1`/`SKP1` inputs/outputs and corruption cases | P3-03; connected P3-06 |
| AES-256-GCM streaming | `packet_stream.cpp` | packet split/finalize and `SKT1` fixtures | P3-04; connected P3-06 |
| scrypt fixed profile | `file_crypto.cpp`, format docs | exact `SKP1` derived-key/decryption fixture and resource tests | P3-05; connected P3-06 |
| capability mask/no fallback | Enterprise runtime contract | seven-bit mask, profile readiness, Linux dependency inspection | P3-06 engineering checkpoint complete |

The P3-06 Enterprise adapter uses only public AnoCrypto-C APIs for the seven
listed capabilities. Unsupported service routing and silent OpenSSL fallback
are prohibited. `SKT1`, `SKF1`, `SKP1`, nonce/AAD composition, file handling,
transactional output, and product workflow remain AnoSecureKit-owned.

P3-06 does not provide hosted Windows/macOS results, trusted release provenance,
production signing, timestamping, KCMVP validation, or external release
authorization.
