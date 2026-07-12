# Build Identity ABI-0 Decision

Status: P1-29 engineering decision retained.

AnoCrypto-C retains the existing public `ano_crypto_build_identity_t` layout
for the 0.x ABI-0 line. ABI version 1 field order, meaning, and pointer ownership
remain unchanged.

Rules:

- fields are not reordered, removed, or silently appended under ABI version 1;
- strings remain module-owned read-only pointers;
- incompatible layout changes require a new ABI version;
- tests verify field order and runtime query behavior;
- new services should prefer opaque contexts and function APIs rather than
  expanding the build-identity structure.

P1-25 added eight reviewed cipher functions, growing the stored public-symbol
baseline from 53 to 61 without changing this layout. P1-28 adds eight reviewed
CMAC functions and grows the baseline from 61 to 69, again without changing the
build-identity layout or ABI version.

P1-29 retains the 69-symbol baseline and the ABI version 1 layout while
hardening private CMAC state and review artifacts only.
