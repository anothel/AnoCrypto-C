# CMAC Entry Gate Review

- Version slice: P1-27
- Review status: TODO
- Reviewer: TODO
- Date: TODO
- Source revision: TODO

## Decision

- [ ] AES-CMAC is selected before AES-CTR for the next narrow implementation candidate.
- [ ] The initial scope is AES-128/192/256, full 16-byte tags, streaming compute, one-shot compute, and one-shot verify.
- [ ] A dedicated CMAC key purpose is required before implementation.
- [ ] Tag truncation, CTR, GCM, GMAC, protocol formatting, persistence, and export remain deferred.

## No-implementation boundary

- [ ] `ac_cmac.h` is absent.
- [ ] No CMAC implementation/KAT source is present.
- [ ] No CMAC public symbol or key-type enum value was added.
- [ ] The clean Release public-symbol baseline remains 61.

## Evidence and claims

- [ ] Planned vectors have public provenance.
- [ ] Evidence will exclude key, message, subkey, intermediate, and tag values.
- [ ] No official-validation, completed-review, or universal side-channel claim is made.

## Disposition

- [ ] Approve P1-28 implementation entry.
- [ ] Hold for API/key-purpose revision.
- [ ] Hold for hosted/compiler/legal review.

Notes: TODO
