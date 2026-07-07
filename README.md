# AnoCrypto-C

AnoCrypto-C is a private C99 cryptographic module candidate skeleton.

Current status: P1 foundation skeleton. Cryptographic algorithm processing is not implemented yet.

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

## P1 foundation scope

- Clean-room manifest and exclusion log updates
- SHA-2 hash API skeleton
- HMAC API skeleton
- HMAC_DRBG/Hash_DRBG API skeleton
- CSP/key handle skeleton
- KAT manifest skeleton without vector data
- Negative service-gate tests
- Design document skeletons for hash/HMAC, DRBG, CSP, and self-test

## Important limitations

- No cryptographic algorithm processing implementation is included yet.
- KAT manifest entries are placeholders only.
- Public KAT vectors are not imported yet.
- Thread-safety is not finalized.
- Error recovery policy after fatal state remains TODO.
- This repository does not claim official validation or external review completion.
