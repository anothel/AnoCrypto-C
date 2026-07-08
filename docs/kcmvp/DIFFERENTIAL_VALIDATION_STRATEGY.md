# Differential Validation Strategy

Status: P1-12 draft.

## Purpose

Differential validation is used as an engineering check to compare AnoCrypto-C behavior against public standards, public test vectors, and independently implemented reference behavior where usage rights are clear. It does not replace formal validation.

## Allowed comparison inputs

Allowed inputs:

- Public standards.
- Public test vectors.
- User-owned AnoCrypto materials that are permitted for this project.
- Locally generated non-secret test cases.
- Independent tools or libraries only when license and usage terms are reviewed.

Not allowed:

- Non-public third-party code or documents.
- Customer data.
- Secret keys, entropy captures, private keys, tokens, or deployment material.
- Test vectors from unclear sources.
- Function names, file structures, constants, or wording copied from unclear sources.

## Strategy by service area

| Area | Current strategy | P1-12 status | Next step |
|---|---|---|---|
| SHA-256 | Public KATs, chunking consistency, invalid-argument negative tests | Active | Add cross-tool digest comparison only with generated public messages. |
| HMAC-SHA-256 | Public KATs, long-key path, streaming consistency, handle path coverage | Active | Add generated-message comparison with reviewed reference command/tool. |
| HMAC_DRBG-SHA-256 | Public KATs and boundary vectors | Active | Add deterministic generated-vector manifest with source recipe, not secret seed logs. |
| CSP/key handles | Lifecycle and stale/exhaustion negative tests | Active | Add service access-policy table before new key types. |
| Entropy/provider seams | Status-only tests | Internal only | Do not compare or store entropy bytes. Verify status and failure handling only. |
| Block cipher/modes | Not implemented | Planning only | Select candidate scope and vector sources before code. |
| Public random service | Not public | Planning only | Decide API after entropy and reseed policy review. |

## Generated test case rules

Generated test inputs must be deterministic and non-secret. Recommended inputs:

- ASCII labels such as `AnoCrypto-C P1 generated message 001`.
- Fixed byte sequences generated from counters.
- Fixed chunk partitions for streaming tests.
- Public personalization/additional-input labels for DRBG deterministic tests.

Generated test inputs must not be:

- Real entropy bytes.
- Customer identifiers.
- Private keys or seeds.
- Production tokens.
- Data copied from unclear sources.

## Reference comparison record

Each differential comparison should record:

- Service name.
- AnoCrypto-C API path.
- Reference source/tool and license review status.
- Test input generation recipe.
- Expected output generation method.
- Whether secret material is absent.
- Whether logs include only non-sensitive values.
- Result summary.

## Logging rules

Allowed in logs:

- Test case ID.
- Public input label.
- Status code.
- Digest or MAC values only for public deterministic test vectors.
- Build identity and compiler identity.

Not allowed in logs:

- Secret keys.
- Entropy bytes.
- DRBG internal state.
- Generated secret bytes.
- Private keys.
- Tokens.
- Customer data.

## P1-12 TODO

- Add a `tests/differential/` directory only after a reviewed reference source/tool is selected.
- Add a manifest format for generated non-secret vectors.
- Add CI gating only after the reference command/tool is available on the target runner matrix.
- Keep differential checks separate from KATs so provenance remains clear.
