#pragma once

#include "core/defines.h"
#include "internal/thread_descriptor.h"

/* Set once, per-thread, at thread startup - by the trampoline that
   engine_thread_create's start_routine wrapper runs before invoking
   the user's actual start_routine. NULL for the "main"/initial thread
   until 'engine_thread_self_descriptor_init()' is called for it too -
   see note below */
extern ENGINE_THREAD_LOCAL engine_thread_descriptor *engine_current_thread_descriptor;

/* Returns the calling thread's own descriptor. Never NULL for any
   thread that went through the engine's creation path (including the
   initial/main thread, once explicitly initialized - see
   'engine_thread_self_descriptor_init'). Calling this before that
   initialization has happened is a caller bug, not a condition this
   function checks for - matching the "descriptor always exists for a
   properly-started engine thread" invariant the rest of this codebase
   assumes (e.g. engine_do_cancel dereferences the result unconditionally) */
static inline engine_thread_descriptor *engine_thread_self_descriptor (void) {
    return engine_current_thread_descriptor;
}

/* Called once per thread at startup (by the engine_thread_create
   trampoline, and once explicitly for the process's initial thread -
   see thread_library_init.c) to associate this OS thread with its
   engine_thread_descriptor */
ENGINE_NONNULL(1) static inline void engine_thread_self_descriptor_init (engine_thread_descriptor *descriptor) {
    engine_current_thread_descriptor = descriptor;
}