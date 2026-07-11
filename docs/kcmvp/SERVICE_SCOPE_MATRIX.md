# AnoCrypto-C Service Scope Matrix

Status: P1-19 draft.

## Purpose

This matrix separates implemented services, ABI-reserved services, internal seams, and deferred product-layer features. It is an engineering planning artifact for KCMVP readiness preparation and customer PoC scoping. It is not official validation evidence.

## Service matrix

| Service area | Public API surface | Implementation state | Candidate boundary status | Evidence currently available | Next decision |
|---|---|---|---|---|---|
| Module lifecycle | `ano_crypto_module_*` | Implemented skeleton | In core boundary | Unit/negative tests, install smoke, PoC smoke | Error recovery policy remains TODO. |
| Self-test | `ano_crypto_selftest_*` | Implemented skeleton with startup KAT path | In core boundary | Startup KAT coverage and CTest logs | Integrity check strategy remains TODO. |
| Build/version identity | `ano_crypto_version_*`, `ano_crypto_build_*` | Implemented | In core boundary | Install smoke and PoC smoke query version/build identity | Consider replacing public build identity struct before ABI freeze. |
| SHA-256 | `ano_crypto_hash_*` | Implemented | Candidate core primitive | Public-source KATs and chunk-boundary tests | Keep as P1 baseline. |
| SHA-384/SHA-512 | `ano_crypto_hash_*` algorithm IDs, names, digest-size queries, context-size queries | ABI-reserved only | Not in implemented candidate slice | Reserved-API negative test and SHA-2 scope policy check | Keep reserved until implementation, KAT, differential-validation, and evidence updates are ready. |
| HMAC-SHA-256 | `ano_crypto_hmac_*` | Implemented | Candidate core primitive | Public-source KATs, long-key coverage, streaming tests | Keep as P1 baseline. |
| HMAC key handle path | `ano_crypto_key_*`, `ano_crypto_csp_*`, HMAC handle use | Implemented in-memory slice | Candidate CSP/key lifecycle slice | KAT path, stale-handle, exhaustion, lifecycle negative tests | Persistence/export/wrapping remain out of scope. |
| HMAC_DRBG-SHA-256 | `ano_crypto_drbg_*` | Implemented deterministic caller-entropy core | Candidate DRBG core slice | Public-source KATs, boundary tests, negative tests | Explicit-entropy contexts remain caller-managed across process changes. |
| Internal entropy adapter | Internal only | Implemented seam | Internal support seam | Unit tests and provider status tests | Do not expose entropy bytes or provider internals in logs. |
| Platform entropy provider | Internal only | Implemented Linux/Windows/Apple branch seam | Internal support seam | Status-only engineering tests and failure injection | Hosted CI results and unsupported-platform policy still need review. |
| Public random service | `ano_crypto_random_*` in `ac_random.h` | Implemented with platform entropy-backed HMAC_DRBG-SHA-256 | Candidate core service; final validation mapping pending | Lifecycle/negative/fork tests, provider-disabled lane, failure injection, install/PoC smoke, policy/evidence records | Review hosted results, VM snapshot/suspend guidance, lifecycle stress, and final validation-target inclusion. |
| Block cipher/modes | No public header yet | Not implemented | Not in current candidate slice | Selection criteria only | Select algorithm/mode based on customer and validation-target inputs. |
| KDF expansion | No public header yet | Not implemented | Deferred | None | Defer until block cipher and customer scope are clearer. |
| Public-key primitives | No public header yet | Not implemented | Deferred | None | P2/P3 planning only. |
| AnoSecureKit helpers | Out of AnoCrypto-C core | Not implemented here | Separate product layer | Boundary documents only | Keep out of core. |
| AnoPKI workflows | Out of AnoCrypto-C core | Not implemented here | Separate product layer | Boundary documents only | Keep out of core. |

## Candidate boundary rules

- Keep the initial candidate boundary centered on the shared library, installed public headers, self-test, DRBG, CSP/key lifecycle, random service, and evidence workflow.
- Treat tests, generated evidence, examples, and customer PoC packaging as support artifacts outside the cryptographic module boundary unless a later formal boundary decision says otherwise.
- Do not move SecureKit or PKI workflows into the core module.
- Do not add public APIs for algorithms whose parameter rules, service gating, self-test obligations, and negative tests are not documented.
- Do not interpret implementation of a service as a completed validation decision.

## Evidence expectations by service

Every service that enters the implemented candidate slice should have:

- Public header/API entry or an explicit internal-only note.
- Self-test or KAT strategy where applicable.
- Negative tests for invalid arguments, invalid state, and unsupported parameters.
- CSP/key lifecycle mapping where secret state exists.
- Side-channel review entry where key-dependent code paths exist.
- Install/PoC smoke coverage if the service is customer-facing.
- Evidence notes that avoid secret, entropy, seed, DRBG-state, or generated-byte output.

## Historical decisions

### P1-16 public random boundary

P1-16 reserved `ac_random.h` and fail-closed placeholders so the ABI shape could be reviewed before implementation.

### P1-17 entropy/health boundary

P1-17 added internal deterministic failure injection, bounded health-test candidates, and sticky fatal-state behavior without widening the installed ABI.

### P1-18 entropy operational boundary

P1-18 classified reviewed OS APIs as external conditioned providers, restricted direct-noise health-test skeletons to explicit internal raw-noise candidates, and added source-backed process-change detection.

### P1-19 public random service

P1-19 implements the previously reserved symbols without adding an exported public function.

- The service is fixed to platform entropy-backed HMAC_DRBG-SHA-256.
- Caller-owned storage remains opaque and mutable.
- Explicit-entropy DRBG contexts are rejected by random-service operations, and random-service contexts are rejected by explicit-entropy DRBG generate/reseed operations.
- A detected process change requires a successful random-service reseed before generation continues.
- Prediction resistance, VM snapshot/restore detection, and suspend/resume detection remain unsupported.
- Direct raw-noise acquisition and finalized source-specific health tests remain out of scope.
