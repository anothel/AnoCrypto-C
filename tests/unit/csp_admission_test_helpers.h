#ifndef ANO_CRYPTO_CSP_ADMISSION_TEST_HELPERS_H
#define ANO_CRYPTO_CSP_ADMISSION_TEST_HELPERS_H

#include "ac_internal.h"
#include "ac_csp_internal.h"
#include "ac_rsa_internal.h"
#include "ac_ecdsa_internal.h"
#include "../fixtures/rsa_test_helpers.h"
#include "../fixtures/ecdsa_p256_test_vectors.h"

#define ADMISSION_CASES 7u

/* Synthetic fixtures only. Prepare exactly the material that key.c passes to
 * the insertion boundary, outside the global lock. Scheduling the transition
 * between the initial gate/preparation and insertion is deterministic and
 * needs neither production test hooks nor sleep-based race probabilities. */
typedef struct admission_fixture_s {
    ano_crypto_rsa_key_internal_t rsa[2];
    ano_crypto_ec_p256_key_internal_t ec[2];
    ano_crypto_key_type_t rsa_type[2];
    ano_crypto_key_type_t ec_type[2];
    ano_crypto_csp_handle_t controls[ADMISSION_CASES];
} admission_fixture_t;

static const uint8_t admission_secret[32] = { 1u };

static ano_crypto_status_t admission_public_import(size_t i,
                                                  ano_crypto_csp_handle_t *out)
{
    rsa_test_components_t c;
    if (i < 3u) {
        const ano_crypto_key_type_t types[3] = {
            ANO_CRYPTO_KEY_TYPE_HMAC, ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
            ANO_CRYPTO_KEY_TYPE_CMAC
        };
        return ano_crypto_key_import_secret(types[i], admission_secret,
            sizeof(admission_secret), ANO_CRYPTO_KEY_FLAG_NONE, out);
    }
    if (i < 5u) {
        if (i == 3u) {
            rsa_test_public_components(&c);
        } else {
            rsa_test_private_components(&c);
        }
        return rsa_test_import(&c, ANO_CRYPTO_KEY_FLAG_NONE, out);
    }
    return i == 5u ? ecdsa_import_public(out) : ecdsa_import_private(out);
}

static void admission_stage(admission_fixture_t *f)
{
    size_t i;
    for (i = 0u; i < ADMISSION_CASES; ++i) {
        expect_status("public admission before transition",
            admission_public_import(i, &f->controls[i]), ANO_CRYPTO_OK);
        /* The in-flight imports have already passed their public gate. */
        expect_status("initial admission gate",
            ano_crypto_module_require_operational(), ANO_CRYPTO_OK);
    }
    for (i = 0u; i < 2u; ++i) {
        const ano_crypto_rsa_components_internal_t c = {
            RSA_KAT_N, sizeof(RSA_KAT_N), RSA_KAT_E, sizeof(RSA_KAT_E),
            i ? RSA_KAT_D : NULL, i ? sizeof(RSA_KAT_D) : 0u,
            i ? RSA_KAT_P : NULL, i ? sizeof(RSA_KAT_P) : 0u,
            i ? RSA_KAT_Q : NULL, i ? sizeof(RSA_KAT_Q) : 0u,
            i ? RSA_KAT_DP : NULL, i ? sizeof(RSA_KAT_DP) : 0u,
            i ? RSA_KAT_DQ : NULL, i ? sizeof(RSA_KAT_DQ) : 0u,
            i ? RSA_KAT_QINV : NULL, i ? sizeof(RSA_KAT_QINV) : 0u
        };
        expect_status("prepare RSA outside lock",
            ano_crypto_internal_rsa_validate_and_prepare(
                &c, &f->rsa[i], &f->rsa_type[i]), ANO_CRYPTO_OK);
        expect_status("prepare P256 outside lock",
            ano_crypto_internal_ec_p256_validate_and_prepare(
                ECDSA_P256_X, sizeof(ECDSA_P256_X),
                ECDSA_P256_Y, sizeof(ECDSA_P256_Y),
                i ? ECDSA_P256_D : NULL, i ? sizeof(ECDSA_P256_D) : 0u,
                &f->ec[i], &f->ec_type[i]), ANO_CRYPTO_OK);
    }
}

static void admission_after_transition(admission_fixture_t *f,
                                      ano_crypto_status_t expected)
{
    size_t i;
    for (i = 0u; i < ADMISSION_CASES; ++i) {
        ano_crypto_csp_handle_t out = f->controls[i];
        ano_crypto_csp_state_t state = ANO_CRYPTO_CSP_STATE_INVALID;
        ano_crypto_status_t st;
        int live = 1;
        /* Insertion won: shutdown/fatal must retain a ZEROIZED tombstone. */
        expect_status("admitted key state after transition",
            ano_crypto_key_get_state(f->controls[i], &state), ANO_CRYPTO_OK);
        expect_status("admitted key validity after transition",
            ano_crypto_csp_handle_is_valid(f->controls[i], &live), ANO_CRYPTO_OK);
        if (state != ANO_CRYPTO_CSP_STATE_ZEROIZED || live != 0) {
            (void)fprintf(stderr, "FAIL admission case %lu survived transition\n",
                          (unsigned long)i);
            ++g_failures;
        }
        /* Transition won: the already-prepared import must be rejected. */
        if (i < 3u) {
            const ano_crypto_key_type_t types[3] = {
                ANO_CRYPTO_KEY_TYPE_HMAC, ANO_CRYPTO_KEY_TYPE_BLOCK_CIPHER,
                ANO_CRYPTO_KEY_TYPE_CMAC
            };
            st = ano_crypto_internal_csp_import_secret(types[i],
                admission_secret, sizeof(admission_secret), 0u, &out);
        } else if (i < 5u) {
            st = ano_crypto_internal_csp_import_rsa(
                &f->rsa[i - 3u], f->rsa_type[i - 3u], 0u, &out);
        } else {
            st = ano_crypto_internal_csp_import_ec_p256(
                &f->ec[i - 5u], f->ec_type[i - 5u], 0u, &out);
        }
        expect_status("final admission after transition", st, expected);
        if (out != ANO_CRYPTO_CSP_INVALID_HANDLE) {
            (void)fprintf(stderr, "FAIL rejected admission case %lu returned handle\n",
                          (unsigned long)i);
            ++g_failures;
            (void)ano_crypto_key_destroy(out);
        }
        out = f->controls[i];
        expect_status("public import after transition",
            admission_public_import(i, &out), expected);
        if (out != ANO_CRYPTO_CSP_INVALID_HANDLE) {
            (void)fprintf(stderr, "FAIL rejected public import retained handle\n");
            ++g_failures;
        }
        expect_status("destroy admission tombstone",
            ano_crypto_key_destroy(f->controls[i]), ANO_CRYPTO_OK);
        expect_status("destroyed admission tombstone state",
            ano_crypto_key_get_state(f->controls[i], &state), ANO_CRYPTO_OK);
        if (state != ANO_CRYPTO_CSP_STATE_DESTROYED) {
            (void)fprintf(stderr, "FAIL admission tombstone not DESTROYED\n");
            ++g_failures;
        }
    }
    ano_crypto_internal_zeroize(f, sizeof(*f));
}

#endif
