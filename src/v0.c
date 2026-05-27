#include "common.h"
#include "exp10_versions.h"


__attribute__((noinline))
float exp10_v0(float x) {
    if (isnan(x)) return NAN;
    if (isinf(x)) return x > 0 ? INFINITY : 0.0f;
double y =
    (double)x * LOG2_10;

double dn =
    nearbyint(y);

double r_d =
    y - dn;

float r =
    (float)r_d;

int32_t n =
    (int32_t)dn;

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