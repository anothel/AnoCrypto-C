# Block Cipher and Mode Selection Decision Record

Status: P1-27 AES/CBC remains implemented and hardened; AES-CMAC is selected as the next narrow candidate through a no-implementation gate.

## Implemented decision

- AES-128/192/256 primitive;
- CBC without padding;
- 16-byte blocks and IVs;
- complete non-empty blocks only;
- CSP key handles and opaque caller-owned contexts;
- exact in-place allowed, partial overlap prohibited.

The implemented selection is an engineering slice. It is not an official
validation scope or a claim that later customer requirements cannot reopen the
algorithm/mode decision.

## P1-27 next-candidate decision

AES-CMAC is selected before AES-CTR for the next entry gate. The proposed
future CMAC slice uses AES-128/192/256, full 16-byte tags, streaming compute,
one-shot compute, one-shot verification, and a dedicated CMAC-purpose CSP
handle.

P1-27 adds planning, policy, evidence, package, and clean-room records only. It
does not add a CMAC header, implementation, key-type enum value, function, or
exported symbol.

## Deferred decisions

AES-GCM, AES-CTR, GMAC, padding helpers, tag truncation, hardware acceleration,
and domestic algorithms remain outside P1-27. AES-CTR requires a separate
counter construction, uniqueness, exhaustion, clone/snapshot, and recovery
policy gate. Higher-level envelope/nonce/storage policy remains an AnoSecureKit
responsibility.
