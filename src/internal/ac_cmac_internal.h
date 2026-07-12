#ifndef ANO_CRYPTO_CMAC_INTERNAL_H
#define ANO_CRYPTO_CMAC_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include <anocrypto/ac_status.h>
#include "ac_aes_internal.h"

#define ANO_CRYPTO_INTERNAL_CMAC_CTX_MAGIC 0x434d4331u

typedef struct ano_crypto_internal_cmac_ctx_s {
    uint32_t magic;
    uint32_t initialized;
    size_t pending_len;
    uint8_t chain[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];
    uint8_t pending[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];
    uint8_t subkey1[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];
    uint8_t subkey2[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];
    ano_crypto_internal_aes_ctx_t aes;
} ano_crypto_internal_cmac_ctx_t;

ano_crypto_status_t ano_crypto_internal_cmac_init(
    ano_crypto_internal_cmac_ctx_t *ctx,
    const uint8_t *key,
    size_t key_len);

ano_crypto_status_t ano_crypto_internal_cmac_update(
    ano_crypto_internal_cmac_ctx_t *ctx,
    const uint8_t *input,
    size_t input_len);

ano_crypto_status_t ano_crypto_internal_cmac_final(
    ano_crypto_internal_cmac_ctx_t *ctx,
    uint8_t out[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE]);

ano_crypto_status_t ano_crypto_internal_cmac_compute(
    const uint8_t *key,
    size_t key_len,
    const uint8_t *input,
    size_t input_len,
    uint8_t out[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE]);

void ano_crypto_internal_cmac_uninit(ano_crypto_internal_cmac_ctx_t *ctx);

#endif /* ANO_CRYPTO_CMAC_INTERNAL_H */
