# AnoCrypto-C Release Evidence Workflow

Status: P1-6 draft.

## Purpose

This document describes the engineering workflow for collecting release evidence drafts from the private C99 codebase.

This workflow is not a substitute for formal validation, lab review, legal review, or release approval.

## Draft collection command

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
cmake --build build --target ano_crypto_collect_release_evidence
```

The generated output defaults to:

```text
build/evidence/p1-6/
  BUILD_METADATA.txt
  SOURCE_FILES.sha256
  SOURCE_ARCHIVE_CONTENTS.txt
  SOURCE_ARCHIVE.sha256
  SOURCE_ARCHIVE.log
  AnoCrypto-C-source.tar.gz
  BINARY_FILES.sha256
  ctest.log
  INSTALL_SMOKE_INSTALL.log
  INSTALL_SMOKE_CONFIGURE.log
  INSTALL_SMOKE_BUILD.log
  INSTALL_SMOKE_RUN.log
  INSTALL_SMOKE_SUMMARY.txt
  README.md
```

## Review checklist

Before treating the generated directory as a release-candidate evidence input, review:

- source manifest and exclusion log snapshots
- source archive file list and SHA-256
- source and binary SHA-256 lists
- compiler and target identity
- CTest logs
- SHA-256/HMAC-SHA-256 KAT coverage
- HMAC_DRBG-SHA-256 KAT coverage
- CSP handle lifecycle, exhaustion, and stale-handle coverage
- negative test coverage
- install smoke test log from an installed prefix
- algorithm scope matrix
- SBOM/signing status
- known limitations
- absence of secrets, private keys, tokens, operational entropy samples, or internal deployment details

## P1-6 additions

- Generate `AnoCrypto-C-source.tar.gz` from filtered repository source inputs.
- Generate `SOURCE_ARCHIVE.sha256` and `SOURCE_ARCHIVE_CONTENTS.txt`.
- Capture installed-package smoke logs by installing the current build into an evidence-local prefix and building `tests/install_smoke` as an external consumer.
- Include `ano_crypto_csp_handle_stale_exhaustion_tests` in CTest and evidence output.

## P1-6 known gaps

- SBOM and signing status are placeholders.
- CI-produced evidence bundle layout remains TODO.
- Reproducible build procedure remains TODO.
- Thread-safety evidence for module global state and CSP handle table remains TODO.
- Official validation-package mapping remains TODO.

## Sensitive-data rule

Do not include raw entropy, HMAC key bytes, DRBG state, generated secret bytes, private material, tokens, or internal deployment details in evidence logs.


## P1-6 Thread-Safety Evidence Notes

P1-6 CTest output may include `ano_crypto_thread_lock_tests` and, on POSIX builds, `ano_crypto_csp_thread_safety_tests`.
These logs are engineering concurrency smoke-test evidence only. They must not be described as external review results or broad thread-safety certification.
