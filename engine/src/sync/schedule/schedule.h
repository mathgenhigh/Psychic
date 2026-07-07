#pragma once

#include "../core/numbers.h"
#include "../core/defines.h"
#include "schedule_param.h"

/* Engine scheduling policies (POSIX + optional) */

/* POSIX policies */
#define PROCESS_SCHED_OTHER   0
#define PROCESS_SCHED_FIFO    1
#define PROCESS_SCHED_RR      2

/* Optional (ignored on non-Linux platforms) */
#define PROCESS_SCHED_BATCH   3
#define PROCESS_SCHED_IDLE    5

/* Engine schedule API */

static inline void engine_sched_param_init (engine_schedule_param *p, integer_32bit prio) {
    if (p)
        p->sched_priority = prio;
}

static inline integer_32bit engine_sched_param_get (const engine_schedule_param *p) {
    return p ? p->sched_priority : 0;
}

