/*
	Dialogue Generator Header
	(C)1997 by K-D Lab, VANGERS Project
	Creator: K-D Lab::KranK
*/

struct DG_EXTERNS { enum {
	ELR_RESULT,        // если получил элика то возвращается номер которым ты приехал среди остальных                              
									// участников элирекции (наличие элика здесь не учитывать!!!), если не участвовал то 0
	ELR_TOTAL,          // количество учасников последней элирекции
	ELR_RATING,        // текущий рейтинг в элирекции (как раньше)

	KER_RESULT,        // если получил элика то возвращается номер которым ты приехал среди остальных                              
									// участников элирекции (наличие элика здесь не учитывать!!!), если не участвовал то 0
	KER_TOTAL,          // количество учасников последней элирекции
	KER_RATING,        // текущий рейтинг в элирекции (как раньше)

	PIP_RESULT,        // если получил элика то возвращается номер которым ты приехал среди остальных                              
									// участников элирекции (наличие элика здесь не учитывать!!!), если не участвовал то 0
	PIP_TOTAL,          // количество учасников последней элирекции
	PIP_RATING,        // текущий рейтинг в элирекции (как раньше)

	ZYK_RESULT,        // если получил элика то возвращается номер которым ты приехал среди остальных                              
									// участников элирекции (наличие элика здесь не учитывать!!!), если не участвовал то 0
	ZYK_TOTAL,          // количество учасников последней элирекции
	ZYK_RATING,        // текущий рейтинг в элирекции (как раньше)

	INV_BAD_CIRT,     // коды начинающиеся с INV_ возвращают количество предметов в инвентори на момент                                    
									 // попадания в эскейв, то есть отнимание придметов неучитывается!
	INV_GOOD_CIRT,
	INV_NYMBOS,
	INV_PHLEGMA,
	INV_HEROIN,
	INV_SHRUB,
	INV_POPONKA,
	INV_UNIQUE_POPONKA,
	INV_TOXICK,  
	INV_ELEECH,                   // живой элик - по нему я сам определяю успех мероприятия
	INV_DEAD_ELEECH,
	INV_PIPETKA,					//количество PIPETKA
	INV_KERNOBOO,				//количество KERNOBOO
	INV_WEEZYK, 				//количество WEEZYK
	INV_RUBBOX, 
	INV_BAD_LARVA, 
	INV_GOOD_LARVA,

	INV_MESSIAH,
	INV_PROTRACTOR,
	INV_FUNCTION83,
	INV_BOOTSECTOR,
	INV_PEELOT,
	INV_SPUMMY,

	INV_LEEPURINGA,
	INV_PALOCHKA,
	INV_BOORAWCHICK,
	INV_NOBOOL,
	INV_PIPKA,
	INV_PEREPONKA,
	INV_ZEEFICK,

	INV_ELEEPOD,
	INV_BEEBOORAT,
	INV_ZEEX,

	THIEF_LEEPURINGA,
	THIEF_PALOCHKA,
	THIEF_BOORAWCHICK,
	THIEF_NOBOOL,
	THIEF_PIPKA,
	THIEF_PEREPONKA,
	THIEF_ZEEFICK,

	HERE_LEEPURINGA,
	HERE_PALOCHKA,
	HERE_BOORAWCHICK,
	HERE_NOBOOL,
	HERE_PIPKA,
	HERE_PEREPONKA,
	HERE_ZEEFICK,

	KILLED_VANGERS,
	MECHOS_CHANGED,
	MECHOS_TYPE,
	CYCLE,
	BEEBS,
	LUCK,
	DOMINANCE,
	TABUTASKS,
	FRESH_TABUTASKS,
	WTABUTASKS,
	TRUE_PASSENGER,	
	BOORAWCHICK_RETURN,

	NYMBOS_DELIVERY,
	PHLEGMA_DELIVERY, 
	HEROIN_DELIVERY,
	SHRUB_DELIVERY,
	POPONKA_DELIVERY,
	TOXICK_DELIVERY,
	CIRT_DELIVERY,
	LARVA_DELIVERY,

	NYMBOS_BONUS,
	PHLEGMA_BONUS, 
	HEROIN_BONUS,
	SHRUB_BONUS,
	POPONKA_BONUS,
	TOXICK_BONUS,

	WEEZYK_ASSIGNMENT,
	LARVA_CYCLE,

	ZIGGER_ZAGGER,
	CART, 
	PUT_PUT,
	BOX, 
	RATTLE,
	HEAVY_IRON,
	STAR, 
	WHIZZER,
	HUSKER,
	COGGED_WIDGET,

	MAX
	}; };

#define DG_GET_NEXT_PHRASE	1000
#define DG_GET_QUERY_LIST	1001
#define DG_ENTER_COMMAND	1002
#define DG_GET_COMMAND		1003
#define DG_TEST_QUERY		1004
#define DG_SKIP_ALL			1005

#define DG_POPONKA_MAX	4
#define DG_ESCAVE_MAX	9
#define DG_SECRETW_MAX	6

#define DG_POPONKA	0
#define DG_TABUTASK	1
#define DG_PASSENGER	2
#define DG_FINAL		3

struct DiagenDispatcher;
struct dgRoom;

extern DiagenDispatcher* dgD;
//extern  int* dgLevel;

struct dgFile {
	char* buf;
	int len,index;
	int external;

		dgFile(char* fname,int _len = 0){ load(fname,_len); }
		dgFile(void){ buf = NULL; len = index = -1; external = 0; }
		~dgFile(void){ if(!external && buf) delete buf; }

	void load(char* fname,int _len = 0);
	char* getElement(int DualElements,int empty_available = 0);
	void free(void){ if(!external && buf) delete buf; len = index = -1; }
	void reset(void){ index = 0; }
	};

struct dgAtom {
	char* data;
	char* qlinks;
	int qlMax,qlPos,qlNum;

	char** variants;
	char** comments;
	int* vcodes;
	int nvariant;

	dgAtom* next;
	dgAtom* prev;
	
		dgAtom(void){ data = qlinks = NULL; next = prev = NULL; variants = comments = NULL; vcodes = NULL; nvariant = 0; }
		dgAtom(char* _data){ data = _data; qlinks = NULL; next = prev = NULL; variants = comments = NULL; vcodes = NULL; nvariant = 0; }
		~dgAtom(void){ if(variants){ delete variants; variants = NULL; nvariant = 0; } }

	void link(dgAtom*& tail);
	char* read(dgFile* pf,char* s);
	
	char* findQLfirst(void);
	char* findQLnext(void);
	
	int getVcode(char* vars,char*& cs);
	};

struct dgQuery {
	char* subj,*subj0,*subj1;
	dgAtom** answers;		// таблица ответов размером N
	int* levels;				//таблица уровней ответов
	int nanswer;				// общее количество возможных ответов
	int lindex;				// текущий индекс ответа в answers
	int mood;
	
	int qlInd;

	dgQuery* next;
	dgQuery* prev;

		dgQuery(void){ subj = subj0 = subj1 = NULL; answers = NULL; nanswer = 0; lindex = 0; mood = 0; }
		~dgQuery(void){ for(int i = 0;i < nanswer;i++) delete answers[i]; if(answers) delete answers; }

	void link(dgQuery*& tail);
	dgQuery* delink(dgQuery*& tail);
	char* findQLfirst(int ind){ return answers[qlInd = ind] -> findQLfirst(); }
	char* findQLnext(void){ return answers[qlInd] -> findQLnext(); }
	int getIndex(void);
	};

struct dgMolecule {
	char* name;
	dgFile* dgf;
	int mood;

	dgAtom* tail;
	dgAtom* curA;			// текущий атом при разговоре

	dgMolecule* next;
	dgMolecule* prev;

		dgMolecule(char* _name){ dgf = NULL; tail = curA = NULL; 
								next = prev = NULL; name = strdup(_name); 
								mood = 0; }
		~dgMolecule(void){ ::free(name); }

	void link(dgMolecule*& tail,int mode = 0);
	dgMolecule* delink(dgMolecule*& tail);
	void accept(dgFile* _dgf);
	void release(void){ tail = curA = NULL; }
	char* getPhrase(int noHandle);
	void goNext(void){ if(curA) curA = curA -> next; }
	dgAtom* getVariantsAtom(void){ if(curA) return curA -> variants ? curA : NULL; return NULL; }
	void reset(void){ curA = tail; }
	int getHandledPhrase(char* s);
	char* getVarPhrase(char* s);
	};

struct dgCell {
	int x,y;
	char* Name;
	int Type,isWaiting,isLooping;
	dgFile Access;
	dgFile PostCMD;
	dgFile StartCMD;
	dgRoom* owner;

		dgCell(dgRoom* r){ Name = NULL; Type = isWaiting = isLooping = -1; owner = r; }
	
	void read(dgFile* dgf);
	int isACCESS(void);
	int doCMD(int startup = 1);
	int analyzeACCESS(char* p);
	int expressACCESS(int endreq,char*& p,int& uplogic);
	};

struct DG_CELLSTATUS {
	enum {
		EMPTY = 0,			// незанятые клетки
		DORMANT,			// еще невовлеченные
		USED,					// уже отработавшие
		WAITING,			// активные, ждущие проверки
		OPENED,				// активные с положительным доступом
		EXPLODED,			// вовлеченные
		HALFLIFE			// ждущие окончания (repeatable = 1)
		};
	};

struct dgRoom {
	const char* roomName;
	const char* escaveNames[2];
	char* counsillorNames[2];
	char* worldNames[2];
	int bios,repeatable;

	dgCell** grid;
	uchar* status;
	uchar* backup_status;
	int gridSX,gridSY;
	int startX,startY;

	int cIndex;
	dgMolecule* currentM;

	dgMolecule* mtail;
	dgQuery* qAtail;
	dgQuery* qVtail;

	DiagenDispatcher* owner;
	dgRoom* next;
	dgRoom* prev;

	char* prefixQ,*postfixQ;

	int visitCounter;
	int comingCounter;

	inline dgCell* getGRID(int x,int y){ if(x < 0 || x >= gridSX || y < 0 || y >= gridSY) return NULL; return *(grid + y*gridSX + x); }
	inline int getSTATUS(int x,int y){ if(x < 0 || x >= gridSX || y < 0 || y >= gridSY) return DG_CELLSTATUS::EMPTY; return *(status + y*gridSX + x); }
	inline void setGRID(int x,int y,dgCell* pc){ if(x < 0 || x >= gridSX || y < 0 || y >= gridSY) return; *(grid + y*gridSX + x) = pc; }
	inline void setSTATUS(int x,int y,uchar val){ if(x < 0 || x >= gridSX || y < 0 || y >= gridSY) return; *(status + y*gridSX + x) = val; }

	void nextState(int x,int y);
	void explodeState(int x,int y);
	void backup(void){ memcpy(backup_status,status,gridSX*gridSY); }
	void restore(void){ memcpy(status,backup_status,gridSX*gridSY); }
	
		dgRoom(DiagenDispatcher* _owner){ roomName = NULL;
										escaveNames[0] = escaveNames[1] = NULL;
										counsillorNames[0] = counsillorNames[1] = NULL;
										worldNames[0] = worldNames[1] = NULL;
										mtail = NULL; owner = _owner; 
										grid = NULL; gridSX = gridSY = 0;
										startX = startY = 0; 
										status = backup_status = NULL; cIndex = -1;
										currentM = NULL; visitCounter = comingCounter = 0;
										prefixQ = postfixQ = NULL; 
										qAtail = qVtail = NULL;
										bios = 0; repeatable = 0;
										}

	void link(dgRoom*& tail);
	void read(dgFile* pf);
	void locateActiveMolecule(void);
	void acceptTEXT(void);
	void acceptDIL(void);
	void acceptQUERY(void);
	
	void startSession(void);
	void endSession(void);
	dgMolecule* getCurrentMolecule(void);
	int isEmptySession(void);
	void setCell(char* name,int value);
	void updateLevel(void);
	
	dgQuery* seekQall(char* _subj);
	dgQuery* seekQvisible(char* _subj);
	dgMolecule* seekM(const char* _name);

	void save(XStream& ff);
	void load(XStream& ff);

#ifdef DIAGEN_TEST
	void test(void);
#endif
	};

struct DiagenDispatcher {
	dgRoom* rtail;
	dgRoom* currentR;
	dgRoom* lastR;

	dgQuery* cqp;			// вспомогательная для first/next
	int cVind;			    // вспомогательный variant-индекс для first/next
	int endSessionLog;

	int eStatus,bStatus,zStatus,sStatus;

	dgAtom* varAtom;					// != NULL в случае, если текущий атом содержит в себе вопрос с вариантами ответа

	dgRoom* invR;

	char* CycleName[2][3][3];
	char* SecretWname[2][DG_SECRETW_MAX];

	DiagenDispatcher(void) {
		rtail = NULL; currentR = NULL; 
		varAtom = NULL;
		lastR = NULL;
		endSessionLog = 0;
		invR = NULL;
		eStatus = bStatus = zStatus = sStatus = 0;
	}

	void init(void);

	dgRoom* seekR(const char* _name,int onlyfirstchar = 0);
	void involveQ(dgAtom* atom);
	void restHandle(dgMolecule* m);

	char* getQprefix(void);
	char* getQpostfix(void);
	char* getQempty(void);
	char* getQend(void);
	char* getQblock(void);
	char* getQout(void);
	char* getQdead(void);

#ifdef DIAGEN_TEST
	void getStatus(void);
#endif

	void startSession(const char* rname);
	void endSession(void);
	void checkSession(const char* rname);
	char* getNextPhrase(void);
	char* getAnswer(char* subject);
	char* findQfirst(void);
	char* findQnext(void);
	int isEnd(void){ return endSessionLog; }

	void save(XStream& ff);
	void load(XStream& ff);

	char* getWorldName(char* name);
	char* getInvText(int type,int id,char* from = NULL,char* to = NULL);
};

/* ------------ Prototypes & Externs ----------------*/
void diagenPrepare(void);
void diagenEventHandle(int code);
