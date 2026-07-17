# Module Detailed Design Skeleton

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-DD-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_SKELETON` |
| Owner role | Implementation lead |
| Reviewer | `TODO` |
| Approval | `NO` |

## 1. Component decomposition

| Component | Primary source area | Responsibility |
|---|---|---|
| Core lifecycle/state | `src/core`, `src/internal` | initialization, state, fatal latch, build identity |
| Self-test | `src/selftest` and algorithm KAT paths | startup/on-demand engineering checks |
| Hash/HMAC | `src/hash`, `src/mac` | SHA-2 and HMAC services |
| DRBG/random/entropy | `src/drbg`, `src/random`, `src/entropy` | deterministic core, OS provider adapter, process/reseed policy |
| AES/CBC | `src/cipher` | AES primitive and CBC without padding |
| CMAC | `src/mac` | AES-CMAC full-tag generation/verification |
| CSP/key handles | `src/csp` | purpose-separated in-memory key slot lifecycle |
| Thread/process abstraction | internal platform layers | locking and process-marker behavior |

Exact file-to-requirement mappings remain in `../KCMVP_EVIDENCE_TRACEABILITY_MATRIX.md`.

## 2. Lifecycle and error handling

The target state and recovery design is controlled by P2-03. Ordinary argument, buffer, unsupported, authentication, stale-handle and reseed-required failures are recoverable unless a service-specific specification states otherwise. Mandatory self-test/integrity or module-global invariant failure is fatal and requires process termination and fresh load.

Implementation deltas remain listed in `../P2_03_IMPLEMENTATION_TEST_DELTA.md`; they are not treated as complete.

## 3. Self-test and integrity design

- Existing startup KAT sequence: implemented engineering behavior.
- On-demand state-transition hardening: `NOT_IMPLEMENTED` where identified by P2-03 delta records.
- Integrity candidate: `INT-HMAC-SHA256-SINGLE-SO-R1`.
- Runtime shared-object measurement, metadata injection and controlled integrity key lifecycle: `NOT_IMPLEMENTED`.

The integrity candidate requires external acceptance and a separate implementation gate.

## 4. Algorithm and service design references

| Area | Controlled engineering input |
|---|---|
| SHA-2/HMAC | public API headers, KAT plans, differential strategy |
| HMAC_DRBG/random | `../KCMVP_ENTROPY_AND_DRBG_SPEC.md` and responsibility matrices |
| AES/CBC | block-cipher design/KAT/hardening records |
| AES-CMAC | CMAC KAT/compiler/hardening records |
| CSP/zeroization | `../KCMVP_CSP_INVENTORY.md`, service matrix and zeroization evidence |

The skeleton does not copy implementation source or test vectors into the routine document set.

## 5. Entropy and DRBG data flow

The selected profile obtains conditioned bytes through Linux `getrandom(flags=0)` via the internal adapter. The initial candidate owns no direct noise source. Instantiate requests 32 bytes entropy and 16 bytes nonce; reseed requests 32 bytes; maximum generate request is 65,536 bytes; implementation reseed interval is `2^48`; prediction resistance is unsupported.

Formal entropy credit, health-test ownership and institution evidence remain `EXTERNAL_CONFIRMATION_REQUIRED`.

## 6. CSP lifecycle and zeroization

Module-owned HMAC/AES/CMAC key slots use purpose/type/generation state and are cleared on destruction, orderly shutdown and locked fatal transition. Caller-owned opaque contexts are not centrally tracked. The local volatile-byte-store primitive and compiler captures are engineering evidence only; exact OE-01 named review is `NOT_RUN`.

## 7. Concurrency and process behavior

- Module-global state and CSP slots use the selected internal locking abstraction.
- Mutable caller-owned contexts require caller serialization unless a specific API states otherwise.
- Process-marker change forces random-service reseed behavior.
- VM/container snapshot and suspend/resume duplication are not automatically detected.

## 8. Secure coding and side-channel status

P2-06 records local source and compiler review, Linux ELF hardening, table-free AES source observations and fixed-length CMAC compare behavior. Exact OE-01 disassembly, statistical timing/cache work, power/EM/fault review and external specialist disposition remain open. No universal side-channel claim is permitted.

## 9. Build and dependency design

The Release candidate is a hidden-internals shared library with tests, examples, hooks, OpenSSL differential provider and sanitizers disabled. Runtime dependencies and allowed ELF imports must be captured on exact OE-01. OpenSSL remains test-only and is not a product runtime dependency.

## 10. Detailed-design completion gate

Before approval, resolve or formally disposition:

- runtime integrity implementation and tests;
- on-demand self-test transition deltas;
- exact OE-01 build/import/disassembly evidence;
- institution-confirmed entropy/conditional-test interpretation;
- formal CSP classification and zeroization method;
- required empirical side-channel/fault assessment;
- all high-priority P2-08 consultation decisions.
