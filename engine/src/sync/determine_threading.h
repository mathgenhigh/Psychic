#pragma once 

#include "../platform/platform_features.h"

/* Allow manual override */
#ifndef ENGINE_THREADING_ENABLED
#   define ENGINE_THREADING_ENABLED  1
#endif

/* Single-threaded override */
#ifdef ENGINE_SINGLE_THREADED
#   undef ENGINE_THREADING_ENABLED
#   define ENGINE_THREADING_ENABLED  0
#endif

/* Backend selection */
#if ENGINE_THREADING_ENABLED 
#   if ENGINE_PLATFORM_WINDOWS
#       define ENGINE_THREADING_WINDOWS  1
#   elif ENGINE_PLATFORM_LINUX 
#       define ENGINE_THREADING_POSIX    1
#   endif
#else 
#   define ENGINE_THREADING_NONE  1
#endif
