
#ifndef KDSPLUS_H
#define KDSPLUS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#ifdef _WINDOWS_
#include <windows.h>
#endif

#define KDWIN	

#include "../lib/xtool/xtcore.h"
#include "xsocket.h"
#include "server.h"

extern int frame;
extern XStream fout;
extern int GlobalExit;
extern int StatLogging;
extern int leave_empty_games;

#ifdef _DEBUG
void win32_check();
#define DBGCHECK	win32_check();
#else
#define DBGCHECK
#endif

#endif
