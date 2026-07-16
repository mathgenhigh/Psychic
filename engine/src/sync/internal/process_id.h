#pragma once

#include "platform/platform_features.h"
#include "core/numbers.h"

#if ENGINE_PLATFORM_WINDOWS
    /* Windows process IDs (DWORD, via 'GetCurrentProcessId()'/'GetProcessId()')
       are unsigned; there is no negative-PID/process-group convention */
    typedef integer_32bit_unsigned engine_process_id;
#   define ENGINE_INVALID_PROCESS_ID    ((engine_process_id)0)
#else 
    /* POSIX pid_t: signed, since negative values denote process groups
       (e.g. kill(-pgid, sig)). True across Linux, macOS, iOS, Android,
       BSD regardless of underlying architecture */
    typedef integer_32bit engine_process_id;
#   define ENGINE_INVALID_PROCESS_ID    ((engine_process_id)-1)
#endif