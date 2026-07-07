# AnoCrypto-C

AnoCrypto-C is a private C99 cryptographic module candidate skeleton.

Current status: P1-3 foundation with SHA-256, HMAC-SHA-256, HMAC_DRBG-SHA-256, expanded KATs, negative tests, and evidence tooling.

Implemented processing scope:

- SHA-256 one-shot and streaming API
- HMAC-SHA-256 one-shot and streaming API
- HMAC_DRBG-SHA-256 deterministic core API
- Startup self-test KATs for SHA-256, HMAC-SHA-256, and HMAC_DRBG-SHA-256
- Extended KAT executables for SHA-256, HMAC-SHA-256, and HMAC_DRBG-SHA-256 public vectors
- Negative API tests for hash/HMAC/DRBG argument handling and context lifecycle behavior

Skeleton-only scope:

- SHA-384/SHA-512 ABI placeholders
- HMAC-SHA-384/HMAC-SHA-512 ABI placeholders
- Hash_DRBG-SHA-256 ABI placeholder
- OS entropy adapter / random service layer
- CSP/key handle lifecycle skeleton

## Build

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Install smoke test

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1-3
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-3
cmake --build build-install-smoke
./build-install-smoke/anocrypto_c_install_smoke
```

## Evidence draft collection

```sh
cmake --build build --target ano_crypto_collect_release_evidence
```

The generated evidence directory is a local engineering draft. It is not official validation evidence and should be reviewed before sharing.

## P1-3 scope

- Fresh C99 SHA-256 implementation retained from P1-1
- Fresh C99 HMAC-SHA-256 implementation retained from P1-1
- Fresh C99 HMAC_DRBG-SHA-256 deterministic core implementation added in P1-3
- Public-source KAT fixtures recorded in `cleanroom/SOURCE_MANIFEST.md`
- Extended public KAT coverage for SHA-256 long-message/million-`a` vectors and RFC 4231 HMAC-SHA-256 cases
- HMAC_DRBG-SHA-256 public CAVP vector flow covering instantiate, reseed, and two generate calls
- Chunk-boundary API consistency tests for one-shot versus streaming behavior
- Startup self-test includes SHA-256, HMAC-SHA-256, and HMAC_DRBG-SHA-256 paths
- Release evidence collection script skeleton

## Important limitations

- This is not an official validation result.
- SHA-384, SHA-512, Hash_DRBG, CSP/key import, block cipher APIs, and OS entropy adapter are not implemented yet.
- KAT pass results are correctness checks only and do not replace formal validation.
- HMAC_DRBG-SHA-256 accepts explicit entropy input; production entropy source policy remains TODO.
- Prediction-resistance requests are rejected until the entropy adapter exists.
- Thread-safety is not finalized.
- Error recovery policy after fatal state remains TODO.
- This repository does not claim official validation or external review completion.
