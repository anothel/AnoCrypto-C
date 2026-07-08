# SBOM and Signing Plan

Status: P1-15 draft.

## Purpose

This document records how AnoCrypto-C will track SBOM and release-signing evidence without adding private signing keys, tokens, customer data, or unpublished deployment details to the repository or PoC package.

P1-14 still does not generate a complete SBOM and does not perform release signing. It adds placeholders, manifest files, and review steps so that later release-candidate packaging can record the same fields consistently.

## SBOM scope

Candidate SBOM inputs:

- source archive file list and hashes
- installed public headers
- produced library files
- CMake package files
- bundled example sources
- selected documentation included in customer PoC packages
- compiler and generator identity from build metadata

Out of scope for P1-14:

- dependency graph attestation from a pinned SBOM generator
- package-manager dependency import
- vendor vulnerability feed mapping
- CI-hosted SBOM artifact upload
- customer-specific binary inventory

## Candidate SBOM formats

| Format | P1-14 position | Notes |
|---|---|---|
| SPDX JSON | Candidate | Tool selection and exact schema version remain TODO. |
| CycloneDX JSON | Candidate | Tool selection and exact schema version remain TODO. |
| Plain hash manifest | Implemented as engineering placeholder | `SOURCE_FILES.sha256`, `BINARY_FILES.sha256`, package manifests, and evidence manifest are used until a real SBOM generator is selected. |

## Signing scope

Candidate signed artifacts:

- source archive
- source archive hash file
- binary hash file
- customer PoC package archive when created
- SBOM file when a generator is selected
- evidence manifest hash file

Signing rules:

- Do not store private signing keys in this repository.
- Do not store signing tokens in this repository.
- Prefer detached signatures over modifying the original artifact.
- Record signer identity policy only after legal/product review.
- Record timestamping policy only after infrastructure review.
- Record verification command examples only after the signing tool is selected.

## P1-14 generated placeholder files

Evidence collection writes:

```text
SBOM_STATUS.txt
SIGNING_STATUS.txt
EVIDENCE_MANIFEST_README.txt
EVIDENCE_MANIFEST.sha256
```

PoC package staging writes:

```text
SBOM_STATUS.txt
SIGNING_STATUS.txt
PACKAGE_REVIEW_MANIFEST.md
PACKAGE_MANIFEST.md
THREAD_SAFETY_PACKAGE_NOTES.txt
```

These files are engineering tracking artifacts. They do not assert that a complete SBOM exists and they do not assert that release signing has been performed.

## Review checklist

Before external sharing, confirm:

- SBOM status is either a reviewed generated SBOM or clearly marked as placeholder/TODO.
- Signing status does not imply signatures exist unless detached signature files are present and verified.
- Package and evidence manifests do not contain secrets, entropy bytes, generated secret bytes, private keys, tokens, customer data, or internal deployment details.
- Hash manifests reference intended files only.
- Local absolute paths are reviewed and redacted when needed.
- Source provenance remains aligned with `cleanroom/SOURCE_MANIFEST.md`.
- Excluded categories remain aligned with `cleanroom/EXCLUSION_LOG.md`.

## Open TODO

- Select SBOM generator and output format.
- Decide whether CI should generate SBOM artifacts for every branch or only release-candidate builds.
- Decide artifact signing tool and detached-signature extension.
- Decide signer identity and verification policy.
- Decide timestamping policy.
- Decide package archive format for customer PoC handoff.
