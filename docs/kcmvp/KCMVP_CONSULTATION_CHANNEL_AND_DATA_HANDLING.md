# KCMVP Consultation Channel and Data Handling

## Status

| Field | Value |
|---|---|
| Policy ID | `CCR-P2-12-R1` |
| Secure channel | `UNCONFIRMED` |
| Disclosure owner | `UNASSIGNED` |
| First-contact attachments | `NONE` |
| Non-public transfer authorization | `NO` |

## Data classes

| Class | Examples | First contact |
|---|---|---|
| C0 Public status | product name, candidate boundary summary, public algorithm names, public blockers | Allowed after owner approval |
| C1 Controlled documents | consultation questions, claims summary, status-only pack | Not attached by default; separate approval required |
| C2 Restricted engineering | source, binaries, detailed evidence, compiler output, test archives, internal issue detail | Prohibited until secure channel and disclosure approval exist |
| C3 Secret/sensitive | credentials, signing keys, private key material, tokens, raw entropy/seed/DRBG state/random output, customer data | Never transmitted through this consultation workflow |

## Channel rules

- Public email addresses are discovery/contact channels, not automatically approved transfer channels.
- Verify the recipient domain and role using an independently obtained official source before sending.
- Do not follow an unsolicited link or upload request without verification.
- Record the exact sent text and SHA-256 of any approved attachment.
- Encrypt controlled or restricted material only using institution-provided, independently verified instructions and keys.
- Keep production signer, TSA, source repository credentials, and customer information outside the consultation package.
- A request for an NDA, contract, account registration, source upload, or entropy sample triggers a new legal/security/owner review.

## Incident rule

If restricted or secret material is sent unintentionally, do not reproduce it in a report. Record the affected artifact/path, recipient, channel, time, distribution status, containment action, credential rotation where applicable, and required follow-up review.
