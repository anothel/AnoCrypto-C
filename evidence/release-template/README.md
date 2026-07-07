# AnoCrypto-C Release Evidence Template

Status: P0 skeleton draft

This directory is a template for release-candidate evidence. It is not a validation report and must not be described as completed certification evidence.

## Release candidate identity

| Field | Value |
|---|---|
| Product | AnoCrypto-C |
| Release candidate tag | TODO |
| Git commit | TODO |
| Source archive SHA-256 | TODO |
| Binary artifact SHA-256 | TODO |
| Compiler identity | TODO |
| Build type/options | TODO |
| Target OS/architecture | TODO |
| CMake version | TODO |
| Build timestamp policy | TODO |

## Module boundary candidate

- Library artifact: TODO
- Public headers: `include/anocrypto/*.h`
- Internal sources: `src/*`
- Tests/evidence: outside module boundary unless explicitly included later

## Test evidence checklist

| Evidence item | Required in P0? | Status | Notes |
|---|---:|---|---|
| Configure log | Yes | TODO | Save CMake configure output. |
| Build log | Yes | TODO | Save compiler and linker output. |
| Unit test log | Yes | TODO | Include `module_state_tests`. |
| Install smoke log | Yes | TODO | Include installed-package smoke test. |
| Self-test log | Yes | TODO | P0 framework only; not algorithm KAT evidence. |
| KAT log | P1 | TODO | Requires public-source vector manifest. |
| Negative test log | P1 | TODO | Add parameter/state failure tests. |
| SBOM | P1 | TODO | Tool/process not selected. |
| Signing record | P1/P2 | TODO | Signing policy not selected. |
| Known limitations | Yes | TODO | Must include no-algorithm-implementation limitation. |

## Known P0 limitations

- Cryptographic algorithms are not implemented.
- The self-test framework contains placeholders only.
- Thread-safety and concurrency policy are not finalized.
- Recovery policy from fatal/error state is not finalized.
- No public KAT vector manifest is included yet.
- No customer integration package is included yet.

## Clean-room evidence

Attach or reference:

- `cleanroom/SOURCE_MANIFEST.md`
- `cleanroom/EXCLUSION_LOG.md`
- Review note confirming no secret keys, seeds, entropy samples, private scalars, tokens, or certificate private keys are present.
- Review note confirming no unsupported assurance claims are present.

## TODO before a P1 evidence package

1. Add algorithm scope matrix.
2. Add KAT source manifest with public-source URLs, versions, and local SHA-256 hashes.
3. Add negative test plan.
4. Add reproducible build notes.
5. Add artifact naming and signing policy.
