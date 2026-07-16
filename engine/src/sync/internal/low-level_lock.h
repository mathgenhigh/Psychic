#pragma once

#include "core/numbers.h"
#include "core/defines.h"

#include "internal/atomic_ops.h"
#include "internal/futex_backend.h"

/* Core futex-based lock state machine: 0 = unlocked, 1 = locked/no
   waiters, 2 = locked/possibly waiters. Fast path (uncontended
   acquire/release) is pure atomic ops, zero syscalls. Only blocks
   or wakes via the futex backend when actually contended */

#define ENGINE_LLL_LOCK_INITIALIZER         0
#define ENGINE_LLL_LOCK_INITIALIZER_LOCKED  1

/* Try to acquire: 0 -> 1: Returns true if acquired, false otherwise.
   Never blocks, never touches syscall path */
static inline bool_8bit engine_lll_trylock (integer_32bit *lock) {
    integer_32bit expected = 0;
    return engine_atomic_compare_exchange_acquire(lock, &expected, 1);
}

/* Same, but sets state to 2 (locked, possibly-waiters). Used by the
   condvar broadcast path once/if the requeue optimization is added -
   safe to use without it too; unlock just does one avoidable wake */
static inline bool_8bit engine_lll_condvar_trylock (integer_32bit *lock) {
    integer_32bit expected = 0;
    return engine_atomic_compare_exchange_acquire(lock, &expected, 2);
}

/* Blocks until 'lock' is acquired, marking it "possibly waiters"
   before each futex_wait so the eventual unlock knows to wake us */
static inline void engine_lll_lock_wait (
    integer_32bit *lock,
    integer_32bit private_flag
) {
    integer_32bit cur;
    while ((cur = engine_atomic_load_acquire(lock)) != 0) {
        if (cur == 1) 
            if (!engine_atomic_compare_exchange_acquire(lock, &cur, 2)) 
                continue;   // raced with a concurrent unlock; retry

        engine_futex_wait_timed(lock, 2, ENGINE_NULLPTR, private_flag);
    }
}

/* Acquire the lock, blocking if necessary */
static inline void engine_lll_lock (
    integer_32bit *lock,
    integer_32bit private_flag
) {
    integer_32bit expected = 0;
    if (!engine_atomic_compare_exchange_acquire(lock, &expected, 1))
        engine_lll_lock_wait(lock, private_flag);
}

/* Unconditional variant: always sets state to 2, blocks if it wasn't 0 */
static inline void engine_lll_condvar_block (
    integer_32bit *lock,
    integer_32bit private_flag
) {
    if (engine_atomic_exchange_acquire(lock, 2) != 0)
        engine_lll_lock_wait(lock, private_flag);
}

/* Release the lock. Wakes one waiter if state was >1. 'private_flag'
   is a function parameter (already a local copy) - this preserves the
   ordering: nothing touches 'lock' after the release exchange except 
   reading the OLD value already captured, avoiding a use-after-free 
   if another thread acquires and frees the lock's memory immediately 
   after release */
static inline void engine_lll_unlock (
    integer_32bit *lock,
    integer_32bit private_flag
) {
    integer_32bit old_val = engine_atomic_exchange_release(lock, 0);
    if (old_val > 1)
        engine_futex_wake(lock, 1, private_flag);
}

static inline bool_8bit engine_lll_islocked (integer_32bit lock_value) {
    return lock_value != ENGINE_LLL_LOCK_INITIALIZER;
}