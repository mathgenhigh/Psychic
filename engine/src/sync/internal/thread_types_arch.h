#pragma once

#include "../core/numbers.h"
#include "../core/defines.h"

/* Engine-specific pthread-like type sizes.
   These sizes correspond this engine's internal structs,
   not glibc's ABI */

#if ENGINE_WORDSIZE == 64
    /* Per-arch size macros */
#   define ENGINE_SIZEOF_MUTEX_T        40
#   define ENGINE_SIZEOF_COND_T         48
#   define ENGINE_SIZEOF_RWLOCK_T       32
#   define ENGINE_SIZEOF_BARRIER_T      32
#else 
#   define ENGINE_SIZEOF_MUTEX_T        32
#   define ENGINE_SIZEOF_RWLOCK_T       48
#   define ENGINE_SIZEOF_COND_T         32
#   define ENGINE_SIZEOF_BARRIER_T      20
#endif

#define ENGINE_SIZEOF_ONCE_FLAG_T    sizeof(integer_32bit)

#define ENGINE_SIZEOF_MUTEXATTR_T       4
#define ENGINE_SIZEOF_CONDATTR_T        4
#define ENGINE_SIZEOF_RWLOCKATTR_T      4
#define ENGINE_SIZEOF_BARRIERATTR_T     4 
#define ENGINE_SIZEOF_SPINLOCK_T        8

/* Alignment constraints for internal lock fields used with atomics/futex */
#define ENGINE_LOCK_ALIGNMENT       ENGINE_ALIGNOF(integer_32bit_unsigned)
#define ENGINE_ONCE_ALIGNMENT       ENGINE_ALIGNOF(integer_32bit)