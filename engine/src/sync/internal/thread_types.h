#pragma once

#include "thread_shared_types.h"

/* Thread identifier (engine-level) */
typedef integer_64bit_unsigned engine_thread_id;

/* Mutex attributes (opaque) */
typedef union {
    integer_8bit  __size[ENGINE_SIZEOF_MUTEXATTR_T];
    integer_32bit __align;
} engine_mutex_attr;

/* Condvar attributes (opaque) */
typedef union {
    integer_8bit  __size[ENGINE_SIZEOF_CONDATTR_T];
    integer_32bit __align;
} engine_condvar_attr;

/* Thread-specific key */
typedef integer_32bit_unsigned engine_thread_key;

/* Once-only execution */
typedef integer_32bit ENGINE_ALIGN(ENGINE_ONCE_ALIGNMENT) engine_thread_once;

/* Thread attributes (opaque) */
typedef union engine_thread_attr_u {
    integer_8bit __size[ENGINE_WORDSIZE == 64 ? 56 : 36];
    long int     __align;
} engine_thread_attr;

/* Mutex type (opaque, but with internal view) */
typedef union engine_mutex_u {
    engine_mutex __data;
    integer_8bit __size[ENGINE_SIZEOF_MUTEX_T];
    long int     __align;
} engine_mutex_type;

/* Condvar type */
typedef union engine_condvar_u {
    engine_condvar __data;
    integer_8bit   __size[ENGINE_SIZEOF_COND_T];
    long long int  __align;
} engine_condvar_type;

/* RW-lock type */
typedef union engine_rwlock_u {
    engine_rwlock __data;
    integer_8bit  __size[ENGINE_SIZEOF_RWLOCK_T];
    long int      __align;
} engine_rwlock_type;

/* Spinlock type */
typedef union engine_spinlock_u {
    engine_spinlock __data;
    integer_8bit    __size[ENGINE_SIZEOF_SPINLOCK_T];
    long int        __align;
} engine_spinlock_type;

/* Barrier type */
typedef union engine_barrier_u {
    engine_barrier __data;
    integer_8bit   __size[ENGINE_SIZEOF_BARRIER_T];
    long int       __align; 
} engine_barrier_type;

/* Barrier attributes */
typedef union engine_barrier_attr_u {
    integer_8bit    __size[ENGINE_SIZEOF_BARRIERATTR_T];
    integer_32bit   __align;
} engine_barrier_attr;