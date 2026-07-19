# AnoCrypto-C API/ABI Review Checklist

Status: P3-04 reviewed additive ABI. The retained P1-29/P3-01 surface remains unchanged on `ANOCRYPTO_C_0`; P3-02 HKDF-SHA-256 remains on `ANOCRYPTO_C_1`; P3-03 one-shot AES-256-GCM remains on `ANOCRYPTO_C_2`; seven generic streaming functions are added on `ANOCRYPTO_C_3`.

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
| `ac_kdf.h` | RFC 5869 HKDF-SHA-256 combined extract-and-expand; one function. |
| `ac_aead.h` | AES-256-GCM one-shot encrypt/decrypt with fixed 12-byte nonce and 16-byte tag. |
| `ac_drbg.h` | Explicit-entropy HMAC_DRBG-SHA-256 core. |
| `ac_random.h` | Source-backed public random lifecycle on supported provider/process-marker branches. |
| `ac_cipher.h` | AES-128/192/256 CBC without padding; eight functions. |
| `ac_cmac.h` | AES-CMAC full-tag compute/verify/streaming; eight functions. |
| `ac_key.h` | In-memory HMAC, block-cipher, and CMAC key purposes; export/persistence deferred. |

## P1-28/P1-29 symbol decision

P1-28 adds `ac_cmac.h`, `ANO_CRYPTO_KEY_TYPE_CMAC = 7`, and exactly eight
`ano_crypto_cmac_*` functions. The clean Release ABI-0 baseline grows from 61
to 69 names. No existing symbol is removed or renamed. Internal CMAC structures,
OpenSSL provider details, and test hooks are not public ABI. P1-29 retains all
69 names and adds no public type, enum, macro, header, or function.

## Required local checks

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DANO_CRYPTO_BUILD_EXAMPLES=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --build build --target ano_crypto_public_surface_check
cmake --build build --target ano_crypto_cmac_policy_check
cmake --build build --target ano_crypto_cmac_hardening_policy_check
cmake --build build --target ano_crypto_cmac_compiler_review
cmake --build build --target ano_crypto_hkdf_sha256_policy_check
cmake --build build --target ano_crypto_aes256_gcm_policy_check
cmake --build build --target ano_crypto_aes256_gcm_streaming_policy_check
cmake --build build --target ano_crypto_clean_release_symbol_baseline_check
cmake --build build --target ano_crypto_symbol_version_check
cmake --build build --target ano_crypto_poc_package_smoke
```

The direct `ano_crypto_exported_symbol_baseline_check` target is intended for a
clean Release-style build with internal-symbol hiding enabled. Test builds expose
internal entry points for internal-unit coverage and therefore use the dedicated
`ano_crypto_clean_release_symbol_baseline_check` wrapper instead.

Symbol-name equality is only one ABI guardrail. Calling convention, public type
size/alignment, loader metadata, semantic behavior, and target-specific support
remain separate review items.


## P1-29 AES-CMAC hardening ABI review

- Exported public-symbol baseline remains 69 symbols.
- The eight public CMAC function signatures and `ANO_CRYPTO_KEY_TYPE_CMAC = 7` remain unchanged.
- The private caller-owned CMAC context extent changes as an opaque implementation detail.
- Consumers must call `ano_crypto_cmac_context_size()` against the loaded library and must not hardcode, serialize, or persist an earlier size.
- Installed headers continue to expose no CMAC context layout or internal state marker.


## P1-32 release provenance and compatibility review

- No public header, function, enum value, macro contract, or exported symbol is added or removed.
- The public header count remains 14 and the clean Release ABI-0 export count remains 69.
- Exact P1-29 public headers and symbol names are retained under `abi/baselines/0.1.0-p1-29-cmac-hardening/` as test/evidence input.
- `release/PUBLIC_HEADERS.sha256` records the current P2-11 public-header files.
- Normalized function declarations, public types, and non-version constants are compared against P1-29. Version string/prerelease macros are intentionally excluded from that equality comparison.
- A C99 consumer built with retained P1-29 headers binds all 69 functions and runs against the P1-32 installed library.
- `AnoCryptoCConfigVersion.cmake` uses package version `0.1.0` with `SameMinorVersion`; an exact 0.1.0 request is accepted and a 0.2.0 request is rejected.
- The retained baseline tree, compatibility source, hosted manifests, and release-internal metadata are not installed and are excluded from the customer PoC package.

These checks are toolchain-specific guardrails. Calling convention, loader behavior, all semantic paths, untested architectures, and applications that persist opaque context bytes remain outside the automated conclusion.

## P1-33 release rehearsal ABI review

- Historical P1-33 release metadata advanced to `0.1.0-p1-33-release-rehearsal`; package version remains `0.1.0`.
- No public header, function, enum value, non-version macro contract, type, or exported symbol is added or removed.
- Public header count remains 14; clean Release export count remains 69 on `ANOCRYPTO_C_0`.
- Normalized declarations, public types, non-version constants, and the stored symbol list remain equal to the P1-29 baseline.
- Current-header/P1-33-library and retained-P1-29-header/P1-33-library consumers bind the same 69 functions.
- Clean tag rehearsal, hosted review metadata, provenance, and signing-handoff changes remain build/test/evidence tooling, not public ABI.


## P2-02 boundary/environment ABI review

- Runtime/version metadata advances to `0.1.0-p3-01-downstream-requirement-consolidation`; package version remains `0.1.0`.
- No public function, header, public type layout, enum value, non-version macro contract, or exported symbol is added or removed.
- Public header count remains 14 and the clean Release symbol baseline remains 69 on `ANOCRYPTO_C_0`.
- Normalized declarations, public types, non-version constants, retained P1-29-header consumption, and stored exported names remain compatibility inputs.
- `cmake/AnoCryptoCConfig.cmake.in` is a case-sensitive source filename correction matching the pre-existing CMake/policy reference; installed package names and ABI are unchanged.
- P1-34 actual hosted review remains blocked and is not replaced by local P2-02 checks.

## P2-03 self-test/integrity ABI review

- public headers remain 14 and exported baseline entries remain 69 on `ANOCRYPTO_C_0`;
- no public function, type, enum value, status, or header is added or removed;
- `ac_module.h` and `ac_selftest.h` changes are comment/contract clarification only;
- the existing `ano_crypto_selftest_run()` symbol is retained, while target on-demand state behavior is tracked as a private implementation delta;
- `INT-HMAC-SHA256-SINGLE-SO-R1`, its metadata format, policy record, checker, and future injection tooling are internal/build-time assets and do not enter public ABI;
- version/prerelease/build-fingerprint metadata advances to `0.1.0-p3-01-downstream-requirement-consolidation`;
- P1-34 actual hosted review remains blocked and is not replaced by local P2-03 checks.

## P3-02 HKDF-SHA-256 additive ABI review

- `include/anocrypto/ac_kdf.h` is the fifteenth public header.
- The public addition is exactly one combined extract-and-expand function: `ano_crypto_hkdf_sha256`.
- All 69 retained names remain unchanged on `ANOCRYPTO_C_0`.
- The new function is the only name on additive line `ANOCRYPTO_C_1`; the current total is 70.
- No new public context, enum, key type, status code, algorithm selector, high-level format, or product workflow is added.
- RFC 5869 empty-input behavior, 0/1/32/8160 output lengths, rejected 8161, pointer-range wrap, output/input overlap, and whole-output clearing on failure are part of the frozen API contract.
- Input ranges may alias each other; a non-empty output range must be disjoint from every non-empty input range.
- OpenSSL is test-only differential evidence and is not a product dependency.
- The exact AnoSecureKit `SKF1` v1 info bytes are a compatibility fixture only; `SKF1` remains AnoSecureKit-owned.
- P1-34 hosted execution/review, repository signing, external release authorization, and KCMVP lane remain `NOT_RUN`/`UNREVIEWED`, `UNSIGNED`, `NO`, and `DEFERRED`.

## P3-03 AES-256-GCM one-shot additive ABI review

- One new header: `ac_aead.h`.
- Two functions only: `ano_crypto_aes256_gcm_encrypt` and
  `ano_crypto_aes256_gcm_decrypt`.
- Existing CSP handle and status types are reused; no new public context or
  product-format type is exposed.
- Decryption authenticates before plaintext output and clears accepted output on
  failure.
- The two names are the only symbols on additive `ANOCRYPTO_C_2`; current total
  is 72 and retained ABI-0/1 manifests are unchanged.
- `SKT1`, `SKF1`, `SKP1`, streaming, scrypt and orchestration remain excluded.


## P3-04 AES-256-GCM streaming additive ABI review

- No new public header; `ac_aead.h` is extended.
- Seven functions only: context size, init, AAD update, payload update, encrypt
  final, decrypt final, and uninit.
- Direction is bound at init and AAD is accepted only before non-empty payload.
- Zero-length AAD/payload updates are no-ops and do not advance the phase.
- Exact in-place payload processing is allowed; partial overlap is rejected.
- Decrypt update output is explicitly unverified. The caller must buffer and
  withhold it until decrypt final succeeds and wipe it on failure.
- Correct-direction finalization consumes the context on success or failure;
  repeated update/final calls fail closed.
- The seven names are the only symbols on additive `ANOCRYPTO_C_3`; current total
  is 79 and retained ABI-0/1/2 manifests are unchanged.
- OpenSSL is test-only differential evidence. `SKT1` is an exact compatibility
  fixture only; its framing and workflow are not implemented in AnoCrypto-C.
