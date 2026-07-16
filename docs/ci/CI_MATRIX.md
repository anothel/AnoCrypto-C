# CI Matrix

Status: P2-02 engineering matrix; OE-01 is internally frozen, while actual hosted lanes remain `NOT_RUN`/`UNREVIEWED`.

| Lane | Linux | macOS | Windows | Notes |
|---|---:|---:|---:|---|
| C99 configure/build/test | Required | Required | Required | Actual hosted artifacts require review. |
| Install/package consumer smoke | Required | Required | Required | Uses installed `AnoCryptoC::AnoCryptoC`. |
| Public surface and 69-symbol baseline | Required | Required | Required | Tests/examples/hooks OFF; internal symbols hidden. |
| SHA-2/HMAC KAT and negative tests | Required | Required | Required | Status-only logs. |
| AES-CBC KAT and negative tests | Required | Required | Required | Complete blocks, no padding. |
| AES-CMAC KAT and negative tests | Required | Required | Required | Full 16-byte tag, dedicated key purpose. |
| OpenSSL differential | Required hosted Linux lane | Optional | Optional | Test-only provider; product and PoC remain independent. |
| GCC ASan + UBSan | Required hosted Linux lane | Not selected | Not selected | Diagnostic evidence only. |
| Thread-safety OFF | Required | Required | Required | Confirms explicit unsupported branch. |
| Platform entropy provider OFF | Required | Required | Required | Public random service must fail closed. |
| Entropy/random test hooks | Dedicated internal lane | Dedicated internal lane | Dedicated internal lane | Hooks disabled in Release/PoC. |
| 32-bit representation | Hosted Linux candidate | — | — | Result not yet reviewed. |
| Big-endian candidate | s390x/QEMU workflow | — | — | Result not yet reviewed. |
| Compiler-output capture | GCC/Clang candidate | Apple Clang candidate | MSVC/clang-cl candidate | Human review required. |

## P2-02 selected-candidate lane

The initial validation candidate is not equivalent to the broad engineering matrix above.
Its clean runtime build is exactly:

| Candidate lane | Frozen value | Current execution state |
|---|---|---|
| `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` | Ubuntu Server 24.04.4 LTS, x86_64, GCC 13.3.0, CMake 3.28.3, Unix Makefiles, Release shared library | `NOT_RUN_ON_SELECTED_OE` |
| Runtime options | tests/examples/hooks/OpenSSL/signing fixtures/sanitizers OFF; hidden symbols/thread safety/OS entropy ON; generic x86-64; LTO OFF | Static policy and local mismatch-negative check available |
| Evidence plan | immutable OS image, CPU/kernel/packages, build command, source/commit, binary hash, SONAME/imports/exports, install smoke and named human review | `PARTIAL` |

The broad Linux/macOS/Windows workflows remain engineering portability lanes. They do not expand the initial candidate and do not constitute execution evidence until runners execute and artifacts receive retained human review.

## P1-28 AES-CMAC addition

CI checks the public eight-function API, dedicated CMAC key type, startup and
extended NIST example KATs, negative/cross-purpose handling, OpenSSL test-only
differential comparison, install/PoC smoke, 69-symbol baseline, internal-source
exclusion, and value-free logs. Workflow presence does not mean hosted or
side-channel review is complete.

## P1-29 CMAC hardening addition

Add the CMAC hardening review and 128-cycle lifecycle stress to ordinary,
Clang, and selected diagnostic lanes. The required OpenSSL lane checks 1,728
CMAC comparison paths. Release-style compiler lanes capture CMAC disassembly
metadata where supported. All lanes run the CMAC hardening policy and retain
the 69-symbol clean Release baseline. Hosted workflow definitions do not count
as reviewed run evidence.


## P2-02 / P1-34 hosted blocker

GitHub Actions run `29256531912` at baseline commit `cf0f3c425dd265c7509fb64a6123db97f71055dd` reached no runner because of account billing/spending-limit status. Actual hosted lanes remain `NOT_RUN`, review remains `UNREVIEWED`, and no artifact was produced. See `HOSTED_CI_EXTERNAL_BLOCKER.md`.
