# AnoCrypto-C

AnoCrypto-C is a private C99 cryptographic module candidate skeleton.

Current status: P1-5 foundation with SHA-256, HMAC-SHA-256, HMAC_DRBG-SHA-256, expanded KATs, negative tests, internal entropy adapter seam, HMAC key handle lifecycle slice, stale-handle/exhaustion tests, and release-evidence draft tooling.

Implemented processing scope:

- SHA-256 one-shot and streaming API
- HMAC-SHA-256 one-shot and streaming API
- HMAC-SHA-256 compute using an in-memory HMAC key handle
- HMAC_DRBG-SHA-256 deterministic core API
- Startup self-test KATs for SHA-256, HMAC-SHA-256, and HMAC_DRBG-SHA-256
- Extended KAT executables for SHA-256, HMAC-SHA-256, HMAC key handle path, and HMAC_DRBG-SHA-256 public vectors
- Negative API tests for hash/HMAC/DRBG/CSP/key argument handling, context lifecycle behavior, handle exhaustion, and stale handle rejection
- Internal entropy adapter seam with callback and OS-reserved provider tests
- Release evidence draft collection with source archive hash and install smoke logs

Skeleton-only / TODO scope:

- SHA-384/SHA-512 ABI placeholders
- HMAC-SHA-384/HMAC-SHA-512 ABI placeholders
- Hash_DRBG-SHA-256 ABI placeholder
- Reviewed OS entropy provider and public random service layer
- Persistent keystore, key export, key wrapping, and multi-process CSP model
- Thread-safety policy for module and CSP global state

## Build

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Install smoke test

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1-5
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-5
cmake --build build-install-smoke
./build-install-smoke/anocrypto_c_install_smoke
```

## Evidence draft collection

```sh
cmake --build build --target ano_crypto_collect_release_evidence
```

The generated evidence directory is a local engineering draft. It is not official validation evidence and should be reviewed before sharing.

Generated files include `SOURCE_ARCHIVE.sha256`, `BINARY_FILES.sha256`, `ctest.log`, and installed-package smoke logs when the default P1-5 target is used.

## P1-5 scope

- Fresh C99 SHA-256 implementation retained from P1-1
- Fresh C99 HMAC-SHA-256 implementation retained from P1-1
- Fresh C99 HMAC_DRBG-SHA-256 deterministic core implementation retained from P1-3
- Internal entropy adapter seam and HMAC key handle lifecycle retained from P1-4
- Public-source KAT fixtures recorded in `cleanroom/SOURCE_MANIFEST.md`
- Extended public KAT coverage for SHA-256 long-message/million-`a` vectors and RFC 4231 HMAC-SHA-256 cases
- HMAC key handle KAT path using RFC 4231 Test Case 2 through the CSP handle lifecycle
- HMAC_DRBG-SHA-256 public CAVP vector flow covering instantiate, reseed, and two generate calls
- Chunk-boundary API consistency tests for one-shot versus streaming behavior
- Startup self-test includes SHA-256, HMAC-SHA-256, and HMAC_DRBG-SHA-256 paths
- CSP handle table exhaustion and stale-handle reuse tests
- Source archive SHA-256 and install smoke log capture in the evidence script

## Important limitations

- This is not an official validation result.
- SHA-384, SHA-512, Hash_DRBG, block cipher APIs, OS entropy provider, and public random service APIs are not implemented yet.
- KAT pass results are correctness checks only and do not replace formal validation.
- HMAC_DRBG-SHA-256 accepts explicit entropy input; production entropy source policy remains TODO.
- The entropy callback seam is test/internal plumbing and is not an operational entropy source.
- HMAC key handles are in-memory only; export, persistence, keystore integration, and thread-safety are not finalized.
- Prediction-resistance requests are rejected until the entropy adapter is connected to reviewed providers.
- Error recovery policy after fatal state remains TODO.
- This repository does not claim official validation or external review completion.
