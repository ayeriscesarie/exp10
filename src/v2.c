#include "common.h"
#include "exp10_versions.h"


__attribute__((noinline))
float exp10_v2(float x) {
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
    
float poly = fmaf(M5_C5, r, M5_C4);
poly = fmaf(poly, r, M5_C3);
poly = fmaf(poly, r, M5_C2);
poly = fmaf(poly, r, M5_C1);
poly = fmaf(poly, r, M5_C0);

float result =
    ldexpf((float)poly, (int32_t)n);
if (x > X_MAX_FLOAT)
    return INFINITY;

if (x < X_MIN_FLOAT)
    return 0.0f;

if (x < X_MIN_NORMAL)
    return powf(10.0f, x);
return result;

}