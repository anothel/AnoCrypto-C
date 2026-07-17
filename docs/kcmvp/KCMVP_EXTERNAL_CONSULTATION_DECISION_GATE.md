# KCMVP External Consultation Decision Gate

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-AR-004` |
| Status | `CONTROLLED_INTERNAL_DECISION_GATE` |
| Policy ID | `LAR-P2-10-R1` |
| Current decision | `CONDITIONAL_NO_GO_PENDING_NAMED_AUTHORIZATION` |
| External consultation | `NOT_CONDUCTED` |

## Decision

The P2-10 assisted rehearsal supports a controlled next step, but does not itself authorize institution contact.

### GO for the following internal actions

- name an independent human reviewer;
- conduct a live internal rehearsal using the frozen pack;
- identify an institution/contact role and protected communication channel;
- verify current public guidance and pack revision immediately before contact;
- prepare response-capture and decision-owner records.

### NO-GO for the following actions

- send source, binary, evidence originals or confidential implementation details through an unapproved channel;
- state or imply formal validation, institution acceptance, security level, hosted success, production signing or release authorization;
- implement runtime integrity or alter public ABI based on an unretained verbal suggestion;
- push, tag, sign, timestamp, submit or publish without separate authorization.

## Preconditions for institution contact

All six must be retained:

1. `AUTH-01`: named project owner authorizes a named institution/contact purpose;
2. `AUTH-02`: named internal human reviewer completes one live rehearsal and records material objections;
3. `AUTH-03`: recipient role, meeting date, confidentiality/recording rule and secure channel are recorded;
4. `AUTH-04`: exact closeout/rehearsal pack SHA-256 and document revision are frozen;
5. `AUTH-05`: latest public process/form references are checked and any drift is recorded;
6. `AUTH-06`: response-capture owner and post-consultation decision owner are assigned.

Satisfying these preconditions permits contact only. It does not authorize source delivery, formal submission, implementation, signing or release.

## Decision after assisted rehearsal

| Action | Decision |
|---|---|
| Assisted desk rehearsal | `COMPLETE_LOCAL_ASSISTED` |
| Named human internal rehearsal | `GO_WHEN_NAMED` |
| Institution contact | `CONDITIONAL_NO_GO_PENDING_NAMED_AUTHORIZATION` |
| Formal submission | `NO_GO` |
| Runtime integrity implementation | `NO_GO_PENDING_RETAINED_ANSWER_OR_EXPLICIT_GATE` |
| Exact OE-01 execution | `NO_GO_UNTIL_ENVIRONMENT_AND_OWNER_APPROVED` |
| P1-34 hosted rerun | `NO_GO_UNTIL_BILLING_RESOLVED_AND_EXPLICITLY_AUTHORIZED` |
| External release | `NO_GO` |

## P2-11 handoff clarification

The `HRH-P2-11-R1` package prepares the records required by `AUTH-02` but does not satisfy it. Reviewer identity is `UNASSIGNED`, human rehearsal is `NOT_RUN`, project-owner authorization is `NOT_RECORDED`, and institution-contact authorization remains `NO`.
