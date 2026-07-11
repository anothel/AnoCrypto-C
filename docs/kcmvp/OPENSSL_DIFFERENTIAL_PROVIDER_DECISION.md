# OpenSSL Differential Provider Decision

Status: P1-23 engineering provider decision

## Decision

AnoCrypto-C selects OpenSSL 3.x `libcrypto` as the first independent,
test-only differential provider for the implemented SHA-2 and HMAC slice.
The provider is used through OpenSSL EVP interfaces by
`tests/differential/sha2_hmac_openssl_differential_tests.c`.

Compared services:

- SHA-256, SHA-384, and SHA-512;
- HMAC-SHA-256, HMAC-SHA-384, and HMAC-SHA-512;
- AnoCrypto-C one-shot and streaming paths against one OpenSSL result;
- deterministic padding, block, long-key, multi-block, and unaligned-input
  cases.

## Provenance and license boundary

- Provider source: the public OpenSSL project and its published source
  releases.
- Minimum selected provider family: OpenSSL 3.0.
- OpenSSL 3.0 and later are published under Apache License 2.0 according to the
  OpenSSL project's license and release pages.
- No OpenSSL implementation source, constants, test harness code, or output
  values are copied into AnoCrypto-C.
- The test includes only public OpenSSL headers and links to `OpenSSL::Crypto`
  in a test executable.
- A downstream distributor must still review the exact OpenSSL package and
  license files supplied by its operating system or toolchain.

Official references:

- `https://openssl-library.org/source/license/`
- `https://openssl-library.org/source/`
- `https://docs.openssl.org/3.0/man3/EVP_DigestInit/`
- `https://docs.openssl.org/3.4/man7/EVP_MAC-HMAC/`

## Build and product boundary

The OpenSSL dependency must remain outside the AnoCrypto-C product target.

- `anocrypto_c` does not link to OpenSSL.
- Installed public headers do not include OpenSSL headers.
- Clean Release and customer PoC SDK builds disable tests and therefore do not
  discover or link the differential provider.
- The stored ABI-0 public symbol list remains unchanged.
- `ANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS` controls optional local test
  discovery.
- `ANO_CRYPTO_REQUIRE_OPENSSL_DIFFERENTIAL_PROVIDER` is reserved for the
  dedicated CI/evidence lane and fails configuration if OpenSSL 3.x is absent.

## Result normalization

A differential run records only:

- pass/fail status;
- OpenSSL provider version string;
- number of in-memory comparisons;
- algorithm/path/input lengths only when triaging a mismatch.

Digest bytes, MAC bytes, generated deterministic keys, and generated messages
are not written to logs or evidence.

## Failure triage

A mismatch must not be resolved by automatically changing either
implementation or replacing the stored KAT vectors. Review in this order:

1. reproduce with the exact provider version and compiler identity;
2. confirm the matching public KAT and module startup self-test;
3. isolate one-shot versus streaming behavior;
4. isolate padding/block/key-length and alignment conditions;
5. inspect API length/output handling and provider return codes;
6. record the issue without output bytes;
7. require source review before changing implementation or evidence.

## Limitations

- Provider agreement is not proof that either implementation is free of the
  same defect.
- This lane is not a replacement for public KATs, formal algorithm testing,
  side-channel review, or KCMVP evaluation.
- Provider versions and operating-system packaging can differ across runners.
- OpenSSL is not an AnoCrypto-C runtime dependency or customer-facing provider.
