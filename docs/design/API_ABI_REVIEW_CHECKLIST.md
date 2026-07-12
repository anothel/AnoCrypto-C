# AnoCrypto-C API/ABI Review Checklist

Status: P1-28 engineering draft.

## Identity

| Item | Expected | Status |
|---|---|---|
| Product | `AnoCrypto-C` | OK |
| CMake package | `AnoCryptoC` | OK |
| Imported target | `AnoCryptoC::AnoCryptoC` | OK |
| Library | `anocrypto_c` | OK |
| Include root | `include/anocrypto/` | OK |
| C API/macro prefix | `ano_crypto_` / `ANO_CRYPTO_` | OK |

## ABI rules

- Public processing functions return `ano_crypto_status_t` and use output parameters.
- Caller-owned contexts are opaque and sized through query functions.
- Public enum values are explicit and append-only within the current ABI line.
- Short buffers, NULL/zero-length conventions, overlap, and lifecycle errors are tested.
- The build-identity ABI version 1 layout is recorded separately and unchanged.
- Release-style builds hide internal symbols where supported.
- Shared library remains the first candidate module boundary.

## Current public processing headers

| Header | Current scope |
|---|---|
| `ac_hash.h` | SHA-256/384/512 one-shot and streaming. |
| `ac_hmac.h` | HMAC-SHA-2 one-shot/streaming and HMAC-purpose handle path. |
| `ac_drbg.h` | Explicit-entropy HMAC_DRBG-SHA-256 core. |
| `ac_random.h` | Source-backed public random lifecycle on supported provider/process-marker branches. |
| `ac_cipher.h` | AES-128/192/256 CBC without padding; eight functions. |
| `ac_cmac.h` | AES-CMAC full-tag compute/verify/streaming; eight functions. |
| `ac_key.h` | In-memory HMAC, block-cipher, and CMAC key purposes; export/persistence deferred. |

## P1-28 symbol decision

P1-28 adds `ac_cmac.h`, `ANO_CRYPTO_KEY_TYPE_CMAC = 7`, and exactly eight
`ano_crypto_cmac_*` functions. The clean Release ABI-0 baseline grows from 61
to 69 names. No existing symbol is removed or renamed. Internal CMAC structures,
OpenSSL provider details, and test hooks are not public ABI.

## Required local checks

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DANO_CRYPTO_BUILD_EXAMPLES=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --build build --target ano_crypto_public_surface_check
cmake --build build --target ano_crypto_cmac_policy_check
cmake --build build --target ano_crypto_exported_symbol_baseline_check
cmake --build build --target ano_crypto_poc_package_smoke
```

Symbol-name equality is only one ABI guardrail. Calling convention, public type
size/alignment, loader metadata, semantic behavior, and target-specific support
remain separate review items.
