# Customer PoC Package Review Manifest

Status: P1-13 draft.

## Purpose

This document defines the review intent for the AnoCrypto-C customer PoC package v0. The package is a customer integration draft for C ABI, installed CMake package lookup, module lifecycle, self-test startup behavior, and limited SHA-256/HMAC-SHA-256/HMAC_DRBG-SHA-256 smoke flows.

## Required package records

The PoC package should contain:

- `README.md`
- `PACKAGE_MANIFEST.md`
- `PACKAGE_REVIEW_MANIFEST.md`
- `SBOM_STATUS.txt`
- `SIGNING_STATUS.txt`
- `INSTALL_PACKAGE.log`
- `sdk/`
- `examples/core_poc/`
- `docs/`
- `evidence-template/`

## Review before external sharing

Confirm:

- the package manifest includes only package-relative paths
- install logs do not disclose internal deployment details that should be removed
- examples do not print key bytes, entropy bytes, seed material, nonce material, DRBG state, generated DRBG output, private keys, or tokens
- docs do not imply official validation evidence or external review completion
- SBOM status is clearly marked as placeholder unless an actual SBOM is generated later
- signing status is clearly marked as not performed unless detached signature files are added later
- package contents match the intended customer PoC scope

## Customer-specific notes

Do not add customer secrets, customer certificates with private keys, production tokens, internal deployment paths, or customer data to this package. Use a separate reviewed customer note for platform, compiler, linker, OS, CPU, and algorithm-scope requirements.

## Open TODO

- Add legal/evaluation notice after legal review.
- Add package archive creation after handoff format is chosen.
- Add detached signature records after signing tool and key-management policy are selected.
- Add generated SBOM records after generator selection.
