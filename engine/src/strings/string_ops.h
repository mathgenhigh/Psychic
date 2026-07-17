#pragma once

#include "core/numbers.h"
#include "core/defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Memory functions */

void *engine_memcpy (
    void *ENGINE_RESTRICT dest,
    void *ENGINE_RESTRICT src,
    engine_size_t n
) ENGINE_THROW ENGINE_NONNULL(1, 2);

void *engine_memmove (
    void *dest,
    const void *src,
    engine_size_t n
) ENGINE_THROW ENGINE_NONNULL(1, 2);

void *engine_memccpy (
    void *ENGINE_RESTRICT dest,
    const void *ENGINE_RESTRICT src,
    integer_32bit c,
    engine_size_t n
) ENGINE_THROW ENGINE_NONNULL(1, 2);

void *engine_memset (
    void *s,
    integer_32bit c,
    engine_size_t n
) ENGINE_THROW ENGINE_NONNULL(1);

integer_32bit engine_memcmp (
    const void *s1,
    const void *s2,
    engine_size_t n
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1, 2);

void *engine_memchr (
    const void *s,
    integer_32bit c,
    engine_size_t n
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1);

void *engine_rawmemchr (
    const void *s,
    integer_32bit c
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1);

void *engine_memrchr (
    const void *s,
    integer_32bit c,
    engine_size_t n
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1);

void *engine_memmem (
    const void *haystack,
    engine_size_t haystack_len,
    const void *needle,
    engine_size_t needle_len
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1, 3);

void *engine_mempcpy (
    void *ENGINE_RESTRICT dest,
    const void *ENGINE_RESTRICT src,
    engine_size_t n
) ENGINE_THROW ENGINE_NONNULL(1, 2);


/* String comparison / length */

integer_32bit engine_strcmp (
    const integer_8bit *s1,
    const integer_8bit *s2
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1, 2);

integer_32bit engine_strncmp (
    const integer_8bit *s1,
    const integer_8bit *s2,
    engine_size_t n
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1, 2);

engine_size_t engine_strlen (const integer_8bit *s) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1);
engine_size_t engine_strnlen (const integer_8bit *s, engine_size_t n) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1);


/* Bounded copy / concat */

/* Safe replacements for strcpy/strcat/strncpy/strncat.
   They fix strcpy/strcat's unbounded problem: 'n' is the
   FULL destination buffer size (not a source-byte count
   like strncat's confusing parameter), the result is always
   null-terminated if n > 0, and the return value is the length
   that WOULD have been produced (allows detecting truncation
   by comparing against 'n') */

engine_size_t engine_strlcpy (
    integer_8bit *ENGINE_RESTRICT dest,
    const integer_8bit *ENGINE_RESTRICT src,
    engine_size_t n
) ENGINE_NONNULL(1, 2);

engine_size_t engine_strlcat (
    integer_8bit *ENGINE_RESTRICT dest,
    const integer_8bit *ENGINE_RESTRICT src,
    engine_size_t n
) ENGINE_NONNULL(1, 2);


/* Strind duplication */

/* Allocates via the engine's own allocator (not raw malloc) -
   caller must release with the matching engine_free/equivalent */

integer_8bit *engine_strdup (const integer_8bit *s) ENGINE_NONNULL(1);
integer_8bit *engine_strndup (const integer_8bit *s, engine_size_t n) ENGINE_NONNULL(1);


/* Searching */

integer_8bit *engine_strchr (
    const integer_8bit *s,
    integer_32bit c
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1);

integer_8bit *engine_strrchr (
    const integer_8bit *s,
    integer_32bit c
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1);

integer_8bit *engine_strstr (
    const integer_8bit *haystack,
    const integer_8bit *needle
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1, 2);

integer_8bit *engine_strcasestr (
    const integer_8bit *haystack,
    const integer_8bit *needle
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1, 2);

engine_size_t engine_strspn (
    const integer_8bit *s,
    const integer_8bit *accept
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1, 2);

engine_size_t engine_strcspn (
    const integer_8bit *s,
    const integer_8bit *reject
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1, 2);

integer_8bit *engine_strpbrk (
    const integer_8bit *s,
    const integer_8bit *accept
) ENGINE_THROW ENGINE_CONSTFN ENGINE_NONNULL(1, 2);


/* Tokenizing */

/* Reentrant only.
   No strtok: the original uses hidden internal static state
   shared across all callers in a process, making it fundamentally
   unsafe to call from more than one thread (or even reentrantly 
   from one thread, e.g. from a signal handler or a nested call).
   Given this engine is thread-first, only the explicit-state version
   is provided */

integer_8bit *engine_strtok_r (
    integer_8bit *ENGINE_RESTRICT s,
    const integer_8bit *ENGINE_RESTRICT delim,
    integer_8bit **ENGINE_RESTRICT save_ptr
) ENGINE_THROW ENGINE_NONNULL(2, 3);

integer_8bit *engine_strsep (
    integer_8bit **ENGINE_RESTRICT string_ptr,
    const integer_8bit *ENGINE_RESTRICT delim
) ENGINE_THROW ENGINE_NONNULL(1, 2);


/* Error strings */

/* One reentrant form only. This always fills 'buf' and 
   returns an error code matching POSIX semantics */

const integer_8bit *engine_strerror (integer_32bit errnum) ENGINE_THROW;
integer_32bit engine_strerror_r (
    integer_32bit errnum,
    integer_8bit *buf,
    engine_size_t buf_len
) ENGINE_NONNULL(2);

#ifdef __cplusplus
}
#endif