# SHA-384 / SHA-512 Family Review Template

Status: P1-22 engineering template.

Record:

- source revision and build identity;
- compiler, target, and build options;
- startup self-test result;
- `ano_crypto_sha512_family_kat_tests` result;
- `ano_crypto_sha512_family_api_negative_tests` result;
- HMAC key-handle KAT result;
- install and PoC smoke result;
- `SHA512_FAMILY_CHECK.txt` result;
- clean Release 61-symbol baseline result;
- hosted platform limitations and outstanding differential-provider review.

Do not include digest values, MAC values, HMAC key bytes, entropy, seed material, DRBG state, private keys, tokens, or customer data.
