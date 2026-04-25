#include "common.h"
#include "exp10_versions.h"


__attribute__((noinline))
float exp10_v0(float x) {
    if (isnan(x)) return NAN;
    if (isinf(x)) return x > 0 ? INFINITY : 0.0f;
    if (x > X_MAX_NORMAL) return INFINITY;
    if (x < X_MIN_NORMAL) return 0.0f;

    float y = x * LOG2_10;
    float n = roundf(y);
    float r = y - n;

    float poly = fmaf(T10_C10, r, T10_C9);
    poly = fmaf(poly, r, T10_C8);
    poly = fmaf(poly, r, T10_C7);
    poly = fmaf(poly, r, T10_C6);
    poly = fmaf(poly, r, T10_C5);
    poly = fmaf(poly, r, T10_C4);
    poly = fmaf(poly, r, T10_C3);
    poly = fmaf(poly, r, T10_C2);
    poly = fmaf(poly, r, T10_C1);
    poly = fmaf(poly, r, T10_C0);

    return pow2i_reconstruct_normal((int32_t)n) * poly;
}