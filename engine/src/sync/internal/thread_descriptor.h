#pragma once

#include "core/numbers.h"
#include "core/engine_limits.h"
#include "schedule/schedule.h"
#include "signals/signal_set.h"

#include "internal/thread_types.h"
#include "internal/cleanup_frame.h"
#include "internal/intrusive_list.h"
#include "internal/process_id.h" 

/* Thread lifecycle state */
typedef enum engine_thread_state_e {
    ENGINE_THREAD_STATE_EXITED   = 0,
    ENGINE_THREAD_STATE_EXITING  = 1,
    ENGINE_THREAD_STATE_JOINABLE = 2,
    ENGINE_THREAD_STATE_DETACHED = 3
} engine_thread_state;

/* Cancellation bitmask */
#define ENGINE_THREAD_CANCELSTATE_BIT      0
#define ENGINE_THREAD_CANCELSTATE_MASK     (1 << ENGINE_THREAD_CANCELSTATE_BIT)

#define ENGINE_THREAD_CANCELTYPE_BIT       1
#define ENGINE_THREAD_CANCELTYPE_MASK      (1 << ENGINE_THREAD_CANCELTYPE_BIT)

#define ENGINE_THREAD_CANCELING_BIT        2
#define ENGINE_THREAD_CANCELING_MASK       (1 << ENGINE_THREAD_CANCELING_BIT)

#define ENGINE_THREAD_CANCELED_BIT         3
#define ENGINE_THREAD_CANCELED_MASK        (1 << ENGINE_THREAD_CANCELED_BIT)

#define ENGINE_THREAD_EXITING_BIT          4
#define ENGINE_THREAD_EXITING_MASK         (1 << ENGINE_THREAD_EXITING_BIT)

#define ENGINE_THREAD_TERMINATED_BIT       5
#define ENGINE_THREAD_TERMINATED_MASK      (1 << ENGINE_THREAD_TERMINATED_BIT)

static inline bool_8bit engine_thread_cancel_enabled (integer_32bit v) {
    return (v & ENGINE_THREAD_CANCELSTATE_MASK) == 0;
}

static inline bool_8bit engine_thread_cancel_async_enabled (integer_32bit v) {
    return (v & ENGINE_THREAD_CANCELTYPE_MASK) != 0;
}

static inline bool_8bit engine_thread_cancel_exiting (integer_32bit v) {
    return (v & ENGINE_THREAD_EXITING_MASK) != 0;
}

static inline bool_8bit engine_thread_cancel_enabled_and_canceled (integer_32bit v) {
    return (v & (ENGINE_THREAD_CANCELSTATE_MASK | ENGINE_THREAD_CANCELED_MASK |
                 ENGINE_THREAD_EXITING_MASK | ENGINE_THREAD_TERMINATED_MASK))
            == ENGINE_THREAD_CANCELED_MASK;
}

static inline bool_8bit engine_thread_cancel_enabled_and_canceled_async (integer_32bit v) {
    return (v & (ENGINE_THREAD_CANCELSTATE_MASK | ENGINE_THREAD_CANCELTYPE_MASK |
                 ENGINE_THREAD_CANCELED_MASK | ENGINE_THREAD_EXITING_MASK |
                 ENGINE_THREAD_TERMINATED_MASK))
            == (ENGINE_THREAD_CANCELTYPE_MASK | ENGINE_THREAD_CANCELED_MASK);
}

/* Stack guard mode */
typedef enum engine_stack_guard_mode_e {
    ENGINE_STACK_GUARD_ADVISORY  = 0,   // madvise(MADV_GUARD) / equivalent hint
    ENGINE_STACK_GUARD_PROT_NONE = 1,   // hard no-access page
    ENGINE_STACK_GUARD_USER      = 2    // caller-provided stack, no engine guard
} engine_stack_guard_mode;

/* Thread-specific data (TSD) 
   Simplified two-level sparse array: a flat, fixed-capacity table */
#ifndef ENGINE_THREAD_KEYS_MAX
#   define ENGINE_THREAD_KEYS_MAX  128
#endif

typedef struct engine_thread_key_data_s {
    integer_32bit_unsigned seq;     // generation counter vs. destroyed keys
    void *data;
} engine_thread_key_data;

/* Thread descriptor */
typedef struct engine_thread_descriptor_s {
    /* Intrusive link for the engine's live-thread registry */
    engine_list_node list_node;

    /* OS-level identifiers */
    engine_thread_id tid;
    engine_process_id owner_process;

    /* Cleanup handler stack */
    engine_cleanup_frame cleanup_stack;

    /* Cancellation state */
    integer_32bit cancel_handling;

    /* Flags copied from thread attributes at creation time */
    integer_32bit flags;

    /* Thread-specific data */
    engine_thread_key_data specific[ENGINE_THREAD_KEYS_MAX];
    bool_8bit specific_used;

    bool_8bit report_events;
    engine_stack_guard_mode stack_mode;
    bool_8bit stopped_start;
    integer_32bit setup_failed;

    /* Low-level lock protecting this descriptor's own fields */
    integer_32bit self_lock;

    engine_thread_state join_state;
    void *result;

    engine_schedule_param sched_param;
    integer_32bit sched_policy;

    void *(*state_routine)(void *);
    void *arg;

    /* Stack bookkeeping */
    void *stack_block;
    engine_size_t stack_block_size;
    engine_size_t guard_size;
    engine_size_t reported_guard_size;

    /* Signal mask to restore at thread startup */
    signal_set signal_mask;

    bool_8bit exiting;
    integer_32bit exit_lock;
} engine_thread_descriptor;
