# Release Evidence Workflow

Status: P1-9 draft.

## Purpose

This workflow defines local engineering evidence collection for anocrypto-c development builds. It is not official validation evidence and should be reviewed before external sharing.

## Current evidence target

```sh
cmake --build build --target ano_crypto_collect_release_evidence
```

Default output path:

```text
build/evidence/p1-9/
```

## Expected generated files

```text
BUILD_METADATA.txt
SOURCE_FILES.sha256
SOURCE_ARCHIVE_CONTENTS.txt
SOURCE_ARCHIVE.sha256
SOURCE_ARCHIVE.log
anocrypto-c-source.tar.gz
BINARY_FILES.sha256
ctest.log
INSTALL_SMOKE_INSTALL.log
INSTALL_SMOKE_CONFIGURE.log
INSTALL_SMOKE_BUILD.log
INSTALL_SMOKE_RUN.log
INSTALL_SMOKE_SUMMARY.txt
README.md
```

## Review checklist before sharing

- Confirm the source archive excludes build outputs, generated evidence bundles, local caches, and copied third-party material.
- Confirm logs do not include entropy bytes, DRBG seed material, generated random bytes, MAC output, key material, private keys, tokens, customer data, or internal deployment details.
- Confirm KAT vector provenance is recorded in `cleanroom/SOURCE_MANIFEST.md`.
- Confirm excluded categories are recorded in `cleanroom/EXCLUSION_LOG.md`.
- Confirm claims remain limited to engineering correctness and readiness preparation.
- Confirm platform provider logs include provider name/status only.

## P1-9 additions

- Internal HMAC_DRBG-SHA-256 instantiate/reseed helper tests are included in CTest.
- Evidence may include `ano_crypto_drbg_entropy_helper_tests` pass/fail status.
- Helper tests must not print entropy, seed material, nonce bytes, generated random bytes, or deterministic callback byte streams.
- Public random-service API remains a design draft only and is not part of the installed public ABI.

## Known gaps

- No SBOM generation yet.
- No release signing evidence yet.
- No platform CI evidence for Windows/Apple provider branches yet.
- No reproducible build procedure beyond source/binary hash capture.
- No formal validation evidence.
- No public random-service API review record yet.
