# Release Evidence Workflow

Status: P1-10 draft.

## Purpose

This workflow defines local engineering evidence collection for anocrypto-c development builds. It is not official validation evidence and should be reviewed before external sharing.

## Current evidence target

```sh
cmake --build build --target ano_crypto_collect_release_evidence
```

Default output path:

```text
build/evidence/p1-10/
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
PUBLIC_SURFACE_CHECK.txt
REPRODUCIBLE_BUILD_NOTES.txt
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

## P1-10 additions

- Reproducible-build notes are now captured in `docs/kcmvp/REPRODUCIBLE_BUILD_NOTES.md` and generated evidence as `REPRODUCIBLE_BUILD_NOTES.txt`.
- Public surface policy checks are captured as `PUBLIC_SURFACE_CHECK.txt`.
- CI matrix skeleton is recorded in `docs/ci/CI_MATRIX.md` and `.github/workflows/ci.yml`.
- API/ABI review checklist is recorded in `docs/design/API_ABI_REVIEW_CHECKLIST.md`.
- Evidence collection now records that reproducible-build procedure is draft-only and that byte-identical binaries are not claimed.

## Known gaps

- No SBOM generation yet.
- No release signing evidence yet.
- CI workflow has not yet been reviewed against all customer toolchains.
- Source archive timestamp normalization is still TODO if byte-for-byte archives become required.
- Binary reproducibility across hosts/toolchains is still TODO.
- No formal validation evidence.
- No public random-service API review record yet.
