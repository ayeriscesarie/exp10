#include "bench.h"
#include "exp10_versions.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

row_result_t measure_scalar_row(scalar_fn_t fn, int accuracy_samples, int bench_n, int repeats) {
    row_result_t rr;
    memset(&rr, 0, sizeof(rr));

    /* accuracy */
    {
        double max_abs_ulp = 0.0;
        double sum_abs_ulp = 0.0;
        unsigned valid = 0;
        float worst_x = 0.0f;

        const int pts_per_interval = 64;

        /* отрицательные интервалы: [-32,-16], [-16,-8], ..., [-2,-1] */
        for (int k = 5; k >= 1; --k) {
            double left  = -ldexp(1.0, k);
            double right = -ldexp(1.0, k - 1);

            for (int j = 0; j < pts_per_interval; ++j) {
                double t = (double)j / (double)(pts_per_interval - 1);
                double x = left + (right - left) * t;

                if (x < X_MIN_NORMAL || x > X_MAX_NORMAL) continue;

                float my = fn((float)x);
                double ref = (double)powl(10.0L, (long double)x);

                double ulp = ulp_error_float(ref, my);
                double abs_ulp = fabs(ulp);

                if (!isfinite(abs_ulp)) continue;

                sum_abs_ulp += abs_ulp;
                valid++;

                if (abs_ulp > max_abs_ulp) {
                    max_abs_ulp = abs_ulp;
                    worst_x = (float)x;
                }
            }
        }

        /* около нуля: [-2^-1,2^-1], [-2^-2,2^-2], ... */
        for (int k = 1; k <= 10; ++k) {
            double left  = -ldexp(1.0, -k);
            double right =  ldexp(1.0, -k);

            for (int j = 0; j < pts_per_interval; ++j) {
                double t = (double)j / (double)(pts_per_interval - 1);
                double x = left + (right - left) * t;

                if (x < X_MIN_NORMAL || x > X_MAX_NORMAL) continue;

                float my = fn((float)x);
                double ref = (double)powl(10.0L, (long double)x);

                double ulp = ulp_error_float(ref, my);
                double abs_ulp = fabs(ulp);

                if (!isfinite(abs_ulp)) continue;

                sum_abs_ulp += abs_ulp;
                valid++;

                if (abs_ulp > max_abs_ulp) {
                    max_abs_ulp = abs_ulp;
                    worst_x = (float)x;
                }
            }
        }

        /* положительные интервалы: [1,2], [2,4], ..., [32,64] */
        for (int k = 0; k <= 5; ++k) {
            double left  = ldexp(1.0, k);
            double right = ldexp(1.0, k + 1);

            for (int j = 0; j < pts_per_interval; ++j) {
                double t = (double)j / (double)(pts_per_interval - 1);
                double x = left + (right - left) * t;

                if (x < X_MIN_NORMAL || x > X_MAX_NORMAL) continue;

                float my = fn((float)x);
                double ref = (double)powl(10.0L, (long double)x);

                double ulp = ulp_error_float(ref, my);
                double abs_ulp = fabs(ulp);

                if (!isfinite(abs_ulp)) continue;

                sum_abs_ulp += abs_ulp;
                valid++;

                if (abs_ulp > max_abs_ulp) {
                    max_abs_ulp = abs_ulp;
                    worst_x = (float)x;
                }
            }
        }

        rr.max_abs_ulp = max_abs_ulp;
        rr.avg_abs_ulp = (valid > 0) ? (sum_abs_ulp / (double)valid) : 0.0;
        rr.worst_x = worst_x;
    }

    /* latency: dependent chain */
    {
        double best = 1e100;

        static const float base_args[16] = {
            -3.0f, -2.5f, -2.0f, -1.5f,
            -1.0f, -0.5f,  0.0f,  0.5f,
             1.0f,  1.5f,  2.0f,  2.5f,
             3.0f, -2.2f,  1.3f, -0.8f
        };

        volatile float prev = 0.12345f;

        for (int warm = 0; warm < 10000; ++warm) {
            float arg = base_args[warm & 15] + 0x1p-20f * prev;
            prev = fn(arg);
        }

        for (int rep = 0; rep < repeats; ++rep) {
            uint64_t t0 = start_tsc();

            for (int i = 0; i < bench_n; ++i) {
                float arg = base_args[i & 15] + 0x1p-20f * prev;
                prev = fn(arg);
            }

            uint64_t t1 = stop_tsc();

            double cpe = (double)(t1 - t0) / (double)bench_n;
            if (cpe < best) best = cpe;
        }

        rr.latency_cpe = best;
    }

    /* scalar throughput: measured but not shown */
    {
        double best = 1e100;

        static float args[4] = {0.10f, 0.20f, 0.30f, 0.40f};
        volatile float r0 = 0.0f;
        volatile float r1 = 0.0f;
        volatile float r2 = 0.0f;
        volatile float r3 = 0.0f;

        for (int warm = 0; warm < 10000; ++warm) {
            r0 = fn(args[0]);
            r1 = fn(args[1]);
            r2 = fn(args[2]);
            r3 = fn(args[3]);
        }

        for (int rep = 0; rep < repeats; ++rep) {
            uint64_t t0 = start_tsc();

            for (int i = 0; i < bench_n; ++i) {
                r0 = fn(args[0]);
                r1 = fn(args[1]);
                r2 = fn(args[2]);
                r3 = fn(args[3]);
            }

            uint64_t t1 = stop_tsc();

            double cpe = (double)(t1 - t0) / (double)(4 * bench_n);
            if (cpe < best) best = cpe;
        }

        rr.throughput_cpe = best;
    }

    return rr;
}


row_result_t measure_v7_avx2(int bench_n, int repeats) {
    row_result_t rr;
    memset(&rr, 0, sizeof(rr));

    enum { N = 1 << 15 };
    static float in[N];
    static float out[N];

    for (int i = 0; i < N; ++i) {
        float t = (float)i / (float)(N - 1);
        in[i] = -5.0f + 10.0f * t;
    }

    /* accuracy: compare AVX2 output to scalar V5 */
    {
        exp10_v7_avx2_kernel(in, out, N);

        double max_abs_ulp = 0.0;
        double sum_abs_ulp = 0.0;
        unsigned valid = 0;

        for (int i = 0; i < N; ++i) {
            double ref = (double)exp10_v5(in[i]);
            double ulp = ulp_error_float(ref, out[i]);
            double abs_ulp = fabs(ulp);

            if (!isfinite(abs_ulp)) continue;

            sum_abs_ulp += abs_ulp;
            valid++;

            if (abs_ulp > max_abs_ulp) {
                max_abs_ulp = abs_ulp;
            }
        }

        rr.max_abs_ulp = max_abs_ulp;
        rr.avg_abs_ulp = (valid > 0) ? (sum_abs_ulp / (double)valid) : 0.0;
        rr.worst_x = 0.0f;
    }

    /* throughput only */
    {
        double best = 1e100;

        for (int warm = 0; warm < 50; ++warm) {
            exp10_v7_avx2_kernel(in, out, N);
        }

        for (int rep = 0; rep < repeats; ++rep) {
            uint64_t t0 = start_tsc();

            for (int k = 0; k < bench_n; ++k) {
                exp10_v7_avx2_kernel(in, out, N);
            }

            uint64_t t1 = stop_tsc();

            double total_elems = (double)bench_n * (double)N;
            double cpe = (double)(t1 - t0) / total_elems;
            if (cpe < best) best = cpe;
        }

        rr.throughput_cpe = best;
        rr.latency_cpe = 0.0;
    }

    return rr;
}

void print_header(void) {
    printf("\n");
    printf("---------------------------------------------------------------------------------------------------------------\n");
    printf("| %-7s | %-54s | %-12s | %-12s | %-22s |\n",
           "version", "optimizations made", "throughput", "latency", "accuracy");
    printf("---------------------------------------------------------------------------------------------------------------\n");
}

void print_row(const char *version, const char *opt, const row_result_t *rr, int has_latency, int has_throughput) {
    char thr[32], lat[32], acc[64];

    if (rr->skipped) {
        snprintf(thr, sizeof(thr), "skipped");
        snprintf(lat, sizeof(lat), "skipped");
        snprintf(acc, sizeof(acc), "skipped");
    } else {
        if (has_throughput) snprintf(thr, sizeof(thr), "%.2f cpe", rr->throughput_cpe);
        else snprintf(thr, sizeof(thr), "-");

        if (has_latency) snprintf(lat, sizeof(lat), "%.2f cpe", rr->latency_cpe);
        else snprintf(lat, sizeof(lat), "-");

       snprintf(acc, sizeof(acc), "max=%.2f, avg=%.2f", rr->max_abs_ulp, rr->avg_abs_ulp);
    }

    printf("| %-7s | %-54s | %-12s | %-12s | %-22s |\n",
           version, opt, thr, lat, acc);
}

void print_footer(void) {
    printf("---------------------------------------------------------------------------------------------------------------\n");
    printf("\n");
}