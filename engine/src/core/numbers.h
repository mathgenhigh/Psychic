#pragma once 

typedef char            integer_8bit;
typedef signed char     integer_8bit_signed;
typedef unsigned char   integer_8bit_unsigned;

typedef short           integer_16bit;
typedef signed short    integer_16bit_signed;
typedef unsigned short  integer_16bit_unsigned;

typedef int             integer_32bit;
typedef signed int      integer_32bit_signed;
typedef unsigned int    integer_32bit_unsigned;

typedef long long           integer_64bit;
typedef signed long long    integer_64bit_signed;
typedef unsigned long long  integer_64bit_unsigned;

typedef float       floating_point_32bit;
typedef double      floating_point_64bit;

typedef integer_8bit_unsigned bool_8bit;
#define true   1 
#define false  0

/* intptr_t, uintptr_t and ptrdiff_t replacement */
#if defined(__SIZEOF_POINTER__)  
#   if __SIZEOF_POINTER__ == 8
        typedef integer_64bit_signed    integer_pointer_signed;
        typedef integer_64bit_unsigned  integer_pointer_unsigned;
        typedef integer_64bit_signed    pointer_difference;
#   elif __SIZEOF_POINTER__ == 4 
        typedef integer_32bit_signed    integer_pointer_signed;
        typedef integer_32bit_unsigned  integer_pointer_unsigned;
        typedef integer_32bit_signed    pointer_difference;
#   elif __SIZEOF_POINTER__ == 2 
        typedef integer_16bit_signed    integer_pointer_signed;
        typedef integer_16bit_unsigned  integer_pointer_unsigned;
        typedef integer_16bit_signed    pointer_difference;
#   else 
#       error "Unsupported pointer size"
#   endif
#else 
    /* Fallback */
    typedef integer_8bit pointer_size_check[
        sizeof(void *) == 8 ||
        sizeof(void *) == 4 ||
        sizeof(void *) == 2 ? 1 : -1
    ];
    
#   if sizeof(void *) == 8 
        typedef integer_64bit_signed    integer_pointer_signed;
        typedef integer_64bit_unsigned  integer_pointer_unsigned;
        typedef integer_64bit_signed    pointer_difference;
#   elif sizeof(void *) == 4 
        typedef integer_32bit_signed    integer_pointer_signed;
        typedef integer_32bit_unsigned  integer_pointer_unsigned;
        typedef integer_32bit_signed    pointer_difference;
#   elif sizeof(void *) == 2 
        typedef integer_16bit_signed    integer_pointer_signed;
        typedef integer_16bit_unsigned  integer_pointer_unsigned;
        typedef integer_16bit_signed    pointer_difference;
#   endif
#endif

/* Returns 1 if 'type' is a pointer type, 0 otherwise */ 
#if defined(__GNUC__)
#   define pointer_type(type) (__builtin_classify_type((type)0) == 5)
#else 
#   define pointer_type(type)   \
        _Generic((type)0,       \
            void *: 1,          \
            default: pointer_type_fallback((type)0) \
        )
#   define pointer_type_fallback(x)   \
        _Generic(&(x),                \
            void **: 1,               \
            default: 0                \
        )
#endif

/* Detect machine word size (pointer size) */
#if __SIZEOF_POINTER__ == 8 
#   define ENGINE_WORDSIZE  64
#elif __SIZEOF_POINTER__ == 4
#   define ENGINE_WORDSIZE  32
#else 
#   define ENGINE_WORDSIZE  16
#endif

#if ENGINE_WORDSIZE == 64
    typedef integer_64bit_unsigned engine_size_t;
#else 
    typedef integer_32bit_unsigned engine_size_t;
#endif

/* Return 'T' casted to integer pointer if 'P' is true, 
   or just 'T' if 'P' is false */
#define integer_if_pointer_type_sub(T, P)                                       \
    typeof(*(0 ? (typeof(0 ? (T *)0 : (void *)(P)))0                            \
               : (typeof(0 ? (integer_pointer_signed *)0 : (void *) (!(P))))0   \
    ))

/* Return integer pointer if 'EXPR' has a pointer type,
   or the type of 'EXPR' otherwise */
#define integer_if_pointer_type(expr)  \
    integer_if_pointer_type_sub(typeof((typeof(expr))0, pointer_type(typeof(expr))))

/* Cast an integer or a pointer 'VAL' to integer with proper type */
#define cast_to_integer(val) ((integer_if_pointer_type(val))(val))

/* Cast an integer 'VAL' to 'void *' pointer */
#define cast_to_pointer(val) ((void *)(integer_pointer_unsigned)(val))

/* Align a value by rounding down to closest size.
   e.g. Using size of 4096, we get this behavior:
	{4095, 4096, 4097} = {0, 4096, 4096}  */
#define ALIGN_DOWN(base, size)  ((base) & -((typeof(base))(size)))

/* Align a value by rounding up to closest size.
   e.g. Using size of 4096, we get this behavior:
	{4095, 4096, 4097} = {4096, 4096, 8192} 
    
   Note: The size argument has side effects (expanded multiple times)
*/
#define ALIGN_UP(base, size)  ALIGN_DOWN((base) + (size) - 1, (size))

/* Same as 'ALIGN_DOWN()', but automatically casts when 
   base is a pointer */
#define PTR_ALIGN_DOWN(base, size)   \
    ((typeof(base))ALIGN_DOWN((integer_pointer_unsigned)(base), (size)))

/* Same as 'ALIGN_UP()', but automatically casts when base is a pointer */
#define PTR_ALIGN_UP(base, size)     \
    ((typeof(base))ALIGN_UP((integer_pointer_unsigned)(base), (size)))

/* Check if 'BASE' is aligned on 'SIZE' */
#define PTR_IS_ALIGNED(base, size)   \
    ((((integer_pointer_unsigned)(base)) & (size - 1)) == 0)

/* Returns pointer difference between 'P1' and 'P2' */
#define PTR_DIFF(p1, p2)    \
    ((pointer_difference)((integer_pointer_unsigned)(p1) - (integer_pointer_unsigned)(p2)))
