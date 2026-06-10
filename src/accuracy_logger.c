#include "accuracy_logger.h"
#include "common.h"
#include "exp10_versions.h"

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <immintrin.h>
#include <stdint.h>
#include <string.h>
#if defined(_MSC_VER)
#include <intrin.h>
#endif

void exp10_v7_avx2_kernel(
    const float *x,
    float *out,
    size_t n
);
void exp10_v5_avx2_kernel(
    const float *x,
    float *out,
    size_t n
);


 void write_v3_accuracy_report(
    FILE *file,
    int samples_per_range,
    double max_allowed_ulp
)

    {typedef struct {
        float left;
        float right;
        const char *name;
    } range_t;

    static const range_t ranges[] = {
        {-50.0f, -35.0f, "dataset 0.1.0"},
        {-35.0f, -25.0f, "dataset 0.1.1"},
        {-32.0f, -16.0f, "dataset 0.1.2"},
        {-16.0f,  -8.0f, "dataset 0.1.3"},
        { -8.0f,  -4.0f, "dataset 0.1.4"},
        { -4.0f,  -2.0f, "dataset 0.1.5"},
        { -2.0f,  -1.0f, "dataset 0.1.6"},
        { -1.0f,  -0.5f, "dataset 0.1.7"},
        { -0.5f,  -0.25f, "dataset 0.1.8"},
        { -0.25f, -0.125f, "dataset 0.1.9"},
        { -0.125f, 0.0f, "dataset 0.1.10"},
        { 0.0f,    0.125f, "dataset 0.1.11"},
        { 0.125f,  0.25f, "dataset 0.1.12"},
        { 0.25f,   0.5f, "dataset 0.1.13"},
        { 0.5f,    1.0f, "dataset 0.1.14"},
        { 1.0f,    2.0f, "dataset 0.1.15"},
        { 2.0f,    4.0f, "dataset 0.1.16"},
        { 4.0f,    8.0f, "dataset 0.1.17"},
        { 8.0f,   16.0f, "dataset 0.1.18"},
        {16.0f,   32.0f, "dataset 0.1.19"},
        {32.0f,   40.0f, "dataset 0.1.20"},
    };

    int fail_count = 0;
    int range_count =
        (int)(sizeof(ranges) / sizeof(ranges[0]));

    fprintf(file, "\n");
    fprintf(file, "===== Accuracy report for V3_avx =====\n");

    for (int i = 0; i < range_count; ++i) {

        range_t r = ranges[i];

        double max_abs_ulp = 0.0;
        double sum_abs_ulp = 0.0;


        float worst_x = r.left;

        int valid = 0;

        float in[8];
        float out[8];

        for (int k = 0; k < samples_per_range; k += 8) {

            for (int j = 0; j < 8; ++j) {

                int idx = k + j;

                if (idx >= samples_per_range)
                    break;

                float t =
                    (float)idx /
                    (float)(samples_per_range - 1);

                float x =
                    r.left +
                    (r.right - r.left) * t;

                in[j] = x;
            }
exp10_v3_avx2_kernel(in, out, 8);


            for (int j = 0; j < 8; ++j) {

                int idx = k + j;

                if (idx >= samples_per_range)
                    break;

                float x = in[j];

                double ref =
                    powl(10.0L, (long double)x);

                float my = out[j];

                if (!isfinite(ref) || !isfinite(my)) {
                    continue;
                }

                double ulp =
                    ulp_error_float(ref, my);

                double abs_ulp = fabs(ulp);

                if (abs_ulp > max_abs_ulp) {
                    max_abs_ulp = abs_ulp;
                    worst_x = x;
                }

                sum_abs_ulp += abs_ulp;
                valid++;
            }
        }

        double avg_abs_ulp =
            valid > 0
            ? sum_abs_ulp / valid
            : 0.0;

        const char *status =
            max_abs_ulp <= max_allowed_ulp
            ? "PASS"
            : "FAIL";

        if (status[0] == 'F') {
            fail_count++;
        }

        fprintf(
            file,
            "V3_avx: %-4s #  maxULP %+.6e  avgULP %+.6e  "
            "[%+.6e .. %+.6e] : %s\n",

            status,

            max_abs_ulp,
            avg_abs_ulp,

            r.left,
            r.right,

            r.name
        );
    }

    fprintf(
        file,
        "V3_avx: %s ** SUMMARY ** failed ranges: %d / %d\n",

        fail_count == 0
            ? "PASS"
            : "FAIL",

        fail_count,
        range_count
    );
}



 void write_v4_accuracy_report(
    FILE *file,
    int samples_per_range,
    double max_allowed_ulp
)

    {typedef struct {
        float left;
        float right;
        const char *name;
    } range_t;

    static const range_t ranges[] = {
        {-50.0f, -35.0f, "dataset 0.1.0"},
        {-35.0f, -25.0f, "dataset 0.1.1"},
        {-32.0f, -16.0f, "dataset 0.1.2"},
        {-16.0f,  -8.0f, "dataset 0.1.3"},
        { -8.0f,  -4.0f, "dataset 0.1.4"},
        { -4.0f,  -2.0f, "dataset 0.1.5"},
        { -2.0f,  -1.0f, "dataset 0.1.6"},
        { -1.0f,  -0.5f, "dataset 0.1.7"},
        { -0.5f,  -0.25f, "dataset 0.1.8"},
        { -0.25f, -0.125f, "dataset 0.1.9"},
        { -0.125f, 0.0f, "dataset 0.1.10"},
        { 0.0f,    0.125f, "dataset 0.1.11"},
        { 0.125f,  0.25f, "dataset 0.1.12"},
        { 0.25f,   0.5f, "dataset 0.1.13"},
        { 0.5f,    1.0f, "dataset 0.1.14"},
        { 1.0f,    2.0f, "dataset 0.1.15"},
        { 2.0f,    4.0f, "dataset 0.1.16"},
        { 4.0f,    8.0f, "dataset 0.1.17"},
        { 8.0f,   16.0f, "dataset 0.1.18"},
        {16.0f,   32.0f, "dataset 0.1.19"},
        {32.0f,   40.0f, "dataset 0.1.20"},
    };

    int fail_count = 0;
    int range_count =
        (int)(sizeof(ranges) / sizeof(ranges[0]));

    fprintf(file, "\n");
    fprintf(file, "===== Accuracy report for V4_avx =====\n");

    for (int i = 0; i < range_count; ++i) {

        range_t r = ranges[i];

        double max_abs_ulp = 0.0;
        double sum_abs_ulp = 0.0;


        float worst_x = r.left;

        int valid = 0;

        float in[8];
        float out[8];

        for (int k = 0; k < samples_per_range; k += 8) {

            for (int j = 0; j < 8; ++j) {

                int idx = k + j;

                if (idx >= samples_per_range)
                    break;

                float t =
                    (float)idx /
                    (float)(samples_per_range - 1);

                float x =
                    r.left +
                    (r.right - r.left) * t;

                in[j] = x;
            }
exp10_v4_avx2_kernel(in, out, 8);


            for (int j = 0; j < 8; ++j) {

                int idx = k + j;

                if (idx >= samples_per_range)
                    break;

                float x = in[j];

                double ref =
                    powl(10.0L, (long double)x);

                float my = out[j];

                if (!isfinite(ref) || !isfinite(my)) {
                    continue;
                }

                double ulp =
                    ulp_error_float(ref, my);

                double abs_ulp = fabs(ulp);

                if (abs_ulp > max_abs_ulp) {
                    max_abs_ulp = abs_ulp;
                    worst_x = x;
                }

                sum_abs_ulp += abs_ulp;
                valid++;
            }
        }

        double avg_abs_ulp =
            valid > 0
            ? sum_abs_ulp / valid
            : 0.0;

        const char *status =
            max_abs_ulp <= max_allowed_ulp
            ? "PASS"
            : "FAIL";

        if (status[0] == 'F') {
            fail_count++;
        }

        fprintf(
            file,
            "V4_avx: %-4s #  maxULP %+.6e  avgULP %+.6e  "
            "[%+.6e .. %+.6e] : %s\n",

            status,

            max_abs_ulp,
            avg_abs_ulp,

            r.left,
            r.right,

            r.name
        );
    }

    fprintf(
        file,
        "V4_avx: %s ** SUMMARY ** failed ranges: %d / %d\n",

        fail_count == 0
            ? "PASS"
            : "FAIL",

        fail_count,
        range_count
    );
}

void exp10_v5_avx2_kernel(
    const float *x,
    float *out,
    size_t n
);

 void write_v5_accuracy_report(
    FILE *file,
    int samples_per_range,
    double max_allowed_ulp
)


{
    typedef struct {
        float left;
        float right;
        const char *name;
    } range_t;

    static const range_t ranges[] = {
        {-50.0f, -35.0f, "dataset 0.1.0"},
        {-35.0f, -25.0f, "dataset 0.1.1"},
        {-32.0f, -16.0f, "dataset 0.1.2"},
        {-16.0f,  -8.0f, "dataset 0.1.3"},
        { -8.0f,  -4.0f, "dataset 0.1.4"},
        { -4.0f,  -2.0f, "dataset 0.1.5"},
        { -2.0f,  -1.0f, "dataset 0.1.6"},
        { -1.0f,  -0.5f, "dataset 0.1.7"},
        { -0.5f,  -0.25f, "dataset 0.1.8"},
        { -0.25f, -0.125f, "dataset 0.1.9"},
        { -0.125f, 0.0f, "dataset 0.1.10"},
        { 0.0f,    0.125f, "dataset 0.1.11"},
        { 0.125f,  0.25f, "dataset 0.1.12"},
        { 0.25f,   0.5f, "dataset 0.1.13"},
        { 0.5f,    1.0f, "dataset 0.1.14"},
        { 1.0f,    2.0f, "dataset 0.1.15"},
        { 2.0f,    4.0f, "dataset 0.1.16"},
        { 4.0f,    8.0f, "dataset 0.1.17"},
        { 8.0f,   16.0f, "dataset 0.1.18"},
        {16.0f,   32.0f, "dataset 0.1.19"},
        {32.0f,   40.0f, "dataset 0.1.20"},
    };

    int fail_count = 0;
    int range_count =
        (int)(sizeof(ranges) / sizeof(ranges[0]));

    fprintf(file, "\n");
    fprintf(file, "===== Accuracy report for V5_avx =====\n");

    for (int i = 0; i < range_count; ++i) {

        range_t r = ranges[i];

        double max_abs_ulp = 0.0;
        double sum_abs_ulp = 0.0;


        float worst_x = r.left;

        int valid = 0;

        float in[8];
        float out[8];

        for (int k = 0; k < samples_per_range; k += 8) {

            for (int j = 0; j < 8; ++j) {

                int idx = k + j;

                if (idx >= samples_per_range)
                    break;

                float t =
                    (float)idx /
                    (float)(samples_per_range - 1);

                float x =
                    r.left +
                    (r.right - r.left) * t;

                in[j] = x;
            }
exp10_v5_avx2_kernel(in, out, 8);


            for (int j = 0; j < 8; ++j) {

                int idx = k + j;

                if (idx >= samples_per_range)
                    break;

                float x = in[j];

                double ref =
                    powl(10.0L, (long double)x);

                float my = out[j];

                if (!isfinite(ref) || !isfinite(my)) {
                    continue;
                }

                double ulp =
                    ulp_error_float(ref, my);

                double abs_ulp = fabs(ulp);

                if (abs_ulp > max_abs_ulp) {
                    max_abs_ulp = abs_ulp;
                    worst_x = x;
                }

                sum_abs_ulp += abs_ulp;
                valid++;
            }
        }

        double avg_abs_ulp =
            valid > 0
            ? sum_abs_ulp / valid
            : 0.0;

        const char *status =
            max_abs_ulp <= max_allowed_ulp
            ? "PASS"
            : "FAIL";

        if (status[0] == 'F') {
            fail_count++;
        }

        fprintf(
            file,
            "V5_avx: %-4s #  maxULP %+.6e  avgULP %+.6e  "
            "[%+.6e .. %+.6e] : %s\n",

            status,

            max_abs_ulp,
            avg_abs_ulp,

            r.left,
            r.right,

            r.name
        );
    }

    fprintf(
        file,
        "V5_avx: %s ** SUMMARY ** failed ranges: %d / %d\n",

        fail_count == 0
            ? "PASS"
            : "FAIL",

        fail_count,
        range_count
    );
}

static void write_v7_accuracy_report(
    FILE *file,
    int samples_per_range,
    double max_allowed_ulp
)


{
    typedef struct {
        float left;
        float right;
        const char *name;
    } range_t;

    static const range_t ranges[] = {
        {-50.0f, -35.0f, "dataset 0.1.0"},
        {-35.0f, -25.0f, "dataset 0.1.1"},
        {-32.0f, -16.0f, "dataset 0.1.2"},
        {-16.0f,  -8.0f, "dataset 0.1.3"},
        { -8.0f,  -4.0f, "dataset 0.1.4"},
        { -4.0f,  -2.0f, "dataset 0.1.5"},
        { -2.0f,  -1.0f, "dataset 0.1.6"},
        { -1.0f,  -0.5f, "dataset 0.1.7"},
        { -0.5f,  -0.25f, "dataset 0.1.8"},
        { -0.25f, -0.125f, "dataset 0.1.9"},
        { -0.125f, 0.0f, "dataset 0.1.10"},
        { 0.0f,    0.125f, "dataset 0.1.11"},
        { 0.125f,  0.25f, "dataset 0.1.12"},
        { 0.25f,   0.5f, "dataset 0.1.13"},
        { 0.5f,    1.0f, "dataset 0.1.14"},
        { 1.0f,    2.0f, "dataset 0.1.15"},
        { 2.0f,    4.0f, "dataset 0.1.16"},
        { 4.0f,    8.0f, "dataset 0.1.17"},
        { 8.0f,   16.0f, "dataset 0.1.18"},
        {16.0f,   32.0f, "dataset 0.1.19"},
        {32.0f,   40.0f, "dataset 0.1.20"},
    };

    int fail_count = 0;
    int range_count =
        (int)(sizeof(ranges) / sizeof(ranges[0]));

    fprintf(file, "\n");
    fprintf(file, "===== Accuracy report for V7 =====\n");

    for (int i = 0; i < range_count; ++i) {

        range_t r = ranges[i];

        double max_abs_ulp = 0.0;
        double sum_abs_ulp = 0.0;


        float worst_x = r.left;

        int valid = 0;

        float in[8];
        float out[8];

        for (int k = 0; k < samples_per_range; k += 8) {

            for (int j = 0; j < 8; ++j) {

                int idx = k + j;

                if (idx >= samples_per_range)
                    break;

                float t =
                    (float)idx /
                    (float)(samples_per_range - 1);

                float x =
                    r.left +
                    (r.right - r.left) * t;

                in[j] = x;
            }

            exp10_v7_avx2_kernel(in, out, 8);

            for (int j = 0; j < 8; ++j) {

                int idx = k + j;

                if (idx >= samples_per_range)
                    break;

                float x = in[j];

                double ref =
                    powl(10.0L, (long double)x);

                float my = out[j];

                if (!isfinite(ref) || !isfinite(my)) {
                    continue;
                }

                double ulp =
                    ulp_error_float(ref, my);

                double abs_ulp = fabs(ulp);

                if (abs_ulp > max_abs_ulp) {
                    max_abs_ulp = abs_ulp;
                    worst_x = x;
                }

                sum_abs_ulp += abs_ulp;
                valid++;
            }
        }

        double avg_abs_ulp =
            valid > 0
            ? sum_abs_ulp / valid
            : 0.0;

        const char *status =
            max_abs_ulp <= max_allowed_ulp
            ? "PASS"
            : "FAIL";

        if (status[0] == 'F') {
            fail_count++;
        }

        fprintf(
            file,
            "V7: %-4s #  maxULP %+.6e  avgULP %+.6e  "
            "[%+.6e .. %+.6e] : %s\n",

            status,

            max_abs_ulp,
            avg_abs_ulp,

            r.left,
            r.right,

            r.name
        );
    }

    fprintf(
        file,
        "V7: %s ** SUMMARY ** failed ranges: %d / %d\n",

        fail_count == 0
            ? "PASS"
            : "FAIL",

        fail_count,
        range_count
    );
}

static inline uint64_t rdtsc_now(void)
{
#if defined(_MSC_VER)
    return __rdtsc();
#else
    unsigned hi, lo;

    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));

    return ((uint64_t)hi << 32) | lo;
#endif
}

typedef struct {
    float left;
    float right;
    const char *name;
} range_t;

static const range_t ranges[] = {
    {-50.0f, -35.0f, "dataset 0.1.0"},
    {-35.0f, -25.0f, "dataset 0.1.1"},

    {-32.0f, -16.0f, "dataset 0.1.2"},
    {-16.0f,  -8.0f, "dataset 0.1.3"},
    { -8.0f,  -4.0f, "dataset 0.1.4"},
    { -4.0f,  -2.0f, "dataset 0.1.5"},
    { -2.0f,  -1.0f, "dataset 0.1.6"},

    { -1.0f,  -0.5f, "dataset 0.1.7"},
    { -0.5f,  -0.25f, "dataset 0.1.8"},
    { -0.25f, -0.125f, "dataset 0.1.9"},

    { -0.125f, 0.0f, "dataset 0.1.10"},
    {  0.0f,   0.125f, "dataset 0.1.11"},

    { 0.125f, 0.25f, "dataset 0.1.12"},
    { 0.25f,  0.5f,  "dataset 0.1.13"},
    { 0.5f,   1.0f,  "dataset 0.1.14"},

    { 1.0f,   2.0f,  "dataset 0.1.15"},
    { 2.0f,   4.0f,  "dataset 0.1.16"},
    { 4.0f,   8.0f,  "dataset 0.1.17"},
    { 8.0f,   16.0f, "dataset 0.1.18"},
    {16.0f,   32.0f, "dataset 0.1.19"},

    {32.0f,   40.0f, "dataset 0.1.20"}
};

static void test_one_range(
    FILE *file,
    const char *version_name,
    scalar_fn_t fn,
    range_t r,
    int samples,
    double max_allowed_ulp,
    int range_index,
    int *fail_count
) {
    double max_abs_ulp = 0.0;
    double sum_abs_ulp = 0.0;



    float worst_x = r.left;


    int valid = 0;

    for (int i = 0; i < samples; ++i) {
        float t = (float)i / (float)(samples - 1);
        float x = r.left + (r.right - r.left) * t;

        float my = fn(x);
        double ref = powl(10.0L, (long double)x);

        if (!isfinite(ref) || !isfinite(my)) {
            continue;
        }

        double ulp = ulp_error_float(ref, my);
        double abs_ulp = fabs(ulp);

        if (abs_ulp > max_abs_ulp) {
            max_abs_ulp = abs_ulp;
            worst_x = x;
        }



        sum_abs_ulp += abs_ulp;
        valid++;
    }

    double avg_abs_ulp = valid > 0 ? sum_abs_ulp / valid : 0.0;

    const char *status = max_abs_ulp <= max_allowed_ulp ? "PASS" : "FAIL";

    if (status[0] == 'F') {
        (*fail_count)++;
    }

fprintf(
    file,
    "%s: %-4s #  maxULP %.6e at x=%+.6e  "
    "avgULP %.6e  "
    "[%+.6e .. %+.6e] : %s\n",

    version_name,
    status,

    max_abs_ulp,
    worst_x,

    avg_abs_ulp,

    r.left,
    r.right,

    r.name
);
}

void write_accuracy_report_for_version(
    const char *filename,
    const char *version_name,
    scalar_fn_t fn,
    int samples_per_range,
    double max_allowed_ulp
) {
    FILE *file = fopen(filename, "a");
    const int LAT_N = 200000;

    volatile float sink = 0.0f;

    uint64_t t0 = rdtsc_now();

    for (int i = 0; i < LAT_N; ++i) {
        float x = -10.0f + 20.0f * ((float)i / (float)LAT_N);

        sink += fn(x);
    }

uint64_t t1 = rdtsc_now();

double latency_cpe =
    (double)(t1 - t0) / (double)LAT_N;
    if (file == NULL) {
        printf("Cannot open accuracy report file: %s\n", filename);
        return;
    }

    int fail_count = 0;
    int range_count = (int)(sizeof(ranges) / sizeof(ranges[0]));

    fprintf(file, "\n");
    fprintf(file, "===== Accuracy report for %s =====\n", version_name);
    fprintf(file, "Latency: %.2f cpe\n", latency_cpe);
    for (int i = 0; i < range_count; ++i) {
        test_one_range(
            file,
            version_name,
            fn,
            ranges[i],
            samples_per_range,
            max_allowed_ulp,
            i,
            &fail_count
        );
    }

    fprintf(
        file,
        "%s: %s ** SUMMARY ** failed ranges: %d / %d\n",
        version_name,
        fail_count == 0 ? "PASS" : "FAIL",
        fail_count,
        range_count
    );
    printf("version_name = [%s]\n", version_name);
fflush(stdout);
if (strcmp(version_name, "V6") == 0) {
    write_v7_accuracy_report(
        file,
        samples_per_range,
        max_allowed_ulp
    );
}
if (strcmp(version_name, "V3") == 0)
{
    write_v3_accuracy_report(
        file,
        samples_per_range,
        max_allowed_ulp
    );
}
if (strcmp(version_name, "V5") == 0) {
    printf("CALLING V5_AVX REPORT\n");
fflush(stdout);
    write_v5_accuracy_report(
        file,
        samples_per_range,
        max_allowed_ulp
    );
    
}

if (strcmp(version_name, "V4") == 0)
{
    write_v4_accuracy_report(
        file,
        samples_per_range,
        max_allowed_ulp
    );
}
    fclose(file);
}