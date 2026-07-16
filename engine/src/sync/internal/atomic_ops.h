#pragma once

#include "platform/platform_features.h"
#include "core/numbers.h"
#include "core/defines.h"

/* Fences */
#define ENGINE_ATOMIC_FULL_BARRIER      __atomic_thread_fence(__ATOMIC_SEQ_CST)
#define ENGINE_ATOMIC_READ_BARRIER      __atomic_thread_fence(__ATOMIC_ACQUIRE)
#define ENGINE_ATOMIC_WRITE_BARRIER     __atomic_thread_fence(__ATOMIC_RELEASE)

/* Spin-wait hint */
#if defined(__x86_64__) || defined(__i386__)
#   define ENGINE_ATOMIC_SPIN_NOP   __builtin_ia32_pause()
#elif defined(__aarch64__)
#   define ENGINE_ATOMIC_SPIN_NOP   __asm__ __volatile__("yield")
#else 
#   define ENGINE_ATOMIC_SPIN_NOP   ((void)0)
#endif

/* Lock-free check */
#define ENGINE_ATOMIC_CHECK_SIZE(mem) \
    _Static_assert(__atomic_always_lock_free(sizeof(*(mem)), (mem)), \
                   "atomic not lock free!")
/* Core operations */

#define engine_atomic_load_relaxed(mem)             \
    (__atomic_load_n((mem), __ATOMIC_RELAXED))

#define engine_atomic_load_acquire(mem)             \
    (__atomic_load_n((mem), __ATOMIC_ACQUIRE))

#define engine_atomic_store_relaxed(mem, val)       \
    __atomic_store_n((mem), (val), __ATOMIC_RELAXED)

#define engine_atomic_store_release(mem, val)       \
    __atomic_store_n((mem), (val), __ATOMIC_RELEASE)

#define engine_atomic_compare_exchange_acquire(mem, expected, desired)      \
    __atomic_compare_exchange_n((mem), (expected), (desired), 0,            \
                                __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)

#define engine_atomic_compare_exchange_release(mem, expected, desired)      \
    __atomic_compare_exchange_n((mem), (expected), (desired), 0,            \
                                __ATOMIC_RELEASE, __ATOMIC_RELAXED)

#define engine_atomic_fetch_add_relaxed(mem, operand)       \
    __atomic_fetch_add((mem), (operand), __ATOMIC_RELAXED)

#define engine_atomic_fetch_add_acquire(mem, operand)       \
    __atomic_fetch_add((mem), (operand), __ATOMIC_ACQUIRE)

#define engine_atomic_fetch_add_release(mem, operand)       \
    __atomic_fetch_add((mem), (operand), __ATOMIC_RELEASE)

#define engine_atomic_fetch_and_relaxed(mem, operand)       \
    __atomic_fetch_and((mem), (operand), __ATOMIC_RELAXED)

#define engine_atomic_fetch_or_relaxed(mem, operand)        \
    __atomic_fetch_or((mem), (operand), __ATOMIC_RELAXED)

#define engine_atomic_exchange_relaxed(mem, val)            \
    __atomic_exchange_n((mem), (val), __ATOMIC_RELAXED)

#define engine_atomic_exchange_acquire(mem, val)            \
    __atomic_exchange_n((mem), (val), __ATOMIC_ACQUIRE)

    #define engine_atomic_exchange_release(mem, val)        \
    __atomic_exchange_n((mem), (val), __ATOMIC_RELEASE)

/* Atomic max */
#define ENGINE_ATOMIC_MAX(ptr, val)                                                 \
    do {                                                                            \
        ENGINE_TYPEOF(*(ptr)) old = __atomic_load_n((ptr), __ATOMIC_RELAXED);       \
        while (old < (val) &&                                                       \
            !__atomic_compare_exchange_n((ptr), &old, (val), 0,                     \
                                            __ATOMIC_SEQ_CST, __ATOMIC_RELAXED));   \
    } while (0)

/* Decrement if positive */
#define engine_atomic_decrement_if_positive(mem)                                    \
    ({                                                                              \
        ENGINE_TYPEOF(*(mem))old;                                                   \
        do {                                                                        \
            old = __atomic_load_n((mem), __ATOMIC_RELAXED);                         \
            if (old <= 0)                                                           \
                break;                                                              \
        } while (!__atomic_compare_exchange_n((mem), &old, old - 1, 0,              \
                                             __ATOMIC_ACQUIRE, __ATOMIC_RELAXED));  \
        old;                                                                        \
    })

#if ENGINE_WORDSIZE == 64
#   define ENGINE_SEM_T_ALIGN       8
#else 
#   define ENGINE_SEM_T_ALIGN       4
#endif