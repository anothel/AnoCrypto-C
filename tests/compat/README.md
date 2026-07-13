# Release compatibility consumers

The standalone C99 project builds two consumers against the installed P1-32
package:

1. a current-header/current-library consumer, which checks the installed source
   surface and package discovery;
2. a retained P1-29-header/current-library consumer, which checks the frozen
   `0.1.0-p1-29-cmac-hardening` header contract against the P1-32 library.

The shared probe references all 69 ABI-0 functions and checks stable enum,
macro, handle-width, and build-identity layout assumptions. Runtime output is
limited to version, header mode, data-model sizes, counts, and pass/fail status.
It does not print keys, messages, tags, digests, entropy, generated random
bytes, seed material, or DRBG state.

This is an engineering regression guard. It does not establish compatibility
for untested compiler ABIs, calling conventions, loaders, or all semantic
behavior.
