# AES-CMAC Implementation Review

- Version slice: P1-28
- Review status: TODO
- Reviewer: TODO
- Date: TODO
- Source revision: TODO

## Scope

- [ ] AES-128/192/256 CMAC is implemented.
- [ ] Only full 16-byte tags are accepted.
- [ ] One-shot compute/verify and streaming compute are covered.
- [ ] Dedicated CMAC-purpose CSP handles are enforced.
- [ ] Tag truncation, CTR, GCM, GMAC, persistence, export, and protocol formatting remain deferred.

## Tests and evidence

- [ ] Startup KAT passes without logging values.
- [ ] Public NIST example KATs pass for all three key sizes.
- [ ] Negative and wrong-purpose handle tests pass.
- [ ] Test-only differential comparison passes.
- [ ] Install and PoC smoke pass without printing tags or keys.

## ABI and package

- [ ] Eight reviewed CMAC symbols are added and no prior symbol is removed.
- [ ] Clean Release baseline contains 69 public symbols.
- [ ] Public `ac_cmac.h` is installed.
- [ ] Internal CMAC source/context/tests and OpenSSL provider artifacts are absent from SDK and PoC packages.

## Remaining review

- [ ] Target-specific compiler and timing/cache review remains TODO.
- [ ] No official-validation or universal side-channel claim is made.

Notes: TODO
