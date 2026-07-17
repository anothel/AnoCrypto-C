# Downstream Source Conflicts and Drift

## `SRC-CONFLICT-ASK-001` — AnoSecureKit Community identity

AnoSecureKit Enterprise revision `3a62318...` records:

- vendored Community tree `9b498f5431f217ac71e8d2e2c92df9e8b0122626`;
- pinned upstream commit `102965bee601f2adfb1b624e8425d1c95aaa8c8d`;
- an unresolved conflict between that mapping and the current `v0.4.0` tag.

The separately supplied Community pack identifies `v0.4.0` as commit
`694459ebe497d15ba75ef76a52fa7c36ddd7bcce`, tree
`5478a5c37e96aa1017f8f4604234de48c1115d7c`.

A file-by-file comparison found the same 182 paths. After CRLF/LF normalization,
181 files are byte-identical. The only semantic difference is
`src/internal/secure_wipe.hpp`, where the separate release source exports the
wipe function and the Enterprise vendored copy does not. Provider operations,
`SKT1`/`SKF1`/`SKP1` semantics, fixtures and primitive requirements are
unchanged by that difference.

Decision:

- requirement analysis may proceed;
- exact dependency lineage remains `OPEN_DOWNSTREAM_SYNC_DECISION`;
- AnoCrypto-C must not rewrite the Enterprise dependency record;
- AnoSecureKit Enterprise should later choose and pin one exact Community commit/tree.

## `SRC-DRIFT-PKI-001` — stale embedded revision labels

The AnoPKI collection manifests bind Community `ae3ddee...` and Enterprise
`68b379e...`. Some selected source-derived documents still contain older markers
such as `71f0e898...` or `152a657c...`.

Decision:

- exact source ZIPs and collection manifests are authoritative;
- requirement rows are validated against the exact source paths and tests;
- the stale labels are downstream documentation debt, not AnoCrypto-C feature authority.
