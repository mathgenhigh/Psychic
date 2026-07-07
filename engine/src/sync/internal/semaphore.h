#pragma once

#include "../core/numbers.h"
#include "../core/defines.h"
#include "timer/internal/time_specification.h"
#include "timer/internal/clock_id.h"
#include "internal_types.h"

#if ENGINE_WORDSIZE == 64
#   define ENGINE_SIZEOF_SEM_T  32
#else 
#   define ENGINE_SIZEOF_SEM_T  16
#endif

typedef union enging_sem_u {
    engine_semaphore_internal __data;
    integer_8bit     __size[ENGINE_SIZEOF_SEM_T];
    long int         __align;
} sem_t;

/* Initialize semaphore */
integer_32bit engine_sem_init (
    sem_t *sem, 
    integer_32bit pshared, 
    integer_32bit_unsigned value
) _throw nonnull(1);

/* Destroy semaphore */
integer_32bit engine_sem_destroy (sem_t *sem) _throw nonnull(1);

/* Open named semaphore */
sem_t *engine_sem_open (const integer_8bit *name, integer_32bit oflag, ...) _throw nonnull(1);

/* Close named semaphore */
integer_32bit engine_sem_close (sem_t *sem) _throw nonnull(1);

/* Unlink named semaphore */
integer_32bit engine_sem_unlink (const integer_8bit *name) _throw nonnull(1);

/* Wait (cancellation point) */
integer_32bit engine_sem_wait (sem_t *sem) nonnull(1);

/* Timed wait */
integer_32bit engine_set_timedwait (sem_t *sem, const engine_timespec *abs_time) nonnull(1, 2);

/* Clock wait */
integer_32bit engine_sem_clockwait (
    sem_t *sem, 
    engine_clockid clock, 
    const engine_timespec *abs_time
) nonnull(1, 3);

/* Try wait */
integer_32bit engine_sem_trywait (sem_t *sem) thrownl nonnull(1);

/* Post */
integer_32bit engine_sem_post (sem_t *sem) thrownl nonnull(1);

/* Get value */
integer_32bit engine_sem_getvalue (sem_t *sem, integer_32bit *sval) _throw nonnull(1, 2);