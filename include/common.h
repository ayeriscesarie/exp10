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
#define X_MAX_NORMAL  38.53184f
#define X_MIN_NORMAL -37.92978f

#define X_MAX_FLOAT   38.53184f
#define X_MIN_FLOAT  -44.85347f
#define LOG2_10_F 3.32192809489f
#define SHIFTER 12582912.0f /* 0x1.8p23 */

#define LOG2_10      3.32192809488736234787

/* V1+ : split log2(10) */
#define LOG2_10_HI 3.3203125f // = 425/128, представим во float точно
#define LOG2_10_LO 0.0016155949f // log2(10) - HI

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

#define M7_C0 1.0000000000f
#define M7_C1 0.6931471825f
#define M7_C2 0.2402265072f
#define M7_C3 0.0555041097f
#define M7_C4 0.0096181291f
#define M7_C5 0.0013333558f
#define M7_C6 0.0001540353f
#define M7_C7 0.0000152527f

/* V6: tiny poly for 2^r on very small interval [-1/64, 1/64] */
#define S2_C0  1.0f
#define S2_C1  0.6931471805599453f
#define S2_C2  0.2402265069591007f
#define S2_C3 0.05550410866482158f

#define D_LOG2_10 3.32192809488736234787
#define D_LOG2_10_HI 3.32177734375
#define D_LOG2_10_LO 0.00015075113736234787

#define D_T10_C0 1.0
#define D_T10_C1 0.69314718055994972712
#define D_T10_C2 0.24022650695909281149
#define D_T10_C3 0.05550410866446074676
#define D_T10_C4 0.00961812910792206668
#define D_T10_C5 0.00133335582283256038
#define D_T10_C6 0.00015403530062300925
#define D_T10_C7 0.00001525265823062512
#define D_T10_C8 0.00000132156229030099
#define D_T10_C9 0.00000010208518951521
#define D_T10_C10 0.00000000704302446800

#define D_T6_C0 1.00000000055416826683
#define D_T6_C1 0.69314720573728527686
#define D_T6_C2 0.24022646890642110340
#define D_T6_C3 0.05550328776936466757
#define D_T6_C4 0.00961848895603934807
#define D_T6_C5 0.00133999312287381577
#define D_T6_C6 0.00015345812298903661

#define D_M5_C0 1.00000007165468218772
#define D_M5_C1 0.69314696706485023903
#define D_M5_C2 0.24022119723824822413
#define D_M5_C3 0.05550713273360018130
#define D_M5_C4 0.00967554133528327441
#define D_M5_C5 0.00132764720345635674


#define D_S2_C0 1.00000000042995673688
#define D_S2_C1 0.69315734356519509429
#define D_S2_C2 0.24022474583238120904

#define D_TABLE_SCALE 64.0
#define D_INV_TABLE_SCALE (1.0 / 64.0)

double pow2i_reconstruct_normal_double(int32_t n);

#define TABLE_BITS 6
#define TABLE_SIZE (1 << TABLE_BITS)

#define TABLE_SCALE ((float)TABLE_SIZE)
#define INV_TABLE_SCALE (1.0f / (float)TABLE_SIZE)



typedef float (*scalar_fn_t)(float);

extern float g_exp2_table[TABLE_SIZE];

float F32(uint32_t x);
uint32_t U32(float x);

double F64(uint64_t x);
uint64_t U64(double x);
double my_nearbyint_custom(double x);
double ulp_error_float(double ref, float test);

float pow2i_reconstruct_normal(int32_t n);
float ref_exp10f(float x);
float round_shifter(float x);

uint64_t start_tsc(void);
uint64_t stop_tsc(void);

void init_table(void);

float my_ldexpf(float poly, int n);

int my_isnanf(float x);
int my_isinff(float x);
int32_t my_floor_int(double x);
float my_inf(void);
float my_nan(void);

float my_fabsf(float x);

double my_fabs(double x);
#endif