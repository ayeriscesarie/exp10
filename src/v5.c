#include "common.h"
#include "exp10_versions.h"

__attribute__((noinline))
float exp10_v5(float x) {
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


    static int dbg2 = 0;

float poly = fmaf(M7_C7, r, M7_C6);
poly = fmaf(poly, r, M7_C5);
poly = fmaf(poly, r, M7_C4);
poly = fmaf(poly, r, M7_C3);
poly = fmaf(poly, r, M7_C2);
poly = fmaf(poly, r, M7_C1);
poly = fmaf(poly, r, M7_C0);

float result =
    my_ldexpf(poly, (int32_t)n);
if (x > X_MAX_FLOAT)
    return INFINITY;

return result;
}