#include "internal/format_core.h"

#include "io_lib.h"
#include "strings/string_ops.h"

typedef enum length_mode_e {
    LEN_NONE,
    LEN_HH,
    LEN_H,
    LEN_L,
    LEN_LL,
    LEN_Z,
    LEN_T,
    LEN_J
} length_mode;

typedef struct format_spec_s {
    integer_32bit width;
    integer_32bit precision;    // -1 = not specified
    length_mode length;
    integer_8bit conv;
    bool_8bit left, plus, space, hash, zero;
} format_spec;


/* Emits 'text' (length len) into 'out', applying width/left-justify
   padding. This single helper is what every conversion below reduces 
   to - the actual "printf" logic is just: produce the digits/chars,
   then pad */
static engine_size_t emit_padded (
    engine_format_sink sink,
    void *ctx,
    const integer_8bit *text,
    engine_size_t len,
    const format_spec *spec,
    integer_8bit pad_char
) {
    integer_32bit pad = spec->width > (integer_32bit)len 
        ? spec->width - (integer_32bit)len
        : 0;

    if (!spec->left)
        for (integer_32bit i = 0; i < pad; ++i)
            sink(ctx, pad_char);
            
    for (engine_size_t i = 0; i < len; ++i)
        sink(ctx, text[i]);

    if (spec->left)
        for (integer_32bit i = 0; i < pad; ++i)
            sink(ctx, ' ');

    return (engine_size_t)pad + len;
}


static engine_size_t format_signed (
    engine_format_sink sink,
    void *ctx,
    integer_64bit_signed val,
    const format_spec *spec
) {
    integer_8bit buf[32];
    bool_8bit negative = val < 0;

    integer_64bit_unsigned mag = negative 
        ? (integer_64bit_unsigned)(-(val + 1)) + 1
        : (integer_64bit_unsigned)val;

    integer_32bit i = 31;
    buf[i--] = '\0';

    do {
        buf[i--] = '0' + (mag % 10);
        mag /= 10;
    } while (mag > 0);

    integer_8bit *digits = &buf[i + 1];

    integer_8bit sign = negative ? '-' : (spec->plus ? '+' : (spec->space ? ' ' : 0));
    
    integer_8bit full[40];
    integer_32bit pos = 0;
    
    if (sign)
        full[pos++] = sign;

    engine_size_t dlen = engine_strlen(digits);

    /* zero-padding to width goes between sign and digits, not before it */
    if (spec->zero && !spec->left && spec->width > (integer_32bit)(pos + dlen)) {
        integer_32bit zeros = spec->width - pos - (integer_32bit)dlen;
        for (integer_32bit z = 0; z < zeros; ++z)
            full[pos++] = '0';
    }

    engine_memcpy(full + pos, digits, dlen);
    pos += (integer_32bit)dlen;

    format_spec spec_no_zero_pad = *spec;
    if (spec->zero)
        spec_no_zero_pad.width = 0;     // already handled above
    
    return emit_padded(sink, ctx, full, pos, spec->zero ? &spec_no_zero_pad : spec, ' ');
}


static engine_size_t format_unsigned (
    engine_format_sink sink,
    void *ctx,
    integer_64bit_unsigned val,
    integer_32bit base,
    bool_8bit upper,
    const format_spec *spec
) {
    static const integer_8bit *digits_lower = "0123456789abcdef";
    static const integer_8bit *digits_upper = "0123456789ABCDEF";

    const integer_8bit *digit_chars = upper ? digits_upper : digits_lower;

    integer_8bit buf[32];
    integer_32bit i = 31;
    buf[i--] = '\0';

    do {
        buf[i--] = digit_chars[val % base];
        val /= base;
    } while (val > 0);

    return emit_padded(
        sink, ctx, &buf[i + 1], 
        engine_strlen(&buf[i + 1]), spec, 
        spec->zero && !spec->left ? '0' : ' '
    );
}


static engine_size_t format_string (
    engine_format_sink sink,
    void *ctx,
    const integer_8bit *s,
    const format_spec *spec
) {
    if (!s)
        s = "(null)";

    engine_size_t len = engine_strlen(s);

    if (spec->precision >= 0 && (engine_size_t)spec->precision < len)
        len = spec->precision;

    return emit_padded(sink, ctx, s, len, spec, ' ');
}


/* Float conversions are delegated to the toolchain's own snprintf,
   deliberately - correct IEEE-754 shortest-round-trip formatting
   is itself a substantial, separate numerial-algorithms project.
   This engine reuses the platform C library's already-correct 
   implementation rather than risking a subtly-wrong reimplementation */
static engine_size_t format_float (
    engine_format_sink sink,
    void *ctx,
    floating_point_64bit val,
    const format_spec *spec
) {
    integer_8bit fmt[16], buf[64];
    integer_32bit prec = spec->precision >= 0 ? spec->precision : 6;

    engine_snprintf(fmt, sizeof(fmt), "%%.%d%c", prec, spec->conv);

    integer_32bit n = engine_snprintf(buf, sizeof(buf), fmt, val);
    if (n > 0)
        return emit_padded(
            sink, ctx, buf, 
            (engine_size_t)n, spec, 
            spec->zero && !spec->left ? '0' : ' '
        );
    
    return 0;
}


integer_64bit_signed engine_vformat_core (
    engine_format_sink sink,
    void *ctx,
    const integer_8bit *format, 
    va_list ap
) {
    integer_64bit_signed written = 0;
    const integer_8bit *f = format;

    while (*f) {
        if (*f != '%') {
            sink(ctx, *f);
            written++;
            f++;
            continue;
        }
        
        f++; // skip '%'
        if (*f == '%') {
            sink(ctx, '%');
            written++;
            f++;
            continue;
        }

        format_spec spec = {0};
        spec.precision = -1;

        /* flags */
        for (;;) {
            if (*f == '-') {
                spec.left = true;
                f++;
            }
            else if (*f == '+') {
                spec.plus = true;
                f++;
            }
            else if (*f == ' ') {
                spec.space = true;
                f++;
            }
            else if (*f == '#') {
                spec.hash = true;
                f++;
            }
            else if (*f == '0') {
                spec.zero = true;
                f++;
            }
            else 
                break;
        }

        /* width */
        if (*f == '*') {
            spec.width = va_arg(ap, integer_32bit);
            f++;
        }
        else 
            while (*f >= '0' && *f <= '9') {
                spec.width = spec.width * 10 + (*f - '0');
                f++;
            }

        /* precision */
        if (*f == '.') {
            f++;
            if (*f == '*' ) {
                spec.precision = va_arg(ap, integer_32bit);
                f++;
            }
            else 
                while (*f >= '0' && *f <= '9') {
                    spec.precision = spec.precision * 10 + (*f - '0');
                    f++;
                }
        }

        /* length modifier */
        if (*f == 'h') {
            f++;
            spec.length = (*f == 'h') ? (f++, LEN_HH) : LEN_H;
        }
        else if (*f == 'l') {
            f++;
            spec.length = (*f == 'l') ? (f++, LEN_LL) : LEN_L;
        }
        else if (*f == 'z') {
            spec.length = LEN_Z;
            f++;
        }
        else if (*f == 't') {
            spec.length = LEN_T;
            f++;
        }
        else if (*f == 'j') {
            spec.length = LEN_J;
            f++;
        }

        spec.conv = *f;
        if (*f)
            f++;

        switch (spec.conv) {
            case 'd': case 'i': {
                integer_64bit_signed val = (spec.length == LEN_LL || spec.length == LEN_J)
                    ? va_arg(ap, integer_64bit_signed)
                    : (spec.length == LEN_L) ? (integer_64bit_signed)va_arg(ap, long)
                    : va_arg(ap, integer_32bit);
                written += format_signed(sink, ctx, val, &spec);
                break;
            }
            case 'u': case 'o': case 'x': case 'X': {
                integer_64bit_unsigned val = (spec.length == LEN_LL || spec.length == LEN_J)
                    ? va_arg(ap, integer_64bit_unsigned)
                    : (spec.length == LEN_L) ? (integer_64bit_unsigned)va_arg(ap, unsigned long)
                    : va_arg(ap, integer_32bit_unsigned);
                integer_32bit base = (spec.conv == 'o') ? 8 : (spec.conv == 'u') ? 10 : 16;
                written += format_unsigned(sink, ctx, val, base, spec.conv == 'X', &spec);
                break;
            }
            case 'c':
                integer_8bit c = (integer_8bit)va_arg(ap, integer_32bit);
                sink(ctx, c);
                written++;
                break;
            case 's':
                written += format_string(sink, ctx, va_arg(ap, const integer_8bit *), &spec);
                break;
            case 'p': {
                spec.hash = true;
                written += format_unsigned(sink, ctx, (integer_64bit_unsigned)(integer_pointer_unsigned)va_arg(ap, void *), 16, false, &spec);
                break;
            }
            case 'f': case 'e': case 'E': case 'g': case 'G':
                written += format_float(sink, ctx, va_arg(ap, floating_point_64bit), &spec);
                break;
            default:
                /* Unknown conversion */
                sink(ctx, '%');
                sink(ctx, spec.conv);
                written += 2;
                break;
        }
    }

    return written;
}