# AnoCrypto-C

AnoCrypto-C is a private C99 cryptographic module candidate skeleton.

Current status: P1-1 foundation with a small reviewed algorithm slice.

Implemented processing scope:

- SHA-256 one-shot and streaming API
- HMAC-SHA-256 one-shot and streaming API
- Startup self-test KATs for SHA-256 and HMAC-SHA-256

Skeleton-only scope:

- SHA-384/SHA-512 ABI placeholders
- HMAC-SHA-384/HMAC-SHA-512 ABI placeholders
- HMAC_DRBG/Hash_DRBG interface skeleton
- CSP/key handle lifecycle skeleton

## Build

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Install smoke test

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1
cmake --build build-install-smoke
./build-install-smoke/anocrypto_c_install_smoke
```

## P1-1 scope

- Fresh C99 SHA-256 implementation
- Fresh C99 HMAC-SHA-256 implementation
- Public-source KAT fixtures recorded in `cleanroom/SOURCE_MANIFEST.md`
- KAT test executable for SHA-256 and HMAC-SHA-256
- Startup self-test uses internal KAT calls without opening the external crypto service gate
- Negative service-gate tests remain in place

## Important limitations

- This is not an official validation result.
- SHA-384, SHA-512, DRBG, CSP/key import, and block cipher APIs are not implemented yet.
- KAT pass results are correctness checks only and do not replace formal validation.
- Thread-safety is not finalized.
- Error recovery policy after fatal state remains TODO.
- This repository does not claim official validation or external review completion.
