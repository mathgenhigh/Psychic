#include "core/defines.h"

#include "sync/internal/futex_backend.h"
#include "sync/internal/futex_ops.h"

#include <sys/syscall.h>
#include <unistd.h>
#include <errno.h>

integer_32bit engine_futex_wait_timed (
    volatile integer_32bit *futex_word,
    integer_32bit expected,
    const engine_timespec *timeout,
    integer_32bit private_flag
) {
    integer_32bit op = engine_futex_private_flag(ENGINE_FUTEX_WAIT, private_flag);
    long ret = syscall(SYS_futex, futex_word, op, expected, timeout, ENGINE_NULLPTR, 0);
    return (ret < 0) ? -errno : 0;
}

integer_32bit engine_futex_wake (
    volatile integer_32bit *futex_word,
    integer_32bit num_to_wake,
    integer_32bit private_flag
) {
    integer_32bit op = engine_futex_private_flag(ENGINE_FUTEX_WAKE, private_flag);
    long ret = syscall(SYS_futex, futex_word, op, num_to_wake, ENGINE_NULLPTR, ENGINE_NULLPTR, 0);
    return (ret < 0) ? -errno : 0;
}