#pragma once

/* Unlike glibc's kernel-features.h, this does NOT attempt to support
   arbitrary historical kernel versions - this engine has one minimum
   supported kernel version, defined below. Everything older is simply
   not a supported target; there is no runtime/version-conditional
   fallback path for syscalls predating this baseline */

#include "core/defines.h"

/* Encoded identically to Linux's own 'LINUX_VERSION_CODE':
    (major << 16) | (minor << 8) | patch */
#ifndef ENGINE_MIN_LINUX_KERNEL_VERSION
#   define ENGINE_MIN_LINUX_KERNEL_VERSION  0x050400    // 5.4 - minimum floor
#endif

/* Only worth checking features introduced AFTER chosen floor.
   Anything older than 'ENGINE_MIN_LINUX_KERNEL_VERSION' is unconditionally
   available and needs no flag at all */
#if ENGINE_MIN_LINUX_KERNEL_VERSION >= 0x050800
#   define ENGINE_HAS_FACCESSAT2    1
#else
#   define ENGINE_HAS_FACCESSAT2    0
#endif

#if ENGINE_MIN_LINUX_KERNEL_VERSION >= 0x050900
#   define ENGINE_HAS_CLOSE_RANGE   1
#else
#   define ENGINE_HAS_CLOSE_RANGE   0
#endif

#if ENGINE_MIN_LINUX_KERNEL_VERSION >= 0x050300
#   define ENGINE_HAS_CLONE3        1
#else 
#   define ENGINE_HAS_CLONE3        0
#endif

/* Anything below the minimum floor (statfs64, pselect, preadv/pwritev,
   sendmmsg, the entire socket-syscall block, SysV IPC syscalls,
   time64 syscalls on 64-bit time_t) is simply always true for this
   engine's supported range and needs no macro at all */