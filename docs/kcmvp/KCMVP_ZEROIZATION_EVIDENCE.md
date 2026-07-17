# KCMVP Zeroization Evidence and Limitations — P2-05

Status: `FROZEN_INTERNAL_POLICY`; selected-OE evidence and formal acceptance remain pending.

## 1. Policy summary

Policy `CZP-P2-05-R1` uses the following layered evidence model. Selected OE review status: `NOT_RUN`.

1. source-path review of each CSP and sensitive intermediate;
2. behavioral tests for state invalidation, explicit destroy, orderly shutdown, fatal
   transition, reinitialization, failure cleanup, and caller-owned uninit;
3. optimized shared-library disassembly capture for manual target/compiler review;
4. ABI, install, PoC, policy, provenance, and sensitive-output regression checks;
5. exact OE-01 retained execution and human disposition, still `NOT_RUN`.

Passing local tests does not prove that all copies in registers, stack spill slots, kernel
buffers, swap, crash dumps, hibernation images, hypervisor snapshots, or storage media have
been removed.

## 2. Zeroization primitive

`ano_crypto_internal_zeroize()` performs byte stores through a volatile-qualified pointer.
The public `ano_crypto_csp_zeroize()` validates null/length semantics and delegates to the
same implementation.

Internal frozen wording:

- source-level explicit clearing is implemented;
- local optimized compiler output is captured for manual review;
- exact OE-01 GCC 13.3.0 machine-code capture and retained reviewer disposition are
  `NOT_RUN`;
- no universal compiler, CPU, optimizer, or physical-remanence guarantee is made.

## 3. P2-05 module-owned table remediation

Before P2-05, explicit `ano_crypto_key_destroy()` and slot reuse cleared key bytes, but
module shutdown and fatal state transition did not clear all live table slots.

P2-05 adds an internal locked table sweep:

- every live HMAC/CBC/CMAC slot has its 128-byte key storage cleared;
- length, key type, and flags are cleared;
- state becomes `ZEROIZED` and the current generation is retained;
- the handle becomes invalid for cryptographic use;
- a caller may complete explicit destroy, moving the tombstone to `DESTROYED`;
- a reused slot receives a new generation and invalidates the old handle.

The sweep is called during orderly shutdown, fatal transitions reached while the global
lock is held, and shutdown acknowledgment in sticky error state. If acquiring the global
lock itself fails, the last-resort fatal latch is recorded but concurrent safe table wipe
cannot be guaranteed. The recovery boundary remains process termination and fresh load.

## 4. Caller-owned context responsibility

The module has no registry of caller-owned opaque contexts. Therefore:

- `module_shutdown` clears only module-owned CSP slots;
- the application must call `ano_crypto_hmac_uninit`, `ano_crypto_cipher_uninit`,
  `ano_crypto_cmac_uninit`, and `ano_crypto_drbg_uninstantiate`/`random_uninit`;
- abandoned contexts cannot be remotely wiped by the module;
- freeing memory without explicit uninit is outside the module's guaranteed cleanup path;
- application teardown, crash dumps, swap, hibernation and snapshot controls belong to the
  operator/environment policy unless an institution assigns them differently.

## 5. Evidence matrix

| Evidence ID | Claim boundary | Artifact / test | Current disposition |
|---|---|---|---|
| `ZE-001` | Public zeroize clears requested caller buffer | service-gate negative test; `csp_zeroization_policy_review` | `LOCAL_PASS` |
| `ZE-002` | Explicit key destroy invalidates and clears module slot | key lifecycle and stale/exhaustion tests | `LOCAL_PASS` |
| `ZE-003` | Orderly shutdown zeroizes every live module-owned slot | `csp_shutdown_zeroization_tests` | `LOCAL_PASS` |
| `ZE-004` | Fatal transition zeroizes live module-owned slots and stays sticky | `csp_fatal_zeroization_tests` | `LOCAL_PASS` |
| `ZE-005` | HMAC/CBC/CMAC temporary key snapshots are cleared | source review plus algorithm operational tests | `READY_FOR_INTERNAL_REVIEW` |
| `ZE-006` | Caller-owned contexts clear on final/uninit/failure paths | HMAC, AES-CBC, CMAC and DRBG tests | `READY_FOR_INTERNAL_REVIEW`; abandoned-context responsibility remains external |
| `ZE-007` | Optimizer retains an effective zeroization routine | `ZEROIZATION_COMPILER_OUTPUT_REVIEW.txt` and disassembly | local capture only; manual review required |
| `ZE-008` | Exact OE-01 GCC 13.3.0 output reviewed and retained | future selected-OE artifact | `NOT_RUN` |
| `ZE-009` | No public ABI widening | public surface and clean Release symbol checks | 14/69/`ANOCRYPTO_C_0` |
| `ZE-010` | No sensitive runtime values in evidence | sensitive artifact scanner and status-only review | `LOCAL_PASS` |

## 6. Compiler review procedure

For each selected build:

1. build the clean Release shared library with the frozen candidate options;
2. record compiler, version, target, build type and library SHA-256;
3. disassemble the shared library with the selected tool;
4. locate `ano_crypto_internal_zeroize`, `ano_crypto_internal_csp_zeroize_all_unlocked`,
   `ano_crypto_internal_csp_destroy`, and relevant uninit/final routines where symbols are
   retained;
5. confirm that the zeroization routine contains a store loop or equivalent clearing call
   and returns only after the stores;
6. inspect call sites or inlined regions for early-return bypasses;
7. record inlining/stripping limitations and do not infer absence from a missing symbol;
8. retain reviewer, UTC date, artifact hash and disposition.

Automation captures artifacts but does not make the human disposition.

## 7. Required exact-OE findings record

The future OE-01 review must record at least:

- immutable OS/image and compiler package identity;
- CPU model and relevant optimization flags;
- shared-library SHA-256 and source commit/archive binding;
- disassembler and version;
- reviewed function/range markers;
- observed clearing construction;
- unresolved register/spill or inlining concerns;
- reviewer, UTC date and `PASS`, `PASS_WITH_LIMITATIONS`, or `FAIL` disposition.

Until this exists, selected-OE zeroization evidence remains `NOT_RUN`.

## 8. Non-claims

P2-05 does not claim:

- physical memory remanence elimination;
- zeroization of caller memory not passed to an uninit/zeroize API;
- clearing of OS/provider/kernel copies;
- clearing after power loss, forced termination, kernel panic or hardware failure;
- clearing of swap, core dumps, crash telemetry, hibernation or VM snapshots;
- immunity from compiler, debugger, DMA, cold-boot or fault attacks;
- formal validation or institution acceptance.
