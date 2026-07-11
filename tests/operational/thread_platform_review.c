#include <stdio.h>
#include "ac_thread_internal.h"

#ifndef ANO_CRYPTO_TEST_CONCURRENT_CSP_AVAILABLE
#define ANO_CRYPTO_TEST_CONCURRENT_CSP_AVAILABLE 0
#endif

#ifndef ANO_CRYPTO_TEST_SANITIZER_MODE
#define ANO_CRYPTO_TEST_SANITIZER_MODE "none"
#endif

int main(void)
{
    const char *backend = ano_crypto_internal_thread_backend_name();
    int available = ano_crypto_internal_thread_safety_available();

    if (backend == 0) {
        fprintf(stderr, "thread platform review backend query failed\n");
        return 1;
    }

    printf("thread_backend=%s\n", backend);
    printf("thread_safety_available=%s\n", available != 0 ? "yes" : "no");
    printf("concurrent_csp_test_built=%s\n",
           ANO_CRYPTO_TEST_CONCURRENT_CSP_AVAILABLE != 0 ? "yes" : "no");
    printf("sanitizer_mode=%s\n", ANO_CRYPTO_TEST_SANITIZER_MODE);
    return 0;
}
