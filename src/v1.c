#include "common.h"
#include "exp10_versions.h"


__attribute__((noinline))
float exp10_v1(float x) {
    if (isnan(x)) return NAN;
    if (isinf(x)) return x > 0 ? INFINITY : 0.0f;
    if (x > X_MAX_NORMAL) return INFINITY;
    if (x < X_MIN_NORMAL) return 0.0f;

    double xd = (double)x;

    double y = fma(xd, D_LOG2_10_HI, xd * D_LOG2_10_LO);
    double n = nearbyint(y);
    double r = fma(-1.0, n, y);

    double poly = fma(D_T6_C6, r, D_T6_C5);
    poly = fma(poly, r, D_T6_C4);
    poly = fma(poly, r, D_T6_C3);
    poly = fma(poly, r, D_T6_C2);
    poly = fma(poly, r, D_T6_C1);
    poly = fma(poly, r, D_T6_C0);

    double result = pow2i_reconstruct_normal_double((int32_t)n) * poly;
    return (float)result;

}