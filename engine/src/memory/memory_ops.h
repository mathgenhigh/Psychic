#pragma once

#include "core/numbers.h"
#include "core/defines.h"

#ifdef __cplusplus
extern "C" {
#endif

void *engine_malloc (engine_size_t size) ENGINE_THROW ENGINE_MALLOC_FN ENGINE_ALLOC_SIZE(1) ENGINE_NODISCARD;
void *engine_calloc (engine_size_t nmemb, engine_size_t size) ENGINE_THROW ENGINE_MALLOC_FN ENGINE_ALLOC_SIZE(1, 2) ENGINE_NODISCARD;

/* No 'ENGINE_MALLOC_FN' here, deliberately: if 'engine_realloc' returns
   the same pointer it was given, the old and new pointers legitimately
   alias the same object, which the 'malloc' attribute's no-aliasing
   guarantee would contradict */
void *engine_realloc (void *ptr, engine_size_t size) ENGINE_THROW ENGINE_ALLOC_SIZE(2) ENGINE_NODISCARD;

/* Like 'engine_realloc', but for nmemb * size, with the multiplication
   itself checke for overflow rather than left to the caller - this is
   genuinely the point of 'engine_reallocarray' existing at all:
   nmemb * size overflowing silently and under-allocating is a real,
   historically exploited bug class */
void *engine_reallocarray (void *ptr, engine_size_t nmemb, engine_size_t size) ENGINE_THROW ENGINE_ALLOC_SIZE(2, 3) ENGINE_NODISCARD;

void engine_free (void *ptr) ENGINE_THROW;

/* Aligned allocation. 'alignment' must be a power of two. Must
   be freed with 'engine_free', same as 'engine_malloc' */
void *engine_aligned_alloc (engine_size_t alignment, engine_size_t size) ENGINE_THROW ENGINE_MALLOC_FN ENGINE_ALLOC_ALIGN(1) ENGINE_ALLOC_SIZE(2) ENGINE_NODISCARD;

#ifdef __cplusplus
}
#endif