# AnoCrypto-C

AnoCrypto-C is a private C99 cryptographic module candidate skeleton. The repository slug may remain `anocrypto-c`, but the product/brand spelling is `AnoCrypto-C`.

Current status: P1-12 foundation with SHA-256, HMAC-SHA-256, HMAC_DRBG-SHA-256, expanded KATs, negative tests, internal platform entropy provider seam, internal DRBG instantiate/reseed helper, HMAC_DRBG boundary vectors, CMake package naming normalization, HMAC key handle lifecycle slice, stale-handle/exhaustion tests, limited internal global-state locking, reproducible-build notes, CI matrix skeleton, API/ABI review checklist, public-surface policy check, release-evidence draft tooling, customer PoC package v0 skeleton, and scope/validation planning documents.

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
- Release evidence draft collection with source archive hash, public-surface check, reproducible-build notes, install smoke logs, PoC package smoke logs, and scope-review notes
- Limited internal locking for module/self-test/CSP global state
- CI matrix skeleton for Linux, macOS, and Windows build/test/install-smoke paths
- API/ABI review checklist for public headers, package identity, and symbol-surface TODOs
- Release-style internal symbol hiding option for supported compilers
- Customer PoC package v0 staging and standalone public-API example smoke
- Development workflow, service scope matrix, differential validation strategy, block-cipher selection gate, side-channel checklist, and customer PoC intake questionnaire drafts

Skeleton-only / TODO scope:

- SHA-384/SHA-512 ABI placeholders
- HMAC-SHA-384/HMAC-SHA-512 ABI placeholders
- Hash_DRBG-SHA-256 ABI placeholder
- Public random service layer and reviewed customer/platform entropy policy
- Persistent keystore, key export, key wrapping, and multi-process CSP model
- Public per-context concurrency guarantees and reviewed platform CI coverage
- Byte-identical source archive and binary reproducibility rules
- SBOM/signing workflow
- PoC package legal/evaluation notice and customer-specific support boundary
- Block cipher/mode implementation, pending P1-12 scope decision review

## Build

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Install smoke test

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1-12
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-12
cmake --build build-install-smoke
./build-install-smoke/anocrypto_c_install_smoke
```

## Core PoC example

Build the example from the build tree:

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DANO_CRYPTO_BUILD_EXAMPLES=ON
cmake --build build
./build/examples/core_poc/anocrypto_c_core_poc
```

Build the example as an installed-package consumer:

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1-12
cmake -S examples/core_poc -B build-core-poc -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-12
cmake --build build-core-poc
./build-core-poc/anocrypto_c_core_poc
```

The example prints status-only success messages. It does not print raw key material, entropy input, nonce input, DRBG state, or generated DRBG bytes.

## Customer PoC package v0

Create a local package draft:

```sh
cmake --build build --target ano_crypto_create_poc_package
```

Run the package smoke flow:

```sh
cmake --build build --target ano_crypto_poc_package_smoke
```

The package smoke target stages an installed SDK prefix, configures the packaged `examples/core_poc` project with `find_package(AnoCryptoC)`, builds it, and runs it.

## Evidence draft collection

```sh
cmake --build build --target ano_crypto_collect_release_evidence
```

The generated evidence directory is a local engineering draft. It is not official validation evidence and should be reviewed before sharing.

Generated files include `SOURCE_ARCHIVE.sha256`, `BINARY_FILES.sha256`, `PUBLIC_SURFACE_CHECK.txt`, `REPRODUCIBLE_BUILD_NOTES.txt`, `SCOPE_REVIEW_NOTES.txt`, `ctest.log`, installed-package smoke logs, and PoC package smoke logs when the default P1-12 target is used.

## P1-12 scope

- P1-10 reproducible build notes, CI matrix skeleton, API/ABI checklist, public-surface checks, and symbol-hiding option retained
- P1-11 customer PoC package v0 staging and smoke retained
- Development workflow plan added
- Service scope matrix added
- Differential validation strategy draft added
- Block cipher/mode selection decision gate added
- Side-channel review checklist added
- Customer PoC intake questionnaire added
- Evidence workflow extended with scope-review notes
- Clean-room source manifest and exclusion log updated for planning additions

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
- Customer PoC package v0 is an integration draft and still needs legal/evaluation notice review before external sharing.
- P1-12 planning documents do not add new cryptographic algorithm implementations.
- This repository does not claim official validation or external review completion.
