#include "platform/platform_features.h"

#if ENGINE_PLATFORM_WINDOWS

#include "memory/internal/os_memory.h"

#include <memoryapi.h>
#include <sysinfoapi.h>

void *engine_os_alloc_pages (engine_size_t size) {
    /* MEM_COMMIT | MEM_RESERVE in one call: simplest equivalent to
       POSIX's single 'mmap()' call. A more advanced allocator might
       reserve a large region up front and commit pages lazily as
       needed */
    return VirtualAlloc(ENGINE_NULLPTR, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

integer_32bit engine_os_free_pages (void *ptr, engine_size_t size) {
    (void)size;     // 'VirtualFree' with MEM_RELEASE requires size == 0
    return VirtualFree(ptr, 0, MEM_RELEASE) ? 0 : -1;
}

engine_size_t engine_os_page_size (void) {
    static engine_size_t cached = 0;

    if (cached == 0) {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        cached = (engine_size_t)info.dwPageSize;
    }

    return cached;
}

#endif