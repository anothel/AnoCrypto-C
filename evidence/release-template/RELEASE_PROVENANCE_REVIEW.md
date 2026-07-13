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
| SPDX 2.3 source inventory | TODO | Exact path/SHA-256/SHA-1 records; source files only; no dependency/vulnerability analysis |
| Public-header manifest | TODO | 14 reviewed headers |
| Exported-symbol list | TODO | 69-symbol ABI-0 line |
| Git worktree state | TODO | Recorded tag or completed hosted disposition requires `CLEAN` |
| Hosted artifact disposition | TODO | Non-placeholder reviewer/date/commit-bound `PASS` required |
| Provenance bundle verification | TODO | `RELEASE_PROVENANCE_CHECK.txt` |
| Signing handoff verification | TODO | `RELEASE_SIGNING_HANDOFF_CHECK.txt`; verifies metadata, not a signature |
| Sensitive artifact scan | TODO | Evidence, installed SDK, and PoC package must pass |

## Offline signing and authorization

| Item | Result | Evidence/limitations |
|---|---|---|
| Repository signing status | UNSIGNED | No production detached signature exists |
| Handoff readiness | BLOCKED | Production signer/TSA, hosted/tag state, and approver identities remain incomplete |
| Signature subject | POLICY SET | `RELEASE_ARTIFACTS.sha256` only |
| Detached-signature profile | POLICY SET | Ed25519, raw 64 bytes, `RELEASE_ARTIFACTS.sha256.ed25519` |
| Approved signer fingerprint | UNPROVISIONED | SHA-256 over DER SubjectPublicKeyInfo required |
| Production key custody | UNPROVISIONED | Offline hardware-backed custody required; no key/token belongs in repository, CI, evidence, or PoC package |
| RFC 3161 timestamp | UNPROVISIONED | Required over detached-signature file; TSA policy/trust anchor not provisioned |
| Security Reviewer identity | UNPROVISIONED | Must become stable, non-placeholder identity |
| Product Release Owner identity | UNPROVISIONED | Must become stable, non-placeholder identity different from Security Reviewer |
| Cryptographic signature verification | NOT PERFORMED | Fixture verification does not verify a production signature |
| GitHub attestation | SECONDARY ONLY | Never trust root or detached-signature replacement |
| External release authorization | NO | Authorization handoff remains `BLOCKED` |

## Fixture boundary

Debug-only opt-in OpenSSL fixtures may generate ephemeral test keys, signatures, certificates, and RFC 3161 messages. Confirm tests remove them. Never retain them as evidence or treat them as production signer/TSA provisioning.

## Claims boundary

Do not treat a hash manifest, source inventory, expected tag, workflow artifact, GitHub attestation, policy record, or passing fixture as proof of production signing, completed hosted review, external-release authorization, universal compatibility, side-channel assurance, or official validation.
