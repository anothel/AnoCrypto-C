# P3-07 Implementation and Test Delta

Status: `DESIGN_ONLY_VALIDATED_NO_PUBLIC_DELTA`

## Change class

P3-07 changes requirement, ownership, architecture, API-candidate, roadmap, and
validation documents only.

It adds no:

- public header or public declaration;
- public function, type, enum, constant, or exported symbol;
- C source or cryptographic implementation;
- ABI map or exported-symbol manifest entry;
- CMake library target or installed SDK file;
- runtime version change;
- SHA-1, RSA, ECDSA, PKI codec, or provider-routing implementation.

## Added documents

- `docs/design/C99_PUBLIC_KEY_KEY_HANDLE_ARCHITECTURE_GATE.md`;
- `docs/product/P3_07_PUBLIC_KEY_REQUIREMENT_REVIEW.md`;
- this implementation/test delta;
- `docs/roadmap/P3_08_RSA_PKCS1_V15_SHA2_NEXT_WORK_PROMPT.md`.

## Updated documents

- AnoPKI requirement trace;
- CSP lifecycle and thread-safety policy/support matrix;
- clean-room source manifest;
- cross-repository ownership;
- capability gap register;
- product operation/capability matrix;
- current/active/next, C99 priority, and development-workflow roadmap;
- P3 product-driven roadmap;
- P3 ABI evolution gate;
- P3-07 entry record;
- downstream source baseline Markdown/JSON;
- project source-replacement plan;
- root current-status summary.

## Frozen public baseline

- public headers: 16;
- exported symbols: 80;
- ABI lines: `ANOCRYPTO_C_0` through `ANOCRYPTO_C_4`;
- runtime version: `0.1.0-p3-05-scrypt-fixed-profile`;
- retained compatibility baseline: P1-29;
- `ANOCRYPTO_C_5`: not introduced or reserved.

## Source and pack validation

- P3-07 entry working-source ZIP SHA-256:
  `1aac40228d16978b44ec4b95fc9779f98ba1605248c40efd6afc7e35132563bc` — `PASS`;
- CURRENT ONLY Sources Pack manifest: 12/12 files — `PASS`;
- exact AnoPKI Community commit
  `ae3ddee72dd05f274b9bbae1b514cdc4c4de03f5` and relevant operation,
  provider, policy, and test paths re-read — `PASS`;
- AnoPKI Enterprise revision/hash records for
  `68b379e7684c9d8b44c53728949774088352a9d9` cross-checked — `PASS`;
- raw AnoPKI Enterprise source ZIP byte-for-byte recomputation in this runtime —
  `EXTERNAL_CONFIRMATION_REQUIRED`.

## Protected-tree validation

The following current trees/files are byte-identical to the P3-07 entry
baseline:

- `include/` — `PASS`;
- `src/` — `PASS`;
- `abi/` — `PASS`;
- `release/PUBLIC_HEADERS.sha256` — `PASS`;
- `release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake` — `PASS`;
- `include/anocrypto/ac_version.h` — `PASS`.

The final documentation diff contains no public header, implementation source,
ABI map, exported-symbol manifest, build target, test source, example source, or
runtime-version change.

## Local build and regression validation

Environment used for this local engineering check:

- GCC 14.2.0;
- CMake with Unix Makefiles for Debug/Release; Ninja for the clean Release surface check;
- OpenSSL 3.5.5 in test-only differential targets;
- Linux x86-64.

Results:

- Debug configure/build — `PASS`;
- Debug full CTest — **88/88 passed**;
- Release configure/build — `PASS`;
- Release full CTest — **88/88 passed**;
- current-header/current-library consumer — `PASS`;
- retained P1-29-header/current-library consumer — `PASS`;
- policy and ABI checks — `PASS`;
- install/export smoke — `PASS`;
- customer PoC build-tree smoke — `PASS`;
- OpenSSL test-only differential targets — `PASS`.

## Clean production Release surface

A separate shared Release configuration was built with tests/examples and
OpenSSL differential disabled and internal symbol hiding enabled.

- public headers: **16**;
- public dynamic symbols: **80**, exactly matching the stored current manifest;
- internal `ano_crypto_internal_*` export leakage: none;
- `ANOCRYPTO_C_0`: 69 symbols;
- `ANOCRYPTO_C_1`: 1 symbol;
- `ANOCRYPTO_C_2`: 2 symbols;
- `ANOCRYPTO_C_3`: 7 symbols;
- `ANOCRYPTO_C_4`: 1 symbol;
- `ANOCRYPTO_C_5`: absent;
- production runtime dependency on `libcrypto` or `libssl`: none.

## Artifact validation handoff rule

The final source ZIP and one-commit patch may be handed off only after all of
the following pass against the frozen documentation commit:

- patch application to the baseline parent and exact Git-tree comparison;
- source ZIP tree extraction and exact Git-tree comparison;
- exclusion of `.git`, build trees, generated binaries, and local logs;
- SHA-256 recomputation for the ZIP and patch.

The external `.sha256` handoff file records the final artifact digests. A failed
comparison stops artifact delivery rather than being downgraded to a TODO.

## External status retained

- KCMVP: `DEFERRED`;
- hosted lanes: `NOT_RUN`;
- hosted review: `UNREVIEWED`;
- repository signing: `UNSIGNED`;
- signing handoff: `BLOCKED`;
- external release authorization: `NO`.
