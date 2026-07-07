# AnoCrypto-C Priority TODO

## Completed in P1-1

- Fresh C99 SHA-256 implementation.
- Fresh C99 HMAC-SHA-256 implementation.
- Public-source SHA-256/HMAC-SHA-256 KATs in startup self-test.
- KAT test executable for one-shot and streaming APIs.
- Service-gate negative tests updated to reflect implemented SHA-256/HMAC-SHA-256.

## P1-2 Immediate TODO

1. Add expanded public SHA-256/HMAC-SHA-256 vector corpus.
2. Record vector source date and local file SHA-256 checksums.
3. Add chunk-boundary and long-message streaming tests.
4. Add HMAC long-key tests.
5. Add differential validation workflow against an independently selected tool.
6. Decide SHA-384/SHA-512 implementation timing.
7. Start HMAC_DRBG-SHA-256 implementation design.
8. Add CSP/key handle lifecycle implementation for HMAC keys.
9. Add release evidence collection script.
10. Add CI matrix.

## Still not included

- DRBG implementation.
- CSP/key handle implementation.
- SHA-384/SHA-512 processing.
- Block cipher or AEAD processing.
- Public-key algorithms.
- Official validation evidence.
