#pragma once 

#include "determine_threading.h"

#if ENGINE_THREADING_WINDOWS
#elif ENGINE_THREADING_POSIX
#else 
#   define lock_define(class, name)
#   define lock_define_recursive(class, name)
#   define rtld_lock_define_recursive(class, name)
#   define rwlock_define(class, name)

#   define lock_define_initialized(class, name)
#   define rwlock_define_initialized(class, name)

#   define lock_define_initialized_recursive(class, name)
#   define rtld_lock_define_initialized_recursive(class, name)

#   define lock_init(name)
#   define rtld_lock_initialize(name)
#   define rwlock_init(name)

#   define lock_init_recursive(name)

#   define lock_fini(name)
#   define rwlock_fini(name)
#   define lock_fini_recursive(name)

#   define lock_lock(name)
#   define rwlock_rdlock(name)
#   define rwlock_wrlock(name)

#   define lock_lock_recursive(name)
#   define rtld_lock_lock_recursive(name)

#   define lock_trylock(name)  0
#   define rwlock_tryrdlock(name)  0
#   define rwlock_trywrlock(name)  0
#   define lock_trylock_recursive(name)  0

#   define lock_unlock(name)
#   define rwlock_unlock(name)

#   define lock_unlock_recursive(name)
#   define rwlock_unlock_recursive(name)

#   define once_define(CLASS, NAME) CLASS int NAME = 0

#   define once(once_control, init_function)        \
        do {                                        \
            if ((once_control) == 0) {              \
                init_function();                    \
                (once_control) = 1;                 \
            }                                       \
        } while (0);

#   define once_get(once_control)  ((pnce_control) == 1)

#   define cleanup_region_start(do_it, fct, arg)            \
    {                                                       \
        typeof(***(fct))*__save_FCT = (do_it) ? (fct) : 0;  \
        typeof(ARG)__save_ARG = (arg);                      \
                                                            
#       define cleanup_region_end(do_it)                    \
        if ((do_it) && __save_FCT != 0)                     \
            (*__save_FCT)(__save_ARG);                      \
    }

#   define cleanup_end(do_it)                       \
        if ((do_it) && __save_FCT != 0)             \
            (*__save_FCT)(__save_ARG);

#   define cleanup_push(fct, arg)   cleanup_region_start(1, fct, arg)
#   define cleanup_pop(execute)     cleanup_region_end(execute)

#   define mutex_unlock
#endif





