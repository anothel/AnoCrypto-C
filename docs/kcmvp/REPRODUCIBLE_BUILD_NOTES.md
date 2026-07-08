# AnoCrypto-C Reproducible Build Notes

Status: P1-10 draft.

## Purpose

These notes define the first engineering procedure for repeatable AnoCrypto-C builds and source archives. They are a preparation checklist, not a claim that every binary is currently byte-identical across hosts.

## Current reproducibility target

P1-10 targets reproducible inputs and reviewable evidence first:

- Clean source tree with reviewed source manifest and exclusion log.
- Stable CMake package identity: `AnoCryptoC`, `AnoCryptoC::AnoCryptoC`, library target/file `anocrypto_c`.
- Explicit build options captured in the evidence bundle.
- Source-file hash list generated in sorted path order.
- Source archive content list generated from the same filtered file set.
- Binary hash list captured after local build.
- CTest and install-smoke logs captured as status evidence.
- Public surface policy check captured before release-candidate review.

## Reference local commands

```sh
cmake -S . -B build \
  -DANO_CRYPTO_BUILD_TESTS=ON \
  -DANO_CRYPTO_ENABLE_STRICT_WARNINGS=ON \
  -DANO_CRYPTO_ENABLE_THREAD_SAFETY=ON \
  -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=ON \
  -DANO_CRYPTO_BUILD_GIT_COMMIT=$(git rev-parse --verify HEAD) \
  -DANO_CRYPTO_BUILD_FINGERPRINT=AnoCrypto-C-p1-10-repro-ci-abi \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --build build --target ano_crypto_public_surface_check
cmake --build build --target ano_crypto_collect_release_evidence
```

Install smoke can also be run manually:

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1-10
cmake -S tests/install_smoke -B build-install-smoke \
  -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-10
cmake --build build-install-smoke --parallel
./build-install-smoke/anocrypto_c_install_smoke
```

Release-style exported-symbol check without internal test hooks:

```sh
cmake -S . -B build-release \
  -DANO_CRYPTO_BUILD_TESTS=OFF \
  -DANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel
cmake --build build-release --target ano_crypto_public_surface_check
```

## Inputs to pin before release-candidate use

| Area | P1-10 rule | Status |
|---|---|---|
| Source tree | Use a clean commit and record commit hash. | Required |
| Build type | Record `CMAKE_BUILD_TYPE` or multi-config configuration. | Required |
| Compiler | Record compiler ID and version. | Required |
| Generator | Record CMake generator and platform where applicable. | TODO |
| Options | Record all `ANO_CRYPTO_*` build options. | Required |
| Install prefix | Use a temporary prefix and review logs for path disclosure. | Required |
| Archive contents | Generate sorted content list and hash list. | Required |
| Timestamps | Normalize archive timestamps if byte-for-byte source archives become a release requirement. | TODO |
| Binary paths | Avoid embedding local absolute paths in release binaries where toolchain permits. | TODO |
| Internal symbols | Hide internal symbols in release-style shared builds where supported. | Partial |
| Locale/timezone | Record or normalize build environment variables for release jobs. | TODO |
| Debug info | Decide whether debug info is stripped, split, or retained. | TODO |
| SBOM/signing | Add SBOM and signing records when the release process is ready. | TODO |

## Evidence bundle additions

P1-10 evidence should include:

- `REPRODUCIBLE_BUILD_NOTES.txt`
- `PUBLIC_SURFACE_CHECK.txt`
- `SOURCE_FILES.sha256`
- `SOURCE_ARCHIVE_CONTENTS.txt`
- `SOURCE_ARCHIVE.sha256`
- `BINARY_FILES.sha256`
- `ctest.log`
- `INSTALL_SMOKE_*.log`

## Review cautions

- Do not include secret keys, entropy bytes, generated secret bytes, private keys, tokens, customer data, or internal deployment details in logs.
- Treat source and binary hashes as traceability evidence only.
- Treat local test and smoke results as engineering evidence only.
- Mark unresolved gaps as TODO instead of upgrading claims.
