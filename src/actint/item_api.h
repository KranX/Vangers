
enum actintItemTypes
{
	// Weapons...
	ACI_MACHOTINE_GUN_LIGHT    = 1,
	ACI_MACHOTINE_GUN_HEAVY,

	ACI_SPEETLE_SYSTEM_LIGHT,
	ACI_SPEETLE_SYSTEM_HEAVY,

	ACI_GHORB_GEAR_LIGHT,
	ACI_GHORB_GEAR_HEAVY,

	ACI_BEEBBANOZA_BLOCKADE,

	ACI_CRUSTEST_CANNON,

	ACI_TERMINATOR,
	ACI_TERMINATOR2,

	// Ammo...
	ACI_SPEETLE_SYSTEM_AMMO0,
	ACI_SPEETLE_SYSTEM_AMMO1,

	ACI_CRUSTEST_CANNON_AMMO,

	// Devices...
	ACI_COPTE_RIG,			// Jumper...
	ACI_EMPTY_COPTE_RIG,		// Empty jumper...

	ACI_CUTTE_RIG,			// Sail device...
	ACI_EMPTY_CUTTE_RIG,		// Empty sail device...

	ACI_CROT_RIG,			// Wormer device...
	ACI_EMPTY_CROT_RIG,		// Empty wormer device...

	ACI_LHARK_DEVICE,		// Some device...
	ACI_EMPTY_LHARK_DEVICE, 	// Some empty device...

	ACI_RADAR_DEVICE,

	// Items...
	ACI_CIRTAINER,			// Container for cirt...
	ACI_EMPTY_CIRTAINER,		// Empty container...

	ACI_CONLARVER,
	ACI_EMPTY_CONLARVER,

	// Wares...
	ACI_NYMBOS,			// Eleepod's eggs...
	ACI_PHLEGMA,			// Incubator slime...
	ACI_HEROIN,
	ACI_SHRUB,
	ACI_POPONKA,
	ACI_TOXICK,

	// Cultic units...
	ACI_ELEECH,			// Small eleepod...
	ACI_KERNOBOO,
	ACI_PIPETKA,
	ACI_WEEZYK,

	// Artefacts...
	ACI_PROTRACTOR,
	ACI_MECHANIC_MESSIAH,
	ACI_FUNCTION83,
	ACI_SPUMMY,
	ACI_BOOT_SECTOR,
	ACI_PEELOT,

	ACI_LEEPURINGA, 		// Statue...
	ACI_DEAD_ELEECH,

	// Tokens...
	ACI_RUBOX,

	// Mehos parts...
	ACI_SANDOLL_PART1,
	ACI_SANDOLL_PART2,

	ACI_LAST_MOGGY_PART1,
	ACI_LAST_MOGGY_PART2,

	ACI_QUEEN_FROG_PART1,
	ACI_QUEEN_FROG_PART2,

	ACI_LAWN_MOWER_PART1,
	ACI_LAWN_MOWER_PART2,

	ACI_WORMASTER_PART1,
	ACI_WORMASTER_PART2,

	ACI_TANKACID,

	// Additional items...

	// Weapons...
	ACI_AMPUTATOR,
	ACI_DEGRADATOR,
	ACI_MECHOSCOPE,

	// Ammo...
	ACI_GLUEK,

	// Warez...
	ACI_TABUTASK,
	ACI_ELEEPOD,
	ACI_BEEBOORAT,
	ACI_ZEEX,

	// Personal Articles...
	ACI_PALOCHKA,
	ACI_PIPKA,
	ACI_NOBOOL,
	ACI_BOORAWCHIK,
	ACI_PEREPONKA,
	ACI_ZEEFICK,

	// Some more items...
	ACI_TABUTASK_FAILED,
	ACI_TABUTASK_SUCCESSFUL,

	ACI_EMPTY_AMPUTATOR,
	ACI_EMPTY_DEGRADATOR,
	ACI_EMPTY_MECHOSCOPE,

	ACI_ROTTEN_KERNOBOO,
	ACI_ROTTEN_PIPETKA,
	ACI_ROTTEN_WEEZYK,

	ACI_MAX_TYPE
};

enum actintItemEvents
{
	ACI_PUT_ITEM	= 1,
	ACI_DROP_ITEM,
	ACI_CHANGE_ITEM_DATA,

	ACI_ACTIVATE_ITEM,
	ACI_DEACTIVATE_ITEM,
	ACI_GET_ITEM_DATA,
	ACI_CHECK_MOUSE,
	ACI_SET_DROP_LEVEL,

	ACI_PUT_IN_SLOT,

	ACI_LOCK_INTERFACE,
	ACI_UNLOCK_INTERFACE,

	ACI_SHOW_TEXT,
	ACI_HIDE_TEXT,

	ACI_SHOW_ITEM_TEXT,

	ACI_DROP_CONFIRM,

	ACI_MAX_EVENT
};

enum aciProtractorEvents
{
	ACI_PROTRACTOR_EVENT1 = 1,
	ACI_PROTRACTOR_EVENT2,
	ACI_PROTRACTOR_EVENT3,
	ACI_PROTRACTOR_EVENT4,
	ACI_PROTRACTOR_EVENT5,
	ACI_PROTRACTOR_EVENT6,
	ACI_PROTRACTOR_EVENT7,
	ACI_PROTRACTOR_EVENT8,

	ACI_MAX_PROTRACTOR_EVENT
};

enum aciMechMessiahEvents
{
	ACI_MECH_MESSIAH_EVENT1 = 1,
	ACI_MECH_MESSIAH_EVENT2,
	ACI_MECH_MESSIAH_EVENT3,
	ACI_MECH_MESSIAH_EVENT4,
	ACI_MECH_MESSIAH_EVENT5,
	ACI_MECH_MESSIAH_EVENT6,

	ACI_MAX_MECH_MESSIAH_EVENT
};

// flags...
const int 	ACI_WEAPON	= 0x01;
const int 	ACI_ACTIVE	= 0x02;

struct actintItemData
{
	int type;
	int data0;
	int data1;
	int maxData;

	int flags;
	int slot;

	void* actintOwner;
	void* stuffOwner;

	actintItemData(void);
	actintItemData(int tp);
};

// aciPromptData::align_type values...
#define ACI_PROMPT_UP		0
#define ACI_PROMPT_CENTER	1

struct aciPromptData
{
	int NumStr;
	int align_type;
	int fontID;

	int CurTimer;

	unsigned char** StrBuf;
	unsigned int* ColBuf;
	int* TimeBuf;

	int* PosX;
	int* PosY;

	void alloc_mem(int size);
	void free_mem(void);
	void copy_data(aciPromptData* src);

	void add_str(int num,unsigned char* p);

	void redraw(int scr_x,int scr_y,int scr_sx,int scr_sy);
	void quant(void);

	aciPromptData(void){ NumStr = 0; StrBuf = NULL; TimeBuf = NULL; align_type = ACI_PROMPT_UP; CurTimer = 0; PosX = PosY = NULL; ColBuf = NULL; }
	aciPromptData(int size);

	~aciPromptData(void);
};

extern int aciCurEnergy;
extern int aciMaxEnergy;
extern int aciMaxArmor;
extern int aciCurArmor;
extern int aciPickupItems;
extern int aciPickupDevices;

extern int aciRacingFlag;
extern char* aciCurRaceType;
extern char* aciCurRaceInfo;
extern int aciGamerColor;

extern const char* aci_curLocationName;

void aciSendEvent2actint(int code,actintItemData* p,int data = 0);
void aciSendEvent2itmdsp(int code,actintItemData* p,int data = 0);
char getObjectPosition(int& x,int& y);
actintItemData* aciGetLast(void);

int aciGetScreenItem(int x,int y);

int aciReInitItem(actintItemData* p);
int aciReInitItemXY(actintItemData* p,int x,int y);
void aciKillLinks(void);

void aciWorldLinksON(void);
void aciWorldLinksOFF(void);
void aciOpenWorldLink(int w1,int w2);
int aciWorldLinkExist(int w1,int w2,int sp_flag = 1);

int uvs_aciKillItem(int id,int id1 = -1);
int uvs_aciKillOneItem(int id,int x,int y);

void uvs_aciChangeItem(int id_from,int id_to);
void uvs_aciChangeOneItem(int id_from,int id_to,int x,int y);

void aciChangeItem(actintItemData* p);

void aciActivateItemFunction(int itemID,int fncID);
void aciDeactivateItemFunction(int itemID,int fncID);

int aciCheckItemPickUp(actintItemData* p);

void aciActionEvent(void);
int aciCheckSlot(void);

void aciGetItemCoords(actintItemData* p,int& x,int& y);
void aciGetItemCoords(int id,int& x,int& y);
int aciPutItem(int id,int x = -1,int y = -1);
void aciRemoveItem(actintItemData* d);
void aciNewMechos(int newID);

void aciSendPrompt(aciPromptData* p);

void aciAddTeleportMenuItem(int id,int fnc_id);
void aciDeleteTeleportMenuItem(int fnc_id);
void aciClearTeleportMenu(void);

char* aciGetPlayerName(void);
char* aciGetPlayerPassword(void);
int aciGetPlayerColor(void);

void aciRefreshTargetsMenu(void);
void aciPrepareText(char* ptr);

void aciDropMoveItem(void);
void SetWorldBeebos(int n);

void aciSetRedraw(void);

void aciPrepareEndImage(void);
void aciInitEndGame(int id);
int aciTextQuant(void);
char* aciLoadPackedFile(XStream& fh,int& sz);

enum GameOverStatus
{
	GAME_OVER_EXPLOSION = 1,
	GAME_OVER_INFERNAL_LOCKED,
	GAME_OVER_SPOBS_LOCKED,
	GAME_OVER_ALL_LOCKED,
	GAME_OVER_LUCKY,
	GAME_OVER_LAMPASSO,
	GAME_OVER_ABORT,
	GAME_OVER_NETWORK
};

extern int GameOverID;
extern char* GameOverText;

void XpeditionOFF(int type);
void FakeOfMight(void);

void GetNetworkGameTime(int& day,int& hour,int& min,int& sec);
