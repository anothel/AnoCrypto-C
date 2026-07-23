# Project Source Replacement Plan

Status: `PHASE_1_CURRENT_ONLY_REPLACEMENT_READY`

## Replacement rule

The Phase 1 CLOSED CURRENT ONLY pack replaces every older active P1/P2/P3
status, roadmap, and next-work pack. Old and new CURRENT ONLY packs must not
remain active together.

## Current authorities

| Artifact | Role | Identity |
|---|---|---|
| AnoCrypto-C P3-10 source | final Phase 1 core source | `98f544a574f70b387c300f171243b4e87174937a` / `913979ceea9110de5dbfe9e6702c634071102b00901840763367ca83e15d59c4` |
| AnoSecureKit Enterprise P3-06 | symmetric/KDF downstream checkpoint | `53824f00ff3460ba05ac866d1dd58d9a0c01ff3c` |
| AnoPKI Enterprise P3-11 source | final PKI integration source | `c1e0bbbefa2003657c7fefdf5924f964c4bab3ec` / `fefae0c7c3fa340c782010ef4ca46cf8db6b923f437497cc6a7d4e1b54a798d2` |
| AnoPKI Enterprise P3-11 patch | exact Enterprise delta | `c822f58dc497d78f4173a43f8b8d8eef8e4c06c54db8f52745fa01a2183f66ac` |
| AnoPKI Enterprise P3-11 validation | detailed validation record | `ee3060381cd19d52c4bc78b55cfbcb93d9d04c44cd61dcdf18df10146774d837` |
| AnoPKI Enterprise P3-12 closeout | Phase 1 decision record | `1fb6fb3d6b85886ad5acf21ff994adb9177b90d10c0823dd0356e67629a2da58` |
| AnoPKI Enterprise P3-12 JSON | machine-readable evidence freeze | `6f150022f604a623300f6d495cfab4d337bc5d30e57345525205d0ec5188b9c9` |
| Phase 1 CLOSED CURRENT ONLY pack | direction/status authority | generated from this documentation refresh |

## Keep active

1. the new Phase 1 CLOSED CURRENT ONLY pack;
2. the exact AnoCrypto-C P3-10 source ZIP;
3. P3-11 source, patch, and validation records;
4. P3-12 closeout and validation records.

## Archive but do not keep active

- old P3 entry and next-work packs;
- P3-11 kickoff material;
- external customer release program drafts;
- intermediate P3-02 through P3-09 source/patch bundles;
- historical KCMVP preparation packs.

## Next-work rule

There is no active next-work prompt. Reopening requires a new product decision,
new exact baselines, and a separately authorized phase.
