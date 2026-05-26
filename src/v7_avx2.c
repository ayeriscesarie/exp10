#include "common.h"
#include "exp10_versions.h"

#include <immintrin.h>

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

void exp10_v7_avx2_kernel(const float *x, float *out, size_t n) {
    const __m256 c_log2_10_hi =
    _mm256_set1_ps(LOG2_10_HI);

    const __m256 c_log2_10_lo =
        _mm256_set1_ps(LOG2_10_LO);
    const __m256i c_bias_i = _mm256_set1_epi32(FLOAT_BIAS);

    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 vx = _mm256_loadu_ps(x + i);

        __m256 y_hi =
            _mm256_mul_ps(vx, c_log2_10_hi);

        __m256 y_lo =
            _mm256_mul_ps(vx, c_log2_10_lo);

        __m256 nps =
            _mm256_round_ps(
                y_hi,
                _MM_FROUND_TO_NEAREST_INT |
                _MM_FROUND_NO_EXC
            );

        __m256 r =
            _mm256_sub_ps(y_hi, nps);

        r =
            _mm256_add_ps(r, y_lo);
            

        __m256 poly = poly_m5_avx2(r);

        __m256i ni = _mm256_cvtps_epi32(nps);
        const __m256i min_n = _mm256_set1_epi32(-126);
        const __m256i max_n = _mm256_set1_epi32(127);

        ni = _mm256_max_epi32(ni, min_n);
        ni = _mm256_min_epi32(ni, max_n);
        __m256i ei = _mm256_add_epi32(ni, c_bias_i);
        __m256i bits = _mm256_slli_epi32(ei, FLOAT_MANTISSA_LENGTH);
        __m256 two_pow_n = _mm256_castsi256_ps(bits);

        __m256 res = _mm256_mul_ps(two_pow_n, poly);
        float tmp_in[8];
        float tmp_out[8];

        _mm256_storeu_ps(tmp_in, vx);
        _mm256_storeu_ps(tmp_out, res);

        for (int k = 0; k < 8; ++k) {

            if (tmp_in[k] < X_MIN_NORMAL ||
                tmp_in[k] > X_MAX_NORMAL)
            {
                tmp_out[k] = exp10_v5(tmp_in[k]);
            }
        }

        res = _mm256_loadu_ps(tmp_out);
                _mm256_storeu_ps(out + i, res);
            }

            for (; i < n; ++i) {
                out[i] = exp10_v5(x[i]);
            }
        }
#endif