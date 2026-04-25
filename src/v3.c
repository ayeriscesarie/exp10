#include "common.h"
#include "exp10_versions.h"


__attribute__((noinline))
float exp10_v3(float x) {
    if (isnan(x)) return NAN;
    if (isinf(x)) return x > 0 ? INFINITY : 0.0f;
    if (x > X_MAX_NORMAL) return INFINITY;
    if (x < X_MIN_NORMAL) return 0.0f;
    float y = fmaf(x, LOG2_10_HI, x * LOG2_10_LO);
    float n = roundf(y);
    float r = y - n;

    float r2 = r * r;
    float t54 = fmaf(M5_C5, r, M5_C4);
    float t32 = fmaf(M5_C3, r, M5_C2);
    float t10 = fmaf(M5_C1, r, M5_C0);
    float poly = fmaf(t54, r2, t32);
    poly = fmaf(poly, r2, t10);

    return pow2i_reconstruct_normal((int32_t)n) * poly;
}
