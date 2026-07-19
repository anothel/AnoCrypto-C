# AnoSecureKit SKP1 v1 scrypt Compatibility Evidence

Status: `PASS_PRIMITIVE_ONLY`

Exact downstream evidence uses the retained `skp1-known-password-file.hex` vector:

- password bytes: `known SKP1 password`;
- salt: header bytes 12..43, `000102...1f`;
- parameters: `N=32768`, `r=8`, `p=1`, max memory 64 MiB;
- expected 32-byte key: `b3fbb7c9f7c9a44138ec4385aecd459dede321f597eba92e7e0b8138cf4ad3a0`;
- the derived key authenticates and decrypts the exact first record to `known SKP1 vector` using the existing generic AES-256-GCM API.

The test copies only fixed primitive evidence. AnoCrypto-C does not parse, serialize, create, or orchestrate SKP1. Empty-password rejection remains AnoSecureKit product policy rather than a primitive restriction. The downstream Community dependency identity conflict remains `OPEN_DOWNSTREAM_SYNC_DECISION`.
