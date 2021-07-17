/*
		uniVang-STAND Header File
		(C)1997 by K-D Lab
		Author: K-D Lab::KranK, KoTo
*/

#define LOC_TIME 10

// количество основных миров
const int MAIN_WORLD_MAX = 4;
// общее количество миров включая секретные
const int WORLD_MAX = 10;
// количество основных биосов в Цепи
const int BIOS_MAX = 3;
// количество приказов в памяти с-вангеров
const int ORDER_V_MAX = 8;

const int GAME_MAX = 2;

// количество эмуКвантов в стэнде на одну секунду
const int EMU_MAX = 2;

//zNfo interesting constants

const int V_MAX_VANGER = 200;			// Количество участников гонки
const int V_PRICE = 400;			// управляет скоростью появления с-вангеров
const int V_SPEED_MAX = 12;		  // max скорость с-вангеров в неактивной зоне
const int V_SPEED_DOLLY = 2;		  // max скорость Куклы в неактивной зоне
const int V_PGAP_TIME = 40;		  // время нахождения c-вангерв в коридоре при переходе с мира на мир
const int V_AH_TIME = 1;			// время сбора нюхи
const int V_CIRT_Q = 7; 			// заглушка, количесво нюхи при сборе в любом месте
const int V_CIRT_R = 1<<12;	            // максимальный радиус распространения нюхи от куклы

const int V_LOCTRIP_SCORE = 2;			// rating: за местную поездку
const int V_LONGTRIP_SCORE = 5; 		// rating: за межмировую поездку
const int V_CIRTDELIVERY_SCORE = 10;	  // rating: за привоз нюхи
const int V_RACE_SCORE = 10;						// rating: за гонку
const int V_RACE_VANGER = 6;						// 

const int V_RANGER_MAX = 15;			// рэйтинг, переводящий c-вангера в другой статус (ex. ranger'а)

// параметры движения Кукол
const int DOLLY_TAIL_LEN = 10;
const int DOLLY_TAIL_PERIOD = 20;

const int uvsTreasureThief = 1;
const int uvsTreasureInShop = 2;
const int uvsTreasureInGamer = 3;
const int uvsTreasureInWait = 4;

// предекларации
struct uvsWorld;
struct uvsEscave;
struct uvsSpot;
struct uvsPassage;
struct uvsBunch;
struct uvsDolly;
struct uvsItem;
struct uvsVanger;
struct uvsContimer;
struct uvsShop;
struct uvsMechos;
struct uvsMechosType;
struct uvsItemType;
struct uvsActInt;
struct uvsTradeItem;
struct uvsCrypt;
struct uvsElement;
struct uvsMechosList;


struct EscaveEngine;
struct SpotEngine;
struct PassageEngine;

// some important externs
extern int CurrentWorld;
extern uvsWorld* WorldTable[WORLD_MAX];
extern uvsContimer ConTimer;

extern uvsMechosType** uvsMechosTable;
extern uvsItemType** uvsItemTable;

extern int MAX_ITEM_TYPE;	//  число инвентори
extern int MAX_MECHOS_TYPE;  //  число типов мехосов
extern uvsActInt* GMechos;
extern uvsActInt* GItem;
extern uvsActInt* GGamer;
extern uvsActInt* GGamerMechos;
extern uvsActInt* GTreasureItem;
//extern uvsMechosList* GGamerList;
extern int uvsKronActive;
extern int uvsCurrentWorldUnable;

/* --- Global Consts --- */

struct UVS_KRON_FLAG{
	enum{
		SPETLE	= 0x00000001,		//1
		FLY        = 0x00000002,	   //2	Обязательные 0000000F
		RITUAL = 0x00000004,		//4
		RAFFA  = 0x00000008,		//8

		ELIPOD = 0x00000010,		//16
		BEEBO  = 0x00000020,		//32	Хоть один должен быть выставлен 000000F0
		ZEEX     = 0x00000040,		//64
		ALIEN  = 0x000000080,

		ELEREC = 0x00000100,		//256
		PIP	     = 0x00000200,		//512	Если есть один должен быть выставлен 00000F00
		KERN      = 0x00000400,			//1024
		ZEEXL    = 0x00000800			//2048
	};
};

struct UVS_KRON_EVENT{
	enum{
		BEEB,//0
		GRIB,//1
		HIVE,//2
		CRYPT,//3
		DAMAGE,//4
		FLY_ONLINE,//5
		END_CYCLE,//6
		SHOT,//7
		WEAPON_ON,//8
		END_RACE,//9
		KILL_DOLLY,//10
		GOLD_BEEB,//11			золотой биб
		WHIRLPOOL,//12			водоворот
		SWAMP,//13				болото
		RECHARGE_SHEILD,//14
		BAD_CIRT,//15
		ARK_NOY//16				жуки арказнойные
	};
};

struct UVS_TABUTASK_WORK {
	enum {
		KILL,
		DEFENSE,
		GO_OUT,
		DELIVER,
		DELIVER_ONE,
		GO_WITHOUT_DAMAGE,
		GO_WITHOUT_GRIB,
		GO_FLY,
		CHANGE_CYCLE,
		SHOT_ANY,
		END_RACE_RAFFA,
		END_RACE,
		END_RACE_WEAPON,
		KILL_BEEB,
		KILL_GOLD_BEEB,
		DELIVER_OR,
		DELIVER_BAD_CIRT,
		ARK_NOY,
		END_CYCLE_IN_WATER,
		NONE
	};
};

struct UVS_TABUTASK_STATUS {
	enum {
		ACTIVE = 1,
		GOOD   =  2,
		BAD      = 4,
		SLEEP = 8,
		OK = 16
	};
};

struct UVS_GAME_ID {
	enum {
		ELEECH = 1,
		PIPETKA,
		KERNOBOO,
		WEEZYK
	};
};

struct UVS_CAR_TYPE {
	enum {
		RAFFA = 0,
		LIGHT,
		MICROBUS,
		ATW,
		TRACK,
		SPECIAL
	};
};

struct UVS_BUNCH_STATUS {
	enum {
		KILL_GAMER = 0x01,
		UNABLE = 0x02
	};
};

struct UVS_DOLLY_STATUS {
	enum {
		CATCH   = 0x01,
		UNABLE	= 0x02
	};
};

struct UVS_DOLLY_TYPE {
	enum {
		OWN   = 0x01,
		ALIEN = 0x02
	};
};

struct UVS_MECHOS_USED {
	enum {
		RACE = 0x01,
		HARVEST = 0x02,
		CARAVAN = 0x04,
		KILLER = 0x08,
		USES = 0x10
		};
	};

typedef int UvsObjectType;
struct UVS_OBJECT {
	enum {
		DOLLY = 1,
		VANGER,
		FLY_FARMER
		};
	};

struct UVS_TOWN {
	enum {
		SPOT = 0,
		ESCAVE = 1
		};
	};


typedef int UvsCryptType;
struct UVS_CRYPT {
	enum {
		DEVICE = 0x0001,
		AMMO = 0x0002,
		ARMO_LIGHT = 0x0006,
		ARMO_HEAVY = 0x0007,
		/*ARTEFACT = 0x0004,
		PART = 0x0008,*/

		USED = 0x0010
	};
};

typedef int UvsTargetType;
struct UVS_TARGET {
	enum {
		ESCAVE = 1,
		SPOT = 2,			//2
		PASSAGE = 3,	//3
		DOLLY = 4,		   //4
		VANGER = 5,		 //5
		ITEM = 6,			  //6
		FLY_FARMER = 7,//7
		BEEB = 8,		//8
		GRIB = 9,		//9
		RAFFA = 10,		//10
		HIVE = 11,			//11
		CRYPT = 12,		//12
		VANGERS = 13,	//13
		RACE = 14,			//14
		WHIRLPOOL = 15,
		SWAMP = 16,
		SHEILD = 17,
		NONE = 18
		};
	};

struct UVS_GAME_TYPE {
	enum {
		RACE = 0,
		HARVEST
		};
	};


typedef int UvsItemtType;
struct UVS_ITEM_STATUS {
	enum {
		ARTIFACT = 1,
		WEAPON,		//  2
		DEVICE,		//  3
		AMMO,		//  4
		GOODS,		//  5
		THING,		//  6
		TREASURE,	//  7
		MECHOS_PART		//8
		};
	};

typedef int UvsItemtType;
struct UVS_ITEM_TYPE {
	enum {
		MACHOTINE_GUN_LIGHT = 0,
		MACHOTINE_GUN_HEAVY,

		SPEETLE_SYSTEM_LIGHT,
		SPEETLE_SYSTEM_HEAVY,

		GHORB_GEAR_LIGHT,
		GHORB_GEAR_HEAVY,

		BEEBBANOZA_BLOCKADE,

		CRUSTEST_CANNON,
		AMPUTATOR,
		DEGRADATOR,
		MECHOSCOPE,
		
		TERMINATOR,
		TERMINATOR2,

			// Ammo...
		SPEETLE_SYSTEM_AMMO0,
		SPEETLE_SYSTEM_AMMO1,

		CRUSTEST_CANNON_AMMO,
		GLUEK,

		// Devices...		
		COPTE_RIG,			// Jumper...
		CUTTE_RIG,			// Sail device...
		CROT_RIG,			// Some device...

		RADAR_DEVICE,			
		LHARK_DEVICE,		// Some device...
	
		// Items...
		//UVS_ITEM_STATUS::THING
		CIRTAINER,
		CONLARVER,

		// Wares...
		NYMBOS,			// Eleepod's eggs...
		PHLEGMA,		// Incubator slime...
		HEROIN,
		SHRUB,
		POPONKA,
		TOXICK,
		TABUTASK,
		ELEEPOD,
		BEEBOORAT,
		ZEEX,

		// Cultic units...
		ELEECH,			// Small eleepod...
		KERNOBOO,
		PIPETKA,
		WEEZYK,

																				   
		// Artefacts...
		PROTRACTOR,
		MECHANIC_MESSIAH,
		FUNCTION83,
		SPUMMY,
		SECTOR,
		PEELOT,

		LEEPURINGA, 	// Statue...
		PALOCHKA, 		// Statue...
		PIPKA, 			// Statue...
		NOBOOL, 		// Statue...
		BOORAWCHICK, 	// Statue...
		PEREPONKA, 		// Statue...
		ZEEFICK, 		// Statue...
		DEAD_ELEECH,

		// Tokens...
		RUBBOX,

		SANDOLL_PART1,
		SANDOLL_PART2,

		LAST_MOGGY_PART1,
		LAST_MOGGY_PART2,

		QUEEN_FROG_PART1,
		QUEEN_FROG_PART2,

		LAWN_MOWER_PART1,
		LAWN_MOWER_PART2,

		WORMASTER_PART1,
		WORMASTER_PART2,

		TANKACID,
		DEAD_KERNOBOO,
		DEAD_PIPETKA,
		DEAD_WEEZYK
		};
	};

//zmod BOT
typedef int UvsVangerShapeType;
struct UVS_VANGER_SHAPE {
	enum {
		GAMER = 1,
		BUNCH_SLAVE,	//2
		ASSASIN,		//3
		RANGER,			//4
		THIEF,			//5
		KILLER,			//6
		CARAVAN,		//7
		GAMER_SLAVE,	//8
		TABUTASK,		//9
		NETWORK			//10
		};
	};

typedef int UvsVangerStatusType;
struct UVS_VANGER_STATUS {
	enum {
		ESCAVE_SLEEPING = 1,
		ESCAVE_WAITING,				//2
		SPOT_WAITING,
		SPOT_SLEEPING,				//4
		GAME_BEGIN_ESCAVE,
		GAME_BEGIN_SPOT,			//6
		GAME_HUNTER_BEGIN_ESCAVE,
		GAME_HUNTER_BEGIN_SPOT,		//8
		PASSAGE_GAP,
		AIM_HANDLE,					//10
		MOVEMENT,
		FREE_MOVEMENT,				//12
		WAIT_GAMER,
		RACE,						//14
		RACE_HUNTER,
		GATHERING,					//16
		SHOPPING,
		IN_SHOP,					//18
		GO_NEW_WORLD,
		GO_NEW_ESCAVE,				//20
		MOVE_TO_MECHOS,
		KILLED_BY_GAMER				//22
		};
	};

typedef int UvsOrderType;
struct UVS_ORDER {
	enum {
		NONE = 0,
		MOVING,
		HUNTING,
		CIRT_GATHERING
		};
	};

//zmod event list
typedef int UvsEventType;
struct UVS_EVENT {
	enum {
		DEATH = 1,
		GO_SPOT,		//2
		GO_ESCAVE,
		GO_PASSAGE,		//4
		GO_FROM_ESCAVE,
		GO_FROM_SPOT,	//6
		GO_AIM,
		GO_CIRT,		//8
		GO_KILL,
		GO_CORN,		//10
		GO_NEW_ESCAVE,
		SPOT_ARRIVAL,	//12
		ESCAVE_ARRIVAL,
		PASSAGE_ARRIVAL,//14
		AIM_ARRIVAL,
		THIEF,			//16
		THIEF_FROM_SHOP,
		GO_NEW_WORLD,	//18
		FATALITY
		};
	};

/* --------------------------------------- Aux Structs ------------------------------------- */
// обработка *.prm файлов
struct PrmFile {
	char* buf;
	int len;
	int index;

		PrmFile(void){ buf = NULL; len = index = 0;}
	void init(const char* name);			// загрузить файл на обработку
	char* getAtom(void);			// получить следующий атом - строку, разделенную с другими пустотой (или в кавычках)
	void finit(void) {
		delete[] buf;
	}
};

// организация двухсвязных списков производных объектов
// хвост->prev указывает на последний элемент, а последний->next равен NULL
struct listElem {
	listElem* next;
	listElem* prev;

	listElem(void){ next = prev = NULL;};
	virtual ~listElem() {}

	void link(listElem*& tail);		// добавляет объект (this) в список с хвостом на tail (обычно глобальный указатель) в конец
	listElem* seekName(const char* name);  // ищет по списку (this как хвост) name, сравнивая его со значением виртуального метода GetName()

	virtual char* GetName(void){ return NULL; }
};
/* --------------------------------------- Global Objects ------------------------------------- */

struct uvsTradeItem : listElem {
	int type;
	char* town_name;

	uvsTradeItem(void) : listElem() {type = 0; town_name = NULL;}
	~uvsTradeItem(void){ 
		if(town_name) delete[] town_name;
	}
};

struct uvsActInt : listElem{
	int pos_x, pos_y;
	int type;
	int price;
	int sell_price;
	int param1;
	int param2;

	uvsActInt(void) : listElem() { type = price = sell_price = param1 = param2 = 0; pos_x = pos_y = 0;}
	void delink(uvsActInt*&);

	void load( XStream&, int what = 0);
	void save( XStream&);
};

// учет глобального времени
struct uvsContimer {
	int counter;				// простой внутренний счетчик
	XBuffer sTime;				// internal
	int day,hour,min,sec;		// разбивка на производные от counter составляющие для вывода на экран

		uvsContimer( void ){ counter = day = hour = min = sec = 0;}

	void Start(void);// начать отсчет времени

	void load( XStream&);
	void save( XStream&);
	void Quant(void);		// прошла единица времени (условная секунда)
	char* GetTime(void);		// получить строку для вывода на экран в формате: "N day, nn:nn:nn"
	};

// некий объект/место на общей карте мира, куда можно задаться целью попасть или догнать
// самостоятельно не используется
struct uvsTarget {
	int pos_x,pos_y;
#ifdef _ROAD_
	UnitOrderType unitPtr;
#endif
		uvsTarget(void ) {pos_x = pos_y = 0;}
		virtual ~uvsTarget() {}

	};

// приказ для динамических объектов (ex. вангеров)
struct uvsOrder {
	UvsOrderType type;			// тип приказа, UVS_ORDER::?, если NONE - приказа нет
	UvsEventType event;			// событие, породившее приказ

	UvsTargetType  target;		//  вид обьекта
	uvsTarget* Ptarget;				// указатель на обьект

		// обнулятор
	uvsOrder(void){ type = UVS_ORDER::NONE; target = UVS_TARGET::NONE; Ptarget = NULL; event = UVS_EVENT::DEATH; }
	};

/* ------------------------------------- Stationary Objects ------------------------------------ */

// параметры периода из культового цикла
struct uvsCultGame {
	int GameType;								//  вид игры
	int typeTownBeg, typeTownEnd;			//  типы исходного и конечного места
	listElem* townBeg, *townEnd;				//  указатели на исходное и конечное места
	int GoodsTypeBeg, GoodsTypeEnd;		//  вид товара отпускаемого и принимаемого
	int GoodsTypeLast;												//  вид товара измененное
	int GoodsTypeBegCount, GoodsTypeEndCount;// количество товара отпускаемого и принимаемого
	int GoodsBegCount, GoodsEndCount;		// количество товара отпущеного и принятого
	int start;												//  флаг начала гонки
	int result;												//  текущий результат
	int total;												//  количество участников
	int flag, first, all, sucks;					// результаты игрока
	uchar* score;										//  результат заезда

		uvsCultGame( void ){GameType = typeTownBeg = typeTownEnd = GoodsTypeBeg = GoodsTypeEnd = GoodsTypeLast = GoodsTypeBegCount = GoodsTypeEndCount = GoodsBegCount =GoodsEndCount = result = total = flag = first = all = sucks = 0;
											start = 0;
											townBeg = townEnd = NULL;  score  = NULL;
		}

		~uvsCultGame(void){
			if (score) {
				delete[] score;
			}
		}
	uvsCultGame(PrmFile* pfile,char* atom);
};

struct uvsCultStage {
	char* name;				// имя цикла
	char* pal_name; 	//   имя файла палитры цикла
	int time;					//  время полураспада в мин
	int cirtMAX;				// количество нюхи (cirt), необходимое для завершения периода
	int cirtQ;				    //	количество данной нюхи "на складе"
	int priceQ;					//  коэффициент цен
	int GamerCirt;
	uvsDolly* Pdolly;		    // указатель на Куклу, управляющую этим периодом
	uvsCultGame* Pgame;	//  указатель на культовую игру

		uvsCultStage(void){ name  = pal_name = NULL; time = cirtMAX = cirtQ = 0; Pdolly = NULL; Pgame = NULL; priceQ = 100; GamerCirt = 0;}
		~uvsCultStage(void){
			free();
		}
		void free(void);
	};

// организация банчей - социумов Цепи
struct uvsBunch : listElem {
	uvsEscave* Pescave;			// указатель на эскэйв, в котором находится банч
	int biosNindex; 				// номер биоса, к которому относится банч: E-B-Z
	int cycleN;					// число периодов в цикле банча
	uvsCultStage* cycleTable;		//  таблица периодов банча размером cycleN
	int currentStage;				 // номер текущего периода
	int vngResource;				// ресурс, позволяющий нанимать вангеров
	int status;						//  состояние по отношению к игроку
	int id;

		// pfile - обработчик bunches.prm, atom - первая значимая строка
		uvsBunch(void) : listElem() {Pescave = NULL; biosNindex = cycleN = currentStage = vngResource = 0; cycleTable = NULL; status = 0; id = 0;}
		uvsBunch(PrmFile* pfile,char* atom);
		uvsBunch(XStream& pfile, PrmFile* pf,char* atom);
		~uvsBunch(void);

	void save( XStream&);
	void Quant(void);
	void QuantCirt(int);

	int nextCycle(void);
	int game_soon( char * );
	int game_now( char *, uvsCultGame*& );
	void begin_game( void );
	void begin_race( void );
	void begin_harvest( void );
	void end_game( void );
	void end_race( void );
	void end_harvest( void );

	int real_hunter_now(uvsVanger*);
	int game_exit( void );
	int game_wait_end( void );

	int addGOODS_any(uvsCultGame*);
	int bring_game_GOODS( char*, listElem*&  );
	int bring_race_GOODS( char*, listElem*&  );
	int bring_harvest_GOODS( char*, listElem*&  );

	char* GetCurrentStageName(void){ return cycleTable[currentStage].name; }
	// сколько на складе нюхи?
	int GetCirtQ(void){ return cycleTable[currentStage].cirtQ; }
	// привезли еще нюхи
	int addCirt(uvsItem*);
	void randVRes(void){ vngResource = RND(V_PRICE) + V_PRICE/2; }
	void Check(char*);
	};

// организация миров Цепи
struct uvsWorld : listElem {
	char* name;					// наименование
	int x_size,y_size;				// размеры
	int gIndex;						// глобальный номер мира
	int GamerVisit; 			//  количество посещений данного места

	uvsEscave** escT;			// таблица эскэйвов мира
	int escTmax;					// общее число эскэйвов
	uvsSpot** sptT; 				// таблица локаций мира
	int sptTmax;					// общее число локаций
	uvsPassage** pssT;			// таблица коридоров на мире
	int pssTmax;					// общее число коридоров
	listElem* Pitem;				//указатель на инвентори
	uvsElement *Panymal;	//указатель на двигающие обьекты
	int locked;				// внешний статус занятости прохода вангером

		// pfile - обработчик worlds.prm, atom - первая значимая строка
	uvsWorld(void):listElem(){ name = NULL; gIndex = x_size = y_size = GamerVisit = escTmax = sptTmax = pssTmax = locked = 0;
												escT = NULL; sptT = NULL; pssT = NULL; Pitem = NULL; Panymal = NULL;
	}
		~uvsWorld(void);
		uvsWorld(PrmFile* pfile,char* atom);
		uvsWorld(XStream& pfile);

		void save( XStream&);

	void Quant(void);
	virtual char* GetName(void){ return name; }
	void generate_item(int);
	void updateResource(void);
	void updateCrypt(void);
	void getItemFromCrypt( int );
	void remove_target(uvsTarget*, int);
	void ChangeGoodsInList(int);
	void ChangeTabuTaskInList(int, int);
	uvsTarget* get_anymal( int pos_x, int pos_y, int _type);


	// возвращает указатель на Куклу, производящую нюху для биоса biosInd, если она есть на этом мире
	// или создает таковую при ее отсутствии
	uvsDolly* generateDolly(int biosInd);
	// возвращает случ. spot мира
	uvsSpot* getRandSpot(void){ return sptT[RND(sptTmax)]; }
	// возвращает коридор, ведущий на aim
	uvsPassage* getPassage(uvsWorld* aim);
	};

// организация эскэйвов - городов Цепи
struct uvsEscave : uvsTarget, listElem {
	char* name;					// наименование
	int TradeCount; 				// количество производимых продуктов
	listElem* Ptrade;		//указатель на производимые продукты
	listElem* Pgood;		//указатель на ввозимые продукты
	uvsWorld* Pworld;			// указатель на мир-владелец
	uvsBunch* Pbunch;			// указатель на банч, живущий в эскэйве
	uvsShop* Pshop; 			// указатель на магазин
	listElem*	Pitem;				// указатель на сокровище
	int locked;				// внешний статус занятости прохода вангером
	int GamerVisit; 	//  количество посещений данного места

	int TabuTaskGood;
	int TabuTaskID;
	int TabuTaskCount;
	int lastTabuTaskGood;

	int PassagerStatistic[WORLD_MAX];
	int PassagerFrom;
	int PassagerWait;

		// pfile - обработчик escaves.prm, atom - первая значимая строка
	uvsEscave(void) : uvsTarget(), listElem() {
		VNG_DEBUG()<<"uvsEscave::uvsEscave(void) "<<this<<std::endl;
		name = NULL; Ptrade = Pitem = NULL; TradeCount = locked = GamerVisit = 0;
		Pworld = NULL; Pbunch = NULL; Pshop = NULL; Pgood = NULL; TabuTaskID = TabuTaskCount = TabuTaskGood = 0;
		PassagerFrom = 0; PassagerWait = 0; lastTabuTaskGood = 0;
	}

		~uvsEscave(void);
		uvsEscave(PrmFile* pfile,char* atom);
		uvsEscave(XStream& pfile);

		void save( XStream&);

	virtual char* GetName(void){ return name; }

	int GetCurrentStage(void){ return Pbunch -> currentStage; }
	char* GetCurrentStageName(void){ return Pbunch -> GetCurrentStageName(); }

	int may_go_away(uvsMechos*& Pm, int shape);
	int game_soon( void );
	int game_now( uvsCultGame*& );
	int real_hunter_now(uvsVanger*);

	int arrest( uvsVanger* );

	int bring_game_GOODS( listElem*&  );
	void add_goods_to_shop( void );
	void link_good(void);

	int ItemCoef( int );
	int SeekID( int );

	void QuantChange(int counter);
	};

// организация локаций - вспомогательных мест
struct uvsSpot : uvsTarget, listElem {
	char* name;					// наименование
	int TradeCount; 				// количество производимых продуктов
	listElem* Ptrade;		//указатель на производимые продукты
	listElem* Pgood;		//указатель на ввозимые продукты
	uvsWorld* Pworld;			// указатель на мир-владелец
	uvsShop* Pshop; 			// указатель на магазин
	listElem*	Pitem;				// указатель на сокровище
	int locked;				// внешний статус занятости прохода вангером
	int GamerVisit; 	//  количество посещений данного места

	int TabuTaskGood;
	int TabuTaskID;
	int TabuTaskCount;
	int lastTabuTaskGood;

	int PassagerStatistic[WORLD_MAX];
	int PassagerFrom;
	int PassagerWait;

		// pfile - обработчик spots.prm, atom - первая значимая строка
		uvsSpot(void) : uvsTarget(), listElem() { name = NULL; Ptrade = NULL; TradeCount = locked = GamerVisit = 0;
									   Pworld = NULL; Pshop = NULL; Pgood = NULL; Pitem = NULL; TabuTaskID = TabuTaskCount = TabuTaskGood = 0;
									   PassagerFrom = 0; PassagerWait = 0; lastTabuTaskGood = 0;
		}

		~uvsSpot(void);
		uvsSpot(PrmFile* pfile,char* atom);
		uvsSpot(XStream& pfile);

		void save( XStream&);

	virtual char* GetName(void){ return name; }

	void link_good(void);

	int may_go_away(uvsMechos*& Pm);
	int game_soon( void );
	int game_now( uvsCultGame*& );

	int real_hunter_now(uvsVanger*);
	int bring_game_GOODS( listElem*&  );
	void add_goods_to_shop( void );

	int ItemCoef( int );
	int SeekID( int );
	};

// организация коридоров - проходов между мирами
struct uvsPassage : uvsTarget, listElem {
	char* name;					// наименование
	uvsWorld* Pworld;		// указатель на мир-владелец
	uvsWorld* Poutput;		// указатель на мир-другой конец коридора
	int locked;				// внешний статус занятости прохода вангером
	//int world_locked;				// внешний статус занятости прохода вангером

		// pfile - обработчик passages.prm, atom - первая значимая строка
		uvsPassage(void) : uvsTarget(), listElem() { name = NULL; Pworld = NULL; Poutput = NULL; locked = 0;}

		~uvsPassage(void);
		uvsPassage(PrmFile* pfile,char* atom);
		uvsPassage(XStream& pfile);

		void save( XStream&);
		virtual char* GetName(void){ return name; }
	};

struct uvsShop : listElem {
	listElem *Pmechos;
	listElem *Pitem;
	int *Price;
	char *ItemHere;
	int locked;

		uvsShop(void): listElem(){Pmechos = NULL; Pitem = NULL; locked  = 0; Price = NULL; ItemHere = NULL; };
		uvsShop(XStream& pfile);
		~uvsShop(void);

		void save(XStream& pfile);
		void updateResource(void);
		void updateList(listElem*);

		void prepare_list_for_ActInt( uvsActInt*& Mechos, uvsActInt*& Item, int where);
		void get_list_from_ActInt( uvsActInt*& Mechos, uvsActInt*& Item);

		void addMechos(uvsMechos* am);
		void sellMechos(uvsMechos*& am, int type = 0);
		int  FindMechos(int);

		void addItem(uvsItem* am);
		void sellItem(uvsItem*& am, int& res);
};

struct uvsCrypt : listElem, uvsTarget {
	//char *name;
	int pos_z;
	UvsCryptType type;

		uvsCrypt(void): listElem(), uvsTarget(){type = 0; pos_z = 0;}
		uvsCrypt(int _x, int _y, int _z, int _type){ type = _type; pos_z = _z; pos_y = _y; pos_x = _x;}
		uvsCrypt( XStream& );
		//uvsCrypt(PrmFile* pfile);
		~uvsCrypt(void);

	void save( XStream& );
};

/* ------------------------------------- Dynamic Objects ------------------------------------ */

// организация двухсвязных списков объектов
// хвост->prev указывает на последний элемент, а последний->next равен NULL

//  параметры инвентори

struct uvsTabuTaskType : listElem {
	int status;					// активность задания
	int cash;					//  стоимость 
	int luck;					//  параметр удачи
	int cycle;					//  время в циклах
	int target;					//  тип-обьект задания
	int work_on_target;			//  цель для обьукта
	int item_number;			//  количество предметов
	int* item;					//  предмет дя задания
	int* item_count;				   //  количество для задания
	int* real_count;				   //  текущее количество для задания
	int param;					//  Дополнительный параметр
	int priority;					//  приоритет
	uvsTarget *Ptarget;			//  текущая цель
	uvsWorld* Pworld;				//  требуемый мир

	uvsTabuTaskType(PrmFile* pfile,char* atom);
	~uvsTabuTaskType(void);

	void activate(void);
	void activateVANGER(void);
	void activateDOLLY(void);

	int is_able(void);
	int checkCycleEvent(int, int&);
	int Erase(int, int&);
};

struct uvsItemType{
	char* name;					//  имя
	int type;						//  тип
	int SteelerTypeFull;		   //  тип по Steeler
	int SteelerTypeEmpty;		   //  тип по Steeler
	int size;						//  размер слота
	int count;					   //  количество на мир
	int param1, param2;		//  Начальное значение параметров
	int gamer_use;

		uvsItemType(void){ name = NULL; gamer_use = param1 = param2 = type = SteelerTypeFull = SteelerTypeEmpty = size = count = 0; }
		uvsItemType(PrmFile* pfile);
		uvsItemType(XStream& pfile);
		~uvsItemType(void){
			if (name) delete[] name;
		}

	void save(XStream& pfile);
};

// обьекты-инвентори
struct uvsItem : uvsTarget, listElem {
	int pos_z;
	int type;					// Тип инвентори - ссылка на таблицу
	int param1;
	int param2;

		uvsItem(void):uvsTarget(), listElem() { type = 0;  param1 = 0; param2 = 0; pos_z = 0;};

		uvsItem(int itemType, int x = 0, int y = 0){ type = itemType; pos_x = x; pos_y = y; 
										param1 = uvsItemTable[itemType]-> param1; 
										param2 = uvsItemTable[itemType]-> param2; pos_z = 0;
		};
		uvsItem(int x , int y , int z, int itemType){ type = itemType; pos_x = x; pos_y = y; 
									       param1 = uvsItemTable[itemType]-> param1; 
									       param2 = uvsItemTable[itemType]-> param2;  pos_z = z;
		};
		uvsItem(XStream& pfile);

	int ActIntType(void);
	int GetCoef(void);

	void save( XStream&);
	void delink(listElem*&);
	};

// обьекты-мехосы
struct uvsMechos : listElem{
	int type;					//  тип мехоса - ссылка на таблицу
	int color;					//  цвет мехоса
	int status;					//
	int teleport;				//zNfo//количество телепортаций 

		uvsMechos(void) : listElem(){ type = 0; color = 0;status = 0; teleport = 0; sort();}
		uvsMechos(int MechosType){ type = MechosType; color = 0;status = 0; teleport = 0; sort();};
		uvsMechos(XStream& pfile);

	void sort(void);
	void save(XStream& pfile);
	void delink(listElem*&);
};

//  параметры мехоса
struct uvsMechosType{
	char *name;						//  имя
	int type;							//  вид мехоса
	int constractor;				//  сбор мехоса по частям
	int price;							  //  цена
	int sell_price;							  //  цена
	int box[4];							//  вместимость( количество слотов данного вида)
	int gamer_use;						   //  использование игроком
	int gamer_kill;							//  убито игроком
	//int count;							//  количество на мир
	int MaxSpeed;
	int MaxArmor;
	int MaxEnergy;
	int DeltaEnergy;
	int DropEnergy;
	int DropTime;
	int MaxFire;
	int MaxWater;
	int MaxOxigen;
	int MaxFly;
	int MaxDamage;
	int MaxTeleport;

		uvsMechosType(void){ name  = NULL; constractor = price = sell_price = MaxSpeed  =MaxArmor = MaxEnergy = DeltaEnergy = DropEnergy = MaxFire = MaxWater = MaxOxigen = MaxFly = MaxTeleport = box[0] = box[1] = box[2] = box[3]= 0; 
						   gamer_use = gamer_kill = 0;
		}
		uvsMechosType(PrmFile* pfile);
		uvsMechosType(XStream& pfile);

void save(XStream& pfile);
};


struct uvsElement {
	uvsElement* enext;
	uvsElement* eprev;
	UvsObjectType type;				// UVS_OBJECT::?, тип объекта

	uvsElement(void){ enext = eprev = NULL; type = 0;}

	void elink(uvsElement*& tail);		// добавляет объект (this) в список с хвостом на tail (обычно глобальный указатель) в конец
	void eunlink(uvsElement*& tail);	// исключает из списка
	void delink(uvsElement*& tail);   //  добавляет обьект в список для удаления

	virtual void Quant(void){}
	virtual void emuQuant(void){}
	};

//  обьекты - фермеры

#define FLY_CORN_DELTA  512					//  количество на мире
#define FLY_FARMER_COUNT  5					//  количество на мире
#define FLY_FARMER_LIVE  5400					// время жизни на мире в квантах
#define FLY_FARMER_FIRST_TIME  200		//  время первой стадии
#define FLY_FARMER_CORN  20						//  мах количество семя на борту

struct uvsFlyFarmer : uvsTarget, uvsElement {
	uvsWorld* Pworld;				// указатель на мир-владелец
	int corn_type;
	int timer;
	int corn;
	int x_speed, y_speed;

	uvsFlyFarmer(int _t, uvsWorld* pw): uvsTarget(), uvsElement(){ 
				timer = 0; corn = FLY_FARMER_CORN; 
				corn_type = _t; type = UVS_OBJECT::FLY_FARMER;
				Pworld = pw;   pos_x = RND( pw -> x_size );
				pos_y = RND( pw -> y_size );  setSpeed();
	};
	uvsFlyFarmer( XStream& );

	virtual void Quant(void);
	virtual void emuQuant(void){};
	void setSpeed(void);
	void GrowCorn(void);				//  высаживание семя на землю

	void save( XStream& );
};

// объекты-Куклы
struct uvsDolly : uvsElement, uvsTarget, listElem {
	uvsWorld* Pworld;				// указатель на мир-владелец
	int gIndex;						// номер куклы
	int biosNindex; 				// номер биоса, "кормящегося" от данной Куклы
	int x_speed,y_speed;			// составляющие скорости
	int status;

	int tail_x[DOLLY_TAIL_LEN];		// internal for moving calx
	int tail_y[DOLLY_TAIL_LEN];		// internal
	int tail_cInd,tail_counter;		// internal

	uvsDolly(void): uvsElement(), uvsTarget(), listElem() {
		Pworld = NULL;
		gIndex = biosNindex = x_speed = y_speed = 0;
		status = 0;
		memset(tail_x, -1, sizeof(tail_x));
		memset(tail_y, -1, sizeof(tail_y));
	}
	uvsDolly(uvsWorld* pw,int biosInd);
	uvsDolly(XStream &);

	void save( XStream&);

	void setSpeed(void);			// устанавливает новый (случайный) вектор и модуль скорости, возвращает количество нюхи
	int getCirt(int x,int y);

	virtual void Quant(void);
	void ActiveQuant(void);
	};

// объекты-вангеры
struct uvsVanger : uvsElement, uvsTarget {
	int gIndex;							// номер вангера

	uvsMechos* Pmechos;					// указатель на мехос
	listElem* Pitem;					// указатель на инвентори
	uvsWorld* Pworld;					// указатель на мир-владелец
	uvsEscave* Pescave;					// привязанный на данный момент эскэйв
	uvsSpot* Pspot; 					// привязанный на данный момент спот
	uvsPassage* Ppassage;				// привязанный на данный момент коридор
	UvsVangerShapeType shape;			// показывает текущее состояние вангера из UVS_VANGER_SHAPE::?
	UvsVangerStatusType status;			// показывает текущий статус вангера из UVS_VANGER_STATUS::?
	uvsBunch* owner;					// если это раб, то owner указывает на банч-хозяин
	int biosNindex; 					// номер биоса данного c-вангера
	uvsOrder* orderT;					// таблица приказов
	int locTimer;						// используется для выжидания
	int rating;							// рейтинг вангера, определяет его ценность

	int speed;							// стэндовая скорость

	uvsVanger(void) : uvsElement(), uvsTarget(){ 
		Pmechos = NULL; 
		Pitem = NULL; 
		Pworld = NULL; 
		Pescave = NULL; 
		Pspot = NULL; 
		Ppassage = NULL;
		shape = status = biosNindex = locTimer = rating = speed = 0; 
		owner = NULL; 
		orderT = NULL;		
	}

	uvsVanger(uvsEscave* pe);
	uvsVanger(XStream&);
	~uvsVanger(void);

	void save( XStream&);

	virtual void Quant(void);
	virtual void emuQuant(void);					// эмулирует в стэнде внешние события

	void Event(UvsEventType ev);

	int isBunchReady(void){ return !owner -> vngResource; }
	int emptyContainer( uvsEscave* );

	void prepare_diagen_statistic(void);
	void prepare_list_for_ActInt( uvsActInt*&, uvsActInt*&, uvsShop*, int where );
	void get_list_from_ActInt( uvsActInt*&, uvsActInt*& );

	void start_race(void);


	void thief_from_escave( uvsEscave* );
	void thief_from_spot( uvsSpot* );
	void thief_from_shop( uvsShop* );

	int  prepare_killer(void);
	void prepare_caravan(void);
	void go_from_escave(void);
	void go_from_spot(void);
	void goCirt(void);
	void goDolly(void);
	void goHome(void);
	void goNewEscave(void); 				// смена эскэйва
	void go_to_GAMER( void );

	void addInventory(void);
	int addItem(uvsItem* pi, int w = 0);
	//int addItem(uvsItem* pi, int w = 1);
	void sell_treasure_Item( uvsShop* );
	void sell_move_Item( uvsShop*, int where );
	int makeJump(void);					// выйти из коридора на др. его конце, возвращает 1 если удачно, 0 - если выход занят
	void aimHandle(void);					// выполнить по прибытии на место задание
	void homeArrival();					// разгрузка
	void spotArrival();					// разгрузка
	int add_to_game( uvsCultGame* );
	int add_to_race( uvsCultGame* );
	int addGOODS(uvsCultGame*);

	int game_result( const char* );				//  результат в игре
	int race_result( uvsCultGame*, const char* );				//  результат в игре
	int harvest_result( uvsCultGame*, const char* );				//  результат в игре
	void break_harvest(void);										//  завершение гонки и получение приказа домой

	void update(int x,int y);				// внешний метод, сообщает о положении с-вангера
	void shiftOrders(void); 				// первый приказ выполнен
	// собрать нюху
	void gatherCirt(uvsDolly* dolly);
	void gatherCirt(int x, int y, int& p1, int& p2);
	int CurrentDolly(void);

	int GoodTabuTask(void);
	int newGoodTabuTask(void);
	int GoodWorldTabuTask(void);
	int CirtDelivery(void);
	int sellCirt(void);
	void _sellCirt_(void);
	void checkDolly(void);
	void _checkDolly_(void);
	int checkCycle(int stage);
	void set_target(uvsTarget*, int);
	void UpdateAllTarget(void);

	// построить цепочку приказов
	int buildWay(uvsWorld* fromW,uvsWorld* toW);
	// изменить рэйтинг (возможно и shape) вангера
	void changeRating(int val);
	void KillStatic(void);
	void destroy(int how = 0);

	void get_shop(int);
	void prepare_shop(int);

	void emuStep(void);					// эмулирует в стэнде движение
	void emuCheck(void);					// проверка на прибытие при эмуляции

#ifdef _ROAD_
	// устанавливает для UNITS текущий приказ
	 UvsTargetType getOrder(uvsTarget*&);
	int isActive(void){
		if ( status != UVS_VANGER_STATUS::RACE_HUNTER 
	  && status != UVS_VANGER_STATUS::RACE 
	  && status != UVS_VANGER_STATUS::MOVEMENT 
	  && status != UVS_VANGER_STATUS::FREE_MOVEMENT 
	  && status != UVS_VANGER_STATUS::GATHERING
	  && status != UVS_VANGER_STATUS::WAIT_GAMER
	  ) return 0;
		return ((CurrentWorld == Pworld -> gIndex) && uvsKronActive );
	}
#endif
};

struct uvsGamerResult{
	int game_result;
	int game_total;
	int game_elr_result;
	int game_ker_result;
	int game_pip_result;
	int game_zyk_result;
	int dead_eleech;
	int dead_eleech_now;
	int dead_weezyk;
	int get_eleech;
	int get_weezyk;
	int eleech;
	int nymbos;
	int phlegma;
	int heroin;
	int shrub;
	int poponka;
	int toxick;
	int pipetka;
	int kernoboo;
	int weezyk;
	int eleepod;
	int beeboorat;
	int zeex;
	int rubbox;
	int nymbos_total;
	int phlegma_total;
	int heroin_total;
	int shrub_total;
	int poponka_total;
	int toxick_total;
	int theif;
	int theif_on_world;
	int cirt_bad;
	int cirt_good;
	int dolly;
	int mechos_variant;
	int mechos_type;
	int vanger_kill;
	int key;
	int leepuringa;
	int earth_unable;
	int last_game_type;
	int luck;
	int dominance;
	int palochka;
	int pipka;
	int nobool;	
	int boorawchick;
	int pereponka;
	int zeefick;
	int protractor;
	int mechanic_messiah;
	int boot_sector;
	int peelot;
	int spummy;
	int fun83;
	int add_luck;
	int add_dominance;
	int dolly_cycle;
	int rubbox_count;
	int thief_leepuringa;
	int thief_palochka;
	int thief_pipka;
	int thief_nobool;	
	int thief_boorawchick;
	int thief_pereponka;
	int thief_zeefick;
	int passagerOk;
	int nymbos_bonus;
	int phlegma_bonus;
	int heroin_bonus;
	int shrub_bonus;
	int poponka_bonus;
	int toxick_bonus;
	int BoorawchickGoHimself;
	int unik_poponka;
	int nymbos_buy;
	int phlegma_buy;
	int heroin_buy;
	int shrub_buy;
	int poponka_buy;
	int toxick_buy;

	void Init( void );
	void LocalInit( void );

	void Save(XStream &pfile);
	void Load(XStream &pfile);
};

// объекты-раффа
struct uvsRaffa : uvsElement, uvsTarget {
	};


/* --- Global Functions Prototypes --- */
void uniVangPrepare(void);			// общая инициализация
void uniVangSave(XStream &pfile);
void uniVangLoad(XStream &pfile);
uvsWorld* getWorld(int n);			// возвращает указатель на мир по его глобальному номеру
void uvsQuant(void);				    // общий квант uvs-вычислений
void uvsWorldReload(int newW);		 // перегрузка в игре мира (генерит с-вангеров)
void uvsAddStationaryObjs(void);	   // добавляет все статические объекты в UNITS
void FreeList( listElem*& );					//  удаляет элементы списка
void FreeMechosList(uvsMechosList*& pe);
void FreeFarmer( uvsElement*& );					//  удаляет элементы списка
void FinishFirstShopPrepare(int);
void _FirstShopPrepare(void);
void FirstShopPrepare(int);
void WaitGameInTown(int);
void WaitEndGameInTown(int cWorld);
int uvsSetItemType(int type,int param1,int param2);

void uniVangDelete(void);

int aci_getPrice(uvsActInt *);
extern uvsGamerResult GamerResult;
extern int uvsCurrentCycle;
extern int uvsBunchNumber;

void uvsPrepareQuant(void);
void uvsCloseQuant(void);

void uvsCreateNewCrypt(int x, int y, int z, int type, int iWorld);
void uvsCreateNewItem(int x, int y, int z, int type, int iWorld);
void uvsCreateItem_in_Crypt(void);
int uvsCatchDolly(uvsDolly* pd);
int  uvsFreeDolly(int _x, int _y, int _index_);
int uvsgetDGdata(int code);
uvsVanger* uvsMakeNewGamerInEscave(uvsEscave* pe, int what = 1);
//void uvsMakeMechosSlaveList(void);
void uvsMakeGamerFromSlave(uvsVanger* pv);
void uvsSetTownName(char* _name_ );  //  NULL - no town to remove
void uvsChangeRaiting( void );
int uvsGenerateItemForCrypt( int type );
void uvsDomChangeFromItem( int type, int what, int where );
void uvsActIntSell( uvsActInt *pa);
void uvsActIntSellMechos( uvsActInt *pn);
int uvsChangeGoodsParam( int type, int& param1, int& param2, int);
int uvsChangeTabuTask( int& param1, int& param2, int type, int status);
char* uvsGetLarvaWorld(int n);
uvsPassage* GetPassage(int fromWID, int toWID);
int uvsGameRaceNumber(void);
int uvsGameRaceFinish(void);
int uvsPrepareItemToDiagen(int type, int param1, int param2, int& what, int& ID, char*& NameFrom, char*& NameTO);
void uvsChangeTownTabuTask(int type);
void uvsCheckKronIventTabuTask(int KronType, int Kroncount, int MYresult = 0, int MYitem = 0);
void uvsKronDeleteItem(int type, int param2, int param1);
int ActInt_param2(int InType);
uvsVanger* uvsCreateNetVanger(int CarType, int Color, int PassageIndex,int TownTabutaskID);
