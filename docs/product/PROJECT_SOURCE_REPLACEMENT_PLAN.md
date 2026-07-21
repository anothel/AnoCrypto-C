# AnoCrypto-C GPT Project Source Replacement Plan

Status: `P3_08_COMPLETE_P3_09_ENTRY_REFRESH_REQUIRED`

The former P1-to-P2 and P3-01/P3-02 CURRENT ONLY packs are obsolete. They must
not coexist with the replacement pack because they identify P3-02 as active and
freeze a superseded 14-header/69-symbol surface.

## Current artifact roles

| Artifact | Repository role | Identity |
|---|---|---|
| AnoCrypto-C P3-08 RSA implementation source | current working source; RSA-2048 PKCS#1 v1.5 SHA-2 on ABI-5 | runtime version `0.1.0-p3-08-rsa-pkcs1-v15-sha2`; 17 headers / 83 symbols |
| AnoCrypto-C P3-07 architecture-gate source | current working source; P3-05 implementation plus P3-07 design documents | runtime version `0.1.0-p3-05-scrypt-fixed-profile`; P3-07 adds no code/API/ABI change |
| AnoCrypto-C P3-05 source | retained implementation baseline | commit `89e16a261ab7696d9489167d7403d9f2c704629e`, SHA-256 `18aa9050a808e4d541b2a6a10e91aa8e51367b95962b90db2183e26377bdbac1` |
| AnoSecureKit Enterprise P3-06 source | downstream parity/no-fallback checkpoint evidence | commit `53824f00ff3460ba05ac866d1dd58d9a0c01ff3c`, artifact SHA-256 `db17e4bc4426a45a8bc3bd0e6802e3cec954a800146a69b600d63070049313df` |

P3-06 is not an AnoCrypto-C core version. P3-07 is a documentation-only
working-source successor to P3-05 and retains the same runtime identity.

## Replacement CURRENT ONLY pack

1. `00_UPLOAD_AND_REPLACEMENT_GUIDE.md`
2. `01_CURRENT_ONLY_STATUS.md`
3. `02_PRODUCT_AND_CROSS_REPOSITORY_BOUNDARIES.md`
4. `03_IMPLEMENTED_CAPABILITY_INVENTORY.md`
5. `04_DOWNSTREAM_SOURCE_BASELINES.md`
6. `05_PRODUCT_OPERATION_CAPABILITY_MATRIX.md`
7. `06_APPROVED_GAPS_AND_P3_ROADMAP.md`
8. `07_API_ABI_EVOLUTION_POLICY.md`
9. `08_KCMVP_DEFERRED_POSITIONING.md`
10. `09_NEXT_WORK_PROMPT.md`
11. `10_P3_02_TO_P3_06_CLOSEOUT_AND_P3_07_ENTRY_REVIEW.md`
12. `11_PUBLIC_SURFACE_AND_ABI_BASELINE.md`
13. `PACK_MANIFEST.sha256`

## Authority order

1. exact current repository source ZIP and committed manifests;
2. this replacement CURRENT ONLY pack;
3. exact downstream Community/Enterprise source revisions;
4. completed task patches and evidence;
5. historical transition documents, for history only.

The repository-side refresh is recorded in
`docs/product/P3_06_CLOSEOUT_P3_07_ENTRY_DOCUMENTATION_DELTA.md`.

The generated pack is a separate GPT-project context artifact. It is not copied
into the product source tree and does not replace file-level design, test, or
evidence documents.

## P3-09 entry refresh

Before P3-09 implementation starts, replace the P3-07/P3-08 entry pack with a P3-08-closeout/P3-09-entry pack. That pack must identify P3-08 as complete, record the 17-header/83-symbol `ANOCRYPTO_C_5` surface, select only the reviewed P-256 ECDSA gate, and state that no additional header, symbol count, or ABI line is pre-approved. Historical packs must not remain active beside it.
