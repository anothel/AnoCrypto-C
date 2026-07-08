# AnoCrypto-C Service Scope Matrix

Status: P1-14 draft.

## Purpose

This matrix separates implemented services, ABI-reserved services, internal seams, and deferred product-layer features. It is an engineering planning artifact for KCMVP readiness preparation and customer PoC scoping. It is not official validation evidence.

## Service matrix

| Service area | Public API surface | Implementation state | Candidate boundary status | Evidence currently available | Next decision |
|---|---|---|---|---|---|
| Module lifecycle | `ano_crypto_module_*` | Implemented skeleton | In core boundary | Unit/negative tests, install smoke, PoC smoke | Error recovery policy remains TODO. |
| Self-test | `ano_crypto_selftest_*` | Implemented skeleton with startup KAT path | In core boundary | Startup KAT coverage and CTest logs | Integrity check strategy remains TODO. |
| Build/version identity | `ano_crypto_version_*`, `ano_crypto_build_*` | Implemented | In core boundary | Install smoke and PoC smoke query version/build identity | Consider replacing public build identity struct before ABI freeze. |
| SHA-256 | `ano_crypto_hash_*` | Implemented | Candidate core primitive | Public-source KATs and chunk-boundary tests | Keep as P1 baseline. |
| SHA-384/SHA-512 | `ano_crypto_hash_*` algorithm IDs and size queries | ABI-reserved only | Not in implemented candidate slice | Size-query behavior only | Decide implementation timing. |
| HMAC-SHA-256 | `ano_crypto_hmac_*` | Implemented | Candidate core primitive | Public-source KATs, long-key coverage, streaming tests | Keep as P1 baseline. |
| HMAC key handle path | `ano_crypto_key_*`, `ano_crypto_csp_*`, HMAC handle use | Implemented in-memory slice | Candidate CSP/key lifecycle slice | KAT path, stale-handle, exhaustion, lifecycle negative tests | Persistence/export/wrapping remain out of scope. |
| HMAC_DRBG-SHA-256 | `ano_crypto_drbg_*` | Implemented deterministic core | Candidate DRBG core slice | Public-source KATs, boundary tests, negative tests | Public random service and entropy policy remain separate decisions. |
| Internal entropy adapter | Internal only | Implemented seam | Internal support seam | Unit tests and provider status tests | Do not expose entropy bytes or provider internals in logs. |
| Platform entropy provider | Internal only | Implemented Linux/Windows/Apple branch seam | Internal support seam | Status-only engineering tests | Hosted CI coverage still needs review. |
| Public random service | No public header yet | Design draft only | Not in public ABI | Design notes only | Decide after entropy/reseed policy review. |
| Block cipher/modes | No public header yet | Not implemented | Not in current candidate slice | Selection criteria only | Select algorithm/mode based on customer and validation-target inputs. |
| KDF expansion | No public header yet | Not implemented | Deferred | None | Defer until block cipher and customer scope are clearer. |
| Public-key primitives | No public header yet | Not implemented | Deferred | None | P2/P3 planning only. |
| AnoSecureKit helpers | Out of AnoCrypto-C core | Not implemented here | Separate product layer | Boundary documents only | Keep out of core. |
| AnoPKI workflows | Out of AnoCrypto-C core | Not implemented here | Separate product layer | Boundary documents only | Keep out of core. |

## Candidate boundary rules

- Keep the initial candidate boundary centered on the shared library, installed public headers, self-test, DRBG, CSP/key lifecycle, and evidence workflow.
- Treat tests, generated evidence, examples, and customer PoC packaging as support artifacts outside the cryptographic module boundary unless a later formal boundary decision says otherwise.
- Do not move SecureKit or PKI workflows into the core module.
- Do not add public APIs for algorithms whose parameter rules, service gating, self-test obligations, and negative tests are not documented.

## Evidence expectations by service

Every service that enters the implemented candidate slice should have:

- Public header/API entry or an explicit internal-only note.
- Self-test or KAT strategy where applicable.
- Negative tests for invalid arguments, invalid state, and unsupported parameters.
- CSP access policy when key material is involved.
- Output-length and aliasing rules.
- Evidence workflow entry.
- Clean-room source/provenance record.
- Claims-boundary review.

## Current P1-12 conclusion

Do not start block cipher implementation yet. First collect customer/platform inputs and choose the candidate algorithm/mode scope with a documented decision record.
