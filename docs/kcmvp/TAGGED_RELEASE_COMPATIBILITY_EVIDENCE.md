# Tagged Release Compatibility Evidence

Status: P1-31 engineering evidence plan; not official validation evidence.

## Evidence objective

Provide a reproducible record that one reviewed source snapshot retains the P1-29 public C contract while using the P1-31 engineering version identity. The evidence is intended to support configuration-management and interface-review work, not to replace formal test or submission requirements.

## Required local records

- `release/ANOCRYPTO_C_RELEASE_IDENTITY.cmake`
- `release/PUBLIC_HEADERS.sha256`
- `abi/baselines/0.1.0-p1-29-cmac-hardening/`
- `RELEASE_COMPATIBILITY_CHECK.txt`
- `TAGGED_COMPATIBILITY_SMOKE.txt`
- `EXPORTED_SYMBOL_BASELINE_CHECK.txt`
- current-header and P1-29-header consumer logs
- exact/incompatible CMake package-version results
- `RELEASE_COMPATIBILITY_REVIEW_NOTES.txt`

## Required tagged-build record

For a release-tagged workflow, record:

- expected and actual tag names;
- checked-out `HEAD` commit;
- workflow-supplied commit;
- tag-resolved commit;
- clean working-tree/repository status where applicable;
- source archive and binary hashes;
- toolchain, target, build options, and installed-package test status.

The three commit identities must match. Annotated-tag signature or organization release authorization is a separate TODO until a signing policy is selected.

## Hosted artifact review

Downloaded hosted bundles must be manifest-verified and indexed. Human review must then disposition Linux 64-bit, Linux 32-bit, big-endian, macOS, Windows, compiler-output, differential, and compatibility artifacts. Presence or hash verification alone is not support approval.

## Data minimization

Evidence may contain names, statuses, counts, versions, target metadata, file sizes, and hashes. It must not contain keys, raw entropy, deterministic entropy-hook bytes, seed material, DRBG state, generated random bytes, AES inputs/outputs, CMAC tags, private keys, signing secrets, tokens, customer data, or internal deployment details.

## Known limitations

- Normalized declarations do not model every language-ABI rule.
- Symbol equality does not prove calling convention, loader, layout, or semantic compatibility.
- The P1-29 consumer covers the reviewed public surface and selected lifecycle calls, not every legal operation sequence.
- Hosted platform results are TODO until an actual run is reviewed.
- This evidence does not establish official validation status.
