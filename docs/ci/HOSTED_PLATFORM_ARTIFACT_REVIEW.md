# Hosted Platform Artifact Review

Status: P1-30 template. Actual hosted Windows, macOS, Linux 32-bit, Linux s390x big-endian, tagged-compatibility, differential, and compiler-output review results are not recorded in this source archive.

Use this table only after the corresponding workflow run has completed, its `HOSTED_ARTIFACT_MANIFEST.txt` has been verified after download, and the final hosted artifact index has been generated. Workflow YAML or artifact presence alone is not a completed review.

| Platform | Workflow run/commit | Bundle manifest | Thread/CSP | SHA-512/AES/CMAC | Random/process marker | P1-29 header/P1-30 library | Reviewer/date | Limitations |
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

Artifacts must remain status-only. Do not attach entropy captures, generated random bytes, key material, MAC/CMAC output, seed material, DRBG state, private keys, signing material, tokens, customer data, or internal deployment details.
