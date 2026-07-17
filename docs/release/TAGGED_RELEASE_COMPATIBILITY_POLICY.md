# Tagged Release Compatibility Policy

Status: P1-33 engineering policy. This document does not establish universal ABI compatibility or release provenance by itself.

## Purpose

P1-33 retains a project-owned compatibility input from `0.1.0-p1-29-cmac-hardening` and checks the next engineering slice without adding public cryptographic services. The policy covers release identity consistency, the retained public C surface, installed-package consumption, isolated synthetic tag rehearsal, and status-only hosted artifacts.

## Reviewed identity

The authoritative engineering metadata is `release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake`:

- package version: `0.1.0`
- engineering version: `0.1.0-p2-12-consultation-contact-readiness`
- expected tag metadata: `v0.1.0-p2-12-consultation-contact-readiness`
- ABI line: `ANOCRYPTO_C_0`
- retained baseline: `0.1.0-p1-29-cmac-hardening`
- public headers: 14
- public exports: 69
- CMake package compatibility: `SameMinorVersion`

The expected tag is metadata until a Git-backed check proves that the tag, supplied commit, and checked-out `HEAD` identify one commit. Tag existence, signer identity, and release authorization remain separate reviews.

## Frozen compatibility input

`abi/baselines/0.1.0-p1-29-cmac-hardening/` contains exact project-owned P1-29 public headers, their SHA-256 manifest, and the reviewed exported-symbol list. It is test/evidence input and is not installed or copied into the customer PoC package.

Baseline updates are not automatic. A later intentional public API or ABI change requires a separate decision, symbol delta review, source-compatibility review, documentation update, and new baseline identifier.

## Automated guardrails

`ano_crypto_release_compatibility_policy_check` verifies:

- central release metadata and current version macros agree;
- the baseline and current public-header hash manifests contain 14 canonical-LF text hashes, independent of checkout/archive line endings;
- normalized public function declarations, public types, and non-version constants match P1-29;
- both stored exported-symbol lists contain the same 69 names;
- package compatibility generation uses the reviewed policy;
- release/evidence/CI identity strings are coherent.

`ano_crypto_tagged_release_compatibility_smoke` additionally builds a clean installed package and runs:

- a P1-33-header/P1-33-library consumer;
- a P1-29-header/P1-33-library consumer that binds all 69 public functions;
- an exact `find_package(AnoCryptoC 0.1.0 EXACT)` probe;
- a negative request confirming the 0.1.x package does not satisfy `0.2.0`.

## Compatibility boundary

A passing result is evidence for the tested source snapshot, compiler, target, loader, and runtime only. It does not prove:

- compatibility on an untested compiler, architecture, calling convention, or loader;
- semantic compatibility for every legal argument sequence;
- compatibility with applications that persisted opaque context bytes or hardcoded queried sizes;
- side-channel properties, official validation status, or product suitability;
- authenticity of an unsigned archive or unverified tag.

Opaque caller-owned contexts must always be sized through the loaded library and must not be serialized or persisted.

## Clean tagged-release rehearsal

The P1-33 rehearsal uses an isolated clean clone or equivalent temporary Git repository. A synthetic local tag may be created there only. The user repository is not tagged or modified.

The rehearsal must prove that the expected tag, supplied commit, tag-resolved commit, and checked-out `HEAD` identify one commit and that the checkout is clean. Dirty worktrees, a moved tag, a moved `HEAD`, ref mismatch, or commit mismatch fail closed. A successful rehearsal is local procedure evidence; it is not proof that an actual release tag exists, is signed, or is authorized.

## Artifact handling

Hosted bundles are text-only status artifacts with a manifest recording release identity, lane, platform, architecture, compiler, workflow commit/ref, artifact origin, file size, and SHA-256. Review records add reviewer, UTC review date, disposition, and limitations. Manifest verification establishes bundle integrity after creation, not human approval of the underlying platform result.

Actual GitHub-hosted results and synthetic/local fixtures must remain distinct. Missing hosted execution is `NOT_RUN`; downloaded but unreviewed output is `UNREVIEWED`. Synthetic fixtures may report a mechanical regression pass but may not be recorded as hosted `PASS` or `APPROVED` evidence.

Do not place keys, entropy, seed material, DRBG state, generated random bytes, private scalars, authentication tags, signing material, tokens, customer data, or internal deployment details in a bundle.
