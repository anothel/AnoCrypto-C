# Hosted Platform Artifact Review

Status: P1-32 template. Actual hosted Windows, macOS, Linux 32-bit, Linux s390x big-endian, tagged-compatibility, differential, compiler-output, and release-provenance review results are not recorded in this source archive.

Use this table only after the corresponding workflow run has completed, its `HOSTED_ARTIFACT_MANIFEST.txt` has been verified after download, and the final hosted artifact index has been generated. Workflow YAML or artifact presence alone is not a completed review.

| Platform | Workflow run/commit | Bundle manifest | Thread/CSP | SHA-512/AES/CMAC | Random/process marker | P1-29 header/P1-32 library | Reviewer/date | Limitations |
|---|---|---|---|---|---|---|---|---|
| Linux 64-bit | TODO | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| Linux 32-bit | TODO | TODO | TODO | TODO | provider disabled in portability lane | TODO | TODO | TODO |
| Linux s390x big-endian | TODO | TODO | TODO | TODO | provider disabled in portability lane | TODO | TODO | TODO |
| macOS | TODO | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| Windows | TODO | TODO | TODO | TODO | TODO | TODO | TODO | TODO |

## Manifest and index procedure

1. Each lane creates a text-only bundle with `tools/create_hosted_artifact_manifest.cmake`.
2. The same lane verifies the bundle with `tools/check_hosted_artifact_manifest.cmake`.
3. A final job downloads required bundles, verifies every manifest again, and writes the hosted artifact index and SHA-256 list.
4. A reviewer records disposition before platform-support or compatibility wording changes.

Compiler-output presence is not a completed timing/cache or side-channel review. Expected-tag metadata is not release provenance until the tag, supplied commit, and checked-out `HEAD` are verified together.

## Signing-handoff boundary

Hosted artifact review is a prerequisite input to the offline-signing handoff, not release authorization. Current production signer and RFC 3161 TSA policy remain `UNPROVISIONED`; the repository is `UNSIGNED`; handoff readiness is `BLOCKED`; external release authorization is `NO`.

GitHub attestation may be retained as optional secondary evidence. It is not the trust root and does not replace verification of the raw 64-byte Ed25519 signature over `RELEASE_ARTIFACTS.sha256`, the approved signer fingerprint (SHA-256 over DER SubjectPublicKeyInfo), or the RFC 3161 timestamp over the signature file. Security Reviewer and Product Release Owner must be stable, non-placeholder, different identities when approval is later recorded.

Artifacts must remain status-only. Do not attach entropy captures, generated random bytes, key material, MAC/CMAC output, seed material, DRBG state, private keys, signing material, detached signatures, timestamp requests/responses, tokens, customer data, or internal deployment details.
