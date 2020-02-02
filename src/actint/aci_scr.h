#ifndef __ACI_SCR_H__
#define __ACI_SCR_H__

#include <SDL.h>

// aciScreenResource::type values...
#define ACS_NONE	0
#define ACS_BMP 	1
#define ACS_BML 	2
#define ACS_BMO 	3
#define ACS_XBM 	4

// aciScreenResource::flags...
#define ACS_LOADED		0x01
#define ACS_NO_FLUSH		0x02
#define ACS_BUILD_RESOURCE	0x04

struct aciScreenResource : XListElement
{
	int ID;
	int type;
	int flags;

	char* fname;

	void set_name(char* p);

	virtual void load(void){ };
	virtual void free(void){ };

	virtual void change_coords(int dx,int dy) { 
		std::cout<<"aciScreenResource::change_coords(dx="<<dx<<", dy="<<dy<<")"<<std::endl;
	};

	virtual void redraw(int x,int y,int frame,int mode) {};

	aciScreenResource(void);
	virtual ~aciScreenResource(void);
};

struct aciScreenResourceBMP : aciScreenResource
{
	int SizeX;
	int SizeY;

	unsigned char* data;

	virtual void load(void);
	virtual void free(void);

	virtual void redraw(int x,int y,int frame,int mode);

	aciScreenResourceBMP(void);
	~aciScreenResourceBMP(void);
};

struct aciScreenResourceBML : aciScreenResource
{
	int SizeX;
	int SizeY;
	int NumFrames;

	unsigned char* data;
	unsigned char** frameTable;

	virtual void load(void);
	virtual void free(void);

	virtual void redraw(int x,int y,int frame,int mode);

	aciScreenResourceBML(void);
	~aciScreenResourceBML(void);
};

struct aciScreenResourceBMO : aciScreenResource
{
	int SizeX;
	int SizeY;
	int NumFrames;

	int OffsX;
	int OffsY;

	unsigned char* data;
	unsigned char** frameTable;

	virtual void load(void);
	virtual void free(void);

	virtual void change_coords(int dx,int dy){ OffsX += dx; OffsY += dy; }
	virtual void redraw(int x,int y,int frame,int mode);

	aciScreenResourceBMO(void);
	~aciScreenResourceBMO(void);
};

struct aciScreenResourceXBM : aciScreenResource
{
	int PosX;
	int PosY;

	int CenterX;
	int CenterY;

	int bSizeX;
	int bSizeY;

	int SizeX;
	int SizeY;

	int SideX;
	int SideY;

	int ImageSize;
	unsigned char* data;

	virtual void load(void);
	virtual void free(void);

	virtual void change_coords(int dx,int dy){ PosX += dx; PosY += dy; }
	virtual void redraw(int x,int y,int frame,int mode);

	aciScreenResourceXBM(void);
	~aciScreenResourceXBM(void);
};

struct aciScreenKey : XListElement
{
	int code;
};

struct aciScreenKeyObject
{
	XList* codes;

	int KeyTrap(int code);
	void AddKey(int code);

	aciScreenKeyObject(void);
	~aciScreenKeyObject(void);
};

// aciScreenEventCommand::flags...
#define ACS_COMMAND_STARTED	0x01

struct aciScreenEventCommand : XListElement
{
	int flags;
	int code;
	int StartTimer;
	int data[4];

	void init(void);
	aciScreenEventCommand(void);
};

// aciScreenEvent::flags...
#define ACS_EVENT_ACTIVE	0x01

struct aciScreenEvent : XListElement
{
	int flags;

	int CurTimer;
	int MaxTimer;

	XList* commSeq;
	aciScreenKeyObject* keyObj;

	void AddCommand(aciScreenEventCommand* p);
	void AddKey(int code){ keyObj -> AddKey(code); }

	int KeyTrap(int code){ return keyObj -> KeyTrap(code); }

	void Start(void);
	void Stop(void);

	void Quant(void);

	aciScreenEvent(void);
	~aciScreenEvent(void);
};

struct aciScreenFrameSequence : XListElement
{
	int ID;
	int ResourceID;

	int StartFrame;
	int EndFrame;
	int CurFrame;

	int FrameDelta;

	void reset(void){ CurFrame = StartFrame; }

	aciScreenFrameSequence(void);
};

#define ACS_EVENT_LINE_SIZE	20

struct aciScreenEventLine
{
	int size;
	int first_index;
	int last_index;
	int flags;

	aciScreenEventCommand** table;
	char* mem_heap;

	void clear(void);
	void put(int cd,int dt0 = 0,int dt1 = 0,int dt2 = 0,int dt3 = 0);
	aciScreenEventCommand* get(void);

	aciScreenEventLine(void);
	~aciScreenEventLine(void);
};

// aciScreenObject:type values...
#define ACS_BASE_OBJ		0x01
#define ACS_INPUT_FIELD_OBJ	0x02
#define ACS_SCROLLER_OBJ	0x03

// aciScreenObject::flags...
#define ACS_REDRAW_OBJECT	0x01
#define ACS_ISCREEN_FONT	0x02
#define ACS_ALIGN_CENTER	0x04
#define ACS_ACTIVE_STRING	0x08
#define ACS_BACK_RES		0x10

struct aciScreenObject : XListElement
{
	int ID;
	int type;

	int PosX;
	int PosY;

	int SizeX;
	int SizeY;

	int flags;

	int curFrame;
	int ResID;
	aciScreenResource* resPtr;

	aciScreenFrameSequence* activeSeq;

	XList* frameSeq;
	XList* events;

	virtual void init(void){ }
	virtual void redraw(void);
	virtual void Quant(void){ }

	virtual void change_coords(int dx,int dy){ PosX += dx; PosY += dy; }
	virtual int CheckXY(int x,int y);

	int isActive(void);

	void prepare(void);
	void StopEvents(void);
	void EventQuant(void);

	void AddSeq(aciScreenFrameSequence* p);
	void AddEvent(aciScreenEvent* p);

	void set_redraw(void){ flags |= ACS_REDRAW_OBJECT; }

	aciScreenFrameSequence* GetSequence(int id);

	aciScreenObject(void);
	~aciScreenObject(void);
};

struct aciScreenInputField : aciScreenObject
{
	char* string;
	int MaxStrLen;

	int font;
	int Color;
	int Space;

	int Color0;
	int Color1;
	int Color2;

	virtual void redraw(void);
	virtual void init(void);
	void alloc_str(void);

	void change_state(void);
	int get_state(void);
	void set_state(int v);

	virtual void Quant(void);

	void set_string(char* p){ strcpy(string,p); }

	aciScreenInputField(void);
	~aciScreenInputField(void);
};

struct aciScreenScroller : aciScreenObject
{
	int CurValue;
	int MaxValue;

	int Color;
	int BackColor;

	int ScrResID;
	aciScreenResource* ScrRes;

	int sResID;
	aciScreenResource* sRes;

	int ScrResSX;
	int ScrResSY;

	int sResSX;
	int sResSY;

	int ActiveSX;
	int ActiveSY;

	virtual void init(void);
	virtual void redraw(void);
	virtual int CheckXY(int x,int y);

	void Change(int x);

	aciScreenScroller(void);
	~aciScreenScroller(void);
};

struct aciScreen : XListElement
{
	int ID;
	int flags;

	XList* events;
	XList* objList;

	XList* resources;

	int backResID;
	aciScreenResource* backRes;

	void AddEvent(aciScreenEvent* p);
	void AddObject(aciScreenObject* p);
	void AddResource(aciScreenResource* p);

	void ChangeCoords(int dx,int dy);

	aciScreenObject* GetObject(int id);
	aciScreenResource* GetResource(int id);

	void alloc_mem(void);
	void free_mem(void);

	int redraw(int force = 0);

	void prepare(void);
	void init_objects(void);

	void KeyTrap(int code);
	void EventQuant(void);

	aciScreen(void);
	~aciScreen(void);
};

// aciScreenDispatcher::flags...
#define ACS_FORCED_REDRAW	0x01
#define ACS_FIRST_REDRAW	0x02
#define ACS_NEED_EXIT		0x04

struct aciScreenDispatcher
{
	int flags;
	int QuantCode;

	XList* scrList;

	int curScrID;
	aciScreen* curScr;

	XList* resources;

	aciScreenInputField* activeInput;

	aciScreenEventLine* eventLine;
	aciScreenEventLine* flushLine;

	void AddScreen(aciScreen* p);
	void AddResource(aciScreenResource* p);

	aciScreen* GetScreen(int id);
	aciScreenObject* GetObject(int id);
	aciScreenResource* GetResource(int id);

	void KeyTrap(int code, SDL_Event *event);

	void alloc_mem(void);
	void free_mem(void);

	int Quant(int flush_log = 1);
	void EventQuant(void);
	int ObjectQuant(void);
	void FlushEvents(void);

	int redraw(void);
	void flush(void);

	void PrepareInput(int obj_id);
	void InputQuant(SDL_Event *event);
	void CancelInput(void);
	void DoneInput(void);

	void ChangeCoords(int dx,int dy);

	void SetResource(int obj_id,int res_id,int fr);
	void SetSequence(int obj_id,int seq_id);
	void ChangeScroller(int obj_id,int x);
	void SetScroller(int obj_id,int v,int max_v);

	int GetObjectValue(int obj_id,int flag = 0);

	aciScreenDispatcher(void);
	~aciScreenDispatcher(void);
};

extern aciScreenDispatcher* acsScrD;

aciScreenResource* acsAllocResource(int type);
void acsFreeResource(aciScreenResource* p);

aciScreenObject* acsAllocObject(int type);
void acsFreeObject(aciScreenObject* p);
int sdlEventToCode(SDL_Event *event);
unsigned char UTF8toCP866(unsigned short utf);

#endif
