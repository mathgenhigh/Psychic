#pragma once

#include "core/numbers.h"

#if ENGINE_HAS_FUTEX
/* Linux futex(2) operation codes and the engine's private/shared futex
   convention. Included only when ENGINE_HAS_FUTEX is true - see
   platform_features.h. This header only defines constants; the actual
   syscall invocation lives in internal/futex_backend.h's Linux .c file */

/* Raw Linux futex op codes actually used by this engine */
#define ENGINE_FUTEX_WAIT           0
#define ENGINE_FUTEX_WAKE           1
#define ENGINE_FUTEX_WAIT_BITSET    9
#define ENGINE_FUTEX_WAKE_BITSET    10

#define ENGINE_FUTEX_PRIVATE_FLAG       128
#define ENGINE_FUTEX_CLOCK_REALTIME     256
#define ENGINE_FUTEX_BITSET_MATCH_ANY   0xFFFFFFFFU

/* Mirrors engine_process_state (ENGINE_THREADING_PROCESS_PRIVATE/
   SHARED): whether a futex word is only ever touched by threads
   within this process, or shared across processes via shared memory */
#define ENGINE_LLL_PRIVATE  0
#define ENGINE_LLL_SHARED   ENGINE_FUTEX_PRIVATE_FLAG

/* Combines a base futex op with private/shared flag */
static inline integer_32bit engine_futex_private_flag (
    integer_32bit op,
    integer_32bit private_flag
) {
    return (op | ENGINE_FUTEX_PRIVATE_FLAG) ^ private_flag;
}

/* 'CLOCK_REALTIME' / 'CLOCK_MONOTONIC' are the only clocks the 'futex()'
   syscall itself accepts for timed waits */
static inline bool_8bit engine_futex_supported_clockid (integer_32bit clock_id) {
    return clock_id == CLOCK_REALTIME || clock_id == CLOCK_MONOTONIC;
}

#endif