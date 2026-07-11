# C99 Entropy Source and Operational Policy

Status: P1-18 engineering policy draft.

## Purpose

P1-18 fixes the internal classification, health-test ownership, provider probe, and process-boundary policy that must be reviewed before any public random-generation service is enabled.

This document is an engineering boundary record. It does not establish entropy quality, platform certification, formal validation status, or suitability for every customer environment.

## P1-18 decision summary

1. Linux `getrandom()`, Windows `BCryptGenRandom()`, and Apple `SecRandomCopyBytes()` are treated as **external conditioned random providers**.
2. AnoCrypto-C does not treat those API outputs as direct raw-noise samples.
3. The P1-17 duplicate-block startup/continuous candidates must not be attached to an OS-provider source.
4. Those candidates may attach only to an explicitly classified internal raw-noise engineering callback.
5. The platform-provider operational probe records only availability/status and always zeroizes its temporary buffer.
6. A source-backed internal HMAC_DRBG context records the creating process identity. A detected process change causes generation to fail with `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED` and clears caller output.
7. Successful reseed from the configured source refreshes the process marker.
8. The public explicit-entropy DRBG API remains caller-managed; P1-18 does not silently claim that caller-provided entropy contexts are automatically fork-safe.
9. The public random API remains reserved and returns `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` for processing operations.

## Source classes

| Internal class | Examples | Health-test owner | P1-18 handling |
|---|---|---|---|
| `NONE` | Unconfigured source | None | Reject requests. |
| `CALLBACK_UNSPECIFIED` | Test/customer integration callback without raw-source classification | Caller/integrator | No module health-test attachment. Callback contract and restart policy remain caller-defined. |
| `EXTERNAL_CONDITIONED_RBG` | Reviewed OS API branch selected by CMake | External provider boundary | Status-only operational probe; no module RCT/APT-style test over returned bytes. |
| `RAW_NOISE_TEST_CANDIDATE` | Explicit internal engineering callback | Module engineering candidate | P1-17 bounded duplicate-block skeleton may be attached for state-transition tests only. |

The raw-noise class is internal and is not installed or exported as public ABI.

## Why OS-provider output is not the raw-noise test point

A health test intended to observe a noise source must be placed at the source boundary before conditioning when such a boundary is available and within the module design. The current OS APIs expose an already conditioned provider service, not direct access to raw samples. Applying the current duplicate-block candidate to those returned bytes would test a different layer and could create misleading evidence.

Therefore P1-18 records provider health-test ownership as external and keeps module health-test candidates detached from OS-provider output.

## Operational probe

`ano_crypto_internal_entropy_source_probe_operational()` is internal and accepts only the OS-provider class.

Behavior:

- requests a fixed 32-byte temporary buffer from the configured provider;
- returns provider status only;
- clears the temporary buffer before return on success or failure;
- rejects callback and raw-noise candidate classes;
- does not write the temporary value to logs, evidence, fixtures, or package records.

The probe is an availability/path check. It is not an entropy-quality test.

## Failure and state policy

| Condition | Status | Output/context action | Module state |
|---|---|---|---|
| OS provider unsupported | `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` | Output cleared | Unchanged |
| OS provider runtime failure or partial failure | `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED` | Output cleared; failed helper instantiation clears context | Unchanged unless a separate fatal health policy has already latched error state |
| Raw-noise candidate health failure, operation-local policy | `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED` | Current output cleared | Unchanged |
| Raw-noise candidate health failure, module-fatal policy | `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED` | Current output cleared; affected source-backed DRBG context cleared by helper failure path | Sticky module `ERROR_STATE` |
| Source-backed DRBG process marker changed | `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED` | Generate output cleared | Unchanged |
| Reseed succeeds after process change | `ANO_CRYPTO_OK` | Process marker refreshed | Unchanged |
| Reseed source fails after process change | Source failure status | Entropy temporary cleared; DRBG context remains unusable until a successful reviewed recovery path | Unchanged unless fatal health policy applies |

A transient provider failure is not automatically promoted to module-fatal state in P1-18. The fatal transition remains reserved for an explicitly configured module-fatal health-test policy.

## Process and fork boundary

The process guard is embedded only in the private DRBG context implementation.

- On supported targets, the creating process ID is stored as non-secret metadata after source-backed instantiation.
- Before generation, a source-backed context compares the current process ID with the stored marker.
- A mismatch fails before DRBG state is advanced and clears the requested output.
- Reseed from the internal source updates the marker after the DRBG update succeeds.
- On targets without a reviewed process identity primitive, the guard stays inactive and the limitation must remain documented.

P1-18 does not add at-fork callbacks, background reseeding, global random state, or a public process-reset API.

## Startup and continuous health-test policy

Current decision:

- no startup or continuous raw-noise test is enabled for the OS-provider class;
- the P1-17 duplicate-block implementation remains an internal engineering skeleton;
- its sample block size, cutoff, false-positive analysis, and source placement are not final;
- a future direct noise-source integration must define source-specific RCT/APT or other reviewed tests before module use;
- failure behavior must be chosen explicitly as operation-local or module-fatal and documented in the source configuration.

## Logging and evidence boundary

Allowed records:

- provider branch name;
- source class name or numeric policy identifier;
- status name;
- process-guard test pass/fail result;
- policy-check result;
- source/binary/package hashes.

Prohibited records:

- provider output;
- entropy or seed material;
- nonce material used for DRBG instantiation;
- DRBG key/value state;
- generated random output;
- private keys, tokens, customer secrets, or customer deployment details.

## Public ABI impact

- No public header exposes source-class, health-owner, restart-policy, process-guard, or raw-noise-candidate types.
- No new public function is added.
- The private DRBG context size increases. Callers must continue to use `ano_crypto_drbg_context_size()` instead of hard-coding a size.
- Exported public symbol names are expected to remain unchanged.

## References used for the policy review

- NIST SP 800-90B, *Recommendation for the Entropy Sources Used for Random Bit Generation*.
- NIST SP 800-90C, *Recommendation for Random Bit Generator (RBG) Constructions*.
- Linux `getrandom(2)` manual page.
- Microsoft `BCryptGenRandom` documentation.
- Apple Security framework `SecRandomCopyBytes` documentation.

These references are used to establish terminology and boundary questions. P1-18 does not copy third-party implementation code or source-specific thresholds.

## TODO / confirmation required

1. Confirm the final target operating systems and minimum supported versions.
2. Review process-change behavior on Windows and Apple hosted CI.
3. Decide whether unsupported process identity must disable a future public random service.
4. Select a direct noise source only if a customer/validation scope requires one.
5. For any direct noise source, define sample representation, startup test, continuous test, thresholds, false-positive handling, and recovery policy.
6. Decide whether the public random service uses caller-owned opaque storage or library-owned handles.
7. Keep public random generation reserved until the above items and cross-platform CI evidence are reviewed.
