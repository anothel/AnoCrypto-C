# Release Evidence Workflow

Status: P1-13 draft.

## Purpose

This workflow defines local engineering evidence collection for anocrypto-c development builds. It is not official validation evidence and should be reviewed before external sharing.

## Current evidence target

```sh
cmake --build build --target ano_crypto_collect_release_evidence
```

Default output path:

```text
build/evidence/p1-13/
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
SBOM_STATUS.txt
SIGNING_STATUS.txt
EVIDENCE_MANIFEST_README.txt
EVIDENCE_MANIFEST.sha256
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
- Confirm P1-12 scope-review notes still match the intended customer and validation-target candidate boundary.
- Confirm P1-13 SBOM/signing placeholder status and evidence manifest are accurate.

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


## P1-13 additions

- SBOM/signing plan document added: `docs/kcmvp/SBOM_SIGNING_PLAN.md`.
- Evidence manifest policy added: `docs/kcmvp/EVIDENCE_MANIFEST_POLICY.md`.
- Customer PoC package review manifest added: `docs/business/POC_PACKAGE_REVIEW_MANIFEST.md`.
- Evidence collection writes `SBOM_STATUS.txt`, `SIGNING_STATUS.txt`, `EVIDENCE_MANIFEST_README.txt`, and `EVIDENCE_MANIFEST.sha256`.
- PoC package staging writes `PACKAGE_REVIEW_MANIFEST.md`, `SBOM_STATUS.txt`, and `SIGNING_STATUS.txt`.
- PoC package smoke checks that package manifest/review/status files exist.

## Known gaps

- No complete SBOM generation yet; P1-13 records placeholder status only.
- No release signing evidence yet; P1-13 records placeholder status only.
- CI workflow has not yet been reviewed against all customer toolchains.
- PoC package legal/evaluation notice remains TODO pending legal review.
- Source archive timestamp normalization is still TODO if byte-for-byte archives become required.
- Binary reproducibility across hosts/toolchains is still TODO.
- No official validation evidence.
- No public random-service API review record yet.
- No selected block cipher/mode implementation scope yet.
- No differential-reference tool/source has been selected yet.
