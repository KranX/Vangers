#ifndef __XCOMPAT_H
#define __XCOMPAT_H

/*
 * Small portability shims for compilers whose C runtime does not expose POSIX
 * names. Only Microsoft Visual C++ needs them; GCC/clang (including MinGW)
 * already provide the POSIX functions.
 *
 * `strdup` is intentionally not mapped here: MSVC still declares the POSIX
 * name (as deprecated), and the build defines _CRT_NONSTDC_NO_WARNINGS for it.
 */

#include <string.h>

#ifdef _MSC_VER
#	ifndef strcasecmp
#		define strcasecmp _stricmp
#	endif
#	ifndef strncasecmp
#		define strncasecmp _strnicmp
#	endif
#endif

#endif
