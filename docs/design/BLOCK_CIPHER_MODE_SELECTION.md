# Block Cipher and Mode Selection Decision Record

Status: P1-15 draft.

## Decision summary

No block cipher or mode implementation is added in P1-12. The next implementation should start only after customer requirements, target platforms, candidate validation boundary, test-vector sources, self-test requirements, CSP handling, and side-channel review expectations are recorded.

## Why this is a separate gate

Block cipher support changes more than a source file. It can affect:

- Public API and ABI.
- Service registry and module-state gating.
- Self-test/KAT requirements.
- Key/CSP lifecycle and zeroization paths.
- Error behavior for invalid key sizes, IV/nonce lengths, padding, and authentication tags.
- Output-buffer aliasing rules.
- Side-channel review scope.
- Customer PoC package contents.
- Evidence workflow and release notes.

## Candidate decision inputs

| Input | Required before implementation? | Notes |
|---|---:|---|
| Customer algorithm requirement | Yes | Record algorithm, mode, key sizes, platform, and integration use case. |
| KCMVP candidate scope | Yes | Confirm whether the algorithm/mode belongs in the candidate core boundary. |
| Public test-vector source | Yes | Source must be public or user-owned and recorded in clean-room records. |
| API/ABI draft | Yes | Include context sizing, output sizing, aliasing rules, and status mapping. |
| CSP lifecycle mapping | Yes | Define import/use/destroy/export restrictions before code. |
| Self-test/KAT design | Yes | Add known-answer and negative paths before or with implementation. |
| Side-channel checklist | Yes | Review key-dependent branches, table lookups, memory access patterns, and cleanup paths. |
| CI runner support | Recommended | Make sure target toolchains build the new code. |
| Evidence workflow update | Yes | Add logs and scope notes without stronger claims than the evidence supports. |

## Candidate implementation options

| Option | Description | P1-12 position |
|---|---|---|
| AES-ECB internal primitive only | Minimal primitive for later modes and KATs | Possible starting point, but not externally useful by itself. |
| AES-CBC | Common legacy integration mode | Requires padding/no-padding policy and IV handling rules. |
| AES-CTR | Stream-like mode | Requires counter construction and reuse policy. |
| AES-GCM | AEAD mode | Requires nonce/tag/AAD policy and misuse-resistant higher-level guidance outside core. |
| Domestic algorithm candidate | Customer/KCMVP-driven choice | Requires explicit requirement and public/reference vector source review. |

## API design constraints

- Keep C99 only.
- Do not require C++ runtime features.
- Return `ano_crypto_status_t` and use output parameters.
- Avoid public struct layout exposure where possible.
- Provide context-size query before caller-allocated contexts.
- Define whether input/output aliasing is allowed per function.
- Define exact behavior for short output buffers.
- Keep nonce/padding/policy helpers that belong to AnoSecureKit out of the core.

## P1-12 decision

Block cipher implementation is deferred. The next code step should begin with an API sketch and tests after a customer or validation-target candidate scope is selected.

## Open TODOs

- Decide first algorithm and mode.
- Decide whether the first block cipher API should be one-shot only, streaming, or both.
- Decide context allocation style.
- Decide whether AEAD belongs in the first block cipher wave or a separate wave.
- Decide KAT and negative vector sources.
- Decide if a separate service registry file is needed before code.
