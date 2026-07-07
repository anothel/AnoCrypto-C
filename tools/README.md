# AnoCrypto-C Tools

This directory contains engineering helper scripts for the private C99 codebase.

## `collect_release_evidence.cmake`

Creates a local release-evidence draft from a configured build tree:

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
cmake --build build --target ano_crypto_collect_release_evidence
```

Or run the script directly:

```sh
cmake \
  -DANO_CRYPTO_SOURCE_DIR=. \
  -DANO_CRYPTO_BUILD_DIR=build \
  -DANO_CRYPTO_EVIDENCE_DIR=build/evidence/p1-5 \
  -DANO_CRYPTO_RUN_CTEST=ON \
  -DANO_CRYPTO_CREATE_SOURCE_ARCHIVE=ON \
  -DANO_CRYPTO_RUN_INSTALL_SMOKE=ON \
  -P tools/collect_release_evidence.cmake
```

P1-5 output includes source-file hashes, a source archive SHA-256, binary hashes, CTest output, and optional install-prefix smoke logs.

The generated directory is an engineering draft only. Review it before sharing because it can contain local paths, compiler identifiers, and test logs.
