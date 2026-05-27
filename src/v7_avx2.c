#include "common.h"
#include "exp10_versions.h"

#include <immintrin.h>
#include <math.h>

#if defined(__AVX2__) && defined(__FMA__)

static inline __m256 poly_m5_avx2(__m256 r) {
    const __m256 c0 = _mm256_set1_ps(M5_C0);
    const __m256 c1 = _mm256_set1_ps(M5_C1);
    const __m256 c2 = _mm256_set1_ps(M5_C2);
    const __m256 c3 = _mm256_set1_ps(M5_C3);
    const __m256 c4 = _mm256_set1_ps(M5_C4);
    const __m256 c5 = _mm256_set1_ps(M5_C5);

    __m256 p = _mm256_fmadd_ps(c5, r, c4);
    p = _mm256_fmadd_ps(p, r, c3);
    p = _mm256_fmadd_ps(p, r, c2);
    p = _mm256_fmadd_ps(p, r, c1);
    p = _mm256_fmadd_ps(p, r, c0);

    return p;
}

void exp10_v7_avx2_kernel(
    const float *x,
    float *out,
    size_t n
) {
    const __m256i c_bias_i =
        _mm256_set1_epi32(FLOAT_BIAS);

    const __m256i min_n =
        _mm256_set1_epi32(-126);

    const __m256i max_n =
        _mm256_set1_epi32(127);

    size_t i = 0;

    for (; i + 8 <= n; i += 8) {

        __m256 vx =
            _mm256_loadu_ps(x + i);

        __m256 mask_over =
            _mm256_cmp_ps(
                vx,
                _mm256_set1_ps(X_MAX_FLOAT),
                _CMP_GT_OQ
            );

        __m256 mask_under =
            _mm256_cmp_ps(
                vx,
                _mm256_set1_ps(X_MIN_FLOAT),
                _CMP_LT_OQ
            );

        float xin[8];
        float r_arr[8];
        int n_arr[8];

        _mm256_storeu_ps(xin, vx);

        for (int k = 0; k < 8; ++k) {

            double y =
                (double)xin[k] * LOG2_10;

            double dn =
                nearbyint(y);

            double r =
                y - dn;

            n_arr[k] = (int)dn;
            r_arr[k] = (float)r;
        }

        __m256 r =
            _mm256_loadu_ps(r_arr);

        __m256 poly =
            poly_m5_avx2(r);

        __m256i ni =
            _mm256_set_epi32(
                n_arr[7],
                n_arr[6],
                n_arr[5],
                n_arr[4],
                n_arr[3],
                n_arr[2],
                n_arr[1],
                n_arr[0]
            );

        ni = _mm256_max_epi32(ni, min_n);
        ni = _mm256_min_epi32(ni, max_n);

        __m256i ei =
            _mm256_add_epi32(
                ni,
                c_bias_i
            );

        __m256i bits =
            _mm256_slli_epi32(
                ei,
                FLOAT_MANTISSA_LENGTH
            );

        __m256 two_pow_n =
            _mm256_castsi256_ps(bits);

        __m256 res =
            _mm256_mul_ps(
                two_pow_n,
                poly
            );

        float tmpx[8];
        float tmpres[8];

        _mm256_storeu_ps(tmpx, vx);
        _mm256_storeu_ps(tmpres, res);

        for (int k = 0; k < 8; ++k) {

            if (
                tmpx[k] < X_MIN_NORMAL &&
                tmpx[k] > X_MIN_FLOAT
            ) {
                tmpres[k] =
                    powf(10.0f, tmpx[k]);
            }

            if (tmpx[k] > X_MAX_FLOAT) {
                tmpres[k] = INFINITY;
            }

            if (tmpx[k] < X_MIN_FLOAT) {
                tmpres[k] = 0.0f;
            }
        }

        res = _mm256_loadu_ps(tmpres);

        res = _mm256_blendv_ps(
            res,
            _mm256_set1_ps(INFINITY),
            mask_over
        );

        res = _mm256_blendv_ps(
            res,
            _mm256_setzero_ps(),
            mask_under
        );

        _mm256_storeu_ps(out + i, res);
    }

    for (; i < n; ++i) {
        out[i] = exp10_v5(x[i]);
    }
}

#endif