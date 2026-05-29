#include "common.h"
#include "exp10_versions.h"

__attribute__((noinline))
float exp10_v4(float x) {
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

    float poly = fmaf(M5_C5, r, M5_C4);
    poly = fmaf(poly, r, M5_C3);
    poly = fmaf(poly, r, M5_C2);
    poly = fmaf(poly, r, M5_C1);
    poly = fmaf(poly, r, M5_C0);

float result =
    my_ldexpf((float)poly, (int32_t)n);
    if (x > X_MAX_FLOAT)
    return INFINITY;

return result;

}
