# Test Document Set Skeleton

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-TS-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_INTERNAL_SKELETON` |
| Owner role | Validation test lead |
| Reviewer | `TODO` |
| Approval | `NO` |

## 1. Test evidence classes

| Class | Meaning |
|---|---|
| `LOCAL_ENGINEERING_ONLY` | Developer-controlled local execution. |
| `SYNTHETIC_POLICY_REGRESSION` | Fixture or workflow-contract test; not actual platform/release evidence. |
| `ACTUAL_HOSTED` | Real hosted run with downloaded, verified artifacts and human disposition. |
| `INSTITUTION_TEST` | Testing-institution result; never pre-filled by the vendor. |
| `NOT_RUN` | No qualifying execution exists. |

## 2. Environment matrix

The target test environment is OE-01 from P2-02. Exact retained OE-01 execution is `NOT_RUN`. Current Linux local, sanitizer, differential and portability records remain engineering evidence only. Windows/macOS/32-bit/big-endian hosted lanes remain `NOT_RUN/UNREVIEWED`.

## 3. Test ID structure

- `AC-ALG-*`: algorithm KAT/conformance.
- `AC-API-*`: API argument/buffer/alias/state behavior.
- `AC-ST-*`: startup/on-demand/conditional/integrity tests.
- `AC-ENT-*`: entropy/provider/DRBG lifecycle.
- `AC-CSP-*`: key/CSP lifecycle and zeroization.
- `AC-SC-*`: secure-coding/compiler/side-channel review.
- `AC-OE-*`: operating environment, install, dependency and packaging.
- `AC-CM-*`: configuration/provenance/signing/document controls.

Existing test executables and trace IDs must be mapped without renaming evidence as institution tests.

## 4. Planned procedures

| Procedure area | Current status | Required completion |
|---|---|---|
| Startup algorithm KAT | Implemented engineering tests | exact OE-01 retained run and approved procedure wording |
| Runtime module integrity | `NOT_IMPLEMENTED` | implementation plus mutation/truncation/path/permission tests |
| Algorithm conformance/differential | Local engineering evidence | institution-approved vectors/procedure and separated results |
| Negative/misuse/state | Implemented engineering tests | exact expected-status table and selected-OE run |
| Entropy/DRBG | Internal policy/tests | provider/health ownership acceptance and selected-OE evidence |
| CSP/zeroization | Local lifecycle/compiler evidence | formal classification, exact OE review and approved method |
| Concurrency/process | Local/stress evidence | selected-OE scope and reproducible procedure |
| Side-channel/fault | Partial/`NOT_RUN`/`NOT_EVALUATED` | institution-approved depth and specialist results if required |
| Install/public boundary | Implemented local smoke | exact candidate package and binary hash |
| Hosted matrix | `BLOCKED_EXTERNAL` | billing resolution, actual runs, artifact review |

## 5. Procedure template

Each procedure records:

- test ID, requirement ID and design reference;
- code/build configuration and exact artifact hash;
- environment ID and immutable environment evidence;
- prerequisites and non-sensitive inputs;
- ordered steps and expected status/length/state;
- cleanup/zeroization verification;
- result: PASS/FAIL/NOT_RUN/BLOCKED;
- log/evidence hash;
- tester, reviewer, UTC date and limitations.

## 6. Result template

```text
Test ID:
Requirement ID:
Environment ID:
Source commit:
Artifact SHA-256:
Execution class: LOCAL_ENGINEERING_ONLY | ACTUAL_HOSTED | INSTITUTION_TEST
Result: PASS | FAIL | NOT_RUN | BLOCKED
Observed status/state/length:
Sensitive values printed: NO
Evidence path/hash:
Tester/date:
Reviewer/date/disposition:
Limitations/open finding:
```

## 7. Sensitive-result rule

Do not record keys, entropy, nonce/seed, DRBG state/output, plaintext, ciphertext, IV, digest, MAC/CMAC values, integrity keys/tags, private signing data or customer information. Public test-vector provenance may be referenced without duplicating restricted material unnecessarily.

## 8. Institution result separation

Institution test result tables remain blank until authentic institution-issued results are received through an approved channel. Local, synthetic or hosted vendor evidence must never be inserted into those fields.
