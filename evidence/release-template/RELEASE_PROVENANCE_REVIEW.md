# Release Provenance Review

Release version: TODO
Expected tag: TODO
Source commit: TODO
Worktree state: TODO
Reviewer/date: TODO

## Mechanical checks

| Item | Result | Evidence/limitations |
|---|---|---|
| Source archive hash and content binding | TODO | Member list and extracted regular-file hashes must match `SOURCE_FILES.sha256` |
| Source-file hash manifest | TODO | Repository-relative paths only |
| SPDX 2.3 source inventory | TODO | Exact path/SHA-256/SHA-1 records; source files only; no dependency/vulnerability analysis |
| Public-header manifest | TODO | 14 reviewed headers |
| Exported-symbol list | TODO | 69-symbol ABI-0 line |
| Git worktree state | TODO | Tagged or completed hosted review requires `CLEAN` |
| Hosted artifact disposition | TODO | Must include reviewer/date/commit binding for PASS |
| Bundle tamper/path scan | TODO | `RELEASE_PROVENANCE_CHECK.txt` |
| Sensitive material scan | TODO | Status-only; review before sharing |

## Signing and authorization

| Item | Result | Evidence/limitations |
|---|---|---|
| Signing status | UNSIGNED | P1-31 generator performs no signing |
| Detached-signature tool approved | TODO | Tool and format not selected |
| Signer/key-custody policy approved | TODO | No key or token belongs in the repository |
| Cryptographic signature verification | TODO | Must be performed by a future approved verifier |
| Timestamp/revocation policy | TODO | Infrastructure/legal review required |
| External release authorization | NO | `RELEASE_AUTHORIZATION.md` remains TODO |

## Claims boundary

Do not treat a hash manifest, source inventory, expected tag string, uploaded artifact, or reviewer template as proof of signing, provenance attestation, universal compatibility, side-channel review, official validation, or external-release approval.
