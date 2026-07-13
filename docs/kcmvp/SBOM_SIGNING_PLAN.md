# SBOM and Signing Plan

Status: P1-32 unsigned engineering plan. Repository state: `UNSIGNED`; external release: `NO`; offline-signing handoff: `BLOCKED`.

## Purpose

AnoCrypto-C records source-archive contents and release hash bindings, then prepares a key-free offline-signing handoff. Private signing keys, signing tokens, customer data, unpublished deployment details, detached signatures, and timestamp protocol artifacts are excluded from repository, evidence, and customer PoC paths.

P1-32 implements policy, handoff generation/verification, and opt-in test fixtures. It does not provision a production signer/TSA, sign a release, timestamp a production signature, approve hosted artifacts, authorize a tag, or authorize external release.

## Source SBOM scope

Generated artifact:

```text
release-provenance/SOURCE_SBOM.spdx.json
```

Format and scope:

- SPDX 2.3 JSON source inventory;
- exact repository-relative regular files selected for the source archive;
- SHA-256 and SHA-1 per-file checksums;
- SPDX package verification code from sorted per-file SHA-1 values;
- one AnoCrypto-C source-package record;
- release version and generation timestamp.

Explicit limitations:

- dependency discovery and package-manager metadata import are not performed;
- vulnerability analysis is not performed;
- licenses and copyright remain `NOASSERTION` absent later legal review;
- customer binary/runtime inventory is not included;
- source inventory generation is not official validation evidence.

## Provenance and handoff

The provenance bundle contains the source archive, source hashes, SPDX inventory, public-header manifest, exported-symbol list, hosted disposition, unsigned signing status, signing request, authorization record, and complete `RELEASE_ARTIFACTS.sha256` coverage.

The provenance verifier requires safe paths, exact manifest coverage, archive/SBOM/source agreement, matching release identity, current 14-header/69-symbol public inputs, explicit hosted state, `UNSIGNED`, and external authorization `NO`.

The handoff generator accepts only a verified provenance bundle plus source-owned signer/TSA policy records. The handoff verifier re-derives readiness and verifies exact files, hashes, policy, role identity state, and forbidden-material absence. Current unprovisioned inputs correctly produce `Readiness: BLOCKED` with a passing mechanical check.

## Offline-signing profile

| Item | Policy |
|---|---|
| Subject | `RELEASE_ARTIFACTS.sha256` |
| Signature | Ed25519, raw 64-byte detached file |
| Signature filename | `RELEASE_ARTIFACTS.sha256.ed25519` |
| Public-key fingerprint | SHA-256 over DER SubjectPublicKeyInfo |
| Verification family | OpenSSL 3 `pkeyutl` |
| Timestamp | RFC 3161 over detached-signature file; required before external release |
| Key custody | offline hardware-backed production signer |
| Approval roles | Security Reviewer and Product Release Owner; stable, non-placeholder, different identities |
| GitHub attestation | optional secondary evidence only; not trust root |

Current production signer record and timestamp policy are `UNPROVISIONED`. No approved SPKI fingerprint, TSA policy OID, TSA trust anchor, service owner, or current revocation review exists in this source state.

## Test-fixture boundary

Debug-only `ANO_CRYPTO_ENABLE_SIGNING_TEST_FIXTURES=ON` may generate ephemeral OpenSSL Ed25519 and RFC 3161 fixtures. The fixture covers positive/negative verification and removes generated material. Release-like and tests-disabled configurations reject fixture enablement.

Fixture cryptographic success is test evidence only. Fixture keys, certificates, signatures, timestamp requests, and responses are not production artifacts and must not enter release evidence.

## Hosted attestation boundary

GitHub-hosted attestation may supplement a later release record. It cannot replace the raw Ed25519 signature, approved signer fingerprint, RFC 3161 timestamp, independent role approvals, or external authorization. Workflow YAML, an uploaded artifact, a Git tag string, or a hash manifest alone is not authenticated release provenance.

## PoC boundary

The customer PoC package retains package-relative hashes and package-specific SBOM/signing status notes. It excludes the internal source archive/SBOM, signing handoff/request, production signer/TSA records, release authorization record, compatibility baselines, hosted-review tooling, and internal evidence logs.

## Work required before external release

- verify expected tag, supplied commit, and checked-out `HEAD` identify one authorized clean commit;
- review actual hosted artifacts and record reviewer/date/limitations;
- review SBOM syntax/scope with selected downstream tooling;
- provision approved production signer identity, SPKI fingerprint, offline hardware-backed custody, and revocation process;
- provision RFC 3161 policy OID, trust anchor, service owner, and revocation review;
- generate and independently verify the production detached signature and timestamp;
- record distinct Security Reviewer and Product Release Owner approvals;
- confirm no key, token, signature protocol residue, entropy, seed, DRBG state, customer data, or deployment detail is present in shared artifacts;
- record legal/product authorization and regenerate hashes after authorized changes.

## P1-31 historical boundary

P1-31 introduced the SPDX source inventory and unsigned provenance bundle while leaving signature format and verifier selection open. P1-32 supersedes that tooling gap with a fixed offline profile and verifier/handoff tests. Production provisioning and release authorization remain open.
