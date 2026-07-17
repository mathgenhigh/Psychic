#pragma once

#include "core/numbers.h"
#include "core/defines.h"

#include "internal/stream.h"
#include <stdarg.h>

/* Portable core printf engine.
   Supports: flags (-+0 #space), width (literal or *), precision
   (literal or *), length modifiers (hh h l ll z t j), and conversions
   (d i u o x X c s p f e g %) - which covers the overwhelmening majority
   of real-world format strings */

typedef void (*engine_format_sink)(void *ctx, integer_8bit c);

integer_64bit_signed engine_vformat_core (
    engine_format_sink sink,
    void *ctx,
    const integer_8bit *format,
    va_list ap
) ENGINE_NONNULL(1, 2);