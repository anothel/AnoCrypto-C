# KCMVP Side-Channel Review and Limitations — P2-06

Status: source and local-compiler review only. Policy ID `SCP-P2-06-R1`.

## Threat and observation model

The initial review considers timing and cache behavior visible to a local or
remote observer of software execution on the P2-02 x86-64 shared-library
candidate. Power, electromagnetic, microarchitectural transient-execution,
physical probing, and active fault attacks are not evaluated. Thread scheduling,
locking, OS entropy calls, process changes, and public input sizes may vary
runtime and are not hidden by the API.

## Primitive/service disposition

| Area | Source-level observation | Local compiler observation | Status |
|---|---|---|---|
| AES encrypt/decrypt | No S-box/T-table lookup arrays; S-box is computed with fixed-iteration GF operations. Round count is fixed by public key class (128/192/256). | Local GCC 14 output contains arithmetic/vector operations and fixed-offset stack/round-key accesses. Exact GCC 13.3 review pending. | `PARTIAL_SOURCE_AND_LOCAL_COMPILER_ONLY` |
| AES key schedule | Branches depend on public key size and word index, not key bytes; S-box computation is table-free. | Captured locally; selected-OE disposition absent. | `PARTIAL` |
| CBC | Work scales with public complete-block input length and public encrypt/decrypt mode. No padding parser exists. | No claim beyond primitive review. | `DOCUMENTED_PUBLIC_LENGTH_VARIATION` |
| CMAC generation | Work scales with public message length; final complete/partial block selection depends on public length. | Local capture retained. | `PARTIAL` |
| CMAC verification | Always compares the full fixed 16-byte tag and accumulates mismatch bits; mismatch position does not end the loop. | Local GCC 14 emits one fixed-length loop for the compare. Statistical timing validation is absent. | `FIXED_LENGTH_CONSTANT_WORK_CANDIDATE` |
| SHA-2 | Compression loop counts are fixed per processed block; total work scales with public input length. | No selected-OE disposition. | `DOCUMENTED_PUBLIC_LENGTH_VARIATION` |
| HMAC | Work scales with public message/key lengths; key preprocessing branches on key length. The API does not promise that key length is hidden. | No selected-OE disposition. | `PUBLIC_LENGTH_ASSUMPTION_REQUIRED` |
| HMAC_DRBG | Generate work scales with public output/additional-input lengths; internal comparisons are used for self-tests/engineering health candidates. | No empirical timing assessment. | `PARTIAL` |
| CSP/locking | Handle validity, purpose, generation, state, and lock contention affect timing. These are control-plane metadata, not a constant-time primitive interface. | Scheduler/lock effects not characterized. | `OUTSIDE_PRIMITIVE_CLAIM` |
| Zeroization | Volatile byte-store loop remains visible in local optimized output. | Exact OE-01 optimizer/register/spill review pending. | `PARTIAL` |

## Cache and memory-access findings

- Product AES source contains no secret-indexed S-box or T-table array.
- AES state and round-key accesses use fixed offsets for a selected public key
  class, but compiler spills and target-specific lowering require exact-binary
  review.
- The local GCC 14 AES output is not accepted as evidence for GCC 13.3.0.
- No cache-event counter study, Prime+Probe/Flush+Reload study, or cross-core
  leakage assessment has been performed.
- Generic x86-64 is retained; hardware AES dispatch is not implemented and no
  AES-NI side-channel claim is made.

## Timing-review boundary

P2-06 deliberately does not add a noisy pass/fail microbenchmark. A local timing
threshold can create false assurance and is not a substitute for an approved
methodology, fixed CPU/power-management controls, sufficient sampling, leakage
model, statistical acceptance rule, and independent review.

Before stronger wording, define and execute:

1. exact OE image, CPU model/microcode, frequency/governor, affinity and noise
   controls;
2. test classes for key/data/tag differences while public lengths remain fixed;
3. sample-size and statistical method;
4. cache and branch-performance counter collection policy;
5. named reviewer, artifact hashes, finding disposition and rerun criteria.

## Fault, power, EM and transient execution

These areas are `NOT_EVALUATED`. The software candidate currently has no masking,
redundant computation, infective countermeasure, fault sensor, power filtering,
or physical boundary. CET/IBT/shadow-stack and speculative-execution controls are
separate platform-hardening decisions and do not establish cryptographic
side-channel resistance.

## Consultation questions

- What side-channel evidence is expected for a portable software-only shared
  library at the intended security level?
- Is source plus selected-binary human review sufficient for the initial scope,
  or is statistical timing/cache testing required?
- Must public key length, input length, and operation type be treated as secret?
- Are hardware AES instructions expected, permitted, or required for the target?
- What fault/power/EM evidence is applicable to this software boundary?
