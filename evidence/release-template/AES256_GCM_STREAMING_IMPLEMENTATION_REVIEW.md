# AES-256-GCM Streaming Implementation Review

Status: `P3_04_LOCAL_EVIDENCE`

- API gate: seven generic streaming functions in existing `ac_aead.h`.
- Context storage: `malloc`/equivalently aligned; misaligned input is rejected.
- State: AAD -> payload -> terminal finalization; zero-length update is a no-op.
- CSP lifetime: init snapshots a live key; handle destroy blocks new init but does not revoke that context.
- Decrypt update: explicitly unverified output; caller buffering/wipe required.
- Buffer contract: exact in-place payload, disjoint `out_written`, failure zeroing.
- ABI: 69 ABI-0 + 1 ABI-1 + 2 ABI-2 + 7 ABI-3 = 79 symbols.
- Evidence: NIST chunk splits, negative/state, OpenSSL differential, exact SKT1.
- Product formats/workflows: excluded from production implementation.
- External status: unchanged and not release-authorized.
