#pragma once

#include "../core/numbers.h"
#include "../core/defines.h"

#include "atomic_ops.h"
#include "semaphore.h"
#include "thread_types.h"
#include "cleanup_frame.h"

#include "schedule/schedule_param.h"

#include "signals/signal_set.h"

/* Internal thread attributes struct */
typedef struct engine_thread_attr_internal_s {
    engine_schedule_param sched_param;
    integer_32bit sched_policy;

    integer_32bit flags;        // detach state, inherit/sched, scope, etc.
    engine_size_t guard_size;

    void *stack_addr;
    engine_size_t stack_size;

    signal_set signal_mask;
    integer_32bit signal_mask_set;

    integer_32bit cancel_state;
    integer_32bit cancel_type;
    integer_32bit cancel_pending;

    engine_cleanup_frame *cleanup_stack;
} engine_thread_attr_internal;

/* Thread flags */
#define THREAD_ATTR_FLAG_DETACHSTATE        0x0001
#define THREAD_ATTR_FLAG_NOTINHERITSCHED    0x0002
#define THREAD_ATTR_FLAG_SCOPEPROCESS       0x0004
#define THREAD_ATTR_FLAG_STACKADDR          0x0008
#define THREAD_ATTR_FLAG_SCHED_SET          0x0020
#define THREAD_ATTR_FLAG_POLICY_SET         0x0040
#define THREAD_ATTR_FLAG_SIGMASK_SET        0x0080

/* Transparent thread attributes struct */
typedef union engine_thread_attr_transparent_u {
    engine_thread_attr external;
    engine_thread_attr_internal internal;
} engine_thread_attr_transparent;

/* Mutex attributes internal struct */
typedef struct engine_mutex_attr_internal_s {
    integer_32bit mutex_kind;   // pshared bit + mutex type
} engine_mutex_attr_internal;

/* Condvar attributes internal struct */
typedef struct engine_condvar_attr_internal_s {
    integer_32bit value;        // pshared + engine_clockid bits
} engine_condvar_attr_internal;

/* Rwlock attributes internal struct */
typedef struct engine_rwlock_attr_internal_s {
    integer_32bit lock_kind;
    integer_32bit pshared;
} engine_rwlock_attr_internal;

/* Mutex attributes internal struct */
typedef struct engine_barrier_attr_internal_s {
    integer_32bit_unsigned  in;
    integer_32bit_unsigned  current_round;
    integer_32bit_unsigned  count;
    integer_32bit           shared;
    integer_32bit_unsigned  out;
} engine_barrier_attr_internal;

#define THREAD_BARRIER_THRESHOLD (UINT_MAX / 2)

/* Thread-local key internal struct */
typedef struct engine_thread_key_internal_s {
    integer_pointer_unsigned seq;
    void (*destr)(void *);
} engine_thread_key_internal;

#define THREAD_KEY_UNUSED(seq)      (((seq) & 1) == 0)
#define THREAD_KEY_USABLE(seq)      ((integer_pointer_unsigned)(seq) < (integer_pointer_unsigned)((seq) + 2))

/* Semaphore internal struct */
typedef struct engine_semaphore_internal_s {
    integer_32bit  value;       // current semaphore count
    integer_32bit  shared;      // 0 = private, 1 = process-shared
    integer_32bit  futex_word;  // Linux futex or platform wait primitive
    integer_32bit  pad;         // alignment / future use
} engine_semaphore_internal;


#if defined(__cplusplus)
static_assert(sizeof(sem_t) >= sizeof(engine_semaphore_internal), 
               "sem_t must be large enough");

static_assert(ENGINE_ALIGNOF(sem_t) >= ENGINE_ALIGNOF(engine_semaphore_internal),
               "sem_t must be aligned enough");
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
_Static_assert(sizeof(sem_t) >= sizeof(engine_semaphore_internal), 
               "sem_t must be large enough");

_Static_assert(ENGINE_ALIGNOF(sem_t) >= ENGINE_ALIGNOF(engine_semaphore_internal),
               "sem_t must be aligned enough");
#endif

/* Rwlock internal flag check */
#define THREAD_RWLOCK_RECURSIVE(rwlock)     \
    ((rwlock)->__data.__flags != 0)