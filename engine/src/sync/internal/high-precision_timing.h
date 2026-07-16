#pragma once

#include "core/numbers.h"

/* x86/x86_64: rdtsc intrinsic */
#if defined(__x86_64__) || defined(__i386__)
#   include <x86intrin.h>
integer_64bit_unsigned engine_read_cycle_counter (void) { return __rdtsc(); }
#elif defined(__aarch64__)
/* ARM64: CNTVCT_EL0 virtual counter register */
integer_64bit_unsigned engine_read_cycle_counter (void) {
    integer_64bit_unsigned val;
    __asm__ volatile("mrs %0, cntvct_el0": "=r"(val));
    return val;
}
#elif ENGINE_PLATFORM_WINDOWS
/* Portable fallback via QueryPerformanceCounter, not a true cycle
   count but a high-resolution timer - acceptable substitute */
#   include <profileapi.h>
integer_64bit_unsigned engine_read_cycle_counter (void) {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (integer_64bit_unsigned)li.QuadPart;
}
#endif