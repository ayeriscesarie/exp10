#include <immintrin.h>
#include <stdint.h>
#include <stddef.h>

#include "common.h"
#include "exp10_versions.h"
void exp10_v4_avx2_kernel(
    const float *in,
    float *out,
    size_t count)
{
    const __m256 log2_hi =
        _mm256_set1_ps(LOG2_10_HI);

    const __m256 log2_lo =
        _mm256_set1_ps(LOG2_10_LO);

    const __m256 c0 = _mm256_set1_ps(M5_C0);
    const __m256 c1 = _mm256_set1_ps(M5_C1);
    const __m256 c2 = _mm256_set1_ps(M5_C2);
    const __m256 c3 = _mm256_set1_ps(M5_C3);
    const __m256 c4 = _mm256_set1_ps(M5_C4);
    const __m256 c5 = _mm256_set1_ps(M5_C5);

    const __m256i bias =
        _mm256_set1_epi32(127);

    size_t i = 0;

    for (; i + 8 <= count; i += 8)
    {
        __m256 x =
            _mm256_loadu_ps(in + i);

        __m256 y =
            _mm256_fmadd_ps(
                x,
                log2_hi,
                _mm256_mul_ps(x, log2_lo));

        __m256 half =
            _mm256_set1_ps(0.5f);

        __m256 signmask =
            _mm256_cmp_ps(
                y,
                _mm256_setzero_ps(),
                _CMP_GT_OS);

        __m256 shifted =
            _mm256_blendv_ps(
                _mm256_sub_ps(y, half),
                _mm256_add_ps(y, half),
                signmask);

        __m256i n =
            _mm256_cvttps_epi32(shifted);

        __m256 dn =
            _mm256_cvtepi32_ps(n);

__m256 r =
    _mm256_fmadd_ps(
        x,
        log2_hi,
        _mm256_sub_ps(
            _mm256_setzero_ps(),
            dn));

r =
    _mm256_add_ps(
        r,
        _mm256_mul_ps(x, log2_lo));

        __m256 poly =
            _mm256_fmadd_ps(c5, r, c4);

        poly =
            _mm256_fmadd_ps(poly, r, c3);

        poly =
            _mm256_fmadd_ps(poly, r, c2);

        poly =
            _mm256_fmadd_ps(poly, r, c1);

        poly =
            _mm256_fmadd_ps(poly, r, c0);

__m256 result;

/* normal: -126 <= n <= 127 */

__m256i normal_mask =
    _mm256_and_si256(
        _mm256_cmpgt_epi32(
            n,
            _mm256_set1_epi32(-127)),
        _mm256_cmpgt_epi32(
            _mm256_set1_epi32(128),
            n));

__m256i exp_bits =
    _mm256_slli_epi32(
        _mm256_add_epi32(
            n,
            bias),
        23);

__m256 normal_scale =
    _mm256_castsi256_ps(exp_bits);

result =
    _mm256_mul_ps(
        poly,
        normal_scale);

/* subnormal: -149 <= n < -126 */

__m256i sub_mask =
    _mm256_and_si256(
        _mm256_cmpgt_epi32(
            n,
            _mm256_set1_epi32(-150)),
        _mm256_cmpgt_epi32(
            _mm256_set1_epi32(-126),
            n));

__m256i sub_shift =
    _mm256_add_epi32(
        n,
        _mm256_set1_epi32(149));

__m256i sub_bits =
    _mm256_sllv_epi32(
        _mm256_set1_epi32(1),
        sub_shift);

__m256 sub_scale =
    _mm256_castsi256_ps(sub_bits);

__m256 sub_result =
    _mm256_mul_ps(
        poly,
        sub_scale);

result =
    _mm256_blendv_ps(
        result,
        sub_result,
        _mm256_castsi256_ps(sub_mask));

/* underflow: n < -149 */

__m256i under_mask =
    _mm256_cmpgt_epi32(
        _mm256_set1_epi32(-149),
        n);

result =
    _mm256_blendv_ps(
        result,
        _mm256_setzero_ps(),
        _mm256_castsi256_ps(under_mask));

/* overflow: n > 127 */

__m256i over_mask =
    _mm256_cmpgt_epi32(
        n,
        _mm256_set1_epi32(127));

result =
    _mm256_blendv_ps(
        result,
        _mm256_set1_ps(INFINITY),
        _mm256_castsi256_ps(over_mask));
        _mm256_storeu_ps(
            out + i,
            result);
    }

    for (; i < count; ++i)
    {
        out[i] = exp10_v4(in[i]);
    }
    
}