/*
	"Critical Section" sinchronisation method
	for "protected" data in multi-processing application
*/

#ifndef __XCRITICAL_H__
#define __XCRITICAL_H__

#include "SDL_mutex.h"

struct XCriticalSection
{
	SDL_mutex *csection;
	XCriticalSection(SDL_mutex *section) { csection = section; SDL_mutexP(csection); }
	~XCriticalSection(void) { SDL_mutexV(csection); }
};


#endif //__XCRITICAL_H__
