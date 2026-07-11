#ifndef ANO_CRYPTO_CIPHER_INTERNAL_H
#define ANO_CRYPTO_CIPHER_INTERNAL_H

#include <stdint.h>
#include <anocrypto/ac_cipher.h>
#include "ac_aes_internal.h"

#define ANO_CRYPTO_INTERNAL_CIPHER_CTX_MAGIC 0x41434331u

typedef struct ano_crypto_internal_cipher_ctx_s {
    uint32_t magic;
    uint32_t algorithm;
    uint32_t mode;
    uint32_t direction;
    uint32_t initialized;
    uint32_t reserved;
    uint8_t chain[ANO_CRYPTO_INTERNAL_AES_BLOCK_SIZE];
    ano_crypto_internal_aes_ctx_t aes;
} ano_crypto_internal_cipher_ctx_t;

#endif /* ANO_CRYPTO_CIPHER_INTERNAL_H */
