#include <anocrypto/anocrypto.h>

#include <string.h>

int main(void)
{
    const char *version = 0;
    if (ano_crypto_version_string(&version) != ANO_CRYPTO_OK || version == 0) {
        return 1;
    }
    return strcmp(version, "0.1.0-p3-09-ecdsa-p256-sha256") == 0 ? 0 : 2;
}
