#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "ac_ecdsa_internal.h"
#include "ac_hmac_internal.h"
#include "ac_internal.h"
#include "ac_sha256_internal.h"

#define P256_LIMBS ANO_CRYPTO_EC_P256_LIMBS
#define P256_PRODUCT_LIMBS (2u * P256_LIMBS + 2u)
#define P256_SCALAR_BITS 256u

typedef struct p256_mod_ctx_s {
    const uint32_t *mod;
    const uint32_t *rr;
    const uint32_t *one;
    const uint32_t *minus_two;
    uint32_t n0_inv;
} p256_mod_ctx_t;

typedef struct p256_affine_s {
    uint32_t x[P256_LIMBS];
    uint32_t y[P256_LIMBS];
} p256_affine_t;

typedef struct p256_jacobian_s {
    uint32_t x[P256_LIMBS];
    uint32_t y[P256_LIMBS];
    uint32_t z[P256_LIMBS];
} p256_jacobian_t;

typedef struct rfc6979_state_s {
    uint8_t k[32];
    uint8_t v[32];
} rfc6979_state_t;

static const uint32_t k_p[P256_LIMBS] = {
    0xffffffffu, 0xffffffffu, 0xffffffffu, 0x00000000u,
    0x00000000u, 0x00000000u, 0x00000001u, 0xffffffffu
};
static const uint32_t k_p_rr[P256_LIMBS] = {
    0x00000003u, 0x00000000u, 0xffffffffu, 0xfffffffbu,
    0xfffffffeu, 0xffffffffu, 0xfffffffdu, 0x00000004u
};
static const uint32_t k_p_one[P256_LIMBS] = {
    0x00000001u, 0x00000000u, 0x00000000u, 0xffffffffu,
    0xffffffffu, 0xffffffffu, 0xfffffffeu, 0x00000000u
};
static const uint32_t k_p_minus_two[P256_LIMBS] = {
    0xfffffffdu, 0xffffffffu, 0xffffffffu, 0x00000000u,
    0x00000000u, 0x00000000u, 0x00000001u, 0xffffffffu
};
static const uint32_t k_n[P256_LIMBS] = {
    0xfc632551u, 0xf3b9cac2u, 0xa7179e84u, 0xbce6faadu,
    0xffffffffu, 0xffffffffu, 0x00000000u, 0xffffffffu
};
static const uint32_t k_n_rr[P256_LIMBS] = {
    0xbe79eea2u, 0x83244c95u, 0x49bd6fa6u, 0x4699799cu,
    0x2b6bec59u, 0x2845b239u, 0xf3d95620u, 0x66e12d94u
};
static const uint32_t k_n_one[P256_LIMBS] = {
    0x039cdaafu, 0x0c46353du, 0x58e8617bu, 0x43190552u,
    0x00000000u, 0x00000000u, 0xffffffffu, 0x00000000u
};
static const uint32_t k_n_minus_two[P256_LIMBS] = {
    0xfc63254fu, 0xf3b9cac2u, 0xa7179e84u, 0xbce6faadu,
    0xffffffffu, 0xffffffffu, 0x00000000u, 0xffffffffu
};
static const uint32_t k_curve_b_mont[P256_LIMBS] = {
    0x29c4bddfu, 0xd89cdf62u, 0x78843090u, 0xacf005cdu,
    0xf7212ed6u, 0xe5a220abu, 0x04874834u, 0xdc30061du
};
static const p256_affine_t k_generator_mont = {
    {0x18a9143cu, 0x79e730d4u, 0x5fedb601u, 0x75ba95fcu,
     0x77622510u, 0x79fb732bu, 0xa53755c6u, 0x18905f76u},
    {0xce95560au, 0xddf25357u, 0xba19e45cu, 0x8b4ab8e4u,
     0xdd21f325u, 0xd2e88688u, 0x25885d85u, 0x8571ff18u}
};

static const p256_mod_ctx_t k_field_ctx = {
    k_p, k_p_rr, k_p_one, k_p_minus_two, 0x00000001u
};
static const p256_mod_ctx_t k_order_ctx = {
    k_n, k_n_rr, k_n_one, k_n_minus_two, 0xee00bc4fu
};

static void bn_zero(uint32_t *a)
{
    ano_crypto_internal_zeroize(a, P256_LIMBS * sizeof(*a));
}

static void bn_copy(uint32_t *out, const uint32_t *in)
{
    memcpy(out, in, P256_LIMBS * sizeof(*out));
}

static int bn_cmp(const uint32_t *a, const uint32_t *b)
{
    size_t i = P256_LIMBS;
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

static uint32_t bn_add(uint32_t *out, const uint32_t *a, const uint32_t *b)
{
    uint64_t carry = 0u;
    size_t i;
    for (i = 0u; i < P256_LIMBS; ++i) {
        uint64_t z = (uint64_t)a[i] + (uint64_t)b[i] + carry;
        out[i] = (uint32_t)z;
        carry = z >> 32;
    }
    return (uint32_t)carry;
}

static uint32_t bn_sub(uint32_t *out, const uint32_t *a, const uint32_t *b)
{
    uint64_t borrow = 0u;
    size_t i;
    for (i = 0u; i < P256_LIMBS; ++i) {
        uint64_t difference = (uint64_t)a[i] - (uint64_t)b[i] - borrow;
        out[i] = (uint32_t)difference;
        borrow = difference >> 63;
    }
    return (uint32_t)borrow;
}

static uint32_t bn_is_zero(const uint32_t *a)
{
    uint32_t value = 0u;
    size_t i;
    for (i = 0u; i < P256_LIMBS; ++i) {
        value |= a[i];
    }
    value |= (uint32_t)(0u - value);
    return (value >> 31) ^ 1u;
}

static void bn_select(uint32_t *out, const uint32_t *a, const uint32_t *b, uint32_t choose_b)
{
    uint32_t mask = (uint32_t)(0u - (choose_b & 1u));
    size_t i;
    for (i = 0u; i < P256_LIMBS; ++i) {
        out[i] = (a[i] & ~mask) | (b[i] & mask);
    }
}

static uint32_t ct_is_nonzero_u32(uint32_t value)
{
    return (value | (uint32_t)(0u - value)) >> 31;
}

static uint32_t bn_less_than(const uint32_t *a, const uint32_t *b)
{
    uint32_t difference[P256_LIMBS];
    uint32_t borrow = bn_sub(difference, a, b);
    ano_crypto_internal_zeroize(difference, sizeof(difference));
    return borrow;
}

static uint32_t bn_equal(const uint32_t *a, const uint32_t *b)
{
    uint32_t difference = 0u;
    size_t i;
    for (i = 0u; i < P256_LIMBS; ++i) {
        difference |= a[i] ^ b[i];
    }
    return ct_is_nonzero_u32(difference) ^ 1u;
}

static void bn_from_be(uint32_t *out, const uint8_t in[32])
{
    size_t i;
    bn_zero(out);
    for (i = 0u; i < 32u; ++i) {
        size_t reverse = 31u - i;
        out[i / 4u] |= (uint32_t)in[reverse] << ((i % 4u) * 8u);
    }
}

static void bn_to_be(uint8_t out[32], const uint32_t *in)
{
    size_t i;
    for (i = 0u; i < 32u; ++i) {
        size_t reverse = 31u - i;
        out[reverse] = (uint8_t)(in[i / 4u] >> ((i % 4u) * 8u));
    }
}

static void mont_mul(uint32_t *out, const uint32_t *a, const uint32_t *b,
                     const p256_mod_ctx_t *ctx)
{
    uint32_t t[P256_PRODUCT_LIMBS];
    size_t i;
    ano_crypto_internal_zeroize(t, sizeof(t));

    for (i = 0u; i < P256_LIMBS; ++i) {
        uint64_t carry = 0u;
        size_t j;
        for (j = 0u; j < P256_LIMBS; ++j) {
            uint64_t z = (uint64_t)a[i] * (uint64_t)b[j] +
                         (uint64_t)t[i + j] + carry;
            t[i + j] = (uint32_t)z;
            carry = z >> 32;
        }
        for (j = i + P256_LIMBS; j < P256_PRODUCT_LIMBS; ++j) {
            uint64_t z = (uint64_t)t[j] + carry;
            t[j] = (uint32_t)z;
            carry = z >> 32;
        }
    }

    for (i = 0u; i < P256_LIMBS; ++i) {
        uint32_t m = t[i] * ctx->n0_inv;
        uint64_t carry = 0u;
        size_t j;
        for (j = 0u; j < P256_LIMBS; ++j) {
            uint64_t z = (uint64_t)m * (uint64_t)ctx->mod[j] +
                         (uint64_t)t[i + j] + carry;
            t[i + j] = (uint32_t)z;
            carry = z >> 32;
        }
        for (j = i + P256_LIMBS; j < P256_PRODUCT_LIMBS; ++j) {
            uint64_t z = (uint64_t)t[j] + carry;
            t[j] = (uint32_t)z;
            carry = z >> 32;
        }
    }

    {
        uint32_t candidate[P256_LIMBS];
        uint32_t borrow;
        uint32_t choose_reduced;
        bn_copy(out, t + P256_LIMBS);
        borrow = bn_sub(candidate, out, ctx->mod);
        choose_reduced = ct_is_nonzero_u32(t[2u * P256_LIMBS]) | (borrow ^ 1u);
        bn_select(out, out, candidate, choose_reduced);
        ano_crypto_internal_zeroize(candidate, sizeof(candidate));
    }
    ano_crypto_internal_zeroize(t, sizeof(t));
}

static void mod_add(uint32_t *out, const uint32_t *a, const uint32_t *b,
                    const p256_mod_ctx_t *ctx)
{
    uint32_t sum[P256_LIMBS];
    uint32_t reduced[P256_LIMBS];
    uint32_t carry = bn_add(sum, a, b);
    uint32_t borrow = bn_sub(reduced, sum, ctx->mod);
    uint32_t choose_reduced = carry | (borrow ^ 1u);
    bn_select(out, sum, reduced, choose_reduced);
    ano_crypto_internal_zeroize(sum, sizeof(sum));
    ano_crypto_internal_zeroize(reduced, sizeof(reduced));
}

static void mod_sub(uint32_t *out, const uint32_t *a, const uint32_t *b,
                    const p256_mod_ctx_t *ctx)
{
    uint32_t diff[P256_LIMBS];
    uint32_t adjusted[P256_LIMBS];
    uint32_t borrow = bn_sub(diff, a, b);
    (void)bn_add(adjusted, diff, ctx->mod);
    bn_select(out, diff, adjusted, borrow);
    ano_crypto_internal_zeroize(diff, sizeof(diff));
    ano_crypto_internal_zeroize(adjusted, sizeof(adjusted));
}

static void mod_mul_small(uint32_t *out, const uint32_t *a, unsigned factor,
                          const p256_mod_ctx_t *ctx)
{
    uint32_t acc[P256_LIMBS];
    uint32_t current[P256_LIMBS];
    unsigned value = factor;
    bn_zero(acc);
    bn_copy(current, a);
    while (value != 0u) {
        if ((value & 1u) != 0u) {
            mod_add(acc, acc, current, ctx);
        }
        value >>= 1;
        if (value != 0u) {
            mod_add(current, current, current, ctx);
        }
    }
    bn_copy(out, acc);
    ano_crypto_internal_zeroize(acc, sizeof(acc));
    ano_crypto_internal_zeroize(current, sizeof(current));
}

static void mod_to_mont(uint32_t *out, const uint32_t *normal, const p256_mod_ctx_t *ctx)
{
    mont_mul(out, normal, ctx->rr, ctx);
}

static void mod_from_mont(uint32_t *out, const uint32_t *mont, const p256_mod_ctx_t *ctx)
{
    uint32_t one[P256_LIMBS];
    bn_zero(one);
    one[0] = 1u;
    mont_mul(out, mont, one, ctx);
    ano_crypto_internal_zeroize(one, sizeof(one));
}

static void mod_inverse_mont(uint32_t *out, const uint32_t *a, const p256_mod_ctx_t *ctx)
{
    uint32_t result[P256_LIMBS];
    uint32_t squared[P256_LIMBS];
    uint32_t multiplied[P256_LIMBS];
    size_t bit = P256_SCALAR_BITS;
    bn_copy(result, ctx->one);
    while (bit > 0u) {
        uint32_t exponent_bit;
        --bit;
        mont_mul(squared, result, result, ctx);
        mont_mul(multiplied, squared, a, ctx);
        exponent_bit = (ctx->minus_two[bit / 32u] >> (bit % 32u)) & 1u;
        bn_select(result, squared, multiplied, exponent_bit);
    }
    bn_copy(out, result);
    ano_crypto_internal_zeroize(result, sizeof(result));
    ano_crypto_internal_zeroize(squared, sizeof(squared));
    ano_crypto_internal_zeroize(multiplied, sizeof(multiplied));
}

static void reduce_once(uint32_t *value, const uint32_t *mod)
{
    uint32_t reduced[P256_LIMBS];
    uint32_t borrow = bn_sub(reduced, value, mod);
    bn_select(value, value, reduced, borrow ^ 1u);
    ano_crypto_internal_zeroize(reduced, sizeof(reduced));
}

static void point_set_infinity(p256_jacobian_t *point)
{
    ano_crypto_internal_zeroize(point, sizeof(*point));
}

static void point_from_affine(p256_jacobian_t *out, const p256_affine_t *in)
{
    bn_copy(out->x, in->x);
    bn_copy(out->y, in->y);
    bn_copy(out->z, k_field_ctx.one);
}

static void point_select(p256_jacobian_t *out,
                         const p256_jacobian_t *a,
                         const p256_jacobian_t *b,
                         uint32_t choose_b)
{
    bn_select(out->x, a->x, b->x, choose_b);
    bn_select(out->y, a->y, b->y, choose_b);
    bn_select(out->z, a->z, b->z, choose_b);
}

static void point_double(p256_jacobian_t *out, const p256_jacobian_t *in)
{
    uint32_t delta[P256_LIMBS];
    uint32_t gamma[P256_LIMBS];
    uint32_t beta[P256_LIMBS];
    uint32_t alpha[P256_LIMBS];
    uint32_t t1[P256_LIMBS];
    uint32_t t2[P256_LIMBS];
    uint32_t gamma2[P256_LIMBS];

    mont_mul(delta, in->z, in->z, &k_field_ctx);
    mont_mul(gamma, in->y, in->y, &k_field_ctx);
    mont_mul(beta, in->x, gamma, &k_field_ctx);
    mod_sub(t1, in->x, delta, &k_field_ctx);
    mod_add(t2, in->x, delta, &k_field_ctx);
    mont_mul(alpha, t1, t2, &k_field_ctx);
    mod_mul_small(alpha, alpha, 3u, &k_field_ctx);

    mont_mul(t1, alpha, alpha, &k_field_ctx);
    mod_mul_small(t2, beta, 8u, &k_field_ctx);
    mod_sub(out->x, t1, t2, &k_field_ctx);

    mod_add(t1, in->y, in->z, &k_field_ctx);
    mont_mul(t1, t1, t1, &k_field_ctx);
    mod_sub(t1, t1, gamma, &k_field_ctx);
    mod_sub(out->z, t1, delta, &k_field_ctx);

    mod_mul_small(t1, beta, 4u, &k_field_ctx);
    mod_sub(t1, t1, out->x, &k_field_ctx);
    mont_mul(t1, alpha, t1, &k_field_ctx);
    mont_mul(gamma2, gamma, gamma, &k_field_ctx);
    mod_mul_small(t2, gamma2, 8u, &k_field_ctx);
    mod_sub(out->y, t1, t2, &k_field_ctx);

    ano_crypto_internal_zeroize(delta, sizeof(delta));
    ano_crypto_internal_zeroize(gamma, sizeof(gamma));
    ano_crypto_internal_zeroize(beta, sizeof(beta));
    ano_crypto_internal_zeroize(alpha, sizeof(alpha));
    ano_crypto_internal_zeroize(t1, sizeof(t1));
    ano_crypto_internal_zeroize(t2, sizeof(t2));
    ano_crypto_internal_zeroize(gamma2, sizeof(gamma2));
}

static void point_mixed_add_complete(p256_jacobian_t *out,
                                     const p256_jacobian_t *left,
                                     const p256_affine_t *right)
{
    uint32_t z1z1[P256_LIMBS];
    uint32_t u2[P256_LIMBS];
    uint32_t s2[P256_LIMBS];
    uint32_t h[P256_LIMBS];
    uint32_t hh[P256_LIMBS];
    uint32_t i4[P256_LIMBS];
    uint32_t j[P256_LIMBS];
    uint32_t r2[P256_LIMBS];
    uint32_t v[P256_LIMBS];
    uint32_t t1[P256_LIMBS];
    uint32_t t2[P256_LIMBS];
    p256_jacobian_t standard;
    p256_jacobian_t doubled;
    p256_jacobian_t infinity;
    p256_jacobian_t right_j;
    p256_jacobian_t selected;
    uint32_t left_infinity;
    uint32_t h_zero;
    uint32_t r_zero;

    mont_mul(z1z1, left->z, left->z, &k_field_ctx);
    mont_mul(u2, right->x, z1z1, &k_field_ctx);
    mont_mul(t1, left->z, z1z1, &k_field_ctx);
    mont_mul(s2, right->y, t1, &k_field_ctx);
    mod_sub(h, u2, left->x, &k_field_ctx);
    mont_mul(hh, h, h, &k_field_ctx);
    mod_mul_small(i4, hh, 4u, &k_field_ctx);
    mont_mul(j, h, i4, &k_field_ctx);
    mod_sub(r2, s2, left->y, &k_field_ctx);
    mod_mul_small(r2, r2, 2u, &k_field_ctx);
    mont_mul(v, left->x, i4, &k_field_ctx);

    mont_mul(t1, r2, r2, &k_field_ctx);
    mod_sub(t1, t1, j, &k_field_ctx);
    mod_mul_small(t2, v, 2u, &k_field_ctx);
    mod_sub(standard.x, t1, t2, &k_field_ctx);

    mod_sub(t1, v, standard.x, &k_field_ctx);
    mont_mul(t1, r2, t1, &k_field_ctx);
    mont_mul(t2, left->y, j, &k_field_ctx);
    mod_mul_small(t2, t2, 2u, &k_field_ctx);
    mod_sub(standard.y, t1, t2, &k_field_ctx);

    mod_add(t1, left->z, h, &k_field_ctx);
    mont_mul(t1, t1, t1, &k_field_ctx);
    mod_sub(t1, t1, z1z1, &k_field_ctx);
    mod_sub(standard.z, t1, hh, &k_field_ctx);

    point_double(&doubled, left);
    point_set_infinity(&infinity);
    point_from_affine(&right_j, right);

    left_infinity = bn_is_zero(left->z);
    h_zero = bn_is_zero(h);
    r_zero = bn_is_zero(r2);

    point_select(&selected, &standard, &doubled, h_zero & r_zero);
    point_select(&selected, &selected, &infinity, h_zero & (r_zero ^ 1u));
    point_select(out, &selected, &right_j, left_infinity);

    ano_crypto_internal_zeroize(z1z1, sizeof(z1z1));
    ano_crypto_internal_zeroize(u2, sizeof(u2));
    ano_crypto_internal_zeroize(s2, sizeof(s2));
    ano_crypto_internal_zeroize(h, sizeof(h));
    ano_crypto_internal_zeroize(hh, sizeof(hh));
    ano_crypto_internal_zeroize(i4, sizeof(i4));
    ano_crypto_internal_zeroize(j, sizeof(j));
    ano_crypto_internal_zeroize(r2, sizeof(r2));
    ano_crypto_internal_zeroize(v, sizeof(v));
    ano_crypto_internal_zeroize(t1, sizeof(t1));
    ano_crypto_internal_zeroize(t2, sizeof(t2));
    ano_crypto_internal_zeroize(&standard, sizeof(standard));
    ano_crypto_internal_zeroize(&doubled, sizeof(doubled));
    ano_crypto_internal_zeroize(&infinity, sizeof(infinity));
    ano_crypto_internal_zeroize(&right_j, sizeof(right_j));
    ano_crypto_internal_zeroize(&selected, sizeof(selected));
}

static void point_scalar_multiply(p256_jacobian_t *out,
                                  const uint32_t scalar[P256_LIMBS],
                                  const p256_affine_t *point)
{
    p256_jacobian_t accumulator;
    p256_jacobian_t doubled;
    p256_jacobian_t added;
    size_t bit = P256_SCALAR_BITS;
    point_set_infinity(&accumulator);
    while (bit > 0u) {
        uint32_t scalar_bit;
        --bit;
        point_double(&doubled, &accumulator);
        point_mixed_add_complete(&added, &doubled, point);
        scalar_bit = (scalar[bit / 32u] >> (bit % 32u)) & 1u;
        point_select(&accumulator, &doubled, &added, scalar_bit);
    }
    memcpy(out, &accumulator, sizeof(*out));
    ano_crypto_internal_zeroize(&accumulator, sizeof(accumulator));
    ano_crypto_internal_zeroize(&doubled, sizeof(doubled));
    ano_crypto_internal_zeroize(&added, sizeof(added));
}

static ano_crypto_status_t point_to_affine(p256_affine_t *out, const p256_jacobian_t *in)
{
    uint32_t z_inv[P256_LIMBS];
    uint32_t z2[P256_LIMBS];
    uint32_t z3[P256_LIMBS];
    if (bn_is_zero(in->z) != 0u) {
        return ANO_CRYPTO_ERR_AUTHENTICATION_FAILED;
    }
    mod_inverse_mont(z_inv, in->z, &k_field_ctx);
    mont_mul(z2, z_inv, z_inv, &k_field_ctx);
    mont_mul(z3, z2, z_inv, &k_field_ctx);
    mont_mul(out->x, in->x, z2, &k_field_ctx);
    mont_mul(out->y, in->y, z3, &k_field_ctx);
    ano_crypto_internal_zeroize(z_inv, sizeof(z_inv));
    ano_crypto_internal_zeroize(z2, sizeof(z2));
    ano_crypto_internal_zeroize(z3, sizeof(z3));
    return ANO_CRYPTO_OK;
}

static int affine_is_on_curve(const p256_affine_t *point)
{
    uint32_t y2[P256_LIMBS];
    uint32_t x2[P256_LIMBS];
    uint32_t x3[P256_LIMBS];
    uint32_t three_x[P256_LIMBS];
    uint32_t rhs[P256_LIMBS];
    int result;
    mont_mul(y2, point->y, point->y, &k_field_ctx);
    mont_mul(x2, point->x, point->x, &k_field_ctx);
    mont_mul(x3, x2, point->x, &k_field_ctx);
    mod_mul_small(three_x, point->x, 3u, &k_field_ctx);
    mod_sub(rhs, x3, three_x, &k_field_ctx);
    mod_add(rhs, rhs, k_curve_b_mont, &k_field_ctx);
    result = bn_cmp(y2, rhs) == 0 ? 1 : 0;
    ano_crypto_internal_zeroize(y2, sizeof(y2));
    ano_crypto_internal_zeroize(x2, sizeof(x2));
    ano_crypto_internal_zeroize(x3, sizeof(x3));
    ano_crypto_internal_zeroize(three_x, sizeof(three_x));
    ano_crypto_internal_zeroize(rhs, sizeof(rhs));
    return result;
}

static void affine_from_normal(p256_affine_t *out, const uint32_t *x, const uint32_t *y)
{
    mod_to_mont(out->x, x, &k_field_ctx);
    mod_to_mont(out->y, y, &k_field_ctx);
}

static void affine_to_normal(uint32_t *x, uint32_t *y, const p256_affine_t *point)
{
    mod_from_mont(x, point->x, &k_field_ctx);
    mod_from_mont(y, point->y, &k_field_ctx);
}

static ano_crypto_status_t hmac_sha256(const uint8_t key[32],
                                       const uint8_t *input,
                                       size_t input_len,
                                       uint8_t out[32])
{
    return ano_crypto_internal_hmac_sha256_compute(key, 32u, input, input_len, out);
}

static ano_crypto_status_t rfc6979_reject(rfc6979_state_t *state)
{
    uint8_t input[33];
    uint8_t next_k[32];
    uint8_t next_v[32];
    ano_crypto_status_t st;
    memcpy(input, state->v, 32u);
    input[32] = 0x00u;
    st = hmac_sha256(state->k, input, sizeof(input), next_k);
    if (st == ANO_CRYPTO_OK) {
        st = hmac_sha256(next_k, state->v, sizeof(state->v), next_v);
    }
    if (st == ANO_CRYPTO_OK) {
        memcpy(state->k, next_k, sizeof(state->k));
        memcpy(state->v, next_v, sizeof(state->v));
    }
    ano_crypto_internal_zeroize(input, sizeof(input));
    ano_crypto_internal_zeroize(next_k, sizeof(next_k));
    ano_crypto_internal_zeroize(next_v, sizeof(next_v));
    return st;
}

static ano_crypto_status_t rfc6979_init(rfc6979_state_t *state,
                                        const uint32_t d[P256_LIMBS],
                                        const uint32_t z[P256_LIMBS])
{
    uint8_t input[97];
    uint8_t d_bytes[32];
    uint8_t z_bytes[32];
    uint8_t next_k[32];
    uint8_t next_v[32];
    ano_crypto_status_t st;

    memset(state->k, 0x00, sizeof(state->k));
    memset(state->v, 0x01, sizeof(state->v));
    bn_to_be(d_bytes, d);
    bn_to_be(z_bytes, z);

    memcpy(input, state->v, 32u);
    input[32] = 0x00u;
    memcpy(input + 33u, d_bytes, 32u);
    memcpy(input + 65u, z_bytes, 32u);
    st = hmac_sha256(state->k, input, sizeof(input), next_k);
    if (st == ANO_CRYPTO_OK) {
        st = hmac_sha256(next_k, state->v, sizeof(state->v), next_v);
    }
    if (st == ANO_CRYPTO_OK) {
        memcpy(state->k, next_k, 32u);
        memcpy(state->v, next_v, 32u);
        memcpy(input, state->v, 32u);
        input[32] = 0x01u;
        memcpy(input + 33u, d_bytes, 32u);
        memcpy(input + 65u, z_bytes, 32u);
        st = hmac_sha256(state->k, input, sizeof(input), next_k);
    }
    if (st == ANO_CRYPTO_OK) {
        st = hmac_sha256(next_k, state->v, sizeof(state->v), next_v);
    }
    if (st == ANO_CRYPTO_OK) {
        memcpy(state->k, next_k, 32u);
        memcpy(state->v, next_v, 32u);
    }

    ano_crypto_internal_zeroize(input, sizeof(input));
    ano_crypto_internal_zeroize(d_bytes, sizeof(d_bytes));
    ano_crypto_internal_zeroize(z_bytes, sizeof(z_bytes));
    ano_crypto_internal_zeroize(next_k, sizeof(next_k));
    ano_crypto_internal_zeroize(next_v, sizeof(next_v));
    return st;
}

static ano_crypto_status_t rfc6979_next(rfc6979_state_t *state, uint32_t k[P256_LIMBS])
{
    uint8_t next_v[32];
    ano_crypto_status_t st;
    for (;;) {
        st = hmac_sha256(state->k, state->v, sizeof(state->v), next_v);
        if (st != ANO_CRYPTO_OK) {
            ano_crypto_internal_zeroize(next_v, sizeof(next_v));
            return st;
        }
        memcpy(state->v, next_v, sizeof(state->v));
        bn_from_be(k, state->v);
        if (bn_is_zero(k) == 0u && bn_less_than(k, k_n) != 0u) {
            ano_crypto_internal_zeroize(next_v, sizeof(next_v));
            return ANO_CRYPTO_OK;
        }
        st = rfc6979_reject(state);
        if (st != ANO_CRYPTO_OK) {
            ano_crypto_internal_zeroize(next_v, sizeof(next_v));
            return st;
        }
    }
}

static void order_mul_normal(uint32_t *out, const uint32_t *a, const uint32_t *b)
{
    uint32_t am[P256_LIMBS];
    uint32_t bm[P256_LIMBS];
    uint32_t product[P256_LIMBS];
    mod_to_mont(am, a, &k_order_ctx);
    mod_to_mont(bm, b, &k_order_ctx);
    mont_mul(product, am, bm, &k_order_ctx);
    mod_from_mont(out, product, &k_order_ctx);
    ano_crypto_internal_zeroize(am, sizeof(am));
    ano_crypto_internal_zeroize(bm, sizeof(bm));
    ano_crypto_internal_zeroize(product, sizeof(product));
}

static void order_inverse_normal(uint32_t *out, const uint32_t *a)
{
    uint32_t am[P256_LIMBS];
    uint32_t inverse[P256_LIMBS];
    mod_to_mont(am, a, &k_order_ctx);
    mod_inverse_mont(inverse, am, &k_order_ctx);
    mod_from_mont(out, inverse, &k_order_ctx);
    ano_crypto_internal_zeroize(am, sizeof(am));
    ano_crypto_internal_zeroize(inverse, sizeof(inverse));
}

ano_crypto_status_t ano_crypto_internal_ec_p256_validate_and_prepare(
    const uint8_t *public_x,
    size_t public_x_len,
    const uint8_t *public_y,
    size_t public_y_len,
    const uint8_t *private_scalar,
    size_t private_scalar_len,
    ano_crypto_ec_p256_key_internal_t *out_key,
    ano_crypto_key_type_t *out_type)
{
    uint32_t x[P256_LIMBS];
    uint32_t y[P256_LIMBS];
    uint32_t d[P256_LIMBS];
    p256_affine_t public_point;
    p256_jacobian_t derived_j;
    p256_affine_t derived;
    ano_crypto_status_t st = ANO_CRYPTO_ERR_INVALID_ARGUMENT;

    if (public_x == 0 || public_y == 0 || out_key == 0 || out_type == 0 ||
        public_x_len != 32u || public_y_len != 32u ||
        (private_scalar == 0 && private_scalar_len != 0u) ||
        (private_scalar != 0 && private_scalar_len != 32u)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    ano_crypto_internal_zeroize(out_key, sizeof(*out_key));
    *out_type = ANO_CRYPTO_KEY_TYPE_PUBLIC_KEY;
    bn_from_be(x, public_x);
    bn_from_be(y, public_y);
    if (bn_cmp(x, k_p) >= 0 || bn_cmp(y, k_p) >= 0) {
        goto cleanup;
    }
    affine_from_normal(&public_point, x, y);
    if (!affine_is_on_curve(&public_point)) {
        goto cleanup;
    }

    out_key->kind = ANO_CRYPTO_EC_P256_KEY_KIND_PUBLIC;
    bn_copy(out_key->x, x);
    bn_copy(out_key->y, y);
    if (private_scalar != 0) {
        bn_from_be(d, private_scalar);
        if (bn_is_zero(d) != 0u || bn_less_than(d, k_n) == 0u) {
            goto cleanup;
        }
        point_scalar_multiply(&derived_j, d, &k_generator_mont);
        st = point_to_affine(&derived, &derived_j);
        if (st != ANO_CRYPTO_OK || bn_equal(derived.x, public_point.x) == 0u ||
            bn_equal(derived.y, public_point.y) == 0u) {
            st = ANO_CRYPTO_ERR_INVALID_ARGUMENT;
            goto cleanup;
        }
        out_key->kind = ANO_CRYPTO_EC_P256_KEY_KIND_PRIVATE;
        bn_copy(out_key->d, d);
        *out_type = ANO_CRYPTO_KEY_TYPE_PRIVATE_KEY;
    }
    st = ANO_CRYPTO_OK;

cleanup:
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(out_key, sizeof(*out_key));
    }
    ano_crypto_internal_zeroize(x, sizeof(x));
    ano_crypto_internal_zeroize(y, sizeof(y));
    ano_crypto_internal_zeroize(d, sizeof(d));
    ano_crypto_internal_zeroize(&public_point, sizeof(public_point));
    ano_crypto_internal_zeroize(&derived_j, sizeof(derived_j));
    ano_crypto_internal_zeroize(&derived, sizeof(derived));
    return st;
}

ano_crypto_status_t ano_crypto_internal_ecdsa_p256_verify_digest(
    const ano_crypto_ec_p256_key_internal_t *key,
    const uint8_t digest[32],
    const uint8_t signature[64])
{
    uint32_t r[P256_LIMBS];
    uint32_t s[P256_LIMBS];
    uint32_t z[P256_LIMBS];
    uint32_t w[P256_LIMBS];
    uint32_t u1[P256_LIMBS];
    uint32_t u2[P256_LIMBS];
    uint32_t x_normal[P256_LIMBS];
    uint32_t y_normal[P256_LIMBS];
    p256_affine_t public_point;
    p256_jacobian_t p1;
    p256_jacobian_t p2;
    p256_affine_t p2_affine;
    p256_jacobian_t sum;
    p256_affine_t result;
    ano_crypto_status_t st = ANO_CRYPTO_ERR_AUTHENTICATION_FAILED;

    if (key == 0 || digest == 0 || signature == 0 ||
        (key->kind != ANO_CRYPTO_EC_P256_KEY_KIND_PUBLIC &&
         key->kind != ANO_CRYPTO_EC_P256_KEY_KIND_PRIVATE)) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    bn_from_be(r, signature);
    bn_from_be(s, signature + 32u);
    if (bn_is_zero(r) != 0u || bn_is_zero(s) != 0u ||
        bn_cmp(r, k_n) >= 0 || bn_cmp(s, k_n) >= 0) {
        goto cleanup;
    }
    bn_from_be(z, digest);
    reduce_once(z, k_n);
    order_inverse_normal(w, s);
    order_mul_normal(u1, z, w);
    order_mul_normal(u2, r, w);
    affine_from_normal(&public_point, key->x, key->y);
    point_scalar_multiply(&p1, u1, &k_generator_mont);
    point_scalar_multiply(&p2, u2, &public_point);
    if (point_to_affine(&p2_affine, &p2) != ANO_CRYPTO_OK) {
        goto cleanup;
    }
    point_mixed_add_complete(&sum, &p1, &p2_affine);
    if (point_to_affine(&result, &sum) != ANO_CRYPTO_OK) {
        goto cleanup;
    }
    affine_to_normal(x_normal, y_normal, &result);
    reduce_once(x_normal, k_n);
    st = bn_equal(x_normal, r) != 0u ? ANO_CRYPTO_OK : ANO_CRYPTO_ERR_AUTHENTICATION_FAILED;

cleanup:
    ano_crypto_internal_zeroize(r, sizeof(r));
    ano_crypto_internal_zeroize(s, sizeof(s));
    ano_crypto_internal_zeroize(z, sizeof(z));
    ano_crypto_internal_zeroize(w, sizeof(w));
    ano_crypto_internal_zeroize(u1, sizeof(u1));
    ano_crypto_internal_zeroize(u2, sizeof(u2));
    ano_crypto_internal_zeroize(x_normal, sizeof(x_normal));
    ano_crypto_internal_zeroize(y_normal, sizeof(y_normal));
    ano_crypto_internal_zeroize(&public_point, sizeof(public_point));
    ano_crypto_internal_zeroize(&p1, sizeof(p1));
    ano_crypto_internal_zeroize(&p2, sizeof(p2));
    ano_crypto_internal_zeroize(&p2_affine, sizeof(p2_affine));
    ano_crypto_internal_zeroize(&sum, sizeof(sum));
    ano_crypto_internal_zeroize(&result, sizeof(result));
    return st;
}

ano_crypto_status_t ano_crypto_internal_ecdsa_p256_sign_digest(
    const ano_crypto_ec_p256_key_internal_t *key,
    const uint8_t digest[32],
    uint8_t signature[64])
{
    uint32_t z[P256_LIMBS];
    uint32_t k[P256_LIMBS];
    uint32_t r[P256_LIMBS];
    uint32_t s[P256_LIMBS];
    uint32_t rd[P256_LIMBS];
    uint32_t sum[P256_LIMBS];
    uint32_t k_inv[P256_LIMBS];
    uint32_t x_normal[P256_LIMBS];
    uint32_t y_normal[P256_LIMBS];
    p256_jacobian_t point_j;
    p256_affine_t point;
    rfc6979_state_t nonce_state;
    ano_crypto_status_t st;
    unsigned attempt;

    if (key == 0 || digest == 0 || signature == 0 ||
        key->kind != ANO_CRYPTO_EC_P256_KEY_KIND_PRIVATE) {
        return ANO_CRYPTO_ERR_INVALID_ARGUMENT;
    }
    ano_crypto_internal_zeroize(signature, 64u);
    bn_from_be(z, digest);
    reduce_once(z, k_n);
    st = rfc6979_init(&nonce_state, key->d, z);
    if (st != ANO_CRYPTO_OK) {
        goto cleanup;
    }

    for (attempt = 0u; attempt < 16u; ++attempt) {
        st = rfc6979_next(&nonce_state, k);
        if (st != ANO_CRYPTO_OK) {
            goto cleanup;
        }
        point_scalar_multiply(&point_j, k, &k_generator_mont);
        st = point_to_affine(&point, &point_j);
        if (st != ANO_CRYPTO_OK) {
            goto cleanup;
        }
        affine_to_normal(x_normal, y_normal, &point);
        bn_copy(r, x_normal);
        reduce_once(r, k_n);
        if (bn_is_zero(r) != 0u) {
            st = rfc6979_reject(&nonce_state);
            if (st != ANO_CRYPTO_OK) {
                goto cleanup;
            }
            continue;
        }
        order_mul_normal(rd, r, key->d);
        {
            uint32_t carry = bn_add(sum, z, rd);
            uint32_t reduced[P256_LIMBS];
            uint32_t borrow = bn_sub(reduced, sum, k_n);
            bn_select(sum, sum, reduced, carry | (borrow ^ 1u));
            ano_crypto_internal_zeroize(reduced, sizeof(reduced));
        }
        order_inverse_normal(k_inv, k);
        order_mul_normal(s, k_inv, sum);
        if (bn_is_zero(s) != 0u) {
            st = rfc6979_reject(&nonce_state);
            if (st != ANO_CRYPTO_OK) {
                goto cleanup;
            }
            continue;
        }
        bn_to_be(signature, r);
        bn_to_be(signature + 32u, s);
        st = ano_crypto_internal_ecdsa_p256_verify_digest(key, digest, signature);
        if (st != ANO_CRYPTO_OK) {
            st = ANO_CRYPTO_ERR_INTEGRITY_CHECK_FAILED;
            ano_crypto_internal_zeroize(signature, 64u);
        }
        goto cleanup;
    }
    st = ANO_CRYPTO_ERR_INTERNAL_ERROR;

cleanup:
    ano_crypto_internal_zeroize(z, sizeof(z));
    ano_crypto_internal_zeroize(k, sizeof(k));
    ano_crypto_internal_zeroize(r, sizeof(r));
    ano_crypto_internal_zeroize(s, sizeof(s));
    ano_crypto_internal_zeroize(rd, sizeof(rd));
    ano_crypto_internal_zeroize(sum, sizeof(sum));
    ano_crypto_internal_zeroize(k_inv, sizeof(k_inv));
    ano_crypto_internal_zeroize(x_normal, sizeof(x_normal));
    ano_crypto_internal_zeroize(y_normal, sizeof(y_normal));
    ano_crypto_internal_zeroize(&point_j, sizeof(point_j));
    ano_crypto_internal_zeroize(&point, sizeof(point));
    ano_crypto_internal_zeroize(&nonce_state, sizeof(nonce_state));
    if (st != ANO_CRYPTO_OK) {
        ano_crypto_internal_zeroize(signature, 64u);
    }
    return st;
}
