#include "common.h"
#include "exp10_versions.h"

__attribute__((noinline))
float exp10_v6(float x) {
    if (isnan(x)) return NAN;
    if (isinf(x)) return x > 0 ? INFINITY : 0.0f;
    if (x > X_MAX_NORMAL) return INFINITY;
    if (x < X_MIN_NORMAL) return 0.0f;

    float y = fmaf(x, LOG2_10_HI, x * LOG2_10_LO);

    float kf = roundf(y * TABLE_SCALE);
    int32_t k = (int32_t)kf;

    int32_t n = (int32_t)floorf((float)k * INV_TABLE_SCALE);
    int32_t i = k - (n << TABLE_BITS);
    if (i < 0) {
        i += TABLE_SIZE;
        n -= 1;
    }

float r = fmaf(-kf, INV_TABLE_SCALE, y);

    float poly = fmaf(S2_C2, r, S2_C1);
    poly = fmaf(poly, r, S2_C0);

    float base = pow2i_reconstruct_normal(n);
    float table_val = g_exp2_table[i];

    return base * table_val * poly;
}