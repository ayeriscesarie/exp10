#ifndef EXP10_VERSIONS_H
#define EXP10_VERSIONS_H

#include <stddef.h>

float exp10_v0(float x);
float exp10_v1(float x);
float exp10_v2(float x);
float exp10_v3(float x);
float exp10_v4(float x);
float exp10_v5(float x);
float exp10_v6(float x);

void exp10_v7_avx2_kernel(const float *x, float *out, size_t n);
void exp10_v5_avx2_kernel(
    const float *in,
    float *out,
    size_t count);
void exp10_v4_avx2_kernel(
    const float *in,
    float *out,
    size_t count);

void exp10_v3_avx2_kernel(
    const float *in,
    float *out,
    size_t count
);
#endif