# Release Evidence Workflow

Status: P1-12 draft.

## Purpose

This workflow defines local engineering evidence collection for anocrypto-c development builds. It is not official validation evidence and should be reviewed before external sharing.

## Current evidence target

```sh
cmake --build build --target ano_crypto_collect_release_evidence
```

Default output path:

```text
build/evidence/p1-12/
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
SCOPE_REVIEW_NOTES.txt
ctest.log
INSTALL_SMOKE_INSTALL.log
INSTALL_SMOKE_CONFIGURE.log
INSTALL_SMOKE_BUILD.log
INSTALL_SMOKE_RUN.log
INSTALL_SMOKE_SUMMARY.txt
POC_PACKAGE_SMOKE.log
poc-package-smoke/
README.md
```

## Review checklist before sharing

- Confirm the source archive excludes build outputs, generated evidence bundles, local caches, and copied third-party material.
- Confirm logs do not include entropy bytes, DRBG seed material, generated random bytes, MAC output, key material, private keys, tokens, customer data, or internal deployment details.
- Confirm KAT vector provenance is recorded in `cleanroom/SOURCE_MANIFEST.md`.
- Confirm excluded categories are recorded in `cleanroom/EXCLUSION_LOG.md`.
- Confirm claims remain limited to engineering correctness and readiness preparation.
- Confirm platform provider logs include provider name/status only.
- Confirm PoC package manifests and logs do not disclose local customer/internal details before sharing.
- Confirm P1-12 scope-review notes match the intended customer and validation-target candidate boundary.

## P1-12 additions

- P1-10 reproducible-build notes, public surface checks, CI matrix skeleton, and API/ABI checklist are retained.
- P1-11 customer PoC package staging and smoke are retained.
- Scope-review planning artifacts are added:
  - `docs/roadmap/DEVELOPMENT_WORKFLOW.md`
  - `docs/kcmvp/SERVICE_SCOPE_MATRIX.md`
  - `docs/kcmvp/DIFFERENTIAL_VALIDATION_STRATEGY.md`
  - `docs/design/BLOCK_CIPHER_MODE_SELECTION.md`
  - `docs/security/SIDE_CHANNEL_REVIEW_CHECKLIST.md`
  - `docs/business/CUSTOMER_POC_INTAKE_QUESTIONNAIRE.md`
- Evidence collection writes `SCOPE_REVIEW_NOTES.txt` to point reviewers at the current scope-lock artifacts.
- The packaged PoC documentation set includes the new scope and intake documents.

## Known gaps

- No SBOM generation yet.
- No release signing evidence yet.
- CI workflow has not yet been reviewed against all customer toolchains.
- PoC package legal/evaluation notice remains TODO pending legal review.
- Source archive timestamp normalization is still TODO if byte-for-byte archives become required.
- Binary reproducibility across hosts/toolchains is still TODO.
- No official validation evidence.
- No public random-service API review record yet.
- No selected block cipher/mode implementation scope yet.
- No differential-reference tool/source has been selected yet.
