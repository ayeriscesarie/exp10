#include "common.h"

#define EXP_DOUBLE_MASK 0x7ff0000000000000ULL

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

double ulp_error_float(double ref, float test) {
    uint64_t ref_bits = U64(ref);
    uint64_t A_dot = ref_bits & EXP_DOUBLE_MASK;

    A_dot -= (23ULL << 52);

    double ulp_error = (((double)test) - ref) / F64(A_dot);

    return ulp_error;
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
    if (n > 1023) {
        return INFINITY;
    }

    if (n < -1022) {
        return 0.0;
    }

    uint64_t bits = (uint64_t)(n + 1023) << 52;
    return F64(bits);
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