
//#define AML_DEBUG

#define AML_MAX_SPEECH_MODE	4

#define AML_COMMAND_STARTED	0x01

struct aciML_EventCommand : public iListElement
{
	int code;
	int data0;
	int data1;

	int start_timer;
	int flags;

	iListElement* owner;

	void init(void){ code = data0 = data1 = 0; start_timer = 0; flags = 0; }

	aciML_EventCommand(void){ code = data0 = data1 = 0; start_timer = 0; flags = 0; }
};

// aciML_Event flags...
#define AML_ACTIVE_EVENT	0x01
#define AML_SEQUENCE_EVENT	0x02
#define AML_NO_DEACTIVATE	0x04
#define AML_IF_NOT_LOCKED	0x08
#define AML_IF_LOCKED		0x10
#define AML_FINISHED		0x20
#define AML_WAITING		0x40
#define AML_DISABLED		0x80

struct aciML_Event : public iListElement
{
	int startupType;
	int data;

	int rndValue;

	int ChannelID;
	int priority;

	int cur_timer;
	int active_time;

	int flags;

	aKeyObj* keys;
	iList* commands;

	int check_key(int k);
	int check_seq(void);
	void start(void);
	void stop(void){ flags &= ~AML_ACTIVE_EVENT; flags |= AML_FINISHED; }

	void quant(void);
	void inc_timer(void);

	void add_key(int k){ keys -> add_key(k); }
	void add_command(aciML_EventCommand* p);

	aciML_Event(void);
	~aciML_Event(void);
};

#define AML_MAX_DROP_EVENT	8

struct aciML_EventSeq : public iListElement
{
	int ID;
	aKeyObj* listID;

	int flags;

	int size;
	int dropLevel[AML_MAX_DROP_EVENT];
	int ChannelID;

	int* SeqIDs;
	int* SeqModes;

	aciML_Event** SeqEvents;

	void alloc_mem(int sz);
	void free_mem(void);
	int dropEvents(int level);

	void add_id(int id){ listID -> add_key(id); }
	int check_id(int id){ return listID -> check(id); }

	void mergeSeq(aciML_EventSeq* p);

	aciML_EventSeq(void);
	~aciML_EventSeq(void);
};

// aciML_Data::flags...
#define AML_FRAME_CHECK 	0x01
#define AML_DISABLED_ML 	0x02

struct aciML_Data : public iListElement
{
	int ID;
	int ChannelID;

	int flags;
	char* name;

	iList* event_list;

	int key_trap(int key,int mode);
	int check_xy(int x,int y);
	int drop_events(int channel,int pr);
	void active_event_quant(void);
	void rnd_event_quant(void);
	int check_channel_events(int ch);
	void inc_events_timer(void);
	void clear_events(void);

	void add_event(aciML_Event* p);
	int isEventActive(int key,int mode);

	aciML_Data(void);
	~aciML_Data(void);
};

// aciML_ItemData::put_frame() modes...
const int 	AML_PUT_EMPTY		= 0x00;
const int 	AML_PUT_FILLED		= 0x01;
const int 	AML_PUT_LOW_LEVEL	= 0x02;

// aciML_ItemData flags...
const int 	AML_ACTIVE_ITEM 	= 0x01;

struct aciML_ItemData : public iListElement
{
	int PosX;
	int PosY;

	int SizeX;
	int SizeY;

	int NullLevel;
	int EmptyTerrain;

	int ItemID;

	int flags;

	char* frameName;

	unsigned char* frameData;
	unsigned char* empty_frameData;

	void init(void);
	void finit(void);

	void build_empty_frame(void);
	void put_frame(int mode);
	void put2low_level(void);

	int check_xy(int x,int y);
	int check_double_level(void);

	aciML_ItemData(void);
	~aciML_ItemData(void);
};

struct aciML_DataSet : public iListElement
{
	int ID;
	iList* ml_data;
	iList* items;
	iList* seq_list;

	int SpeechChannel;
	int SpeechPriority[AML_MAX_SPEECH_MODE];

	void items_keytrap(int x,int y);

	void add_data(aciML_Data* p);
	void add_item(aciML_ItemData* p);
	void add_seq(aciML_EventSeq* p);

	void put_item(int id);
	void init_items(void);
	void finit_items(void);
	void redraw_items(void);

	void disable_ml(int id);
	void enable_ml(int id);

	aciML_EventSeq* getSeq(int id);

	aciML_DataSet(void);
	~aciML_DataSet(void);
};

const int  AML_MAX_EVENT	 = 50;

struct aciML_EventLine
{
	int size;
	int first_index;
	int last_index;
	int flags;

	aciML_EventCommand** table;
	char* mem_heap;

	void clear(void);
	void put(int cd,int dt0 = 0,int dt1 = 0);
	aciML_EventCommand* get(void);

	aciML_EventLine(void);
	~aciML_EventLine(void);
};

struct aciML_FlushData : public iListElement
{
	int PosX;
	int PosY;

	int SizeX;
	int SizeY;
};

const int 	AML_MAX_FLUSH	= 20;

struct aciML_FlushLine
{
	int size;
	int first_index;
	int last_index;
	int flags;

	aciML_FlushData** table;
	char* mem_heap;

	void clear(void);
	void put(int x,int y,int sx,int sy);
	aciML_FlushData* get(void);

	aciML_FlushLine(void);
	~aciML_FlushLine(void);
};

const int 	AML_MAX_SEQ_EVENT   = 10;

struct aciML_EventSeqLine
{
	int size;
	int first_index;
	int last_index;
	int flags;

	aciML_Event** table;

	void clear(void);
	void put(aciML_Event* p);
	void putSeq(aciML_EventSeq* p);
	aciML_Event* get(void);

	int dropEvents(int level,int channel);

	void firstQuant(void);
	void secondQuant(void);

	aciML_EventSeqLine(void);
	~aciML_EventSeqLine(void);
};

#define AML_MAX_SEQ_LINE_ID	5

// aciML_Dispatcher flags...
#define AML_STARTUP_MODE	0x01
#define AML_SHUTDOWN_MODE	0x02
#define AML_END_SHUTDOWN	0x04
#define AML_PAUSE		0x08

struct aciML_Dispatcher
{
	int flags;
	iList* ml_data;
	iList* ml_data_sets;

	aciML_DataSet* curDataSet;
	aciML_EventSeqLine** SeqList;

	int timer;
	int startup_timer;
	int shutdown_timer;

	aciML_EventLine* event_line;
	aciML_FlushLine* flush_line;

#ifdef	AML_DEBUG
	XStream logFile;
#endif

	void add_data_set(aciML_DataSet* p);
	void init_data_set(int id);

	void event_quant(void);
	void active_event_quant(void);
	void rnd_event_quant(void);
	void inc_events_timer(void);
	int drop_channel_events(int channel,int pr);
	int check_channel_events(int ch);
	void clear_events(void);

	void register_names(void);

	void startup(void);
	void shutdown(void);

	void redraw_items(void);

	void quant(void);

	void key_trap(int key,int mode = -1);

	int isEventActive(int key,int mode);
	aciML_Event* getEvent(int key,int mode);
	aciML_EventSeq* getEventSeq(int id){ return curDataSet -> getSeq(id); }

	void send_event(int cd,int d0 = 0,int d1 = 0);
	void add_flush(int x,int y,int sx,int sy);
	void flush(int mode = 1);

	void init(void);
	void finit(void);

	aciML_Dispatcher(void);
	~aciML_Dispatcher(void);
};

extern aciML_Dispatcher* aciML_D;

void aciML_SendEvent(int cd,int d0 = 0,int d1 = 0);

