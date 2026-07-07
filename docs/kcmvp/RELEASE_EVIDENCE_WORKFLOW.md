# AnoCrypto-C Release Evidence Workflow

Status: P1-4 draft.

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
build/evidence/p1-4/
  BUILD_METADATA.txt
  SOURCE_FILES.sha256
  BINARY_FILES.sha256
  ctest.log
  README.md
```

## Review checklist

Before treating the generated directory as a release-candidate evidence input, review:

- source manifest and exclusion log snapshots
- source and binary SHA-256 lists
- compiler and target identity
- CTest logs
- SHA-256/HMAC-SHA-256 KAT coverage
- HMAC_DRBG-SHA-256 KAT coverage
- negative test coverage
- install smoke test log from an installed prefix
- algorithm scope matrix
- SBOM/signing status
- known limitations
- absence of secrets, private keys, tokens, operational entropy samples, or internal deployment details

## P1-4 known gaps

- Source archive SHA-256 is not generated yet.
- Install-prefix smoke logs are not captured by the evidence script yet.
- SBOM and signing status are placeholders.
- Entropy adapter, CSP/key lifecycle, and thread-safety evidence remain TODO.
- Official validation-package mapping remains TODO.


## P1-4 additions

- Include `ano_crypto_hmac_key_handle_kat_tests` output in KAT logs.
- Include `ano_crypto_csp_key_lifecycle_negative_tests` output in negative test logs.
- Include `ano_crypto_entropy_adapter_tests` output as internal seam test evidence.
- Do not include raw entropy, HMAC key bytes, DRBG state, generated secret bytes, or private material in evidence logs.
