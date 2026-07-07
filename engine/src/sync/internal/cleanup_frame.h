#pragma once

#include "../core/numbers.h"

/* Cancellation handling with integration into exception handling */

/* Engine cleanup frame 

   This structure represents a single cleanup handler install by
   'engine_cleanup_push'. Cleanup handlers are executed:
   
        - when a thread is canceled
        - when 'engine_thread_exit' is called
        - when 'engine_cleanup_pop' is invoked with 'execute' != 0
    
   Frames are stored in a per-thread LIFO stack */
typedef struct engine_cleanup_frame_s {
    void (*routine)(void *);                // cleanup function
    void *arg;                              // argument to routine
    integer_32bit do_it;                    // execute on pop if non-zero
    integer_32bit cancel_type_at_push;      // cancellation type at push
    struct engine_cleanup_frame_s *prev;    // previous frame in stack
} engine_cleanup_frame;

