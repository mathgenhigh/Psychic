#pragma once

/* ============================
       8/16/32/64-BIT LIMITS
   ============================ */

#define ENGINE_INT8_MIN     (-0x7F - 1)
#define ENGINE_INT8_MAX     0x7F
#define ENGINE_UINT8_MAX    0xFFU

#define ENGINE_INT16_MIN    (-0x7FFF - 1)
#define ENGINE_INT16_MAX    0x7FFF
#define ENGINE_UINT16_MAX   0xFFFFU

#define ENGINE_INT32_MIN    (-0x7FFFFFFF - 1)
#define ENGINE_INT32_MAX    0x7FFFFFFF
#define ENGINE_UINT32_MAX   0xFFFFFFFFU

#define ENGINE_INT64_MIN    (-0x7FFFFFFFFFFFFFFFLL- 1)
#define ENGINE_INT64_MAX    0x7FFFFFFFFFFFFFFFLL
#define ENGINE_UINT64_MAX   0xFFFFFFFFFFFFFFFFULL

/* ============================
        WORDSIZE DETECTION 
   ============================ */

#if defined(_WIN64) || defined(__LP64__) || defined(_LP64)      || \
    defined(__x86_64__) || defined(__amd64__)                    || \
    defined(__aarch64__) || defined(__arm64__)                   || \
    defined(__ppc64__) || defined(__powerpc64__)                 || \
    defined(__mips64) || defined(__riscv) && __riscv_xlen == 64  || \
    defined(_M_X64) || defined(_M_AMD64) || defined(_M_ARM64)
#   define ENGINE_WORDSIZE  64
#elif defined(_WIN32) || defined(__i386__) || defined(__arm__)   || \
    defined(_M_IX86) || defined(_M_ARM)                          || \
    defined(__mips__) && !defined(__mips64)
#   define ENGINE_WORDSIZE  32
#else 
#   error "Unrecognized architecture - add its 64/32-bit signature above"
#endif

/* ============================
    WORDSIZE-DEPENDENT LIMITS
   ============================
   Genuinely platform-varying (unlike the fixed-width block above), so
   this branches on ENGINE_WORDSIZE, reusing the constants already
   defined in this same file rather than pulling them from elsewhere
*/

#if ENGINE_WORDSIZE == 64
#   define ENGINE_SIZE_MAX      ENGINE_UINT64_MAX
#   define ENGINE_PTRDIFF_MIN   ENGINE_INT64_MIN
#   define ENGINE_PTRDIFF_MAX   ENGINE_INT64_MAX
#   define ENGINE_INTPTR_MIN    ENGINE_INT64_MIN
#   define ENGINE_INTPTR_MAX    ENGINE_INT64_MAX
#   define ENGINE_UINTPTR_MAX   ENGINE_UINT64_MAX
#elif ENGINE_WORDSIZE == 32
#   define ENGINE_SIZE_MAX      ENGINE_UINT32_MAX
#   define ENGINE_PTRDIFF_MIN   ENGINE_INT32_MIN
#   define ENGINE_PTRDIFF_MAX   ENGINE_INT32_MAX
#   define ENGINE_INTPTR_MIN    ENGINE_INT32_MIN
#   define ENGINE_INTPTR_MAX    ENGINE_INT32_MAX
#   define ENGINE_UINTPTR_MAX   ENGINE_UINT32_MAX
#endif
