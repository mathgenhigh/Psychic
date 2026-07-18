#include "platform/platform_features.h"

#if ENGINE_PLATFORM_LINUX

#include "memory/internal/os_memory.h"

#include <sys/mman.h>
#include <unistd.h>

void *engine_os_alloc_pages (engine_size_t size) {
    /* MAP_ANONYMOUS: not backed by any file - exactly the case this
       engine's allocator needs, since it's carving out fresh heap memory,
       not mapping file contents. No offset parameter needed at all here -
       the entire 32-bit offset wrinkle in the original file simply doesn't
       apply, since anonymous mapping have no file offset */
    void *ptr = mmap(ENGINE_NULLPTR, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    return (ptr == MAP_FAILED) ? ENGINE_NULLPTR : ptr;
}

integer_32bit engine_os_free_pages (void *ptr, engine_size_t size) {
    return (munmap(ptr, size) == 0) ? 0 : -1;
}

engine_size_t engine_os_page_size (void) {
    static engine_size_t cached = 0;
    
    if (cached == 0)
        cached = (engine_size_t)sysconf(_SC_PAGESIZE);

    return cached;
}

#endif