# AnoCrypto-C Release Evidence Template

This directory is a release evidence skeleton. It is not a completed release evidence package.

## Required Per-Candidate Artifacts

- Source archive SHA-256
- Binary SHA-256
- Git commit hash
- Compiler identity
- Build options
- Target OS/architecture
- Startup self-test log
- KAT log
- Negative test log
- Install smoke test log
- Known limitations
- Algorithm scope matrix
- Clean-room source manifest snapshot
- Exclusion log snapshot
- SBOM status
- Signing status

## P1 Foundation Evidence Notes

- Hash/HMAC/DRBG processing APIs are present but return `ANO_CRYPTO_ERR_NOT_IMPLEMENTED`.
- KAT manifest entries are placeholders only.
- `tests/negative/crypto_service_gate_tests.c` verifies that cryptographic services are rejected before module initialization and startup self-test success.
- Zeroization helper has a skeleton test; compiler/platform review is still TODO.

## Commands

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Install smoke:

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1
cmake --build build-install-smoke --parallel
./build-install-smoke/anocrypto_c_install_smoke
```

## TODO

- Add script to capture command output and SHA-256 hashes.
- Add KAT evidence only after public-source vectors are imported and recorded in the clean-room manifest.
- Add CI-produced evidence bundle layout.
