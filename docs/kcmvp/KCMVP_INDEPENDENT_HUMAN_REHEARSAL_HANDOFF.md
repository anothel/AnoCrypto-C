# KCMVP Independent Human Rehearsal Handoff

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-HRH-002` |
| Status | `READY_FOR_NAMED_HUMAN_EXECUTION` |
| Policy ID | `HRH-P2-11-R1` |
| Named reviewer | `UNASSIGNED` |
| Reviewer independence confirmation | `NOT_RECORDED` |
| Live rehearsal | `NOT_RUN` |
| Project-owner authorization record | `NOT_RECORDED` |
| Institution contact authorization | `NO` |

## Handoff decision

The P2-10 assisted desk rehearsal is sufficient to prepare a controlled human-review package. It is not sufficient to satisfy the independent-human-rehearsal precondition.

A named reviewer must conduct the session. An automated assistant, document author, generated transcript, or policy check cannot be represented as the independent reviewer.

## Reviewer eligibility

The project owner must record a reviewer who:

- is a real person with a retained name and role;
- did not solely author the answers being challenged;
- can identify conflicts of interest or limits of expertise;
- agrees to record material objections and unresolved questions;
- understands that the exercise is internal engineering preparation, not an institution decision.

The required degree of organizational independence for any later formal review remains `EXTERNAL_CONFIRMATION_REQUIRED`.

## Frozen inputs

Before the session, record:

- exact handoff-pack filename and SHA-256;
- repository engineering version and source commit or immutable snapshot identifier;
- reviewer name, role, organization/team, and conflict statement;
- project owner and minutes owner;
- session date, location/channel, confidentiality rule, and recording rule;
- any materials added or removed after the package hash was frozen.

Any pack change invalidates the frozen hash and requires a new handoff record.

## Mandatory session outputs

The reviewer must return all of the following:

1. completed live-rehearsal minutes;
2. agenda-by-agenda disposition for all ten Priority-1 topics;
3. objection and defect register with owners and exit evidence;
4. explicit recommendation: `REPEAT_INTERNAL_REHEARSAL`, `READY_FOR_OWNER_CONTACT_DECISION`, or `STOP_AND_REMEDIATE`.

The project owner must then record a separate consultation authorization decision. Reviewer recommendation alone does not authorize institution contact.

## Stop conditions

Stop the session and record the reason if:

- the frozen pack hash cannot be verified;
- reviewer identity or conflict statement is missing;
- source, binary, credential, customer data, or sensitive evidence is unexpectedly included;
- an answer implies institution acceptance, formal validation, hosted success, production signing, or release authorization;
- a material question cannot be answered from retained evidence and is instead guessed;
- the reviewer is asked to approve implementation, signing, submission, or release outside the stated scope.

## Current disposition

The handoff is `READY_FOR_NAMED_HUMAN_EXECUTION`. The live rehearsal itself remains `NOT_RUN`, and institution contact remains unauthorized.
