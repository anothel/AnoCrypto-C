# P3-00 Implementation and Test Delta

Status: `DOCUMENT_AND_POLICY_ONLY`

## Added

- downstream intake and ownership rules;
- product operation/capability matrix template;
- capability gap approval register;
- exact source collection requests;
- current/active/next roadmap;
- product-driven P3 roadmap;
- API/ABI evolution gate;
- deferred KCMVP positioning;
- interim GPT Sources replacement plan;
- status-only downstream intake request package and policy checks.

## Not changed

- cryptographic implementations;
- public headers and declarations;
- exported symbols and symbol version line;
- module lifecycle, self-test, CSP, entropy or zeroization behavior;
- actual downstream adapters;
- hosted, signing, validation or release state.

## Verification intent

P3-00 must preserve all existing tests and add policy/package checks that fail
if an algorithm is marked approved before exact downstream sources are imported.
