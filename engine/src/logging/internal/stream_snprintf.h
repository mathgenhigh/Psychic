#pragma once

#include "core/numbers.h"
#include "core/defines.h"

/* snprintf's truncate-but-keep-counting semantics don't match the fill/flush
   contract engine_stream_ops was designed around - this is a small, 
   self-contained target that engine_vformat_core writes through via its 
   generic sink interface instead of an engine_stream */
typedef struct engine_snprintf_target_s {
    integer_8bit *buf;          // caller's buffer, or NULL if length == 0
    engine_size_t capacity;     // usable byte capacity, i.e. length - 1, reserving room for the NUL - 0 if length was 0
    engine_size_t written;      // total chars that WOULD have been written, including any truncated past capacity - this is snprintf's return value
} engine_snprintf_target;

/* Initializes 'target' to write into 'buffer' (may be NULL only if
   'length' is 0). Matches glibc's documented historic behavior:
   if length > 0, buffer[0] is immediately set to '\0', so a caller
   that formats zero characters still gets a valid empty string
   rather than an untouched/garbage buffer */
void engine_snprintf_target_init (
    engine_snprintf_target *target,
    integer_8bit *buffer,
    engine_size_t length
) ENGINE_NONNULL(1);

/* Called by the format sink for every character produced. Writes
   into 'buf' only while there is room (target->written < capacity);
   once capacity is exhausted, further calls are silently discarded
   except for the running count - this IS the truncation behavior
   snprintf's return value is meant to expose */
void engine_snprintf_target_putc (
    engine_snprintf_target *target, 
    integer_8bit c
) ENGINE_NONNULL(1);

/* Writes the final null terminator - unconditionally, even if the
   underlying format operation failed partway through, matching the
   glibc comment this is ported from ("write the null terminator even
   in case of errors"). Returns the total character count snprintf
   should report; comparing this against the buffer length passed to
   _init is how a caller detects truncation */
integer_64bit_signed engine_snprintf_target_done (engine_snprintf_target *target) ENGINE_NONNULL(1);

/* The format-engine sink adapter - passed as engine_format_sink to
   engine_vformat_core with 'target' as sink_ctx */
void engine_snprintf_sink (void *ctx, integer_8bit c);

integer_32bit engine_vsnprintf (
    integer_8bit *s,
    engine_size_t max_len,
    const integer_8bit *format,
    va_list ap
) ENGINE_NONNULL(1, 3);