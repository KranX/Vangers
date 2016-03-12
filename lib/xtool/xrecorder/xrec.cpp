/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "xglobal.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */

int xtDispatchMessage(SDL_Event* msg);
void xtProcessMessageBuffer(void);
void xtSysQuant(void);

/* --------------------------- DEFINITION SECTION --------------------------- */

//#define _XRECORDER_LOG_

#define XREC_MAX_CMESSAGE	3
#define XREC_MAX_IMESSAGE	3

// Captured events...
static int XREC_CMESSAGES[XREC_MAX_CMESSAGE] =
{
	SDL_KEYDOWN,
	SDL_MOUSEBUTTONDOWN,
	SDL_JOYBUTTONDOWN
};

// Ignored events...
static int XREC_IMESSAGES[XREC_MAX_IMESSAGE] =
{
	SDL_MOUSEMOTION,
	SDL_MOUSEBUTTONUP,
	SDL_KEYUP
};

XRecorder XRec;

#ifdef	_XRECORDER_LOG_
XStream XRecLog;
#endif

XRecorder::XRecorder(void)
{
	flags = 0;
	frameCount = controlCount = 0;

	hFile = NULL;
	nextMsg = NULL;
	hWnd = NULL;
}

void XRecorder::Open(char* fname,int mode)
{
	int fmode;
	if(flags & (XRC_RECORD_MODE | XRC_PLAY_MODE))
		Close();

	if(!nextMsg){
		nextMsg = new XRecorderMessage;
		nextMsg -> data = new SDL_Event;
		nextMsgDataSize = sizeof(SDL_Event);
	}
	if(!hFile)
		hFile = new XStream;

	fmode = (mode == XRC_RECORD_MODE) ? XS_OUT : XS_IN;
	hFile -> open(fname,fmode);
#ifdef	_XRECORDER_LOG_
	if(mode == XRC_RECORD_MODE)
		XRecLog.open("xrec_wr.log",XS_OUT);
	else
		XRecLog.open("xrec_pl.log",XS_OUT);
#endif
	frameCount = 0;
	flags |= mode;
}

void XRecorder::Close(void)
{
	if(flags & (XRC_RECORD_MODE | XRC_PLAY_MODE)){
		hFile -> close();
		flags &= ~(XRC_RECORD_MODE | XRC_PLAY_MODE | XRC_MESSAGE_READ);
#ifdef	_XRECORDER_LOG_
		XRecLog.close();
#endif
	}
}

void XRecorder::GetMessage(void)
{
	if(flags & XRC_PLAY_MODE){
		if(hFile -> eof()){
			Close();
			return;
		}
		*hFile > nextMsg -> Type > nextMsg -> Frame > nextMsg -> DataSize;
		if(nextMsg -> DataSize){
			if(nextMsg -> DataSize > nextMsgDataSize){
				delete nextMsg -> data;
				nextMsg -> data = new SDL_Event;
				nextMsgDataSize = nextMsg -> DataSize;
			}
			hFile -> read((char*)nextMsg -> data, nextMsg -> DataSize);
		}
		flags |= XRC_MESSAGE_READ;
	}
}

void XRecorder::DispatchMessage(void)
{
	SDL_Event e;
	if((flags & XRC_MESSAGE_READ) && frameCount == nextMsg -> Frame){
		switch(nextMsg -> Type){
			case XRC_USER_MESSAGE:
				break;
			case XRC_XMOUSE_MESSAGE:
			case XRC_SYSTEM_MESSAGE:
				memcpy(&e, nextMsg->data, sizeof(SDL_Event));
				xtDispatchMessage(&e);
				break;
		}
#ifdef	_XRECORDER_LOG_
		XRecLog < "\r\nEvent -> " <= nextMsg -> Type < "(" <= nextMsg -> data[0] < "), frame = " <= frameCount;
#endif
	}
}

void XRecorder::Flush(void)
{
#ifdef	_XRECORDER_LOG_
		XRecLog < "\r\nEvent -> " <= nextMsg -> Type < "(" <= nextMsg -> data[0] < "), frame = " <= frameCount;
#endif
	*hFile < nextMsg -> Type < nextMsg -> Frame < nextMsg -> DataSize;
	if(nextMsg -> DataSize){
		hFile -> write((char*)nextMsg -> data, nextMsg -> DataSize);
	}
}

void XRecorder::Quant(void)
{
	
	frameCount ++;

	if(flags & XRC_PLAY_MODE){
		if(!(flags & XRC_MESSAGE_READ))
			GetMessage();

		while(flags & XRC_MESSAGE_READ && frameCount == nextMsg -> Frame){
			DispatchMessage();
			GetMessage();
		}
	}

	xtSysQuant();

	xtClearMessageQueue();
	xtProcessMessageBuffer();
}

void XRecorder::PutSysMessage(int id,SDL_Event *e)
{
	nextMsg -> Type = id;
	nextMsg -> Frame = frameCount;
	nextMsg -> DataSize = sizeof(SDL_Event);

	memcpy(nextMsg -> data, e, sizeof(SDL_Event));

	Flush();
}

/*void XRecorder::PutMessage(int msg,int sz,void* p)
{
	int i;
	int* ptr = (int*)p;

	nextMsg -> Type = msg;
	nextMsg -> Frame = frameCount;
	nextMsg -> DataSize = sz;

	if(nextMsgDataSize < nextMsg -> DataSize){
		delete nextMsg -> data;
		nextMsg -> data = new SDL_Event;
		nextMsgDataSize = nextMsg -> DataSize;
	}

	for(i = 0; i < sz; i ++)
		nextMsg -> data[i] = ptr[i];

	Flush();
}*/

int XRecorder::CheckMessage(int code)
{
	int i;
	for(i = 0; i < XREC_MAX_CMESSAGE; i ++){
		if(XREC_CMESSAGES[i] == code){
			if(flags & XRC_PLAY_MODE)
				Close();
			return 0;
		}
	}
	for(i = 0; i < XREC_MAX_IMESSAGE; i ++){
		if(XREC_IMESSAGES[i] == code) return 0;
	}
	return 1;
}
