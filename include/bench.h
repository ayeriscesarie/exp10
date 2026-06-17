#ifndef BENCH_H
#define BENCH_H

#include "common.h"

typedef struct {
    const char *version;
    const char *optimization;
    scalar_fn_t fn;
} row_desc_t;

typedef struct {
    double max_abs_ulp;
    double avg_abs_ulp;
    float worst_x;
    double latency_cpe;
    double throughput_cpe;
    int skipped;
} row_result_t;

row_result_t measure_scalar_row(
    scalar_fn_t fn,
    int accuracy_samples,
    int bench_n,
    int repeats
);

#if defined(__AVX2__) && defined(__FMA__)
row_result_t measure_v7_avx2(int bench_n, int repeats);
row_result_t measure_v5_avx2(int bench_n, int repeats);
row_result_t measure_sleef_avx2(int bench_n, int repeats);
row_result_t measure_v4_avx2(
    int bench_n,
    int repeats);
    row_result_t measure_v3_avx2(
    int bench_n,
    int repeats);
#endif

void print_header(void);

void print_row(
    const char *version,
    const char *opt,
    const row_result_t *rr,
    int has_latency,
    int has_throughput
);

void print_footer(void);
#endif