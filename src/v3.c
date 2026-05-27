#include "common.h"
#include "exp10_versions.h"


__attribute__((noinline))
float exp10_v3(float x) {
    if (isnan(x)) return NAN;
    if (isinf(x)) return x > 0 ? INFINITY : 0.0f;


    double xd = (double)x;

    double y =
        fma(xd, D_LOG2_10_HI,
                xd * D_LOG2_10_LO);

    double dn =
        nearbyint(y);

    double r_d =
        y - dn;

    float r =
        (float)r_d;

    int32_t n =
        (int32_t)dn;

    float r2 =
        r * r;

    float t54 = fmaf(M5_C5, r, M5_C4);
    float t32 = fmaf(M5_C3, r, M5_C2);
    float t10 = fmaf(M5_C1, r, M5_C0);

    float poly = fmaf(t54, r2, t32);
    poly = fmaf(poly, r2, t10);
    float result =
    ldexpf(poly, (int32_t)n);
    if (x > X_MAX_FLOAT)
    return INFINITY;

if (x < X_MIN_FLOAT)
    return 0.0f;

if (x < X_MIN_NORMAL)
    return powf(10.0f, x);
    return (float)result;
}
