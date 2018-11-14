/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "xglobal.h"
#include "xt_list.h"
#include "../xgraph/xgraph.h"

#ifdef __HAIKU__
#include <unistd.h>
#endif


/* ----------------------------- STRUCT SECTION ----------------------------- */

struct xtMsgHandlerObject
{
	int ID;

	void (*Handler)(SDL_Event*);

	void* list;
	xtMsgHandlerObject* next;
	xtMsgHandlerObject* prev;

	xtMsgHandlerObject(void (*p)(SDL_Event*),int id);
};

/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void xtSetExit(void);
//int xtNeedExit(void);

int xtCallXKey(SDL_Event* m);
void xtSysQuant(void);

void xtAddSysObj(XList* lstPtr,void (*fPtr)(void),int id);
void xtDeleteSysObj(XList* lstPtr,int id);
void xtDeactivateSysObj(XList* lstPtr,int id);

void xtReadConsoleInput(void);

void xtRegisterSysMsgFnc(void (*fPtr)(SDL_Event*),int id);
void xtRegisterSysFinitFnc(void (*fPtr)(void),int id);
void xtUnRegisterSysFinitFnc(int id);
void xtDeactivateSysFinitFnc(int id);
void xtSysFinit(void);

//void xtPostMessage(HANDLE hWnd,int msg,int wp,int lp);
int xtDispatchMessage(SDL_Event* msg);
void xtProcessMessageBuffer(void);

/* --------------------------- DEFINITION SECTION --------------------------- */

//#define _RTO_LOG_

typedef void (*XFNC)();

#define XT_DEFAULT_TABLE_SIZE	32

XRuntimeObject** XRObjTable = NULL;
unsigned int XRObjTableSize = 0;

XRuntimeObject* XRObjFirst = NULL;
XRuntimeObject* XRObjLast = NULL;

const char* XToolClassName = "XToolClass";
const char* XToolWndName = "XToolWindow";

void* XAppHinst = NULL;
void* XGR_hWnd = NULL;

void* hXConOutput = NULL;
void* hXConInput = NULL;

int XAppMode = 0;

void (*press_handler)(SDL_Event* m);
void (*unpress_handler)(SDL_Event* m);

XList XSysQuantLst;
XList XSysFinitLst;
xtList<xtMsgHandlerObject> XSysHandlerLst;

double XTCORE_FRAME_DELTA = 0;
double XTCORE_FRAME_NORMAL = 0;

#ifdef _RTO_LOG_
XStream xtRTO_Log;
#endif

int xtSysQuantDisabled = 0;
extern bool XGR_FULL_SCREEN;


#ifdef win_arg
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
#else
int main(int argc, char *argv[])
#endif
{
#ifdef __HAIKU__
	const char *data_dir = getenv("VANGERS_DATA");
	if(data_dir != NULL){
		chdir(data_dir);
	}
#endif
	int id, prevID, clockDelta, clockCnt, clockNow, clockCntGlobal, clockNowGlobal;
	XRuntimeObject* XObj;
	#ifdef _WIN32
		std::cout<<"Load backtrace"<<std::endl;
		LoadLibraryA("backtrace.dll");
		std::cout<<"Set priority class"<<std::endl;
		SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
		putenv("SDL_AUDIODRIVER=DirectSound");
	#endif
#ifdef win_arg
	std::string cmd_line = szCmdLine;
	if(cmd_line.find("-fullscreen")!=std::string::npos) {
		XGR_FULL_SCREEN = true;
	}
	
#else
	int i;
	for(i=1;i<argc;i++) {
		std::string cmd_key = argv[i];
		if (cmd_key=="-fullscreen")
			XGR_FULL_SCREEN = true;
		}
#endif

	//Set handlers to null
	press_handler = NULL;
	unpress_handler = NULL;
	
	XMsgBuf = new XMessageBuffer;

	initclock();
	prevID = 0;
	id = xtInitApplication();
	XObj = xtGetRuntimeObject(id);
#ifdef _RTO_LOG_
	if(XRec.flags & XRC_PLAY_MODE)
		xtRTO_Log.open("xt_rto_p.log",XS_OUT);
	else
		xtRTO_Log.open("xt_rto_w.log",XS_OUT);
#endif

	while(XObj){
		XObj -> Init(prevID);
		prevID = id;
		id = 0;

		clockCnt = clocki();
		clockCntGlobal = clockCnt;
		while(!id) {
			if(XObj->Timer) {
				id = XObj -> Quant();
				clockNow = clockNowGlobal = clocki();
				clockDelta = clockNow - clockCnt;
				XTCORE_FRAME_DELTA = (clockNowGlobal - clockCntGlobal) / 1000.0;
				XTCORE_FRAME_NORMAL = XTCORE_FRAME_DELTA / 0.050; //20FPS
				clockCntGlobal = clockNowGlobal;
//				std::cout<<"XTCORE_FRAME_DELTA:"<<XTCORE_FRAME_DELTA
//						 <<" XTCORE_FRAME_NORMAL:"<<XTCORE_FRAME_NORMAL
//						 <<" clockDelta:"<<clockDelta<<std::endl;

				if (clockDelta < XObj->Timer) {
					SDL_Delay(XObj->Timer - clockDelta);
				} else {
					std::cout<<"Strange deltas clockDelta:"<<clockDelta<<" Timer:"<<XObj->Timer<<std::endl;
				}
				clockCnt = clocki();
			} else {
				id = XObj -> Quant();
			}

			if(!xtSysQuantDisabled)
				XRec.Quant(); // впускает внешние события, записывает их или воспроизводит
			XGR_Flip();
		}

		XObj -> Finit();
#ifdef _RTO_LOG_
		xtRTO_Log < "\r\nChange RTO: " <= XObj -> ID < " -> " <= id < " frame -> " <= XRec.frameCount;
#endif
		XObj = xtGetRuntimeObject(id);
	}
	xtDoneApplication();
	xtSysFinit();

#ifdef _RTO_LOG_
	xtRTO_Log.close();
#endif
	return 0;
}

void xtCreateRuntimeObjectTable(int len)
{
	int i;
	if(!len) len = XT_DEFAULT_TABLE_SIZE;
	XRObjTableSize = len;
	XRObjTable = new XRuntimeObject*[len];

	for(i = 0; i < len; i ++){
		XRObjTable[i] = NULL;
	}
}

XRuntimeObject* xtGetRuntimeObject(unsigned int id)
{
	if(id == XT_TERMINATE_ID)
		return NULL;
	if(!XRObjTable || !XRObjTableSize || id < 1 || id > XRObjTableSize)
		ErrH.Abort("XTool system error...");
	return XRObjTable[id - 1];
}

void xtRegisterRuntimeObject(XRuntimeObject* p)
{
	if(!XRObjFirst){
		XRObjFirst = XRObjLast = p;
	}
	else {
		XRObjLast -> next = p;
		XRObjLast = p;
	}
	XRObjTable[p -> ID - 1] = p;
}

int xtCallXKey(SDL_Event* m) {
	switch(m->type) {
		case SDL_KEYDOWN:
			if (press_handler) {
				(*press_handler)(m);
			}
			break;
		case SDL_KEYUP:
			if (unpress_handler) {
				(*unpress_handler)(m);
			}
			break;
		case SDL_TEXTINPUT:
			if (press_handler) {
				(*press_handler)(m);
			}
			break;
		case SDL_JOYBUTTONDOWN:
			//std::cout<<"jevent down button:"<<(int)m->jbutton.button<<std::endl;
			if (press_handler) {
				(*press_handler)(m);
			}
			break;
		case SDL_JOYBUTTONUP:
			//std::cout<<"jevent up"<<std::endl;
			if (unpress_handler) {
				(*unpress_handler)(m);
			}
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			//std::cout<<"CONTROLLERBUTTONDOWN"<<std::endl;
			if (press_handler) {
				(*press_handler)(m);
			}
			break;
		case SDL_CONTROLLERBUTTONUP:
			//std::cout<<"CONTROLLERBUTTONUP"<<std::endl;
			if (unpress_handler) {
				(*unpress_handler)(m);
			}
			break;
		case SDL_JOYHATMOTION:
			//std::cout<<"SDL_JOYHATMOTION:"<<(int)m->jhat.hat<<" value"<<(int)m->jhat.value<<" k:"<<std::endl;
			if (press_handler) {
				(*press_handler)(m);
			}
			break;
		case SDL_JOYBALLMOTION:
			//std::cout<<"SDL_JOYBALLMOTION:"<<(int)m->jball.ball<<" xrel:"<<m->jball.xrel<<" yrel:"<<m->jball.yrel<<std::endl;
			break;
		case SDL_JOYAXISMOTION:
			//std::cout<<"SDL_JOYAXISMOTION:"<<(int)m->jaxis.axis<<" value"<<m->jaxis.value<<std::endl;
			break;
	}
	return 1;
}

/*int xtCallXKey(SDL_Event* m)
{
	int rec_flag = 0,ret = 0;
	switch(m->type){
		case SDL_KEYDOWN:
			//std::cout<<"xtCallXKey sym:"<<m->key.keysym.sym<<" scancode:"<<(int)m->key.keysym.scancode<<" SDLK_q:"<<SDLK_q<<std::endl;
			// TODO(amdmi3): this is supposed to be executed on WM_CHAR; non-char keys should probably be filtered here
			XKey.LastChar = m->key.keysym.sym;

			XKey.PressFnc(m->key.keysym.sym, m->key.keysym.sym);
			rec_flag = 1;

			if (m->key.keysym.sym == SDLK_LALT || m->key.keysym.sym == SDLK_RALT || m->key.keysym.sym == SDLK_F10)
				ret = 1;
			break;
		case SDL_KEYUP:
			XKey.UnPressFnc(m->key.keysym.sym, m->key.keysym.sym);
			rec_flag = 1;

			if (m->key.keysym.sym == SDLK_LALT || m->key.keysym.sym == SDLK_RALT || m->key.keysym.sym == SDLK_F10)
				ret = 1;
			break;
	}
	if(rec_flag && XRec.flags & XRC_RECORD_MODE){
		XRec.PutSysMessage(XRC_SYSTEM_MESSAGE, m);
	}

	return ret;
}*/

XList::XList(void)
{
	ClearList();
}

XList::~XList(void)
{
}

void XList::AddElement(XListElement* p)
{
	if(!fPtr){
		fPtr = lPtr = p;
		p -> prev = p;
		p -> next = NULL;
	}
	else {
		lPtr -> next = p;
		p -> prev = lPtr;
		p -> next = NULL;
		lPtr = p;
		fPtr -> prev = p;
	}
	ListSize ++;
}

void XList::RemoveElement(XListElement* p)
{
	XListElement* pPtr,*nPtr;

	ListSize --;

	if(ListSize){
		pPtr = p -> prev;
		nPtr = p -> next;

		pPtr -> next = nPtr;
		if(nPtr) nPtr -> prev = pPtr;

		if(p == fPtr) fPtr = nPtr;
		if(p == lPtr) lPtr = pPtr;

		lPtr -> next = NULL;
		fPtr -> prev = lPtr;
	}
	else
		ClearList();
}

void xtRegisterSysQuant(void (*qPtr)(void),int id)
{
	xtAddSysObj(&XSysQuantLst,qPtr,id);
}

void xtUnRegisterSysQuant(int id)
{
	xtDeleteSysObj(&XSysQuantLst,id);
}

void xtRegisterSysFinitFnc(void (*fPtr)(void),int id)
{
	xtAddSysObj(&XSysFinitLst,fPtr,id);
}

void xtDeactivateSysFinitFnc(int id)
{
	xtDeactivateSysObj(&XSysFinitLst,id);
}

void xtUnRegisterSysFinitFnc(int id)
{
	xtDeleteSysObj(&XSysFinitLst,id);
}

void xtDeleteSysObj(XList* lstPtr,int id)
{
	XSysObject* p = (XSysObject*)lstPtr -> fPtr;
	while(p){
		if(p -> ID == id){
			lstPtr -> RemoveElement((XListElement*)p);
			delete p;
			return;
		}
		p = (XSysObject*)p -> next;
	}
}

void xtDeactivateSysObj(XList* lstPtr,int id)
{
	XSysObject* p = (XSysObject*)lstPtr -> fPtr;

	while(p){
		if(p -> ID == id)
			p -> flags |= XSYS_OBJ_INACTIVE;
		p = (XSysObject*)p -> next;
	}
}

void xtAddSysObj(XList* lstPtr,void (*fPtr)(void),int id)
{
	XSysObject* p = (XSysObject*)lstPtr -> fPtr;

	while(p){
		if(p -> ID == id) return;
		p = (XSysObject*)p -> next;
	}

	p = new XSysObject;
	p -> ID = id;
	p -> QuantPtr = (void*)fPtr;

	lstPtr -> AddElement((XListElement*)p);
}

void xtSysQuant(void)
{
	XSysObject* p = (XSysObject*)XSysQuantLst.fPtr;
	while(p){
		(*(XFNC)(p -> QuantPtr))();
		p = (XSysObject*)p -> next;
	}
}

void xtSysFinit(void)
{
	int i,sz = XSysFinitLst.ListSize;
	XSysObject* p = (XSysObject*)XSysFinitLst.lPtr;
	for(i = 0; i < sz; i ++){
		if(!(p -> flags & XSYS_OBJ_INACTIVE))
			(*(XFNC)(p -> QuantPtr))();
		p = (XSysObject*)p -> prev;
	}
	XRec.Close();
}

/*int xtIsActive(void)
{
//	return (WAIT_OBJECT_0 == WaitForSingleObject(hXActiveWndEvent,0)) ? 1 : 0;
};*/

/*int xtNeedExit()
{
//	return (WAIT_OBJECT_0 == WaitForSingleObject(hXNeedExitEvent, 0)) ? 1 : 0;
};*/

void xtSetExit()
{
	std::cout<<"Exit!"<<std::endl;
	SDL_Quit();
	exit(0);
//	ResetEvent(hXActiveWndEvent);
//	SetEvent(hXNeedExitEvent);
};


extern int Pause;
int xtDispatchMessage(SDL_Event* msg)
{
	int ret = 0;

	xtMsgHandlerObject* p = XSysHandlerLst.first();
	while(p){
		(*p -> Handler)(msg);
		p = p -> next;
	}

	ret += xtCallXKey(msg);
	switch(msg -> type){
		case SDL_QUIT:
			xtSetExit();
			break;
		case SDL_WINDOWEVENT:
			switch (msg->window.event) {
				case SDL_WINDOWEVENT_SHOWN:
					//Pause = 0;
					SDL_LockAudioDevice(1);
					SDL_PauseAudioDevice(1, 0);
					SDL_UnlockAudioDevice(1);
					std::cout<<"window show"<<std::endl;
					break;
				case SDL_WINDOWEVENT_HIDDEN:
					//Pause = 1;
					SDL_LockAudioDevice(1);
					SDL_PauseAudioDevice(1, 1);
					SDL_UnlockAudioDevice(1);
					std::cout<<"window hidden"<<std::endl;
					break;
				case SDL_WINDOWEVENT_RESTORED:
					//Pause = 0;
					SDL_LockAudioDevice(1);
					SDL_PauseAudioDevice(1, 0);
					SDL_UnlockAudioDevice(1);
					std::cout<<"window restored"<<std::endl;
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					//Pause = 1;
					SDL_LockAudioDevice(1);
					SDL_PauseAudioDevice(1, 1);
					SDL_UnlockAudioDevice(1);
					std::cout<<"window focus lost"<<std::endl;
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					//Pause = 0;
					SDL_LockAudioDevice(1);
					SDL_PauseAudioDevice(1, 0);
					SDL_UnlockAudioDevice(1);
					std::cout<<"window focus gained"<<std::endl;
					break;
			}
			break;
	}

	return ret;
}

void xtClearMessageQueue(void)
{
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		//std::cout<<"event "<<event.type<<std::endl;
		if(XRec.CheckMessage(event.type)) {
			xtDispatchMessage(&event);
//			if(!xtDispatchMessage(&event))
//				DispatchMessage(&event);
		} else {
			XMsgBuf -> put(&event);
		}
	}
}

void xtProcessMessageBuffer(void)
{
	SDL_Event event;
	while(XMsgBuf -> get(&event)) {
		if(!(XRec.flags & XRC_PLAY_MODE) || XRec.CheckMessage(event.type)){
			xtDispatchMessage(&event);
//			if(!xtDispatchMessage(&event))
//				DispatchMessage(&event);
		}
	}
}

xtMsgHandlerObject::xtMsgHandlerObject(void (*p)(SDL_Event*),int id)
{
	list = NULL;
	ID = id;

	Handler = p;
}

void xtRegisterSysMsgFnc(void (*fPtr)(SDL_Event*),int id)
{
	xtMsgHandlerObject* p = new xtMsgHandlerObject(fPtr,id);
	XSysHandlerLst.append(p);
}

void win32_break(char* error,char* msg)
{
	std::cout<<"--------------------------------\n";
	std::cout<<error<<"\n";
	std::cout<<msg<<"\n";
	std::cout<<"--------------------------------\n";
}

void* xtGet_hInstance(void)
{
	return XAppHinst;
}

void* xtGet_hWnd(void)
{
	return XGR_hWnd;
}

void xtSet_hWnd(void* hWnd)
{
	XGR_hWnd = hWnd;
}

void xtSysQuantDisable(int v)
{
	xtSysQuantDisabled = v;
}


void set_key_nadlers(void (*pH)(SDL_Event*),void (*upH)(SDL_Event*)) {
	press_handler = pH;
	unpress_handler = upH;
}
