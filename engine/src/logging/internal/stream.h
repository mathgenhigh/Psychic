#pragma once

/* Core buffered-I/O engine. Modeled on glibc's libio architecture 
   (buffer state machine + vtable-style backend dispatch), but 
   rebuilt from first principles */

#include "core/numbers.h"
#include "core/defines.h"

#include "sync/internal/low-level_lock.h"
#include "sync/internal/thread_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Backend Vtable */

/* Every concrete stream backend (real file, in-memory buffer, pipe/socket
   backed, etc.) implements these four operations. The buffering logic in this
   file calls them only at buffer boundaries - never per-byte - which is the 
   entire performance argument for buffering in the first place. 
   
   'cookie' is opaque backend-specific state (an OS file handle, a pointer + 
   offset into a memory block, etc.) - the buffering layer never inspects it,
   only passes it through */
typedef struct engine_stream_ops_s {
    /* Fill 'buf' with up to 'buf_size' fresh bytes from the backend. 
       Called only when the read buffer is exhausted (read_ptr == read_end).
       Returns the number of bytes actually read (may be less then 'buf_size'),
       0 on end-of-stream, or a negative value on error. Must not block
       indefinitely with no way to fail if the backend is closed/invalidated
       concurrently */
    integer_64bit_signed (*fill_buffer)(void *cookie, integer_8bit_unsigned *buf, engine_size_t buf_size);

    /* Flush exactly 'len' bytes from 'buf' to the backend. Called when the
       write buffer is full (write_ptr == write_end) or on an explicit
       'engine_stream_flush'. Returns bytes actually written (a short write
       is only acceptable if the backend cannot currently accept more -
       callers of 'flush_buffer' must handle a partial result rather then
       assume all-or-nothing), or negative on error */
    integer_64bit_signed (*flush_buffer)(void *cookie, const integer_8bit_unsigned *buf, engine_size_t len);

    /* Reposition the backend's underlying cursor. 'whence' uses the same 
      'ENGINE_SEEK_'* values. Returns the new absolute offset, or a negative
       value if the backend does not support seeking (e.g. a pipe) or the
       seek is invalid */
    integer_64bit_signed (*seek) (void *cookie, integer_64bit_signed offset, integer_32bit whence);

    /* Release any backend-owned resources (fd/handle, allocted cookie memory).
       Returns 0 on success, negative on error. Called exactly once, after any
       pending write buffer has already been flushed by the layer above - backends
       should not need to flush anything themselves here */
    integer_32bit (*close)(void *cookie);
} engine_stream_ops;


/* Buffering mode */
typedef enum engine_stream_buffer_mode_e {
    ENGINE_STREAM_UNBUFFERED        = 0,    // every write reaches 'flush_buffer' immediately
    ENGINE_STREAM_LINE_BUFFERED     = 1,    // 'flush_buffer' called upon writing '\n'
    ENGINE_STREAM_FULLY_BUFFERED    = 2     // 'flush_buffer' called only when the buffer fills, or on explicit flush
} engine_stream_buffer_mode;


/* Stream state */

/* The buffer pointer quartet is the actual state machine:

        read_ptr < read_end     -> bytes available without touching the backend
        read_ptr == read_end    -> buffer is exhausted, next read triggers 'fill_buffer'
        
        write_ptr < write_end   -> room to buffer without touching the backend
        write_ptr == write_end  -> buffer is full, next write triggers 'flush_buffer' 
        
   A stream is either 'in read mode* or 'in write mode' at any given
   time (never genuinely both at once, even for streams opened 
   read/write) - switching between the two directions requires reconcilling 
   position via 'seek;, same constraint C's stdio has always had for update-mode
   ("r+"/"w+") streams */
typedef struct engine_stream_s {
    const engine_stream_ops *ops;
    void *cookie;

    /* Absolute backend position corresponding to 'buf_base', used to 
       compute 'engine_stream_tell' without needing a backend round-trip
       on every call. Updated on 'fill_buffer'/'flush_buffer'/'seek' */
    integer_64bit_signed buf_base_position;

    /* Explicit locking. Recursive by thread: the owning thrad may re-enter
       without deadlocking */
    integer_32bit lock;
    engine_thread_id lock_owner;
    integer_32bit lock_depth;

    integer_8bit_unsigned *buf_base;    // start of the allocated backing buffer
    integer_8bit_unsigned *buf_end;     // one past the last allocated byte

    integer_8bit_unsigned *read_ptr;    // next unread byte
    integer_8bit_unsigned *read_end;    // one past the last valid buffered byte

    integer_8bit_unsigned *write_ptr;   // next byte to write into the buffer
    integer_8bit_unsigned *write_end;   // one past the last byte the buffer can hold before 'flush_buffer' is needed

    engine_stream_buffer_mode mode;

    bool_8bit eof_flag;
    bool_8bit error_flag;
    bool_8bit owns_buffer;      // true if 'buf_base' was allocated by this layer
} engine_stream;


/* Lifecycle */

/* Initialize 'stream' to use 'ops'/'cookie' as its backend, with an
   internally-allocated buffer of 'buffer_size' bytes (0 selected a 
   reasonable default). Does not itself open any OS-level resource -
   callers construct 'cookie' beforehand (e.g. having already called
   'open()'/'CreateFile()') and pass it in already valid */
integer_32bit engine_stream_init (
    engine_stream *stream,
    const engine_stream_ops *ops,
    void *cookie,
    engine_size_t buffer_size
) ENGINE_NONNULL(1, 2);

/* Flushes any pending write buffers, then calls ops->close. Does not
   free 'stream' itself - callers own that memory (typically embedded
   in a larger struct) */
integer_32bit engine_stream_close (engine_stream *stream) ENGINE_NONNULL(1);


/* Buffering control */

/* Must be called before the first read/write on the stream - changing
   mode or buffer mid-use is undefined */
integer_32bit engine_stream_set_buffer (
    engine_stream *stream,
    integer_8bit_unsigned *user_buf,
    engine_size_t size,
    engine_stream_buffer_mode mode
) ENGINE_NONNULL(1);


/* Character I/O */

/* These are the two functions worth understanding closely - they ARE
   the buffering state machine. Everything else that read/writes more
   than one byte is built by looping these (or, for efficiency, by
   directly manipulating 'read_ptr/'write_ptr' in bulk when more than a
   few bytes are wanted) */

/* Returns the next byte as an unsigned char cast to in, or 'ENGINE_STREAM_EOF'
   on end-of-stream/error. Only calls 'ops->fill_buffer' when 'read_ptr' == 'read_end' */
integer_32bit engine_stream_getc (engine_stream *stream) ENGINE_NONNULL(1);

/* Writes one byte. Calls 'ops->flush_buffer' when 'write_ptr' == 'write_end'
   (fully/line buffered) or unconditionally (unbuffered mode), and conditionally
   whenever 'c' is '\n' under 'ENGINE_STREAM_LINE_BUFFERED'. Returns the byte 
   written, or 'ENGINE_STREAM_EOF' on error */
integer_32bit engine_stream_putc (integer_32bit c, engine_stream *stream) ENGINE_NONNULL(2);

#define ENGINE_STREAM_EOF  (-1)

/* Push one byte back so the next 'engine_stream_getc' returns it again. Only
   one byte of pushback is guaranteed supported, matching ungetc's own documented
   limit - do not rely on pushing back more than one byte between reads */
integer_32bit engine_stream_ungetc (integer_32bit c, engine_stream *stream) ENGINE_NONNULL(2);


/* Bulk I/O */

/* Not simply 'engine_stream_getc'/'putc' in a loop: these copy directly
   between the caller's buffer and the internal buffer in bulk
   (memcpy-sized chunks), only falling to 'fill_buffer'/'flush_buffer' at
   the actual boundaries, which matters a great deal for large
   transfers - the whole reason fread/fwrite exist as distinct from
   "read one byte at a time" */

engine_size_t engine_stream_read (
    void *dest, 
    engine_size_t size, 
    engine_stream *stream
) ENGINE_NONNULL(1, 3);

engine_size_t engine_stream_write (
    const void *src, 
    engine_size_t size, 
    engine_stream *stream
) ENGINE_NONNULL(1, 3);


/* Positioning */

/* Reconciles buffered state with the backend before delegating to
   'ops->seek' - e.g. a pending write buffer must be flushed before
   seeking, and a read buffer's un-consumed bytes must be accounted
   for so the reported/target position is correct from the caller's
   point of view, not the backend's raw cursor position */

integer_64bit_signed engine_stream_seek (
    engine_stream *stream,
    integer_64bit_signed offset,
    integer_32bit whence
) ENGINE_NONNULL(1);

integer_64bit_signed engine_stream_tell (engine_stream *stream) ENGINE_NONNULL(1);

/* Flush any pending write buffer to the backend without closing */
integer_32bit engine_stream_flush (engine_stream *stream) ENGINE_NONNULL(1);


/* Status */
bool_8bit engine_stream_eof (const engine_stream *stream) ENGINE_NONNULL(1);
bool_8bit engine_stream_error (const engine_stream *stream) ENGINE_NONNULL(1);
void engine_stream_clear_error (engine_stream *stream) ENGINE_NONNULL(1);


/* Explicit locking */

/* Recursive per-thread: the thread already holding the lock (via 
   'engine_stream_lock', or implicitly inside any engine_stream_* call, 
   which all lock/unlock internally by default) may call other 
   engine_stream_* functions without deadlocking */

void engine_stream_lock (engine_stream *stream) ENGINE_NONNULL(1);
integer_32bit engine_stream_trylock (engine_stream *stream) ENGINE_NONNULL(1);
void engine_stream_unlock (engine_stream *stream) ENGINE_NONNULL(1);

#ifdef __cplusplus
}
#endif
