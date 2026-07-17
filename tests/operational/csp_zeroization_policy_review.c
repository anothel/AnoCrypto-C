#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include <anocrypto/ac_csp.h>
#include <anocrypto/ac_module.h>

static int buffer_is_zero(const uint8_t *buffer, size_t len)
{
    uint8_t aggregate = 0u;
    size_t i;

    for (i = 0u; i < len; ++i) {
        aggregate = (uint8_t)(aggregate | buffer[i]);
    }
    return aggregate == 0u;
}

int main(void)
{
    uint8_t buffer[64];
    size_t i;

    for (i = 0u; i < sizeof(buffer); ++i) {
        buffer[i] = (uint8_t)(i + 1u);
    }

    if (ano_crypto_csp_zeroize(buffer, sizeof(buffer)) != ANO_CRYPTO_OK ||
        !buffer_is_zero(buffer, sizeof(buffer))) {
        (void)fprintf(stderr, "csp_zeroization_policy_review: zeroize failure\n");
        return 1;
    }

    (void)puts("policy_id=CZP-P2-05-R1");
    (void)puts("public_zeroize_observed=PASS");
    (void)puts("module_owned_handle_table_shutdown_wipe=IMPLEMENTED_P2_05");
    (void)puts("module_owned_handle_table_fatal_wipe=IMPLEMENTED_P2_05");
    (void)puts("caller_owned_context_tracking=NOT_IMPLEMENTED");
    (void)puts("caller_owned_context_zeroization=CALLER_UNINIT_REQUIRED");
    (void)puts("persistent_export_wrapping=OUT_OF_SCOPE");
    (void)puts("selected_oe_compiler_review=NOT_RUN");
    (void)puts("sensitive_values_printed=0");
    (void)puts("csp_zeroization_policy_review: PASS");
    return 0;
}
