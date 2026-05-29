#include "common.h"
#include "exp10_versions.h"


__attribute__((noinline))
float exp10_v0(float x) {
    if (my_isnanf(x)) return NAN;
    if (my_isinff(x)) return x > 0 ? INFINITY : 0.0f;
    
float y =
    x * LOG2_10_HI +
    x * LOG2_10_LO;

float dn =
    (float)((int32_t)
    (y > 0.0f ? y + 0.5f : y - 0.5f));

float r =
    fmaf(x, LOG2_10_HI, -dn)
    + x * LOG2_10_LO;

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
    my_ldexpf((float)poly, (int32_t)n);
if (x > X_MAX_FLOAT)
    return INFINITY;
if (x < X_MIN_FLOAT)
    return 0.0f;
return result;

}