
#ifndef __XGLOBAL_H
#define __XGLOBAL_H

#ifdef _WIN32
#	include <windows.h>
#	include <process.h>
#endif

#include <SDL3/SDL.h>
#include <ctype.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __WORDSIZE
#else
#	if (defined __LP64__)
#		define __WORDSIZE = 64
#	else
#		define __WORDSIZE = 32
#	endif
#endif

#ifdef WIN32
#	define snprintf sprintf_s
#endif

#define _CONV_BUFFER_LEN 63

#include "port.h"
#include "xtcore.h"
// #include "xconsole.h"
// #include "xkey.h"
#include "xbuffer.h"
#include "xcpuid.h"
#include "xerrhand.h"
#include "xmsgbuf.h"
#include "xstream.h"
#include "xt_list.h"
#include "xutl.h"
#include "xzip.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "iniparser/iniparser.h"
#ifdef __cplusplus
}
#endif
#endif
