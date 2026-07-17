/*
	"Critical Section" sinchronisation method
	for "protected" data in multi-processing application
*/

#ifndef __XCRITICAL_H__
#define __XCRITICAL_H__

#include <SDL3/SDL_mutex.h>

struct XCriticalSection {
	SDL_Mutex *csection;
	XCriticalSection(SDL_Mutex *section) {
		csection = section;
		SDL_LockMutex(csection);
	}
	~XCriticalSection(void) {
		SDL_UnlockMutex(csection);
	}
};

#endif //__XCRITICAL_H__
