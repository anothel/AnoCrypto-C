# C99 scrypt Fixed-Profile API Gate

Status: `APPROVED_P3_05`

## Decision

Expose one generic function, `ano_crypto_scrypt_derive_key`, in the existing `ac_kdf.h`. The cost profile is fixed at `N=32768`, `r=8`, `p=1`, output is 32 bytes, and callers provide exactly bounded workspace. No tunable-cost API or password-product workflow is added.

## Resource contract

- Required workspace: 33,556,544 bytes.
- Downstream compatibility ceiling: 64 MiB. The AnoCrypto-C implementation itself uses the fixed 33,556,544-byte caller workspace plus bounded stack state.
- Workspace must be aligned for `uint32_t` access and have an effective/declared type that permits that access. Storage returned by `malloc` or `calloc` satisfies both requirements; merely aligning a declared `uint8_t` array does not.
- Callers may provide a larger allocation, but only the first 33,556,544 bytes are read and zeroized. The complete supplied workspace range remains part of overlap rejection.
- The implementation performs no production heap allocation.
- The complete used workspace and derived output are zeroized on failure; workspace is zeroized after success.

## Input and alias contract

Empty password/salt are valid at the generic primitive layer. Product layers may reject empty passwords. Password and salt may overlap; output and workspace must be disjoint from all inputs and each other. Pointer ranges must not wrap.

## Ownership

AnoCrypto-C owns only the generic fixed-resource scrypt primitive. File headers, parameter serialization, password prompting/storage/normalization, framing, and output policy remain downstream-owned.
