# Installation and Operator Guide Skeleton

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-OP-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_SKELETON` |
| Owner role | Product security/operator documentation lead |
| Reviewer | `TODO` |
| Approval | `NO` |

## 1. Intended configuration

This guide targets candidate `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` only. Exact OE-01 execution is `NOT_RUN`, so commands remain a controlled candidate procedure rather than an approved installation instruction.

## 2. Build/install controls

- Use the pinned source baseline and exact documented toolchain.
- Enable Release shared-library candidate enforcement.
- Keep tests, examples, internal hooks, OpenSSL differential provider, sanitizers and signing fixtures outside the candidate runtime build.
- Preserve exact shared-object bytes after final integrity/signing processing.
- Record source, package and installed library SHA-256 values.

## 3. Runtime lifecycle

1. Load the intended shared object from the approved path.
2. Call module initialization.
3. Confirm operational state before cryptographic service use.
4. Use only documented services and status/buffer rules.
5. Destroy module-owned CSP handles and uninitialize caller-owned contexts.
6. Perform orderly module shutdown before process termination when possible.

Runtime module integrity is currently `NOT_IMPLEMENTED`; do not claim the library authenticates its own bytes.

## 4. Fatal-state response

After fatal self-test/integrity/global-state failure:

- stop cryptographic processing;
- collect status-only diagnostics;
- do not attempt an in-process reset or bypass;
- stop the consuming process;
- verify artifact/environment through the authorized process;
- reinstall or replace if required;
- start a fresh process only after investigation.

## 5. Entropy and process operation

The candidate uses Linux `getrandom(flags=0)` through the internal adapter. Known fork/process marker change requires reseed. VM/container snapshot duplication and suspend/resume are not automatically detected; cloned state must be discarded and independently reinitialized according to the integration policy.

## 6. Host security controls

Operator policy must address:

- filesystem ownership and write permissions;
- loader/library-path control;
- core dumps and crash telemetry;
- swap and hibernation;
- debugger/ptrace access;
- backup, snapshot and cloning;
- process isolation and least privilege;
- log access and retention;
- package authenticity and rollback.

Exact mandatory controls remain `EXTERNAL_CONFIRMATION_REQUIRED`.

## 7. Diagnostics and non-disclosure

Permitted diagnostics include version, candidate ID, state, status, compiler/OS/CPU identifiers, artifact hash and review disposition. Never output keys, entropy, random bytes, DRBG state, IV, plaintext/ciphertext, digest/MAC/CMAC values, integrity key/tag or production signing material.

## 8. Unsupported initial configurations

Static linkage, Windows, macOS, 32-bit, big-endian, ARM64, musl, Android, iOS, BSD, RTOS, hardware dispatch, persistent/exportable key storage and application/PKI workflows are not approved initial candidate configurations.
