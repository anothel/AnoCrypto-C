# KCMVP Consultation Authorization Record

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-HRH-006` |
| Status | `NOT_RECORDED` |
| Policy ID | `HRH-P2-11-R1` |
| Independent human rehearsal | `NOT_RUN` |
| Institution contact authorization | `NO` |

## Preconditions

| Gate | Required record | Current state |
|---|---|---|
| `AUTH-01` | Named project owner and contact purpose. | `NOT_RECORDED` |
| `AUTH-02` | Named human reviewer, completed minutes, and material objections. | `NOT_RUN` |
| `AUTH-03` | Recipient role, date, confidentiality rule, and protected channel. | `NOT_RECORDED` |
| `AUTH-04` | Exact pack revision and SHA-256 frozen. | `NOT_RECORDED` |
| `AUTH-05` | Current public process/form drift check. | `NOT_RUN` |
| `AUTH-06` | Response-capture owner and post-consultation decision owner. | `UNASSIGNED` |

## Project-owner decision

Select one only after all records are reviewed:

- `AUTHORIZE_CONTROLLED_INSTITUTION_CONTACT`
- `REPEAT_INTERNAL_REHEARSAL`
- `REMEDIATE_BEFORE_CONTACT`
- `DO_NOT_CONTACT`

Current decision: `NOT_RECORDED`

## Scope of any future authorization

Even `AUTHORIZE_CONTROLLED_INSTITUTION_CONTACT` permits only the named consultation using the frozen status-only pack and approved channel. It does not authorize source or binary transfer, formal submission, implementation changes, public ABI changes, hosted rerun, signing, timestamping, tagging, publishing, production use, or external release.
