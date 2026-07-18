# Release Compatibility CI Checklist

Status: P3-03 engineering checklist retaining the P1-33 release controls. Actual hosted runs remain `NOT_RUN` or `UNREVIEWED` until referenced artifacts are reviewed.

## Every supported build lane

- Configure with the P3-03 release fingerprint.
- Build and run the configured CTest set.
- Run `ano_crypto_release_compatibility_policy_check`.
- Preserve all 14 retained headers and 69 `ANOCRYPTO_C_0` exports; verify the current 16-header/72-export additive surface.
- Do not enable entropy/random test hooks in Release-like configurations.

## Tagged-release lane

- Checkout full Git history and tags.
- Require the rehearsal tag to equal `v0.1.0-p3-03-aes256-gcm-oneshot`.
- When strict tag mode is enabled, provide both actual-tag and actual-commit inputs at configure/review time.
- Pass the workflow commit and require the tag, supplied commit, and `HEAD` to resolve to one commit.
- Require a clean checkout; reject dirty state, moved tag, moved `HEAD`, commit mismatch, and ref mismatch.
- Create a synthetic tag only in an isolated temporary clone or repository; never tag the user repository during rehearsal.
- Run `ano_crypto_tagged_release_compatibility_smoke` on Linux, macOS, and Windows.
- Retain only status logs; do not retain cryptographic inputs or outputs.

## Installed-package compatibility

- Build a clean shared Release library with tests/hooks disabled and hidden internal symbols.
- Install to an isolated prefix.
- Build current-header and retained-P1-29-header consumers against that prefix.
- Confirm the current consumer binds 72 functions, including HKDF-SHA-256 and AES-256-GCM one-shot, while the retained P1-29 consumer still binds its original 69 functions against the P3-03 library.
- Confirm exact package version `0.1.0` is accepted.
- Confirm incompatible package request `0.2.0` is rejected.
- Confirm the retained header snapshot and compatibility source are not installed.

## Hosted artifact bundle

- Create `HOSTED_ARTIFACT_MANIFEST.txt` with lane, platform, architecture, compiler, commit, sizes, and SHA-256.
- Verify the manifest after upload/download.
- Reject binary/NUL-containing files, unlisted files, path traversal, duplicate basenames, oversized files, and sensitive-value markers.
- Build the final hosted artifact index only after all required lane/platform manifests are downloaded.
- Record reviewer, date, result, and limitations before changing support wording.
- Record artifact origin as `HOSTED_RUNNER` or `SYNTHETIC_LOCAL`; never use a synthetic pass as hosted evidence.
- Record absent hosted execution as `NOT_RUN` and downloaded-but-unreviewed output as `UNREVIEWED`.

## Claims boundary

A green workflow is engineering evidence for that run. It is not by itself a universal ABI guarantee, release-signature verification, official validation result, completed side-channel review, or proof for untested platforms.
