#pragma once 

/* Compiler detection */
#if defined(__GNUC__) || defined(__clang__)
#   define ENGINE_COMPILER_GCC_FAMILY   1
#   define ENGINE_COMPILER_MSVC         0
#   define ENGINE_COMPILER_UNKNOWN      0
#elif defined(_MSC_VER)
#   define ENGINE_COMPILER_GCC_FAMILY   0
#   define ENGINE_COMPILER_MSVC         1
#   define ENGINE_COMPILER_UNKNOWN      0
#else 
#   define ENGINE_COMPILER_GCC_FAMILY   0
#   define ENGINE_COMPILER_MSVC         0
#   define ENGINE_COMPILER_UNKNOWN      1 
#endif

#if ENGINE_COMPILER_GCC_FAMILY 
#   define ENGINE_ALWAYS_INLINE  inline __attribute__((always_inline))
#   define ENGINE_NOINLINE       __attribute__((noinline))
#   define ENGINE_ALIGN(x)       __attribute__((aligned(x)))
#   define ENGINE_UNREACHABLE()  __builtin_unreachable()
#   define ENGINE_THROW          __attribute__((__nothrow__, __LEAF))
#   define ENGINE_THROWNL        __attribute__((__nothrow__))
#   define ENGINE_NORETURN       __attribute__((__noreturn__))
#   define ENGINE_NONNULL(...)   __attribute__((nonnull(__VA_ARGS__)))
#   define ENGINE_CONSTFN        __attribute__((const))
#   define ENGINE_DEPRECATED     __attribute__((deprecated))
#elif ENGINE_COMPILER_MSVC
#   define ENGINE_ALWAYS_INLINE  __forceinline
#   define ENGINE_NOINLINE       __declspec(noinline)
#   define ENGINE_ALIGN(x)       __declspec(align(x))
#   define ENGINE_UNREACHABLE()  __assume(0)

#   if defined(__cplusplus)
#       define ENGINE_THROW         throw ()
#       define ENGINE_THROWNL       throw ()
#   else
#       define ENGINE_THROW
#       define ENGINE_THROWNL       __declspec(nothrow)
#   endif

#   define ENGINE_NORETURN       __declspec(noreturn)
#   define ENGINE_NONNULL(...)
#   define ENGINE_CONSTFN
#   define ENGINE_DEPRECATED     __declspec(deprecated)
#else 
#   define ENGINE_ALWAYS_INLINE  inline
#   define ENGINE_NOINLINE
#   define ENGINE_ALIGN(x)       do { } while (0)
#   define ENGINE_UNREACHABLE()
#   define ENGINE_THROW
#   define ENGINE_THROWNL
#   define ENGINE_NORETURN
#   define ENGINE_NONNULL(...)
#   define ENGINE_CONSTFN
#   define ENGINE_DEPRECATED
#endif

#if ENGINE_COMPILER_GCC_FAMILY 
#   define ENGINE_TYPEOF(x)     __typeof__(x)
#   define ENGINE_HAS_TYPEOF  1
#elif defined(__cplusplus) && __cplusplus >= 201103L
#   define ENGINE_TYPEOF(x)     decltype(x)
#   define ENGINE_HAS_TYPEOF  1
#else 
#   define ENGINE_HAS_TYPEOF  0
#endif

#if ENGINE_COMPILER_MSVC
#   define ENGINE_ALIGNOF(type) __alignof(type)
#elif __cplusplus
#   define ENGINE_ALIGNOF(type)  alignof(type)
#else 
#   define ENGINE_ALIGNOF(type)  _Alignof(type)
#endif

#if ENGINE_COMPILER_GCC_FAMILY
#   define ENGINE_RESTRICT  __restrict__
#elif ENGINE_COMPILER_MSVC
#   define ENGINE_RESTRICT  __restrict
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#   define ENGINE_RESTRICT  restrict
#else 
#   define ENGINE_RESTRICT
#endif

#if ENGINE_COMPILER_GCC_FAMILY && defined(__has_attribute)
#   if __has_attribute(access)
#       define ENGINE_HAS_ATTR_ACCESS  1
#   else 
#       define ENGINE_HAS_ATTR_ACCESS  0
#   endif
#else 
#   define ENGINE_HAS_ATTR_ACCESS  0
#endif

#if ENGINE_HAS_ATTR_ACCESS
#   define ENGINE_ATTR_ACCESS_NONE(argno)           \
        __attribute__((__access__(__none__, argno)))

#   define ENGINE_ATTR_ACCESS_READ_ONLY(...)        \
        __attribute__((__access__(__read_only__, __VA_ARGS__)))

#   define ENGINE_ATTR_ACCESS_WRITE_ONLY(...)       \
        __attribute__((__access__(__write_only__, __VA_ARGS__)))

#   define ENGINE_ATTR_ACCESS_READ_WRITE(...)       \
        __attribute__((__access__(__read_write__, __VA_ARGS__)))

#else 
#   define ENGINE_ATTR_ACCESS_NONE(argno)
#   define ENGINE_ATTR_ACCESS_READ_ONLY(...)
#   define ENGINE_ATTR_ACCESS_WRITE_ONLY(...)
#   define ENGINE_ATTR_ACCESS_READ_WRITE(...)
#endif

#if defined(__cplusplus) && __cplusplus >= 201103L
#   define ENGINE_THREAD_LOCAL   thread_local
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#   define ENGINE_THREAD_LOCAL   _Thread_local
#elif ENGINE_COMPILER_GCC_FAMILY
#   define ENGINE_THREAD_LOCAL  __thread
#elif ENGINE_COMPILER_MSVC
#   define ENGINE_THREAD_LOCAL  __declspec(thread)
#else 
#   error "No thread-local storage mechanism available for this compiler"
#endif

#if defined(__cplusplus) && __cplusplus >= 201103L
#   define ENGINE_NULLPTR   nullptr
#else
#   define ENGINE_NULLPTR   ((void *)0)
#endif

/* Allocation attributes */
#if ENGINE_COMPILER_GCC_FAMILY
#   define ENGINE_MALLOC_FN         __attribute__((__malloc__))
#   define ENGINE_ALLOC_SIZE(...)   __attribute__((__alloc_size__(__VA_ARGS__)))
#   define ENGINE_ALLOC_ALIGN(n)    __attribute__((__alloc_align__(n)))
#elif ENGINE_COMPILER_MSVC
#   define ENGINE_MALLOC_FN         __declspec(restrict)
#   define ENGINE_ALLOC_SIZE(...)
#   define ENGINE_ALLOC_ALIGN(n)
#else 
#   define ENGINE_ALLOC_FN
#   define ENGINE_ALLOC_SIZE(...)
#   define ENGINE_ALLOC_ALIGN(n)
#endif

/* "Don't ignore this return value" - genuinely important for allocators,
    since discarding the result of 'engine_malloc' is either a pointless
    call or (for 'engine_realloc') a guaranteed leak of the original block
    if reallocation moved it */
#if defined(__cplusplus) && __cplusplus >= 201703L
#   define ENGINE_NODISCARD     [[nodiscard]]
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
#   define ENGINE_NODISCARD     [[nodiscard]]
#elif ENGINE_COMPILER_GCC_FAMILY
#   define ENGINE_NODISCARD     __attribute__((__warn_unused_result__))
#elif ENGINE_COMPILER_MSVC
#   define ENGINE_NODISCARD     _Check_return_
#else
#   define ENGINE_NODISCARD
#endif