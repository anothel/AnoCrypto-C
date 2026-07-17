# Module Basic Design Skeleton

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-BD-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_SKELETON` |
| Owner role | Security architecture lead |
| Reviewer | `TODO` |
| Approval | `NO` |

## 1. Module identity

- Product: `AnoCrypto-C`.
- Engineering version: `0.1.0-p3-01-downstream-requirement-consolidation`.
- Public C99 ABI: 14 public headers, 69 exported symbols, `ANOCRYPTO_C_0`.
- Initial candidate: `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1`.
- Candidate module file: `libanocrypto_c.so.0.1.0`.
- Formal security level: `UNSELECTED / EXTERNAL_CONFIRMATION_REQUIRED`.

## 2. Cryptographic boundary

The candidate logical and physical software boundary is the selected shared object. Public headers describe interfaces but are not executable module contents. Tests, build tools, OpenSSL differential code, evidence tooling, signing tools, customer PoC, and application/PKI products remain outside the runtime boundary.

Authoritative input: `../KCMVP_MODULE_BOUNDARY_SPEC.md`.

## 3. Operating environment

The internal candidate is Ubuntu Server 24.04.4 LTS, x86_64 little-endian, GCC 13.3.0, CMake 3.28.3, Release shared-library build with thread safety and Linux OS entropy enabled. Exact retained execution is `NOT_RUN`; formal variant treatment is `EXTERNAL_CONFIRMATION_REQUIRED`.

Authoritative input: `../KCMVP_OPERATING_ENVIRONMENT_MATRIX.md`.

## 4. Roles and service access

The public API currently has no authenticated user-role mechanism. The consuming process invokes services through the C ABI. Any required crypto-officer/user role mapping and authentication interpretation is `EXTERNAL_CONFIRMATION_REQUIRED`.

Services are available only in the documented module state. Startup self-test must complete before operational cryptographic service use. Fatal failure enters sticky `ERROR_STATE`.

## 5. Approved candidate service scope

Current engineering scope:

- SHA-256, SHA-384, SHA-512;
- HMAC-SHA-256, HMAC-SHA-384, HMAC-SHA-512;
- HMAC_DRBG-SHA-256;
- platform-backed random service;
- AES-128/192/256 primitive and CBC without padding;
- AES-CMAC full 16-byte tag;
- in-memory purpose-separated HMAC, block-cipher, and CMAC CSP handles.

Formal validation-target scope remains `EXTERNAL_CONFIRMATION_REQUIRED`. Deferred algorithms and application/PKI workflows are not silently included.

## 6. Interfaces and ports

- Data input/output: public hash, MAC, DRBG/random, cipher, CMAC and key/CSP parameters and buffers.
- Control input: module lifecycle, self-test flags, context initialization/finalization and key-handle lifecycle calls.
- Status output: `ano_crypto_status_t`, state/build/version/self-test metadata and explicit lengths.
- Power interface: process load/unload and host execution environment, not a module-managed hardware port.
- Maintenance/debug interfaces: none in the Release candidate; internal hooks are excluded.

## 7. State model

The internal design states are `UNINITIALIZED`, `SELF_TEST_RUNNING`, `OPERATIONAL`, `ERROR_STATE`, and `SHUTDOWN`. Exact transitions and service availability are controlled by `../KCMVP_FINITE_STATE_MODEL.md` and `../KCMVP_FAILURE_RECOVERY_SERVICE_MATRIX.md`.

Runtime integrity verification remains `NOT_IMPLEMENTED`. The selected candidate design must not be represented as executed behavior.

## 8. Self-tests and conditional tests

Startup engineering KAT paths cover implemented SHA-2, HMAC, HMAC_DRBG, AES, CBC and CMAC services. The selected shared-object integrity mechanism is `SELECTED_CANDIDATE_NOT_IMPLEMENTED`. Entropy-provider conditional-test responsibility remains provisional and externally confirmable.

## 9. CSP overview

Current CSPs and sensitive intermediates are enumerated in `../KCMVP_CSP_INVENTORY.md`. Module-owned handle slots are cleared on explicit destruction, orderly shutdown and locked fatal transition. Caller-owned opaque contexts retain caller cleanup responsibilities. Persistent storage, wrapping, export and platform keystore are outside the initial candidate.

## 10. Security assumptions and open decisions

- Host kernel, loader, filesystem and `getrandom()` provider are external dependencies.
- Core dumps, swap, hibernation, debugger access, VM snapshots and forced termination require operator controls.
- Exact security level, role mapping, approved algorithms, integrity mechanism, entropy evidence and side-channel depth remain `EXTERNAL_CONFIRMATION_REQUIRED`.
- Actual hosted evidence remains `NOT_RUN/UNREVIEWED` due the retained P1-34 billing blocker.
