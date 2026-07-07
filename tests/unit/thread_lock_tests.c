#include <stdio.h>
#include "ac_thread_internal.h"

#define REQUIRE_OK(expr) do { \
    ano_crypto_status_t status_ = (expr); \
    if (status_ != ANO_CRYPTO_OK) { \
        fprintf(stderr, "%s failed with %s at line %d\n", #expr, ano_crypto_status_name(status_), __LINE__); \
        return 1; \
    } \
} while (0)

int main(void)
{
    REQUIRE_OK(ano_crypto_internal_global_lock());
    ano_crypto_internal_global_unlock();

    REQUIRE_OK(ano_crypto_internal_global_lock());
    ano_crypto_internal_global_unlock();

    return 0;
}
