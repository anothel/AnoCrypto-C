# Downstream Source Baselines

Status: `PHASE_1_CLOSED_PROVENANCE_OPEN`

## Retained P3-01 policy compatibility

Historical intake token: `4_OF_4_IMPORTED_WITH_ONE_IDENTITY_CONFLICT`. The
content conflict was later reconciled; trusted provenance remains open.

## Final Phase 1 integration baselines

| Lane | Repository/baseline | AnoCrypto-C input | Result |
|---|---|---|---|
| AnoSecureKit | Enterprise `53824f00ff3460ba05ac866d1dd58d9a0c01ff3c`, Community `694459ebe497d15ba75ef76a52fa7c36ddd7bcce` | P3-05 `89e16a261ab7696d9489167d7403d9f2c704629e` | local seven-capability parity and no fallback complete |
| AnoPKI | Enterprise `c1e0bbbefa2003657c7fefdf5924f964c4bab3ec`, Community `ab9d76597df93ac1ac8b7938f4d25ba64f59f8dc` | P3-10 `98f544a574f70b387c300f171243b4e87174937a` and pinned DLL `7dd56b00f38699509e90a62968ec0ff2c37a2735e4479c0df4a2c60f8b2e26cd` | local eight-operation parity and no fallback complete |

## AnoPKI artifact bindings

| Artifact | SHA-256 |
|---|---|
| P3-11 source ZIP | `fefae0c7c3fa340c782010ef4ca46cf8db6b923f437497cc6a7d4e1b54a798d2` |
| P3-11 patch | `c822f58dc497d78f4173a43f8b8d8eef8e4c06c54db8f52745fa01a2183f66ac` |
| P3-11 validation | `ee3060381cd19d52c4bc78b55cfbcb93d9d04c44cd61dcdf18df10146774d837` |
| P3-12 closeout Markdown | `1fb6fb3d6b85886ad5acf21ff994adb9177b90d10c0823dd0356e67629a2da58` |
| P3-12 validation JSON | `6f150022f604a623300f6d495cfab4d337bc5d30e57345525205d0ec5188b9c9` |

## Authority and limitations

- AnoPKI Enterprise exact Git commit `c1e0bbbefa2003657c7fefdf5924f964c4bab3ec` is the canonical source tree.
- Community exact baseline is `ab9d76597df93ac1ac8b7938f4d25ba64f59f8dc`; mutable checkout changes were not
  used.
- P3-11 convenience Source ZIP is retained for handoff but is not canonical for
  line-ending-sensitive files.
- The pinned SDK DLL is the exact final Enterprise validation artifact. A
  functionally equivalent rebuild is not identical artifact evidence.
- AnoSecureKit P3-06 was not rerun against the later P3-10 binary; the required
  P3-05 ABI subset remains retained, and no P3-10 Kit qualification is claimed.
- trusted delivery provenance, hosted qualification, signing, timestamping and
  customer release remain open/blocked.
