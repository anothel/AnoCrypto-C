# AnoCrypto-C

AnoCrypto-C is a private C99 cryptographic module candidate skeleton.

Current status: P0 skeleton. Cryptographic algorithms are not implemented yet.

## Build

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Install smoke test

```sh
cmake --install build --prefix /tmp/anocrypto-c-p0
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p0
cmake --build build-install-smoke
./build-install-smoke/anocrypto_c_install_smoke
```

## P0 scope

- Clean-room manifest and exclusion log
- C99 public header skeleton
- Stable status enum draft
- Module lifecycle/state machine skeleton
- Self-test framework skeleton
- Build/version identity API skeleton
- CMake build/install skeleton
- Unit and install smoke test skeleton
- Release evidence template

## Important limitations

- No cryptographic algorithm implementation is included in P0.
- Self-tests are framework placeholders only.
- Public KAT vectors are not imported yet.
- Thread-safety is not finalized.
- This repository does not claim completed validation or formal review.
