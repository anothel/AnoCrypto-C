# KCMVP Candidate Module Boundary Specification

Status: P2-02 internally frozen candidate. Formal acceptability remains `EXTERNAL_CONFIRMATION_REQUIRED`.

## 1. Configuration Identification

| Field | Frozen value |
|---|---|
| Candidate ID | `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` |
| Product | `AnoCrypto-C` |
| Engineering version | `0.1.0-p3-00-downstream-requirement-intake` |
| Module type | Software cryptographic module candidate |
| Linkage form | Shared library only |
| Runtime artifact | `libanocrypto_c.so.0.1.0` |
| SONAME | `libanocrypto_c.so.0` |
| ABI | 14 public headers, 69 exported symbols, `ANOCRYPTO_C_0` |
| Formal validation status | Not validated; external confirmation and formal testing pending |

The machine-readable controlling record is
`release/ANOCRYPTO_C_VALIDATION_CANDIDATE.cmake`.

## 2. Candidate Physical and Logical Boundary

The initial candidate is the code and module-owned mutable state loaded from the
single shared object `libanocrypto_c.so.0.1.0` into one application process.
The candidate boundary includes:

- executable code and read-only data compiled from the current `src/*` runtime sources;
- module lifecycle and startup-self-test state;
- hash, HMAC, HMAC_DRBG, random, AES-CBC, AES-CMAC, CSP-handle, and zeroization logic;
- module-owned global state and in-process CSP slots;
- caller-provided opaque operation contexts only while operated on by module code;
- the internal Linux `getrandom` provider adapter and process marker used by the public random service.

The installed public headers describe the interface but are not executable
runtime components. Symlinks, package metadata, CMake files, documentation, and
customer integration code are also outside the runtime cryptographic boundary.

Formal interpretation of the physical boundary for a dynamically loaded
software module remains `EXTERNAL_CONFIRMATION_REQUIRED`.

## 3. Boundary Diagram

```text
Customer application process

  Application / AnoSecureKit / AnoPKI                         OUTSIDE
  ---------------------------------------------------------------
       C99 API calls, status, lengths, caller-owned buffers
  =================== candidate logical interface =================
  |                                                             |
  |  libanocrypto_c.so.0.1.0                                   |
  |                                                             |
  |  lifecycle + startup self-test + sticky error state         |
  |  SHA-2 / HMAC / HMAC_DRBG / random / AES-CBC / AES-CMAC     |
  |  in-process CSP slots / zeroization / thread lock           |
  |                                                             |
  |  Linux entropy adapter -------- getrandom() request          |
  |                                                             |
  ================= candidate runtime boundary ===================
       |                         |                       |
       v                         v                       v
  glibc / ELF loader       Linux kernel RNG       process/thread scheduler
  EXTERNAL DEPENDENCY      EXTERNAL PROVIDER      EXTERNAL ENVIRONMENT
```

The OS provider and general-purpose operating system are dependencies and
operating-environment elements, not AnoCrypto-C source components. P2-04 freezes
the internal entropy-source responsibility; accepted provider evidence and formal
interpretation remain consultation items.

## 4. Runtime Source Inventory

The candidate shared object is built from these project-owned runtime source
areas:

| Component | Runtime sources | Boundary role |
|---|---|---|
| Core/state/build | `src/core/*.c` | Lifecycle, status, build identity, lock |
| Startup self-test | `src/selftest/*.c` | Startup KAT orchestration and manifest |
| Hash | `src/hash/*.c` | SHA-256/384/512 |
| MAC | `src/mac/*.c` | HMAC-SHA-2 and AES-CMAC |
| Cipher | `src/cipher/*.c` | AES primitive and CBC mode |
| DRBG/random | `src/drbg/*.c` | HMAC_DRBG, random service, entropy/process seams |
| CSP/key | `src/csp/*.c` | Purpose-separated in-memory handles and zeroization |

The exact source file hash set is controlled separately by release provenance
and source-manifest evidence. Generated build files are not source inputs.

## 5. External Interfaces and Data Flows

| Flow ID | Direction | Data category | Interface | Rule |
|---|---|---|---|---|
| DF-01 | Caller → module | Control/status parameters | 69 exported C symbols | Calls require documented module state. |
| DF-02 | Caller → module | Message, key, IV, context, length | Public C API buffers/opaque storage | Caller owns buffers and serialization unless stated otherwise. |
| DF-03 | Module → caller | Status, lengths, digest/MAC/cipher/random output | Output parameters | Sensitive values are not logged or retained as evidence. |
| DF-04 | OS → module | Conditioned random bytes | Linux `getrandom()` through internal adapter | Provider failure propagates and output is cleared. |
| DF-05 | Module ↔ process memory | DRBG/CSP/module state | Module globals and caller-owned contexts | In-process only; no persistence or IPC. |
| DF-06 | Loader/libc/kernel → module | Runtime service and scheduling | ELF loader, libc, pthread-compatible primitives, syscalls | Controlled as operating-environment dependencies. |

## 6. Included Configuration

The initial candidate uses exactly this production-style configuration:

```text
BUILD_SHARED_LIBS=ON
CMAKE_BUILD_TYPE=Release
ANO_CRYPTO_BUILD_TESTS=OFF
ANO_CRYPTO_BUILD_EXAMPLES=OFF
ANO_CRYPTO_ENABLE_STRICT_WARNINGS=ON
ANO_CRYPTO_HIDE_INTERNAL_SYMBOLS=ON
ANO_CRYPTO_ENABLE_THREAD_SAFETY=ON
ANO_CRYPTO_ENABLE_OS_ENTROPY_PROVIDER=ON
ANO_CRYPTO_ENABLE_ENTROPY_TEST_HOOKS=OFF
ANO_CRYPTO_ENABLE_RANDOM_TEST_HOOKS=OFF
ANO_CRYPTO_ENABLE_OPENSSL_DIFFERENTIAL_TESTS=OFF
ANO_CRYPTO_ENABLE_SIGNING_TEST_FIXTURES=OFF
ANO_CRYPTO_SANITIZER_MODE=none
```

`-march=native`, LTO, internal test hooks, automatic symbol export, and test-only
providers are forbidden in this candidate. A future compiler, option, OS,
architecture, static-library, or algorithm-scope change creates a different
configuration requiring change-impact review.

## 7. Explicitly Outside the Candidate Runtime Boundary

- all `tests/*`, `examples/*`, `tools/*`, `.github/*`, `docs/*`, `evidence/*`, and `cleanroom/*` content;
- OpenSSL and differential-test code;
- CMake, compilers, linkers, package managers, CI services, SBOM/provenance/signing tools;
- customer PoC executables and application integration code;
- AnoSecureKit, AnoPKI, and the future C++ wrapper;
- persistent key storage, key wrapping/export, platform keystore adapters, IPC, daemon processes;
- production signer, TSA, signatures, timestamps, and release authorization;
- OS kernel, glibc, loader, filesystem, process manager, hypervisor, and hardware.

Some excluded operating-environment components are nevertheless trusted or
assumed dependencies and must be identified in the submitted environment.

## 8. Loader, Process, Thread, and Operator Assumptions

- One shared object is loaded into one application process.
- The application initializes the module once before cryptographic use and owns orderly shutdown.
- The library must not be unloaded while any call, handle, or context may still be active.
- Thread safety is enabled, but a single caller-owned mutable context is not concurrently mutated.
- CSP handles are process-local and are not shared across processes.
- A fork/process-marker change requires reseed before public random generation.
- VM snapshot/clone and suspend/resume are not automatically detected; inherited random contexts must be discarded and reinitialized under the current policy.
- The operator must protect the shared object and process environment from unauthorized replacement, debugging, memory inspection, and privilege abuse.
- A module-integrity mechanism is not yet implemented; P2-03 must select and specify it before formal submission.

## 9. Boundary Change Control

The following require a new candidate ID or an explicit reviewed revision:

- shared → static linkage or embedding into another binary;
- OS, architecture, ABI, libc, compiler-major/minor, build type, optimization, or LTO change;
- enabling hardware dispatch or architecture-specific instruction paths;
- changing entropy provider, thread model, process model, or module integrity mechanism;
- adding/removing a public header, symbol, service, algorithm, mode, or CSP type;
- enabling hooks, differential providers, sanitizers, fixtures, or examples in a customer/runtime build.

## 10. P2-02 Disposition

The internal candidate boundary is frozen for design and pre-consultation. It is
not a formal validation boundary until the selected 시험기관/전문기관 confirms
module form, environment treatment, entropy-provider responsibility, integrity
mechanism expectations, and configuration-variant handling.
