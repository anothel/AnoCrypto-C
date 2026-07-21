#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_key.h>
#include "ac_csp_internal.h"
#include "ac_internal.h"
#include "ac_thread_internal.h"
#include "ac_rsa_internal.h"
#include "ac_ecdsa_internal.h"

#define ANO_CRYPTO_CSP_HANDLE_INDEX_MASK 0xffffffffULL

typedef struct ano_crypto_csp_slot_s {
    uint32_t generation;
    uint32_t state;
    uint32_t key_type;
    uint32_t flags;
    size_t key_len;
    uint8_t key_material[ANO_CRYPTO_INTERNAL_CSP_MAX_SECRET_BYTES];
    ano_crypto_rsa_key_internal_t rsa_key;
    ano_crypto_ec_p256_key_internal_t ec_p256_key;
} ano_crypto_csp_slot_t;

static ano_crypto_csp_slot_t g_csp_slots[ANO_CRYPTO_INTERNAL_CSP_TABLE_CAPACITY];
static uint32_t g_csp_next_generation = 1u;

static ano_crypto_csp_handle_t make_handle(size_t index, uint32_t generation)
{
    return ((ano_crypto_csp_handle_t)generation << 32) |
           ((ano_crypto_csp_handle_t)(index + 1u) & ANO_CRYPTO_CSP_HANDLE_INDEX_MASK);
}

static int decode_handle(ano_crypto_csp_handle_t handle, size_t *out_index, uint32_t *out_generation)
{
    uint64_t raw_index;
    uint32_t generation;

    if (handle == ANO_CRYPTO_CSP_INVALID_HANDLE || out_index == 0 || out_generation == 0) {
        return 0;
    }

    raw_index = (uint64_t)(handle & ANO_CRYPTO_CSP_HANDLE_INDEX_MASK);
    generation = (uint32_t)(handle >> 32);
    if (raw_index == 0u || raw_index > (uint64_t)ANO_CRYPTO_INTERNAL_CSP_TABLE_CAPACITY || generation == 0u) {
        return 0;
    }

    *out_index = (size_t)(raw_index - 1u);
    *out_generation = generation;
    return 1;
}

static ano_crypto_csp_slot_t *slot_from_handle_unlocked(ano_crypto_csp_handle_t handle)
{
    size_t index = 0u;
    uint32_t generation = 0u;
    ano_crypto_csp_slot_t *slot;

    if (!decode_handle(handle, &index, &generation)) {
        return 0;
    }

    slot = &g_csp_slots[index];
    if (slot->generation != generation) {
        return 0;
    }

    return slot;
}

static const ano_crypto_csp_slot_t *const_slot_from_handle_unlocked(ano_crypto_csp_handle_t handle)
{
    size_t index = 0u;
    uint32_t generation = 0u;
    const ano_crypto_csp_slot_t *slot;

    if (!decode_handle(handle, &index, &generation)) {
        return 0;
    }

    slot = &g_csp_slots[index];
    if (slot->generation != generation) {
        return 0;
    }

    return slot;
}

static int slot_state_is_live(uint32_t state)
{
    return state == (uint32_t)ANO_CRYPTO_CSP_STATE_ALLOCATED ||
           state == (uint32_t)ANO_CRYPTO_CSP_STATE_INITIALIZED ||
           state == (uint32_t)ANO_CRYPTO_CSP_STATE_ACTIVE ||
           state == (uint32_t)ANO_CRYPTO_CSP_STATE_RESTRICTED;
}

static ano_crypto_status_t find_free_slot_unlocked(size_t *out_index)
{
    size_t i;

    if (out_index == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    for (i = 0u; i < ANO_CRYPTO_INTERNAL_CSP_TABLE_CAPACITY; ++i) {
        if (!slot_state_is_live(g_csp_slots[i].state)) {
            *out_index = i;
            return ANO_CRYPTO_OK;
        }
    }

    return ANO_CRYPTO_ERR_RESOURCE_EXHAUSTED;
}


void ano_crypto_internal_csp_zeroize_all_unlocked(void)
{
    size_t i;

    for (i = 0u; i < ANO_CRYPTO_INTERNAL_CSP_TABLE_CAPACITY; ++i) {
        ano_crypto_csp_slot_t *slot = &g_csp_slots[i];

        if (!slot_state_is_live(slot->state)) {
            continue;
        }

        ano_crypto_internal_zeroize(slot->key_material, sizeof(slot->key_material));
        ano_crypto_internal_zeroize(&slot->rsa_key, sizeof(slot->rsa_key));
        ano_crypto_internal_zeroize(&slot->ec_p256_key, sizeof(slot->ec_p256_key));
        slot->key_len = 0u;
        slot->key_type = 0u;
        slot->flags = 0u;
        slot->state = (uint32_t)ANO_CRYPTO_CSP_STATE_ZEROIZED;
    }
}

static uint32_t next_generation_unlocked(void)
{
    uint32_t generation = g_csp_next_generation;

    ++g_csp_next_generation;
    if (g_csp_next_generation == 0u) {
        g_csp_next_generation = 1u;
    }

    if (generation == 0u) {
        generation = g_csp_next_generation;
        ++g_csp_next_generation;
    }

    return generation;
}

ano_crypto_status_t ano_crypto_internal_csp_import_secret(
    ano_crypto_key_type_t key_type,
    const uint8_t *key_material,
    size_t key_material_len,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle)
{
    size_t index = 0u;
    ano_crypto_csp_slot_t *slot;
    ano_crypto_status_t st;

    if (out_handle == 0 || key_material == 0 || key_material_len == 0u) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;

    if (key_material_len > ANO_CRYPTO_INTERNAL_CSP_MAX_SECRET_BYTES) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    if (key_type == ANO_CRYPTO_KEY_TYPE_HMAC) {
        /* HMAC accepts non-empty secret byte strings up to the CSP slot limit. */
    } else if (key_type == ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER ||
               key_type == ANO_CRYPTO_KEY_TYPE_CMAC) {
        if (key_material_len != 16u && key_material_len != 24u &&
            key_material_len != 32u) {
            return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
        }
    } else {
        return ANO_CRYPTO_ERR_NOT_IMPLEMENTED;
    }

    if ((flags & ANO_CRYPTO_KEY_FLAG_EXPORTABLE_CANDIDATE) != 0u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    st = find_free_slot_unlocked(&index);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_global_unlock();
        return st;
    }

    slot = &g_csp_slots[index];
    ano_crypto_internal_zeroize(slot, sizeof(*slot));
    slot->generation = next_generation_unlocked();
    slot->state = (uint32_t)ANO_CRYPTO_CSP_STATE_ALLOCATED;
    slot->key_type = (uint32_t)key_type;
    slot->flags = flags;
    slot->key_len = key_material_len;
    memcpy(slot->key_material, key_material, key_material_len);
    slot->state = (uint32_t)ANO_CRYPTO_CSP_STATE_ACTIVE;

    *out_handle = make_handle(index, slot->generation);
    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_csp_import_rsa(
    const ano_crypto_rsa_key_internal_t *key,
    ano_crypto_key_type_t key_type,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle)
{
    size_t index = 0u;
    ano_crypto_csp_slot_t *slot;
    ano_crypto_status_t st;

    if (key == 0 || out_handle == 0 ||
        (key_type != ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY &&
         key_type != ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    if (flags != ANO_CRYPTO_KEY_FLAG_NONE) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    if ((key_type == ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY &&
         key->kind != ANO_CRYPTO_RSA_KEY_KIND_PUBLIC) ||
        (key_type == ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY &&
         key->kind != ANO_CRYPTO_RSA_KEY_KIND_PRIVATE)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    st = find_free_slot_unlocked(&index);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_global_unlock();
        return st;
    }
    slot = &g_csp_slots[index];
    ano_crypto_internal_zeroize(slot, sizeof(*slot));
    slot->generation = next_generation_unlocked();
    slot->state = (uint32_t)ANO_CRYPTO_CSP_STATE_ALLOCATED;
    slot->key_type = (uint32_t)key_type;
    slot->flags = flags;
    memcpy(&slot->rsa_key, key, sizeof(slot->rsa_key));
    slot->state = (uint32_t)ANO_CRYPTO_CSP_STATE_ACTIVE;
    *out_handle = make_handle(index, slot->generation);
    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_csp_import_ec_p256(
    const ano_crypto_ec_p256_key_internal_t *key,
    ano_crypto_key_type_t key_type,
    uint32_t flags,
    ano_crypto_csp_handle_t *out_handle)
{
    size_t index = 0u;
    ano_crypto_csp_slot_t *slot;
    ano_crypto_status_t st;

    if (key == 0 || out_handle == 0 ||
        (key_type != ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY &&
         key_type != ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    if (flags != ANO_CRYPTO_KEY_FLAG_NONE) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    if ((key_type == ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY &&
         key->kind != ANO_CRYPTO_EC_P256_KEY_KIND_PUBLIC) ||
        (key_type == ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY &&
         key->kind != ANO_CRYPTO_EC_P256_KEY_KIND_PRIVATE)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    st = find_free_slot_unlocked(&index);
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_global_unlock();
        return st;
    }
    slot = &g_csp_slots[index];
    ano_crypto_internal_zeroize(slot, sizeof(*slot));
    slot->generation = next_generation_unlocked();
    slot->state = (uint32_t)ANO_CRYPTO_CSP_STATE_ALLOCATED;
    slot->key_type = (uint32_t)key_type;
    slot->flags = flags;
    memcpy(&slot->ec_p256_key, key, sizeof(slot->ec_p256_key));
    slot->state = (uint32_t)ANO_CRYPTO_CSP_STATE_ACTIVE;
    *out_handle = make_handle(index, slot->generation);
    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_csp_lock_rsa(
    ano_crypto_csp_handle_t handle,
    int require_private,
    const ano_crypto_rsa_key_internal_t **out_key)
{
    const ano_crypto_csp_slot_t *slot;
    ano_crypto_status_t st;
    if (out_key == 0 || handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_key = 0;
    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    slot = const_slot_from_handle_unlocked(handle);
    if (slot == 0 || !slot_state_is_live(slot->state)) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (require_private != 0) {
        if (slot->key_type != (uint32_t)ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY ||
            slot->rsa_key.kind != ANO_CRYPTO_RSA_KEY_KIND_PRIVATE) {
            ano_crypto_internal_global_unlock();
            return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
        }
    } else if ((slot->key_type != (uint32_t)ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY &&
                slot->key_type != (uint32_t)ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY) ||
               (slot->rsa_key.kind != ANO_CRYPTO_RSA_KEY_KIND_PUBLIC &&
                slot->rsa_key.kind != ANO_CRYPTO_RSA_KEY_KIND_PRIVATE)) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    *out_key = &slot->rsa_key;
    return ANO_CRYPTO_OK;
}

void ano_crypto_internal_csp_unlock_rsa(void)
{
    ano_crypto_internal_global_unlock();
}

ano_crypto_status_t ano_crypto_internal_csp_lock_ec_p256(
    ano_crypto_csp_handle_t handle,
    int require_private,
    const ano_crypto_ec_p256_key_internal_t **out_key)
{
    const ano_crypto_csp_slot_t *slot;
    ano_crypto_status_t st;
    if (out_key == 0 || handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    *out_key = 0;
    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }
    slot = const_slot_from_handle_unlocked(handle);
    if (slot == 0 || !slot_state_is_live(slot->state)) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    if (require_private != 0) {
        if (slot->key_type != (uint32_t)ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY ||
            slot->ec_p256_key.kind != ANO_CRYPTO_EC_P256_KEY_KIND_PRIVATE) {
            ano_crypto_internal_global_unlock();
            return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
        }
    } else if ((slot->key_type != (uint32_t)ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY &&
                slot->key_type != (uint32_t)ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY) ||
               (slot->ec_p256_key.kind != ANO_CRYPTO_EC_P256_KEY_KIND_PUBLIC &&
                slot->ec_p256_key.kind != ANO_CRYPTO_EC_P256_KEY_KIND_PRIVATE)) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    *out_key = &slot->ec_p256_key;
    return ANO_CRYPTO_OK;
}

void ano_crypto_internal_csp_unlock_ec_p256(void)
{
    ano_crypto_internal_global_unlock();
}

ano_crypto_status_t ano_crypto_internal_csp_destroy(
    ano_crypto_csp_handle_t handle)
{
    ano_crypto_csp_slot_t *slot;
    ano_crypto_status_t st;

    if (handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    slot = slot_from_handle_unlocked(handle);
    if (slot == 0 ||
        (!slot_state_is_live(slot->state) &&
         slot->state != (uint32_t)ANO_CRYPTO_CSP_STATE_ZEROIZED)) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    /* A ZEROIZED tombstone may be explicitly destroyed after module shutdown
     * or fatal transition without restoring or exposing key material. */
    ano_crypto_internal_zeroize(slot->key_material, sizeof(slot->key_material));
    ano_crypto_internal_zeroize(&slot->rsa_key, sizeof(slot->rsa_key));
    ano_crypto_internal_zeroize(&slot->ec_p256_key, sizeof(slot->ec_p256_key));
    slot->key_len = 0u;
    slot->key_type = 0u;
    slot->flags = 0u;
    slot->state = (uint32_t)ANO_CRYPTO_CSP_STATE_DESTROYED;
    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_csp_copy_secret(
    ano_crypto_csp_handle_t handle,
    ano_crypto_key_type_t expected_type,
    uint8_t *out_key_material,
    size_t out_key_material_capacity,
    size_t *out_key_material_len)
{
    const ano_crypto_csp_slot_t *slot;
    ano_crypto_status_t st;

    if (out_key_material == 0 || out_key_material_len == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    *out_key_material_len = 0u;

    if (handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    slot = const_slot_from_handle_unlocked(handle);
    if (slot == 0 || !slot_state_is_live(slot->state)) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    if (slot->key_type != (uint32_t)expected_type) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    if (slot->key_len == 0u || slot->key_len > ANO_CRYPTO_INTERNAL_CSP_MAX_SECRET_BYTES) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_INTERNAL_ERROR;
    }

    if (slot->key_len > out_key_material_capacity) {
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_ERR_BUFFER_TOO_SMALL;
    }

    memcpy(out_key_material, slot->key_material, slot->key_len);
    *out_key_material_len = slot->key_len;
    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_csp_handle_is_valid(
    ano_crypto_csp_handle_t handle,
    int *out_is_valid)
{
    const ano_crypto_csp_slot_t *slot;
    ano_crypto_status_t st;

    if (out_is_valid == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    slot = const_slot_from_handle_unlocked(handle);
    *out_is_valid = (slot != 0 && slot_state_is_live(slot->state)) ? 1 : 0;
    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_csp_handle_get_state(
    ano_crypto_csp_handle_t handle,
    ano_crypto_csp_state_t *out_state)
{
    const ano_crypto_csp_slot_t *slot;
    ano_crypto_status_t st;

    if (out_state == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }

    st = ano_crypto_internal_global_lock();
    if (st != ANO_CRYPTO_OK) {
        return st;
    }

    slot = const_slot_from_handle_unlocked(handle);
    if (slot == 0) {
        *out_state = ANO_CRYPTO_CSP_STATE_INVALID;
        ano_crypto_internal_global_unlock();
        return ANO_CRYPTO_OK;
    }

    switch ((ano_crypto_csp_state_t)slot->state) {
    case ANO_CRYPTO_CSP_STATE_INVALID:
        *out_state = ANO_CRYPTO_CSP_STATE_INVALID;
        break;
    case ANO_CRYPTO_CSP_STATE_ALLOCATED:
    case ANO_CRYPTO_CSP_STATE_INITIALIZED:
    case ANO_CRYPTO_CSP_STATE_ACTIVE:
    case ANO_CRYPTO_CSP_STATE_RESTRICTED:
    case ANO_CRYPTO_CSP_STATE_ZEROIZED:
    case ANO_CRYPTO_CSP_STATE_DESTROYED:
        *out_state = (ano_crypto_csp_state_t)slot->state;
        break;
    default:
        *out_state = ANO_CRYPTO_CSP_STATE_INVALID;
        break;
    }

    ano_crypto_internal_global_unlock();
    return ANO_CRYPTO_OK;
}

const char *ano_crypto_csp_state_name(
    ano_crypto_csp_state_t state)
{
    switch (state) {
    case ANO_CRYPTO_CSP_STATE_INVALID:
        return "INVALID";
    case ANO_CRYPTO_CSP_STATE_ALLOCATED:
        return "ALLOCATED";
    case ANO_CRYPTO_CSP_STATE_INITIALIZED:
        return "INITIALIZED";
    case ANO_CRYPTO_CSP_STATE_ACTIVE:
        return "ACTIVE";
    case ANO_CRYPTO_CSP_STATE_RESTRICTED:
        return "RESTRICTED";
    case ANO_CRYPTO_CSP_STATE_ZEROIZED:
        return "ZEROIZED";
    case ANO_CRYPTO_CSP_STATE_DESTROYED:
        return "DESTROYED";
    default:
        return "UNKNOWN_CSP_STATE";
    }
}
