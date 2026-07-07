#pragma once

#include "../platform/platform_features.h"
#include "time_type.h"

typedef struct engine_timespec_s {
    integer_64bit tv_sec;   // seconds
    integer_64bit tv_nsec;  // nanoseconds
} engine_timespec; 