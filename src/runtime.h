//TODO: stalkerg need timers kill!!!
//const int  RTO_GAME_QUANT_TIMER	= 1000/20;
const int  RTO_GAME_QUANT_TIMER	= 1000/20;
const int  RTO_ISCREEN_TIMER 	= 1000/20;
const int  RTO_PALETTE_TIMER 	= 1000/20;
const int  RTO_ESCAVE_GATE_TIMER	= 1000/20;
const int  RTO_IMAGE_TIMER		= 1000/20;

// Runtime objects...
enum RoadRuntimeObjID
{
	RTO_GAME_QUANT_ID  = 1,
	RTO_LOADING1_ID,		// 2
	RTO_LOADING2_ID,		// 3
	RTO_LOADING3_ID,		// 4
	RTO_MAIN_MENU_ID,		// 5
	RTO_FIRST_ESCAVE_ID,		// 6
	RTO_FIRST_ESCAVE_OUT_ID,	// 7
	RTO_ESCAVE_ID,			// 8
	RTO_ESCAVE_OUT_ID,		// 9
	RTO_PALETTE_TRANSFORM_ID,	// 10
	RTO_SHOW_IMAGE_ID,		// 11
	RTO_SHOW_AVI_ID,		// 12

	RTO_MAX_ID
};

// BaseRTO flags...
const int  RTO_INIT_FLAG		= 0x01;
const int  RTO_QUANT_FLAG		= 0x02;
const int  RTO_FINIT_FLAG		= 0x04;
const int  RTO_ALL_FLAGS		= RTO_INIT_FLAG | RTO_QUANT_FLAG | RTO_FINIT_FLAG;

extern int IsMainMenu;

struct BaseRTO : XRuntimeObject
{
	int flags;
	int NextID;

	void SetNext(int id){ NextID = id; IsMainMenu = id == RTO_MAIN_MENU_ID; }
	void SetFlag(int fl){ flags |= fl; }
	void ClearFlag(int fl){ flags &= ~fl; }

	BaseRTO(void){ NextID = XT_TERMINATE_ID; flags = 0; }
};

struct LoadingRTO1 : BaseRTO
{
	virtual void Init(int id);
	virtual int Quant(void);
	virtual void Finit(void);

	LoadingRTO1(void){ ID = RTO_LOADING1_ID; Timer = 1; }
};

struct LoadingRTO2 : BaseRTO
{
	virtual void Init(int id);
	virtual int Quant(void);
	virtual void Finit(void);

	LoadingRTO2(void){ ID = RTO_LOADING2_ID; Timer = 1; }
};

struct LoadingRTO3 : BaseRTO
{
	virtual void Init(int id);
	virtual int Quant(void);
	virtual void Finit(void);

	LoadingRTO3(void){ ID = RTO_LOADING3_ID; Timer = 1; }
};

struct GameQuantRTO : BaseRTO
{
	virtual void Init(int id);
	virtual void Finit();
	virtual int Quant(void);

	GameQuantRTO(void){ ID = RTO_GAME_QUANT_ID; Timer = RTO_GAME_QUANT_TIMER; }
};

struct MainMenuRTO : BaseRTO
{
	virtual void Init(int id);
	virtual int Quant(void);
	virtual void Finit(void);

	MainMenuRTO(void){ ID = RTO_MAIN_MENU_ID; Timer = RTO_ISCREEN_TIMER; }
};

struct FirstEscaveRTO : BaseRTO
{
	virtual void Init(int id);
	virtual int Quant(void);
	virtual void Finit(void);

	FirstEscaveRTO(void){ ID = RTO_FIRST_ESCAVE_ID; Timer = RTO_ISCREEN_TIMER; }
};

struct EscaveRTO : BaseRTO
{
	virtual void Init(int id);
	virtual int Quant(void);
	virtual void Finit(void);

	EscaveRTO(void){ ID = RTO_ESCAVE_ID; Timer = RTO_ISCREEN_TIMER; }
};

struct EscaveOutRTO : BaseRTO
{
	virtual void Init(int id);
	virtual int Quant(void);
	virtual void Finit(void);

	EscaveOutRTO(void){ ID = RTO_ESCAVE_OUT_ID; Timer = RTO_ISCREEN_TIMER; }
};

struct FirstEscaveOutRTO : BaseRTO
{
	virtual void Init(int id);
	virtual int Quant(void);
	virtual void Finit(void);

	FirstEscaveOutRTO(void){ ID = RTO_FIRST_ESCAVE_OUT_ID; Timer = RTO_ISCREEN_TIMER; }
};

// ShowImageRTO flags...
#define IMG_RTO_CDTRACK_TIME	0x01
#define IMG_RTO_KEEP_IMAGE	0x02
#define IMG_RTO_NO_IMAGE	0x04
#define IMG_RTO_SHOW_TEXT	0x08
#define IMG_RTO_CD_IMAGE	0x10

#define RTO_SHOW_IMAGE_MAX	5
struct ShowImageRTO : BaseRTO
{
	int count;
	int numFiles;
	int curFile;

	int* Flags;
	char** fileNames;

	int imageSX;
	int imageSY;
	char* imageData;

	virtual void Init(int id);
	virtual int Quant(void);
	virtual void Finit(void);

	void SetNumFiles(int num){ numFiles = num; curFile = 0; }
	void SetName(const char* p,int id = 0){ strcpy(fileNames[id],p); count = 0; Flags[id] = 0; }

	void SetFlag(int id,int fl){ Flags[id] |= fl; }
	void DropFlag(int id,int fl){ Flags[id] &= ~fl; }

	ShowImageRTO(void);
};

// ShowAviRTO flags...
#define AVI_RTO_HICOLOR 	0x01

struct ShowAviRTO : ShowImageRTO
{
	void* aviBuf;

	virtual void Init(int id);
	virtual int Quant(void);
	virtual void Finit(void);

	ShowAviRTO(void);
};

struct PaletteTransformRTO : BaseRTO
{
	virtual int Quant(void);
	void Init(int);
	void Finit();
	PaletteTransformRTO(void){ ID = RTO_PALETTE_TRANSFORM_ID; Timer = RTO_PALETTE_TIMER; }
};

void rtoSetNextID(int id,int next);
void rtoSetFlag(int id,int fl);
void rtoClearFlag(int id,int fl);


