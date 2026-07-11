# Build Identity ABI-0 Decision

Status: P1-26 engineering decision retained.

## Decision

AnoCrypto-C retains the existing public `ano_crypto_build_identity_t` layout for the 0.x ABI-0 line. P1-21 did not add field getters or change its then-current 53-symbol baseline. P1-25 adds eight reviewed cipher symbols and updates the current stored baseline to 61 names without changing the build-identity layout. P1-26 preserves that 61-symbol baseline and makes no build-identity layout change.

The structure is intentionally narrow and query-only. Its field order is recorded in `abi/ANOCRYPTO_C_0_BUILD_IDENTITY_LAYOUT.md` and checked by `ano_crypto_build_identity_abi_tests`.

## Rules

- `abi_version` is the first field and remains `1` for the recorded layout.
- Existing fields are not reordered, removed, or appended under ABI version 1.
- String pointers are owned by the loaded module, are read-only, and are not caller-freed.
- Future additional metadata should first be evaluated for a separate query API or a new ABI-versioned layout.
- Symbol-name equality alone does not prove calling-convention, type-size, semantic, or loader compatibility.

## Rationale

Replacing the structure now would add public API surface without a demonstrated customer requirement. Retaining and documenting the current layout avoids unnecessary churn while still preventing silent extension of the struct.

## Remaining review

- Confirm Windows calling convention and DLL import/export metadata in real hosted artifacts.
- Record type sizes and alignment per supported architecture before any binary-compatibility commitment.
- Revisit field getters if long-term ABI evolution requires independently extensible metadata.
