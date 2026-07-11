# AnoCrypto-C ABI-0 Build Identity Layout

Status: P1-21 engineering ABI baseline.

This file records the field order of `ano_crypto_build_identity_t` for the 0.x ABI-0 line. It is a repository guardrail, not a broad cross-toolchain binary-compatibility guarantee.

1. `uint32_t abi_version`
2. `const char *product_name`
3. `const char *version`
4. `const char *git_commit`
5. `const char *build_type`
6. `const char *compiler`
7. `const char *target`
8. `const char *notes`

Rules:

- `abi_version` remains `1` for this exact layout.
- Fields must not be reordered, removed, or appended while `abi_version` remains `1`.
- Returned string pointers are module-owned, read-only, and valid for the loaded module lifetime.
- A future incompatible layout requires a new ABI version and a separately reviewed API boundary.
- Platform data-model differences, calling convention, compiler ABI, and loader behavior remain separate review items.
