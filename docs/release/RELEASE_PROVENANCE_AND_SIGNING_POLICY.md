# Release Provenance and Signing Policy

Status: P1-31 unsigned engineering policy. This policy does not authorize external release and does not assert that a cryptographic signature, hosted review, or official validation exists.

## Purpose

P1-31 binds the reviewed source archive and public compatibility inputs into one self-contained provenance bundle before another public cryptographic API is considered. The bundle is designed to make missing, stale, or tampered release metadata visible without storing signing credentials in the repository.

The generated bundle is an engineering record. It is not a substitute for:

- tag and commit authorization;
- actual hosted platform artifact review;
- cryptographic signature generation and verification;
- legal/product release approval;
- official validation evidence.

## Bundle contents

`tools/create_release_provenance_bundle.cmake` produces:

```text
release-provenance/
  anocrypto-c-source.tar.gz
  SOURCE_FILES.sha256
  SOURCE_SBOM.spdx.json
  SOURCE_SBOM_SUMMARY.txt
  PUBLIC_HEADERS.sha256
  EXPORTED_SYMBOLS.txt
  HOSTED_ARTIFACT_DISPOSITION.txt
  RELEASE_PROVENANCE.txt
  SBOM_STATUS.txt
  SIGNING_STATUS.txt
  SIGNING_REQUEST.txt
  RELEASE_AUTHORIZATION.md
  RELEASE_ARTIFACTS.sha256
```

`RELEASE_ARTIFACTS.sha256` covers every other file in the bundle and is the only proposed detached-signature subject. The signature format, verifier, signer policy, and timestamp policy remain TODO. The P1-31 generator rejects a signed claim and does not accept key or credential files.

## Source SBOM boundary

P1-31 generates an SPDX 2.3 JSON source inventory for the exact file list used to create `anocrypto-c-source.tar.gz`.

Included:

- repository-relative file paths;
- SHA-256 and SHA-1 file checksums bound to the same SPDX file record;
- SPDX file identifiers;
- one AnoCrypto-C package record;
- SPDX package verification code computed from the sorted per-file SHA-1 values;
- release version and generation time.

Not performed:

- package-manager or transitive dependency discovery;
- license conclusions beyond `NOASSERTION`;
- vulnerability analysis;
- customer binary inventory;
- supplier or distribution authorization assertions.

The verifier also compares archive member names and extracted regular-file hashes with `SOURCE_FILES.sha256`; symlink or other non-regular archive members are rejected. The file therefore documents source-archive contents; it must not be described as a complete dependency or vulnerability SBOM without a later reviewed generator and scope decision.

## Provenance identity

`RELEASE_PROVENANCE.txt` records and binds:

- release and package version;
- expected tag metadata;
- recorded actual tag string, when supplied;
- source commit string and explicit `CLEAN`, `DIRTY`, or `UNKNOWN` worktree state;
- ABI line, public-header count, and public-symbol count;
- source archive, source hash manifest, SPDX source inventory, public-header manifest, exported-symbol list, and hosted disposition hashes;
- unsigned signing status;
- release authorization TODO state.

A recorded tag string or commit string is not an attestation. An untagged engineering bundle may record a dirty worktree, but a recorded actual release tag or completed hosted disposition requires a known commit and `CLEAN` worktree state. Existing strict Git checks remain responsible for proving that expected tag, supplied commit, and checked-out HEAD resolve to one commit in a real Git work tree.

The root `.gitignore` excludes only generated build and hosted-review work directories using anchored patterns. This prevents an in-tree CI build directory from making an otherwise committed checkout appear dirty, while ordinary untracked source paths remain visible to Git. Files intentionally placed under an excluded generated directory are outside the reviewed source-archive scope.

## Hosted artifact disposition

The default bundle contains an explicit `TODO` hosted-artifact disposition. A completed `PASS` record is accepted only when it includes:

- the same release version and expected tag;
- a non-placeholder reviewer;
- a UTC review timestamp;
- a workflow commit matching the provenance commit;
- a provenance worktree state of `CLEAN`;
- `PASS` result fields.

This mechanical check does not perform the human platform, compiler-output, ABI, or side-channel review itself.

## Signing boundary

P1-31 signing mode is `UNSIGNED`.

Rules:

- No private key, signing token, certificate private key, keystore, or credential is stored in source, evidence, or the PoC package.
- No signature file is accepted merely because it exists.
- No `SIGNED`, `VERIFIED`, or external-release authorization status is accepted by the P1-31 unsigned checker.
- A future signing step must use an approved detached-signature tool and independently verify the signature over `RELEASE_ARTIFACTS.sha256`.
- Signer identity, authorization, key custody, timestamping, revocation, and verification retention require separate product/legal/infrastructure decisions.

## External release gate

The generated authorization file intentionally contains:

```text
Status: TODO
External release authorized: NO
```

Changing those lines is not sufficient. A later release procedure must require all of the following:

1. strict tag/commit/HEAD identity verification and a clean reviewed worktree;
2. completed hosted artifact disposition;
3. reviewed source SBOM scope and limitations;
4. approved signer and timestamp policy;
5. cryptographic verification of the detached signature;
6. legal/product authorization and customer-sharing scope;
7. a regenerated hash manifest after every approved metadata change.

## Claims boundary

P1-31 supports the phrases “unsigned engineering provenance bundle,” “source inventory,” and “release preparation gate.” It does not support wording that implies completed signing, completed hosted review, completed supply-chain attestation, universal ABI compatibility, official validation, or external-release approval.
