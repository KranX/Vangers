#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <ctime>
#include <memory>

#if (defined(__unix__) || defined(__APPLE__))
#include <dirent.h>
#endif

#ifdef _NT

#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

namespace KDWIN
{
#include <windows.h>
}
#endif

#ifndef _SURMAP_
#define _ROAD_
//#define RUSSIAN_VERSION
#endif

typedef void* HANDLE;

#include "xglobal.h"
#include "xgraph.h"

#include "common.h"

extern char* win32_findfirst(const char* mask);
extern char* win32_findnext();

typedef unsigned char uchar;

#ifdef _ROAD_
#define ML_COMPRESS_ON
#endif

#ifdef _STEAM_API_
#	include <steam/steam_api.h>
#	ifdef STEAM_CEG
// Steam DRM header file
#		include "cegclient.h"
#	else
#		define Steamworks_InitCEGLibrary() (true)
#		define Steamworks_TermCEGLibrary()
#		define Steamworks_TestSecret()
#		define Steamworks_SelfCheck()
#	endif
#endif

extern bool SetAchievement(std::string name);

#endif
