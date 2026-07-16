#include "sync/schedule/schedule.h"
#include <sched.h>

static integer_32bit map_polic_to_posix (engine_sched_policy policy) {
    switch (policy) {
        case ENGINE_THREADING_SCHED_OTHER: return SCHED_OTHER;
        case ENGINE_THREADING_SCHED_FIFO:  return SCHED_FIFO;
        case ENGINE_THREADING_SCHED_RR:    return SCHED_RR;
#if ENGINE_HAS_SCHED_BATCH_IDLE
        case ENGINE_THREADING_SCHED_BATCH: return SCHED_BATCH;
        case ENGINE_THREADING_SCHED_IDLE:  return SCHED_IDLE;
#endif
        default: return -1;
    }
}

integer_32bit engine_sched_get_priority_min (engine_sched_policy policy) {
    integer_32bit posix_policy = map_policy_to_posix(policy);
    if (posix_policy < 0)
        return -1;

    return sched_get_priority_min(posix_policy);
}

integer_32bit engine_sched_get_priority_max (engine_sched_policy policy) {
    integer_32bit posix_policy = map_polic_to_posix(policy);
    if (posix_policy < 0)
        return -1;

    return sched_get_priority_max(posix_policy);
}