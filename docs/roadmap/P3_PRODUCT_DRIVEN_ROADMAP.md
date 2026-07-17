# P3 Product-Driven Roadmap

Status: `P3_00_INTAKE_ONLY`

## Principle

P3 work is selected from downstream product requirements, not from an algorithm
wish list and not from KCMVP document momentum.

## P3-00 — Requirement intake and roadmap reset

Exit criteria:

- four exact source sets imported;
- exact Enterprise-to-Community dependency revisions recorded;
- operation/capability rows linked to code and tests;
- ownership decisions reviewed;
- no unverified row treated as implementation authority;
- CURRENT ONLY source pack regenerated;
- first implementation work item explicitly approved.

## P3-01 and later

Names and order are intentionally `UNASSIGNED`. Each item must be derived from
an accepted gap and must state:

- downstream authority revision;
- owning product operation;
- minimal AnoCrypto-C scope;
- public/private API impact;
- ABI strategy;
- CSP, self-test, zeroization and error semantics;
- KAT, negative and differential plan;
- install/PoC and downstream integration test;
- no-fallback evidence where applicable.

## Parallel maintenance lanes

P1-34 hosted review and KCMVP governance remain separate lanes. They do not
select product features and do not block local product requirement analysis.
