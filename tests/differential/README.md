# Differential tests

Status: P1-23 engineering correctness checks.

`sha2_hmac_openssl_differential_tests.c` compares the public AnoCrypto-C SHA-256,
SHA-384, SHA-512, HMAC-SHA-256, HMAC-SHA-384, and HMAC-SHA-512 services with a
test-only OpenSSL 3.x `libcrypto` provider.

## Boundary

- OpenSSL is linked only to the differential test executable.
- `anocrypto_c`, installed SDKs, and customer PoC packages do not link to or
  install OpenSSL through this test lane.
- OpenSSL 3.x is located through CMake `OpenSSL::Crypto`.
- The dedicated CI lane enables
  `ANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER=ON`, so provider absence is
  a configuration failure there.
- Ordinary test builds may skip the differential executable when OpenSSL 3.x
  development files are unavailable.

## Corpus

The test creates deterministic, local, non-secret messages and HMAC keys at
runtime. Lengths cover empty input, SHA-256 and SHA-512-family padding edges,
block edges, multi-block inputs, long HMAC keys, unaligned input addresses, and
a one-megabyte hash input. AnoCrypto-C one-shot and streaming outputs are
compared in memory with OpenSSL results.

No digest, MAC, key, entropy, seed, DRBG state, or generated random value is
printed or written to evidence. A passing run reports only provider identity,
provider version, comparison count, and status. A failure reports algorithm,
path, and input lengths without output bytes.

This is engineering differential evidence only. It is separate from startup
self-tests and public KAT provenance and is not official validation evidence.
