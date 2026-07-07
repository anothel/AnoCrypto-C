#include "ac_kat_manifest.h"

/*
 * P1 KAT manifest skeleton.
 *
 * These entries are placeholders for manifest tracking only. They do not contain
 * test vectors and must not be treated as algorithm validation evidence.
 */
static const ano_crypto_internal_kat_manifest_entry_t g_kat_manifest[] = {
    {
        "hash.sha256.placeholder",
        "SHA-256",
        "TODO(P1): public-source KAT manifest entry required before implementation",
        "placeholder"
    },
    {
        "hmac.sha256.placeholder",
        "HMAC-SHA-256",
        "TODO(P1): public-source KAT manifest entry required before implementation",
        "placeholder"
    },
    {
        "drbg.hmac-sha256.placeholder",
        "HMAC_DRBG-SHA-256",
        "TODO(P1): public-source KAT manifest entry required before implementation",
        "placeholder"
    }
};

size_t ano_crypto_internal_kat_manifest_count(void)
{
    return sizeof(g_kat_manifest) / sizeof(g_kat_manifest[0]);
}

const ano_crypto_internal_kat_manifest_entry_t *ano_crypto_internal_kat_manifest_get(
    size_t index)
{
    if (index >= ano_crypto_internal_kat_manifest_count()) {
        return 0;
    }

    return &g_kat_manifest[index];
}
