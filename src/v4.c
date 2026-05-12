#include "common.h"
#include "exp10_versions.h"

__attribute__((noinline))
float exp10_v4(float x) {
    if (isnan(x)) return NAN;
    if (isinf(x)) return x > 0 ? INFINITY : 0.0f;
    if (x > X_MAX_NORMAL) return INFINITY;
    if (x < X_MIN_NORMAL) return 0.0f;

    float y = fmaf(x, LOG2_10_HI, x * LOG2_10_LO);
    float n = round_shifter(y);
    float r = fmaf(-n, 1.0f, y);

    float poly = fmaf(M5_C5, r, M5_C4);
    poly = fmaf(poly, r, M5_C3);
    poly = fmaf(poly, r, M5_C2);
    poly = fmaf(poly, r, M5_C1);
    poly = fmaf(poly, r, M5_C0);

    return pow2i_reconstruct_normal((int32_t)n) * poly;
}
