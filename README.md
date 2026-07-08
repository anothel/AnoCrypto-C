# AnoCrypto-C

AnoCrypto-C is a private C99 cryptographic module candidate skeleton. The repository slug may remain `anocrypto-c`, but the product/brand spelling is `AnoCrypto-C`.

Current status: P1-10 foundation with SHA-256, HMAC-SHA-256, HMAC_DRBG-SHA-256, expanded KATs, negative tests, internal platform entropy provider seam, internal DRBG instantiate/reseed helper, HMAC_DRBG boundary vectors, CMake package naming normalization, HMAC key handle lifecycle slice, stale-handle/exhaustion tests, limited internal global-state locking, reproducible-build notes, CI matrix skeleton, API/ABI review checklist, public-surface policy check, and release-evidence draft tooling.

Implemented processing scope:

- SHA-256 one-shot and streaming API
- HMAC-SHA-256 one-shot and streaming API
- HMAC-SHA-256 compute using an in-memory HMAC key handle
- HMAC_DRBG-SHA-256 deterministic core API
- Internal HMAC_DRBG-SHA-256 instantiate/reseed helper driven by entropy source
- Startup self-test KATs for SHA-256, HMAC-SHA-256, and HMAC_DRBG-SHA-256
- Extended KAT executables for SHA-256, HMAC-SHA-256, HMAC key handle path, and HMAC_DRBG-SHA-256 public vectors
- Negative API tests for hash/HMAC/DRBG/CSP/key argument handling, context lifecycle behavior, handle exhaustion, stale handle rejection, and DRBG helper failure cleanup
- Internal entropy adapter seam with callback and platform OS provider tests
- Release evidence draft collection with source archive hash, public-surface check, reproducible-build notes, and install smoke logs
- Limited internal locking for module/self-test/CSP global state
- CI matrix skeleton for Linux, macOS, and Windows build/test/install-smoke paths
- API/ABI review checklist for public headers, package identity, and symbol-surface TODOs
- Release-style internal symbol hiding option for supported compilers

Skeleton-only / TODO scope:

- SHA-384/SHA-512 ABI placeholders
- HMAC-SHA-384/HMAC-SHA-512 ABI placeholders
- Hash_DRBG-SHA-256 ABI placeholder
- Public random service layer and reviewed customer/platform entropy policy
- Persistent keystore, key export, key wrapping, and multi-process CSP model
- Public per-context concurrency guarantees and reviewed platform CI coverage
- Byte-identical source archive and binary reproducibility rules
- SBOM/signing workflow

## Build

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Install smoke test

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1-10
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-10
cmake --build build-install-smoke
./build-install-smoke/anocrypto_c_install_smoke
```

## Evidence draft collection

```sh
cmake --build build --target ano_crypto_collect_release_evidence
```

The generated evidence directory is a local engineering draft. It is not official validation evidence and should be reviewed before sharing.

Generated files include `SOURCE_ARCHIVE.sha256`, `BINARY_FILES.sha256`, `PUBLIC_SURFACE_CHECK.txt`, `REPRODUCIBLE_BUILD_NOTES.txt`, `ctest.log`, and installed-package smoke logs when the default P1-10 target is used.

## P1-10 scope

- Fresh C99 SHA-256 implementation retained from P1-1
- Fresh C99 HMAC-SHA-256 implementation retained from P1-1
- Fresh C99 HMAC_DRBG-SHA-256 deterministic core implementation retained from P1-3
- Internal entropy adapter seam and HMAC key handle lifecycle retained from P1-4
- Internal platform entropy provider seam retained from P1-7
- Internal HMAC_DRBG-SHA-256 instantiate/reseed helper added for future random-service review
- HMAC_DRBG-SHA-256 boundary vectors for personalization, additional input, reseed-after-generate, max request, and oversized request rejection
- CMake package naming normalized to `AnoCryptoC` / `AnoCryptoC::AnoCryptoC` while keeping the library file name `anocrypto_c` and include root `include/anocrypto/`
- Public-source KAT fixtures recorded in `cleanroom/SOURCE_MANIFEST.md`
- Extended public KAT coverage for SHA-256 long-message/million-`a` vectors and RFC 4231 HMAC-SHA-256 cases
- HMAC key handle KAT path using RFC 4231 Test Case 2 through the CSP handle lifecycle
- HMAC_DRBG-SHA-256 public vector flow covering instantiate, reseed, and two generate calls
- Chunk-boundary API consistency tests for one-shot versus streaming behavior
- Startup self-test includes SHA-256, HMAC-SHA-256, and HMAC_DRBG-SHA-256 paths
- CSP handle table exhaustion and stale-handle reuse tests
- Limited module/self-test/CSP global-state lock seam and POSIX concurrent CSP/HMAC test
- Platform entropy provider smoke test that records provider name/status only, not entropy bytes
- Source archive SHA-256 and install smoke log capture in the evidence script
- Reproducible-build notes and evidence fields added for P1-10
- CI matrix skeleton added for Linux, macOS, and Windows
- API/ABI review checklist added, including the current public build-identity struct TODO
- Public surface policy check target added for naming, header, package, claims-boundary, and Linux exported-symbol scans
- `ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS` added for release-style builds on supported compilers; internal test-hook builds keep internals linkable for internal-unit tests

## Important limitations

- This is not an official validation result.
- SHA-384, SHA-512, Hash_DRBG, block cipher APIs, and public random service APIs are not implemented yet.
- KAT pass results are correctness checks only and do not replace formal validation.
- HMAC_DRBG-SHA-256 accepts explicit entropy input in public APIs; automatic public instantiation from OS entropy remains TODO.
- The entropy callback seam is test/internal plumbing and is not an operational entropy source.
- The platform entropy provider seam and DRBG helper are internal; provider/helper smoke tests are engineering checks only and do not store entropy bytes.
- HMAC key handles are in-memory only; export, persistence, keystore integration, and full thread-safety support are not finalized.
- Prediction-resistance requests are rejected until the entropy adapter, reseed policy, and review evidence are defined.
- Error recovery policy after fatal state remains TODO.
- Public build identity still exposes one versioned struct layout; field getters should be considered before ABI freeze.
- Binary reproducibility across hosts/toolchains is not established yet.
- Internal test-hook builds may expose internal symbols for internal-unit tests; release-style no-test builds should enable symbol hiding where supported.
- This repository does not claim official validation or external review completion.
