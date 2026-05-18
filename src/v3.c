#include "common.h"
#include "exp10_versions.h"


__attribute__((noinline))
float exp10_v3(float x) {
    if (isnan(x)) return NAN;
    if (isinf(x)) return x > 0 ? INFINITY : 0.0f;
    if (x > X_MAX_NORMAL) return INFINITY;
    if (x < X_MIN_NORMAL) return 0.0f;
    double xd = (double)x;

    double y = fma(xd, D_LOG2_10_HI, xd * D_LOG2_10_LO);
    double n = nearbyint(y);
    double r = fma(-1.0, n, y);
    double r2 = r * r;

    double t54 = fma(D_M5_C5, r, D_M5_C4);
    double t32 = fma(D_M5_C3, r, D_M5_C2);
    double t10 = fma(D_M5_C1, r, D_M5_C0);

    double poly = fma(t54, r2, t32);
    poly = fma(poly, r2, t10);
    double result =
    ldexp(poly, (int32_t)n);
    return (float)result;
}
