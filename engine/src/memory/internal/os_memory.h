#pragma once

#include "core/numbers.h"
#include "core/defines.h"

/* Reserve + commit 'size' bytes of fresh, zero-initialized memory
   directly from the OS - not from any arena/freelist. Returns ENGINE_NULLPTR
   on failure. 'size' should already be rounded up to the platform's page
   size by the caller */
void *engine_os_alloc_pages (engine_size_t size) ENGINE_THROW;

/* Returns memory obtained from 'engine_os_alloc_pages' back to the OS.
   'size' must match what was originally requested */
integer_32bit engine_os_free_pages (void *ptr, engine_size_t size) ENGINE_THROW ENGINE_NONNULL(1);

/* The platform's page size/allocation granularity, used to round
   requests up before calling 'engine_os_alloc_pages' */
engine_size_t engine_os_page_size (void) ENGINE_THROW ENGINE_CONSTFN;