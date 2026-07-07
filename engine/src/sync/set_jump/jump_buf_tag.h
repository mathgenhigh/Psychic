#pragma once

#include "../core/numbers.h"
#include "../signals/signal_set.h"

typedef struct jump_buf_raw_s {
    void *regs[16];
} jump_buf_raw;

/* Calling environment, plus possibly a saved signal mask */
typedef struct jump_buf_tag_s {
    jump_buf_raw      __jmpbuf;           // calling environment           
    integer_32bit     __mask_was_saved;   // is the signal mask saved?
    signal_set        __saved_mask;       // saved signal mask 
} jump_buf_tag;


