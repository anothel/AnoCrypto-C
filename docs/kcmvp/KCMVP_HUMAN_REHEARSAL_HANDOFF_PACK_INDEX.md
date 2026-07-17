# KCMVP Independent Human Rehearsal Handoff Pack Index

| Control field | Value |
|---|---|
| Document ID | `AC-KCMVP-HRH-001` |
| Revision | `0.1` |
| Status | `CONTROLLED_HANDOFF_READY` |
| Policy ID | `HRH-P2-11-R1` |
| Engineering version | `0.1.0-p2-11-human-rehearsal-handoff` |
| Package class | `STATUS_ONLY_HUMAN_REHEARSAL_HANDOFF` |
| Named reviewer | `UNASSIGNED` |
| Independent human live rehearsal | `NOT_RUN` |
| Institution contact authorization | `NO` |
| External consultation | `NOT_CONDUCTED` |

## Purpose

This index defines the controlled handoff from the P2-10 assisted desk rehearsal to a future named human internal rehearsal. The package provides the material needed for a reviewer to challenge the current project position, record objections, and return a decision record to the project owner.

It does not record a completed human review, institution answer, formal test, submission, validation decision, signing event, release authorization, source transfer, or binary delivery.

## Controlled documents

The package allowlist is defined by `release/ANOCRYPTO_C_HUMAN_REHEARSAL_HANDOFF_MANIFEST.txt`. It contains:

- this index and the P2-11 handoff controls;
- the P2-10 assisted report, transcript, scorecard, gap register, and contact gate;
- the Priority-1 questions and evidence walkthrough;
- current claims and hosted-blocker status;
- the retained next-work gate.

The package does not contain source code, binaries, build output, evidence originals, keys, credentials, signatures, timestamps, customer data, or institution material.

## Completion boundary

P2-11 is complete when the handoff package, policy, package smoke, traceability, and local regression evidence are complete. The human rehearsal remains `NOT_RUN` until a named reviewer actually performs it and the project owner retains the resulting records.
