# Block Cipher and Mode Selection Decision Record

Status: P1-26 narrow AES/CBC slice implemented and hardened without widening its public scope.

## Implemented decision

- AES-128/192/256 primitive;
- CBC without padding;
- 16-byte blocks and IVs;
- complete non-empty blocks only;
- CSP key handles and opaque caller-owned contexts;
- exact in-place allowed, partial overlap prohibited.

The selection is an engineering implementation slice. It is not an official
validation scope or a claim that later customer requirements cannot reopen the
algorithm/mode decision.

## Deferred decisions

AES-GCM, AES-CTR, CMAC/GMAC, padding helpers, hardware acceleration, and domestic
algorithms remain outside P1-26. Higher-level envelope/nonce/storage policy remains
an AnoSecureKit responsibility.
