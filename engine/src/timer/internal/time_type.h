#pragma once

#include "../core/numbers.h"

#if ENGINE_WORDSIZE == 64
    typedef integer_64bit engine_time_t;
#else 
    typedef integer_32bit engine_time_t;
#endif

