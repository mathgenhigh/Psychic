#pragma once

#include "io_lib.h"

#include "core/numbers.h"
#include "core/defines.h"

#include "internal/stream.h"
#include "internal/intrusive_list.h"
#include "internal/low-level_lock.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Magic number validation */

/* 'engine_file' embeds one of these as its first member. A mismatch 
   means the pointer is not a valid, live 'engine_file' - either 
   corrupted, never initialized, or already closed and freed. Cheap 
   and worth checking in every public entrypoint */
#define ENGINE_FILE_MAGIC   0x454E4749u     // "ENGI"

#define ENGINE_CHECK_FILE(file_ptr, return_value)                                         \
    do {                                                                                  \
        if ((file_ptr) == ENGINE_NULLPTR || (file_ptr)->magic != ENGINE_FILE_MAGIC)       \
            return (return_value);                                                        \
    } while (0)


/* Ops table validation */

/* Guards against a corrupted/hijacked 'ops' pointer being called
   through. Every legitimate backend registers its ops struct here at
   startup rather than callers constructing arbitrary ones on the stack
   - a stack- or heap-allocated ops struct pointer can never validate, 
   which is the point: legitimate backends live in one fixed, known table */
#ifndef ENGINE_MAX_REGISTERED_STREAM_BACKENDS
#   define ENGINE_MAX_REGISTERED_STREAM_BACKENDS  16
#endif

/* Registers 'ops' as a known-legitimate backend. Called once per
   backend type at engine startup (e.g. once for the file backend,
   once for the memory backend) - not per-stream-instance. Returns 0
   on success, negative if the table is full */
integer_32bit engine_stream_register_ops (const engine_stream_ops *ops) ENGINE_NONNULL(1);

/* Returns 'ops' unchanged if it matches a registered backend;
   otherwise terminates the process. Called internally by every stream
   dispatch function before invoking a callback through it - callers of
   engine_stream_* do not call this directly */
const engine_stream_ops *engine_stream_validate_ops (const engine_stream_ops *ops) ENGINE_NONNULL(1);


/* Default backend operations */

integer_64bit_signed engine_stream_default_seek (
    void *cookie,
    integer_64bit_signed offset,
    integer_32bit whence
);

integer_32bit engine_stream_default_close (void *cookie);


/* Global open-stream registry */

void engine_stream_registry_add (engine_list_node *node) ENGINE_NONNULL(1);
void engine_stream_registry_remove (engine_list_node *node) ENGINE_NONNULL(1);

/* Calls 'engine_stream_flush' on every currently-registered stream. 
   Used to implement 'engine_file_flush(ENGINE_NULLPTR)' and
   process-exit cleanup */
integer_32bit engine_stream_flush_all (void);

/* Flushes only line-buffered streams - used before operations that
   expert prior line-buffered output to already be visible (e.g.
   before reading from a terminal after prompting) */
void engine_stream_flush_all_linebuffered (void);

void engine_stream_registry_lock (void);
void engine_stream_registry_unlock (void);


/* Static stream protection */

/* Checked before any deallocation of an engine_file* */
bool_8bit engine_file_is_static (const engine_file *file) ENGINE_NONNULL(1);


/* Growable-buffer delimitered read */

/* Reads from 'stream' until 'delimeter' is seen or EOF, 
   growing/reallocating '*line_ptr' via reallocation as needed,
   matching getdelim's documented growable-buffer contract. Returns 
   the number of characters read (excluding the null terminator), or
   -1 on error or immediate EOF with nothing to read */
integer_64bit_signed engine_stream_getdelim (
    integer_8bit **line_ptr,
    engine_size_t *n,
    integer_32bit delimeter,
    engine_stream *stream
) ENGINE_NONNULL(1, 2, 4);

#ifdef __cplusplus
}
#endif