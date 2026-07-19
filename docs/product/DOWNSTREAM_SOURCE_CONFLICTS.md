# Downstream Source Conflicts and Drift

## `SRC-CONFLICT-ASK-001` — AnoSecureKit Community identity

### P3-01 intake record

AnoSecureKit Enterprise revision `3a62318...` recorded:

- vendored Community tree `9b498f5431f217ac71e8d2e2c92df9e8b0122626`;
- pinned upstream commit `102965bee601f2adfb1b624e8425d1c95aaa8c8d`;
- a conflict between that mapping and the separately supplied `v0.4.0` source.

The separate Community pack identified `v0.4.0` as commit
`694459ebe497d15ba75ef76a52fa7c36ddd7bcce`, tree
`5478a5c37e96aa1017f8f4604234de48c1115d7c`.

The P3-01 comparison found the same 182 paths, with 181 line-ending-normalized
matches and one semantic difference in `src/internal/secure_wipe.hpp`. The
historical state was `OPEN_DOWNSTREAM_SYNC_DECISION`.

### P3-06 follow-up disposition

AnoSecureKit Enterprise P3-06 adopted the separately supplied exact Community
source content. A new line-ending-normalized comparison reports 182/182 path
matches with no remaining semantic source difference.

Current disposition:

- transported source-content discrepancy: `CLOSED_P3_06`;
- exact upstream Git-tree object reproduction: `EXTERNAL_CONFIRMATION_REQUIRED`;
- trusted release provenance acceptance: `OPEN`;
- customer release authorization: `NO`;
- aggregate state: `CONTENT_RECONCILED_PROVENANCE_CONFIRMATION_OPEN`.

This is not a claim that the original upstream Git object was independently
reconstructed, signed, timestamped, or approved for customer release. Historical
P3-02 through P3-05 compatibility evidence may retain the P3-01 state because it
records the state at the time of each core slice.

## `SRC-DRIFT-PKI-001` — stale embedded revision labels

The AnoPKI collection manifests bind Community `ae3ddee...` and Enterprise
`68b379e...`. Some selected source-derived documents still contain older markers
such as `71f0e898...` or `152a657c...`.

Decision:

- exact source ZIPs and collection manifests are authoritative;
- requirement rows are validated against the exact source paths and tests;
- stale labels are downstream documentation debt, not AnoCrypto-C feature
  authority.
