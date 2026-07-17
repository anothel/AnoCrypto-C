# Downstream Source Requests

Status: `READY_TO_COLLECT`

## Required delivery format

For each repository provide:

- one latest clean source ZIP including tests and repository documents;
- exact repository name, branch, commit and tag/base version;
- one CURRENT ONLY pack created from that same revision;
- SHA-256 for the ZIP and pack when available;
- a statement that historical packs must not be mixed with the delivery.

Do not provide credentials, customer data, private keys, build caches or release
signing material.

## Minimal collection order

1. AnoSecureKit Enterprise
2. AnoSecureKit Community baseline consumed by that Enterprise revision
3. AnoPKI Enterprise
4. AnoPKI Community baseline consumed by that Enterprise revision

The Enterprise and its consumed Community baseline should be collected as a
matched pair. A newer unrelated Community main snapshot is not a substitute for
the exact consumed baseline.

## Request prompts

Use the repository-specific prompts under `docs/product/requests/`.
