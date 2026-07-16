#pragma once

#include "core/numbers.h"
#include "platform/platform_features.h"
#include "timer/internal/time_specification.h"

#if ENGINE_HAS_FUTEX
integer_32bit engine_futex_wait_timed (
    volatile integer_32bit *futex_word,
    integer_32bit expected,
    const engine_timespec *timeout,
    integer_32bit private_flag
);

integer_32bit engine_futex_wake (
    volatile integer_32bit *futex_word,
    integer_32bit num_to_wake,
    integer_32bit private_flag
);
#endif