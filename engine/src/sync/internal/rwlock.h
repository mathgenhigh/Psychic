#pragma once

#include "../core/numbers.h"

/* Binary-compatible rwlock layout with glibc's __pthread_rwlock_arch_t.
   Size: 32 bytes on LP32 and LP64 */

/* RW-lock internal representation */
typedef struct engine_rwlock_s {
    integer_32bit_unsigned  readers;
    integer_32bit_unsigned  writers;
    integer_32bit_unsigned  wrphase_futex;
    integer_32bit_unsigned  writers_futex;
    integer_32bit_unsigned  pad3;
    integer_32bit_unsigned  pad4;

#if ENGINE_BYTE_ORDER == ENGINE_BIG_ENDIAN
    integer_8bit pad1;
    integer_8bit pad2;
    integer_8bit shared;
    integer_8bit flags;
#else 
    integer_8bit flags;
    integer_8bit shared;
    integer_8bit pad1;
    integer_8bit pad2;
#endif

    integer_32bit cur_writer;
} engine_rwlock;

#if ENGINE_BYTE_ORDER == ENGINE_BIG_ENDIAN
#   define ENGINE_RWLOCK_INITIALIZER(flags_value)       \
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, (flags_value), 0 }
#else 
#   define ENGINE_RWLOCK_INITIALIZER(flags_value)       \
        { 0, 0, 0, 0, 0, 0, (flags_value), 0, 0, 0, 0 }
#endif