#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <x86intrin.h>
#include <immintrin.h>

#define FLOAT_BIAS 127
#define FLOAT_MANTISSA_LENGTH 23

#define X_MIN_NORMAL (-37.9f)
#define X_MAX_NORMAL ( 38.53184f)

#define SHIFTER 12582912.0f /* 0x1.8p23 */

#define LOG2_10      3.32192809488736234787f

/* V1+ : split log2(10) */
#define LOG2_10_HI   3.32177734375f
#define LOG2_10_LO   0.00015075113736234787f

/* Taylor degree 10 for 2^r */
#define T10_C0  1.0f
#define T10_C1  0.6931471805599453f
#define T10_C2  0.2402265069591007f
#define T10_C3  0.0555041086648216f
#define T10_C4  0.0096181291076285f
#define T10_C5  0.0013333558146428f
#define T10_C6  0.0001540353039338f
#define T10_C7  0.0000152527338050f
#define T10_C8  0.0000013215486790f
#define T10_C9  0.0000001017177780f
#define T10_C10 0.0000000070526790f

/* Taylor degree 6 for 2^r */
#define T6_C0  1.0f
#define T6_C1  0.6931471805599453f
#define T6_C2  0.2402265069591007f
#define T6_C3  0.0555041086648216f
#define T6_C4  0.0096181291076285f
#define T6_C5  0.0013333558146428f
#define T6_C6  0.0001540353039338f

/* V2/V3/V4/V5: degree-5 hand-tuned / minimax-style fit for 2^r on [-0.5, 0.5] */
#define M5_C0  1.00000005f
#define M5_C1  0.69314720f
#define M5_C2  0.24022212f
#define M5_C3  0.05550341f
#define M5_C4  0.00967076f
#define M5_C5  0.00133953f

/* V6: tiny poly for 2^r on very small interval [-1/64, 1/64] */
#define S2_C0  1.0f
#define S2_C1  0.6931471805599453f
#define S2_C2  0.2402265069591007f

#define TABLE_BITS 5
#define TABLE_SIZE (1 << TABLE_BITS)
#define TABLE_SCALE ((float)TABLE_SIZE)
#define INV_TABLE_SCALE (1.0f / (float)TABLE_SIZE)



typedef float (*scalar_fn_t)(float);

extern float g_exp2_table[TABLE_SIZE];

float F32(uint32_t x);
uint32_t U32(float x);

double F64(uint64_t x);
uint64_t U64(double x);

double ulp_error_float(double ref, float test);

float pow2i_reconstruct_normal(int32_t n);
float ref_exp10f(float x);
float round_shifter(float x);

uint64_t start_tsc(void);
uint64_t stop_tsc(void);

void init_table(void);

#endif