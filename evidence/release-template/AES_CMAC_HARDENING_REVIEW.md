# AES-CMAC Hardening Review

- Version slice: P1-29
- Status: engineering review template
- Official validation conclusion: none

## Implementation and API

- [ ] Public CMAC API remains eight functions.
- [ ] Dedicated CMAC-purpose CSP key type remains unchanged.
- [ ] Full 16-byte tag policy remains unchanged.
- [ ] Clean Release exported-symbol baseline remains 69 names.
- [ ] No raw-key, tag-truncation, persistence, export, or protocol API was added.

## Failure and lifecycle

- [ ] Failed reinitialization clears prior private state.
- [ ] Corrupted marked contexts are rejected and cleared.
- [ ] Context/input/output/output-length overlap is rejected before state use.
- [ ] One-shot failures clear the tag output and set written length to zero.
- [ ] Successful final and explicit cleanup clear the reviewed context extent.
- [ ] Short cleanup invalidates the authorized prefix and requires full retry.
- [ ] Oversized cleanup does not write past the reviewed context extent.
- [ ] 128-cycle lifecycle stress passes.

## Differential and concurrency

- [ ] Test-only OpenSSL 3.x CMAC differential reports 1,728 comparisons.
- [ ] No OpenSSL runtime dependency exists in the product library or PoC.
- [ ] Per-thread CMAC handles pass on the configured native thread backend.
- [ ] Shared read-only CMAC-purpose handle compute/verify passes.
- [ ] Temporary tags are cleared and no values are logged.

## Compiler and package review

- [ ] CMAC compiler-output summary is present or explicitly `NOT_CAPTURED`.
- [ ] Disassembly, when captured, is treated as an internal manual-review input.
- [ ] Installed SDK excludes private CMAC/AES headers and implementation source.
- [ ] Customer PoC excludes hardening/stress/differential test source and
      compiler-output artifacts.
- [ ] No key, message, tag, subkey, AES schedule, entropy, seed, DRBG state,
      generated random output, private key, token, or customer value appears.

## Remaining TODO / 확인 필요

- Hosted Windows/macOS artifact review:
- 32-bit/big-endian artifact review:
- Target/compiler-specific reviewer and disposition:
- ThreadSanitizer decision:
- Additional limitations:
