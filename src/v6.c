#include "common.h"
#include "exp10_versions.h"

__attribute__((noinline))
float exp10_v6(float x) {
    if (my_isnanf(x)) return NAN;
    if (my_isinff(x)) return x > 0 ? INFINITY : 0.0f;
 //   if (x > X_MAX_NORMAL) return INFINITY;
   // if (x < X_MIN_FLOAT) return 0.0f;
double xd = (double)x;

double y =
    fma(
        xd,
        D_LOG2_10_HI,
        xd * D_LOG2_10_LO
    );

double kf_d =
    my_nearbyint_custom(
        y * (double)TABLE_SIZE
    );

int32_t k =
    (int32_t)kf_d;

    int32_t n =
    my_floor_int(
        (double)k * D_INV_TABLE_SCALE
    );
    int32_t i = k - (n << TABLE_BITS);

    if (i < 0) {
        i += TABLE_SIZE;
        n -= 1;
    }

float r =
    (float)(
        y -
        kf_d * D_INV_TABLE_SCALE
    );

float poly =
    fmaf(S2_C3, r, S2_C2);

poly =
    fmaf(poly, r, S2_C1);

poly =
    fmaf(poly, r, S2_C0);
    double base = pow2i_reconstruct_normal_double(n);
    double table_val = (double)g_exp2_table[i];

double result_d =
    base * table_val * poly;

float result =
    (float)result_d;
    return result;
}
