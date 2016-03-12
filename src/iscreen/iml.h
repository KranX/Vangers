
typedef int MLAtype;
const int MLPREC = 16;
const int MLNAMELEN = 15;

#define CHTYPEINDEX0		0
#define CHTYPEINDEX1		10
#define CHTYPE0 	(CHTYPEINDEX0 << TERRAIN_OFFSET)
#define CHTYPE1 	(CHTYPEINDEX1 << TERRAIN_OFFSET)

void iregRender(int x0,int y0,int x1,int y1);

struct iMLFrame {
	int x0,y0;
	int sx,sy,sz,ss;
	int csd, cst;
	uchar* delta;
	uchar* terrain;
	uchar* c_delta;
	uchar* c_terrain;
	unsigned* signBits;
	int period;
	int surfType;

		iMLFrame(void){ clear(); }

	void clear(void){ period = 1; delta = terrain = c_delta = c_terrain = NULL; signBits = NULL; surfType = 99; 
					csd = cst = 0; sz = ss = sx = sy = 0;
					}
	void load(XStream& ff,int mode);
	int quant(int& step,MLAtype* alt,int dry,int render = 1,int fastMode = 0);
	int quantAbs(int& step,MLAtype* alt,int dry,int render = 1,int fastMode = 0);
	void start(MLAtype* alt);
	void Render(void){ iregRender(x0,y0,x0 + sx,y0 + sy); }
	};

const int MAX_KEYPHASE = 4;

#define MLM_RELATIVE	0
#define MLM_ABSOLUTE	1
#define MLM_REL2ABS	2

struct iMobileLocation {
	int maxFrame;
	int cFrame;
	iMLFrame* table;
	MLAtype* alt;
	int* steps;
	int mode;
	int altSx,altSy;
	int x0,y0,x1,y1;
	int frozen;
	int goPh,goSkip;
	int fastMode;
	int firstUse;
	int skipFrameFlag;

	int DryTerrain;
	int Impulse;
	int KeyPhase[MAX_KEYPHASE];

	char name[MLNAMELEN + 1];

		iMobileLocation(void){
			maxFrame = cFrame = 0;
			table = NULL;
			alt = NULL;
			steps = NULL;
			altSx = altSy = 0;
			frozen = 1;
			fastMode = 0;
			goPh = -1;
			DryTerrain = 1;
			Impulse = 0;
			KeyPhase[0] = 0;
			goSkip = 0;
			skipFrameFlag = 0;
			KeyPhase[1] = KeyPhase[2] = KeyPhase[3] = -1;
			memset(name,'\0',MLNAMELEN + 1);
			mode = MLM_RELATIVE;
			}
		~iMobileLocation(void);

	void load(char* fname);
	int quant(int render = 1);
	void setPhase(int n);
	int getCurPhase(void){ if(cFrame == maxFrame - 1) return 0; else return cFrame + 1; }
	void goPhase(int val){ goPh = val; if(val != -1) goSkip = 1; }
	void calcBounds(void);
	void setNull(void);
	};

void RegisterIML(char* fname,int id,int channel);
void ReleaseIML(void);
void HandlerIML(int code,int id,int val);
int QuantIML(void);
int CheckIML(int id,int x,int y,int frame = 0);
void PauseIMLall(void);
void ResumeIMLall(void);
int GetPhaseIML(int id);
int isActiveIML(int id);
void LockIML(int id);
void UnlockIML(int id);
int isLockedIML(int id);
int isInQueueIML(int ch_id,int ml_id);
void CleanQueueIML(int ch_id);

void SetFlushIML(int x,int y,int sx,int sy);
void dg_SendEvent(int code);

void ToolzerQuant(int x,int y);
