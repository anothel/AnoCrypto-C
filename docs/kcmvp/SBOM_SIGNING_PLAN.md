# SBOM and Signing Plan

Status: P1-31 unsigned engineering plan.

## Purpose

AnoCrypto-C records source-archive contents and release hash bindings without storing private signing keys, signing tokens, customer data, unpublished deployment details, or cryptographic secret material in the repository, evidence bundle, or customer PoC package.

P1-31 generates a source inventory and an unsigned provenance bundle. It does not perform release signing and does not authorize external release.

## P1-31 SBOM scope

Generated artifact:

```text
release-provenance/SOURCE_SBOM.spdx.json
```

Format and scope:

- SPDX 2.3 JSON source inventory;
- exact repository-relative files selected for the source archive;
- SHA-256 and SHA-1 per-file checksums in each SPDX file record;
- SPDX package verification code from sorted per-file SHA-1 values;
- one AnoCrypto-C source package record;
- release version and generation timestamp.

Explicit limitations:

- dependency discovery is not performed;
- package-manager metadata is not imported;
- vulnerability analysis is not performed;
- licenses and copyright remain `NOASSERTION` unless a later legal review changes the policy;
- customer-specific binary and runtime inventory are not included;
- source inventory generation is not official validation evidence.

## Provenance bundle

P1-31 creates:

```text
RELEASE_PROVENANCE.txt
RELEASE_ARTIFACTS.sha256
SOURCE_FILES.sha256
SOURCE_SBOM.spdx.json
SOURCE_SBOM_SUMMARY.txt
PUBLIC_HEADERS.sha256
EXPORTED_SYMBOLS.txt
HOSTED_ARTIFACT_DISPOSITION.txt
SBOM_STATUS.txt
SIGNING_STATUS.txt
SIGNING_REQUEST.txt
RELEASE_AUTHORIZATION.md
anocrypto-c-source.tar.gz
```

The verifier requires complete manifest coverage, safe relative paths, source-archive member and extracted-content agreement with `SOURCE_FILES.sha256`, exact SPDX path/SHA-256/SHA-1 record binding, matching release identity, explicit clean/dirty/unknown worktree state, current public-header/symbol inputs, an explicit hosted review state, `UNSIGNED` signing state, and `NO` external authorization. A tagged or completed-hosted-review record requires a known commit and clean worktree.

## Signing subject and boundary

The proposed detached-signature subject is:

```text
RELEASE_ARTIFACTS.sha256
```

P1-31 does not select or invoke a signing tool. The following remain TODO:

- detached-signature format and file extension;
- signer identity and authorization policy;
- private-key custody and access control;
- hardware-backed key use, if selected;
- timestamping and revocation handling;
- verification tool and retained verification record;
- organization and legal approval;
- customer distribution policy.

The P1-31 checker rejects signature/key/credential-like bundle files and rejects claims of completed signing or external authorization.

## GitHub-hosted attestation candidate

A hosted attestation service may be evaluated after repository plan, privacy, authorization, and retention requirements are known. It is not enabled by P1-31. An uploaded artifact, Git tag, expected tag string, or hash manifest alone is not treated as a signed attestation.

## PoC boundary

The customer PoC package retains package-relative hash manifests and explicit SBOM/signing status notes. The internal source archive, source SBOM, signing request, release authorization record, compatibility baseline, and hosted review tooling are not copied into the PoC package.

## Review checklist

Before a future external release:

- verify expected tag, supplied commit, and checked-out HEAD identify one reviewed commit with a clean worktree;
- review actual hosted artifacts and record reviewer/date/limitations;
- validate SBOM syntax and scope with the selected downstream tool;
- approve signing tool, signer, key custody, timestamp, revocation, and verification procedures;
- cryptographically verify the detached signature;
- confirm no secret key, seed, entropy, DRBG state, private scalar, token, certificate private key, customer data, or internal deployment detail is present;
- record legal/product authorization;
- regenerate all hashes after approved metadata changes.
