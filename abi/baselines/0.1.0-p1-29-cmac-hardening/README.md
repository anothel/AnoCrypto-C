# P1-29 public compatibility baseline

This directory preserves the project-owned public header and exported-symbol
surface from `0.1.0-p1-29-cmac-hardening` for P1-30 compatibility checks.

The snapshot is used only by tests and policy tooling. It is not installed as
part of the SDK and is not copied into the customer PoC package.

Checks performed from this baseline:

- normalized public function declarations remain unchanged;
- public enum, typedef, and non-version constant declarations remain unchanged;
- the reviewed 69-symbol ABI-0 export list remains unchanged;
- a consumer compiled with these headers can link to and call the current
  shared library;
- the baseline snapshot hashes remain stable.

The expected baseline tag is metadata only. A reviewer must separately confirm
whether that Git tag exists and identifies the intended commit.
