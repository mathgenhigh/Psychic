#pragma once 

/* Define to 1 if the operating system supports memory mapping 
   or remapping, 0 otherwise */
#ifdef __linux__ 
#   define HAVE_MMAP    1
#   define HAVE_MREMAP  1
#endif
