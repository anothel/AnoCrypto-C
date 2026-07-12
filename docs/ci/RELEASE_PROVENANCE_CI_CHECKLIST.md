# Release Provenance CI Checklist

Status: P1-31 engineering checklist. Actual tag authorization, hosted artifact disposition, signing, and external release approval remain TODO.

## Configure and static policy

- Configure with release identity `0.1.0-p1-31-release-provenance`.
- Run `ano_crypto_release_compatibility_policy_check`.
- Run `ano_crypto_release_provenance_policy_check`.
- Confirm the clean Release public surface remains 14 headers and 69 symbols.
- Confirm `ANO_CRYPTO_RELEASE_SIGNING_MODE` is `UNSIGNED`.
- Reject credential/signature-like source files.

## Regression test

Run `ano_crypto_release_provenance_tests` and require:

- normal unsigned bundle creation and verification;
- SPDX source-inventory JSON generation;
- source-SBOM tamper and path/checksum record-mismatch rejection;
- source-archive member/content mismatch rejection;
- unlisted bundle-file and signature-like artifact rejection;
- unverified signed-status rejection;
- unverified external-release authorization rejection;
- unsafe source-path rejection;
- dirty untagged engineering-bundle recording and dirty tagged-build rejection;
- wrong actual-tag rejection;
- complete hosted-disposition fixture acceptance;
- incomplete hosted-disposition rejection.

The fixture uses public repository files only and stores no cryptographic secret material.

## Evidence bundle

Require the release evidence target to create:

- `release-provenance/RELEASE_PROVENANCE.txt`;
- `release-provenance/SOURCE_SBOM.spdx.json`;
- `release-provenance/RELEASE_ARTIFACTS.sha256`;
- `release-provenance/SIGNING_STATUS.txt` with `Status: UNSIGNED`;
- `release-provenance/RELEASE_AUTHORIZATION.md` with `Status: TODO` and `External release authorized: NO`;
- `RELEASE_PROVENANCE_CHECK.txt` with `Result: PASS`.

Confirm the evidence manifest hashes the provenance directory and check record.

## Hosted bundle

The hosted provenance lane may upload status/hash metadata and the source inventory. It must not upload private keys, signing tokens, certificate private keys, customer data, unpublished deployment data, entropy, seed, DRBG state, generated random values, AES values, or CMAC tags.

The hosted artifact index must require a `release-provenance|Linux` bundle category. Hash and presence checks do not complete human disposition.

## PoC boundary

- Include `RELEASE_PROVENANCE_PACKAGE_NOTES.txt` only.
- The internal source archive and source SBOM are not included in the customer PoC package.
- Do not include compatibility baselines, signing requests, hosted review tooling, source manifests, or internal evidence logs.
- Keep `SBOM_STATUS.txt` and `SIGNING_STATUS.txt` accurate for the PoC package itself.

## Manual review still required

- actual Git tag/commit/HEAD verification and clean-worktree confirmation;
- Windows, macOS, Linux 32-bit, s390x, differential, compiler-output, and compatibility artifact disposition;
- SPDX schema/tool validation if a downstream consumer requires it;
- dependency and vulnerability inventory scope;
- signer identity, key custody, detached-signature format, timestamp, and revocation policy;
- cryptographic signature verification and external-release authorization.
