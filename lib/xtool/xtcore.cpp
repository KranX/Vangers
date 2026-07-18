/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../xgraph/xgraph.h"
#include "lang.h"
#include "xglobal.h"
#include "xt_list.h"

#include <SDL3/SDL_main.h>

#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
#	include <locale.h>
#endif

int __internal_argc;
char **__internal_argv;

/* ----------------------------- STRUCT SECTION ----------------------------- */

struct xtMsgHandlerObject {
	int ID;

	void (*Handler)(SDL_Event *);

	void *list;
	xtMsgHandlerObject *next;
	xtMsgHandlerObject *prev;

	xtMsgHandlerObject(void (*p)(SDL_Event *), int id);
};

/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void xtSetExit(void);
// int xtNeedExit(void);

int xtCallXKey(SDL_Event *m);
void xtSysQuant(void);

void xtAddSysObj(XList *lstPtr, void (*fPtr)(void), int id);
void xtDeleteSysObj(XList *lstPtr, int id);
void xtDeactivateSysObj(XList *lstPtr, int id);

void xtReadConsoleInput(void);

void xtRegisterSysMsgFnc(void (*fPtr)(SDL_Event *), int id);
void xtRegisterSysFinitFnc(void (*fPtr)(void), int id);
void xtUnRegisterSysFinitFnc(int id);
void xtDeactivateSysFinitFnc(int id);
void xtSysFinit(void);

// void xtPostMessage(HANDLE hWnd,int msg,int wp,int lp);
int xtDispatchMessage(SDL_Event *msg);
static void xtEventQuant(void);
static void xtProcessMessageBuffer(void);

/* --------------------------- DEFINITION SECTION --------------------------- */

// #define _RTO_LOG_

typedef void (*XFNC)();

#define XT_DEFAULT_TABLE_SIZE 32

XRuntimeObject **XRObjTable = NULL;
unsigned int XRObjTableSize = 0;

XRuntimeObject *XRObjFirst = NULL;
XRuntimeObject *XRObjLast = NULL;

const char *XToolClassName = "XToolClass";
const char *XToolWndName = "XToolWindow";

void *XAppHinst = NULL;
void *XGR_hWnd = NULL;

void *hXConOutput = NULL;
void *hXConInput = NULL;

int XAppMode = 0;

void (*press_handler)(SDL_Event *m);
void (*unpress_handler)(SDL_Event *m);

XList XSysQuantLst;
XList XSysFinitLst;
xtList<xtMsgHandlerObject> XSysHandlerLst;

double XTCORE_FRAME_DELTA = 0;
double XTCORE_FRAME_NORMAL = 0;

#ifdef _RTO_LOG_
XStream xtRTO_Log;
#endif

int xtSysQuantDisabled = 0;
static int xtFrameCount = 0;
static bool xtExitRequested = false;
static void (*xtAudioPauseHandler)(bool) = nullptr;
extern bool XGR_FULL_SCREEN;

int SkipIntro = 0;

bool autoconnect = false;
char *autoconnectHost;
unsigned short autoconnectPort = 2197;
bool autoconnectJoinGame = false;
int autoconnectGameID;

XRuntimeObject *XObj = nullptr;

int getCurRtoId() {
	return XObj == nullptr ? 0 : XObj->ID;
}

int xtGetFrameCount(void) {
	return xtFrameCount;
}

void xtSetAudioPauseHandler(void (*handler)(bool)) {
	xtAudioPauseHandler = handler;
}

int main(int argc, char *argv[]) {
	int id, prevID;
	Uint64 clockDelta, clockCnt, clockNow, clockCntGlobal, clockNowGlobal;
	__internal_argc = argc;
	__internal_argv = argv;

#ifdef _WIN32
	std::cout << "Load backtrace" << std::endl;
	LoadLibraryA("backtrace.dll");
	std::cout << "Set priority class" << std::endl;
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
#endif

	for (int i = 1; i < argc; i++) {
		std::string cmd_key = argv[i];
		if (cmd_key == "-fullscreen") {
			XGR_FULL_SCREEN = true;
		} else if (cmd_key == "-skipintro") {
			SkipIntro = 1;
		} else if (cmd_key == "-russian") {
			setLang(RUSSIAN);
		} else if (cmd_key == "-server") {
			if (argc > i) {
				i++;
				autoconnect = true;
				autoconnectHost = argv[i];
			} else {
				std::cout << "Invalid parameter usage: '-server hostname' expected" << std::endl;
			}
		} else if (cmd_key == "-port") {
			if (argc > i) {
				i++;
				autoconnectPort = (unsigned short)strtol(argv[i], NULL, 0);
			} else {
				std::cout << "Invalid parameter usage: '-port value' expected" << std::endl;
			}
		} else if (cmd_key == "-game") {
			if (argc > i) {
				i++;
				std::string value = argv[i];
				autoconnectJoinGame = true;
				if (value == "new") {
					autoconnectGameID = 0;
				} else if (value == "any") {
					autoconnectGameID = -1;
				} else {
					autoconnectGameID = (int)strtol(argv[i], NULL, 0);
				}
			} else {
				std::cout << "Invalid parameter usage: '-game [id|new|any]' expected" << std::endl;
			}
		} else {
			std::cout << "Unknown parameter: '" << cmd_key << "'" << std::endl;
		}
	}

#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
	std::cout << "Set locale. ";
	char *res = setlocale(LC_NUMERIC, "POSIX");
	std::cout << "Result:" << res << std::endl;
#endif
	// Set handlers to null
	press_handler = NULL;
	unpress_handler = NULL;

	XMsgBuf = new XMessageBuffer;

	initclock();
	prevID = 0;
#ifdef _WIN32
	set_signal_handler();
#endif
	id = xtInitApplication();
	XObj = xtGetRuntimeObject(id);
#ifdef _RTO_LOG_
	xtRTO_Log.open("xt_rto_w.log", XS_OUT);
#endif

	while (XObj) {
		XObj->Init(prevID);
		prevID = id;
		id = 0;

		clockCnt = clocki();
		clockCntGlobal = clockCnt;
		while (!id) {
			if (XObj->Timer) {
				const Uint64 frameTime = static_cast<Uint64>(XObj->Timer);
				id = XObj->Quant();
				clockNow = clockNowGlobal = clocki();
				clockDelta = clockNow - clockCnt;
				XTCORE_FRAME_DELTA = (clockNowGlobal - clockCntGlobal) / 1000.0;
				XTCORE_FRAME_NORMAL = XTCORE_FRAME_DELTA / 0.050; // 20FPS
				clockCntGlobal = clockNowGlobal;
				// std::cout<<"XTCORE_FRAME_DELTA:"<<XTCORE_FRAME_DELTA
				// 		 <<" XTCORE_FRAME_NORMAL:"<<XTCORE_FRAME_NORMAL
				// 		 <<" clockDelta:"<<clockDelta<<std::endl;

				if (clockDelta < frameTime) {
					// std::cout<<"clockDelta:"<<clockDelta<<" Timer:"<<XObj->Timer<<std::endl;
					SDL_Delay(static_cast<Uint32>(frameTime - clockDelta));
				} else {
					std::cout << "Strange deltas clockDelta:" << clockDelta
							  << " Timer:" << XObj->Timer << std::endl;
					if (clockDelta > 300) {
						// something wrong and for preventing abnormal physics set something neutral
						XTCORE_FRAME_NORMAL = 1.0;
					}
				}
				clockCnt = clocki();
			} else {
				id = XObj->Quant();
			}

			if (!xtSysQuantDisabled)
				xtEventQuant();
			XGR_Flip();
			if (xtExitRequested)
				id = XT_TERMINATE_ID;
		}

		XObj->Finit();
#ifdef _RTO_LOG_
		xtRTO_Log < "\r\nChange RTO: " <= XObj->ID < " -> " <= id < " frame -> " <= xtFrameCount;
#endif
		XObj = xtGetRuntimeObject(id);
	}
	xtDoneApplication();
	xtSysFinit();
	SDL_Quit();

#ifdef _RTO_LOG_
	xtRTO_Log.close();
#endif
	return 0;
}

void xtCreateRuntimeObjectTable(int len) {
	int i;
	if (!len)
		len = XT_DEFAULT_TABLE_SIZE;
	XRObjTableSize = len;
	XRObjTable = new XRuntimeObject *[len];

	for (i = 0; i < len; i++) {
		XRObjTable[i] = NULL;
	}
}

XRuntimeObject *xtGetRuntimeObject(unsigned int id) {
	if (id == XT_TERMINATE_ID)
		return NULL;
	if (!XRObjTable || !XRObjTableSize || id < 1 || id > XRObjTableSize)
		ErrH.Abort("XTool system error...");
	return XRObjTable[id - 1];
}

void xtRegisterRuntimeObject(XRuntimeObject *p) {
	if (!XRObjFirst) {
		XRObjFirst = XRObjLast = p;
	} else {
		XRObjLast->next = p;
		XRObjLast = p;
	}
	XRObjTable[p->ID - 1] = p;
}

int xtCallXKey(SDL_Event *m) {
	switch (m->type) {
	case SDL_EVENT_KEY_DOWN:
		if (press_handler) {
			(*press_handler)(m);
		}
		break;
	case SDL_EVENT_KEY_UP:
		if (unpress_handler) {
			(*unpress_handler)(m);
		}
		break;
	case SDL_EVENT_TEXT_INPUT:
		if (press_handler) {
			(*press_handler)(m);
		}
		break;
	case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
		// std::cout<<"CONTROLLERBUTTONDOWN"<<std::endl;
		if (press_handler) {
			(*press_handler)(m);
		}
		break;
	case SDL_EVENT_GAMEPAD_BUTTON_UP:
		// std::cout<<"CONTROLLERBUTTONUP"<<std::endl;
		if (unpress_handler) {
			(*unpress_handler)(m);
		}
		break;
	case SDL_EVENT_MOUSE_WHEEL:
		if (press_handler) {
			(*press_handler)(m);
		}
		break;
	}
	return 1;
}

XList::XList(void) {
	ClearList();
}

XList::~XList(void) {}

void XList::AddElement(XListElement *p) {
	if (!fPtr) {
		fPtr = lPtr = p;
		p->prev = p;
		p->next = NULL;
	} else {
		lPtr->next = p;
		p->prev = lPtr;
		p->next = NULL;
		lPtr = p;
		fPtr->prev = p;
	}
	ListSize++;
}

void XList::RemoveElement(XListElement *p) {
	XListElement *pPtr, *nPtr;

	ListSize--;

	if (ListSize) {
		pPtr = p->prev;
		nPtr = p->next;

		pPtr->next = nPtr;
		if (nPtr)
			nPtr->prev = pPtr;

		if (p == fPtr)
			fPtr = nPtr;
		if (p == lPtr)
			lPtr = pPtr;

		lPtr->next = NULL;
		fPtr->prev = lPtr;
	} else
		ClearList();
}

void xtRegisterSysQuant(void (*qPtr)(void), int id) {
	xtAddSysObj(&XSysQuantLst, qPtr, id);
}

void xtUnRegisterSysQuant(int id) {
	xtDeleteSysObj(&XSysQuantLst, id);
}

void xtRegisterSysFinitFnc(void (*fPtr)(void), int id) {
	xtAddSysObj(&XSysFinitLst, fPtr, id);
}

void xtDeactivateSysFinitFnc(int id) {
	xtDeactivateSysObj(&XSysFinitLst, id);
}

void xtUnRegisterSysFinitFnc(int id) {
	xtDeleteSysObj(&XSysFinitLst, id);
}

void xtDeleteSysObj(XList *lstPtr, int id) {
	XSysObject *p = (XSysObject *)lstPtr->fPtr;
	while (p) {
		if (p->ID == id) {
			lstPtr->RemoveElement((XListElement *)p);
			delete p;
			return;
		}
		p = (XSysObject *)p->next;
	}
}

void xtDeactivateSysObj(XList *lstPtr, int id) {
	XSysObject *p = (XSysObject *)lstPtr->fPtr;

	while (p) {
		if (p->ID == id)
			p->flags |= XSYS_OBJ_INACTIVE;
		p = (XSysObject *)p->next;
	}
}

void xtAddSysObj(XList *lstPtr, void (*fPtr)(void), int id) {
	XSysObject *p = (XSysObject *)lstPtr->fPtr;

	while (p) {
		if (p->ID == id)
			return;
		p = (XSysObject *)p->next;
	}

	p = new XSysObject;
	p->ID = id;
	p->QuantPtr = (void *)fPtr;

	lstPtr->AddElement((XListElement *)p);
}

void xtSysQuant(void) {
	XSysObject *p = (XSysObject *)XSysQuantLst.fPtr;
	while (p) {
		(*(XFNC)(p->QuantPtr))();
		p = (XSysObject *)p->next;
	}
}

void xtSysFinit(void) {
	int i, sz = XSysFinitLst.ListSize;
	XSysObject *p = (XSysObject *)XSysFinitLst.lPtr;
	for (i = 0; i < sz; i++) {
		if (!(p->flags & XSYS_OBJ_INACTIVE))
			(*(XFNC)(p->QuantPtr))();
		p = (XSysObject *)p->prev;
	}
}

/*int xtIsActive(void)
{
//	return (WAIT_OBJECT_0 == WaitForSingleObject(hXActiveWndEvent,0)) ? 1 : 0;
};*/

/*int xtNeedExit()
{
//	return (WAIT_OBJECT_0 == WaitForSingleObject(hXNeedExitEvent, 0)) ? 1 : 0;
};*/

void xtSetExit() {
	std::cout << "Exit!" << std::endl;
	xtExitRequested = true;
};

extern int Pause;
int xtDispatchMessage(SDL_Event *msg) {
	int ret = 0;

	xtMsgHandlerObject *p = XSysHandlerLst.first();
	while (p) {
		(*p->Handler)(msg);
		p = p->next;
	}

	ret += xtCallXKey(msg);
	switch (msg->type) {
	case SDL_EVENT_QUIT:
		xtSetExit();
		break;
	case SDL_EVENT_WINDOW_SHOWN:
	case SDL_EVENT_WINDOW_RESTORED:
	case SDL_EVENT_WINDOW_FOCUS_GAINED:
		if (xtAudioPauseHandler)
			xtAudioPauseHandler(false);
		break;
	case SDL_EVENT_WINDOW_HIDDEN:
	case SDL_EVENT_WINDOW_FOCUS_LOST:
		if (xtAudioPauseHandler)
			xtAudioPauseHandler(true);
		break;
	case SDL_EVENT_USER:
		switch (msg->user.code) {
		case CursorAnimationEvent:
			doCursorAnimation();
		}
		break;
	}

	return ret;
}

void xtClearMessageQueue(void) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_MOTION:
		case SDL_EVENT_MOUSE_BUTTON_UP:
		case SDL_EVENT_KEY_UP:
		case SDL_EVENT_GAMEPAD_BUTTON_UP:
			XMsgBuf->put(&event);
			break;
		default:
			xtDispatchMessage(&event);
			break;
		}
	}
}

static void xtProcessMessageBuffer(void) {
	SDL_Event event;
	while (XMsgBuf->get(&event))
		xtDispatchMessage(&event);
}

static void xtEventQuant(void) {
	xtFrameCount++;
	xtSysQuant();
	xtClearMessageQueue();
	xtProcessMessageBuffer();
}

xtMsgHandlerObject::xtMsgHandlerObject(void (*p)(SDL_Event *), int id) {
	list = NULL;
	ID = id;

	Handler = p;
}

void xtRegisterSysMsgFnc(void (*fPtr)(SDL_Event *), int id) {
	xtMsgHandlerObject *p = new xtMsgHandlerObject(fPtr, id);
	XSysHandlerLst.append(p);
}

void win32_break(char *error, char *msg) {
	std::cout << "--------------------------------\n";
	std::cout << error << "\n";
	std::cout << msg << "\n";
	std::cout << "--------------------------------\n";
}

void *xtGet_hInstance(void) {
	return XAppHinst;
}

void *xtGet_hWnd(void) {
	return XGR_hWnd;
}

void xtSet_hWnd(void *hWnd) {
	XGR_hWnd = hWnd;
}

void xtSysQuantDisable(int v) {
	xtSysQuantDisabled = v;
}

void set_key_handlers(void (*pH)(SDL_Event *), void (*upH)(SDL_Event *)) {
	press_handler = pH;
	unpress_handler = upH;
}
