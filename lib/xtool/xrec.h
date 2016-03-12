
#ifndef __XRECORDER_H
#define __XRECORDER_H

// XRecorderMessage types...
enum XRecorderMessages
{
	XRC_SYSTEM_MESSAGE = 1, 	// Keyoard events...
	XRC_XMOUSE_MESSAGE,		// Mouse events...
	XRC_USER_MESSAGE,		// Custom events...

	XRC_MAX_MESSAGE_ID
};

struct XRecorderMessage
{
	int Type;
	int Frame;
	int DataSize;

	SDL_Event* data;

	XRecorderMessage(void){ Type = 0; DataSize = 0; Frame = 0; data = NULL; }
};

// XRecorder flags...
#define XRC_RECORD_MODE 	0x01
#define XRC_PLAY_MODE		0x02
#define XRC_MESSAGE_READ	0x04
#define XRC_EXTERNAL_QUANT	0x08

#define XRC_CONTROL_COUNT	1024
#define XRC_BUFFER_SIZE 	sizeof(SDL_Event)

struct XRecorder
{
	int flags;

	int frameCount;
	int controlCount;

	void* hWnd;

	XStream* hFile;

	int nextMsgDataSize;
	XRecorderMessage* nextMsg;

	void Quant(void);

	void Open(char* fname,int mode);
	void Close(void);
	void Flush(void);

	//void PutMessage(int msg,int sz = 0,void* p = NULL);
	void PutSysMessage(int id,SDL_Event *e);
	void GetMessage(void);
	void DispatchMessage(void);
	int CheckMessage(int code);

	XRecorder(void);
};

extern XRecorder XRec;

#endif /* __XRECORDER_H */

