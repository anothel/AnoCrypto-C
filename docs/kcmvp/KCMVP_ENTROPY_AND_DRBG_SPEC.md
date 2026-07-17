# KCMVP Entropy and DRBG Specification — P2-04

Status: `FROZEN_INTERNAL_POLICY`; formal acceptance remains `EXTERNAL_CONFIRMATION_REQUIRED`.

## 1. Controlled identifiers

| Item | Value |
|---|---|
| Entropy/DRBG policy | `EDP-P2-04-R1` |
| Validation candidate | `AC-P2-02-LINUX-X86_64-GCC13-SHARED-R1` |
| Selected provider profile | `ENT-OE01-LINUX-GETRANDOM-R1` |
| DRBG mechanism | HMAC_DRBG-SHA-256 |
| Public ABI | 14 headers, 69 symbols, `ANOCRYPTO_C_0` |
| Formal institution acceptance | `EXTERNAL_CONFIRMATION_REQUIRED` |
| Actual selected-OE retained execution | `NOT_RUN` |
| P1-34 hosted evidence | `BLOCKED_EXTERNAL: GITHUB_ACTIONS_BILLING` |

This document freezes an internal engineering candidate. It does not claim that the
selected operating-system provider, entropy credit, health-test allocation, or random-bit
generator construction has been accepted by a KCMVP testing institution.

## 2. Initial candidate boundary

The initial candidate uses the Linux `getrandom()` interface with `flags=0` through the
internal platform adapter. The Linux kernel random subsystem and its underlying noise
sources, conditioning, initialization, and health behavior are outside the AnoCrypto-C
shared-library boundary.

AnoCrypto-C classifies this input as an **external conditioned random provider**, not as
raw noise samples owned by the module.

The initial candidate therefore contains no project-owned direct noise source.

```text
Application
  -> ano_crypto_random_* public service
     -> module-owned HMAC_DRBG-SHA-256 state
        -> internal entropy adapter
           -> Linux getrandom(flags=0)
              -> external Linux kernel random subsystem
```

The direct-noise callback classification and duplicate-block health-test implementation
remain engineering test scaffolding. They are disabled in candidate Release builds and
are not part of `ENT-OE01-LINUX-GETRANDOM-R1`.

## 3. Provider behavior used by the implementation

The Linux adapter:

- calls `getrandom(output + offset, remaining, 0)`;
- retries when interrupted with `EINTR`;
- treats zero-length progress or other provider errors as entropy-source failure;
- loops until the requested length is filled;
- clears the caller output when provider acquisition fails;
- requests no `GRND_RANDOM` or `GRND_NONBLOCK` behavior.

`flags=0` is intentionally selected so early-boot initialization behavior remains owned
by the Linux provider rather than being bypassed by a nonblocking fallback. Exact kernel,
image, package, and provider evidence still requires execution on OE-01.

## 4. Entropy acquisition and DRBG lifecycle

### 4.1 Source-backed public random service

| Event | Provider requests | DRBG action |
|---|---:|---|
| `ano_crypto_random_init()` | 32-byte entropy input, then a separate 16-byte nonce request | Instantiate HMAC_DRBG-SHA-256, set reseed counter to 1, record process marker. |
| `ano_crypto_random_reseed()` | 32-byte entropy input | Update state, reset reseed counter to 1, refresh process marker. |
| `ano_crypto_random_generate()` | No provider request unless the caller separately reseeds | Generate from module-owned state and increment the reseed counter. |
| `ano_crypto_random_uninit()` | None | Clear caller-owned opaque context storage. |

The byte lengths above are input lengths, not a formal claim that every byte carries a
full bit of entropy. Entropy credit and the acceptability of using two separate provider
calls for entropy input and nonce remain `EXTERNAL_CONFIRMATION_REQUIRED`.

### 4.2 Explicit-entropy DRBG API

`ano_crypto_drbg_*` remains a low-level caller-managed interface. The caller supplies
entropy input and nonce directly. AnoCrypto-C checks minimum lengths and state but does
not validate the origin, entropy estimate, independence, or lifecycle of caller-provided
input. It also does not automatically protect this path across process or VM duplication.

The explicit-entropy API must not be represented as equivalent to the source-backed
public random service without a separate integration assessment.

## 5. Frozen limits and unsupported requests

| Property | Frozen implementation value | Disposition |
|---|---:|---|
| HMAC_DRBG entropy-input minimum | 32 bytes | Implemented; entropy credit pending external confirmation. |
| HMAC_DRBG nonce minimum | 16 bytes | Implemented; nonce construction acceptance pending external confirmation. |
| Entropy adapter request ceiling | 65,536 bytes | Internal fail-closed ceiling. |
| Generate output maximum per request | 65,536 bytes | Implemented in DRBG and public random APIs. |
| Reseed interval | `2^48` generate requests | Last allowed request succeeds; the next returns `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED`. |
| Prediction resistance | Unsupported | Requests return `ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER`. |
| Automatic time/byte-based reseed | Not implemented | No wall-clock or cumulative-byte trigger. |
| Personalization/additional-input project cap | No separate cap beyond valid `size_t` input and accessible memory | `PARTIAL`; institution/profile limit decision required. |

The `2^48` interval is an implementation maximum, not a recommendation that a consuming
product should wait that long. A stricter product or institution-approved operational
reseed interval may be required.

## 6. Health-test allocation

### 6.1 Selected OS-provider profile

For `ENT-OE01-LINUX-GETRANDOM-R1`:

- raw kernel noise samples are not visible to AnoCrypto-C;
- provider-internal startup and continuous health behavior is assigned to the external
  operating environment;
- the module does not apply its generic duplicate-block candidates to conditioned
  `getrandom()` output;
- the module records provider identity, source classification, status, and lengths only;
- provider acceptance and required supporting evidence remain
  `EXTERNAL_CONFIRMATION_REQUIRED`.

### 6.2 Direct-noise-source profile

No direct noise source is in the initial candidate. If one is introduced later, it requires
a separate scope gate, source model, entropy assessment, raw-sample access, startup and
continuous health tests, thresholds, false-positive analysis, restart behavior, failure
state mapping, and institution review. Existing duplicate-block code cannot be promoted
as that design without this work.

## 7. Failure and recovery rules

| Condition | Current result | State/recovery rule |
|---|---|---|
| Provider unavailable before init | `ANO_CRYPTO_ERR_NOT_IMPLEMENTED` | Context is cleared; service unavailable. |
| Provider error during random init | `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED` | Destination context is cleared; module is not automatically made fatal. Retry may be attempted after the provider condition is corrected. |
| Provider error during manual reseed | `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED` | Existing DRBG state is retained. Generation remains allowed only when no reseed-interval or process-change gate is active. |
| Reseed failure after interval gate | Entropy error on reseed; later generate remains `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED` | A successful reseed is required. |
| Detected process-marker change | `ANO_CRYPTO_ERR_DRBG_RESEED_REQUIRED` and output clearing | Reseed in the new process before retrying. |
| DRBG internal update failure | Operation error and DRBG instance `ERROR` | Uninstantiate and instantiate a new context. |
| Future direct-source fatal health failure | `ANO_CRYPTO_ERR_ENTROPY_SOURCE_FAILED`; module may enter sticky `ERROR_STATE` | Not part of the selected OS-provider profile; fresh process required if configured as module-fatal. |

Whether an external OS-provider failure must become module-fatal for the validation target
is an explicit pre-consultation question. Current code treats it as an operation failure.

## 8. Process, VM, container, and suspend policy

- Source-backed contexts record a process marker.
- Ordinary POSIX fork changes the marker. Child generation is rejected until successful
  reseed; the parent state is unaffected by that child-side check.
- Process-marker support is a service prerequisite. Unsupported branches fail closed.
- VM snapshot/restore, VM clone, container checkpoint/restore, and duplicated process
  memory are not automatically detected.
- When duplicated or restored state is known or possible, every inherited context must be
  discarded and independently reinitialized. Reseed-only recovery is not sufficient when
  another usable copy of the old state may exist.
- Suspend/resume is not automatically detected. A deployment that treats resume as a
  freshness boundary must discard and reinitialize contexts, unless a separately reviewed
  platform policy explicitly permits reseed-only handling.
- Migration, checkpoint, and suspend behavior remain integration assumptions and
  institution-confirmation items rather than validated automatic module behavior.

## 9. Evidence and non-disclosure

Permitted evidence:

- candidate, provider, kernel, image, compiler, and build identifiers;
- source classification and health-test owner;
- request lengths and documented limits;
- status names, operation counts, and process-marker capability;
- source/binary/artifact hashes and reviewer disposition.

Prohibited evidence:

- raw provider output;
- entropy or nonce seed material;
- HMAC_DRBG Key/V state;
- generated random bytes;
- personalization/additional input containing customer data;
- test-hook deterministic sequences outside transient test memory.

## 10. Closure disposition

P2-04 closes the internal responsibility and evidence model, not formal acceptance.
The following remain open:

1. institution acceptance of Linux `getrandom(flags=0)` as the selected external provider;
2. required operating-environment evidence for provider health and initialization;
3. entropy credit and separate entropy/nonce call interpretation;
4. whether provider failure is recoverable or module-fatal;
5. whether a stricter operational reseed interval is required;
6. personalization and additional-input length profile;
7. exact OE-01 retained execution and human review;
8. P1-34 actual hosted artifact collection.
