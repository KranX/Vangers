
#ifndef __XTCORE_H
#define __XTCORE_H
#include <iostream>
#include <SDL.h>

#ifndef NULL
#define NULL	0L
#endif

// XSysObject IDs...
#define XGRAPH_SYSOBJ_ID	0x01
#define XSOUND_SYSOBJ_ID	0x02
#define XAVI_SYSOBJ_ID		0x03
#define XD3D_SYSOBJ_ID		0x04

struct XListElement
{
	XListElement* next;
	XListElement* prev;

	XListElement(void){ next = prev = NULL; }
};

// fPtr -> prev == lPtr
// lPtr -> next == NULL
struct XList
{
	int ListSize;
	XListElement* fPtr;
	XListElement* lPtr;

	void ClearList(void){ ListSize = 0; fPtr = lPtr = NULL; }

	void AddElement(XListElement* p);
	void RemoveElement(XListElement* p);

	XList(void);
	~XList(void);
};

#define XT_TERMINATE_ID 	0xFFFFFFFF

struct XRuntimeObject
{
	int ID;
	int Timer;

	XRuntimeObject* next;

	virtual void Init(int pID) {
		std::cout<<"Init pure virtual XRuntimeObject pID:"<<pID<<std::endl;
	};
	virtual void Finit(void) {
		};
	virtual int Quant(void){
		if(next)
			return next -> ID;
		return XT_TERMINATE_ID;
	}

	void SetTimer(int t){ Timer = t*10; }

	XRuntimeObject(void){ ID = 1; next = NULL; Timer = 0; }
	XRuntimeObject(int id){ ID = id; next = NULL; Timer = 0; }
	XRuntimeObject(int id,int tm){ ID = id; next = NULL; Timer = tm; }
	~XRuntimeObject(void){ }
};

// XSysObject::flags...
#define XSYS_OBJ_INACTIVE	0x01

struct XSysObject : XListElement
{
	int ID;
	int flags;

	void* QuantPtr;

	XSysObject(void){ ID = 0; flags = 0; QuantPtr = NULL; }
};


void xtCreateRuntimeObjectTable(int len = 0);
void xtRegisterRuntimeObject(XRuntimeObject* p);
XRuntimeObject* xtGetRuntimeObject(unsigned int id);

void xtRegisterSysQuant(void (*qPtr)(void),int id);
void xtUnRegisterSysQuant(int id);

//int xtIsActive(void);
//int xtNeedExit(void);

void xtClearMessageQueue(void);

int xtInitApplication(void);
void xtDoneApplication(void);

void* xtGet_hInstance(void);
void* xtGet_hWnd(void);
void xtSet_hWnd(void* hWnd);
extern void* XGR_hWnd;

void xtSysQuantDisable(int v = 1);
void xtSysQuant(void);

void xtCreateDummyWindow(int x = 0,int y = 0,int sx = 100,int sy = 100);
void xtCloseDummyWindow(void);

void set_key_nadlers(void (*pH)(SDL_Event*),void (*upH)(SDL_Event*));

#define SDLK_JOYSTICK_BUTTON_MASK (1<<28)
#define SDLK_GAMECONTROLLER_BUTTON_MASK (1<<27)
#define SDLK_JOYSTICK_HAT_MASK (1<<26)

extern double XTCORE_FRAME_DELTA;
extern double XTCORE_FRAME_NORMAL;

#endif /* __XTCORE_H */


