#pragma once

#include "../platform/platform_features.h"
#include "../core/defines.h"
#include "jump_buf_tag.h"

typedef jump_buf_tag jump_buf[1];

/* Jump functions declarations */
integer_32bit engine_setjump (jump_buf env) thrownl;
integer_32bit _engine_setjump (jump_buf env) thrownl;

void engine_longjump (jump_buf env, integer_32bit val) thrownl;
void _engine_longjump (jump_buf env, integer_32bit val) thrownl;

/* POSIX versions */
integer_32bit engine_signal_setjump (jump_buf_tag env[1], integer_32bit savemask) thrownl;
void engine_signal_longjump (jump_buf_tag env[1], integer_32bit val) thrownl;

/* Public API macros */
#define setjump(env)    engine_setjump(env)
#define _setjump(env)   _engine_setjump(env)

#define longjump(env, val)   engine_longjump(env, val)
#define _longjump(env, val)  _engine_longjump(env, val)

#define signal_setjmp(env, m)      engine_signal_setjump(env, m)
#define signal_longjump(env, val)  engine_signal_longjump(env, val) 

