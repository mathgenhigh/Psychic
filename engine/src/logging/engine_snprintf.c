#include "internal/stream_snprintf.h"
#include "internal/format_core.h"

#include "io_lib.h"

void engine_snprintf_target_init (
    engine_snprintf_target *target,
    integer_8bit *buffer,
    engine_size_t length
) {
    target->buf = buffer;
    target->written = 0;

    if (length > 0) {
        /* Reserve the last byte for the terminator - capacity is the
           usable data region, not the full buffer size */
        target->capacity = length - 1;

        /* Historic early-truncation behavior: guarantees a
           valid empty string is visible immediately, before any
           formatting has actually run, rather than leaving 'buffer'
           untouched/uninitialized if the caller inspects it early or
           if formatting produces zero characters */
        buffer[0] = '\0';
    }
    else {
        /* length == 0: POSIX explicitly permits 'buffer' to be NULL
           in this case, and mandates that nothing - not even a null
           terminator - is written. capacity stays 0, and _done below
           must not dereference 'buf' at all when capacity is 0 */
        target->capacity = 0;
    }
}

void engine_snprintf_target_putc (
    engine_snprintf_target *target,
    integer_8bit c
) {
    if (target->written < target->capacity)
        target->buf[target->written] = c;

    /* Counted unconditionally, even once capacity is exhausted - 
       the count, not the write, is what makes the truncation signal
       (return value > length passed in) possible */
    target->written++;
}

integer_64bit_signed engine_snprintf_target_done (engine_snprintf_target *target) {
    if (target->capacity > 0) {
        /* Clamp to capacity: if more was "written" (counted) than
           fits, the terminator goes at the last real byte we 
           actually have room for, not past the buffer's end */
        engine_size_t term_pos = (target->written < target->capacity)
            ? target->written
            : target->capacity;
        target->buf[term_pos] = '\0';
    }

    /* capacity == 0 (length was 0 at init): nothing to terminate,
       'buf' may be NULL - do not touch it at all, matching the
       POSIX-mandated zero-length behavior noted in _init */
    
    return (integer_64bit_signed)target->written;
}

void engine_snprintf_sink (void *ctx, integer_8bit c) {
    engine_snprintf_target_putc((engine_snprintf_target *)ctx, c);
}

integer_32bit engine_vsnprintf (
    integer_8bit *s,
    engine_size_t max_len,
    const integer_8bit *format,
    va_list ap
) {
    engine_snprintf_target target;
    engine_snprintf_target_init(&target, s, max_len);

    engine_vformat_core(engine_snprintf_sink, &target, format, ap);

    return (integer_32bit)engine_snprintf_target_done(&target);
}

integer_32bit engine_snprintf (
    integer_8bit *s,
    engine_size_t max_len,
    const integer_8bit *format,
    ...
) {
    va_list ap;
    va_start(ap, format);
    integer_32bit result = engine_vsnprintf(s, max_len, format, ap);
    va_end(ap);
    return result;
}