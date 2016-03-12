#include "global.h"

#include "3d/3d_math.h"

#include "sqexp.h"
#include "backg.h"

#include "terra/vmap.h"

//BackgroundDispatcher* BackD;

void BackgroundElement::BackRestore(void)
{
};

void BackgroundDispatcher::init(int sz)
{
	size = sz;
	point = 0;
	BkData = new BackgroundElement*[size];
};

void BackgroundDispatcher::free(void)
{
	delete[] BkData;
};

void BackgroundDispatcher::close(void)
{
	point = 0;
};

void BackgroundDispatcher::put(BackgroundElement* p)
{
	if(point < size){
		BkData[point] = p;
		point++;
	}else ErrH.Abort("Background stack overflow");
};

void BackgroundDispatcher::restore(void)
{
	while(point != 0){
		point--;
		BkData[point]->BackRestore();
	};
};
