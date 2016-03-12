#include <SDL.h>

const int XMSG_BUFFER_SIZE		= 4096;

struct XMessageBuffer
{
	SDL_Event** table;

	int Size;
	int FirstIndex;
	int LastIndex;

	void clear(void);
	void put(SDL_Event* p);
	//void put(void* hWnd,int msg,SDL_Event &e);
	int get(SDL_Event* p);

	XMessageBuffer(void);
};

extern XMessageBuffer* XMsgBuf;
