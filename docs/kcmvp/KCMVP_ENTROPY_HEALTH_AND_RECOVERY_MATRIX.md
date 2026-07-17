# KCMVP Entropy Health and Recovery Matrix — P2-04

Status: `FROZEN_INTERNAL_POLICY`; not official entropy-source validation evidence.

| Profile/event | Health test in module | Failure classification | Output/context action | Module action | Operator action | Status |
|---|---|---|---|---|---|---|
| OE-01 `getrandom` normal acquisition | No module duplicate-block test | Provider call success | Consume requested bytes transiently; seed buffers later zeroized. | None | None | `READY_FOR_INTERNAL_REVIEW` |
| OE-01 provider unavailable | None | Unsupported provider/process capability | Random context-size/init fails closed; destination is cleared where supplied. | Module remains usable for non-random services. | Correct environment/configuration. | `READY_FOR_INTERNAL_REVIEW` |
| OE-01 provider call error during init | None | Operation-local entropy failure | Provider output and destination context cleared. | Current implementation remains operational. | Retry only after provider condition is corrected. | `EXTERNAL_CONFIRMATION_REQUIRED` for final fatality rule |
| OE-01 provider call error during reseed before a gate | None | Operation-local entropy failure | Temporary entropy cleared; old DRBG state retained. | Current implementation remains operational. | Retry reseed; old state may generate only if other gates permit. | `PARTIAL` |
| OE-01 provider call error while interval/process gate active | None | Entropy failure plus existing reseed gate | Temporary entropy cleared; old gate remains. | Module remains operational. | Successful reseed or context replacement required. | `READY_FOR_INTERNAL_REVIEW` |
| Reseed counter exceeds `2^48` requests | DRBG limit check | Reseed required | Requested output cleared by public random wrapper. | No module-fatal transition. | Reseed successfully before retry. | `READY_FOR_INTERNAL_REVIEW` |
| Process marker changed after fork | Process guard | Reseed required | Requested output cleared. | No module-fatal transition. | Reseed in child, then retry. | `READY_FOR_INTERNAL_REVIEW` |
| Known VM/container memory duplication | No automatic test | Operational assumption violation | No automatic detection or clearing. | No automatic transition. | Discard all inherited contexts and independently reinitialize. | `NOT_IMPLEMENTED` detection |
| Suspend/resume treated as freshness boundary | No automatic test | Operational policy event | No automatic detection. | No automatic transition. | Discard/reinitialize unless a reviewed profile permits reseed-only. | `NOT_IMPLEMENTED` detection |
| Explicit raw-noise test callback duplicate-block failure, operation-only policy | Engineering skeleton | Entropy-source failure | Sample cleared. | No fatal transition. | Test-specific reset. | `TEST_ONLY` |
| Explicit raw-noise test callback duplicate-block failure, module-fatal policy | Engineering skeleton | Fatal entropy-source failure | Sample and affected context cleared. | Sticky `ERROR_STATE`. | Terminate process and fresh load. | `TEST_ONLY`; not selected profile |

## Interpretation rules

1. The generic duplicate-block candidates are not an RCT/APT design and are not attached
   to conditioned OS-provider output.
2. Provider health evidence cannot be inferred from successful local calls alone.
3. Current operation-local handling of Linux provider errors is an implementation fact,
   not a final institution-approved failure classification.
4. No test or evidence file may retain entropy, nonce, DRBG state, or generated output.
