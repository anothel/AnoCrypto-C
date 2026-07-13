# Release Provenance and Signing Policy

Status: P1-33 unsigned engineering policy. External release authorized: NO. The repository is `UNSIGNED`; the offline-signing handoff is `BLOCKED` because the production signer, RFC 3161 timestamp policy, required role identities, actual tag/hosted review, and product release authorization are not provisioned or completed.

## Purpose

P1-33 retains the P1-32 key-free offline-signing profile, binds the reviewed source archive and public compatibility inputs into a self-contained provenance bundle, and rehearses tag identity only in an isolated synthetic checkout. The generators and verifiers make missing, stale, or tampered metadata visible. They do not create an actual tag, sign a release, or authorize distribution.

The generated records are engineering evidence. They are not substitutes for:

- tag, commit, and hosted-artifact authorization;
- production key provisioning or offline signature generation;
- RFC 3161 timestamp issuance and verification;
- Security Reviewer and Product Release Owner approval;
- legal/product release approval;
- official validation evidence.

## Provenance bundle

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

`RELEASE_ARTIFACTS.sha256` covers every other file in the provenance bundle. It is the only detached-signature subject. The provenance checker requires complete manifest coverage, safe relative paths, current release identity, source-archive/SBOM agreement, 14 public headers, 69 ABI-0 symbols, `UNSIGNED` signing state, and external authorization `NO`.

## Source SBOM boundary

P1-33 generates an SPDX 2.3 JSON source inventory for the exact regular-file list used to create `anocrypto-c-source.tar.gz`.

Included:

- repository-relative file paths;
- SHA-256 and SHA-1 checksums bound to each SPDX file record;
- one AnoCrypto-C source-package record;
- SPDX package verification code computed from sorted per-file SHA-1 values;
- release version and generation time.

Not performed:

- package-manager or transitive dependency discovery;
- license conclusions beyond `NOASSERTION`;
- vulnerability analysis;
- customer binary or runtime inventory;
- supplier, distribution, or official-validation assertions.

The verifier compares archive member names and extracted regular-file hashes with `SOURCE_FILES.sha256`. Symlinks and other non-regular members are rejected. Do not describe this source inventory as a complete dependency or vulnerability SBOM.

## Provenance identity and hosted disposition

`RELEASE_PROVENANCE.txt` binds the release identity, expected and recorded tag, source commit, `CLEAN`/`DIRTY`/`UNKNOWN` worktree state, source archive and inventory, public-header manifest, exported-symbol list, hosted disposition, signing status, and authorization state.

A tag or commit string is not an attestation. A recorded release tag or completed hosted disposition requires a known commit and `CLEAN` worktree. Strict Git checks must prove that expected tag, supplied commit, and checked-out `HEAD` resolve to one reviewed commit.

The default hosted execution state is `NOT_RUN`. Downloaded but undispositioned output is `UNREVIEWED`. `PASS` and `APPROVED` are not hosted-review disposition values. The manifest schema reserves `ACCEPTED_WITH_LIMITATIONS` and `REJECTED` for a later named-review workflow, but the P1-33 provenance importer accepts only the explicit `UNREVIEWED` state. Mechanical verification does not perform human platform, compiler-output, ABI, side-channel, or product review.

## Rehearsal and hosted-review boundary

An isolated synthetic local tag may exercise the release procedure, including clean-worktree and tag/commit/`HEAD` equality checks. It does not become actual tag evidence and must be recorded as `SYNTHETIC_LOCAL`. Dirty worktrees, moved tags, moved `HEAD`, commit/ref mismatch, duplicate or missing required lanes, unlisted files, hash mismatch, traversal, symlink, oversized or unexpected binary-like files, sensitive markers, incomplete reviewer metadata, conflicting approval roles, and false `PASS`/`APPROVED` claims are rejected.

Hosted records must bind artifact hashes to workflow commit/ref, platform, compiler, and architecture. P1-33 imports a supplied disposition into provenance only when matching `HOSTED_ARTIFACT_INDEX.txt` and `HOSTED_ARTIFACT_FILES.sha256` hashes accompany an explicit `UNREVIEWED` record; the default no-run record uses `NOT_RUN` sentinels. Promotion to a reviewed disposition is deferred until a later schema validates the complete structured index, reviewer authority, and review record. Synthetic artifacts cannot satisfy hosted review. The signing handoff remains `BLOCKED` and external release remains `NO`.

## Offline-signing profile

Source-owned policy: `release/ANOCRYPTO_C_RELEASE_SIGNING_POLICY.cmake`.

| Field | Required value |
|---|---|
| Signature subject | `RELEASE_ARTIFACTS.sha256` |
| Signature algorithm | Ed25519 |
| Detached-signature encoding | raw 64 bytes |
| Signature file | `RELEASE_ARTIFACTS.sha256.ed25519` |
| Public-key distribution form | PEM SubjectPublicKeyInfo |
| Approved-key fingerprint | SHA-256 over DER-encoded SubjectPublicKeyInfo |
| Verification tool family | OpenSSL 3 `pkeyutl` |
| Timestamp | RFC 3161 over `RELEASE_ARTIFACTS.sha256.ed25519` |
| Required approvers | Security Reviewer and Product Release Owner |
| GitHub attestation | optional secondary evidence; never trust root or replacement for detached-signature verification |

The two approver identities must be stable, non-placeholder, and different after provisioning. Recording the same identity for both roles is rejected. Role records do not authorize release by themselves.

## Key-free handoff

`tools/create_release_signing_handoff.cmake` copies only verified provenance and policy records into a hash-bound handoff. `tools/check_release_signing_handoff.cmake` re-derives readiness, checks manifest coverage and hash bindings, enforces role separation, and rejects private-key or credential material.

Current production state:

- approved production signer record: `UNPROVISIONED`;
- approved signer SPKI fingerprint: unavailable;
- RFC 3161 TSA policy and trust anchor: `UNPROVISIONED`;
- Security Reviewer identity: `UNPROVISIONED`;
- Product Release Owner identity: `UNPROVISIONED`;
- detached signature: absent;
- timestamp response: absent;
- handoff readiness: `BLOCKED`;
- external release authorization: `NO`.

No private key, signing token, keystore, detached signature, timestamp request/response, or credential may be committed, placed in evidence, or copied into the PoC package. Production signing must remain offline and hardware-backed under the approved signer policy.

## Test-fixture boundary

`ANO_CRYPTO_ENABLE_SIGNING_TEST_FIXTURES=ON` enables ephemeral OpenSSL signing and RFC 3161 fixtures only in a tests-enabled Debug build. The fixture exercises positive and negative verifier paths and removes generated key/signature/timestamp material after the test. Release, RelWithDebInfo, MinSizeRel, and tests-disabled configurations reject the option.

Fixture success proves only test-path behavior. Fixture keys, certificates, signatures, timestamp requests, and timestamp responses are not production artifacts, approval records, or release evidence.

## External release gate

The generated handoff authorization file, `release-signing-handoff/RELEASE_AUTHORIZATION.md`, remains:

```text
Status: BLOCKED
External release authorized: NO
```

External release requires all of the following outside this repository state:

1. strict tag/commit/`HEAD` identity verification and a clean reviewed worktree;
2. completed hosted-artifact disposition;
3. reviewed source-SBOM scope and limitations;
4. provisioned approved production signer and RFC 3161 policy/trust anchor;
5. verified raw Ed25519 detached signature and verified timestamp response;
6. distinct, non-placeholder Security Reviewer and Product Release Owner approvals;
7. legal/product authorization and approved customer-sharing scope;
8. regenerated hash manifests after every authorized metadata change.

## Claims boundary

P1-33 supports the phrases `unsigned engineering provenance bundle`, `source inventory`, `key-free offline-signing handoff`, `synthetic tagged-release rehearsal`, and `release preparation gate`. It does not support wording that implies an actual tag, completed signing, completed hosted review, authenticated production attestation, external-release approval, universal ABI compatibility, side-channel assurance, or official validation.
