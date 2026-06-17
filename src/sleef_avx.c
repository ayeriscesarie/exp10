#include <immintrin.h>
#include <stddef.h>
#include <math.h>
#include <sleef.h>

#include "common.h"
#include "exp10_versions.h"

void exp10_sleef_avx2_kernel(
    const float *in,
    float *out,
    size_t count)
{
    const __m256 log2_hi =
        _mm256_set1_ps(LOG2_10_HI);

    const __m256 log2_lo =
        _mm256_set1_ps(LOG2_10_LO);

    const __m256 xmin =
        _mm256_set1_ps(-45.1545f);

    const __m256 xmax =
        _mm256_set1_ps(38.53184f);

    size_t i = 0;

    for (; i + 8 <= count; i += 8)
    {
        __m256 x =
            _mm256_loadu_ps(in + i);

        // y = x * log2(10)
        __m256 y =
            _mm256_fmadd_ps(
                x,
                log2_hi,
                _mm256_mul_ps(x, log2_lo));

        // 2^y using SLEEF
        __m256 result =
            Sleef_exp2f8_u10avx2(y);

        // underflow
        __m256 under =
            _mm256_cmp_ps(
                x,
                xmin,
                _CMP_LT_OS);

        // overflow
        __m256 over =
            _mm256_cmp_ps(
                x,
                xmax,
                _CMP_GT_OS);

        result =
            _mm256_blendv_ps(
                result,
                _mm256_setzero_ps(),
                under);

        result =
            _mm256_blendv_ps(
                result,
                _mm256_set1_ps(INFINITY),
                over);

        _mm256_storeu_ps(
            out + i,
            result);
    }

    // scalar tail
    for (; i < count; ++i)
    {
        if (in[i] > X_MAX_FLOAT)
        {
            out[i] = INFINITY;
            continue;
        }

        if (in[i] < -45.1545f)
        {
            out[i] = 0.0f;
            continue;
        }

        float y =
            fmaf(
                in[i],
                LOG2_10_HI,
                in[i] * LOG2_10_LO);

        out[i] = exp2f(y);
    }
}