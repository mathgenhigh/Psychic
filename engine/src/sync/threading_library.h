#pragma once

#include "../platform/platform_features.h"

#include "internal/thread_types.h"
#include "internal/internal_types.h"
#include "internal/atomic_ops.h"
#include "internal/cpu_affinity.h"

#include "set_jump/set_jump.h"

#include "timer/internal/time_specification.h"
#include "timer/internal/clock_id.h"

#include "signals/signal_set.h"

#include "schedule/schedule.h"

/* Thread detach state */
typedef enum engine_detach_state_e {
    ENGINE_THREADING_CREATE_JOINABLE = 0,
    ENGINE_THREADING_CREATE_DETACHED = 1
} engine_detach_state;

#define THREADING_CREATE_JOINABLE  ENGINE_THREADING_CREATE_JOINABLE
#define THREADING_CREATE_DETACHED  ENGINE_THREADING_CREATE_DETACHED

/* Mutex types */
typedef enum engine_mutex_types_e {
    ENGINE_THREADING_MUTEX_TIMED_NP = 0,
    ENGINE_THREADING_MUTEX_RECURSIVE_NP = 1,
    ENGINE_THREADING_MUTEX_ERRORCHECK_NP = 2,
    ENGINE_THREADING_MUTEX_ADAPTIVE_NP = 3,

    /* POSIX names */
    ENGINE_THREADING_MUTEX_NORMAL     = ENGINE_THREADING_MUTEX_TIMED_NP,
    ENGINE_THREADING_MUTEX_RECURSIVE  = ENGINE_THREADING_MUTEX_RECURSIVE_NP,
    ENGINE_THREADING_MUTEX_ERRORCHECK = ENGINE_THREADING_MUTEX_ERRORCHECK_NP,
    ENGINE_THREADING_MUTEX_DEFAULT    = ENGINE_THREADING_MUTEX_NORMAL,
    
    /* GNU compatibility */
    ENGINE_THREADING_MUTEX_FAST_NP    = ENGINE_THREADING_MUTEX_TIMED_NP
} engine_mutex_types;

#define THREADING_MUTEX_TIMED_NP        ENGINE_THREADING_MUTEX_TIMED_NP
#define THREADING_MUTEX_RECURSIVE_NP    ENGINE_THREADING_MUTEX_RECURSIVE_NP
#define THREADING_MUTEX_ERRORCHECK_NP   ENGINE_THREADING_MUTEX_ERRORCHECK_NP
#define THREADING_MUTEX_ADAPTIVE_NP     ENGINE_THREADING_MUTEX_ADAPTIVE_NP

#define THREADING_MUTEX_NORMAL          ENGINE_THREADING_MUTEX_NORMAL
#define THREADING_MUTEX_RECURSIVE       ENGINE_THREADING_MUTEX_RECURSIVE
#define THREADING_MUTEX_ERRORCHECK      ENGINE_THREADING_MUTEX_ERRORCHECK
#define THREADING_MUTEX_DEFAULT         ENGINE_THREADING_MUTEX_DEFAULT

#define THREADING_MUTEX_FAST_NP         ENGINE_THREADING_MUTEX_FAST_NP

typedef enum engine_mutex_robust_e {
    ENGINE_THREADING_MUTEX_STALLED = 0,
    ENGINE_THREADING_MUTEX_ROBUST  = 1,
} engine_mutex_robust;

#define THREADING_MUTEX_STALLED     ENGINE_THREADING_MUTEX_STALLED
#define THREADING_MUTEX_STALLED_NP  ENGINE_THREADING_MUTEX_STALLED
#define THREADING_MUTEX_ROBUST      ENGINE_THREADING_MUTEX_ROBUST
#define THREADING_MUTEX_ROBUST_NP   ENGINE_THREADING_MUTEX_ROBUST

typedef enum engine_mutex_protocol_e {
    ENGINE_THREADING_PRIO_NONE = 0,
    ENGINE_THREADING_PRIO_INHERIT = 1,
    ENGINE_THREADING_PRIO_PROTECT = 2
} engine_mutex_protocol;

#define THREADING_PRIO_NONE     ENGINE_THREADING_PRIO_NONE
#define THREADING_PRIO_INHERIT  ENGINE_THREADING_PRIO_INHERIT
#define THREADING_PRIO_PROTECT  ENGINE_THREADING_PRIO_PROTECT

#define THREADING_MUTEX_INITIALIZER \
    { { ENGINE_MUTEX_INITIALIZER(THREADING_MUTEX_DEFAULT) } }

#define THREADING_RECURSIVE_MUTEX_INITIALIZER_NP \
    { { ENGINE_MUTEX_INITIALIZER(THREADING_MUTEX_RECURSIVE_NP) } }

#define THREADING_ERRORCHECK_MUTEX_INITIALIZER_NP \
    { { ENGINE_MUTEX_INITIALIZER(THREADING_MUTEX_ERRORCHECK_NP) } }

#define THREADING_ADAPTIVE_MUTEX_INITIALIZER_NP \
    { { ENGINE_MUTEX_INITIALIZER(THREADING_MUTEX_ADAPTIVE_NP) } }

/* Read-write lock types */
typedef enum engine_rwlock_types_e {
    ENGINE_THREADING_RWLOCK_PREFER_READER_NP = 0,
    ENGINE_THREADING_RWLOCK_PREFER_WRITER_NP = 1,
    ENGINE_THREADING_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP = 2,

    ENGINE_THREADING_RWLOCK_DEFAULT_NP = ENGINE_THREADING_RWLOCK_PREFER_READER_NP
} engine_rwlock_types;

#define THREADING_RWLOCK_PREFER_READER_NP   ENGINE_THREADING_RWLOCK_PREFER_READER_NP
#define THREADING_RWLOCK_PREFER_WRITER_NP   ENGINE_THREADING_RWLOCK_PREFER_WRITER_NP
#define THREADING_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP  ENGINE_THREADING_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP
#define THREADING_RWLOCK_DEFAULT_NP         ENGINE_THREADING_RWLOCK_DEFAULT_NP

#define THREADING_RWLOCK_INITIALIZER    \
    { { ENGINE_RWLOCK_INITIALIZER(THREADING_RWLOCK_DEFAULT_NP) } }

#define THREADING_RWLOCK_WRITER_NONRECURSIVE_INITIALIZER_NP     \
    { { ENGINE_RWLOCK_INITIALIZER(THREADING_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP) } }

/* Scheduler inheritance */
typedef enum engine_inherit_schedule_e {
    ENGINE_THREADING_INHERIT_SCHED  = 0,
    ENGINE_THREADING_EXPLICIT_SCHED = 1
} engine_inherit_schedule;

#define THREADING_INHERIT_SCHED     ENGINE_THREADING_INHERIT_SCHED
#define THREADING_EXPLICIT_SCHED    ENGINE_THREADING_EXPLICIT_SCHED

/* Scope handling */
typedef enum engine_scope_e {
    ENGINE_THREADING_SCOPE_SYSTEM  = 0,
    ENGINE_THREADING_SCOPE_PROCESS = 1
} engine_scope;

#define THREADING_SCOPE_SYSTEM      ENGINE_THREADING_SCOPE_SYSTEM
#define THREADING_SCOPE_PROCESS     ENGINE_THREADING_SCOPE_PROCESS

/* Process shared or private flag */
typedef enum engine_process_state_e {
    ENGINE_THREADING_PROCESS_PRIVATE = 0,
    ENGINE_THREADING_PROCESS_SHARED  = 1
} engine_process_state;

#define THREADING_PROCESS_PRIVATE   ENGINE_THREADING_PROCESS_PRIVATE
#define THREADING_PROCESS_SHARED    ENGINE_THREADING_PROCESS_SHARED

#define THREADING_CONDVAR_INITIALIZER   \
    { { {0}, {0}, {0, 0}, 0, 0, {0, 0}, 0, 0 } }

/* Cleanup buffers */
typedef struct engine_threading_cleanup_buffer_s {
    void (*routine)(void *);                        // Function to call 
    void *arg;                                      // Argument
    integer_32bit cancel_type;                      // Saved cancellation type
    struct engine_threading_cleanup_buffer_s *prev; // Linked list
} engine_threading_cleanup_buffer;

/* Cancellation state */
typedef enum engine_cancel_state_e {
    ENGINE_THREAD_CANCEL_ENABLE  = 0,
    ENGINE_THREAD_CANCEL_DISABLE = 1
} engine_cancel_state;

#define THREADING_CANCEL_ENABLE     ENGINE_THREAD_CANCEL_ENABLE
#define THREADING_CANCEL_DISABLE    ENGINE_THREAD_CANCEL_DISABLE

/* Cancellation type */
typedef enum engine_cancel_type_e {
    ENGINE_THREAD_CANCEL_DEFERRED     = 0,
    ENGINE_THREAD_CANCEL_ASYNCHRONOUS = 1
} engine_cancel_type;

#define THREADING_CANCEL_DEFERRED       ENGINE_THREAD_CANCEL_DEFERRED
#define THREADING_CANCEL_ASYNCHRONOUS   ENGINE_THREAD_CANCEL_ASYNCHRONOUS

/* Returned by 'thread_join' when a thread is canceled */
#define THREAD_CANCELED  ((void *)-1)

/* Single execution handling */
#define THREADING_ONCE_INIT  0

/* Create a new thread */
integer_32bit engine_thread_create (
    engine_thread_id *new_thread,
    const engine_thread_attr *attr,
    void *(*start_routine)(void *),
    void *arg
) ENGINE_THROWNL;

/* Terminate calling thread */
void engine_thread_exit (void *retval) ENGINE_NORETURN;

/* Wait for thread termination */
integer_32bit engine_thread_join (engine_thread_id th, void **thread_return);

/* GNU extension: tryjoin */
integer_32bit engine_thread_tryjoin_np (engine_thread_id th, void **thread_return);

/* Timed join */
integer_32bit engine_thread_timedjoin_np (
    engine_thread_id th,
    void **thread_return,
    const engine_timespec *abs_time
);

/* Clock-based timed join */
integer_32bit engine_thread_clockjoin_np (
    engine_thread_id th,
    void **thread_return,
    engine_clockid clock_id,
    const engine_timespec *abs_time
);

/* Detach a thread */
integer_32bit engine_thread_detach (engine_thread_id th) ENGINE_THROW;

/* Get current thread ID */
engine_thread_id engine_thread_self (void) ENGINE_THROW ENGINE_CONSTFN;

/* Compare two thread IDs */
integer_32bit engine_threads_equal (engine_thread_id th1, engine_thread_id th2) ENGINE_THROW ENGINE_CONSTFN;


/* Thread attribute handling */

/* Initialize thread attribute with default attributes */
integer_32bit engine_thread_attr_init (engine_thread_attr *attr) ENGINE_THROW ENGINE_NONNULL(1);

/* Destroy thread attribute */
integer_32bit engine_thread_attr_destroy (engine_thread_attr *attr) ENGINE_THROW ENGINE_NONNULL(1);

/* Get detach state attribute */
integer_32bit engine_thread_attr_getdetachstate (
    const engine_thread_attr *attr,
    integer_32bit *detach_state 
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set detach state attribute */
integer_32bit engine_thread_attr_setdetachstate (
    const engine_thread_attr *attr,
    integer_32bit detach_state
) ENGINE_THROW ENGINE_NONNULL(1);

/* Get the size of the guard area created for stack overflow protection */
integer_32bit engine_thread_attr_getguardsize (
    const engine_thread_attr *attr,
    engine_size_t *guard_size
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set the size of the guard area created for stack overflow protection */
integer_32bit engine_thread_attr_setguardsize (
    const engine_thread_attr *attr,
    engine_size_t guard_size
) ENGINE_THROW ENGINE_NONNULL(1);

/* Return in parameter the scheduling parameters of attribute */
integer_32bit engine_thread_attr_getschedparam (
    const engine_thread_attr *ENGINE_RESTRICT attr,
    engine_schedule_param *ENGINE_RESTRICT param
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set scheduling parameters (priority, etc) in attribute according to parameter */
integer_32bit engine_thread_attr_setschedparam (
    engine_thread_attr *ENGINE_RESTRICT attr,
    const engine_schedule_param *ENGINE_RESTRICT param
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Return in policy the scheduling policy of attribute */
integer_32bit engine_thread_attr_getschedpolicy (
    const engine_thread_attr *ENGINE_RESTRICT attr,
    integer_32bit *ENGINE_RESTRICT policy
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set scheduling policy in attribute according to policy */
integer_32bit engine_thread_attr_setschedpolicy (
    engine_thread_attr *attr,
    integer_32bit policy
) ENGINE_THROW ENGINE_NONNULL(1);

/* Return in inherit the scheduling inheritance mode of attribute */
integer_32bit engine_thread_attr_getinheritsched (
    const engine_thread_attr *ENGINE_RESTRICT attr,
    integer_32bit *ENGINE_RESTRICT inherit
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set scheduling inheritance mode in attribute according to inherit */
integer_32bit engine_thread_attr_setinheritsched (
    engine_thread_attr *attr,
    integer_32bit inherit
) ENGINE_THROW ENGINE_NONNULL(1);

/* Return in scope the scheduling contention scope of attribute */
integer_32bit engine_thread_attr_getscope (
    const engine_thread_attr *ENGINE_RESTRICT attr,
    integer_32bit *ENGINE_RESTRICT scope
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set scheduling contention scope in attribute according to scope */
integer_32bit engine_thread_attr_setscope (
    engine_thread_attr *attr,
    integer_32bit scope
) ENGINE_THROW ENGINE_NONNULL(1);

/* Return the previously set address for the stack */
integer_32bit engine_thread_attr_getstackaddr (
    engine_thread_attr *ENGINE_RESTRICT attr,
    void **ENGINE_RESTRICT stack_addr
) ENGINE_THROW ENGINE_NONNULL(1, 2) ENGINE_DEPRECATED;

/* Set the starting address of the stack of the thread to be created.
   Depending on whether the stack grows up or down, the value must either
   be higher or lower than all the address in the memory block. The minimal
   size of the block must be 'THREAD_STACK_MIN' */
integer_32bit engine_thread_attr_setstackaddr (
    engine_thread_attr *attr,
    void *stack_addr
) ENGINE_THROW ENGINE_NONNULL(1) ENGINE_DEPRECATED;

/* Return the currently used minimal stack size */
integer_32bit engine_thread_attr_getstacksize (
    const engine_thread_attr *ENGINE_RESTRICT attr,
    engine_size_t *ENGINE_RESTRICT stack_size
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Add information about the minimum stack size needed for the thread 
   to be started. This size must never be less than 'THREAD_STACK_MIN' 
   and must also not exceed the system limits */
integer_32bit engine_thread_attr_setstacksize (
    engine_thread_attr *attr,
    engine_size_t stack_size
) ENGINE_THROW ENGINE_NONNULL(1);

/* Return the previously set address for the stack */
integer_32bit engine_thread_attr_getstack (
    const engine_thread_attr *ENGINE_RESTRICT attr,
    void **ENGINE_RESTRICT stack_addr,
    engine_size_t *ENGINE_RESTRICT stack_size
) ENGINE_THROW ENGINE_NONNULL(1, 2, 3);

/* The following two interfaces are intended to replaced the last two. They
   require setting the address as well as the size since only setting the 
   address will make the implementation on some architectures impossible */
integer_32bit engine_thread_attr_setstack (
    engine_thread_attr *attr,
    void *stack_addr,
    engine_size_t stack_size
) ENGINE_THROW ENGINE_NONNULL(1);

/* Thread created with attribute will be limited to run only on the processors 
   represented in mask */
integer_32bit engine_thread_attr_setaffinity_np (
    engine_thread_attr *attr,
    const engine_cpu_mask *mask
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Get CPU mask representing the processors threads created with attribute
   can run on */
integer_32bit engine_thread_attr_getaffinity_np (
    const engine_thread_attr *attr,
    engine_cpu_mask *mask
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Get the default attributes used by 'engine_thread_create' in this process */
integer_32bit engine_thread_getattr_default_np (engine_thread_attr *attr) ENGINE_THROW ENGINE_NONNULL(1);

/* Store signal mask as the signal mask for the new thread in attribute */
integer_32bit engine_thread_attr_setsigmask_np (
    engine_thread_attr *attr,
    const signal_set *signal_mask
);

/* Store the signal mask of attribute in 'signal_mask'. If there is no signal
   mask stored, return 'THREAD_ATTR_NO_SIGMASK_NP'. Return zero on success */
integer_32bit engine_thread_attr_getsigmask_np (
    const engine_thread_attr *attr,
    signal_set *signal_mask 
);

/* Special return value from 'engine_thread_attr_getsigmask_np' if the signal
   mask has not been set */
#define THREAD_ATTR_NO_SIGMASK_NP  (-1)

/* Set the default attributes to be used by 'engine_thread_create' in 
   this process */
integer_32bit engine_thread_setattr_default_np (const engine_thread_attr *attr) ENGINE_THROW ENGINE_NONNULL(1);

/* Initialize thread attribute with attributes corresponding to the 
   already running thread. It shall be called on uninitialized attribute
   and destroyed with 'engine_thread_destroy' when no longer needed */
integer_32bit engine_thread_getattr_np (
    engine_thread_id thread,
    engine_thread_attr *attr
) ENGINE_THROW ENGINE_NONNULL(2);


/* Functions for scheduling control */

/* Set the scheduling parameters for target thread according to policy
   and parameter */
integer_32bit engine_thread_setschedparam (
    engine_thread_id target_thread,
    integer_32bit policy,
    const engine_schedule_param *param
) ENGINE_THROW ENGINE_NONNULL(3);

/* Return in policy and parameter the scheduling parameters for target thread */
integer_32bit engine_thread_getschedparam (
    engine_thread_id target_thread,
    integer_32bit *ENGINE_RESTRICT policy,
    engine_schedule_param *ENGINE_RESTRICT param
) ENGINE_THROW ENGINE_NONNULL(2, 3);

/* Set the scheduling priority for target thread */
integer_32bit engine_thread_setschedprio (
    engine_thread_id target_thread, 
    integer_32bit prio
) ENGINE_THROW;

/* Get thread name visible in the kernel and its interfaces */
integer_32bit engine_thread_getname_np (
    engine_thread_id target_thread,
    integer_8bit *buf,
    engine_size_t buf_len
) ENGINE_THROW ENGINE_NONNULL(2);

/* Set thread name visible in the kernel and its interfaces */
integer_32bit engine_thread_setname_np (
    engine_thread_id target_thread,
    const integer_8bit *name
) ENGINE_THROW ENGINE_NONNULL(2);

/* Limit specified thread to run only on the processors represented
   in mask */
integer_32bit engine_thread_setaffinity_np (
    engine_thread_id thread,
    const engine_cpu_mask *mask
) ENGINE_THROW ENGINE_NONNULL(2);

/* Get CPU mask representing the processors thread can run on */
integer_32bit engine_thread_getaffinity_np (
    engine_thread_id thread,
    engine_cpu_mask *mask
) ENGINE_THROW ENGINE_NONNULL(2);


/* Functions for handling initialization */

/* Ensure that 'init_routine' is executed exactly once for the lifetime of
   the process, even if multiple threads call 'engine_thread_once' with
   the same 'once_control' object.
   
   'once_control' must reference a static or global variable initialized 
   to 'THREAD_ONCE_INIT'. The initialization routine may throw in C++
   environments, therefore this function is intentionally "not" marked
   with 'ENGINE_THROW' */
integer_32bit engine_thread_init_once (
    engine_thread_once *once_control,
    void (*init_routine)(void) 
) ENGINE_NONNULL(1, 2);


/* Functions for handling cancellation */

/* Set the cancellation state of the calling thread to 'state'.
   If 'old_state' is non-NULL, store the previous cancellation state there.
   
   This function is not marked with 'ENGINE_THROW' because cancellation may be
   implemented via stack unwinding in C++ environments */
integer_32bit engine_thread_setcancelstate (
    integer_32bit state,
    integer_32bit *old_state
);

/* Set the cancellation type of the calling thread to 'type'.
   If 'old_type' is non-NULL, store the previous cancellation type there.
   
   As with 'engine_thread_setcancelstate', this function must not be marked
   with 'ENGINE_THROW' to allow compilers to generated unwind information */
integer_32bit engine_thread_setcanceltype (
    integer_32bit type,
    integer_32bit *old_type
);

/* Request cancellation of thread. Depending of the cancellation type
   of the target thread, cancellation may occur immediately or at the
   next defined cancellation point 
   
   This function does not use 'ENGINE_THROW' to allow unwind information */
integer_32bit engine_thread_cancel (engine_thread_id thread);

/* Check whether the calling thread has a pending cancellation request.
   If so, terminate the thread as if 'engine_thread_exit(THREAD_CANCELED)'
   had been invoked.
   
   This function is a cancellation point and must not be marked with 'ENGINE_THROW' */
void engine_thread_testcancel (void);

#ifdef __cplusplus

/* RAII guard equivalent to glibc's '__pthread_cleanup_class', but with
   no dependency on '__GNUC__', '__EXCEPTIONS', or any GCC-only attribute.
   Runs 'routine(arg)' when the guard is destroyed - via normal scope
   exit, via 'ENGINE_CLEANUP_POP(1)', or via an exception unwinding
   through the scope - unless disarmed with 'ENGINE_CLEANUP_POP(0)'.
   
   This does NOT participate in 'engine_thread_cancel()'s cleanup stack;
   it only reacts to C++-visible unwinding (return, break, throw) */
class engine_cleanup_guard {
    public:
        engine_cleanup_guard (void (*routine)(void *), void *arg) noexcept
            : routine_(routine), arg_(arg), do_it_(1) {}

        ~engine_cleanup_guard () {
            if (do_it_)
                routine_(arg_);
        }

        /* Called by 'ENGINE_CLEANUP_POP' - decides wheter the handler runs */
        void finish (int execute) noexcept { do_it_ = execute; }

        engine_cleanup_guard (const engine_cleanup_guard &) = delete;
        engine_cleanup_guard &operator= (const engine_cleanup_guard &) = delete;

    private:
        void (*routine_)(void *);
        void *arg_;
        integer_32bit do_it_;
};

/* Usage mirrors 'pthread_cleanup_push/pop' exactly: must be used in
   matching pairs at the same brace-nesting level. Nesting works because
   each pair opens/closes its own block; the guard name doesn't need to
   be unique because inner scopes simply shadow outer ones */
#define ENGINE_CLEANUP_PUSH(routine, arg)       \
    do {                                        \
        engine_cleanup_guard engine_clframe_((routine), (arg))

#define ENGINE_CLEANUP_POP(execute)             \
        engine_clframe_.finish((execute));      \
    } while (0)

#endif


/* Mutex handling */

/* Initialize a mutex */
integer_32bit engine_mutex_init (
    engine_mutex *mutex,
    const engine_mutex_attr *mutex_attr
) ENGINE_THROW ENGINE_NONNULL(1);

/* Destroy a mutex */
integer_32bit engine_mutex_destroy (engine_mutex *mutex) ENGINE_THROW ENGINE_NONNULL(1);

/* Try locking a mutex */
integer_32bit engine_mutex_trylock (engine_mutex *mutex) ENGINE_THROWNL ENGINE_NONNULL(1);

/* Lock a mutex */
integer_32bit engine_mutex_lock (engine_mutex *mutex) ENGINE_THROWNL ENGINE_NONNULL(1);

/* Wait until lock becomes available, or specified time passes */
integer_32bit engine_mutex_timedlock (
    engine_mutex *ENGINE_RESTRICT mutex,
    const engine_timespec *ENGINE_RESTRICT abs_time
) ENGINE_THROWNL ENGINE_NONNULL(1, 2);

/* Wait until lock becomes available, or until absoule time passes
   according to the specified clock */
integer_32bit engine_mutex_clocklock (
    engine_mutex *ENGINE_RESTRICT mutex,
    engine_clockid clock_id,
    const engine_timespec *ENGINE_RESTRICT abs_time
) ENGINE_THROWNL ENGINE_NONNULL(1, 3);

/* Unlock a mutex */
integer_32bit engine_mutex_unlock (engine_mutex *mutex) ENGINE_THROWNL ENGINE_NONNULL(1);

/* Get the priority ceiling of mutex */
integer_32bit engine_mutex_getprioceiling (
    const engine_mutex *ENGINE_RESTRICT mutex,
    integer_32bit *ENGINE_RESTRICT prio_ceiling
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set the priority ceiling of mutex to 'prio_ceiling', return old
   priority ceiling value in 'old_ceiling' */
integer_32bit engine_mutex_setprioceiling (
    engine_mutex *ENGINE_RESTRICT mutex,
    integer_32bit prio_ceiling,
    integer_32bit *ENGINE_RESTRICT old_ceiling
) ENGINE_THROW ENGINE_NONNULL(1, 3);


/* Functions for handling mutex attributes */

/* Initialize mutex attribute object 'attr' with default attributes 
   (kind is 'THREADING_MUTEX_TIMED_NP') */
integer_32bit engine_mutex_attr_init (engine_mutex_attr *attr) ENGINE_THROW ENGINE_NONNULL(1);

/* Destroy mutex attribute object 'attr' */
integer_32bit engine_mutex_attr_destroy (engine_mutex_attr *attr) ENGINE_THROW ENGINE_NONNULL(1);

/* Get the process-shared flag of the mutex attribute 'attr' */
integer_32bit engine_mutex_attr_getpshared (
    const engine_mutex_attr *ENGINE_RESTRICT attr,
    integer_32bit *ENGINE_RESTRICT p_shared
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set the process-shared flag of the mutex attribute 'attr' */
integer_32bit engine_mutex_attr_setpshared (
    engine_mutex_attr *attr,
    integer_32bit p_shared
) ENGINE_THROW ENGINE_NONNULL(1);

/* Get the mutex type (normal, recursive, errorcheck, adaptive) */
integer_32bit engine_mutex_attr_gettype (
    const engine_mutex_attr *ENGINE_RESTRICT attr,
    integer_32bit *ENGINE_RESTRICT kind
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set the mutex type */
integer_32bit engine_mutex_attr_settype (
    engine_mutex_attr *attr,
    integer_32bit kind
) ENGINE_THROW ENGINE_NONNULL(1);

/* Get the mutex protocol */
integer_32bit engine_mutex_attr_getprotocol (
    const engine_mutex_attr *ENGINE_RESTRICT attr,
    integer_32bit *ENGINE_RESTRICT protocol
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set the mutex protocol */
integer_32bit engine_mutex_attr_setprotocol (
    engine_mutex_attr *attr,
    integer_32bit protocol
) ENGINE_THROW ENGINE_NONNULL(1);

/* Get the mutex priority ceiling attribute */
integer_32bit engine_mutex_attr_getprioceiling (
    const engine_mutex_attr *ENGINE_RESTRICT attr,
    integer_32bit *ENGINE_RESTRICT prio_ceiling
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set the mutex priority ceiling attribute */
integer_32bit engine_mutex_attr_setprioceiling (
    engine_mutex_attr *attr,
    integer_32bit prio_ceiling
) ENGINE_THROW ENGINE_NONNULL(1);


/* Functions for handling read-write locks */

/* Initialize read-write lock using attributes, or use
   the default values if later is NULL */
integer_32bit engine_rwlock_init (
    engine_rwlock *ENGINE_RESTRICT rwlock,
    const engine_rwlock_attr_internal *ENGINE_RESTRICT attr
) ENGINE_THROW ENGINE_NONNULL(1);

/* Destroy read-write lock */
integer_32bit engine_rwlock_destroy (engine_rwlock *rwlock) ENGINE_THROW ENGINE_NONNULL(1);

/* Acquire read block for rwlock */
integer_32bit engine_rwlock_rdlock (engine_rwlock *rwlock) ENGINE_THROWNL ENGINE_NONNULL(1);

/* Try to acquire read lock for rwlock */
integer_32bit engine_rwlock_tryrdlock (engine_rwlock *rwlock) ENGINE_THROWNL ENGINE_NONNULL(1);

/* Timed read lock */
integer_32bit engine_rwlock_timedrdlock (
    engine_rwlock *ENGINE_RESTRICT rwlock,
    const engine_timespec *ENGINE_RESTRICT abs_time
) ENGINE_THROWNL ENGINE_NONNULL(1, 2);

/* Clock-based timed read lock */
integer_32bit engine_rwlock_clockrdlock (
    engine_rwlock *ENGINE_RESTRICT rwlock,
    engine_clockid clock_id,
    const engine_timespec *ENGINE_RESTRICT abs_time
) ENGINE_THROWNL ENGINE_NONNULL(1, 3);

/* Acquire a write lock */
integer_32bit engine_rwlock_wrlock (engine_rwlock *rwlock) ENGINE_THROWNL ENGINE_NONNULL(1);

/* Try to acquire a write lock without blocking */
integer_32bit engine_rwlock_trywrlock (engine_rwlock *rwlock) ENGINE_THROWNL ENGINE_NONNULL(1);

/* Timed write lock */
integer_32bit engine_rwlock_timedwrlock (
    engine_rwlock *ENGINE_RESTRICT rwlock,
    const engine_timespec *ENGINE_RESTRICT abs_time
) ENGINE_THROWNL ENGINE_NONNULL(1, 2);

/* Clock-based timed write lock */
integer_32bit engine_rwlock_clockwrlock (
    engine_rwlock *ENGINE_RESTRICT rwlock,
    engine_clockid clock_id,
    const engine_timespec *ENGINE_RESTRICT abs_time
) ENGINE_THROWNL ENGINE_NONNULL(1, 3);

/* Unlock a read-write lock */
integer_32bit engine_rwlock_unlock (engine_rwlock *rwlock) ENGINE_THROWNL ENGINE_NONNULL(1);


/* Functions for handling read-write lock attributes */

/* Initialize rwlock attribute object 'attr' with default values */
integer_32bit engine_rwlock_attr_init (engine_rwlock_attr_internal *attr) ENGINE_THROW ENGINE_NONNULL(1);

/* Destroy rwlock attribute object 'attr' */
integer_32bit engine_rwlock_attr_destroy (engine_rwlock_attr_internal *attr) ENGINE_THROW ENGINE_NONNULL(1);

/* Return current setting of process-shared attribute of 'attr' in 'pshared' */
integer_32bit engine_rwlock_attr_getpshared (
    const engine_rwlock_attr_internal *ENGINE_RESTRICT attr,
    integer_32bit *ENGINE_RESTRICT pshared
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set process-shared rwlock attribute of 'attr' to 'pshared' */
integer_32bit engine_rwlock_attr_setpshared (
    engine_rwlock_attr_internal *attr,
    integer_32bit pshared
) ENGINE_THROW ENGINE_NONNULL(1);

/* Return current setting of reader/writer preference */
integer_32bit engine_rwlock_attr_getkind_np (
    const engine_rwlock_attr_internal *ENGINE_RESTRICT attr,
    integer_32bit *ENGINE_RESTRICT pref
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set reader/writer preference */
integer_32bit engine_rwlock_attr_setkind_np (
    engine_rwlock_attr_internal *attr,
    integer_32bit pref
) ENGINE_THROW ENGINE_NONNULL(1);


/* Functions for handling condition variable attributes */

/* Initialize condition variable attribute 'attr' */
integer_32bit engine_condvar_init (
    engine_condvar *ENGINE_RESTRICT cond,
    const engine_condvar_attr *ENGINE_RESTRICT cond_attr
) ENGINE_THROW ENGINE_NONNULL(1);

/* Destroy condition variable 'cond' */
integer_32bit engine_condvar_destroy (engine_condvar *cond) ENGINE_THROW ENGINE_NONNULL(1);

/* Wake up one thread waiting on condition variable 'cond' */
integer_32bit engine_condvar_signal (engine_condvar *cond) ENGINE_THROWNL ENGINE_NONNULL(1);

/* Wake up all threads waiting for condition variables 'cond' */
integer_32bit engine_condvar_broadcast (engine_condvar *cond) ENGINE_THROWNL ENGINE_NONNULL(1);

/* Wait for condition variable 'cond' to be signaled or broadcast.
   'mutex' is assumed to be locked before.
   
   This is a cancellation point and therefore this function is not
   marked with 'ENGINE_THROW' */
integer_32bit engine_condvar_wait (
    engine_condvar *ENGINE_RESTRICT cond,
    engine_mutex *ENGINE_RESTRICT mutex
) ENGINE_NONNULL(1, 2);

/* Wait for condition variable 'cond' until absolute time 'abs_time'.
   'mutex' is assumed to be locked before.
   
   This is a cancellation point and therefore this function is not
   marked with 'ENGINE_THROW' */
integer_32bit engine_condvar_timedwait (
    engine_condvar *ENGINE_RESTRICT cond,
    engine_mutex *ENGINE_RESTRICT mutex,
    const engine_timespec *ENGINE_RESTRICT abs_time
) ENGINE_NONNULL(1, 2, 3);

/* Wait for condition variable 'cond' using a specific clock 'clock_id'
   until absolute time 'abs_time'.
   'mutex' is assumed to be locked before. 
   
   This is a cancellation point and therefore this function is not
   marked with 'ENGINE_THROW' */
integer_32bit engine_condvar_clockwait (
    engine_condvar *ENGINE_RESTRICT condvar,
    engine_mutex *ENGINE_RESTRICT mutex,
    engine_clockid clock_id,
    const engine_timespec *ENGINE_RESTRICT abs_time
) ENGINE_NONNULL(1, 2, 4);


/* Functions for handling condition variable attributes */

/* Initialize condition variable attribute 'attr' */
integer_32bit engine_condvar_attr_init (engine_condvar_attr *attr) ENGINE_THROW ENGINE_NONNULL(1);

/* Destroy condition varialbe attribute 'attr' */
integer_32bit engine_condvar_attr_destroy (engine_condvar_attr *attr) ENGINE_THROW ENGINE_NONNULL(1);

/* Get the process-shared flag 'pshared' of the condition variable attribute 'attr' */
integer_32bit engine_condvar_attr_getpshared (
    engine_condvar_attr *ENGINE_RESTRICT attr,
    integer_32bit *ENGINE_RESTRICT pshared
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set the process-shared flag 'pshared' of the condition variable attribute 'attr' */
integer_32bit engine_condvar_attr_setpshared (
    engine_condvar_attr *attr,
    integer_32bit pshared
) ENGINE_THROW ENGINE_NONNULL(1);

/* Get the clock 'clock_id' selected for the condition variable attribute 'attr' */
integer_32bit engine_condvar_attr_getclock (
    engine_condvar_attr *ENGINE_RESTRICT attr,
    engine_clockid *ENGINE_RESTRICT clock_id
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set the clock 'clock_id' selected for the condition variable attribute 'attr' */
integer_32bit engine_condvar_attr_setclock (
    engine_condvar_attr *attr,
    engine_clockid clock_id
) ENGINE_THROW ENGINE_NONNULL(1);


/* Functions to handle spinlocks */

/* Initialize the spinlock 'lock'. If 'pshared' is non-zero the spinlock
   can be shared between different processes */
integer_32bit engine_spinlock_init (
    engine_spinlock *lock,
    integer_32bit pshared
) ENGINE_THROW ENGINE_NONNULL(1);

/* Destroy the spinlock 'lock' */
integer_32bit engine_spinlock_destroy (engine_spinlock *lock) ENGINE_THROW ENGINE_NONNULL(1);

/* Acquire the spinlock 'lock', spinning until it becomes available */
integer_32bit engine_spinlock_lock (engine_spinlock *lock) ENGINE_THROWNL ENGINE_NONNULL(1);

/* Try to acquire the spinlock 'lock' without blocking */
integer_32bit engine_spinlock_trylock (engine_spinlock *lock) ENGINE_THROWNL ENGINE_NONNULL(1);

/* Release the spinlock 'lock' */
integer_32bit engine_spin_unlock (engine_spinlock *lock) ENGINE_THROWNL ENGINE_NONNULL(1);


/* Functions to handle barriers */

/* Initialize 'barrier' with the attributes in 'attr'. The barrier is
   opened when 'count' waiters arrived */
integer_32bit engine_barrier_init (
    engine_barrier *ENGINE_RESTRICT barrier,
    const engine_barrier_attr *ENGINE_RESTRICT attr,
    integer_32bit_unsigned count
) ENGINE_THROW ENGINE_NONNULL(1);

/* Destroy a previously dynamically initialized barrier 'barrier' */
integer_32bit engine_barrier_destroy (engine_barrier *barrier) ENGINE_THROW ENGINE_NONNULL(1);

/* Wait on barrier 'barrier' */
integer_32bit engine_barrier_wait (engine_barrier *barrier) ENGINE_THROWNL ENGINE_NONNULL(1);


/* Initialize barrier attribute 'attr' */
integer_32bit engine_barrier_attr_init (engine_barrier_attr *attr) ENGINE_THROW ENGINE_NONNULL(1);

/* Destroy previously dynamically initialized barrier attribute 'attr' */
integer_32bit engine_barrier_attr_destroy (engine_barrier_attr *attr) ENGINE_THROW ENGINE_NONNULL(1);

/* Get the process-shared flag 'pshared' of the barrier attribute 'attr' */
integer_32bit engine_barrier_attr_getpshared (
    engine_barrier_attr *ENGINE_RESTRICT attr,
    integer_32bit *ENGINE_RESTRICT pshared
) ENGINE_THROW ENGINE_NONNULL(1, 2);

/* Set the process-shared flag 'pshared' of the barrier attribute 'attr' */
integer_32bit engine_barrier_attr_setpshared (
    engine_barrier_attr *attr,
    integer_32bit pshared
) ENGINE_THROW ENGINE_NONNULL(1);


/* Functions for handling thread-specific data */

/* Create a key value identifying a location in the thread-specific
   data area. Each thread maintains a distinct thread-specific data
   area. 'destr_function', if non-NULL, is called with the value
   associated to that key when the key is destroyed.
   'destr_function' is not called if the value associated is NULL when
   the key is destroyed */
integer_32bit engine_thread_key_create (
    engine_thread_key *key,
    void (*destr_function)(void *)
) ENGINE_THROW ENGINE_NONNULL(1);

/* Destroy thread's key 'key' */
integer_32bit engine_thread_key_destroy (engine_thread_key *key) ENGINE_THROW;

/* Return current value of the thread-specific data slot identified by 'key' */
void *engine_thread_key_getspecific (engine_thread_key *key) ENGINE_THROW;

/* Store 'pointer' in the thread-specific data slot identified by 'key' */
integer_32bit engine_thread_key_setspecific (
    engine_thread_key *key,
    const void *pointer
) ENGINE_THROW ENGINE_ATTR_ACCESS_NONE(2);

