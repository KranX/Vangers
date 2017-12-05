
#ifndef __XGLOBAL_H
#define __XGLOBAL_H


#ifdef _WIN32
#include <windows.h>
#include <process.h>
#endif

#include <SDL.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef __WORDSIZE
#else
#if (defined __LP64__)
#define __WORDSIZE = 64
#else
#define __WORDSIZE = 32
#endif
#endif

#ifdef WIN32
#define snprintf sprintf_s
#endif

#define _CONV_BUFFER_LEN	63

#include "port.h"
#include "xtcore.h"
//#include "xconsole.h"
//#include "xkey.h"
#include "xerrhand.h"
#include "xbuffer.h"
#include "xstream.h"
#include "xrec.h"
#include "xutl.h"
#include "xcpuid.h"
#include "xmsgbuf.h"
#include "xzip.h"
#include "xt_list.h"
#ifdef __cplusplus
extern "C"
{
#endif
#include "iniparser/iniparser.h"
#ifdef __cplusplus
}
#endif
#endif

