#include "io_lib.h"
#include "internal/io_lib_private.h"

integer_32bit engine_file_seek (
    engine_file *stream,
    integer_64bit_signed off,
    integer_32bit whence
) {
    integer_32bit result;

    ENGINE_CHECK_FILE(stream, -1);

    engine_stream_lock(&stream->stream);

    integer_64bit_signed new_pos = engine_stream_seek(&stream->stream, off, whence);
    if (new_pos < 0)
        result = -1;
    else {
        /* 'file_seek' clears the EOF indicator on a successful seek - this
            is explicit, documented ISO C behavior (C11 7.21.9.2p4), not an
            incidental side effect: a stream that hit EOF can become readable
            again after seeking backward, so the flag must not linger */
        stream->eof_flag = false;
        result = 0;
    }

    engine_stream_unlock(&stream->stream);
    return result;
}