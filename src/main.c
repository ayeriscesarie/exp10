#include <stdio.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "exp10_versions.h"
#include "bench.h"
#include "accuracy_logger.h"
#define LN10_F 0x1.26bb1cp+1f

float exp10f_v0(float x);

static float libc_exp10f_wrapper(float x)
{
    return expf(x * LN10_F);
}

int main(void) {
    init_table();

    const int ACCURACY_SAMPLES = 50000;
    const int BENCH_N = 20000;
    const int REPEATS = 7;

    row_desc_t rows[] = {
        {"V0", "nothing",                                exp10_v0},
        {"V1", "2-part LOG2_10 + shorter Taylor",        exp10_v1},
        {"V2", "degree-5 minimax-style coeffs",          exp10_v2},
        {"V3", "Estrin scheme",                          exp10_v3},
        {"V4", "shifter instead of roundf",              exp10_v4},
        {"V5", "no explicit fmaf in source",             exp10_v5},
        {"V6", "table of 2^(i/32) + tiny residual poly", exp10_v6},
        {"LIBC", "libc expf(x * ln10)", libc_exp10f_wrapper},
    };

    const int row_count = (int)(sizeof(rows) / sizeof(rows[0]));

    remove("accuracy_report.txt");

    for (int i = 0; i < row_count; ++i) {
        write_accuracy_report_for_version(
            "accuracy_report.txt",
            rows[i].version,
            rows[i].fn,
            10000,
            4.0
        );
    }

    print_header();
    fflush(stdout);

    for (int i = 0; i < row_count; ++i) {
        row_result_t rr = measure_scalar_row(
            rows[i].fn,
            ACCURACY_SAMPLES,
            BENCH_N,
            REPEATS
        );
        
        print_row(rows[i].version, rows[i].optimization, &rr, 1, 0);
        fflush(stdout);
    }

#if defined(__AVX2__) && defined(__FMA__)
    {
        row_result_t rr = measure_v7_avx2(200, REPEATS);
        print_row("V7", "AVX2 vector kernel (throughput-oriented)", &rr, 0, 1);
        fflush(stdout);
    }
#else
    {
        row_result_t rr;
        memset(&rr, 0, sizeof(rr));

        rr.skipped = 1;

        print_row("V7", "AVX2 vector kernel (not compiled / not supported)", &rr, 0, 1);
        fflush(stdout);
    }
#endif

    print_footer();

    printf("Notes:\n");
    printf("1) scalar rows report latency only\n");
    printf("2) vector row V7 reports throughput only\n");
    printf("3) accuracy report was written to accuracy_report.txt\n");
    printf("\n");
    printf("%.20e\n", exp10_v0(-38.83288f));
    printf("%.20e\n", powf(10.0f,-38.83288f));
    printf("%.20e\n", exp10_v6(-38.83288f));

float my = exp10_v6(-38.83288f);
float ref = powf(10.0f,-38.83288f);

printf("my  = %.20e\n", my);
printf("ref = %.20e\n", ref);
printf("diff = %.20e\n", fabsf(my - ref));

uint32_t a, b;

memcpy(&a, &my, 4);
memcpy(&b, &ref, 4);

printf("%u %u diff=%u\n", a, b, abs((int)a - (int)b));
}
