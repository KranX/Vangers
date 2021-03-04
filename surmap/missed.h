//
// Created by caiiiycuk on 15.01.2021.
//

#ifndef SURMAP_MISSED_H
#define SURMAP_MISSED_H

#include <functional>

#define CLOCK()		(SDL_GetTicks()*18/1000)

class XKeyWrapper {
  public:
	XKeyWrapper();
	void init(std::function<void(int)> handlerFn, void*);
	void finit();
	bool Pressed(int sdlkKey);
};

class XConWrapper {
  public:
	XConWrapper& operator<(const char* str);
	XConWrapper& operator<=(int value);
};

char *iniparser_getstring_surmap(/*dictionary*/ void * d, const char * cat, const char * key);
void mainWinMinimize();
void mainWinMaximize();

extern XKeyWrapper XKey;
extern XConWrapper XCon;

#endif // SURMAP_MISSED_H
