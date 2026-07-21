#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "ac_internal.h"
#include "ac_rsa_internal.h"

#define RSA_MAX_LIMBS ANO_CRYPTO_RSA_LIMBS
#define RSA_PRODUCT_LIMBS (2u * RSA_MAX_LIMBS + 2u)

static void bn_zero(uint32_t *a, size_t len)
{
    ano_crypto_internal_zeroize(a, len * sizeof(*a));
}

static void bn_copy(uint32_t *out, const uint32_t *in, size_t len)
{
    memcpy(out, in, len * sizeof(*out));
}

static int bn_cmp(const uint32_t *a, const uint32_t *b, size_t len)
{
    size_t i = len;
    while (i > 0u) {
        --i;
        if (a[i] < b[i]) {
            return -1;
        }
        if (a[i] > b[i]) {
            return 1;
        }
    }
    return 0;
}

static uint32_t bn_sub(uint32_t *out, const uint32_t *a, const uint32_t *b, size_t len)
{
    uint64_t borrow = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) {
        uint64_t av = (uint64_t)a[i];
        uint64_t bv = (uint64_t)b[i] + borrow;
        out[i] = (uint32_t)(av - bv);
        borrow = av < bv ? 1u : 0u;
    }
    return (uint32_t)borrow;
}

static uint32_t bn_add(uint32_t *out, const uint32_t *a, const uint32_t *b, size_t len)
{
    uint64_t carry = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) {
        uint64_t z = (uint64_t)a[i] + (uint64_t)b[i] + carry;
        out[i] = (uint32_t)z;
        carry = z >> 32;
    }
    return (uint32_t)carry;
}

static void bn_add_low(uint32_t *acc, size_t acc_len, const uint32_t *b, size_t b_len)
{
    uint64_t carry = 0u;
    size_t i;
    for (i = 0u; i < b_len; ++i) {
        uint64_t z = (uint64_t)acc[i] + (uint64_t)b[i] + carry;
        acc[i] = (uint32_t)z;
        carry = z >> 32;
    }
    while (carry != 0u && i < acc_len) {
        uint64_t z = (uint64_t)acc[i] + carry;
        acc[i] = (uint32_t)z;
        carry = z >> 32;
        ++i;
    }
}

static void bn_from_be(uint32_t *out, size_t out_len, const uint8_t *in, size_t in_len)
{
    size_t i;
    bn_zero(out, out_len);
    for (i = 0u; i < in_len; ++i) {
        size_t reverse = in_len - 1u - i;
        out[i / 4u] |= (uint32_t)in[reverse] << ((i % 4u) * 8u);
    }
}

static void bn_to_be(uint8_t *out, size_t out_len, const uint32_t *in, size_t in_len)
{
    size_t i;
    (void)in_len;
    for (i = 0u; i < out_len; ++i) {
        size_t reverse = out_len - 1u - i;
        out[reverse] = (uint8_t)(in[i / 4u] >> ((i % 4u) * 8u));
    }
}

static int bn_is_one(const uint32_t *a, size_t len)
{
    size_t i;
    if (a[0] != 1u) {
        return 0;
    }
    for (i = 1u; i < len; ++i) {
        if (a[i] != 0u) {
            return 0;
        }
    }
    return 1;
}

static void bn_mul(const uint32_t *a, size_t a_len, const uint32_t *b, size_t b_len,
                   uint32_t *out, size_t out_len)
{
    size_t i;
    bn_zero(out, out_len);
    for (i = 0u; i < a_len; ++i) {
        uint64_t carry = 0u;
        size_t j;
        for (j = 0u; j < b_len && i + j < out_len; ++j) {
            uint64_t z = (uint64_t)a[i] * (uint64_t)b[j] +
                         (uint64_t)out[i + j] + carry;
            out[i + j] = (uint32_t)z;
            carry = z >> 32;
        }
        j = i + b_len;
        while (carry != 0u && j < out_len) {
            uint64_t z = (uint64_t)out[j] + carry;
            out[j] = (uint32_t)z;
            carry = z >> 32;
            ++j;
        }
    }
}

static void bn_mod_reduce(uint32_t *out, size_t mod_len,
                          const uint32_t *input, size_t input_len,
                          const uint32_t *mod)
{
    size_t bit = input_len * 32u;
    uint32_t rem[RSA_MAX_LIMBS];
    bn_zero(rem, RSA_MAX_LIMBS);
    while (bit > 0u) {
        uint32_t carry = 0u;
        size_t i;
        --bit;
        for (i = 0u; i < mod_len; ++i) {
            uint32_t next = rem[i] >> 31;
            rem[i] = (rem[i] << 1) | carry;
            carry = next;
        }
        rem[0] |= (input[bit / 32u] >> (bit % 32u)) & 1u;
        if (carry != 0u || bn_cmp(rem, mod, mod_len) >= 0) {
            (void)bn_sub(rem, rem, mod, mod_len);
        }
    }
    bn_copy(out, rem, mod_len);
    ano_crypto_internal_zeroize(rem, sizeof(rem));
}

static uint32_t mont_n0_inv(uint32_t n0)
{
    uint32_t x = 1u;
    unsigned i;
    for (i = 0u; i < 5u; ++i) {
        x *= 2u - n0 * x;
    }
    return (uint32_t)(0u - x);
}

static void bn_mod_double(uint32_t *a, const uint32_t *mod, size_t len)
{
    uint32_t carry = 0u;
    size_t i;
    for (i = 0u; i < len; ++i) {
        uint32_t next = a[i] >> 31;
        a[i] = (a[i] << 1) | carry;
        carry = next;
    }
    if (carry != 0u || bn_cmp(a, mod, len) >= 0) {
        (void)bn_sub(a, a, mod, len);
    }
}

static void mont_compute_rr(uint32_t *rr, const uint32_t *mod, size_t len)
{
    size_t i;
    bn_zero(rr, len);
    rr[0] = 1u;
    for (i = 0u; i < len * 64u; ++i) {
        bn_mod_double(rr, mod, len);
    }
}

static void mont_mul(uint32_t *out, const uint32_t *a, const uint32_t *b,
                     const uint32_t *mod, uint32_t n0_inv, size_t len)
{
    uint32_t t[RSA_PRODUCT_LIMBS];
    size_t i;
    bn_zero(t, RSA_PRODUCT_LIMBS);

    for (i = 0u; i < len; ++i) {
        uint64_t carry = 0u;
        size_t j;
        for (j = 0u; j < len; ++j) {
            uint64_t z = (uint64_t)a[i] * (uint64_t)b[j] +
                         (uint64_t)t[i + j] + carry;
            t[i + j] = (uint32_t)z;
            carry = z >> 32;
        }
        j = i + len;
        while (carry != 0u) {
            uint64_t z = (uint64_t)t[j] + carry;
            t[j] = (uint32_t)z;
            carry = z >> 32;
            ++j;
        }
    }

    for (i = 0u; i < len; ++i) {
        uint32_t m = t[i] * n0_inv;
        uint64_t carry = 0u;
        size_t j;
        for (j = 0u; j < len; ++j) {
            uint64_t z = (uint64_t)m * (uint64_t)mod[j] +
                         (uint64_t)t[i + j] + carry;
            t[i + j] = (uint32_t)z;
            carry = z >> 32;
        }
        j = i + len;
        while (carry != 0u) {
            uint64_t z = (uint64_t)t[j] + carry;
            t[j] = (uint32_t)z;
            carry = z >> 32;
            ++j;
        }
    }

    bn_copy(out, t + len, len);
    if (t[2u * len] != 0u || bn_cmp(out, mod, len) >= 0) {
        (void)bn_sub(out, out, mod, len);
    }
    ano_crypto_internal_zeroize(t, sizeof(t));
}

static void mont_to(uint32_t *out, const uint32_t *a,
                    const uint32_t *mod, const uint32_t *rr,
                    uint32_t n0_inv, size_t len)
{
    mont_mul(out, a, rr, mod, n0_inv, len);
}

static void mont_from(uint32_t *out, const uint32_t *a,
                      const uint32_t *mod, uint32_t n0_inv, size_t len)
{
    uint32_t one[RSA_MAX_LIMBS];
    bn_zero(one, RSA_MAX_LIMBS);
    one[0] = 1u;
    mont_mul(out, a, one, mod, n0_inv, len);
    ano_crypto_internal_zeroize(one, sizeof(one));
}

static void bn_select(uint32_t *out, const uint32_t *a, const uint32_t *b,
                      uint32_t choose_b, size_t len)
{
    uint32_t mask = (uint32_t)(0u - (choose_b & 1u));
    size_t i;
    for (i = 0u; i < len; ++i) {
        out[i] = (a[i] & ~mask) | (b[i] & mask);
    }
}

static void mont_exp(uint32_t *out, const uint32_t *base,
                     const uint32_t *exponent, size_t exponent_bits,
                     const uint32_t *mod, const uint32_t *rr,
                     uint32_t n0_inv, size_t len)
{
    uint32_t one[RSA_MAX_LIMBS];
    uint32_t acc[RSA_MAX_LIMBS];
    uint32_t base_m[RSA_MAX_LIMBS];
    uint32_t squared[RSA_MAX_LIMBS];
    uint32_t multiplied[RSA_MAX_LIMBS];
    size_t bit = exponent_bits;

    bn_zero(one, RSA_MAX_LIMBS);
    one[0] = 1u;
    mont_to(acc, one, mod, rr, n0_inv, len);
    mont_to(base_m, base, mod, rr, n0_inv, len);

    while (bit > 0u) {
        uint32_t v;
        --bit;
        mont_mul(squared, acc, acc, mod, n0_inv, len);
        mont_mul(multiplied, squared, base_m, mod, n0_inv, len);
        v = (exponent[bit / 32u] >> (bit % 32u)) & 1u;
        bn_select(acc, squared, multiplied, v, len);
    }

    mont_from(out, acc, mod, n0_inv, len);
    ano_crypto_internal_zeroize(one, sizeof(one));
    ano_crypto_internal_zeroize(acc, sizeof(acc));
    ano_crypto_internal_zeroize(base_m, sizeof(base_m));
    ano_crypto_internal_zeroize(squared, sizeof(squared));
    ano_crypto_internal_zeroize(multiplied, sizeof(multiplied));
}

static void public_exp(uint32_t *out, const uint32_t *base,
                       const uint32_t *mod, const uint32_t *rr,
                       uint32_t n0_inv, size_t len)
{
    uint32_t exponent[1];
    exponent[0] = ANO_CRYPTO_RSA_PUBLIC_EXPONENT;
    mont_exp(out, base, exponent, 17u, mod, rr, n0_inv, len);
}

static int component_absent(const uint8_t *ptr, size_t len)
{
    return ptr == 0 && len == 0u;
}

static int component_exact(const uint8_t *ptr, size_t len, size_t expected)
{
    return ptr != 0 && len == expected;
}

static void scalar_blind_exponent(uint32_t out[ANO_CRYPTO_RSA_BLINDED_EXP_LIMBS],
                                  const uint32_t exponent[ANO_CRYPTO_RSA_HALF_LIMBS],
                                  const uint32_t prime[ANO_CRYPTO_RSA_HALF_LIMBS],
                                  uint32_t scalar)
{
    uint32_t pm1[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint64_t carry = 0u;
    size_t i;
    bn_copy(pm1, prime, ANO_CRYPTO_RSA_HALF_LIMBS);
    pm1[0] -= 1u;
    for (i = 0u; i < ANO_CRYPTO_RSA_HALF_LIMBS; ++i) {
        uint64_t z = (uint64_t)pm1[i] * (uint64_t)scalar + carry;
        out[i] = (uint32_t)z;
        carry = z >> 32;
    }
    out[ANO_CRYPTO_RSA_HALF_LIMBS] = (uint32_t)carry;
    carry = 0u;
    for (i = 0u; i < ANO_CRYPTO_RSA_HALF_LIMBS; ++i) {
        uint64_t z = (uint64_t)out[i] + (uint64_t)exponent[i] + carry;
        out[i] = (uint32_t)z;
        carry = z >> 32;
    }
    out[ANO_CRYPTO_RSA_HALF_LIMBS] += (uint32_t)carry;
    ano_crypto_internal_zeroize(pm1, sizeof(pm1));
}

ano_crypto_status_t ano_crypto_internal_rsa_validate_and_prepare(
    const ano_crypto_rsa_components_internal_t *components,
    ano_crypto_rsa_key_internal_t *out_key,
    ano_crypto_key_type_t *out_type)
{
    uint32_t d[RSA_MAX_LIMBS];
    uint32_t product[RSA_MAX_LIMBS];
    uint32_t temp_product[RSA_MAX_LIMBS];
    uint32_t pm1[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t qm1[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t reduced[ANO_CRYPTO_RSA_HALF_LIMBS];
    int is_public;

    if (components == 0 || out_key == 0 || out_type == 0) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    ano_crypto_internal_zeroize(out_key, sizeof(*out_key));
    *out_type = ANO_CRYPTO_KEY_TYPE_SECRET_BYTES;

    if (!component_exact(components->modulus, components->modulus_len, 256u) ||
        !component_exact(components->public_exponent, components->public_exponent_len, 3u)) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    if (components->modulus[0] < 0x80u || (components->modulus[255] & 1u) == 0u ||
        components->public_exponent[0] != 0x01u ||
        components->public_exponent[1] != 0x00u ||
        components->public_exponent[2] != 0x01u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    is_public = component_absent(components->private_exponent, components->private_exponent_len) &&
                component_absent(components->prime1, components->prime1_len) &&
                component_absent(components->prime2, components->prime2_len) &&
                component_absent(components->exponent1, components->exponent1_len) &&
                component_absent(components->exponent2, components->exponent2_len) &&
                component_absent(components->coefficient, components->coefficient_len);

    bn_from_be(out_key->n, ANO_CRYPTO_RSA_LIMBS, components->modulus, components->modulus_len);
    out_key->n0_inv = mont_n0_inv(out_key->n[0]);
    mont_compute_rr(out_key->n_rr, out_key->n, ANO_CRYPTO_RSA_LIMBS);
    out_key->modulus_bits = 2048u;

    if (is_public) {
        out_key->kind = ANO_CRYPTO_RSA_KEY_KIND_PUBLIC;
        *out_type = ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY;
        return ANO_CRYPTO_OK;
    }

    if (!component_exact(components->private_exponent, components->private_exponent_len, 256u) ||
        !component_exact(components->prime1, components->prime1_len, 128u) ||
        !component_exact(components->prime2, components->prime2_len, 128u) ||
        !component_exact(components->exponent1, components->exponent1_len, 128u) ||
        !component_exact(components->exponent2, components->exponent2_len, 128u) ||
        !component_exact(components->coefficient, components->coefficient_len, 128u)) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }
    if (components->prime1[0] < 0x80u || components->prime2[0] < 0x80u ||
        (components->prime1[127] & 1u) == 0u || (components->prime2[127] & 1u) == 0u) {
        return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
    }

    bn_from_be(d, RSA_MAX_LIMBS, components->private_exponent, 256u);
    bn_from_be(out_key->p, ANO_CRYPTO_RSA_HALF_LIMBS, components->prime1, 128u);
    bn_from_be(out_key->q, ANO_CRYPTO_RSA_HALF_LIMBS, components->prime2, 128u);
    bn_from_be(out_key->dp, ANO_CRYPTO_RSA_HALF_LIMBS, components->exponent1, 128u);
    bn_from_be(out_key->dq, ANO_CRYPTO_RSA_HALF_LIMBS, components->exponent2, 128u);
    bn_from_be(out_key->qinv, ANO_CRYPTO_RSA_HALF_LIMBS, components->coefficient, 128u);

    if (bn_cmp(out_key->p, out_key->q, ANO_CRYPTO_RSA_HALF_LIMBS) == 0) {
        goto invalid_private;
    }
    bn_mul(out_key->p, ANO_CRYPTO_RSA_HALF_LIMBS,
           out_key->q, ANO_CRYPTO_RSA_HALF_LIMBS,
           product, RSA_MAX_LIMBS);
    if (bn_cmp(product, out_key->n, RSA_MAX_LIMBS) != 0) {
        goto invalid_private;
    }

    bn_copy(pm1, out_key->p, ANO_CRYPTO_RSA_HALF_LIMBS);
    bn_copy(qm1, out_key->q, ANO_CRYPTO_RSA_HALF_LIMBS);
    pm1[0] -= 1u;
    qm1[0] -= 1u;
    bn_mod_reduce(reduced, ANO_CRYPTO_RSA_HALF_LIMBS, d, RSA_MAX_LIMBS, pm1);
    if (bn_cmp(reduced, out_key->dp, ANO_CRYPTO_RSA_HALF_LIMBS) != 0) {
        goto invalid_private;
    }
    bn_mod_reduce(reduced, ANO_CRYPTO_RSA_HALF_LIMBS, d, RSA_MAX_LIMBS, qm1);
    if (bn_cmp(reduced, out_key->dq, ANO_CRYPTO_RSA_HALF_LIMBS) != 0) {
        goto invalid_private;
    }

    /* Reject algebraically inconsistent private components at import rather
     * than allowing the first private operation to trip the sticky fault gate. */
    {
        const uint32_t public_exponent[1] = {ANO_CRYPTO_RSA_PUBLIC_EXPONENT};
        bn_mul(out_key->dp, ANO_CRYPTO_RSA_HALF_LIMBS,
               public_exponent, 1u, temp_product, RSA_MAX_LIMBS);
        bn_mod_reduce(reduced, ANO_CRYPTO_RSA_HALF_LIMBS,
                      temp_product, ANO_CRYPTO_RSA_HALF_LIMBS + 1u, pm1);
        if (!bn_is_one(reduced, ANO_CRYPTO_RSA_HALF_LIMBS)) {
            goto invalid_private;
        }
        bn_mul(out_key->dq, ANO_CRYPTO_RSA_HALF_LIMBS,
               public_exponent, 1u, temp_product, RSA_MAX_LIMBS);
        bn_mod_reduce(reduced, ANO_CRYPTO_RSA_HALF_LIMBS,
                      temp_product, ANO_CRYPTO_RSA_HALF_LIMBS + 1u, qm1);
        if (!bn_is_one(reduced, ANO_CRYPTO_RSA_HALF_LIMBS)) {
            goto invalid_private;
        }
    }

    bn_mul(out_key->q, ANO_CRYPTO_RSA_HALF_LIMBS,
           out_key->qinv, ANO_CRYPTO_RSA_HALF_LIMBS,
           temp_product, RSA_MAX_LIMBS);
    bn_mod_reduce(reduced, ANO_CRYPTO_RSA_HALF_LIMBS,
                  temp_product, RSA_MAX_LIMBS, out_key->p);
    if (!bn_is_one(reduced, ANO_CRYPTO_RSA_HALF_LIMBS)) {
        goto invalid_private;
    }

    out_key->p0_inv = mont_n0_inv(out_key->p[0]);
    out_key->q0_inv = mont_n0_inv(out_key->q[0]);
    mont_compute_rr(out_key->p_rr, out_key->p, ANO_CRYPTO_RSA_HALF_LIMBS);
    mont_compute_rr(out_key->q_rr, out_key->q, ANO_CRYPTO_RSA_HALF_LIMBS);
    out_key->kind = ANO_CRYPTO_RSA_KEY_KIND_PRIVATE;
    *out_type = ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY;
    ano_crypto_internal_zeroize(d, sizeof(d));
    ano_crypto_internal_zeroize(product, sizeof(product));
    ano_crypto_internal_zeroize(temp_product, sizeof(temp_product));
    ano_crypto_internal_zeroize(pm1, sizeof(pm1));
    ano_crypto_internal_zeroize(qm1, sizeof(qm1));
    ano_crypto_internal_zeroize(reduced, sizeof(reduced));
    return ANO_CRYPTO_OK;

invalid_private:
    ano_crypto_internal_zeroize(d, sizeof(d));
    ano_crypto_internal_zeroize(product, sizeof(product));
    ano_crypto_internal_zeroize(temp_product, sizeof(temp_product));
    ano_crypto_internal_zeroize(pm1, sizeof(pm1));
    ano_crypto_internal_zeroize(qm1, sizeof(qm1));
    ano_crypto_internal_zeroize(reduced, sizeof(reduced));
    ano_crypto_internal_zeroize(out_key, sizeof(*out_key));
    return ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
}

ano_crypto_status_t ano_crypto_internal_rsa_public(
    const ano_crypto_rsa_key_internal_t *key,
    const uint8_t input[256],
    uint8_t output[256])
{
    uint32_t in[RSA_MAX_LIMBS];
    uint32_t out[RSA_MAX_LIMBS];
    if (key == 0 || input == 0 || output == 0 ||
        (key->kind != ANO_CRYPTO_RSA_KEY_KIND_PUBLIC &&
         key->kind != ANO_CRYPTO_RSA_KEY_KIND_PRIVATE)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    bn_from_be(in, RSA_MAX_LIMBS, input, 256u);
    if (bn_cmp(in, key->n, RSA_MAX_LIMBS) >= 0) {
        ano_crypto_internal_zeroize(in, sizeof(in));
        return ANO_CRYPTO_ERR_AUTHENTICATION_FAILED;
    }
    public_exp(out, in, key->n, key->n_rr, key->n0_inv, RSA_MAX_LIMBS);
    bn_to_be(output, 256u, out, RSA_MAX_LIMBS);
    ano_crypto_internal_zeroize(in, sizeof(in));
    ano_crypto_internal_zeroize(out, sizeof(out));
    return ANO_CRYPTO_OK;
}

ano_crypto_status_t ano_crypto_internal_rsa_private(
    const ano_crypto_rsa_key_internal_t *key,
    const uint8_t input[256],
    const uint8_t blind[8],
    uint8_t output[256])
{
    uint32_t m[RSA_MAX_LIMBS];
    uint32_t mp[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t mq[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t sp[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t sq[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t sq_mod_p[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t diff[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t diff_m[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t qinv_m[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t h_m[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t h[ANO_CRYPTO_RSA_HALF_LIMBS];
    uint32_t qh[RSA_MAX_LIMBS];
    uint32_t result[RSA_MAX_LIMBS];
    uint32_t check[RSA_MAX_LIMBS];
    uint32_t ep[ANO_CRYPTO_RSA_BLINDED_EXP_LIMBS];
    uint32_t eq[ANO_CRYPTO_RSA_BLINDED_EXP_LIMBS];
    uint32_t k1;
    uint32_t k2;
    uint32_t borrow;
    ano_crypto_status_t st = ANO_CRYPTO_OK;

    if (key == 0 || input == 0 || blind == 0 || output == 0 ||
        key->kind != ANO_CRYPTO_RSA_KEY_KIND_PRIVATE) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    bn_from_be(m, RSA_MAX_LIMBS, input, 256u);
    if (bn_cmp(m, key->n, RSA_MAX_LIMBS) >= 0) {
        st = ANO_CRYPTO_ERR_UNSUPPORTED_PARAMETER;
        goto cleanup;
    }
    k1 = ((uint32_t)blind[0] << 24) | ((uint32_t)blind[1] << 16) |
         ((uint32_t)blind[2] << 8) | (uint32_t)blind[3];
    k2 = ((uint32_t)blind[4] << 24) | ((uint32_t)blind[5] << 16) |
         ((uint32_t)blind[6] << 8) | (uint32_t)blind[7];
    k1 |= 1u;
    k2 |= 1u;
    scalar_blind_exponent(ep, key->dp, key->p, k1);
    scalar_blind_exponent(eq, key->dq, key->q, k2);

    bn_mod_reduce(mp, ANO_CRYPTO_RSA_HALF_LIMBS, m, RSA_MAX_LIMBS, key->p);
    bn_mod_reduce(mq, ANO_CRYPTO_RSA_HALF_LIMBS, m, RSA_MAX_LIMBS, key->q);
    mont_exp(sp, mp, ep, ANO_CRYPTO_RSA_BLINDED_EXP_LIMBS * 32u,
             key->p, key->p_rr, key->p0_inv, ANO_CRYPTO_RSA_HALF_LIMBS);
    mont_exp(sq, mq, eq, ANO_CRYPTO_RSA_BLINDED_EXP_LIMBS * 32u,
             key->q, key->q_rr, key->q0_inv, ANO_CRYPTO_RSA_HALF_LIMBS);

    bn_mod_reduce(sq_mod_p, ANO_CRYPTO_RSA_HALF_LIMBS,
                  sq, ANO_CRYPTO_RSA_HALF_LIMBS, key->p);
    borrow = bn_sub(diff, sp, sq_mod_p, ANO_CRYPTO_RSA_HALF_LIMBS);
    if (borrow != 0u) {
        (void)bn_add(diff, diff, key->p, ANO_CRYPTO_RSA_HALF_LIMBS);
    }
    mont_to(diff_m, diff, key->p, key->p_rr, key->p0_inv, ANO_CRYPTO_RSA_HALF_LIMBS);
    mont_to(qinv_m, key->qinv, key->p, key->p_rr, key->p0_inv, ANO_CRYPTO_RSA_HALF_LIMBS);
    mont_mul(h_m, diff_m, qinv_m, key->p, key->p0_inv, ANO_CRYPTO_RSA_HALF_LIMBS);
    mont_from(h, h_m, key->p, key->p0_inv, ANO_CRYPTO_RSA_HALF_LIMBS);
    bn_mul(key->q, ANO_CRYPTO_RSA_HALF_LIMBS, h, ANO_CRYPTO_RSA_HALF_LIMBS,
           qh, RSA_MAX_LIMBS);
    bn_copy(result, qh, RSA_MAX_LIMBS);
    bn_add_low(result, RSA_MAX_LIMBS, sq, ANO_CRYPTO_RSA_HALF_LIMBS);

    public_exp(check, result, key->n, key->n_rr, key->n0_inv, RSA_MAX_LIMBS);
    if (bn_cmp(check, m, RSA_MAX_LIMBS) != 0) {
        st = ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED;
        goto cleanup;
    }
    bn_to_be(output, 256u, result, RSA_MAX_LIMBS);

cleanup:
    ano_crypto_internal_zeroize(m, sizeof(m));
    ano_crypto_internal_zeroize(mp, sizeof(mp));
    ano_crypto_internal_zeroize(mq, sizeof(mq));
    ano_crypto_internal_zeroize(sp, sizeof(sp));
    ano_crypto_internal_zeroize(sq, sizeof(sq));
    ano_crypto_internal_zeroize(sq_mod_p, sizeof(sq_mod_p));
    ano_crypto_internal_zeroize(diff, sizeof(diff));
    ano_crypto_internal_zeroize(diff_m, sizeof(diff_m));
    ano_crypto_internal_zeroize(qinv_m, sizeof(qinv_m));
    ano_crypto_internal_zeroize(h_m, sizeof(h_m));
    ano_crypto_internal_zeroize(h, sizeof(h));
    ano_crypto_internal_zeroize(qh, sizeof(qh));
    ano_crypto_internal_zeroize(result, sizeof(result));
    ano_crypto_internal_zeroize(check, sizeof(check));
    ano_crypto_internal_zeroize(ep, sizeof(ep));
    ano_crypto_internal_zeroize(eq, sizeof(eq));
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(output, 256u);
    }
    return st;
}
