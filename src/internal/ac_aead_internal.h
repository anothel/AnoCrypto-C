#ifndef ANO_CRYPTO_AC_AEAD_INTERNAL_H
#define ANO_CRYPTO_AC_AEAD_INTERNAL_H

#include <stddef.h>
#include <stdint.h>
#include "ac_aes_internal.h"

#define ANO_CRYPTO_INTERNAL_GCM_STREAM_MAGIC UINT32_C(0x4147434d)
#define ANO_CRYPTO_INTERNAL_GCM_STREAM_STATE_VERSION UINT32_C(1)
#define ANO_CRYPTO_INTERNAL_GCM_STREAM_PHASE_AAD UINT32_C(1)
#define ANO_CRYPTO_INTERNAL_GCM_STREAM_PHASE_PAYLOAD UINT32_C(2)

typedef struct ano_crypto_internal_aes256_gcm_stream_ctx_s {
    uint32_t magic;
    uint32_t initialized;
    uint32_t state_version;
    uint32_t direction;
    uint32_t phase;
    uint32_t reserved;
    uint64_t aad_len;
    uint64_t text_len;
    size_t pending_len;
    size_t stream_offset;
    ano_crypto_internal_aes_ctx_t aes;
    uint8_t h[16];
    uint8_t j0[16];
    uint8_t counter[16];
    uint8_t stream[16];
    uint8_t ghash_state[16];
    uint8_t pending[16];
} ano_crypto_internal_aes256_gcm_stream_ctx_t;

#endif /* ANO_CRYPTO_AC_AEAD_INTERNAL_H */
