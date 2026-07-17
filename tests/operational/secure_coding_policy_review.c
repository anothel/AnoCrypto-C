#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <anocrypto/anocrypto.h>

static int fail(const char *step, ano_crypto_status_t status)
{
    (void)fprintf(stderr, "secure_coding_policy_review: FAIL step=%s status=%s\n",
                  step, ano_crypto_status_name(status));
    return 1;
}

int main(void)
{
    static const uint8_t key_bytes[16] = {
        0x2bu, 0x7eu, 0x15u, 0x16u, 0x28u, 0xaeu, 0xd2u, 0xa6u,
        0xabu, 0xf7u, 0x15u, 0x88u, 0x09u, 0xcfu, 0x4fu, 0x3cu
    };
    static const uint8_t input[16] = {
        0x6bu, 0xc1u, 0xbeu, 0xe2u, 0x2eu, 0x40u, 0x9fu, 0x96u,
        0xe9u, 0x3du, 0x7eu, 0x11u, 0x73u, 0x93u, 0x17u, 0x2au
    };
    uint8_t tag[ANO_CRYPTO_CMAC_TAG_SIZE];
    uint8_t altered[ANO_CRYPTO_CMAC_TAG_SIZE];
    size_t written = 0u;
    size_t i;
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_status_t st;

    st = ano_crypto_module_init(0);
    if (st != ANO_CRYPTO_OK) {
        return fail("module_init", st);
    }

    st = ano_crypto_key_import_secret(
        ANO_CRYPTO_KEY_TYPE_CMAC, key_bytes, sizeof(key_bytes), 0u, &handle);
    if (st != ANO_CRYPTO_OK) {
        return fail("key_import", st);
    }

    st = ano_crypto_cmac_compute_with_key(
        handle, input, sizeof(input), tag, sizeof(tag), &written, 0u);
    if (st != ANO_CRYPTO_OK || written != sizeof(tag)) {
        return fail("cmac_compute", st);
    }

    for (i = 0u; i < sizeof(altered); ++i) {
        altered[i] = tag[i];
    }
    altered[0] = (uint8_t)(altered[0] ^ 1u);

    st = ano_crypto_cmac_verify_with_key(
        handle, input, sizeof(input), tag, sizeof(tag), 0u);
    if (st != ANO_CRYPTO_OK) {
        return fail("verify_match", st);
    }
    st = ano_crypto_cmac_verify_with_key(
        handle, input, sizeof(input), altered, sizeof(altered), 0u);
    if (st != ANO_CRYPTO_ERR_AUTHENTICATION_FAILED) {
        return fail("verify_mismatch", st);
    }

    (void)ano_crypto_csp_zeroize(tag, sizeof(tag));
    (void)ano_crypto_csp_zeroize(altered, sizeof(altered));
    st = ano_crypto_key_destroy(handle);
    if (st != ANO_CRYPTO_OK) {
        return fail("key_destroy", st);
    }
    st = ano_crypto_module_shutdown();
    if (st != ANO_CRYPTO_OK) {
        return fail("module_shutdown", st);
    }

    (void)printf("policy_id=SCP-P2-06-R1\n");
    (void)printf("cmac_fixed_tag_review=PASS\n");
    (void)printf("timing_claim=none\n");
    (void)printf("sensitive_values_printed=0\n");
    (void)printf("secure_coding_policy_review: PASS\n");
    return 0;
}
