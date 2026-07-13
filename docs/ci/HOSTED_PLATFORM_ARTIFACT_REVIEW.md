# Hosted Platform Artifact Review

Status: P1-33 review model. Actual hosted results are `NOT_RUN` or `UNREVIEWED` unless a referenced workflow artifact has been downloaded, verified, and dispositioned by a named reviewer.

Use this table only after the corresponding workflow run has completed, its `HOSTED_ARTIFACT_MANIFEST.txt` has been verified after download, and the final hosted artifact index has been generated. Workflow YAML or artifact presence alone is not a completed review.

## Required lane/platform model

| Lane | Platform | Architecture | Compiler | Current hosted state |
|---|---|---|---|---|
| `platform-thread-safety-ON` | Linux | runner-default | cmake-configured | NOT_RUN |
| `platform-thread-safety-ON` | Windows | runner-default | cmake-configured | NOT_RUN |
| `platform-thread-safety-ON` | macOS | runner-default | cmake-configured | NOT_RUN |
| `clean-release-symbol-compatibility` | Linux | runner-default | cmake-configured | NOT_RUN |
| `clean-release-symbol-compatibility` | Windows | runner-default | cmake-configured | NOT_RUN |
| `clean-release-symbol-compatibility` | macOS | runner-default | cmake-configured | NOT_RUN |
| `tagged-release-compatibility` | Linux | runner-default | cmake-configured | NOT_RUN |
| `tagged-release-compatibility` | Windows | runner-default | cmake-configured | NOT_RUN |
| `tagged-release-compatibility` | macOS | runner-default | cmake-configured | NOT_RUN |
| `linux-32bit-portability` | Linux | x86 | gcc-m32 | NOT_RUN |
| `linux-s390x-big-endian` | Linux | s390x | gcc-s390x-linux-gnu | NOT_RUN |
| `aes-cmac-compiler-output-gcc` | Linux | x86_64 | gcc | NOT_RUN |
| `aes-cmac-compiler-output-clang` | Linux | x86_64 | clang | NOT_RUN |
| `openssl-differential` | Linux | x86_64 | system-default | NOT_RUN |
| `release-provenance` | Linux | x86_64 | gcc | NOT_RUN |
| `release-signing-handoff` | Linux | x86_64 | gcc | NOT_RUN |

The clean-checkout tag rehearsal is `SYNTHETIC_LOCAL` evidence and is reviewed separately. It never changes a hosted state above to `PASS`, `APPROVED`, or `REVIEWED`. Additional Clang provenance/signing bundles may be retained as supplemental records; they do not replace any required tuple.

## Per-artifact review fields

Each actual hosted record must contain artifact SHA-256, workflow run id, workflow commit, workflow ref/tag, platform, architecture, compiler, lane, reviewer identity, UTC review date, disposition, limitations, and origin `HOSTED_RUNNER`. Downloaded but not dispositioned artifacts remain `UNREVIEWED`. Synthetic fixtures use `SYNTHETIC_LOCAL`; no workflow execution remains `NOT_RUN`.

## Manifest and index procedure

1. Each lane creates a text-only bundle with `tools/create_hosted_artifact_manifest.cmake`.
2. The same lane verifies the bundle with `tools/check_hosted_artifact_manifest.cmake`.
3. A final job downloads required bundles, verifies every manifest again, checks commit/ref and required lane uniqueness, and writes the hosted artifact index and SHA-256 list. The disposition records SHA-256 for both files.
4. A later review workflow must validate the complete structured index and record reviewer authority, UTC review date, disposition, and limitations before platform-support or compatibility wording changes. P1-33-generated provenance remains explicitly `UNREVIEWED`.

Missing or duplicate lanes, commit/ref mismatch, an unlisted file, artifact hash mismatch, traversal, symlink, oversized file, unexpected binary-like file, sensitive marker, missing reviewer/date, conflicting approval roles, or false `PASS`/`APPROVED` state fails closed.

Compiler-output presence is not a completed timing/cache or side-channel review. Expected-tag metadata is not release provenance until the tag, supplied commit, and checked-out `HEAD` are verified together.

## Signing-handoff boundary

Hosted artifact review is a prerequisite input to the offline-signing handoff, not release authorization. Until every required actual hosted record is reviewed, the disposition is incomplete. Current production signer and RFC 3161 TSA policy remain `UNPROVISIONED`; the repository is `UNSIGNED`; handoff readiness is `BLOCKED`; external release authorization is `NO`.

GitHub attestation may be retained as optional secondary evidence. It is not the trust root and does not replace verification of the raw 64-byte Ed25519 signature over `RELEASE_ARTIFACTS.sha256`, the approved signer fingerprint (SHA-256 over DER SubjectPublicKeyInfo), or the RFC 3161 timestamp over the signature file. Security Reviewer and Product Release Owner must be stable, non-placeholder, different identities when approval is later recorded.

A hosted artifact reviewer does not automatically fill either release-authorization role. If one person is assigned conflicting Security Reviewer and Product Release Owner approvals, authorization remains blocked.

Artifacts must remain status-only. Do not attach entropy captures, generated random bytes, key material, MAC/CMAC output, seed material, DRBG state, private keys, signing material, detached signatures, timestamp requests/responses, tokens, customer data, or internal deployment details.
