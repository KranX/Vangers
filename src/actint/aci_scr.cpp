/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"
#include "../iscreen/hfont.h"

#include "aci_evnt.h"
#include "aci_scr.h"

#include "a_consts.h"
#include "acsconst.h"


#include "../network.h"

#ifndef _WIN32
#include <arpa/inet.h> // ntohl() FIXME: remove
#endif

/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */

extern int iMouseX;
extern int iMouseY;

extern int NetworkON;

extern char* aciSTR_ON;
extern char* aciSTR_OFF;

extern int iRussian;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

int iGetKeyName(int vkey,int shift,int lng_flag);
void ibsout(int x,int y,void* ptr);

int acsEventActive(aciScreenEventCommand* p);

void acsSendEvent(aciScreenEventCommand* p);
void acsSetFlush(int x,int y,int sx,int sy);

aciScreenResource* acsGetResource(int id);
void acsGetResourceSize(aciScreenResource* p,int& x,int& y,int& sx,int& sy);
int acsGetMaxFrame(int res_id);

void aOutStr(int x,int y,int font,int color,unsigned char* str,int space = 1);
int aStrLen(unsigned char* str,int font,int space = 1);

void acsOutStr(int x,int y,int fnt,int col,unsigned char* str,int space);
void aciPutSpr32(int x,int y,int sx,int sy,void* buf,int col,int col_sz);
int acsStrLen(int fnt,unsigned char* str,int space);

void acsChangeStrState(int id);

void acsLoadFonts(void);
void acsFreeFonts(void);

void acsHandleExtEvent(int code,int data0 = 0,int data1 = 0,int data2 = 0);

/* --------------------------- DEFINITION SECTION --------------------------- */

#define aci_FREE_LIST(lst,p1,p2,tp)	p1 = (tp*)lst -> fPtr;		\
					while(p1){			\
						p2 = (tp*)p1 -> next;	\
						delete p1;		\
						p1 = p2;		\
					}				\
					lst -> ClearList();

#define ACS_MAX_FONT		2
#define ACS_INPUT_TIMER 	3

const int ACS_DELTA_X = (800 - 640) / 2;
const int ACS_DELTA_Y = (600 - 480) / 2;

int acsInputTimer = 0;
int acsInputFlag = 0;

int acsFirstRedraw = 0;

char* acsBackupStr = NULL;
aciScreenDispatcher* acsScrD;

HFont** acsFntTable = NULL;

int aciCurCredits00 = 0;

int acsEventActive(aciScreenEventCommand* p)
{
	aciScreenObject* obj;
	switch(p -> code){
		case ACS_EXEC_SEQUENCE:
			obj = acsScrD -> GetObject(p -> data[0]);
			if(obj -> activeSeq) return 0;
			break;
		case ACS_INPUT_STRING:
			if(acsScrD -> activeInput) return 0;
			break;
		case ACS_EXT_EVENT:
			if(p -> data[0] == ACS_CHANGE_SOUND_MODE){
				obj = acsScrD -> GetObject(ACS_SOUND_MODE);
				if(obj -> activeSeq) return 0;
			}
			if(p -> data[0] == ACS_CHANGE_MUSIC_MODE){
				obj = acsScrD -> GetObject(ACS_MUSIC_MODE);
				if(obj -> activeSeq) return 0;
			}
			if(p -> data[0] == ACS_CHANGE_TUTORIAL_MODE){
				obj = acsScrD -> GetObject(ACS_TUTORIAL_MODE);
				if(obj -> activeSeq) return 0;
			}
			break;
	}
	return 1;
}

aciScreenResource* acsGetResource(int id)
{
	return acsScrD -> GetResource(id);
}

void acsSendEvent(aciScreenEventCommand* p)
{
	acsScrD -> eventLine -> put(p -> code,p -> data[0],p -> data[1],p -> data[2], p -> data[3]);
}

void acsSetFlush(int x,int y,int sx,int sy)
{
	acsScrD -> flushLine -> put(0,x,y,sx,sy);
}

aciScreenScroller::aciScreenScroller(void)
{
	type = ACS_SCROLLER_OBJ;

	CurValue = 0;
	MaxValue = 1;
	Color = BackColor = 0;

	ScrResID = 0;
	ScrRes = NULL;

	sResID = 0;
	sRes = NULL;

	ActiveSX = ActiveSY = 0;

	ScrResSX = ScrResSY = 0;
	sResSX = sResSY = 0;
}

aciScreenScroller::~aciScreenScroller(void)
{
}

aciScreenInputField::aciScreenInputField(void)
{
	type = ACS_INPUT_FIELD_OBJ;

	MaxStrLen = 0;
	string = NULL;
	Color = 140;
	Space = 1;

	Color0 = 140;
	Color1 = -1;
	Color2 = -1;

	font = 0;
}

aciScreenInputField::~aciScreenInputField(void)
{
	if(string) delete string;
}

aciScreen::aciScreen(void)
{
	ID = 0;
	flags = 0;

	backResID = 0;
	backRes = NULL;

	events = new XList;
	objList = new XList;
	resources = new XList;
}

aciScreen::~aciScreen(void)
{
	aciScreenEvent* ev,*ev1;
	aciScreenObject* obj,*obj1;
	aciScreenResource* res,*res1;

	res = (aciScreenResource*)resources -> fPtr;
	while(res){
		res1 = (aciScreenResource*)res -> next;
		res -> free();
		acsFreeResource(res);
		res = res1;
	}
	delete resources;

	aci_FREE_LIST(events,ev,ev1,aciScreenEvent);
	delete events;

	obj = (aciScreenObject*)objList -> fPtr;
	while(obj){
		obj1 = (aciScreenObject*)obj -> next;
		acsFreeObject(obj);
		obj = obj1;
	}
	delete objList;
}

aciScreenDispatcher::aciScreenDispatcher(void)
{
	flags = 0;
	curScr = NULL;
	curScrID = 1;

	QuantCode = 0;

	activeInput = NULL;

	scrList = new XList;
	resources = new XList;

	eventLine = new aciScreenEventLine;
	flushLine = new aciScreenEventLine;
}

aciScreenDispatcher::~aciScreenDispatcher(void)
{
	//aciScreen *scr, *scr1;
	aciScreenResource *res, *res1;

	res = (aciScreenResource*)resources -> fPtr;
	while(res){
		res1 = (aciScreenResource*)res -> next;
		res -> free();
		acsFreeResource(res);
		res = res1;
	}
	delete resources;

	//aci_FREE_LIST(scrList,NULL,NULL,aciScreen);
	delete scrList;

	delete eventLine;
	delete flushLine;
}

aciScreenEventLine::aciScreenEventLine(void)
{
	int i;
	table = new aciScreenEventCommand*[ACS_EVENT_LINE_SIZE];
	mem_heap = new char[ACS_EVENT_LINE_SIZE * sizeof(aciScreenEventCommand)];
	for(i = 0; i < ACS_EVENT_LINE_SIZE; i ++){
		table[i] = (aciScreenEventCommand*)(mem_heap + i * sizeof(aciScreenEventCommand));
		table[i] -> init();
	}
	clear();
}

aciScreenEventLine::~aciScreenEventLine(void)
{
	delete[] mem_heap;
	delete[] table;
	clear();
}

aciScreenEventCommand::aciScreenEventCommand(void)
{
	init();
}

aciScreenObject::aciScreenObject(void)
{
	ID = 0;
	type = ACS_BASE_OBJ;

	PosX = PosY = 0;
	SizeX = SizeY = 0;

	flags = 0;

	curFrame = 0;
	ResID = -1;
	resPtr = NULL;

	frameSeq = new XList;
	events = new XList;
}

aciScreenObject::~aciScreenObject(void)
{
	aciScreenFrameSequence* s,*s1;
	aciScreenEvent* ev,*ev1;

	aci_FREE_LIST(frameSeq,s,s1,aciScreenFrameSequence);
	delete frameSeq;

	aci_FREE_LIST(events,ev,ev1,aciScreenEvent);
	delete events;
}

aciScreenFrameSequence::aciScreenFrameSequence(void)
{
	ID = 0;
	ResourceID = -1;
	StartFrame = EndFrame = CurFrame = FrameDelta = 0;
}

aciScreenEvent::aciScreenEvent(void)
{
	flags = 0;
	CurTimer = 0;
	MaxTimer = 1;

	commSeq = new XList;
	keyObj = new aciScreenKeyObject;
}

aciScreenEvent::~aciScreenEvent(void)
{
	aciScreenEventCommand* p,*p1;
	aci_FREE_LIST(commSeq,p,p1,aciScreenEventCommand);
	delete commSeq;
	delete keyObj;
}

aciScreenKeyObject::aciScreenKeyObject(void)
{
	codes = new XList;
}

aciScreenKeyObject::~aciScreenKeyObject(void)
{
	aciScreenKey* p,*p1;
	aci_FREE_LIST(codes,p,p1,aciScreenKey);
	delete codes;
}

aciScreenResource::aciScreenResource(void)
{
	ID = 0;
	type = ACS_NONE;
	flags = 0;

	fname = NULL;
}

aciScreenResource::~aciScreenResource(void)
{
	if(fname)
		delete fname;
}

aciScreenResourceBMP::aciScreenResourceBMP(void)
{
	type = ACS_BMP;
	SizeX = SizeY = 0;
	data = NULL;
}

aciScreenResourceBMP::~aciScreenResourceBMP(void)
{
	if(data)
		delete data;
}

aciScreenResourceBML::aciScreenResourceBML(void)
{
	type = ACS_BML;
	SizeX = SizeY = NumFrames = 0;
	data = NULL;
}

aciScreenResourceBML::~aciScreenResourceBML(void)
{
	if(data)
		delete data;
}

aciScreenResourceBMO::aciScreenResourceBMO(void)
{
	type = ACS_BMO;
	SizeX = SizeY = NumFrames = 0;
	OffsX = OffsY = 0;
	data = NULL;
}

aciScreenResourceBMO::~aciScreenResourceBMO(void)
{
	if(data)
		delete data;
}

aciScreenResourceXBM::aciScreenResourceXBM(void)
{
	type = ACS_XBM;
	PosX = PosY = 0;
	CenterX = CenterY = 0;
	bSizeX = bSizeY = 0;
	SizeX = SizeY = 0;
	SideX = SideY = 0;

	ImageSize = 0;
	data = NULL;
}

aciScreenResourceXBM::~aciScreenResourceXBM(void)
{
	if(data)
		delete data;
}

void aciScreenResourceBMP::load(void)
{
	XStream fh;
	short sx,sy;
	int sz;

	if(!fname) return;
	fh.open(fname,XS_IN);

	fh > sx > sy;

	SizeX = sx;
	SizeY = sy;
	sz = SizeX * SizeY;

	data = new unsigned char[sz];
	fh.read(data,sz);

	fh.close();

	flags |= ACS_LOADED;
}

void aciScreenResourceBMP::free(void)
{
	if(flags & ACS_LOADED && data){
		delete data;
	}
	flags &= ~ACS_LOADED;
	data = NULL;
}

void aciScreenResourceBML::load(void)
{
	XStream fh;
	short sx,sy,num;
	int i,sz;

	if(!fname) return;
	fh.open(fname,XS_IN);

	fh > sx > sy > num;

	SizeX = sx;
	SizeY = sy;
	NumFrames = num;
	sz = SizeX * SizeY * NumFrames;

	data = new unsigned char[sz];
	fh.read(data,sz);

	fh.close();

	frameTable = new unsigned char*[NumFrames];
	for(i = 0; i < NumFrames; i ++) frameTable[i] = data + i * SizeX * SizeY;

	flags |= ACS_LOADED;
}

void aciScreenResourceBML::free(void)
{
	if(flags & ACS_LOADED){
		if(data) delete data;
		if(frameTable) delete frameTable;
	}
	flags &= ~ACS_LOADED;
	data = NULL;
	frameTable = NULL;
}

void aciScreenResourceBMO::load(void)
{
	XStream fh;
	short sx,sy,num,ox,oy;
	int i,sz;

	if(!(flags & ACS_BUILD_RESOURCE)){
		if(!fname) return;
		fh.open(fname,XS_IN);

		fh > sx > sy > num > ox > oy;

		SizeX = sx;
		SizeY = sy;
		OffsX = ox;
		OffsY = oy;
		NumFrames = num;
		sz = SizeX * SizeY * NumFrames;

		data = new unsigned char[sz];
		fh.read(data,sz);

		fh.close();
	}
	else {
		NumFrames = 1;
		sz = SizeX * SizeY * NumFrames;
		data = new unsigned char[sz];
		XGR_GetSpr(OffsX,OffsY,SizeX,SizeY,data);
	}

	frameTable = new unsigned char*[NumFrames];
	for(i = 0; i < NumFrames; i ++) frameTable[i] = data + i * SizeX * SizeY;

	flags |= ACS_LOADED;
}

void aciScreenResourceBMO::free(void)
{
	if(flags & ACS_LOADED){
		if(data) {
			delete[] data;
		}
		if(frameTable) {
			delete[] frameTable;
		}
	}
	flags &= ~ACS_LOADED;
	data = NULL;
	frameTable = NULL;
}

void aciScreenResourceXBM::load(void)
{
	XStream fh;
	if(!fname) return;

	fh.open(fname,XS_IN);
	fh > PosX > PosY > bSizeX > bSizeY > SizeX > SizeY > ImageSize;
	data = new unsigned char[ImageSize];
	fh.read(data,ImageSize);
	fh.close();

	SideX = SizeX/2;
	SideY = SizeY/2;

	CenterX = PosX + SideX;
	CenterY = PosY + SideY;

	flags |= ACS_LOADED;
}

void aciScreenResourceXBM::free(void)
{
	if(flags & ACS_LOADED && data){
		delete data;
	}
	flags &= ~ACS_LOADED;
	data = NULL;
}

void aciScreenResourceBMP::redraw(int x,int y,int frame,int mode)
{
	if(x < 0) x = 0;
	if(y < 0) y = 0;

	if(data){
		XGR_PutSpr(x,y,SizeX,SizeY,data,mode);
		if(!(flags & ACS_NO_FLUSH))
			acsSetFlush(x,y,SizeX,SizeY);
	}
}

void aciScreenResourceBML::redraw(int x,int y,int frame,int mode)
{
	if(data && frameTable){
		XGR_PutSpr(x,y,SizeX,SizeY,frameTable[frame],mode);
		if(!(flags & ACS_NO_FLUSH))
			acsSetFlush(x,y,SizeX,SizeY);
	}
}

void aciScreenResourceBMO::redraw(int x,int y,int frame,int mode)
{
	if(data && frameTable){
		if(x == -1 && y == -1){
			XGR_PutSpr(OffsX,OffsY,SizeX,SizeY,frameTable[frame],mode);
			if(!(flags & ACS_NO_FLUSH))
				acsSetFlush(OffsX,OffsY,SizeX,SizeY);
		}
		else {
			XGR_PutSpr(x,y,SizeX,SizeY,frameTable[frame],mode);
			if(!(flags & ACS_NO_FLUSH))
				acsSetFlush(x,y,SizeX,SizeY);
		}
	}
}

void aciScreenResourceXBM::redraw(int x,int y,int frame,int mode)
{
	if(data){
		if(x == -1 && y == -1){
			ibsout(PosX,PosY,data);
			if(!(flags & ACS_NO_FLUSH))
				acsSetFlush(PosX,PosY,SizeX,SizeY);
		}
		else {
			ibsout(x,y,data);
			if(!(flags & ACS_NO_FLUSH))
				acsSetFlush(x,y,SizeX,SizeY);
		}
	}
}

int aciScreenKeyObject::KeyTrap(int code)
{
	
	aciScreenKey* p = (aciScreenKey*)codes -> fPtr;
	while(p){
		//std::cout<<"aciScreenKeyObject::KeyTrap "<<p->code<<" "<<code<<std::endl;
		if(p -> code == code)
			return 1;
		p = (aciScreenKey*)p -> next;
	}
	return 0;
}

void aciScreenKeyObject::AddKey(int code)
{
	aciScreenKey* p = new aciScreenKey;
	p -> code = code;
	codes -> AddElement((XListElement*)p);
}

void aciScreenEventCommand::init(void)
{
	int i;
	for(i = 0; i < 4; i ++) data[i] = 0;
	code = StartTimer = 0;
	flags = 0;
}

void aciScreenEventLine::put(int cd,int dt0,int dt1,int dt2,int dt3)
{
	if(size < ACS_EVENT_LINE_SIZE){
		table[last_index] -> code = cd;
		table[last_index] -> data[0] = dt0;
		table[last_index] -> data[1] = dt1;
		table[last_index] -> data[2] = dt2;
		table[last_index] -> data[3] = dt3;

		last_index ++;
		if(last_index >= ACS_EVENT_LINE_SIZE)
			last_index = 0;
		size ++;
	}
	else
		ErrH.Abort("Event buffer overflow...");
}

aciScreenEventCommand* aciScreenEventLine::get(void)
{
	aciScreenEventCommand* p;
	if(size){
		p = table[first_index];

		first_index ++;
		if(first_index >= ACS_EVENT_LINE_SIZE)
			first_index = 0;

		size --;
		return p;
	}
	return NULL;
}

void aciScreenEventLine::clear(void)
{
	size = 0;
	first_index = 0;
	last_index = 0;
}

void aciScreenEvent::AddCommand(aciScreenEventCommand* p)
{
	commSeq -> AddElement((XListElement*)p);
}

void aciScreenObject::AddSeq(aciScreenFrameSequence* p)
{
	frameSeq -> AddElement((XListElement*)p);
}

void aciScreenObject::AddEvent(aciScreenEvent* p)
{
	events -> AddElement((XListElement*)p);
}

void aciScreen::AddEvent(aciScreenEvent* p)
{
	events -> AddElement((XListElement*)p);
}

void aciScreen::AddObject(aciScreenObject* p)
{
	objList -> AddElement((XListElement*)p);
}

void aciScreen::AddResource(aciScreenResource* p)
{
	resources -> AddElement((XListElement*)p);
}

void aciScreenDispatcher::AddScreen(aciScreen* p)
{
	scrList -> AddElement((XListElement*)p);
}

void aciScreenDispatcher::AddResource(aciScreenResource* p)
{
	resources -> AddElement((XListElement*)p);
}

aciScreenResource* aciScreen::GetResource(int id)
{
	aciScreenResource* p = (aciScreenResource*)resources -> fPtr;
	while(p){
		if(p -> ID == id) return p;
		p = (aciScreenResource*)p -> next;
	}
	return NULL;
}

aciScreenResource* aciScreenDispatcher::GetResource(int id)
{
	aciScreenResource* p = (aciScreenResource*)resources -> fPtr;
	while(p){
		if(p -> ID == id) return p;
		p = (aciScreenResource*)p -> next;
	}
	return curScr -> GetResource(id);
}

aciScreen* aciScreenDispatcher::GetScreen(int id)
{
	aciScreen* p = (aciScreen*)scrList -> fPtr;
	while(p){
		if(p -> ID == id) return p;
		p = (aciScreen*)p -> next;
	}
	return NULL;
}

aciScreenObject* aciScreenDispatcher::GetObject(int id)
{
	return curScr -> GetObject(id);
}

aciScreenObject* aciScreen::GetObject(int id)
{
	aciScreenObject* p = (aciScreenObject*)objList -> fPtr;
	while(p){
		if(p -> ID == id) return p;
		p = (aciScreenObject*)p -> next;
	}
	return NULL;
}

void aciScreen::alloc_mem(void)
{
	aciScreenResource* p = (aciScreenResource*)resources -> fPtr;
	while(p){
		p -> load();
		p = (aciScreenResource*)p -> next;
	}
}

void aciScreen::free_mem(void)
{
	aciScreenResource* p = (aciScreenResource*)resources -> fPtr;
	while(p){
		p -> free();
		p = (aciScreenResource*)p -> next;
	}
}

void aciScreenDispatcher::alloc_mem(void)
{
	aciScreenResource* p = (aciScreenResource*)resources -> fPtr;
	while(p){
		p -> load();
		p = (aciScreenResource*)p -> next;
	}
	curScr = GetScreen(curScrID);
	curScr -> prepare();
	curScr -> alloc_mem();
	curScr -> init_objects();

	curScr -> backRes = GetResource(curScr -> backResID);

	flags &= ~ACS_NEED_EXIT;
	flags |= ACS_FORCED_REDRAW;
	flags |= ACS_FIRST_REDRAW;

	KeyTrap(ACS_STARTUP_KEY, nullptr);

	acsLoadFonts();
}

void aciScreenDispatcher::free_mem(void)
{
	aciScreenResource* p = (aciScreenResource*)resources -> fPtr;
	if(activeInput) CancelInput();
	while(p){
		p -> free();
		p = (aciScreenResource*)p -> next;
	}

	acsFreeFonts();
}

void aciScreen::init_objects(void)
{
	aciScreenObject* p = (aciScreenObject*)objList -> fPtr;
	while(p){
		p -> init();
		p -> prepare();
		p = (aciScreenObject*)p -> next;
	}
}

void aciScreenDispatcher::KeyTrap(int code, SDL_Event *event)
{
	if(activeInput) {
		InputQuant(event);
	}
	if (event == nullptr || event->type != SDL_TEXTINPUT) {
		curScr->KeyTrap(code);
	}
}

void aciScreen::KeyTrap(int code)
{
	//std::cout<<"aciScreen::KeyTrap"<<std::endl;
	int x,y,mouse = 0;
	aciScreenEvent* ev;
	aciScreenObject* p;

	if(code < 0){
		mouse = 1;
		x = iMouseX;
		y = iMouseY;
	}

	ev = (aciScreenEvent*)events -> fPtr;
	while(ev){
		if(!(ev -> flags & ACS_EVENT_ACTIVE) && ev -> KeyTrap(code))
			ev -> Start();
		ev = (aciScreenEvent*)ev -> next;
	}

	p = (aciScreenObject*)objList -> fPtr;
	while(p){
		if(!mouse || p -> CheckXY(x,y)){
			ev = (aciScreenEvent*)p -> events -> fPtr;
			while(ev){
				if(!(ev -> flags & ACS_EVENT_ACTIVE) && ev -> KeyTrap(code)) {
					//std::cout<<"Start"<<std::endl;
					ev -> Start();
					}
				ev = (aciScreenEvent*)ev -> next;
			}
		}
		p = (aciScreenObject*)p -> next;
	}
}

void aciScreenObject::prepare(void)
{
	aciScreenFrameSequence* seq;
	activeSeq = NULL;

	StopEvents();
	seq = (aciScreenFrameSequence*)frameSeq -> fPtr;
	while(seq){
		if(seq -> StartFrame == ACS_MAX_FRAME)
			seq -> StartFrame = acsGetMaxFrame(seq -> ResourceID);
		if(seq -> EndFrame == ACS_MAX_FRAME)
			seq -> EndFrame = acsGetMaxFrame(seq -> ResourceID);
		seq = (aciScreenFrameSequence*)seq -> next;
	}
}

void aciScreenObject::StopEvents(void)
{
	aciScreenEvent* ev;
	ev = (aciScreenEvent*)events -> fPtr;
	while(ev){
		ev -> Stop();
		ev = (aciScreenEvent*)ev -> next;
	}
}

int aciScreenObject::isActive(void)
{
	aciScreenEvent* ev;
	ev = (aciScreenEvent*)events -> fPtr;
	while(ev){
		if(ev -> flags & ACS_EVENT_ACTIVE) return 1;
		ev = (aciScreenEvent*)ev -> next;
	}
	return 0;
}

void aciScreen::prepare(void)
{
	aciScreenEvent* ev;

	ev = (aciScreenEvent*)events -> fPtr;
	while(ev){
		ev -> Stop();
		ev = (aciScreenEvent*)ev -> next;
	}
}

void aciScreenEvent::Start(void)
{
	Stop();
	CurTimer = 0;
	flags |= ACS_EVENT_ACTIVE;
}

void aciScreenEvent::Stop(void)
{
	flags &= ~ACS_EVENT_ACTIVE;
	aciScreenEventCommand* p = (aciScreenEventCommand*)commSeq -> fPtr;
	while(p){
		p -> flags &= ~ACS_COMMAND_STARTED;
		p = (aciScreenEventCommand*)p -> next;
	}
}

void aciScreenEvent::Quant(void)
{
	aciScreenEventCommand* p = (aciScreenEventCommand*)commSeq -> fPtr;
	int nextQuant = 1;

	while(p){
		if(p -> StartTimer == CurTimer){
			if(p -> flags & ACS_COMMAND_STARTED){
				if(!acsEventActive(p))
					nextQuant = 0;
			}
			else {
				p -> flags |= ACS_COMMAND_STARTED;
				acsSendEvent(p);
				nextQuant = 0;
			}
		}
		p = (aciScreenEventCommand*)p -> next;
	}
	if(nextQuant){
		CurTimer ++;
		if(CurTimer > MaxTimer) Stop();
	}
}

void aciScreenObject::EventQuant(void)
{
	aciScreenEvent* ev = (aciScreenEvent*)events -> fPtr;
	while(ev){
		if(ev -> flags & ACS_EVENT_ACTIVE)
			ev -> Quant();
		ev = (aciScreenEvent*)ev -> next;
	}
}

void aciScreen::EventQuant(void)
{
	aciScreenObject* p = (aciScreenObject*)objList -> fPtr;
	aciScreenEvent* ev = (aciScreenEvent*)events -> fPtr;
	while(ev){
		if(ev -> flags & ACS_EVENT_ACTIVE)
			ev -> Quant();
		ev = (aciScreenEvent*)ev -> next;
	}
	while(p){
		p -> EventQuant();
		p = (aciScreenObject*)p -> next;
	}
}

void aciScreenDispatcher::EventQuant(void)
{
	curScr -> EventQuant();
}

void aciScreenDispatcher::FlushEvents(void)
{
	aciScreenEventCommand* ev;

	while(eventLine -> size){
		ev = eventLine -> get();
		switch(ev -> code){
			case ACS_EXIT:
				QuantCode = ev -> data[0];
				flags |= ACS_NEED_EXIT;
				break;
			case ACS_SET_RESOURCE:
				SetResource(ev -> data[0],ev -> data[1],ev -> data[2]);
				break;
			case ACS_EXEC_SEQUENCE:
				SetSequence(ev -> data[0],ev -> data[1]);
				break;
			case ACS_CHANGE_SCROLLER:
				ChangeScroller(ev -> data[0],XGR_MouseObj.PosX + XGR_MouseObj.SpotX);
				break;
			case ACS_INPUT_STRING:
				PrepareInput(ev -> data[0]);
				break;
			case ACS_EXT_EVENT:
				acsHandleExtEvent(ev -> data[0],ev -> data[1],ev -> data[2],ev -> data[3]);
				break;
			case ACS_CHANGE_STR_STATE:
				acsChangeStrState(ev -> data[0]);
				break;
		}
		//std::cout<<"aciScreenDispatcher::FlushEvents "<<ev->code<<" QuantCode:"<<QuantCode<<std::endl;
	}
}

void aciScreenDispatcher::flush(void)
{
	aciScreenEventCommand* ev;
	while(flushLine -> size){
		ev = flushLine -> get();
		XGR_Flush(ev -> data[0],ev -> data[1],ev -> data[2],ev -> data[3]);
	}
}

void aciScreenObject::redraw(void)
{
	if(resPtr)
		resPtr -> redraw(PosX,PosY,curFrame,XGR_HIDDEN_FON);
	acsSetFlush(PosX,PosY,SizeX,SizeY);
	flags &= ~ACS_REDRAW_OBJECT;
}

void aciScreen::ChangeCoords(int dx,int dy)
{
	aciScreenResource* p = (aciScreenResource*)resources -> fPtr;
	while(p){
		p -> change_coords(dx,dy);
		p = (aciScreenResource*)p -> next;
	}
}

void aciScreenDispatcher::ChangeCoords(int dx,int dy)
{
	aciScreenResource* p = (aciScreenResource*)resources -> fPtr;
	while(p){
		p -> change_coords(dx,dy);
		p = (aciScreenResource*)p -> next;
	}
	curScr -> ChangeCoords(dx,dy);
}

int aciScreen::redraw(int force)
{
	int ret = 0;
	aciScreenResource* res = (aciScreenResource*)resources -> fPtr;
	aciScreenObject* p = (aciScreenObject*)objList -> fPtr;
	if(force && backRes){
		backRes -> redraw(-1,-1,0,XGR_HIDDEN_FON);
	}
	if(acsFirstRedraw){
		while(res){
			if(res -> flags & ACS_BUILD_RESOURCE){
				res -> free();
				res -> load();
			}
			res = (aciScreenResource*)res -> next;
		}
		acsFirstRedraw = 0;
	}
	while(p){
		if(force || p -> flags & ACS_REDRAW_OBJECT){
			p -> redraw();
			ret = 1;
		}
		p = (aciScreenObject*)p -> next;
	}
	return ret;
}

int aciScreenDispatcher::redraw(void)
{
	int force,ret = 0;

	if(flags & ACS_FORCED_REDRAW){
		force = 1;
		flags ^= ACS_FORCED_REDRAW;
	}
	else
		force = 0;

	if(flags & ACS_FIRST_REDRAW){
		flags ^= ACS_FIRST_REDRAW;
		acsFirstRedraw = 1;
	}
	else
		acsFirstRedraw = 0;

	ret = curScr -> redraw(force);
	return ret;
}

int aciScreenDispatcher::Quant(int flush_log)
{
	int sz,obj_flag = 0;

	EventQuant();
	FlushEvents();
	ObjectQuant();

	if(activeInput){
		acsInputTimer ++;
		if(acsInputTimer >= ACS_INPUT_TIMER){
			acsInputTimer = 0;
			acsInputFlag = 1;
		}
		if(acsInputFlag){
			sz = strlen(activeInput -> string);
			if(activeInput -> string[sz - 1] == '_')
				activeInput -> string[sz - 1] = ' ';
			else
				activeInput -> string[sz - 1] = '_';
			activeInput -> flags |= ACS_REDRAW_OBJECT;
			acsInputFlag = 0;
		}
	}

	obj_flag = redraw();

	if(flush_log)
		flush();
	else
		flushLine -> clear();

	if(!obj_flag && !NetworkON){
		//KDWIN::Sleep(100);
	}

	return QuantCode;
}

int aciScreenDispatcher::ObjectQuant(void)
{
	int ret = 0;
	aciScreenObject* p = (aciScreenObject*)curScr -> objList -> fPtr;
	while(p){
		if(p -> activeSeq){
			p -> ResID = p -> activeSeq -> ResourceID;
			p -> resPtr = GetResource(p -> ResID);
			p -> curFrame = p -> activeSeq -> CurFrame;
			if(p -> activeSeq -> CurFrame == p -> activeSeq -> EndFrame)
				p -> activeSeq = NULL;
			else
				p -> activeSeq -> CurFrame += p -> activeSeq -> FrameDelta;
			p -> flags |= ACS_REDRAW_OBJECT;
			ret = 1;
		}
		p -> Quant();
		p = (aciScreenObject*)p -> next;
	}
	return ret;
}

void aciScreenResource::set_name(char* p)
{
	int sz = strlen(p) + 1;
	fname = new char[sz];
	strcpy(fname,p);
}

void aciScreenDispatcher::SetResource(int obj_id,int res_id,int fr)
{
	aciScreenResource* res = GetResource(res_id);
	aciScreenObject* obj = GetObject(obj_id);

	if(obj && res){
		obj -> ResID = res -> ID;
		obj -> resPtr = res;
		if(fr == ACS_MAX_FRAME)
			fr = acsGetMaxFrame(res_id);
		obj -> curFrame = fr;
		res -> flags |= ACS_NO_FLUSH;
		acsGetResourceSize(res,obj -> PosX,obj -> PosY,obj -> SizeX,obj -> SizeY);
	}
	else
		ErrH.Abort("SetResource() failed...");
}

void aciScreenDispatcher::SetSequence(int obj_id,int seq_id)
{
	aciScreenFrameSequence* seq;
	aciScreenObject* obj = GetObject(obj_id);

	if(obj){
		seq = obj -> GetSequence(seq_id);
		obj -> activeSeq = seq;
		seq -> CurFrame = seq -> StartFrame;
	}
	else
		ErrH.Abort("SetSequence() failed...");
}

aciScreenFrameSequence* aciScreenObject::GetSequence(int id)
{
	aciScreenFrameSequence* seq = (aciScreenFrameSequence*)frameSeq -> fPtr;
	while(seq){
		if(seq -> ID == id) return seq;
		seq = (aciScreenFrameSequence*)seq -> next;
	}
	return NULL;
}

int acsGetMaxFrame(int res_id)
{
	aciScreenResource* p;
	aciScreenResourceBML* bml;
	aciScreenResourceBMO* bmo;

	p = acsScrD -> GetResource(res_id);
	if(!p) return 0;

	switch(p -> type){
		case ACS_BML:
			bml = (aciScreenResourceBML*)p;
			return (bml -> NumFrames - 1);
		case ACS_BMO:
			bmo = (aciScreenResourceBMO*)p;
			return (bmo -> NumFrames - 1);
	}
	return 0;
}

void acsGetResourceSize(aciScreenResource* p,int& x,int& y,int& sx,int& sy)
{
	aciScreenResourceBMP* bmp;
	aciScreenResourceBML* bml;
	aciScreenResourceBMO* bmo;
	aciScreenResourceXBM* xbm;

	switch(p -> type){
		case ACS_BMP:
			bmp = (aciScreenResourceBMP*)p;
			sx = bmp -> SizeX;
			sy = bmp -> SizeY;
			break;
		case ACS_BML:
			bml = (aciScreenResourceBML*)p;
			sx = bml -> SizeX;
			sy = bml -> SizeY;
			break;
		case ACS_BMO:
			bmo = (aciScreenResourceBMO*)p;
			x = bmo -> OffsX;
			y = bmo -> OffsY;
			sx = bmo -> SizeX;
			sy = bmo -> SizeY;
			break;
		case ACS_XBM:
			xbm = (aciScreenResourceXBM*)p;
			x = xbm -> PosX;
			y = xbm -> PosY;
			sx = xbm -> SizeX;
			sy = xbm -> SizeY;
			break;
	}
}

int aciScreenObject::CheckXY(int x,int y)
{
	if(x >= PosX && x < PosX + SizeX && y >= PosY && y < PosY + SizeY)
		return 1;
	return 0;
}

int aciScreenScroller::CheckXY(int x,int y)
{
	int dx = 0;
	if(ScrRes) dx = ScrResSX * 3;
	if(x >= PosX - dx && x < PosX + SizeX + dx && y >= PosY && y < PosY + SizeY)
		return 1;
	return 0;
}

aciScreenResource* acsAllocResource(int type)
{
	aciScreenResource* p = NULL;
	aciScreenResourceBMP* bmp;
	aciScreenResourceBML* bml;
	aciScreenResourceBMO* bmo;
	aciScreenResourceXBM* xbm;

	switch(type){
		case ACS_BMP:
			bmp = new aciScreenResourceBMP;
			p = bmp;
			break;
		case ACS_BML:
			bml = new aciScreenResourceBML;
			p = bml;
			break;
		case ACS_BMO:
			bmo = new aciScreenResourceBMO;
			p = bmo;
			break;
		case ACS_XBM:
			xbm = new aciScreenResourceXBM;
			p = xbm;
			break;
	}
	return p;
}

void acsFreeResource(aciScreenResource* p)
{
	aciScreenResourceBMP* bmp;
	aciScreenResourceBML* bml;
	aciScreenResourceBMO* bmo;
	aciScreenResourceXBM* xbm;

	switch(p -> type){
		case ACS_BMP:
			bmp = (aciScreenResourceBMP*)p;
			delete bmp;
			break;
		case ACS_BML:
			bml = (aciScreenResourceBML*)p;
			delete bml;
			break;
		case ACS_BMO:
			bmo = (aciScreenResourceBMO*)p;
			delete bmo;
			break;
		case ACS_XBM:
			xbm = (aciScreenResourceXBM*)p;
			delete xbm;
			break;
	}
}

aciScreenObject* acsAllocObject(int type)
{
	aciScreenObject* p = NULL;
	aciScreenScroller* scr;
	aciScreenInputField* inp;
	switch(type){
		case ACS_BASE_OBJ:
			p = new aciScreenObject;
			break;
		case ACS_SCROLLER_OBJ:
			scr = new aciScreenScroller;
			p = scr;
			break;
		case ACS_INPUT_FIELD_OBJ:
			inp = new aciScreenInputField;
			p = inp;
			break;
	}
	return p;
}

void acsFreeObject(aciScreenObject* p)
{
	aciScreenScroller* scr;
	aciScreenInputField* inp;
	switch(p -> type){
		case ACS_BASE_OBJ:
			delete p;
			break;
		case ACS_SCROLLER_OBJ:
			scr = (aciScreenScroller*)p;
			delete scr;
			break;
		case ACS_INPUT_FIELD_OBJ:
			inp = (aciScreenInputField*)p;
			delete inp;
			break;
	}
}

void aciScreenScroller::redraw(void)
{
	int x,y,dx,dy,sx,_sx,_sy,sdx,sdy;

	if(ActiveSX) _sx = ActiveSX;
	else _sx = SizeX;

	if(ActiveSY) _sy = ActiveSY;
	else _sy = SizeY;

	sdx = (SizeX - _sx) / 2;
	sdy = (SizeY - _sy) / 2;

	if(flags & ACS_BACK_RES)
		aciScreenObject::redraw();
	if(Color){
		if(!ScrRes)
			sx = (CurValue * _sx) / MaxValue;
		else
			sx = (CurValue * (_sx - ScrResSX)) / MaxValue + ScrResSX/2;

		XGR_Rectangle(PosX + sdx,PosY + sdy,_sx,_sy,BackColor,BackColor,XGR_FILLED);
		XGR_Rectangle(PosX + sdx,PosY + sdy,sx,_sy,Color,Color,XGR_FILLED);
	}
	if(!(flags & ACS_BACK_RES))
		aciScreenObject::redraw();
	if(sRes){
		dx = (SizeX - sResSX) / 2;
		dy = (SizeY - sResSY) / 2;
		sRes -> redraw(PosX + dx,PosY + dy,0,XGR_HIDDEN_FON);
	}
	if(ScrRes){
		sx = (CurValue * (_sx - ScrResSX)) / MaxValue;
		x = PosX + sx;
		y = PosY + ((SizeY - ScrResSY) >> 1);
		ScrRes -> redraw(x + sdx,y,0,XGR_HIDDEN_FON);
	}
}

void aciScreenScroller::Change(int x)
{
	x -= PosX;
	CurValue = (x * MaxValue) / SizeX;

	if(CurValue < 0) CurValue = 0;
	if(CurValue > MaxValue) CurValue = MaxValue;

	flags |= ACS_REDRAW_OBJECT;
}

void aciScreenInputField::redraw(void)
{
	int sz;
	aciScreenObject::redraw();
	if(!(flags & ACS_ALIGN_CENTER)){
		if(flags & ACS_ISCREEN_FONT)
			acsOutStr(PosX,PosY,font,Color,(unsigned char*)string,Space);
		else
			aOutStr(PosX,PosY,font,Color,(unsigned char*)string,Space);
	}
	else {
		if(flags & ACS_ISCREEN_FONT){
			sz = acsStrLen(font,(unsigned char*)string,Space);
			acsOutStr(PosX + (SizeX - sz)/2,PosY,font,Color,(unsigned char*)string,Space);
		}
		else {
			sz = aStrLen((unsigned char*)string,font,Space);
			aOutStr(PosX + (SizeX - sz)/2,PosY,font,Color,(unsigned char*)string,Space);
		}
	}
}

void aciScreenDispatcher::ChangeScroller(int obj_id,int x)
{
	aciScreenScroller* p = (aciScreenScroller*)GetObject(obj_id);
	p -> Change(x);
}

void aciScreenDispatcher::SetScroller(int obj_id,int v,int max_v)
{
	aciScreenScroller* p = (aciScreenScroller*)GetObject(obj_id);
	if(!max_v) max_v = 1;
	p -> CurValue = (p -> MaxValue * v) / max_v;
	p -> flags |= ACS_REDRAW_OBJECT;
}

void aciScreenScroller::init(void)
{
	int x,y;
	aciScreenObject::init();
	ScrRes = acsGetResource(ScrResID);
	if(ScrRes)
		acsGetResourceSize(ScrRes,x,y,ScrResSX,ScrResSY);

	sRes = acsGetResource(sResID);
	if(sRes)
		acsGetResourceSize(sRes,x,y,sResSX,sResSY);
}

void aciScreenDispatcher::InputQuant(SDL_Event *event)
{
	int sz;
	unsigned char* ptr = NULL;
	unsigned char chr;
	HFont* hfnt = NULL;

	if(!activeInput) {
		return;
	}

	if (!(event != nullptr && (event->type == SDL_KEYDOWN || event->type == SDL_TEXTINPUT)))
		return;

	ptr = (unsigned char*)activeInput -> string;
	if(activeInput -> flags & ACS_ISCREEN_FONT) {
		hfnt = acsFntTable[activeInput -> font];
	}

	if (event != nullptr && event->type == SDL_KEYDOWN) {
		if (event->key.keysym.sym > 0 && event->key.keysym.sym < 127) {
			chr = event->key.keysym.sym;
		} else {
			return;
		}

		switch(event->key.keysym.sym) {
			case SDLK_RETURN:
				DoneInput();
				break;
			case SDLK_ESCAPE:
				CancelInput();
				break;
			case SDLK_LEFT:
			case SDLK_BACKSPACE:
				sz = strlen((char*)ptr);
				if(sz > 1) {
					ptr[sz - 1] = 0;
					ptr[sz - 2] = '_';
					activeInput->flags |= ACS_REDRAW_OBJECT;
				}
				break;
		}
	} else if (event != nullptr && event->type == SDL_TEXTINPUT) {
		if ((unsigned char)event->text.text[0] < 128) {
			chr = event->text.text[0];
		} else {
			unsigned short utf = ((unsigned short *)event->text.text)[0];
			utf = ntohs(utf);
			// All cyrilic chars should be coding in two octets. 8, 11 bit-index for 1,2 octets
			if ((utf & (1<<(7))) && !(utf & (1<<(10)))) {
				chr = UTF8toCP866(utf);
			} else {
				chr = ' ';
			}
		}
		if(hfnt && chr < (hfnt->StartChar-hfnt->EndChar+1) && (hfnt->data[chr]->Flags & NULL_HCHAR) && chr != ' ')
			return;
		sz = strlen((char*)ptr);
		if(sz <= activeInput->MaxStrLen) {
			ptr[sz - 1] = chr;
			ptr[sz] = '_';
			ptr[sz + 1] = 0;
			activeInput -> flags |= ACS_REDRAW_OBJECT;
		}
	}

}

// Only for russian letters
unsigned char UTF8toCP866(unsigned short utf) {
	if (utf >= 0xd090 && utf <= 0xd0bf) {
		return utf - 0xd010;
	}

	if (utf >= 0xd180 && utf <= 0xd18f) {
		return utf - 0xd0a0;
	}
	switch(utf) {
		case 0xd081: //Ё
			return 0xf0;
		case 0xd191: //ё
			return 0xf1;
	}
	return 0xdb;
}

void aciScreenDispatcher::PrepareInput(int obj_id)
{
	int sz;
	aciScreenInputField* p = (aciScreenInputField*)GetObject(obj_id);
	if(p){
		if(activeInput){
			activeInput -> StopEvents();
			CancelInput();
		}
		sz = strlen(p -> string) + 1;
		if(!acsBackupStr){
			acsBackupStr = new char[sz];
			strcpy(acsBackupStr,p -> string);
		}
		p -> string[sz - 1] = '_';
		p -> string[sz] = 0;
		p -> flags |= ACS_ACTIVE_STRING;
		activeInput = p;
		SDL_StartTextInput();
	}
}

void aciScreenInputField::alloc_str(void)
{
	string = new char[MaxStrLen + 2];
	memset(string,0,MaxStrLen + 2);
}

void acsLoadFonts(void)
{
	int i;
	XBuffer XBuf;
	if(acsFntTable) acsFreeFonts();
	acsFntTable = new HFont*[ACS_MAX_FONT];
	for(i = 0; i < ACS_MAX_FONT; i ++){
		XBuf.init();
		XBuf < "resource/actint/640x480/hfonts/font";
		if(i < 10) XBuf < "0";
		XBuf <= i < ".fnc";
		acsFntTable[i] = new HFont;
		acsFntTable[i] -> Flags |= FONT_COLORS;
		acsFntTable[i] -> load(XBuf.GetBuf());

		acsFntTable[i] -> data[32] -> LeftOffs = acsFntTable[i] -> data[95] -> LeftOffs;
		acsFntTable[i] -> data[32] -> RightOffs = acsFntTable[i] -> data[95] -> RightOffs;
	}
}

void acsFreeFonts(void)
{
	int i;
	if(!acsFntTable) return;
	for(i = 0; i < ACS_MAX_FONT; i ++)
		delete acsFntTable[i];
	delete[] acsFntTable;
	acsFntTable = NULL;
}

void acsOutStr(int x,int y,int fnt,int col,unsigned char* str,int space)
{
	int _x,_y,i,sx,sy,ss,sz = strlen((char*)str),col_sz;
	HFont* p = acsFntTable[fnt];
	HChar* ch;
	unsigned char* ptr;

	col_sz = (col >> 8) & 0xFF;
	col &= 0xFF;

	_x = x;
	_y = y;
	for(i = 0; i < sz; i ++){
		ch = p -> data[str[i]];
		ptr = ch -> HeightMap;

		sx = ch -> SizeX;
		sy = ch -> SizeY;
		ss = sx * sy;

		if(str[i] == '\n'){
			_x = x;
			_y += sy + space;
		}
		else {
			aciPutSpr32(_x - ch -> LeftOffs,_y,sx,sy,ptr,col,col_sz);
			_x += sx - ch -> RightOffs - ch -> LeftOffs + space;
		}
	}
}

int aciScreenDispatcher::GetObjectValue(int obj_id,int flag)
{
	int ret = 0;
	aciScreenObject* p = GetObject(obj_id);
	if(p){
		switch(p -> type){
			case ACS_BASE_OBJ:
				if(p -> curFrame) ret = 1;
				break;
			case ACS_SCROLLER_OBJ:
				if(!flag)
					ret = ((aciScreenScroller*)p) -> CurValue;
				else
					ret = ((aciScreenScroller*)p) -> MaxValue;
				break;
		}
	}
	return ret;
}

int acsStrLen(int fnt,unsigned char* str,int space)
{
	int i,sx,sz = strlen((char*)str),len = 0;
	HFont* p = acsFntTable[fnt];
	HChar* ch;

	for(i = 0; i < sz; i ++){
		ch = p -> data[str[i]];
		sx = ch -> SizeX;

		if(str[i] != '\n'){
			len += sx - ch -> RightOffs - ch -> LeftOffs + space;
		}
	}
	return len;
}

void aciScreenDispatcher::CancelInput(void)
{
	strcpy(activeInput -> string,acsBackupStr);
	activeInput -> flags |= ACS_REDRAW_OBJECT;
	activeInput -> flags &= ~ACS_ACTIVE_STRING;
	activeInput = NULL;
	delete acsBackupStr;
	acsBackupStr = NULL;
	SDL_StopTextInput();
}

void aciScreenDispatcher::DoneInput(void)
{
	int sz;
	sz = strlen(activeInput -> string);
	activeInput -> string[sz - 1] = 0;
	activeInput -> flags |= ACS_REDRAW_OBJECT;
	activeInput -> flags &= ~ACS_ACTIVE_STRING;
	activeInput = NULL;
	delete acsBackupStr;
	acsBackupStr = NULL;
	SDL_StopTextInput();
}

void aciScreenInputField::Quant(void)
{
	int prev = Color;
	Color = Color0;
	if(!(flags & ACS_ACTIVE_STRING)){
		if(Color1 != -1 && CheckXY(XGR_MouseObj.PosX + XGR_MouseObj.SpotX,XGR_MouseObj.PosY + XGR_MouseObj.SpotY)){
			Color = Color1;
		}
	}
	else {
		if(Color2 != -1) Color = Color2;
	}
	if(prev != Color) flags |= ACS_REDRAW_OBJECT;
}

void aciScreenInputField::init(void)
{
	Color = Color0;
	flags &= ~ACS_ACTIVE_STRING;
}

void aciScreenInputField::change_state(void)
{
	if(!strcmp(string,aciSTR_OFF)){
		strcpy(string,aciSTR_ON);
	}
	else {
		strcpy(string,aciSTR_OFF);
	}
	flags |= ACS_REDRAW_OBJECT;
}

void aciScreenInputField::set_state(int v)
{
	if(v){
		strcpy(string,aciSTR_ON);
	}
	else {
		strcpy(string,aciSTR_OFF);
	}
	flags |= ACS_REDRAW_OBJECT;
}

int aciScreenInputField::get_state(void)
{
	if(!strcmp(string,aciSTR_OFF))
		return 0;
	return 1;
}

void acsChangeStrState(int id)
{
	aciScreenInputField* p = (aciScreenInputField*)acsScrD -> GetObject(id);
	if(p) p -> change_state();
}

void acsSetStrState(int id,int state)
{
	aciScreenInputField* p = (aciScreenInputField*)acsScrD -> GetObject(id);
	if(p) p -> set_state(state);
}

int acsGetStrState(int id)
{
	aciScreenInputField* p = (aciScreenInputField*)acsScrD -> GetObject(id);
	if(p) return p -> get_state();
	return 0;
}



