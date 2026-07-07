# AnoCrypto-C Priority TODO

## Completed in P0

- Clean-room manifest and exclusion log skeleton
- C99 public core headers
- Module lifecycle and startup self-test skeleton
- Build/version identity API skeleton
- CMake build/install skeleton
- Initial unit and install smoke tests

## Added in P1 Foundation

- SHA-2 hash API skeleton
- HMAC API skeleton
- HMAC_DRBG/Hash_DRBG API skeleton
- CSP/key handle skeleton
- KAT manifest skeleton
- Negative service-gate test
- Design document skeletons

## Next P1 Work

1. Choose SHA-2/HMAC implementation strategy.
2. Add public-source KAT manifest entries with provenance.
3. Implement SHA-256 first, then SHA-384/SHA-512.
4. Implement HMAC on top of the selected hash interface.
5. Choose HMAC_DRBG or Hash_DRBG as the first DRBG candidate.
6. Add release evidence collection scripts.
7. Expand negative tests and install smoke tests.
8. Review ABI before any external PoC package.
