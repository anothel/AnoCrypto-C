# AnoCrypto-C Capability Gap Register

Status: `NO_PRODUCT_GAP_APPROVED`

## Current baseline

The retained P2 baseline contains the existing SHA-2, HMAC, HMAC_DRBG, random,
AES-CBC, AES-CMAC and CSP/lifecycle services described by the repository. The
public baseline remains 14 headers, 69 symbols and `ANOCRYPTO_C_0`.

## Candidate gap table

| Gap ID | Downstream requirement | Existing capability result | Ownership result | API/ABI impact | Test/evidence plan | Priority | State |
|---|---|---|---|---|---|---|---|
| `GAP-TBD` | `WAITING_FOR_CURRENT_DOWNSTREAM_SOURCES` | `UNASSESSED` | `UNDECIDED` | `UNASSESSED` | `NOT_WRITTEN` | `UNRANKED` | `NOT_APPROVED` |

## Approval gate

A gap becomes implementation work only when:

1. the requirement is accepted from an exact downstream revision;
2. ownership is assigned to AnoCrypto-C;
3. existing services are proven insufficient;
4. the minimal API and private implementation scope are reviewed;
5. ABI compatibility strategy is approved;
6. KAT, negative, differential, lifecycle, zeroization, install and downstream
   integration evidence is planned;
7. a numbered work item and single commit title are approved.
