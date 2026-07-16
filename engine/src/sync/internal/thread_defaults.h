#pragma once

#if ENGINE_PLATFORM_WINDOWS
#   define ENGINE_THREAD_STACK_MIN  (64 * 1024)     // Windows default reserve
#else
#   define ENGINE_THREAD_STACK_MIN  (16 * 1024)     // typical POSIX minimum
#endif

#define ENGINE_THREAD_GUARD_SIZE_DEFAULT    4096    // one page, typical