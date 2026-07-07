#pragma once

#include "../core/numbers.h"

/* Schedule struct to describe a process' schedulability */
typedef struct engine_schedule_param_s {
    integer_32bit sched_priority;
} engine_schedule_param;