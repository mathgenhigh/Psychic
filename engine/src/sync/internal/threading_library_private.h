#pragma once

#include "threading_library.h"

#include "core/numbers.h"
#include "core/defines.h"
#include "core/engine_limits.h"

#include "internal/thread_types.h"
#include "internal/thread_defaults.h"
#include "internal/thread_descriptor.h"
#include "internal/thread_self.h"
#include "internal/atomic_ops.h"
#include "internal/cleanup_frame.h"

#include "schedule/schedule.h"

#ifdef __cplusplus 
extern "C" {
#endif

/* Once-control states */

/* Backing states for 'engine_thread_init_once' / 'THREADING_ONCE_INIT' */
#define ENGINE_ONCE_INPROGRESS      1
#define ENGINE_ONCE_DONE            2
#define ENGINE_ONCE_FORK_GEN_INCR   4

/* Condition variable internal bits */
#define ENGINE_CONDVAR_CLOCK_MONOTONIC_MASK    2
#define ENGINE_CONDVAR_SHARED_MASK             4

/* Read-write lock internal state */

/* Backing state machine for 'engine_rwlock_rdlock/wrlock' */
#define ENGINE_RWLOCK_WRPHASE           1
#define ENGINE_RWLOCK_WRLOCKED          2
#define ENGINE_RWLOCK_RWAITING          4
#define ENGINE_RWLOCK_READER_SHIFT      3
#define ENGINE_RWLOCK_READER_OVERFLOW       \
    ((integer_32bit_unsigned)1 << (sizeof(integer_32bit_unsigned) * 8 - 1))
#define ENGINE_RWLOCK_WRHANDOVER        ENGINE_RWLOCK_READER_OVERFLOW
#define ENGINE_RWLOCK_FUTEX_USED        2

/* Mutex kind / process-shared extraction */
static inline integer_32bit engine_mutex_kind (const integer_32bit *kind_word) {
    return engine_atomic_load_relaxed(kind_word) & 127;
}

static inline integer_32bit engine_mutex_pshared (const integer_32bit *kind_word) {
    return engine_atomic_load_relaxed(kind_word) & 128;
}

/* Atomic operations on descriptor members */
#define ENGINE_DESCRIPTOR_CAS(descr, member, old_val, new_val)      \
    engine_atomic_compare_exchange_acquire(&(descr)->member, (old_val), (new_val))

/* Validation helpers */
static inline integer_32bit engine_check_schedule_policy (integer_32bit policy) {
    if (policy == THREADING_SCHED_OTHER ||
        policy == THREADING_SCHED_FIFO  ||
        policy == THREADING_SCHED_RR)
        return 0;

    return -1;    // caller maps to its real error convention
}

/* Requires the scheduling module to expose min/max priority queries
   for a given policy - stub until that surface exists */
static inline integer_32bit engine_check_sched_priority (
    integer_32bit priority,
    engine_sched_policy policy
) {
    integer_32bit min = engine_sched_get_priority_min(policy);
    integer_32bit max = engine_sched_get_priority_max(policy);
    if (min >= 0 && max >= 0 && priority >= min && priority <= max)
        return 0;

    return -1;
}

static inline integer_32bit engine_check_stacksize (engine_size_t size) {
    return (size >= ENGINE_THREAD_STACK_MIN) ? 0 : -1;
}

/* Descriptor validity check */
static inline bool_8bit engine_thread_descriptor_valid (const engine_thread_descriptor *td) {
    return engine_atomic_load_relaxed(&td->join_state) != ENGINE_THREAD_STATE_EXITED;
}

/* Cancellation delivery */
static inline void engine_do_cancel (void *result) {
    engine_thread_descriptor *self = engine_thread_self_descriptor();

    self->result = result;

    integer_32bit old_val = engine_atomic_load_relaxed(&self->cancel_handling);
    integer_32bit new_val;
    do {
        new_val = old_val | ENGINE_THREAD_CANCELSTATE_MASK | ENGINE_THREAD_EXITING_MASK;
        new_val &= ~ENGINE_THREAD_CANCELTYPE_MASK;
        if (old_val == new_val)
            break;
    } while (!ENGINE_DESCRIPTOR_CAS(self, cancel_handling, &old_val, new_val));

    engine_cleanup_run_all();
}

#ifdef __cplusplus
}
#endif