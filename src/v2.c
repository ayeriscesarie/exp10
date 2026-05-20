#include "common.h"
#include "exp10_versions.h"


__attribute__((noinline))
float exp10_v2(float x) {
    if (isnan(x)) return NAN;
    if (isinf(x)) return x > 0 ? INFINITY : 0.0f;
    if (x > X_MAX_NORMAL) return INFINITY;
   if (x < X_MIN_FLOAT) return 0.0f;
double xd = (double)x;

double y = fma(xd, D_LOG2_10_HI, xd * D_LOG2_10_LO);
double n = nearbyint(y);
double r = fma(-1.0, n, y);

double poly = fma(D_M5_C5, r, D_M5_C4);
poly = fma(poly, r, D_M5_C3);
poly = fma(poly, r, D_M5_C2);
poly = fma(poly, r, D_M5_C1);
poly = fma(poly, r, D_M5_C0);

float result =
    ldexp((float)poly, (int32_t)n);
return result;

}