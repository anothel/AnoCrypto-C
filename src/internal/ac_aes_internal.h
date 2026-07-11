#ifndef ANO_CRYPTO_AES_INTERNAL_H
#define ANO_CRYPTO_AES_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>

#define ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE 16u
#define ANO_CRYPTO_INTERNAL_AES_MAX_ROUND_KEY_SIZE 240u
#define ANO_CRYPTO_INTERNAL_AES128_ROUND_KEY_SIZE 176u
#define ANO_CRYPTO_INTERNAL_AES192_ROUND_KEY_SIZE 208u
#define ANO_CRYPTO_INTERNAL_AES256_ROUND_KEY_SIZE 240u
#define ANO_CRYPTO_INTERNAL_AES_CTX_MAGIC 0x41455331u

typedef struct ano_crypto_internal_aes_ctx_s {
    uint8_t round_keys[ANO_CRYPTO_INTERNAL_AES_MAX_ROUND_KEY_SIZE];
    uint32_t rounds;
    uint32_t round_key_size;
    uint32_t magic;
    uint32_t initialized;
} ano_crypto_internal_aes_ctx_t;

ano_crypto_status_t ano_crypto_internal_aes_init(
    ano_crypto_internal_aes_ctx_t *ctx,
    const uint8_t *key,
    size_t key_len);

ano_crypto_status_t ano_crypto_internal_aes_encrypt_block(
    const ano_crypto_internal_aes_ctx_t *ctx,
    const uint8_t input[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE],
    uint8_t output[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE]);

ano_crypto_status_t ano_crypto_internal_aes_decrypt_block(
    const ano_crypto_internal_aes_ctx_t *ctx,
    const uint8_t input[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE],
    uint8_t output[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE]);

void ano_crypto_internal_aes_uninit(ano_crypto_internal_aes_ctx_t *ctx);

#endif /* ANO_CRYPTO_AES_INTERNAL_H */
