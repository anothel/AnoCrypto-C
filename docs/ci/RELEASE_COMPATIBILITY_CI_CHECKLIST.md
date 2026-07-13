# Release Compatibility CI Checklist

Status: P1-32 engineering checklist. Actual hosted-run disposition remains TODO until the referenced workflow run is reviewed.

## Every supported build lane

- Configure with the P1-32 release fingerprint.
- Build and run the configured CTest set.
- Run `ano_crypto_release_compatibility_policy_check`.
- Preserve the 14-header and 69-export baselines.
- Do not enable entropy/random test hooks in Release-like configurations.

## Tagged-release lane

- Checkout full Git history and tags.
- Require `github.ref_name` to equal `v0.1.0-p1-32-signing-handoff`.
- When strict tag mode is enabled, provide both actual-tag and actual-commit inputs at configure/review time.
- Pass the workflow commit and require the tag, supplied commit, and `HEAD` to resolve to one commit.
- Run `ano_crypto_tagged_release_compatibility_smoke` on Linux, macOS, and Windows.
- Retain only status logs; do not retain cryptographic inputs or outputs.

## Installed-package compatibility

- Build a clean shared Release library with tests/hooks disabled and hidden internal symbols.
- Install to an isolated prefix.
- Build current-header and retained-P1-29-header consumers against that prefix.
- Confirm both consumers bind 69 functions and report the P1-32 runtime version.
- Confirm exact package version `0.1.0` is accepted.
- Confirm incompatible package request `0.2.0` is rejected.
- Confirm the retained header snapshot and compatibility source are not installed.

## Hosted artifact bundle

- Create `HOSTED_ARTIFACT_MANIFEST.txt` with lane, platform, architecture, compiler, commit, sizes, and SHA-256.
- Verify the manifest after upload/download.
- Reject binary/NUL-containing files, unlisted files, path traversal, duplicate basenames, oversized files, and sensitive-value markers.
- Build the final hosted artifact index only after all required lane/platform manifests are downloaded.
- Record reviewer, date, result, and limitations before changing support wording.

## Claims boundary

A green workflow is engineering evidence for that run. It is not by itself a universal ABI guarantee, release-signature verification, official validation result, completed side-channel review, or proof for untested platforms.
