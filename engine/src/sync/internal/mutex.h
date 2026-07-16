#pragma once

#include "thread_types_arch.h"

/* Binary-compatible mutex layout with glibc's __pthread_mutex_s.
   Size: 32 bytes on LP32, 40 bytes on LP64 */

/* Internal intrusive list types (for wait queues, etc.) */
typedef struct engine_internal_mutex_list_s {
    struct engine_internal_mutex_list_s *prev;
    struct engine_internal_mutex_list_s *next;
} engine_mutex_list;

typedef struct engine_internal_mutex_slist_s {
    struct engine_internal_mutex_slist_s *next;
} engine_mutex_slist;

/* Mutex internal representation (engine ABI) */
typedef struct engine_mutex_s {
    integer_32bit lock ENGINE_ALIGN(ENGINE_LOCK_ALIGNMENT);
    integer_32bit_unsigned count;
    integer_32bit owner;

#if ENGINE_WORDSIZE == 64
    integer_32bit_unsigned nusers;
#endif

    integer_32bit kind;

#if ENGINE_WORDSIZE != 64
    integer_32bit_unsigned nusers;
#endif

#if ENGINE_WORDSIZE == 64
    integer_32bit spins;
    engine_mutex_list list;
#   define ENGINE_MUTEX_HAVE_PREV   1
#else 
    union {
        integer_32bit spins;
        engine_slist list;
    };
#   define ENGINE_MUTEX_HAVE_PREV   0
#endif
} engine_mutex;

#if ENGINE_MUTEX_HAVE_PREV == 1
#   define ENGINE_MUTEX_INITIALIZER(kind_value)     \
        { 0, 0, 0, 0, (kind_value), 0, { 0, 0 } }
#else 
#   define ENGINE_MUTEX_INITIALIZER(kind_value)     \
        { 0, 0, 0, (kind_value), 0, { 0 } }
#endif