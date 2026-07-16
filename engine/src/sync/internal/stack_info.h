#pragma once

/* Architecture-dependent (NOT platform/OS-dependent) facts about how
   the hardware stack grows and what protection its pages need */

/* Every architecture this engine targets (x86, x86-64, ARM, ARM64,
   RISC-V) grows the stack downward. PA-RISC is the one mainstream
   historical exception */
#define ENGINE_STACK_GROWS_DOWN  1

/* Default protection for newly allocated stack pages: readable and
   writable, not executable */
#if ENGINE_PLATFORM_WINDOWS
#   define ENGINE_STACK_PROT_PERMS  0x04    // PAGE_READWRITE
#else 
#   define ENGINE_STACK_PROT_PERMS  0x03    // PROT_READ | PROT_WRITE
#endif