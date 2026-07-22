# Project Source Replacement Plan

Status: `P3_10_COMPLETE_P3_11_ENTRY_REFRESH_REQUIRED`

## Current core authority

| Artifact | Role | Identity |
|---|---|---|
| AnoCrypto-C P3-10 source | current core source | `0.1.0-p3-10-restricted-sha1-ocsp-certid`; 18 headers / 85 symbols / ABI-7 |
| P3-10 patch and SHA-256 | exact implementation delta and integrity | generated from the reviewed single commit |
| AnoPKI exact Community/Enterprise sources | downstream operation authority | revisions already recorded in requirement trace |

The next CURRENT ONLY pack must replace P3-07/P3-09-era status, next-work, and
public-surface documents. Old and new CURRENT ONLY packs must not remain active
together.

The replacement pack must:

- identify P3-10 as the current core baseline;
- freeze 18 headers, 85 symbols, and ABI lines 0 through 7;
- mark all eight approved software-core/design gaps satisfied;
- select P3-11 AnoPKI Enterprise parity/no-fallback as downstream work;
- preserve KCMVP, hosted, signing, provenance, and release blockers.
