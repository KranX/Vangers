
/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "xglobal.h"

/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */
/* --------------------------- DEFINITION SECTION --------------------------- */

XMessageBuffer* XMsgBuf = NULL;

XMessageBuffer::XMessageBuffer(void)
{
	int i;
	char* heap = new char[XMSG_BUFFER_SIZE * sizeof(SDL_Event)];

	table = new SDL_Event*[XMSG_BUFFER_SIZE];
	for(i = 0; i < XMSG_BUFFER_SIZE; i ++)
		table[i] = (SDL_Event*)(heap + i * sizeof(SDL_Event));

	clear();
}

void XMessageBuffer::put(SDL_Event* p)
{
//	std::cout<<"XMessageBuffer::put Size:"<<Size<<std::endl;
	if(Size < XMSG_BUFFER_SIZE){
		memcpy((char*)table[LastIndex],(char*)p,sizeof(SDL_Event));
		LastIndex ++;
		if(LastIndex >= XMSG_BUFFER_SIZE)
			LastIndex = 0;
		Size ++;
	}
	else
		ErrH.Abort("XMessageBuffer overflow...");
}

/*void XMessageBuffer::put(HANDLE hWnd,int msg,SDL_Event &e)
{
	if(Size < XMSG_BUFFER_SIZE){
		*table[LastIndex] = e;

		LastIndex ++;
		if(LastIndex >= XMSG_BUFFER_SIZE)
			LastIndex = 0;
		Size ++;
	}
	else
		ErrH.Abort("XMessageBuffer overflow...");
}*/

int XMessageBuffer::get(SDL_Event* p)
{
	if(Size){
		memcpy((char*)p,(char*)table[FirstIndex],sizeof(SDL_Event));

		FirstIndex ++;
		if(FirstIndex >= XMSG_BUFFER_SIZE)
			FirstIndex = 0;

		Size --;

		return 1;
	}
	return 0;
}

void XMessageBuffer::clear(void)
{
	Size = 0;
	FirstIndex = LastIndex = 0;
}


