#pragma once

#include "mutex.h"
#include "rwlock.h"

typedef struct engine_spinlock_s {
    integer_32bit value;    // 0 = unlocked, 1 = locked
    integer_32bit pshared;  // 0 = private, 1 = process-shared
} engine_spinlock;

/* Common threading primitives definitions for engine, inspired
   by glib'c thread-shared-types.h, but with engine's different
   own ABI and semantics */

/* Wide counter type for condvars/sequence numbers */
typedef struct {
    integer_64bit_unsigned value;
} engine_atomic_wide_counter;

/* Condvar internal representation */
typedef struct engine_condvar_s {
    engine_atomic_wide_counter wseq;        // wait sequence 
    engine_atomic_wide_counter g1_start;    // group start 
    integer_32bit_unsigned  g_size[2];      // group sizes
    integer_32bit_unsigned  g1_orig_size;
    integer_32bit_unsigned  wrefs;          // waiters refs 
    integer_32bit_unsigned  g_signals[2];
} engine_condvar;

/* Barrier internal representation */
typedef struct engine_barrier_s {
    integer_32bit_unsigned  count;
    integer_32bit_unsigned  trip;
    integer_32bit_unsigned  flags;
    integer_32bit_unsigned  pad;
} engine_barrier;

/* Thread and TSS types for C11-style APIs */
typedef integer_32bit_unsigned  engine_tss;
typedef integer_64bit_unsigned  engine_thrd;

/* Once flag */
typedef struct {
    integer_32bit data;
} engine_once_flag;

#define ENGINE_ONCE_FLAG_INIT  { 0 }
