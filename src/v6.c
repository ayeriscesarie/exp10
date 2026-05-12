#include "common.h"
#include "exp10_versions.h"

__attribute__((noinline))
float exp10_v6(float x) {
    if (isnan(x)) return NAN;
    if (isinf(x)) return x > 0 ? INFINITY : 0.0f;
    if (x > X_MAX_NORMAL) return INFINITY;
    if (x < X_MIN_NORMAL) return 0.0f;

    double xd = (double)x;

    double y = fma(xd, D_LOG2_10_HI, xd * D_LOG2_10_LO);

    double kf_d = nearbyint(y * D_TABLE_SCALE);
    int32_t k = (int32_t)kf_d;

    int32_t n = (int32_t)floor((double)k * D_INV_TABLE_SCALE);
    int32_t i = k - (n << TABLE_BITS);

    if (i < 0) {
        i += TABLE_SIZE;
        n -= 1;
    }

    double r = fma(-kf_d, D_INV_TABLE_SCALE, y);

    double poly = fma(D_S2_C2, r, D_S2_C1);
    poly = fma(poly, r, D_S2_C0);

    double base = pow2i_reconstruct_normal_double(n);
    double table_val = (double)g_exp2_table[i];

    double result = base * table_val * poly;
    return (float)result;
}