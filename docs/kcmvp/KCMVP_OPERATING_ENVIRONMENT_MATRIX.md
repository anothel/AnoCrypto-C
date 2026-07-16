# KCMVP Candidate Operating Environment Matrix

Status: P2-02 internal environment freeze. Actual hosted evidence is still `NOT_RUN` and formal environment acceptance is `EXTERNAL_CONFIRMATION_REQUIRED`.

## 1. Selected Initial Candidate

| Field | OE-01 frozen value | Evidence state |
|---|---|---|
| Candidate ID | `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` | Configuration record present |
| Operating system | Ubuntu Server 24.04.4 LTS (`noble`) | Official release selected; immutable image hash TODO |
| Architecture | x86_64 / AMD64, little-endian | Frozen; exact host CPU model TODO |
| Object/ABI | ELF, System V AMD64 ABI | Frozen |
| Kernel track | Ubuntu 24.04 GA 6.8.x generic | Exact package/version captured per candidate run |
| C library | glibc 2.39 | Frozen family/version; exact package captured per run |
| C compiler | GCC 13.3.0 (`gcc-13 13.3.0-6ubuntu2~24.04.1`) | Frozen; candidate build not yet retained |
| CMake | 3.28.3 (`3.28.3-1build7`) | Frozen |
| Generator | Unix Makefiles | Frozen; exact `make` package captured per run |
| Build type | Release | Frozen |
| Linkage | Shared library | Frozen |
| CPU tuning | Generic x86-64; no `-march=native` | Frozen |
| LTO | OFF | Frozen |
| Thread safety | ON | Frozen |
| OS entropy provider | ON, `linux-getrandom` | Frozen implementation path; responsibility confirmation pending |
| Tests/examples/hooks | OFF in candidate binary | Frozen |
| OpenSSL/sanitizer/fixtures | OFF in candidate binary | Frozen |
| Public surface | 14 headers / 69 symbols / `ANOCRYPTO_C_0` | Baseline retained |
| Actual hosted run | `NOT_RUN` | `BLOCKED_EXTERNAL: GITHUB_ACTIONS_BILLING` |
| Formal acceptance | `EXTERNAL_CONFIRMATION_REQUIRED` | Pre-consultation question |

Ubuntu 24.04 LTS and the named GCC/CMake package versions are used only to
identify an internal candidate. An official ISO hash, installed package
manifest, kernel package, CPU model, hypervisor state, compiler command line,
linked dependency list, and resulting binary hash must be captured for the
actual candidate run.

## 2. Candidate Build Command

The controlled candidate command is:

```sh
cmake -S . -B build-candidate \
  -G "Unix Makefiles" \
  -DCMAKE_C_COMPILER=/usr/bin/gcc-13 \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON \
  -DANO_CRYPTO_ENFORCE_VALIDATION_CANDIDATE=ON \
  -DANO_CRYPTO_BUILD_TESTS=OFF \
  -DANO_CRYPTO_BUILD_EXAMPLES=OFF \
  -DANO_CRYPTO_ENABLE_STRICT_WARNINGS=ON \
  -DANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON \
  -DANO_CRYPTO_ENABLE_THREAD_SAFETY=ON \
  -DANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=ON \
  -DANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF \
  -DANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=OFF \
  -DANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=OFF \
  -DANO_CRYPTO_ENABLE_SIGNING_TEST_FIXTURES=OFF \
  -DANO_CRYPTO_SANITIZER_MODE=none
cmake --build build-candidate --parallel
```

The enforcement option checks the major environment and build-option boundary.
It does not replace immutable image/package/binary evidence or formal review.

## 3. Environment Ownership and Evidence Plan

| Owner role | Responsibility | Required retained evidence |
|---|---|---|
| Configuration owner | Approve candidate ID and changes | Reviewed candidate record and change history |
| Build operator | Provision immutable environment and run controlled build | OS image hash, package manifest, commands, environment capture, build log |
| Security reviewer | Review boundary, dependencies, compiler output, and exclusions | Signed/reviewed disposition, findings, limitations |
| Test operator | Run candidate-specific test package and verify hashes | Test procedure/results tied to source and binary hashes |
| Release approver | Authorize tag/signing only after gates close | Separate approval, signer/TSA records; currently unprovisioned |
| 시험기관/전문기관 | Confirm accepted module/environment treatment | Retained response; currently `EXTERNAL_CONFIRMATION_REQUIRED` |

## 4. Deferred or Unsupported Environments

| ID | Environment/configuration | Current status | Reason / re-entry gate |
|---|---|---|---|
| OE-D01 | Windows x64, MSVC/clang-cl shared library | `DEFERRED` | Implementation/workflow exists but actual hosted evidence is absent; separate environment decision required. |
| OE-D02 | macOS x86_64 or arm64, Apple Clang shared library | `DEFERRED` | Actual hosted evidence absent; architecture/provider/loader review required. |
| OE-D03 | Linux x86_64 with Clang | `ENGINEERING_ONLY` | Differential/compiler evidence candidate; not initial submitted compiler. |
| OE-D04 | Debian 13 x86_64 with GCC 14 | `LOCAL_ENGINEERING_ONLY` | Used for local regression in P2-02; not the frozen validation candidate. |
| OE-D05 | Linux 32-bit / i686 | `DEFERRED_BLOCKED_EXTERNAL` | Workflow definition exists; no actual hosted result. |
| OE-D06 | Big-endian Linux / s390x | `DEFERRED_BLOCKED_EXTERNAL` | Workflow/QEMU candidate exists; no actual hosted result. |
| OE-D07 | Static library or source amalgamation | `UNSUPPORTED_INITIAL_CANDIDATE` | Changes physical boundary and consuming-binary assumptions. |
| OE-D08 | musl, Android, iOS, BSD, embedded RTOS, bare metal | `UNSUPPORTED` | No reviewed provider/thread/ABI/evidence path. |
| OE-D09 | ARM64, RISC-V, PowerPC, hardware dispatch/AES instructions | `UNSUPPORTED_INITIAL_CANDIDATE` | Requires separate compiler/CPU/side-channel/configuration review. |
| OE-D10 | Thread safety OFF | `TEST_CONFIGURATION_ONLY` | Caller-global serialization mode is not the initial candidate. |
| OE-D11 | OS entropy provider OFF | `NEGATIVE_TEST_CONFIGURATION_ONLY` | Public random service intentionally fails closed. |
| OE-D12 | Entropy/random hooks, signing fixtures, OpenSSL differential | `TEST_ONLY` | Forbidden in Release candidate and customer runtime. |
| OE-D13 | ASan/UBSan build | `DIAGNOSTIC_ONLY` | Not a release/runtime candidate. |
| OE-D14 | VM snapshot/clone continuation or inherited active random context | `UNSUPPORTED_OPERATION` | Module does not automatically detect duplication. |
| OE-D15 | Suspend/resume without product-defined discard/reinit policy | `UNSUPPORTED_OPERATION` | No automatic freshness-boundary detection. |
| OE-D16 | `dlclose`/unload with active calls, handles, or contexts | `UNSUPPORTED_OPERATION` | Lifetime and zeroization cannot be safely guaranteed. |

`DEFERRED` means the source may compile or have workflow coverage; it does not
mean validated, supported, reviewed, or approved for customer deployment.

## 5. Runtime Dependency Matrix

| Dependency | Candidate use | Boundary status | Required review |
|---|---|---|---|
| ELF dynamic loader | Loads the shared object and resolves imports | Outside module | Record loader/libc package and library dependency output. |
| glibc `libc.so.6` | Memory, process, and threading/runtime functions | Outside module | Freeze package version and linked symbols. |
| Linux kernel `getrandom` | Conditioned entropy provider call | Outside module / external provider | Confirm health/responsibility interpretation and retain failure behavior. |
| Linux process/thread scheduler | Process ID and lock behavior | Outside module | Retain fork/thread tests on exact candidate. |
| Filesystem/package manager | Stores/installs binary | Outside module | Protect artifact and verify hashes before load. |
| OpenSSL | None in candidate runtime | Excluded | Differential tests only. |
| C++ runtime | None | Excluded | C99 core only. |

## 6. Candidate Evidence Gate

Before OE-01 may be called a retained candidate result, collect and review:

1. official Ubuntu image and SHA-256 record;
2. complete installed-package manifest and update state;
3. kernel, glibc, GCC, CMake, make, binutils, loader, and CPU identity;
4. full configure/build commands and compiler/linker flags;
5. source commit/tree hash and clean-worktree status;
6. shared-object hash, SONAME, ELF headers, imports, dependencies, and 69-symbol export list;
7. CTest/policy/install/PoC evidence from the same source, with the clean candidate binary separately identified;
8. human review of compiler output, dependencies, exclusions, and known gaps;
9. institution response on whether one or more exact environment variants are required.

P1-34 remains separately blocked. Local or synthetic results cannot satisfy
items requiring actual hosted or selected-candidate execution.

## 7. External Confirmation Questions

- Is this shared-library logical boundary acceptable, and are headers treated only as interface specifications?
- May Ubuntu 24.04 LTS x86_64/GCC 13 be treated as one software operating environment, or must kernel/CPU/patch-level variants be separate?
- Must the external OS `getrandom` provider and glibc/kernel be inside, outside, or explicitly trusted within the submitted boundary?
- Are VM/container/hypervisor variants separate operating environments?
- Which environment evidence and change thresholds trigger retest or a new validation entry?

Until retained answers exist, the candidate is frozen for internal planning only.
