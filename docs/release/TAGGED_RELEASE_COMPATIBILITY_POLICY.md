# Tagged Release Compatibility Policy

Status: P3-09 additive-ABI engineering policy. This document does not establish universal ABI compatibility or release provenance by itself.

## Purpose

P3-10 retains the project-owned `0.1.0-p1-29-cmac-hardening` compatibility input and every additive ABI slice through fixed-profile scrypt, retains the three reviewed RSA-2048/e=65537 PKCS#1 v1.5 SHA-2 functions on `ANOCRYPTO_C_5`, and adds one reviewed P-256 key-import function on `ANOCRYPTO_C_6`, and adds one restricted OCSP CertID SHA-1 function on `ANOCRYPTO_C_7`. The policy covers release identity consistency, the unchanged retained C surface, additive headers and symbols, installed-package consumption, isolated synthetic tag rehearsal, and status-only hosted artifacts.

## Reviewed identity

The authoritative engineering metadata is `release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake`:

- package version: `0.1.0`;
- engineering version: `0.1.0-p3-10-restricted-sha1-ocsp-certid`;
- expected tag metadata: `v0.1.0-p3-10-restricted-sha1-ocsp-certid`;
- current ABI line: `ANOCRYPTO_C_7`;
- retained baseline: `0.1.0-p1-29-cmac-hardening`;
- retained/current public headers: 14 / 17;
- retained `ANOCRYPTO_C_0` / current total exports: 69 / 84;
- CMake package compatibility: `SameMinorVersion`.

The expected tag is metadata until a Git-backed check proves that the tag, supplied commit, and checked-out `HEAD` identify one commit. Tag existence, signer identity, and release authorization remain separate reviews.

## Frozen compatibility input

`abi/baselines/0.1.0-p1-29-cmac-hardening/` contains exact project-owned P1-29 public headers, their SHA-256 manifest, and the reviewed exported-symbol list. It is test/evidence input and is not installed or copied into the customer PoC package.

Baseline updates are not automatic. A later intentional public API or ABI change requires a separate decision, symbol delta review, source-compatibility review, documentation update, and new baseline identifier.

## Automated guardrails

`ano_crypto_release_compatibility_policy_check` verifies:

- central release metadata and current version macros agree;
- the retained baseline manifest contains 14 canonical-LF header hashes and the current manifest contains 17;
- normalized declarations, public types, and non-version constants for the retained 14 headers match P1-29;
- `ANOCRYPTO_C_0` retains 69 names; `ANOCRYPTO_C_1` contains HKDF; `ANOCRYPTO_C_2` contains two one-shot GCM symbols; `ANOCRYPTO_C_3` contains seven streaming symbols; `ANOCRYPTO_C_4` contains fixed-profile scrypt; and `ANOCRYPTO_C_5` contains only RSA import, sign, and verify; `ANOCRYPTO_C_6` contains only P-256 import; `ANOCRYPTO_C_7` contains only restricted OCSP CertID SHA-1, for 85 exports total;
- package compatibility generation uses the reviewed policy;
- release/evidence/CI identity strings are coherent.

`ano_crypto_tagged_release_compatibility_smoke` additionally builds a clean installed package and runs:

- a current-header/current-library consumer that binds all 84 public functions and exercises the additive primitive APIs;
- a P1-29-header/current-library consumer that binds all 69 retained public functions;
- an exact `find_package(AnoCryptoC 0.1.0 EXACT)` probe;
- a negative request confirming the 0.1.x package does not satisfy `0.2.0`.

## Compatibility boundary

A passing result is evidence for the tested source snapshot, compiler, target, loader, and runtime only. It does not prove compatibility on untested environments, semantic compatibility for every legal argument sequence, compatibility with persisted opaque context bytes, side-channel properties, official validation status, or product suitability. Opaque caller-owned contexts must be sized through the loaded library and must not be serialized or persisted.

## Clean tagged-release rehearsal

The P3-09 rehearsal uses an isolated clean clone or equivalent temporary Git repository. A synthetic local tag may be created there only. The user repository is not tagged or modified.

The rehearsal must prove that the expected tag, supplied commit, tag-resolved commit, and checked-out `HEAD` identify one commit and that the checkout is clean. Dirty worktrees, moved refs, or commit mismatch fail closed. A successful rehearsal is local procedure evidence; it is not proof that an actual release tag exists, is signed, or is authorized.

## Artifact handling

Hosted bundles are text-only status artifacts. Actual hosted results and synthetic/local fixtures remain distinct. Missing hosted execution is `NOT_RUN`; downloaded but unreviewed output is `UNREVIEWED`. Do not place keys, entropy, seed material, DRBG state, generated random bytes, private key components, signatures, credentials, tokens, customer data, or internal deployment details in a bundle.
