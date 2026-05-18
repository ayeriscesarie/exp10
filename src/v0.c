#include "common.h"
#include "exp10_versions.h"


__attribute__((noinline))
float exp10_v0(float x) {
    if (isnan(x)) return NAN;
    if (isinf(x)) return x > 0 ? INFINITY : 0.0f;
    if (x > X_MAX_NORMAL) return INFINITY;
    if (x < X_MIN_NORMAL) return 0.0f;

double xd = (double)x;

double y = xd * D_LOG2_10;
double n = nearbyint(y);
double r = fma(-1.0, n, y);

double poly = fma(D_T10_C10, r, D_T10_C9);
poly = fma(poly, r, D_T10_C8);
poly = fma(poly, r, D_T10_C7);
poly = fma(poly, r, D_T10_C6);
poly = fma(poly, r, D_T10_C5);
poly = fma(poly, r, D_T10_C4);
poly = fma(poly, r, D_T10_C3);
poly = fma(poly, r, D_T10_C2);
poly = fma(poly, r, D_T10_C1);
poly = fma(poly, r, D_T10_C0);

double result =
    ldexp(poly, (int32_t)n);
return (float)result;

}