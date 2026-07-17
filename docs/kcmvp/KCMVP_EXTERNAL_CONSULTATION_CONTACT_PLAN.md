# KCMVP External Consultation Contact Plan

## Control status

| Field | Value |
|---|---|
| Policy ID | `CCR-P2-12-R1` |
| Readiness | `READY_FOR_OWNER_SELECTION_AFTER_HUMAN_REHEARSAL` |
| Independent human rehearsal | `NOT_RUN` |
| Project-owner authorization | `NOT_RECORDED` |
| Institution selection | `UNSELECTED` |
| Contact channel | `UNSELECTED` |
| First contact | `NOT_SENT` |
| External consultation | `NOT_CONDUCTED` |

## Purpose

Prepare a controlled first-contact path that asks whether and how a test institution will conduct pre-consultation. This is not a test application, source submission, formal validation request, or release authorization.

## Entry conditions

All conditions are mandatory:

1. a named independent reviewer completes the P2-11 live rehearsal;
2. minutes, objections, recommendation, and owner receipt are retained;
3. the project owner selects one institution and one authorized sender;
4. current public contact data and procedure sources are rechecked;
5. the exact outbound message and attachment list receive owner approval;
6. an approved secure follow-up channel is confirmed before non-public material is transmitted.

## First-contact payload

Allowed in the first message:

- product name and high-level C99 software-module positioning;
- candidate shared-library boundary and target operating-environment summary;
- high-level algorithm/service inventory;
- explicit statements that formal validation has not started and runtime integrity is not implemented;
- the ten prioritized consultation topics;
- a request for the institution's preferred process, secure channel, expected preliminary material, schedule, and commercial prerequisites.

Not allowed in the first message:

- source code or binary;
- build/evidence archives;
- credentials, private keys, signing artifacts, or tokens;
- raw cryptographic values, entropy, seed, DRBG state, random output, customer data, or internal deployment detail;
- claims that formal validation, institution acceptance, external-assessment completion, or production deployment approval has occurred.

## Decision outcomes

| Outcome | Meaning | Next action |
|---|---|---|
| `NO_GO` | Entry conditions are incomplete. | Keep contact authorization `NO`. |
| `CONTACT_APPROVED` | Owner authorizes a named sender to transmit the approved first-contact text only. | Record recipient, channel, time, exact content hash, and delivery result. |
| `SECURE_FOLLOWUP_APPROVED` | Institution provides an accepted secure channel and requested-material list. | Perform a separate disclosure review before sending any attachment. |
| `FORMAL_APPLICATION_CONSIDERATION` | Institution explains formal entry requirements. | Create a new authorization gate; do not infer approval from this plan. |

## Stop conditions

Stop and obtain a new decision if the institution requests source, binaries, entropy samples, credentials, personal data, an NDA, a contract, a security-level commitment, or implementation changes.
