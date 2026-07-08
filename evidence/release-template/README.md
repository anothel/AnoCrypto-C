# anocrypto-c Release Evidence Template

This directory is a release evidence skeleton. It is not a completed release evidence package.

## Required Per-Candidate Artifacts

- Source archive SHA-256
- Binary SHA-256
- Git commit hash
- Compiler identity
- Build options
- Target OS/architecture
- Startup self-test log
- KAT log
- Negative test log
- Install smoke test log
- Known limitations
- Algorithm scope matrix
- Service scope matrix
- Differential validation strategy notes
- Side-channel review checklist status
- Customer PoC intake/scope notes when applicable
- Clean-room source manifest snapshot
- Exclusion log snapshot
- SBOM status
- Signing status
- Reproducible build notes
- Public surface check result
- CI matrix result summary
- Customer PoC package smoke log
- Customer PoC package manifest review

## P1-13 Evidence Notes

- SHA-256 and HMAC-SHA-256 processing APIs are implemented for the current P1 slice.
- HMAC-SHA-256 can also be computed through an in-memory HMAC key handle lifecycle slice.
- HMAC_DRBG-SHA-256 deterministic core APIs are implemented for the current P1 slice.
- The internal HMAC_DRBG-SHA-256 instantiate/reseed helper from the prior P1 slice remains covered by unit tests.
- Extended KAT tests cover public SHA-256 vectors, RFC 4231 HMAC-SHA-256 cases, the HMAC key handle path, and a public HMAC_DRBG-SHA-256 flow.
- Deterministic chunk-boundary tests cover one-shot versus streaming API consistency.
- Negative tests cover representative module gating, hash/HMAC/DRBG/CSP/key invalid arguments, finalized-context behavior, stale handles, and cleanup paths.
- Entropy adapter and provider tests verify internal seam behavior without storing raw entropy in logs.
- `tools/collect_release_evidence.cmake` creates a local engineering evidence draft from a build tree and can capture source archive SHA-256, public surface check output, reproducible-build notes, installed-package smoke logs, PoC package smoke logs, and scope-review notes.
- `docs/kcmvp/REPRODUCIBLE_BUILD_NOTES.md` records the current repeatable-build procedure and unresolved gaps.
- `docs/design/API_ABI_REVIEW_CHECKLIST.md` records the current public header and symbol-surface review checklist.
- `examples/core_poc` records the current public-API customer PoC integration smoke.
- `docs/roadmap/DEVELOPMENT_WORKFLOW.md` records the task flow and next-step plan.
- `docs/kcmvp/SERVICE_SCOPE_MATRIX.md` records implemented, reserved, internal-only, and deferred services.
- `docs/kcmvp/DIFFERENTIAL_VALIDATION_STRATEGY.md` records allowed comparison sources and logging limits.
- `docs/design/BLOCK_CIPHER_MODE_SELECTION.md` records the block-cipher implementation gate.
- `docs/security/SIDE_CHANNEL_REVIEW_CHECKLIST.md` records key-dependent code path review questions.
- `docs/business/CUSTOMER_POC_INTAKE_QUESTIONNAIRE.md` records non-secret customer intake questions.
- `docs/kcmvp/SBOM_SIGNING_PLAN.md` records SBOM/signing placeholders and boundaries.
- `docs/kcmvp/EVIDENCE_MANIFEST_POLICY.md` records manifest generation rules.
- `docs/business/POC_PACKAGE_REVIEW_MANIFEST.md` records customer package review fields.
- `tools/collect_release_evidence.cmake` writes `SBOM_STATUS.txt`, `SIGNING_STATUS.txt`, `EVIDENCE_MANIFEST_README.txt`, and `EVIDENCE_MANIFEST.sha256` for P1-13 evidence drafts.

## Commands

```sh
cmake -S . -B build -DANO_CRYPTO_BUILD_TESTS=ON -DANO_CRYPTO_BUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Install smoke:

```sh
cmake --install build --prefix /tmp/anocrypto-c-p1-13
cmake -S tests/install_smoke -B build-install-smoke -DCMAKE_PREFIX_PATH=/tmp/anocrypto-c-p1-13
cmake --build build-install-smoke --parallel
./build-install-smoke/anocrypto_c_install_smoke
```

Customer PoC package smoke:

```sh
cmake --build build --target ano_crypto_poc_package_smoke
```

Evidence draft:

```sh
cmake --build build --target ano_crypto_collect_release_evidence
```

## TODO

- Add KAT vector local-copy hash policy when vectors are stored as separate data files.
- Add CI-produced evidence bundle layout after workflow review.
- Replace SBOM/signing placeholder fields with reviewed generator/signing outputs when tool and key-management policies are selected.
- Add customer PoC legal/evaluation notice after legal review.
- Add reviewed platform CI evidence for Linux, Windows, and Apple entropy provider branches.
- Add archive timestamp normalization if byte-identical source archives become required.
- Add public random-service API only after provider, reseed policy, fork-safety, and validation-boundary review.
- Add first block cipher/mode implementation only after P1-12 scope inputs are reviewed.
- Add differential tests only after a reviewed public/reference source or tool is selected.

## P1-13 Log Review

If present, review entropy/thread/helper test logs for:

- status-only output
- no key bytes
- no MAC/DRBG output bytes
- no entropy bytes
- no seed or nonce bytes
- no broad support claims beyond the documented global-state lock, internal platform entropy provider, and internal DRBG helper policies
- provider logs include provider name/status only and no entropy bytes
- public surface check is PASS or has documented TODO-only warnings
- reproducibility gaps remain marked as TODO
- PoC package manifests and logs do not include secrets, customer data, or internal deployment details
- scope-review notes do not expand the implemented algorithm boundary by implication
- SBOM/signing status files remain clear placeholders unless reviewed generated artifacts are present
- evidence and package manifests use intended file paths only
