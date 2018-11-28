//----------------------------------------New header-------------------------------------------------------------

#define SIMPLE_PARTICLE_ON
#define PARTICLE_ON
#define STACK_EMPTY_IGNORE
#define MOVELAND_ON
#define DEVICE_ON

//#define STACK_EMPTY_IGNORE

const int SOBJ_ACTIVE = 1;
const int SOBJ_DISCONNECT = 2;
const int SOBJ_AUTOMAT = 4;
const int SOBJ_LINK = 8;
const int SOBJ_WAIT_CONFIRMATION = 16;

const int FIXED_SHIFT = 8;

const int DSHIFT = 8;

const char MAX_ACTION_UNIT = 1;
const char MAX_EFFECT_TYPE = 15;
const char MAX_ITEMS_OBJECT = 3;
const char MAX_MAP_OBJECT = 5;

const int MAX_INSECT_UNIT = 30;
const int MAX_BULLET_PAGE = 32;
const int MAX_CLEF_OBJECT = 1;
const int MAX_HORDE_OBJECT = 50;
const int MAX_HORDE_SOURCE_OBJECT = 32;
const int MAX_FISH_WARRIOR = 32;

const int ROTOR_PROCESS_LIFE_TIME = 120;

const int TELEPORT_ESCAVE_ID = 83000;

struct ActionUnit;

struct StorageType
{
	int Max,ObjectPointer;
	GeneralObject** Data;

	void Init(int _max = 100);
	void Free(void);
	GeneralObject* Active(void);
	void Deactive(GeneralObject* p);
	GeneralObject* GetAll(void);

	void Check(void);
};

struct MemoryStorageType;

struct StorageClusterType
{
	int MaxPoint,CurrPoint;
	int PointSize;
	char** PointData;
	char* Data;
	StorageClusterType* Next;

	void Init(MemoryStorageType* p);
	void Free(void);
	char CheckSpace(void);
	GeneralObject* GetPoint(void);
	void PutPoint(GeneralObject* p);
};

struct MemoryStorageType
{
	int NumCluster;
	int ElementSize;
	int MaxElement;

	StorageClusterType* Tail;

	void Init(int mElement,int eSize);
	void Free(void);
	GeneralObject* GetPoint(void);
	void PutPoint(GeneralObject* p);
};

struct GameObjectDispatcher
{
	int Num;
	BaseObject* Tail;
	BaseObject* ViewTail;
	int GlobalTime;
	char FirstQuant;
	int cWorld;

	void Init(void);
	void Free(void);
	void Open(void);
	void Close(void);
	void InitTime(void);

	void ConnectBaseList(BaseObject* p);
	void DisconnectBaseList(BaseObject* p);
	void Quant(void);
	void DrawQuant(void);
	void Sort(void);
	void ConnectViewList(BaseObject* p);
	void ConnectNextView(BaseObject* p,BaseObject* link);
	void ConnectPrevView(BaseObject* p,BaseObject* link);

	void NetEvent(void);	
};

struct UnitListType
{
	int Num;
	GeneralObject* Tail;

	virtual void Init(void);
	virtual void Free(void);
	virtual void Quant(void);
	virtual void NetEvent(int type,int id);	
	virtual void FreeUnit(GeneralObject* p);

	virtual void ConnectTypeList(GeneralObject* p);
	virtual void DisconnectTypeList(GeneralObject* p);
	GeneralObject* GetNetObject(int id);
	void ClearNet(void);
};

struct UnitBaseListType : UnitListType
{
	virtual void ConnectTypeList(GeneralObject* p);
	virtual void DisconnectTypeList(GeneralObject* p);
};

struct UnitList 
{
	int Total;
	int* MaxUnit;
	StorageType* UnitStorage;
	GeneralObject** UnitData;

	int Num;
	GeneralObject* Tail;

	void Init(Parser& in);	
	void Open(Parser& in);

	void ConnectTypeList(GeneralObject* p);
	void DisconnectTypeList(GeneralObject* p);
	void Quant(void);

	void ConnectObject(BaseObject* p);

	void Add2Next(GeneralObject* p,GeneralObject* t);
	void Add2Prev(GeneralObject* p,GeneralObject* t);

	GeneralObject* GetNetObject(int id);

	virtual void NetEvent(int type,int id);

	GeneralObject* GetObject(int type){ return UnitStorage[type].Active(); };
	void ClearNet(void);
};

void GeneralSystemInit(void);
void GeneralSystemFree(void);
void GeneralSystemClose(void);
void GeneralSystemOpen(void);

void GeneralTableInit(void);
void GeneralTableFree(void);
void GeneralTableOpen(void);

void OpenCyclicPal(void);
void CloseCyclicPal(void);

extern GameObjectDispatcher GameD;

int compare(BaseObject* p,BaseObject* n);

int getDistX(int s0,int s1);
int getDistY(int s0,int s1);
//int objDist(BaseObject* e1,BaseObject* e2);

void setMapPixel(int px,int py,int col);
void putMapPixel(int px,int py,int col);
//int path_trace(const Vector& c1,const Vector& c2);
//void line_trace(const Vector& c1,const Vector& c2);

void G2L(int x,int y,int& xl,int& yl);
int MapLineTrace(Vector& c1,Vector& c2);
char GetCollisionMap(int x,int y,int z);
char GetMapLevel(Vector& v);
uchar GetAlt(int x,int y,int z,uchar& alt);
uchar GetGlobalAlt(int x,int y);

int G2LS(int x,int y,int z,int& sx,int& sy);
int G2LF(int x,int y,int z,int& sx,int& sy);
int global_to_screen_coords(int x, int y, int z, int &sx, int &sy);
void G2LQ(int x,int y,int z,int& sx,int& sy);
void G2LP(int x,int y,int z,int& sx,int& sy);
void S2G(int xs,int ys,int& xg,int& yg);

//void SetMapBuff(int x,int y,int x_size,int y_size,uchar* MapBuf);

void DrawMechosWheelDown(int cx1,int cy1,int cx2,int cy2,int lh,int h,int delta,int nx,int ny,char step);
void DrawMechosWheelUp(int cx1,int cy1,int cx2,int cy2,int lh,int h,int delta,int nx,int ny,char step);
void DrawMechosBody(int x,int y,int speed,int level);
void DrawMechosParticle(int x,int y,int speed,int level,int n,Object* p=NULL);

int TouchSphere(Vector& r0,Vector& r1,Vector& c,int rad,int& r);
int TouchSphereZ(Vector& r0,Vector& r1,Vector& c,int rad,int& r);

void DrawShadow(int x,int y,int x_size,int y_size,uchar* buff);

char GetMapLevelType(Vector& v,uchar*& type);
int BigGetAlt(int x,int y,int z,uchar& alt,uchar terrain);

extern char SetWorldFlag;
extern int CurrentWorld;
extern int TurnSideX;
extern int TurnSideY;

const int PTrack_mask_x = (map_size_x << 8) - 1;
extern int PTrack_mask_y;

void ScreenLineTrace(Vector& v0,Vector& v1,uchar* ColorTable,uchar flag);

void CheckActiveJump(BaseObject* p);

//void PrepareProtoCrypt(void);

extern uchar** WorldPalData;
extern int WorldPalNum;
extern int WorldPalCurrent;

extern int PalIterLock;

extern int GeneralMapReload;
extern int GeneralLoadReleaseFlag;


int CheckStartJump(Object* p);

void GeneralSystemLoad(XStream& in);
void GeneralSystemSave(XStream& out);

extern uchar* 	palbufSrc;
//extern int WorldBulletNum;

extern int GameBulletNum;

#define NETWORK_IN_STREAM events_in
#define NETWORK_OUT_STREAM events_out

int GetMasterID(int x,int y,int radius);

const int AI_MESSAGE_DELTA = 20;

extern int aiNumMessage;

const int AI_MESSAGE_MAX_STRING = 16;
const int AI_MESSAGE_LEN_STRING = 200;
const int MAX_MLLINK_NAME = 32;

const int MAX_AI_MESSAGE_SPEED = 0;

struct aiMessageType
{
	int LastFrame;
	int Num;
	int Type;
	char Data[AI_MESSAGE_MAX_STRING][AI_MESSAGE_LEN_STRING];
	int Time[AI_MESSAGE_MAX_STRING];
	int Color[AI_MESSAGE_MAX_STRING];

	void Load(Parser& in);
	void Send(int speed = 0,int n = 0xff,int sf = 1);
	int GetTime(int n = 0xff);
};

const int AI_MESSAGE_INDEX_LUCK = -1;
const int AI_MESSAGE_INDEX_DOMINANCE = -2;
const int AI_MESSAGE_INDEX_TABUTASK = -3;

struct aiMessageListElem
{
	aiMessageListElem* Next;
	aiMessageListElem* Prev;
	int Speed,Mask,SpeedFlag;
	int Index;
	int Luck,Dominance,TabuTask;
};

struct aiMessageList
{
	int Num;
	aiMessageListElem* Tail;
	aiMessageListElem* View;
	int Time;

	void Open(void);
	void Close(void);
	void Send(int ind,int speed = 0,int n = 0xff,int sf = 1);
	void Quant(void);
	void Connect(aiMessageListElem* p);
	void Disconnect(aiMessageListElem* p);
	void SendDominance(int dom);
	void SendLuck(int luck);
	void SendTabuTask(int luck);
};

extern aiMessageList aiMessageQueue;
extern aiMessageType* aiMessageData;

const int AI_MESSAGE_FOSTRAL = 0;
const int AI_MESSAGE_GLORX = 1;
const int AI_MESSAGE_NECROSS = 2;
const int AI_MESSAGE_XPLO = 3;
const int AI_MESSAGE_KHOX = 4;
const int AI_MESSAGE_BOOZINA = 5;
const int AI_MESSAGE_WEEXOW = 6;
const int AI_MESSAGE_HMOK = 7;
const int AI_MESSAGE_THREALL = 8;
const int AI_MESSAGE_ZNOY = 9;

const int AI_MESSAGE_PODISH = 10;
const int AI_MESSAGE_INCUBATOR = 11;
const int AI_MESSAGE_VIGBOO = 12;
const int AI_MESSAGE_LAMPASSO = 13;
const int AI_MESSAGE_OGOROD = 14;
const int AI_MESSAGE_BZONE = 15;
const int AI_MESSAGE_ZEEPA = 16;
const int AI_MESSAGE_SPOBS = 17;

const int AI_MESSAGE_SPIRAL = 18;
const int AI_MESSAGE_GHORB = 19;
const int AI_MESSAGE_ARMOR = 20;
const int AI_MESSAGE_COPTER = 21;
const int AI_MESSAGE_DGHORB = 22;
const int AI_MESSAGE_DCOPTER = 23;
const int AI_MESSAGE_DENERGY = 24;
const int AI_MESSAGE_ENERGY = 25;
const int AI_MESSAGE_MACHOTINE = 26;
const int AI_MESSAGE_RANDOM_UPDATE = 27;

const int AI_MESSAGE_NYMBOS_LOST = 28;
const int AI_MESSAGE_PHLEGMA_LOST = 29;
const int AI_MESSAGE_PODISH_WAY = 30;
const int AI_MESSAGE_INCUBATOR_WAY = 31;
const int AI_MESSAGE_20 = 32;
const int AI_MESSAGE_01 = 33;
const int AI_MESSAGE_12 = 34;
const int AI_MESSAGE_ELR_LOW = 35;
const int AI_MESSAGE_ELR_HI = 36;
const int AI_MESSAGE_GLORX_WAY = 37;
//-------NEW--------
const int AI_MESSAGE_DOWN_ELLECH = 38;
const int AI_MESSAGE_DOWN_KERN = 39;
const int AI_MESSAGE_DOWN_PIP = 40;
const int AI_MESSAGE_DOWN_WEEZYK = 41;
const int AI_MESSAGE_LOW_ARMOR = 42;
const int AI_MESSAGE_AUTOMATIC_ON = 43;
const int AI_MESSAGE_AUTOMATIC_OFF = 44;
const int AI_MESSAGE_NECROSS_MOVE = 45;
const int AI_MESSAGE_GLORX_MOVE_RUB = 46;
const int AI_MESSAGE_GLORX_MOVE_PRORUB = 47;

const int AI_MESSAGE_INCOMING_MESSAGE = 48;

const int AI_MESSAGE_DESTROY_SPOBS_MESSAGE = 49;
const int AI_MESSAGE_DESTROY_THREALL_MESSAGE = 50;

//NEW for STEAM REALESE
const int AI_MESSAGE_CAMERA_HELP = 51;

void NetworkSetTnt(int id);

void aiMessageTouch(int speed,int ind);

void FreeAll(int mode = 1);

extern int BulletUpdateFreq;
extern int JumpBallUpdateFreq;
extern int StuffUpdateFreq;
extern int DebrisUpdateFreq;
extern int HordeUpdateFreq;

const int CPAL_CHANGE_CYCLE = 1;
const int CPAL_SHOW_PASSAGE = 2;
const int CPAL_HIDE_PASSAGE = 3;
const int CPAL_RESTORE_CHANGE = 4;
const int CPAL_PASSAGE_TO = 5;
const int CPAL_PASSAGE_FROM = 6;
const int CPAL_SPOBS_TO = 7;
const int CPAL_THREALL_TO = 8;

const int SPOBS_PAL_TERRAIN = 2;

struct PalPoint
{
	int Mode;
	int Time;
	int FirstColor[768];
	int DeltaColor[768];
	PalPoint* Next;
	PalPoint* Prev;

	void Set(int mode,int time,uchar* p1 = NULL,uchar* p2 = NULL);
	void Quant(void);
};

struct PalConstructor
{
	PalPoint* pPrev;
	PalPoint* Tail;
	int PalEnable;

	void Init(void);
	void Free(void);
	void Quant(void);
	void Set(int mode,int time,uchar* p1 = NULL,uchar* p2 = NULL);
	void Clear(PalPoint* p);
};

void NetworkWorldOpen(void);
void NetworkWorldClose(void);

extern PalConstructor PalCD;
void ClearNetwork(void);

extern int MaxOxigenBar;

extern int VangerLuck;

void InitProtoCrypt(void);
void FreeProtoCrypt(void);
void LoadProtoCrypt(XStream& in,int v);
void SaveProtoCrypt(XStream& in);

const int MAX_SKIP_LOCATION = 20;
extern int ChangeWorldConstraction;
extern int ChangeWorldSkipQuant;
//#define ITEM_LOG

extern unsigned int NetGlobalTime;
int check_dynamics_locals();

#define CHECK_SICHER_CRASH(s) {if(check_dynamics_locals() || aciCurArmor > aciMaxArmor || aciCurEnergy > aciMaxEnergy) ErrH.Abort(s);}

extern int aiCutLuck;
extern int aiCutDominance;

void NetworkSetLocation(int id);

struct aiRndType
{
	unsigned aiRndVal;
	void aiInit(unsigned d) { aiRndVal=d; }
	aiRndType() { aiRndVal=0; }
	inline unsigned aiRnd(unsigned m){
		aiRndVal ^= aiRndVal >> 3;
		aiRndVal ^= aiRndVal << 28;
		aiRndVal &= 0x7FFFFFFF;
		if(!m) return 0;
		return aiRndVal%m;
	};	
};

extern int NetworkLoadEnable;

inline int GetDistTime(unsigned int t1,unsigned int t2)
{
	unsigned int l1 = t1 - t2;
	unsigned int l2 = t2 - t1;
	if(l2 > l1) return l1;
	else return l2;
};

struct PassageImageType
{
	int xSize,ySize;
	char* Name;
	char* Data;
	char Pal[768];
	void Open(char* name);
	void Close(void);
	void Show(void);
	void Hide(void);
};

extern int PassageBmpNum;
extern int PassageBmpPrev;
extern PassageImageType* PassageImageData;

//!!!!!!!!!!!!!!
//extern XStream StorageLog;
//!!!!!!!!!!!!!!!!!!!
