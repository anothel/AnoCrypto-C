# Release Provenance CI Checklist

Status: P3-05 unsigned engineering checklist retaining P1-33 controls. Actual tag authorization, hosted-artifact review, production signing, RFC 3161 timestamping, role approval, and external release remain incomplete.

## Configure and static policy

- Configure with release identity `0.1.0-p3-08-rsa-pkcs1-v15-sha2`.
- Run `ano_crypto_release_compatibility_policy_check`.
- Run `ano_crypto_release_provenance_policy_check`.
- Confirm the clean Release public surface is 17 headers and 83 symbols, with 69 names unchanged on `ANOCRYPTO_C_0`, HKDF-SHA-256 on `ANOCRYPTO_C_1`, two one-shot AES-256-GCM names on `ANOCRYPTO_C_2`, seven streaming names on `ANOCRYPTO_C_3`, and fixed-profile scrypt on `ANOCRYPTO_C_4`.
- Confirm signing state is `UNSIGNED`, handoff readiness is `BLOCKED`, and external authorization is `NO`.
- Confirm production signer and timestamp records remain explicitly `UNPROVISIONED` until separately approved.
- Reject key, token, credential, signature, timestamp-request, and timestamp-response artifacts from source/evidence/package paths.

## Key-free regression tests

Run `ano_crypto_release_provenance_tests` and require:

- normal unsigned bundle creation and verification;
- SPDX source-inventory generation and source archive/member/hash agreement;
- SBOM, archive, manifest, path, and checksum tamper rejection;
- unlisted file and signature-like artifact rejection;
- false signed/external-authorization claim rejection;
- dirty tagged-build and wrong-tag rejection;
- completed and incomplete hosted-disposition fixture coverage.

Run `ano_crypto_release_signing_handoff_tests` and require:

- verified key-free handoff creation;
- exact policy/subject/algorithm/encoding/fingerprint/timestamp bindings;
- handoff manifest and copied-input tamper rejection;
- production signer/TSA unprovisioned state -> `BLOCKED`;
- missing role identity -> `BLOCKED`;
- identical non-placeholder Security Reviewer/Product Release Owner identity rejection;
- no private key, signing token, signature, or RFC 3161 request/response retained.

Run `ano_crypto_sensitive_release_artifact_tests` and require synthetic
source-archive, installed-SDK, PoC, and evidence roots to reject prohibited
credential suffixes, generic private-key armor, token assignments, and
timestamp/signature fixture files.

These tests use public repository records only and create no production cryptographic material.

## Opt-in OpenSSL fixture

Run `ano_crypto_release_signing_fixture_tests` only with:

```text
ANO_CRYPTO_BUILD_TESTS=ON
ANO_CRYPTO_ENABLE_SIGNING_TEST_FIXTURES=ON
CMAKE_BUILD_TYPE=Debug
```

Require:

- ephemeral Ed25519 signing and raw 64-byte detached-signature verification;
- signer fingerprint verification as SHA-256 of DER SubjectPublicKeyInfo;
- RFC 3161 timestamp verification over the detached-signature file;
- negative coverage for wrong signature length/content, mismatched public key/fingerprint, timestamp policy OID/trust anchor mismatch, and malformed timestamp response;
- cleanup of generated key, certificate, signature, timestamp request, and timestamp response files.

Configuration must reject fixture enablement for Release, RelWithDebInfo, MinSizeRel, or tests-disabled builds. Fixture artifacts are never evidence or production credentials.

## Evidence target

Require the release-evidence dependency chain to prepare and verify prerequisites before final collection:

- clean Release symbol/runtime-dependency report;
- installed-package smoke;
- PoC-package smoke;
- tagged-compatibility smoke;
- full CTest result;
- prepared provenance bundle/check;
- key-free signing handoff/check;
- complete `prepared-release-evidence/p3-08/EVIDENCE_MANIFEST.sha256` status tree.

Final evidence must contain:

- `release-provenance/RELEASE_PROVENANCE.txt`;
- `release-provenance/SOURCE_SBOM.spdx.json`;
- `release-provenance/RELEASE_ARTIFACTS.sha256`;
- `release-provenance/SIGNING_STATUS.txt` with `Status: UNSIGNED`;
- `release-signing-handoff/SIGNING_HANDOFF_STATUS.txt` with `Readiness: BLOCKED`;
- `RELEASE_PROVENANCE_CHECK.txt` and `RELEASE_SIGNING_HANDOFF_CHECK.txt` with `Result: PASS`;
- `SENSITIVE_ARTIFACT_SCAN.txt` with `Result: PASS`;
- release authorization records with `External release authorized: NO`.

Confirm evidence manifests cover the copied provenance and handoff directories. Confirm the final collector only validates/copies the prepared manifest tree and launches no configuration/build/CTest/package/policy/status-test subprocess.

## Compiler and dependency coverage

- Run full Release configure/build/CTest/evidence with GCC and Clang.
- Run the OpenSSL-unavailable Release lane and require product build/tests to pass without OpenSSL differential/fixture tests.
- Run tests-off, thread-safety-off, sanitizer, install, PoC, and clean symbol lanes required by the release plan.
- Inspect clean Release runtime dependencies and require no `libcrypto` or `libssl` dependency in the product library.
- Preserve the 69-symbol ABI-0 baseline and verify the current 83-symbol additive manifest and ELF version nodes where supported.

## Hosted bundle

The hosted provenance lane may upload status/hash metadata and source inventory. It must not upload private keys, signing tokens, certificates with private keys, signatures, timestamp requests/responses, customer data, unpublished deployment data, entropy, seed, DRBG state, generated random values, AES values, or CMAC tags.

The hosted artifact index requires the configured release-provenance and signing-handoff categories. Presence, hash, workflow, or GitHub attestation does not complete human disposition and is secondary to the approved detached-signature path.

## P1-33 rehearsal and hosted-review regression

- Run the tagged-release rehearsal only in an isolated clean Git checkout with a synthetic local tag.
- Require tag, supplied commit, and `HEAD` equality; reject dirty state, moved tag, moved `HEAD`, commit mismatch, and ref mismatch.
- Require explicit Linux, Windows, macOS, Linux 32-bit, s390x big-endian, GCC/Clang compiler-output, OpenSSL differential, release-provenance, and signing-handoff categories.
- Bind artifact hash, workflow commit/ref, platform, compiler, architecture, reviewer, UTC review date, disposition, and actual-versus-synthetic origin.
- Reject missing or duplicate lanes, unlisted files, hash mismatch, traversal, symlinks, oversized or unexpected binary-like files, sensitive markers, missing reviewer/date, conflicting approval roles, and false `PASS`/`APPROVED` status.
- Keep no-run hosted results `NOT_RUN`, unreviewed downloaded artifacts `UNREVIEWED`, signing `UNSIGNED`, handoff `BLOCKED`, and external authorization `NO`.

## PoC boundary

- Include package-specific `RELEASE_PROVENANCE_PACKAGE_NOTES.txt` only.
- Exclude internal source archive/SBOM, handoff, signing request, production signer/TSA records, compatibility baselines, hosted-review tooling, and internal evidence logs.
- Keep package `SBOM_STATUS.txt` and `SIGNING_STATUS.txt` accurate.
- Run the sensitive-artifact scan across evidence, installed SDK, and PoC package before sharing.

## Manual work still required

- actual Git tag/commit/`HEAD` authorization and clean-worktree confirmation;
- review of actual hosted Windows, macOS, Linux 32-bit, and s390x artifacts;
- source-SBOM downstream schema/tool and dependency/vulnerability scope review;
- production signer identity, key custody, SPKI fingerprint, revocation, and hardware-backed operation provisioning;
- RFC 3161 TSA policy OID, trust anchor, service owner, and revocation review provisioning;
- distinct Security Reviewer and Product Release Owner identities/approvals;
- cryptographic verification record, legal/product approval, and external-release authorization.
