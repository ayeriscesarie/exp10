#include "common.h"
#include <stdint.h>
#include <string.h>
#include <float.h>
#include <math.h>

#define EXP_DOUBLE_MASK 0x7ff0000000000000ULL
#define SAFE_DOUBLE_LIMIT 9.007199254740991e+15

float g_exp2_table[TABLE_SIZE];

float F32(uint32_t x) {
    union {
        uint32_t u;
        float f;
    } v = { x };

    return v.f;
}

uint32_t U32(float x) {
    union {
        float f;
        uint32_t u;
    } v = { x };

    return v.u;
}

double F64(uint64_t x) {
    union {
        uint64_t u;
        double d;
    } v = { x };

    return v.d;
}

uint64_t U64(double x) {
    union {
        uint64_t u;
        double d;
    } v = { .d = x };

    return v.u;
}

double ulp_error_float(double ref, float test)
{
    if (!isfinite(ref) || !isfinite(test))
        return 0.0;

    float ref_f = (float)ref;

    float next =
        nextafterf(ref_f, INFINITY);

    double ulp =
        (double)next - (double)ref_f;

    if (ulp == 0.0)
        return 0.0;

    return ((double)test - (double)ref_f) / ulp;
}

float pow2i_reconstruct_normal(int32_t n) {
    if (n > 127) {
        return INFINITY;
    }

    if (n < -126) {
        return 0.0f;
    }

    uint32_t bits = (uint32_t)(n + FLOAT_BIAS) << FLOAT_MANTISSA_LENGTH;

    return F32(bits);
}

double pow2i_reconstruct_normal_double(int32_t n)
{
    if (n > 1023)
        return INFINITY;

    if (n >= -1022)
    {
        uint64_t bits =
            (uint64_t)(n + 1023) << 52;

        return F64(bits);
    }

    if (n < -1074)
        return 0.0;

    uint64_t mantissa =
        1ULL << (n + 1074);

    return F64(mantissa);
}

float ref_exp10f(float x) {
    return (float)powl(10.0L, (long double)x);
}

float round_shifter(float x) {
    float s = copysignf(SHIFTER, x);
    float y = x + s;
    y = y - s;

    return y;
}

uint64_t start_tsc(void) {
    return __rdtsc();
}

uint64_t stop_tsc(void) {
    unsigned aux;

    return __rdtscp(&aux);
}

void init_table(void) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        
        g_exp2_table[i] = exp2f((float)i * INV_TABLE_SCALE);
    }
}

float my_ldexpf(float poly, int n)
{
    if (n > 127)
        return INFINITY;

    if (n >= -126)
    {
        uint32_t bits =
            (uint32_t)(n + FLOAT_BIAS)
            << FLOAT_MANTISSA_LENGTH;

        float two_pow_n = F32(bits);

        return poly * two_pow_n;
    }

    // subnormal range

    if (n < -149)
        return 0.0f;

    uint32_t bits =
        1u << (n + 149);

    float two_pow_n = F32(bits);

    return poly * two_pow_n;
}


int my_isnanf(float x)
{
    union {
        float f;
        uint32_t u;
    } v;

    v.f = x;

    uint32_t exp =
        (v.u >> 23) & 0xFF;

    uint32_t mant =
        v.u & 0x7FFFFF;

    return (exp == 0xFF) && (mant != 0);
}

int my_isinff(float x)
{
    union {
        float f;
        uint32_t u;
    } v;

    v.f = x;

    uint32_t exp =
        (v.u >> 23) & 0xFF;

    uint32_t mant =
        v.u & 0x7FFFFF;

    return (exp == 0xFF) && (mant == 0);
}

int32_t my_floor_int(double x)
{
    int32_t i = (int32_t)x;

    if ((double)i > x)
        --i;

    return i;
}

float my_inf(void)
{
    return F32(0x7F800000u);
}

float my_nan(void)
{
    return F32(0x7FC00000u);
}

float my_fabsf(float x)
{
    union {
        float f;
        uint32_t u;
    } v;

    v.f = x;
    v.u &= 0x7FFFFFFF;

    return v.f;
}

double my_fabs(double x)
{
    union {
        double d;
        uint64_t u;
    } v;

    v.d = x;

    v.u &= 0x7FFFFFFFFFFFFFFFULL;

    return v.d;
}

double my_nearbyint_custom(double x) {
 
    // Слишком большие числа уже являются целыми
    if (my_fabs(x) > SAFE_DOUBLE_LIMIT) {
        return x;
    }

    // Отбрасываем дробную часть (округление к нулю)
    long long truncated = (long long)x;
    double fractional = x - (double)truncated;

    // Если дробная часть ровно 0.5 (или -0.5)
    if (fractional == 0.5 || fractional == -0.5) {

        // Округляем до ближайшего ЧЕТНОГО
        if ((truncated & 1LL) == 0) {
            return (double)truncated;
        }

        return (double)(
            truncated +
            (fractional > 0.0 ? 1 : -1)
        );
    }

    // Если дробная часть > 0.5
    else if (fractional > 0.5) {
        return (double)(truncated + 1);
    }

    // Если дробная часть < -0.5
    else if (fractional < -0.5) {
        return (double)(truncated - 1);
    }

    // Во всех остальных случаях возвращаем просто отброшенную дробную часть
    return (double)truncated;
}

