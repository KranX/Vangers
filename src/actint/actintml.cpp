/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"

#include "../iscreen/hfont.h"
#include "../iscreen/iscreen.h"
#include "../iscreen/iworld.h"
#include "../iscreen/iml.h"

#include "item_api.h"
#include "actint.h"
#include "a_consts.h"
#include "../iscreen/ikeys.h"

#include "../uvs/diagen.h"

#include "mlconsts.h"
#include "mlstruct.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */
/* ----------------------------- EXTERN SECTION ----------------------------- */

extern int iMouseX;
extern int iMouseY;

extern int iTheEND;
extern int aciEndSpeech;
extern int aciBreathFlag;

extern int ActiveTerrain[];

extern actIntDispatcher* aScrDisp;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void aciNextPhrase(void);
void aciClearQuestMenu(void);
void aciExitShop(void);

void iShopForcedExit(void);
void iLockShop(void);
void iUnlockShop(void);
void iLockExit(void);
void iUnlockExit(void);
void aciAcceptRubox(void);
void aciAddRubox(void);
void aciExpropriation(void);
void aciChange2Raffa(void);
void aciWaitNextPeriod(void);
void aciOfferWeezyk(void);
void aciEscaveKickOut(void);

void aciStealItem(int type);

void aciML_Close(int id,int phase);
void aciML_Invert(int id,int phase);
aciML_Event* aciML_GetEvent(int key,int mode);
void aciML_KeyTrap(int key,int mode);
void aciML_StartEvent(aciML_Event* ev);

void aci_SendEvent(int cd,int dt = 0);
void aciOfferEleech(void);

int isEventActive(int key,int mode);
int isChannelActive(int ch);

void StartupIML(void);
void InitIML(void);

void put_map(int x,int y,int sx,int sy);

void get_fon(int x,int y,int sx,int sy,unsigned char* buf);
void put_fon(int x,int y,int sx,int sy,unsigned char* buf);

void get_terr_fon(int x,int y,int sx,int sy,unsigned char* buf);
void put_terr_fon(int x,int y,int sx,int sy,unsigned char* buf);

void iSoundQuant(int value = 0);

/* --------------------------- DEFINITION SECTION --------------------------- */

#define AML_MIN_SHUTDOWN_TIMER		10
#define AML_SHUTDOWN_DELTA		3

aciML_Dispatcher* aciML_D;

int aciML_ToolzerON = 0;
int aciML_ToolzerRadius = 0;

int aciML_ShutdownCount = 0;

#ifdef AML_DEBUG
int aciML_logFile = 0;
char* aciML_logFileNameW = "iml_wr.log";
char* aciML_logFileNameR = "iml_pl.log";
char* aciML_logFileName = NULL;
#endif

void aciML_KeyTrap(int key,int mode)
{
	aciML_D -> key_trap(key,mode);
}

aciML_Event* aciML_GetEvent(int key,int mode)
{
	return aciML_D -> getEvent(key,mode);
}

void aciML_Pause(void)
{
	PauseIMLall();
	aciML_D -> flags |= AML_PAUSE;
}

void aciML_Resume(void)
{
	ResumeIMLall();
	aciML_D -> flags &= ~AML_PAUSE;
}

int isEventActive(int key,int mode)
{
	return aciML_D -> isEventActive(key,mode);
}

int isChannelActive(int ch)
{
	return aciML_D -> check_channel_events(ch);
}

void dg_SendEvent(int code)
{
	switch(code){
		case AML_GET_QUESTION:
			aci_SendEvent(EV_SHOW_QUESTS);
			break;
		case AML_TAKE_ELEECH:
			aciOfferEleech();
			break;
		case AML_TAKE_WEEZYK:
			aciOfferWeezyk();
			break;
		case AML_WAIT_NEXT_PERIOD:
			aciWaitNextPeriod();
			break;
		case AML_CLEAR_QUESTION:
			aciClearQuestMenu();
			break;
		case AML_ABORT_ESCAVE_SESSION:
			iShopForcedExit();
			break;
		case AML_BLOCK_SHOP:
			iLockShop();
			break;
		case AML_UNBLOCK_SHOP:
			iUnlockShop();
			break;
		case AML_BLOCK_EXIT:
			iLockExit();
			break;
		case AML_UNBLOCK_EXIT:
			iUnlockExit();
			break;
		case AML_LAMPASSO_THEEND:
			iShopForcedExit();
			GameOverID = GAME_OVER_LAMPASSO;
			iTheEND = 1;
			break;
		case AML_KICK_OUT_FROM_ESCAVE:
			aciEscaveKickOut();
			break;
		case AML_ACCEPT_RUBBOX:
			aciAcceptRubox();
			break;
		case AML_ADD_RUBBOX:
			aciAddRubox();
			break;
		case AML_EXPROPRIATION:
			aciExpropriation();
			break;
		case AML_CHANGE_TO_RAFFA:
			aciChange2Raffa();
			break;
	}
}

void aciML_Close(int id,int phase)
{
	int cur_phase = GetPhaseIML(id);
	if(cur_phase != phase)
		HandlerIML(AML_GO_PHASE,id,phase);
}

void aciML_Invert(int id,int phase)
{
	int cur_phase = GetPhaseIML(id);
	if(cur_phase != phase){
		HandlerIML(AML_GO_PHASE,id,phase);
	}
	else {
		HandlerIML(AML_GO_PHASE,id,0);
	}
}

int aciML_DropEvents(int channel,int pr)
{
	return aciML_D -> drop_channel_events(channel,pr);
}

void aciML_SendEvent(int cd,int d0,int d1)
{
	aciML_D -> send_event(cd,d0,d1);
}

void SetFlushIML(int x,int y,int sx,int sy)
{
	aciML_D -> add_flush(x,y,sx,sy);
}

aciML_Event::aciML_Event(void)
{
	startupType = AML_KEY;
	data = 0;

	rndValue = 0;

	cur_timer = 0;
	active_time = 1;
	flags = 0;

	ChannelID = 0;
	priority = 0;

	keys = new aKeyObj;
	commands = new iList;
}

aciML_Event::~aciML_Event(void)
{
	aciML_EventCommand* cm,*cm1;

	delete keys;
	cm = (aciML_EventCommand*)commands -> last;
	while(cm){
		cm1 = (aciML_EventCommand*)cm -> prev;
		delete cm;
		cm = cm1;
	}
	delete commands;
}

aciML_Data::aciML_Data(void)
{
	ID = 0;
	ChannelID = 0;
	flags = 0;
	name = NULL;
	event_list = new iList;
}

aciML_Data::~aciML_Data(void)
{
	aciML_Event* ev,*ev1;

	if(name) delete name;

	ev = (aciML_Event*)event_list -> last;
	while(ev){
		ev1 = (aciML_Event*)ev -> prev;
		delete ev;
		ev = ev1;
	}
	delete event_list;
}

int aciML_Data::check_xy(int x,int y)
{
	if(!(flags & AML_FRAME_CHECK))
		return CheckIML(ID,x,y);
	else
		return CheckIML(ID,x,y,1);
}

aciML_EventSeqLine::aciML_EventSeqLine(void)
{
	int i;
	table = new aciML_Event*[AML_MAX_SEQ_EVENT];
	for(i = 0; i < AML_MAX_SEQ_EVENT; i ++){
		table[i] = NULL;
	}
	flags = 0;
	clear();
}

aciML_EventSeqLine::~aciML_EventSeqLine(void)
{
	delete table;
}

aciML_EventLine::aciML_EventLine(void)
{
	int i;
	table = new aciML_EventCommand*[AML_MAX_EVENT];
	mem_heap = new char[AML_MAX_EVENT * sizeof(aciML_EventCommand)];
	for(i = 0; i < AML_MAX_EVENT; i ++){
		table[i] = (aciML_EventCommand*)(mem_heap + i * sizeof(aciML_EventCommand));
		table[i] -> init();
	}
	flags = 0;
	clear();
}

aciML_EventLine::~aciML_EventLine(void)
{
	delete mem_heap;
	delete table;
	clear();
}

void aciML_EventSeqLine::put(aciML_Event* p)
{
	if(size < AML_MAX_SEQ_EVENT){
		table[last_index] = p;
		p -> flags &= ~AML_FINISHED;

		last_index ++;
		if(last_index >= AML_MAX_SEQ_EVENT)
			last_index = 0;
		size ++;
	}
	else
		ErrH.Abort("iML Event buffer overflow...");
}

void aciML_EventLine::put(int cd,int dt0,int dt1)
{
	if(size < AML_MAX_EVENT){
		table[last_index] -> code = cd;
		table[last_index] -> data0 = dt0;
		table[last_index] -> data1 = dt1;

		last_index ++;
		if(last_index >= AML_MAX_EVENT)
			last_index = 0;
		size ++;
	}
	else
		ErrH.Abort("iML Event buffer overflow...");
}

aciML_Event* aciML_EventSeqLine::get(void)
{
	aciML_Event* p;
	if(size){
		p = table[first_index];

		first_index ++;
		if(first_index >= AML_MAX_SEQ_EVENT)
			first_index = 0;

		size --;
		return p;
	}
	return NULL;
}

aciML_EventCommand* aciML_EventLine::get(void)
{
	aciML_EventCommand* p;
	if(size){
		p = table[first_index];

		first_index ++;
		if(first_index >= AML_MAX_EVENT)
			first_index = 0;

		size --;
		return p;
	}
	return NULL;
}

void aciML_EventSeqLine::clear(void)
{
	size = 0;
	first_index = 0;
	last_index = 0;
}

void aciML_EventLine::clear(void)
{
	size = 0;
	first_index = 0;
	last_index = 0;
}

aciML_Dispatcher::aciML_Dispatcher(void)
{
	int i;
	flags = 0;

	ml_data_sets = new iList;

	SeqList = new aciML_EventSeqLine*[AML_MAX_SEQ_LINE_ID];
	for(i = 0; i < AML_MAX_SEQ_LINE_ID; i ++)
		SeqList[i] = new aciML_EventSeqLine;

	timer = 0;
	startup_timer = shutdown_timer = 0;

	event_line = new aciML_EventLine;
	flush_line = new aciML_FlushLine;
}

aciML_Dispatcher::~aciML_Dispatcher(void)
{
	int i;
	aciML_DataSet* p,*p1;

	for(i = 0; i < AML_MAX_SEQ_LINE_ID; i ++)
		delete SeqList[i];
	delete[] SeqList;

	p = (aciML_DataSet*)ml_data_sets -> last;
	while(p){
		p1 = (aciML_DataSet*)p -> prev;
		delete p;
		p = p1;
	}
	delete ml_data_sets;

	delete event_line;
	delete flush_line;
}

aciML_DataSet::aciML_DataSet(void)
{
	int i;
	ml_data = new iList;
	items = new iList;
	seq_list = new iList;

	SpeechChannel = 0;
	for(i = 0; i < AML_MAX_SPEECH_MODE; i ++)
		SpeechPriority[i] = 0;
}

aciML_DataSet::~aciML_DataSet(void)
{
	aciML_Data* p,*p1;
	aciML_ItemData* it,*it1;
	aciML_EventSeq* seq,*seq1;

	seq = (aciML_EventSeq*)seq_list -> last;
	while(seq){
		seq1 = (aciML_EventSeq*)seq -> prev;
		delete seq;
		seq = seq1;
	}
	delete seq_list;

	p = (aciML_Data*)ml_data -> last;
	while(p){
		p1 = (aciML_Data*)p -> prev;
		delete p;
		p = p1;
	}
	delete ml_data;

	it = (aciML_ItemData*)items -> last;
	while(it){
		it1 = (aciML_ItemData*)it -> prev;
		delete it;
		it = it1;
	}
	delete items;
}

void aciML_Dispatcher::event_quant(void)
{
	aciML_EventCommand* ev;
	while(event_line -> size){
		ev = event_line -> get();

#ifdef AML_DEBUG
		logFile < "\r\n" <= XRec.frameCount < "\t" <= ev -> code < "\t" <= ev -> data0 < "\t" <= ev -> data1;
#endif
#ifndef _ACI_NO_SHOP_ANIMATION_
		switch(ev -> code){
			case AML_NEXT_PHRASE:
				aciNextPhrase();
				break;
			case AML_EXIT:
				flags |= AML_PAUSE;
				aciExitShop();
				break;
			case AML_DISABLE_ML:
				curDataSet -> disable_ml(ev -> data0);
				break;
			case AML_ENABLE_ML:
				curDataSet -> enable_ml(ev -> data0);
				break;
			case AML_CLOSE_SPEECH:
				aciEndSpeech = 1;
				break;
			case AML_BREATH_ON:
				aciBreathFlag = 1;
				break;
			case AML_BREATH_OFF:
				aciBreathFlag = 0;
				break;
			case AML_LOCK:
				LockIML(ev -> data0);
				break;
			case AML_UNLOCK:
				UnlockIML(ev -> data0);
				break;
			case AML_CLOSE_ML:
				aciML_Close(ev -> data0,ev -> data1);
				break;
			case AML_INVERT_PHASE:
				aciML_Invert(ev -> data0,ev -> data1);
				break;
			case AML_SHUTDOWN_EV:
				shutdown();
				break;
			case AML_PAUSE_ALL:
				PauseIMLall();
				flags |= AML_PAUSE;
				break;
			case AML_RESUME_ALL:
				ResumeIMLall();
				flags &= ~AML_PAUSE;
				break;
			case AML_KEY_TRAP:
				key_trap(ev -> data1,ev -> data0);
				break;
			case AML_SEND2ACTINT:
				aci_SendEvent(ev -> data0 + ACI_MAX_EVENT,ev -> data1);
				break;
			case AML_CLEAR_QUEUE:
				CleanQueueIML(ev -> data0);
				break;
			case AML_ADD_IN_CH_QUEUE:
				if(!isInQueueIML(ev -> data0,ev -> data1) && !isActiveIML(ev -> data1)){
					HandlerIML(AML_PUT_IN_CH_QUEUE,ev -> data0,ev -> data1);
				}
				break;
			case AML_START_SOUND:
				iSoundQuant(ev -> data0);
				break;
			case AML_PUT_SEQUENCE:
				SeqList[ev -> data0] -> putSeq(curDataSet -> getSeq(ev -> data1));
				break;
			default:
				HandlerIML(ev -> code,ev -> data0,ev -> data1);
				break;
		}
#endif
	}
}

void aciML_Dispatcher::register_names(void)
{
	aciML_Data* p = (aciML_Data*)ml_data -> last;
	while(p){
		RegisterIML(p -> name,p -> ID,p -> ChannelID);
		p = (aciML_Data*)p -> prev;
	}
}

void aciML_Dispatcher::startup(void)
{
	int i;

	flags |= AML_STARTUP_MODE;
	aciML_ToolzerON = 0;
	timer = 0;

	if(!ml_data -> Size)
		aci_SendEvent(EV_NEXT_PHRASE);

	for(i = 0; i < AML_MAX_SEQ_LINE_ID; i ++)
		SeqList[i] -> clear();

	flags &= ~AML_PAUSE;

#ifdef AML_DEBUG
	if(XRec.flags & XRC_PLAY_MODE)
		aciML_logFileName = aciML_logFileNameR;
	else
		aciML_logFileName = aciML_logFileNameW;

	if(aciML_logFile){
		logFile.open(aciML_logFileName,XS_OUT | XS_APPEND | XS_NOREPLACE);
	}
	else {
		logFile.open(aciML_logFileName,XS_OUT);
		aciML_logFile = 1;
	}
#endif
}

void aciML_Dispatcher::shutdown(void)
{
	aciML_ShutdownCount = 0;
	flags |= AML_SHUTDOWN_MODE;
	flags &= ~AML_STARTUP_MODE;
	timer = 0;
}

void aciML_Dispatcher::send_event(int cd,int d0,int d1)
{
	event_line -> put(cd,d0,d1);
}

void aciML_Dispatcher::add_flush(int x,int y,int sx,int sy)
{
	flush_line -> put(x,y,sx,sy);
}

void aciML_Dispatcher::key_trap(int k,int mode)
{
	aciML_Data* p = (aciML_Data*)ml_data -> last;

	if(mode == -1){
		mode = AML_KEY;

		if(flags & AML_STARTUP_MODE){
			mode = AML_STARTUP;
		}
		else {
			if(flags & AML_SHUTDOWN_MODE)
				mode = AML_SHUTDOWN;
		}
	}
	while(p){
		p -> key_trap(k,mode);
		p = (aciML_Data*)p -> prev;
	}
	if(!iScreenOffs && mode == AML_KEY && (k == iMOUSE_LEFT_PRESS_CODE || k == iMOUSE_RIGHT_PRESS_CODE)){
		curDataSet -> items_keytrap(iMouseX,iMouseY);
	}
}

int aciML_Data::key_trap(int key,int mode)
{
	int ch_id = 0,locked = isLockedIML(ID),ev_flag = 1;
	aciML_Event* ev;
	if(mode == AML_KEY && (key == -1 || key == -2)){
		if(!check_xy(iMouseX + iScreenOffs,iMouseY))
			return ch_id;
	}
	switch(mode){
		case AML_KEY:
			ev = (aciML_Event*)event_list -> last;
			while(ev){
				ev_flag = 1;
				if(ev -> flags & AML_IF_LOCKED && !locked) ev_flag = 0;
				if(ev -> flags & AML_IF_NOT_LOCKED && locked) ev_flag = 0;
				if(ev_flag){
					if(ev -> startupType == mode && ev -> check_key(key)){
						if(ev -> flags & AML_ACTIVE_EVENT && ev -> flags & AML_NO_DEACTIVATE) return -1;
						if(ev -> ChannelID){
							ch_id = ev -> ChannelID;
							if(!aciML_DropEvents(ch_id,ev -> priority)) return -1;
						}
						ev -> start();
					}
				}
				ev = (aciML_Event*)ev -> prev;
			}
			break;
		case AML_STARTUP:
		case AML_SHUTDOWN:
			ev = (aciML_Event*)event_list -> last;
			while(ev){
				ev_flag = 1;
				if(ev -> flags & AML_IF_LOCKED && !locked) ev_flag = 0;
				if(ev -> flags & AML_IF_NOT_LOCKED && locked) ev_flag = 0;
				if(ev_flag){
					if(ev -> startupType == mode && ev -> data == key){
						if(ev -> ChannelID){
							ch_id = ev -> ChannelID;
							if(!aciML_DropEvents(ch_id,ev -> priority)) return -1;
						}
						ev -> start();
					}
				}
				ev = (aciML_Event*)ev -> prev;
			}
			break;
	}
	return ch_id;
}

void aciML_Event::start(void)
{
	aciML_EventCommand* cm;
	if(flags & AML_ACTIVE_EVENT && flags & AML_NO_DEACTIVATE) return;
	cm = (aciML_EventCommand*)commands -> last;
	while(cm){
		cm -> flags &= ~AML_COMMAND_STARTED;
		cm = (aciML_EventCommand*)cm -> prev;
	}

	flags &= ~AML_FINISHED;
	flags |= AML_ACTIVE_EVENT;
	cur_timer = 0;
}

int aciML_Event::check_key(int key)
{
	return keys -> check(key);
}

aciML_FlushLine::aciML_FlushLine(void)
{
	int i;
	table = new aciML_FlushData*[AML_MAX_FLUSH];
	mem_heap = new char[AML_MAX_FLUSH * sizeof(aciML_FlushData)];
	for(i = 0; i < AML_MAX_FLUSH; i ++){
		table[i] = (aciML_FlushData*)(mem_heap + i * sizeof(aciML_FlushData));
	}
	clear();
}

aciML_FlushLine::~aciML_FlushLine(void)
{
	delete mem_heap;
	delete table;
}

void aciML_FlushLine::put(int x,int y,int sx,int sy)
{
	if(size < AML_MAX_EVENT){
		table[last_index] -> PosX = x;
		table[last_index] -> PosY = y;
		table[last_index] -> SizeX = sx;
		table[last_index] -> SizeY = sy;

		last_index ++;
		if(last_index >= AML_MAX_FLUSH)
			last_index = 0;
		size ++;
	}
	else
		ErrH.Abort("iML Flush buffer overflow...");
}

aciML_FlushData* aciML_FlushLine::get(void)
{
	aciML_FlushData* p;
	if(size){
		p = table[first_index];

		first_index ++;
		if(first_index >= AML_MAX_FLUSH)
			first_index = 0;

		size --;
		return p;
	}
	return NULL;
}

void aciML_FlushLine::clear(void)
{
	size = 0;
	first_index = 0;
	last_index = 0;
}

void aciML_Event::add_command(aciML_EventCommand* p)
{
	commands -> connect((iListElement*)p);
	p -> owner = (iListElement*)this;
}

void aciML_Data::add_event(aciML_Event* p)
{
	event_list -> connect((iListElement*)p);
}

void aciML_Dispatcher::add_data_set(aciML_DataSet* p)
{
	ml_data_sets -> connect((iListElement*)p);
}

void aciML_Dispatcher::flush(int mode)
{
	aciML_FlushData* p;
	while(flush_line -> size){
		p = flush_line -> get();
		put_map(p -> PosX,p -> PosY,p -> SizeX,p -> SizeY);
//		  if(mode) XGR_Flush(p -> PosX,p -> PosY,p -> SizeX,p -> SizeY);
	}
	//if(mode)
	//	XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
}

void aciML_Dispatcher::quant(void)
{
	int i,log;

	if(flags & AML_PAUSE) return;

	if(flags & AML_STARTUP_MODE){
		key_trap(timer,AML_STARTUP);
	}
	else {
		if(flags & AML_SHUTDOWN_MODE){
			key_trap(timer,AML_SHUTDOWN);
		}
		else {
			if(!iScreenOffs && aciML_ToolzerON){
				ToolzerQuant(iMouseX,iMouseY);
				aScrDisp -> aml_check_redraw(iMouseX,iMouseY);
			}
		}
	}

	for(i = 0; i < AML_MAX_SEQ_LINE_ID; i ++)
		SeqList[i] -> firstQuant();

	rnd_event_quant();
	active_event_quant();
	event_quant();
	log = QuantIML();
	inc_events_timer();

	for(i = 0; i < AML_MAX_SEQ_LINE_ID; i ++)
		SeqList[i] -> secondQuant();

	timer ++;
	if(flags & AML_STARTUP_MODE){
		if(timer >= startup_timer){
			flags ^= AML_STARTUP_MODE;
			timer = 0;
		}
	}
	else {
		if(flags & AML_SHUTDOWN_MODE){
			if(timer >= shutdown_timer || (!log && timer >= AML_MIN_SHUTDOWN_TIMER)){
				aciML_ShutdownCount ++;

			}
			if(aciML_ShutdownCount >= AML_SHUTDOWN_DELTA){
				flags ^= AML_SHUTDOWN_MODE;
				flags |= AML_END_SHUTDOWN | AML_PAUSE;
				timer = 0;
				clear_events();
#ifdef AML_DEBUG
				logFile.close();
#endif
			}
		}
	}
}

void aciML_DataSet::init_items(void)
{
	aciML_ItemData* p = (aciML_ItemData*)items -> last;
	while(p){
		p -> init();
		p = (aciML_ItemData*)p -> prev;
	}
}

void aciML_DataSet::put_item(int id)
{
	int i,flag = 0,num;
	aciML_ItemData* p = (aciML_ItemData*)items -> last;
	while(p){
		if(p -> ItemID == id) flag = 1;
		p = (aciML_ItemData*)p -> prev;
	}
	if(!flag) return;

	flag = 0;

	while(!flag){
		p = (aciML_ItemData*)items -> first;
		num = rand()%items -> Size;
		for(i = 0; i < num; i ++)
			p = (aciML_ItemData*)p -> next;
		if(p -> ItemID == id){
			p -> flags |= AML_ACTIVE_ITEM;
			flag = 1;
		}
	}
}

void aciML_DataSet::finit_items(void)
{
	aciML_ItemData* p = (aciML_ItemData*)items -> last;
	while(p){
		p -> finit();
		p = (aciML_ItemData*)p -> prev;
	}
}

void aciML_Dispatcher::init(void)
{
	StartupIML();
	register_names();
	InitIML();

	clear_events();

	curDataSet -> init_items();
}

void aciML_Dispatcher::finit(void)
{
	ReleaseIML();
	curDataSet -> finit_items();
}

int aciML_Dispatcher::drop_channel_events(int channel,int pr)
{
	aciML_Data* p = (aciML_Data*)ml_data -> last;
	while(p){
		if(!p -> drop_events(channel,pr)) return 0;
		p = (aciML_Data*)p -> prev;
	}
	return 1;
}

int aciML_Data::drop_events(int channel,int pr)
{
	aciML_Event* ev = (aciML_Event*)event_list -> last;
	while(ev){
		if(ev -> ChannelID == channel){
			if(ev -> flags & AML_ACTIVE_EVENT){
				if(ev -> priority > pr) return 0;
				ev -> stop();
			}
			if(ev -> flags & AML_WAITING){
				ev -> cur_timer = ev -> data;
			}
		}
		ev = (aciML_Event*)ev -> prev;
	}
	return 1;
}

void aciML_Dispatcher::active_event_quant(void)
{
	aciML_Data* p = (aciML_Data*)ml_data -> last;
	while(p){
		p -> active_event_quant();
		p = (aciML_Data*)p -> prev;
	}
}

void aciML_Dispatcher::rnd_event_quant(void)
{
	aciML_Data* p = (aciML_Data*)ml_data -> last;
	while(p){
		p -> rnd_event_quant();
		p = (aciML_Data*)p -> prev;
	}
}

int aciML_Dispatcher::check_channel_events(int ch)
{
	int ret = 0;
	aciML_Data* p = (aciML_Data*)ml_data -> last;
	while(p){
		ret = p -> check_channel_events(ch);
		if(ret) return ret;
		p = (aciML_Data*)p -> prev;
	}
	return 0;
}

void aciML_Dispatcher::clear_events(void)
{
	aciML_Data* p = (aciML_Data*)ml_data -> last;
	while(p){
		p -> flags &= ~AML_DISABLED_ML;
		p -> clear_events();
		p = (aciML_Data*)p -> prev;
	}
	event_line -> clear();
}

void aciML_Dispatcher::inc_events_timer(void)
{
	aciML_Data* p = (aciML_Data*)ml_data -> last;
	while(p){
		p -> inc_events_timer();
		p = (aciML_Data*)p -> prev;
	}
}

void aciML_Data::active_event_quant(void)
{
	aciML_Event* ev = (aciML_Event*)event_list -> last;
	while(ev){
		if(ev -> flags & AML_ACTIVE_EVENT){
			ev -> quant();
		}
		ev = (aciML_Event*)ev -> prev;
	}
}

void aciML_Data::rnd_event_quant(void)
{
	int code;
	aciML_Event* ev = (aciML_Event*)event_list -> last;
	if(flags & AML_DISABLED_ML) return;
	while(ev){
		if(ev -> rndValue && !(ev -> flags & AML_ACTIVE_EVENT)){
			if(!(ev -> flags & AML_WAITING)){
				ev -> flags |= AML_WAITING;
				ev -> cur_timer = ev -> data;
			}
			else {
				ev -> cur_timer --;
				if(ev -> cur_timer <= 0){
					if(!rand()%ev -> rndValue){
						code = ((iScanCode*)ev -> keys -> codes -> last) -> code;
						aciML_KeyTrap(code,ev -> startupType);
						if(ev -> flags & AML_ACTIVE_EVENT)
							ev -> flags &= ~AML_WAITING;
						else
							ev -> cur_timer = ev -> data;
					}
					else
						ev -> cur_timer = ev -> data;
				}
			}
		}
		ev = (aciML_Event*)ev -> prev;
	}
}

int aciML_Data::check_channel_events(int ch)
{
	aciML_Event* ev = (aciML_Event*)event_list -> last;
	while(ev){
		if(ev -> flags & AML_ACTIVE_EVENT && ev -> ChannelID == ch)
			return 1;

		ev = (aciML_Event*)ev -> prev;
	}
	return 0;
}

void aciML_Data::clear_events(void)
{
	aciML_Event* ev = (aciML_Event*)event_list -> last;
	while(ev){
		ev -> flags &= ~AML_ACTIVE_EVENT;
		ev -> flags &= ~AML_FINISHED;
		ev -> flags &= ~AML_WAITING;

		ev = (aciML_Event*)ev -> prev;
	}
}

void aciML_Data::inc_events_timer(void)
{
	aciML_Event* ev = (aciML_Event*)event_list -> last;
	while(ev){
		if(ev -> flags & AML_ACTIVE_EVENT){
			ev -> inc_timer();
		}
		ev = (aciML_Event*)ev -> prev;
	}
}

void aciML_Event::quant(void)
{
	aciML_EventCommand* cm = (aciML_EventCommand*)commands -> last;
	while(cm){
		if(cm -> start_timer == cur_timer && !(cm -> flags & AML_COMMAND_STARTED)){
			switch(cm -> code){
				case AML_IS_LOCKED:
					if(isLockedIML(cm -> data0)){
						stop();
						return;
					}
					break;
				case AML_IS_NOT_LOCKED:
					if(!isLockedIML(cm -> data0)){
						stop();
						return;
					}
					break;
				case AML_EVENT_IS_ACTIVE:
					if(isEventActive(cm -> data1,cm -> data0)){
						stop();
						return;
					}
					break;
				case AML_IS_ACTIVE:
					if(isActiveIML(cm -> data0)){
						stop();
						return;
					}
					break;
				default:
					aciML_SendEvent(cm -> code,cm -> data0,cm -> data1);
					break;
			}
			cm -> flags |= AML_COMMAND_STARTED;
		}
		cm = (aciML_EventCommand*)cm -> prev;
	}
}

void aciML_Event::inc_timer(void)
{
	if(!(flags & AML_SEQUENCE_EVENT) || check_seq())
		cur_timer ++;
	if(cur_timer > active_time)
		stop();
}

int aciML_Event::check_seq(void)
{
	aciML_EventCommand* cm = (aciML_EventCommand*)commands -> last;
	while(cm){
		if(cm -> start_timer == cur_timer){
			if(!(cm -> flags & AML_COMMAND_STARTED)) return 0;
			switch(cm -> code){
				case AML_WAIT_CHANNEL:
					if(isChannelActive(cm -> data0)) return 0;
					break;
				case AML_GO_INFINITE:
					if(!isActiveIML(cm -> data0)) return 0;
					break;
				case AML_PUT_IN_QUEUE:
					if(!isActiveIML(cm -> data1)) return 0;
					break;
				case AML_PUT_IN_CH_QUEUE:
					if(!isLockedIML(cm -> data1) && (isInQueueIML(cm -> data0,cm -> data1))) return 0;
					break;
				case AML_ADD_IN_CH_QUEUE:
					if(!isLockedIML(cm -> data1) && (isInQueueIML(cm -> data0,cm -> data1) || !isActiveIML(cm -> data1))) return 0;
					break;
				case AML_GO_PHASE:
				case AML_CLOSE_ML:
				case AML_INVERT_PHASE:
					if(isActiveIML(cm -> data0)) return 0;
					break;
			}
		}
		cm = (aciML_EventCommand*)cm -> prev;
	}
	return 1;
}

void aciML_DataSet::add_data(aciML_Data* p)
{
	ml_data -> connect((iListElement*)p);
}

void aciML_DataSet::add_seq(aciML_EventSeq* p)
{
	seq_list -> connect((iListElement*)p);
}

void aciML_DataSet::add_item(aciML_ItemData* p)
{
	items -> connect((iListElement*)p);
}

void aciML_DataSet::disable_ml(int id)
{
	aciML_Data* p = (aciML_Data*)ml_data -> last;
	while(p){
		if(p -> ID == id){
			p -> flags |= AML_DISABLED_ML;
			return;
		}
		p = (aciML_Data*)p -> prev;
	}
}

void aciML_DataSet::enable_ml(int id)
{
	aciML_Data* p = (aciML_Data*)ml_data -> last;
	while(p){
		if(p -> ID == id){
			p -> flags &= ~AML_DISABLED_ML;
			return;
		}
		p = (aciML_Data*)p -> prev;
	}
}

void aciML_Dispatcher::init_data_set(int id)
{
	aciML_DataSet* p = (aciML_DataSet*)ml_data_sets -> last;
	while(p){
		if(p -> ID == id){
			ml_data = p -> ml_data;
			curDataSet = p;
			return;
		}
		p = (aciML_DataSet*)p -> prev;
	}
}

int aciML_Dispatcher::isEventActive(int key,int mode)
{
	aciML_Data* p = (aciML_Data*)ml_data -> last;
	while(p){
		if(p -> isEventActive(key,mode)) return 1;
		p = (aciML_Data*)p -> prev;
	}
	return 0;
}

int aciML_Data::isEventActive(int key,int mode)
{
	aciML_Event* ev = (aciML_Event*)event_list -> last;
	while(ev){
		if(ev -> flags & AML_ACTIVE_EVENT){
			if(mode == AML_STARTUP_MODE || mode == AML_SHUTDOWN_MODE){
				if(key == ev -> data) return 1;
			}
			else {
				if(ev -> check_key(key)) return 1;
			}
		}
		ev = (aciML_Event*)ev -> prev;
	}
	return 0;
}

aciML_ItemData::aciML_ItemData(void)
{
	ItemID = -1;
	flags = 0;

	PosX = PosY = 0;
	SizeX = SizeY = 0;

	NullLevel = 128;
	EmptyTerrain = -1;

	frameName = NULL;
	frameData = NULL;
	empty_frameData = NULL;
}

aciML_ItemData::~aciML_ItemData(void)
{
	if(frameName) delete frameName;
	if(frameData) delete frameData;
	if(empty_frameData) delete empty_frameData;
}

int aciML_ItemData::check_xy(int x,int y)
{
	if(x >= PosX && x < PosX + SizeX && y >= PosY && y < PosY + SizeY)
		return 1;

	return 0;
}

void aciML_ItemData::init(void)
{
	short sx,sy,sz,ox,oy;
	XStream fh;

	if(!frameName) return;
	fh.open(frameName,XS_IN);

	fh > sx > sy > sz > ox > oy;
	if(sz != 2) ErrH.Abort("Bad aciML_ItemData::frameData...");

	flags &= ~AML_ACTIVE_ITEM;

	SizeX = sx;
	SizeY = sy;

	PosX = ox;
	PosY = oy;

	if(frameData) delete frameData;
	frameData = new unsigned char[SizeX * SizeY * sz];
	fh.read(frameData,SizeX * SizeY * sz);
	fh.close();

	build_empty_frame();
}

void aciML_ItemData::finit(void)
{
	if(frameData){
		delete[] frameData;
		frameData = NULL;
	}
	if(empty_frameData){
		delete[] empty_frameData;
		empty_frameData = NULL;
	}
}

void aciML_ItemData::put_frame(int mode)
{
	if(!frameData) return;
	switch(mode){
		case AML_PUT_FILLED:
			put_fon(PosX,PosY,SizeX,SizeY,frameData);
			put_terr_fon(PosX,PosY,SizeX,SizeY,frameData + SizeX * SizeY);
			break;
		case AML_PUT_EMPTY:
			put_fon(PosX,PosY,SizeX,SizeY,empty_frameData);
			put_terr_fon(PosX,PosY,SizeX,SizeY,empty_frameData + SizeX * SizeY);
			break;
		case AML_PUT_LOW_LEVEL:
			put2low_level();
			break;
	}
	iregRender(PosX,PosY,PosX + SizeX,PosY + SizeY);
	put_map(PosX,PosY,SizeX,SizeY);
	SetFlushIML(PosX,PosY,SizeX,SizeY);
}

void aciML_ItemData::build_empty_frame(void)
{
	int x,y,dx;
	int data,attr,terr,ch_terr = 7 << TERRAIN_OFFSET;
	unsigned char* height_data,*height_data1,*attr_data,*attr_data1;

	if(empty_frameData) delete empty_frameData;
	empty_frameData = new unsigned char[SizeX * SizeY * 2];
	memcpy(empty_frameData,frameData,SizeX * SizeY * 2);

	height_data = frameData;
	attr_data = frameData + SizeX * SizeY;

	height_data1 = empty_frameData;
	attr_data1 = empty_frameData + SizeX * SizeY;

	for(y = 0; y < SizeY; y ++){
		for(x = 0; x < SizeX; x ++){
			data = *height_data;
			attr = *attr_data;
			if(IS_DOUBLE(attr)){
				terr = GET_TERRAIN(attr);
				if(!ActiveTerrain[terr]){
					dx = NullLevel - (data - NullLevel)/2;
					if(dx < 0) dx = 0;
					*height_data1 = dx;
					if(*height_data == *height_data1) *height_data1 = dx - 1;
					SET_TERRAIN(*attr_data1,ch_terr);
				}
			}
			height_data ++;
			attr_data ++;

			height_data1 ++;
			attr_data1 ++;
		}
	}
/*
	XStream fh("ACTINT\\TEMP\\itm.bml",XS_OUT);
	fh < (short)SizeX < (short)SizeY < (short)4;
	fh.write(frameData,SizeX * SizeY);
	fh.write(empty_frameData,SizeX * SizeY);
	fh.write(frameData + SizeX * SizeY,SizeX * SizeY);
	fh.write(empty_frameData + SizeX * SizeY,SizeX * SizeY);
	fh.close();
*/
}

void aciML_Dispatcher::redraw_items(void)
{
	curDataSet -> redraw_items();
}

void aciML_DataSet::redraw_items(void)
{
	aciML_ItemData* p = (aciML_ItemData*)items -> last;
	while(p){
		if(p -> flags & AML_ACTIVE_ITEM)
			p -> put_frame(AML_PUT_FILLED);
		else
			p -> put_frame(AML_PUT_EMPTY);

		p = (aciML_ItemData*)p -> prev;
	}
}

void aciML_DataSet::items_keytrap(int x,int y)
{
	aciML_ItemData* p = (aciML_ItemData*)items -> last;
	while(p){
		if(p -> flags & AML_ACTIVE_ITEM && p -> check_xy(x,y) && p -> check_double_level()){
			p -> put_frame(AML_PUT_LOW_LEVEL);
			aciStealItem(p -> ItemID);
			return;
		}
		p = (aciML_ItemData*)p -> prev;
	}
}

//TODO need fast malloc
int aciML_ItemData::check_double_level(void)
{
	int i,num = 0,sz1 = 0,sz = SizeX * SizeY;
	unsigned char* ptr;

	ptr = new unsigned char[sz];

	get_terr_fon(PosX,PosY,SizeX,SizeY,ptr);
	for(i = 0; i < sz; i ++){
		if(frameData[i] != empty_frameData[i]){
			if(IS_DOUBLE(ptr[i])) num ++;
			sz1 ++;
		}
	}
	delete[] ptr;

	if(num < sz1/3) return 1;
	return 0;
}

//TODO need fast malloc
void aciML_ItemData::put2low_level(void)
{
	int i,sz = SizeX * SizeY,terr;
	unsigned char* ptr;

	ptr = new unsigned char[sz * 2];

	get_fon(PosX,PosY,SizeX,SizeY,ptr);
	get_terr_fon(PosX,PosY,SizeX,SizeY,ptr + sz);
	for(i = 0; i < sz; i += 2){
		if(frameData[i] != empty_frameData[i]){
			terr = GET_TERRAIN_TYPE(empty_frameData[i + sz]);
			if(ptr[i] > empty_frameData[i])
				ptr[i] = empty_frameData[i];
			SET_TERRAIN(ptr[i + sz],terr);
			if(!IS_DOUBLE(ptr[sz + i])){
				ptr[i + 1] = ptr[i];
				SET_TERRAIN(ptr[i + sz + 1],terr);
			}
		}
	}

	put_fon(PosX,PosY,SizeX,SizeY,ptr);
	put_terr_fon(PosX,PosY,SizeX,SizeY,ptr + sz);

	delete[] ptr;
}

aciML_EventSeq::aciML_EventSeq(void)
{
	int i;
	ID = 0;
	size = 0;
	for(i = 0; i < AML_MAX_DROP_EVENT; i ++) dropLevel[i] = 0;

	listID = new aKeyObj;

	ChannelID = 0;

	flags = 0;

	SeqIDs = NULL;
	SeqModes = NULL;
	SeqEvents = NULL;
}

aciML_EventSeq::~aciML_EventSeq(void)
{
	delete listID;
	free_mem();
}

void aciML_EventSeq::alloc_mem(int sz)
{
	size = sz;
	SeqIDs = new int[sz];
	SeqModes = new int[sz];
	SeqEvents = new aciML_Event*[sz];
}

void aciML_EventSeq::free_mem(void)
{
	if(SeqIDs) delete SeqIDs;
	if(SeqModes) delete SeqModes;
	if(SeqEvents) delete SeqEvents;

	SeqIDs = NULL;
	SeqModes = NULL;
	SeqEvents = NULL;
}

int aciML_EventSeqLine::dropEvents(int level,int channel)
{
	int i,index = 0,dropFlag = 0;
	index = last_index - 1;
	if(index < 0) index = AML_MAX_SEQ_EVENT - 1;

	for(i = 0; i < size; i ++){
		if(table[index] -> ChannelID == channel && table[index] -> priority == level){
			if(!(table[index] -> flags & AML_ACTIVE_EVENT)){
				dropFlag = 1;
				last_index = index;
				table[index] -> stop();
				size --;
			}
		}
		else {
			break;
		}

		index --;
		if(index < 0) index = AML_MAX_SEQ_EVENT - 1;
	}
	if(!size) clear();
	return dropFlag;
}

int aciML_EventSeq::dropEvents(int level)
{
	int i,ret = 0;
	for(i = 0; i < size; i ++){
		if(SeqEvents[i]){
			if(SeqEvents[i] -> priority == dropLevel[level]){
				SeqEvents[i] = NULL;
				ret = 1;
			}
			else
				break;
		}
	}
	return ret;
}

void aciML_EventSeqLine::putSeq(aciML_EventSeq* p)
{
	int i,index = 0,dropFlag = 0,level;
	index = last_index - 1;
	if(index < 0) index = AML_MAX_SEQ_EVENT - 1;

	if(!p || p -> size >= AML_MAX_SEQ_EVENT - size) return;

	for(i = 0; i < AML_MAX_DROP_EVENT; i ++){
		if(dropEvents(p -> dropLevel[i],p -> ChannelID))
			dropFlag ++;
		else
			break;
	}

	for(i = 0; i < p -> size; i ++){
		p -> SeqEvents[i] = aciML_GetEvent(p -> SeqIDs[i],p -> SeqModes[i]);
	}

	for(level = 0; level < dropFlag; level ++){
		if(!p -> dropEvents(level)) break;
	}
	for(i = 0; i < p -> size; i ++){
		if(p -> SeqEvents[i])
			put(p -> SeqEvents[i]);
	}
}

aciML_Event* aciML_Dispatcher::getEvent(int key,int mode)
{
	aciML_Data* p = (aciML_Data*)ml_data -> last;
	aciML_Event* ev;

	while(p){
		ev = (aciML_Event*)p -> event_list -> last;
		while(ev){
			if(mode == AML_STARTUP_MODE || mode == AML_SHUTDOWN_MODE){
				if(key == ev -> data) return ev;
			}
			else {
				if(ev -> check_key(key)) return ev;
			}
			ev = (aciML_Event*)ev -> prev;
		}
		p = (aciML_Data*)p -> prev;
	}
	return NULL;
}

void aciML_EventSeqLine::firstQuant(void)
{
	int code;
	aciML_Event* ev;
	if(!size) return;

	ev = table[first_index];
	if(!(ev -> flags & AML_ACTIVE_EVENT)){
		if(ev -> startupType == AML_KEY){
			code = ((iScanCode*)ev -> keys -> codes -> last) -> code;
			aciML_KeyTrap(code,ev -> startupType);
		}
		else
			aciML_KeyTrap(ev -> data,ev -> startupType);

		if(!(ev -> flags & AML_ACTIVE_EVENT)) get();
	}
}

void aciML_EventSeqLine::secondQuant(void)
{
	aciML_Event* ev;
	if(!size) return;

	ev = table[first_index];
	if(!(ev -> flags & AML_ACTIVE_EVENT) && ev -> flags & AML_FINISHED)
		get();
}

aciML_EventSeq* aciML_DataSet::getSeq(int id)
{
	aciML_EventSeq* seq = (aciML_EventSeq*)seq_list -> last;
	while(seq){
		if(seq -> ID == id || seq -> check_id(id)) return seq;
		seq = (aciML_EventSeq*)seq -> prev;
	}
	return NULL;
}

void aciML_EventSeq::mergeSeq(aciML_EventSeq* p)
{
	int i;

	for(i = 0; i < p -> size; i ++){
		SeqIDs[size + i] = p -> SeqIDs[i];
		SeqModes[size + i] = p -> SeqModes[i];
	}
	size += p -> size;
}

