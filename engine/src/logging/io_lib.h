#pragma once

#include "internal/stream.h"

#include "core/numbers.h"
#include "core/defines.h"

#include "sync/internal/low-level_lock.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Types */

typedef struct engine_file_s {
    engine_stream stream;

    /* Explicit lock support. Recursive: a thread already holding the lock
       via 'engine_flockfile' can still call other engine_f* functions on
       the same stream without deadlocking */
    integer_32bit lock;
    integer_32bit lock_owner_tid;   // for recursive-lock detection
    integer_32bit lock_depth;

    integer_32bit_unsigned magic;

    bool_8bit eof_flag;
    bool_8bit error_flag;
} engine_file;

typedef struct {
    integer_64bit offset;
} engine_file_pos;

#define ENGINE_IOFBF    0   // fully buffered
#define ENGINE_IOLBF    1   // line buffered
#define ENGINE_IONBF    2   // unbuffered

#define ENGINE_BUFSIZ   8192
#define ENGINE_EOF      (-1)

#define ENGINE_SEEK_SET     0
#define ENGINE_SEEK_CUR     1
#define ENGINE_SEEK_END     2


/* Standard streams */
extern engine_file *engine_standard_input;
extern engine_file *engine_standard_output;
extern engine_file *engine_standard_error;


/* File operations */

engine_file *engine_file_open (
    const integer_8bit *file_name,
    const integer_8bit *modes
) ENGINE_NONNULL(1, 2);

engine_file *engine_file_reopen (
    const integer_8bit *file_name,
    const integer_8bit *modes,
    engine_file *stream
) ENGINE_NONNULL(2, 3);

integer_32bit engine_file_close (engine_file *stream) ENGINE_NONNULL(1);
integer_32bit engine_file_flush (engine_file *stream);

engine_file *engine_file_descriptor_open (
    integer_32bit fd, 
    const integer_8bit *modes
) ENGINE_NONNULL(2);

/* Memory-backed streams */
engine_file *engine_file_memory_open (
    void *buf,
    engine_size_t len,
    const integer_8bit *modes
) ENGINE_NONNULL(3);

engine_file *engine_file_open_memory_stream (
    integer_8bit **buf_loc,
    engine_size_t *size_loc
) ENGINE_NONNULL(1, 2);

integer_32bit engine_file_remove (const integer_8bit *file_name) ENGINE_THROW ENGINE_NONNULL(1);
integer_32bit engine_file_rename (
    const integer_8bit *old_name,
    const integer_8bit *new_name
) ENGINE_THROW ENGINE_NONNULL(1, 2);


/* Buffering control */
void engine_setbuffer (engine_file *stream, integer_8bit *buf) ENGINE_NONNULL(1);

integer_32bit engine_setvbuffer (
    engine_file *stream,
    integer_8bit *buf,
    integer_32bit mode,
    engine_size_t size
) ENGINE_NONNULL(1);


/* Character I/O */

integer_32bit engine_file_getchar (engine_file *stream) ENGINE_NONNULL(1);
integer_32bit engine_getchar (void);
integer_32bit engine_file_putchar (integer_32bit c, engine_file *stream) ENGINE_NONNULL(2);
integer_32bit engine_putchar (integer_32bit c);
integer_32bit engine_ungetc (integer_32bit c, engine_file *stream) ENGINE_NONNULL(2);


/* Line / buffer I/O */

integer_8bit *engine_file_gets (
    integer_8bit *s,
    integer_32bit n,
    engine_file *stream
) ENGINE_NONNULL(1, 3);

integer_32bit engine_file_puts (const integer_8bit *s, engine_file *stream) ENGINE_NONNULL(1, 2);
integer_32bit engine_puts (const integer_8bit *s) ENGINE_NONNULL(1);

integer_64bit_signed engine_getdelim (
    integer_8bit **line_ptr,
    engine_size_t *n,
    integer_32bit delimeter,
    engine_file *stream
) ENGINE_NONNULL(1, 2, 4);

integer_64bit_signed engine_getline (
    integer_8bit **line_ptr,
    engine_size_t *n,
    engine_file *stream
) ENGINE_NONNULL(1, 2, 3);

engine_size_t engine_file_read (
    void *ptr,
    engine_size_t size,
    engine_size_t n,
    engine_file *stream
) ENGINE_NONNULL(4);

engine_size_t engine_file_write (
    const void *ptr,
    engine_size_t size,
    engine_size_t n,
    engine_file *stream
) ENGINE_NONNULL(4);


/* Positioning */

integer_32bit engine_file_seek (
    engine_file *stream,
    integer_64bit_signed off,
    integer_32bit whence
) ENGINE_NONNULL(1);

integer_32bit engine_file_tell (engine_file *stream) ENGINE_NONNULL(1);
void engine_rewind (engine_file *stream) ENGINE_NONNULL(1);
integer_32bit engine_file_getpos (engine_file *stream, engine_file_pos *pos) ENGINE_NONNULL(1, 2);
integer_32bit engine_file_setpos (engine_file *stream, const engine_file_pos *pos) ENGINE_NONNULL(1, 2);


/* Status */

void engine_clearerr (engine_file *stream) ENGINE_NONNULL(1);
integer_32bit engine_file_eof (engine_file *stream) ENGINE_NONNULL(1);
integer_32bit engine_file_error (engine_file *stream) ENGINE_NONNULL(1);

/* Returns -1 (no valid OS handle) for streams not backed by a real
   file descriptor/handle */
integer_32bit engine_file_number (engine_file *stream) ENGINE_NONNULL(1);


/* Explicit stream locking */

void engine_file_lockfile (engine_file *stream) ENGINE_NONNULL(1);
integer_32bit engine_file_trylockfile (engine_file *stream) ENGINE_NONNULL(1);
void engine_file_unlockfile (engine_file *stream) ENGINE_NONNULL(1);


/* Formatted output */

integer_32bit engine_printf (const integer_8bit *format, ...) ENGINE_NONNULL(1);
integer_32bit engine_file_printf (engine_file *stream, const integer_8bit *format, ...) ENGINE_NONNULL(1, 2);
integer_32bit engine_snprintf (integer_8bit *s, engine_size_t max_len, const integer_8bit *format, ...) ENGINE_NONNULL(1, 3);


/* Formatted input */

integer_32bit engine_file_scanf (engine_file *stream, const integer_8bit *format, ...) ENGINE_NONNULL(1, 2);
integer_32bit engine_sscanf (const integer_8bit *s, const integer_8bit *format, ...) ENGINE_THROW ENGINE_NONNULL(1, 2);

#ifdef __cplusplus
}
#endif