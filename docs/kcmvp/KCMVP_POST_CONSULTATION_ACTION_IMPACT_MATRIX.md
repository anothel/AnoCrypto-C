# KCMVP Post-consultation Action and Impact Matrix

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-AI-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_REHEARSAL` |
| Owner role | Configuration manager |
| Reviewer | `TODO` |
| Approval | `NO` |
| Policy ID | `PCR-P2-08-R1` |

## Action Classes

| Class | Meaning | Required control |
|---|---|---|
| `ACT-0_RECORD_ONLY` | Answer confirms existing wording with no code/test/config change. | Update response record, decision register, and affected controlled documents. |
| `ACT-1_DOCUMENT_REVISION` | Answer changes terminology, form, operator guidance, or evidence metadata only. | Numbered document work item, review, policy check, no public ABI change. |
| `ACT-2_INTERNAL_IMPLEMENTATION` | Answer requires internal implementation or test work without public ABI change. | Numbered implementation work item, threat/risk review, tests, evidence, compatibility checks. |
| `ACT-3_CONFIGURATION_OR_OE_VARIANT` | Answer changes build profile, module variant, compiler/OS/CPU scope, or retest matrix. | Candidate revision, configuration-management review, exact target evidence. |
| `ACT-4_PUBLIC_API_ABI_GATE` | Answer appears to require a new/changed public symbol, type, constant, header, or semantic contract. | Stop; perform explicit API/ABI and product-layer gate before implementation. |
| `ACT-5_ALGORITHM_SCOPE_GATE` | Answer requires or removes an algorithm/service. | Stop; perform separate validation-target/product scope gate. |
| `ACT-6_EXTERNAL_EVIDENCE` | Answer requires institution, specialist, hosted, entropy, side-channel, signing, or exact-OE evidence. | Define external owner, protected channel, artifact format, reviewer, retention, and blocker. |
| `ACT-7_DEFER_OR_EXCLUDE` | Requirement is outside the initial candidate or cannot be met safely. | Record exclusion, impact, operator/customer wording, and escalation route. |

## Decision Routing

| Decision group | Typical answer effect | Default action class | Mandatory follow-up |
|---|---|---|---|
| Boundary/shared versus static | Accept current shared-object boundary | `ACT-0_RECORD_ONLY` or `ACT-1_DOCUMENT_REVISION` | Update basic/detailed design and boundary matrix. |
| Boundary or multiple variants rejected | Separate static/shared or expand boundary | `ACT-3_CONFIGURATION_OR_OE_VARIANT`; possibly `ACT-4_PUBLIC_API_ABI_GATE` | Re-freeze candidate before mechanism work. |
| Security level | Level and assurance interpretation selected | `ACT-1_DOCUMENT_REVISION`, `ACT-2_INTERNAL_IMPLEMENTATION`, and/or `ACT-6_EXTERNAL_EVIDENCE` | Perform full requirements impact review. |
| Runtime integrity mechanism accepted | HMAC candidate accepted with conditions | `ACT-2_INTERNAL_IMPLEMENTATION` plus configuration-management controls | Implement only in a separate numbered item; preserve credentials outside repository/CI. |
| Runtime integrity mechanism rejected | Detached/public-key/other design required | `ACT-2_INTERNAL_IMPLEMENTATION` or `ACT-3_CONFIGURATION_OR_OE_VARIANT` | New design review; no automatic code change. |
| Self-test/conditional-test classification | Terminology/order/trigger change | `ACT-1_DOCUMENT_REVISION` or `ACT-2_INTERNAL_IMPLEMENTATION` | Update FSM, test plan, and recovery matrix. |
| External entropy provider accepted | Current responsibility model accepted | `ACT-0_RECORD_ONLY`, `ACT-1_DOCUMENT_REVISION`, `ACT-6_EXTERNAL_EVIDENCE` | Capture exact OE/provider records and accepted evidence. |
| Direct entropy source required | Initial model rejected | `ACT-5_ALGORITHM_SCOPE_GATE` or separate entropy-source gate plus `ACT-6_EXTERNAL_EVIDENCE` | Do not promote generic health-test scaffolding. |
| CSP/zeroization classification | Additional CSPs/evidence/OS controls required | `ACT-1_DOCUMENT_REVISION`, `ACT-2_INTERNAL_IMPLEMENTATION`, or `ACT-6_EXTERNAL_EVIDENCE` | Reconcile inventory, code paths, operator controls, and exact compiler evidence. |
| Side-channel scope | Empirical timing/cache or specialist assessment required | `ACT-6_EXTERNAL_EVIDENCE`; possibly `ACT-2_INTERNAL_IMPLEMENTATION` | Define target, method, samples, acceptance rule, reviewer, and protected artifacts. |
| Official templates and traceability | Current skeleton format accepted/changed | `ACT-1_DOCUMENT_REVISION` | Reissue controlled documents with exact form/revision. |
| Submission artifacts and channel | Source/binary/disassembly/evidence required | `ACT-6_EXTERNAL_EVIDENCE` | Establish approved recipient, channel, redaction, hashes, and retention. |
| Change/retest policy | OS/compiler/bug-fix classes defined | `ACT-1_DOCUMENT_REVISION` and `ACT-3_CONFIGURATION_OR_OE_VARIANT` | Add configuration/change-impact procedure before maintenance claims. |
| Signing before testing | Signer/TSA or package identity required | `ACT-6_EXTERNAL_EVIDENCE` | Provision only after explicit authorization and role separation. |
| PoC distribution | Allowed with conditions or prohibited | `ACT-1_DOCUMENT_REVISION` and separate legal/product gate | Apply notices/license/config restrictions; no automatic distribution. |

## Impact Evaluation Fields

For each accepted answer, record:

- affected requirement IDs and decision IDs;
- candidate/boundary/OE impact;
- public API/ABI and algorithm-scope impact;
- source, test, evidence, document, packaging, operator, legal, and schedule impact;
- whether exact OE-01, hosted, institution, specialist, signing, or external evidence is required;
- owner, reviewer, work-item number, completion criteria, and rollback/defer condition.

## Stop Conditions

Stop implementation and escalate when:

- an answer is ambiguous, verbal-only, contradictory, or outside the respondent's authority;
- an answer may change the public ABI, algorithm scope, module boundary, security level, entropy-source ownership, integrity mechanism, or external claims;
- protected source/evidence/credential transfer is requested without an approved channel;
- an answer would relabel local or synthetic results as hosted or institution evidence;
- signing, tag, timestamp, external submission, release, or customer distribution is implied without explicit authorization.
