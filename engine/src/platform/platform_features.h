#pragma once

/* ============================
   PLATFORM DETECTION
   ============================ */

#if defined(_WIN32) || defined(_WIN64)
#   define ENGINE_PLATFORM_WINDOWS 1
#else 
#   define ENGINE_PLATFORM_WINDOWS 0   
#endif

#if defined(__ANDROID__)
#   define ENGINE_PLATFORM_ANDROID 1
#else 
#   define ENGINE_PLATFORM_ANDROID 0
#endif

#if defined(__linux__)
#   define ENGINE_PLATFORM_LINUX 1
#else 
#   define ENGINE_PLATFORM_LINUX 0
#endif

/* Apple platforms: __APPLE__/__MACH__ alone cover macOS, iOS, iOS
   Simulator, tvOS, and watchOS all at once. Use TargetConditionals.h to
   tell them apart instead of conflating them */
#if defined(__APPLE__) && defined(__MACH__)
#   include <TargetConditionals.h>

#   if defined(TARGET_OS_OSX) && TARGET_OS_OSX
#       define ENGINE_PLATFORM_MACOS 1
#   else 
#       define ENGINE_PLATFORM_MACOS 0
#   endif

#   if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#       define ENGINE_PLATFORM_IOS   1
#   else
#       define ENGINE_PLATFORM_IOS   0
#   endif

#else
#   define ENGINE_PLATFORM_MACOS 0
#   define ENGINE_PLATFORM_IOS   0
#endif

/* BSD family: only fires for real BSDs, not Linux/macOS (both also
   satisfy some Unix-ish predefines on certain toolchains) */
#if !ENGINE_PLATFORM_LINUX && !ENGINE_PLATFORM_MACOS && !ENGINE_PLATFORM_IOS &&   \
    (defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__))
#   define ENGINE_PLATFORM_BSD  1
#else 
#   define ENGINE_PLATFORM_BSD  0
#endif

#if !ENGINE_PLATFORM_WINDOWS && !ENGINE_PLATFORM_LINUX && !ENGINE_PLATFORM_ANDROID && \
    !ENGINE_PLATFORM_MACOS && !ENGINE_PLATFORM_IOS && !ENGINE_PLATFORM_BSD
#   define ENGINE_PLATFORM_UNKNOWN 1
#else 
#   define ENGINE_PLATFORM_UNKNOWN 0
#endif

/* ============================
   LIBC DETECTION
   ============================ */

/* '__GLIBC__' is glibc's own self-identifying macro, defined in
   <features.h> and pulled in transitively by any standard header */
#if defined(__GLIBC__)
#   define ENGINE_LIBC_GLIBC 1
#else 
#   define ENGINE_LIBC_GLIBC 0
#endif

#if defined(__BIONIC__)
#   define ENGINE_LIBC_BIONIC 1
#else 
#   define ENGINE_LIBC_BIONIC 0
#endif

/* musl deliberately defines no self-identifying macro (an intentional
   upstream decision, precisely to prevent "glibc vs musl" preprocessor
   shortcuts like this one). Best available heuristic: Linux, and
   neither glibc nor bionic. Not airtight, but there is no better signal
   available from the preprocessor alone */
#if ENGINE_PLATFORM_LINUX && !ENGINE_LIBC_GLIBC && !ENGINE_LIBC_BIONIC
#   define ENGINE_LIBC_MUSL  1
#else 
#   define ENGINE_LIBC_MUSL  0
#endif

#if ENGINE_PLATFORM_WINDOWS
#   define ENGINE_LIBC_MSVCRT 1
#else 
#   define ENGINE_LIBC_MSVCRT 0
#endif

/* ============================
   ENDIANNESS DETECTION
   ============================ */

#define ENGINE_LITTLE_ENDIAN  1234
#define ENGINE_BIG_ENDIAN     4321 
#define ENGINE_PDP_ENDIAN     3412

/* Detect byte order using compiler built-ins */
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
#   if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ 
#       define ENGINE_BYTE_ORDER  ENGINE_LITTLE_ENDIAN 
#   elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ 
#       define ENGINE_BYTE_ORDER  ENGINE_BIG_ENDIAN
#   elif defined(__ORDER_PDP_ENDIAN__) && __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#       define ENGINE_BYTE_ORDER ENGINE_PDP_ENDIAN
#   else 
#       error "Unrecognized __BYTE_ORDER__ value"
#   endif 
#elif ENGINE_PLATFORM_WINDOWS
    /* MSVC defines none of the above. Every Windows target Microsoft
       currently supports (x86, x64, ARM, ARM64) runs little-endian */
#   define ENGINE_BYTE_ORDER  ENGINE_LITTLE_ENDIAN
#else 
#   error "Cannot determine system endianess"
#endif

/* Floating-point word order (usually same as byte order) */
#ifndef ENGINE_FLOAT_WORD_ORDER
#   define ENGINE_FLOAT_WORD_ORDER  ENGINE_BYTE_ORDER 
#endif

/* Utility macro similar to glibc's __LONG_LONG_PAIR */ 
#if ENGINE_BYTE_ORDER == ENGINE_LITTLE_ENDIAN 
#   define ENGINE_LONG_LONG_PAIR(HI, LO)  LO, HI
#elif ENGINE_BYTE_ORDER == ENGINE_BIG_ENDIAN 
#   define ENGINE_LONG_LONG_PAIR(HI, LO)  HI, LO
#else 
    /* PDP-endian: no sane single ordering for a 64-bit pair from two
       32-bit halves: middle-endian layout, deliberately unsupported.
       Fails loudly instead of silently leaving the macro undefined */
#   error "ENGINE_LONG_LONG_PAIR is not supported on PDP-endian targets"
#endif

/* ============================
   PLATFORM FEATURES
   ============================ */

/* Linux futex availability */
#if ENGINE_PLATFORM_LINUX || ENGINE_PLATFORM_ANDROID
#   define ENGINE_HAS_FUTEX 1
#else 
#   define ENGINE_HAS_FUTEX 0
#endif

/* Windows 'WaitOnAddress()' availability */
#if ENGINE_PLATFORM_WINDOWS
#   define ENGINE_HAS_WAITONADDRESS 1
#else 
#   define ENGINE_HAS_WAITONADDRESS 0
#endif

/* POSIX threads availability */
#if ENGINE_PLATFORM_LINUX || ENGINE_PLATFORM_MACOS || ENGINE_PLATFORM_IOS || \
    ENGINE_PLATFORM_BSD || ENGINE_PLATFORM_ANDROID
#   define ENGINE_HAS_PTHREADS 1
#else 
#   define ENGINE_HAS_PTHREADS 0
#endif

/* 'fork()/pthread_atfork()'-style semantics: no equivalent on Windows */
#if !ENGINE_PLATFORM_WINDOWS
#   define ENGINE_HAS_FORK 1
#else 
#   define ENGINE_HAS_FORK 0
#endif

/* OS-level thread ID (distinct from the engine's thread handle type):
   Linux 'gettid()', macOS/iOS 'pthread_threadid_np()', Windows
   'GetCurrentThreadId()/GetThreadId()'. Implementation differs per
   platform; this only advertises availability of *some* mechanism */
#if ENGINE_PLATFORM_LINUX || ENGINE_PLATFORM_ANDROID || \
    ENGINE_PLATFORM_MACOS || ENGINE_PLATFORM_IOS || ENGINE_PLATFORM_WINDOWS
#   define ENGINE_HAS_GETTID 1
#else 
#   define ENGINE_HAS_GETTID 0
#endif