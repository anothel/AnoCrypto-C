# Tagged Release Compatibility Evidence

Status: P1-33 engineering evidence plan; not official validation evidence.

## Evidence objective

Provide a reproducible record that one reviewed source snapshot retains the P1-29 public C contract while using the P1-33 engineering version identity. The evidence is intended to support configuration-management and interface-review work, not to replace formal test or submission requirements.

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

The three commit identities must match and the checkout must be clean. P1-33 may prove this with a synthetic local tag in an isolated temporary repository. Actual tag creation, tag signing, and organization release authorization remain separate operations and are not performed by rehearsal.

Dirty state, a moved tag, a moved `HEAD`, commit mismatch, or ref/tag mismatch invalidates the rehearsal.

## Hosted artifact review

Downloaded hosted bundles must be manifest-verified and indexed. Human review must then disposition Linux 64-bit, Linux 32-bit, s390x big-endian, macOS, Windows, GCC/Clang compiler-output, OpenSSL differential, compatibility, provenance, and signing-handoff artifacts. Presence or hash verification alone is not support approval.

Record artifact hash, workflow commit/ref, platform, compiler, architecture, reviewer, UTC review date, disposition, and origin. When no hosted execution exists, state is `NOT_RUN`; downloaded but undispositioned output is `UNREVIEWED`. Synthetic rehearsal artifacts remain `SYNTHETIC_LOCAL` and cannot satisfy `HOSTED_RUNNER` review.

## Data minimization

Evidence may contain names, statuses, counts, versions, target metadata, file sizes, and hashes. It must not contain keys, raw entropy, deterministic entropy-hook bytes, seed material, DRBG state, generated random bytes, AES inputs/outputs, CMAC tags, private keys, signing secrets, tokens, customer data, or internal deployment details.

## Known limitations

- Normalized declarations do not model every language-ABI rule.
- Symbol equality does not prove calling convention, loader, layout, or semantic compatibility.
- The P1-29 consumer covers the reviewed public surface and selected lifecycle calls, not every legal operation sequence.
- Hosted platform results are `NOT_RUN` or `UNREVIEWED` until an actual run is reviewed.
- This evidence does not establish official validation status.
