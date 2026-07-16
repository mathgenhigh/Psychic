#pragma once

#include "core/numbers.h"
#include "core/defines.h"
#include "schedule_param.h"

/* Engine scheduling policies */
typedef enum engine_sched_policy_e {
    ENGINE_THREADING_SCHED_OTHER = 0,
    ENGINE_THREADING_SCHED_FIFO  = 1,
    ENGINE_THREADING_SCHED_RR    = 2,
    ENGINE_THREADING_SCHED_BATCH = 3,
    ENGINE_THREADING_SCHED_IDLE  = 5
} engine_sched_policy;

#define THREADING_SCHED_OTHER  ENGINE_THREADING_SCHED_OTHER
#define THREADING_SCHED_FIFO   ENGINE_THREADING_SCHED_FIFO
#define THREADING_SCHED_RR     ENGINE_THREADING_SCHED_RR
#define THREADING_SCHED_BATCH  ENGINE_THREADING_SCHED_BATCH
#define THREADING_SCHED_IDLE   ENGINE_THREADING_SCHED_IDLE

/* Engine schedule API */

ENGINE_NONNULL(1) static inline void engine_sched_param_init (engine_schedule_param *p, integer_32bit prio) {
    p->sched_priority = prio;
}

ENGINE_NONNULL(1) static inline integer_32bit engine_sched_param_get (const engine_schedule_param *p) {
    return p->sched_priority;
}

/* Returns the minimum valid engine_schedule_param->sched_priority for
   the given policy, or a negative value if 'policy' is invalid.
   For 'ENGINE_THREADING_SCHED_OTHER/BATCH/IDLE', both min and max are 0
   on POSIX platforms - these policies don't use sched_priority for
   in-policy scheduling weight (see nice()-equivalent, not yet ported) */
integer_32bit engine_sched_get_priority_min (engine_sched_policy policy) ENGINE_THROW;

/* Returns the maximum valid priority for the given policy, or a
   negative value if 'policy' is invalid */
integer_32bit engine_sched_get_priority_max (engine_sched_policy policy) ENGINE_THROW;