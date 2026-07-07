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

## P1-6 Evidence Notes

- SHA-256 and HMAC-SHA-256 processing APIs are implemented for the current P1 slice.
- HMAC-SHA-256 can also be computed through an in-memory HMAC key handle lifecycle slice.
- HMAC_DRBG-SHA-256 deterministic core APIs are implemented for the current P1 slice.
- Extended KAT tests cover public SHA-256 vectors, RFC 4231 HMAC-SHA-256 cases, the HMAC key handle path, and a public CAVP HMAC_DRBG-SHA-256 flow.
- Deterministic chunk-boundary tests cover one-shot versus streaming API consistency.
- `tests/negative/crypto_service_gate_tests.c` verifies that cryptographic services are rejected before module initialization and startup self-test success.
- `tests/negative/hash_hmac_api_negative_tests.c` verifies representative invalid argument, short output buffer, and finalized-context behavior.
- `tests/negative/drbg_api_negative_tests.c` verifies DRBG argument, entropy-length, flag, state, and cleanup behavior.
- `tests/negative/csp_key_lifecycle_negative_tests.c` verifies HMAC key handle lifecycle rejection and cleanup paths.
- `tests/negative/csp_handle_stale_exhaustion_tests.c` verifies fixed-table exhaustion, generation-based stale-handle rejection, and post-reuse state queries.
- `tests/unit/entropy_adapter_tests.c` verifies internal entropy adapter seam behavior without storing raw entropy in logs.
- `tools/collect_release_evidence.cmake` creates a local engineering evidence draft from a build tree and can capture source archive SHA-256 plus installed-package smoke logs.

## Commands

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Install smoke:

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1-6
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-6
cmake --build build-install-smoke --parallel
./build-install-smoke/anocrypto_c_install_smoke
```

Evidence draft:

```sh
cmake --build build --target ano_crypto_collect_release_evidence
```

## TODO

- Add KAT vector local-copy hash policy when vectors are stored as separate data files.
- Add CI-produced evidence bundle layout.
- Add SBOM and signing evidence fields.
- Add reproducible build notes.
- Add thread-safety test/evidence strategy for global module state and CSP table.


## P1-6 Thread-Safety Log Review

If present, review thread-safety test logs for:

- status-only output
- no key bytes
- no MAC/DRBG output bytes
- no entropy bytes
- no broad support claims beyond the documented P1-6 global-state lock policy
