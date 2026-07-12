# Evidence Manifest Policy

Status: P1-29 engineering draft.

## Purpose

AnoCrypto-C release evidence is currently a local engineering draft. P1-29 retains the manifest policy so each generated evidence directory and customer PoC package can be reviewed with consistent hash and placeholder records.

## Manifest files

| File | Producer | Purpose |
|---|---|---|
| `SOURCE_FILES.sha256` | evidence script | Hashes source-tree files selected for the local source archive. |
| `SOURCE_ARCHIVE_CONTENTS.txt` | evidence script | Lists source archive input paths. |
| `SOURCE_ARCHIVE.sha256` | evidence script | Hashes the local source archive when archive creation is enabled. |
| `BINARY_FILES.sha256` | evidence script | Hashes matched build outputs. |
| `EVIDENCE_MANIFEST.sha256` | evidence script | Hashes generated evidence files after collection. |
| `PACKAGE_MANIFEST.md` | PoC package script | Hashes staged PoC package files. |
| `PACKAGE_REVIEW_MANIFEST.md` | PoC package script | Records package review intent and sensitive-data boundaries. |
| `THREAD_SAFETY_CHECK.txt` | evidence script | Records support-matrix and thread-safety policy-check output. |
| `THREAD_SAFETY_PACKAGE_NOTES.txt` | PoC package script | Records package-facing thread-safety notes for customer PoC review. |

## Hash algorithm

Use SHA-256 for the retained engineering manifests. If later release policy requires additional digest algorithms, add them as separate files instead of changing existing file semantics silently.

## Manifest generation rules

- Sort file paths before writing manifest entries.
- Use repository-relative or package-relative paths when possible.
- Avoid adding local absolute paths to package manifests.
- Exclude generated manifest files from their own hash list.
- Exclude transient build directories unless the evidence workflow intentionally captures them.
- Do not include raw key material, entropy captures, generated secret bytes, private keys, tokens, customer data, or internal deployment details.

## Evidence manifest boundaries

The evidence manifest can include local logs, installed smoke outputs, package smoke logs, placeholder status files, and generated package manifest files. It remains a review aid, not official validation evidence.

Review before sharing:

- local paths in logs
- compiler and build-system identifiers
- package smoke directory contents
- generated CMake cache files
- any accidentally copied customer or internal data

## PoC package manifest boundaries

The PoC package manifest should include only package-relative files staged for the PoC handoff. It should not include source control metadata, local build cache directories, signing keys, tokens, entropy captures, or customer-specific materials.

## Open TODO

- Add optional archive-level hash for a compressed PoC package when package archive creation is added.
- Add CI artifact manifest after hosted workflow review.
- Add exported-symbol baseline hash if a stable baseline file is introduced.
- Add SBOM hash once a real SBOM generator is selected.

## P1-28 AES-CMAC records

Include `AES_CMAC_IMPLEMENTATION_CHECK.txt` and `AES_CMAC_REVIEW_NOTES.txt` in
the generated manifest. The files may contain scope, status, symbol count,
provider identity, comparison count, and review metadata only. CMAC keys,
messages, subkeys, intermediate state, and tags are prohibited.


## P1-29 AES-CMAC hardening records

Include the following status-only records when generated:

- `AES_CMAC_HARDENING_CHECK.txt`;
- `AES_CMAC_HARDENING_REVIEW.log`;
- `AES_CMAC_LIFECYCLE_STRESS.log`;
- `CMAC_COMPILER_OUTPUT_REVIEW.txt`;
- `CMAC_COMPILER_OUTPUT_DISASSEMBLY.txt` for internal manual review;
- `AES_CMAC_HARDENING_REVIEW_NOTES.txt`.

The compiler-output disassembly remains an internal engineering artifact and is
excluded from the customer PoC package. None of these records may contain CMAC
keys, message bytes, computed or supplied tags, AES schedules, chaining state,
or other secret-dependent values.
