#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <anocrypto/anocrypto.h>

static int g_failures = 0;

static void expect_status(const char *name, ano_crypto_status_t got, ano_crypto_status_t expected)
{
    if (got != expected) {
        fprintf(stderr, "FAIL %s: got %s expected %s\n",
                name, ano_crypto_status_name(got), ano_crypto_status_name(expected));
        ++g_failures;
    }
}

static int bytes_equal(const uint8_t *a, const uint8_t *b, size_t len)
{
    uint8_t diff = 0u;
    size_t i;

    for (i = 0u; i < len; ++i) {
        diff = (uint8_t)(diff | (uint8_t)(a[i] ^ b[i]));
    }

    return diff == 0u;
}

int main(void)
{
    static const uint8_t key[] = { 'J', 'e', 'f', 'e' };
    static const uint8_t msg[] = {
        'w','h','a','t',' ','d','o',' ','y','a',' ','w','a','n','t',' ',
        'f','o','r',' ','n','o','t','h','i','n','g','?'
    };
    static const uint8_t expected_sha256[32] = {
        0x5bu, 0xdcu, 0xc1u, 0x46u, 0xbfu, 0x60u, 0x75u, 0x4eu,
        0x6au, 0x04u, 0x24u, 0x26u, 0x08u, 0x95u, 0x75u, 0xc7u,
        0x5au, 0x00u, 0x3fu, 0x08u, 0x9du, 0x27u, 0x39u, 0x83u,
        0x9du, 0xecu, 0x58u, 0xb9u, 0x64u, 0xecu, 0x38u, 0x43u
    };
    static const uint8_t expected_sha384[48] = {
        0xafu, 0x45u, 0xd2u, 0xe3u, 0x76u, 0x48u, 0x40u, 0x31u,
        0x61u, 0x7fu, 0x78u, 0xd2u, 0xb5u, 0x8au, 0x6bu, 0x1bu,
        0x9cu, 0x7eu, 0xf4u, 0x64u, 0xf5u, 0xa0u, 0x1bu, 0x47u,
        0xe4u, 0x2eu, 0xc3u, 0x73u, 0x63u, 0x22u, 0x44u, 0x5eu,
        0x8eu, 0x22u, 0x40u, 0xcau, 0x5eu, 0x69u, 0xe2u, 0xc7u,
        0x8bu, 0x32u, 0x39u, 0xecu, 0xfau, 0xb2u, 0x16u, 0x49u
    };
    static const uint8_t expected_sha512[64] = {
        0x16u, 0x4bu, 0x7au, 0x7bu, 0xfcu, 0xf8u, 0x19u, 0xe2u,
        0xe3u, 0x95u, 0xfbu, 0xe7u, 0x3bu, 0x56u, 0xe0u, 0xa3u,
        0x87u, 0xbdu, 0x64u, 0x22u, 0x2eu, 0x83u, 0x1fu, 0xd6u,
        0x10u, 0x27u, 0x0cu, 0xd7u, 0xeau, 0x25u, 0x05u, 0x54u,
        0x97u, 0x58u, 0xbfu, 0x75u, 0xc0u, 0x5au, 0x99u, 0x4au,
        0x6du, 0x03u, 0x4fu, 0x65u, 0xf8u, 0xf0u, 0xe6u, 0xfdu,
        0xcau, 0xeau, 0xb1u, 0xa3u, 0x4du, 0x4au, 0x6bu, 0x4bu,
        0x63u, 0x6eu, 0x07u, 0x0au, 0x38u, 0xbcu, 0xe7u, 0x37u
    };
    ano_crypto_csp_handle_t handle = ANO_CRYPTO_CSP_INVALID_HANDLE;
    ano_crypto_csp_state_t state = ANO_CRYPTO_CSP_STATE_INVALID;
    uint8_t mac[64];
    size_t written = 0u;
    int is_valid = 0;

    expect_status("module init", ano_crypto_module_init(0), ANO_CRYPTO_OK);
    expect_status("import hmac key", ano_crypto_key_import_secret(
                      ANO_CRYPTO_KEY_TYPE_HMAC,
                      key, sizeof(key),
                      ANO_CRYPTO_KEY_FLAG_VALIDATION_TARGET_CANDIDATE,
                      &handle), ANO_CRYPTO_OK);

    if (handle == ANO_CRYPTO_CSP_INVALID_HANDLE) {
        fprintf(stderr, "FAIL import hmac key returned invalid handle\n");
        ++g_failures;
    }

    expect_status("handle valid query", ano_crypto_csp_handle_is_valid(handle, &is_valid), ANO_CRYPTO_OK);
    if (is_valid != 1) {
        fprintf(stderr, "FAIL imported handle not valid\n");
        ++g_failures;
    }

    expect_status("key state active", ano_crypto_key_get_state(handle, &state), ANO_CRYPTO_OK);
    if (state != ANO_CRYPTO_CSP_STATE_ACTIVE) {
        fprintf(stderr, "FAIL key state: got %s expected ACTIVE\n", ano_crypto_csp_state_name(state));
        ++g_failures;
    }

    expect_status("hmac-sha256 with key handle", ano_crypto_hmac_compute_with_key(
                      ANO_CRYPTO_HASH_ALG_SHA256, handle, msg, sizeof(msg),
                      mac, sizeof(mac), &written), ANO_CRYPTO_OK);
    if (written != sizeof(expected_sha256) ||
        !bytes_equal(mac, expected_sha256, sizeof(expected_sha256))) {
        fprintf(stderr, "FAIL hmac-sha256 handle KAT mismatch\n");
        ++g_failures;
    }

    written = 0u;
    expect_status("hmac-sha384 with key handle", ano_crypto_hmac_compute_with_key(
                      ANO_CRYPTO_HASH_ALG_SHA384, handle, msg, sizeof(msg),
                      mac, sizeof(mac), &written), ANO_CRYPTO_OK);
    if (written != sizeof(expected_sha384) ||
        !bytes_equal(mac, expected_sha384, sizeof(expected_sha384))) {
        fprintf(stderr, "FAIL hmac-sha384 handle KAT mismatch\n");
        ++g_failures;
    }

    written = 0u;
    expect_status("hmac-sha512 with key handle", ano_crypto_hmac_compute_with_key(
                      ANO_CRYPTO_HASH_ALG_SHA512, handle, msg, sizeof(msg),
                      mac, sizeof(mac), &written), ANO_CRYPTO_OK);
    if (written != sizeof(expected_sha512) ||
        !bytes_equal(mac, expected_sha512, sizeof(expected_sha512))) {
        fprintf(stderr, "FAIL hmac-sha512 handle KAT mismatch\n");
        ++g_failures;
    }

    expect_status("destroy hmac key", ano_crypto_key_destroy(handle), ANO_CRYPTO_OK);
    expect_status("key state destroyed", ano_crypto_key_get_state(handle, &state), ANO_CRYPTO_OK);
    if (state != ANO_CRYPTO_CSP_STATE_DESTROYED) {
        fprintf(stderr, "FAIL key state after destroy: got %s expected DESTROYED\n",
                ano_crypto_csp_state_name(state));
        ++g_failures;
    }

    expect_status("handle invalid after destroy", ano_crypto_csp_handle_is_valid(handle, &is_valid), ANO_CRYPTO_OK);
    if (is_valid != 0) {
        fprintf(stderr, "FAIL destroyed handle still valid\n");
        ++g_failures;
    }

    expect_status("hmac with destroyed key", ano_crypto_hmac_compute_with_key(
                      ANO_CRYPTO_HASH_ALG_SHA256,
                      handle,
                      msg, sizeof(msg),
                      mac, sizeof(mac),
                      &written), ANO_CRYPTO_ERR_INVALID_ARGUMENT);

    expect_status("module shutdown", ano_crypto_module_shutdown(), ANO_CRYPTO_OK);

    if (g_failures != 0) {
        fprintf(stderr, "hmac_key_handle_kat_tests: %d failure(s)\n", g_failures);
        return 1;
    }

    puts("hmac_key_handle_kat_tests: PASS");
    return 0;
}
