# Installed SDK package-version tests

The [exact-version probe](exact/) requests CMake package version `0.1.0`
and checks the loaded library's runtime version string. The
[relocation smoke script](run_relocated_sdk_smoke.sh) is an optional local
Linux developer check using a trusted checkout and build tools. It requires
Bash, CMake, GCC, GNU Make, GNU coreutils, and `ldd`.

From the repository root:

```sh
bash tests/package_version/run_relocated_sdk_smoke.sh
bash tests/package_version/run_relocated_sdk_smoke.sh --missing-config
```

The first command builds a fresh shared Release SDK and installs it into a
new scratch directory under `TMPDIR` (or `/tmp`). It moves the install to a
sibling path with spaces, checks the old path is absent and the library
digest is unchanged, then configures and builds the exact-version probe.
The probe runs after the cached config path and Linux loader resolution
match the moved installation. Success returns zero.

The second command exercises the same build and move, then directs package
discovery to a missing config in an empty scratch root. Expect a CMake
package-not-found diagnostic and a nonzero exit from the probe configure
step. The script stops before building or running that probe. Other failure
or timeout diagnostics require investigation.

Each invocation prints and retains its own scratch directory for inspection.
This smoke covers the tested Linux configuration under the existing
exact-version contract. It does not establish general SDK relocatability
or qualify the selected Q9 artifact.
