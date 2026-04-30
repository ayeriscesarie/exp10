#ifndef ACCURACY_LOGGER_H
#define ACCURACY_LOGGER_H

#include "common.h"

void write_accuracy_report_for_version(
    const char *filename,
    const char *version_name,
    scalar_fn_t fn,
    int samples_per_range,
    double max_allowed_ulp
);

#endif