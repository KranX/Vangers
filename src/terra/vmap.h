#ifndef __TERRA__VMAP_H__
#define __TERRA__VMAP_H__

const uint H_POWER = MAP_POWER_X;
const uint H_SIZE = 1 << H_POWER;
const uint H2_SIZE = 2*H_SIZE;

extern uint V_POWER;
extern uint V_SIZE;

extern int* FloodLvl;
extern uchar FloodLEVEL;

extern double zMod_cycle;
extern double zMod_flood_level_delta;

struct vrtWorld {
	char* name;
	char* fname;
	
	vrtWorld(void){ name = fname = NULL; }
};

struct vrtNode { vrtNode* next; };

struct PrmFile;
struct vrtMap {
	uchar** lineT; //Масив с вокселями.
	uchar** lineTcolor; //Масив цветов(картинка) поверхности.

	int upLine,downLine;

	uchar* dHeap;
	int freeMax,freeTail;
	vrtNode* freeNodes;

	uchar* dHeap_c;
	int freeMax_c,freeTail_c;
	vrtNode* freeNodes_c;

	PrmFile* pFile;
	int maxWorld,cWorld;
	vrtWorld* wTable;

	uchar* changedT;
	uchar* keepT;

	XStream fmap,kmap;
	int foffset,offset;
	char* fileName;
	char* paletteName;
	char* scenarioName;
	char* fname,*kname;
	char* pname[2];

	int isCompressed;
	int* st_table;
	short* sz_table;
	uchar* inbuf;

	bool on;

#ifdef _SURMAP_
	XStream fsss;
	int* sssT;
#endif

		vrtMap(void);
		~vrtMap(void);

	void init(void);
	void finit(void);
	void load(const char* name,int nWorld);
	void reload(int nWorld);
	void release(void);
	void allocHeap(void);
	void lockHeap(void);
	void dump_terrain(void);
	void accept(int up,int down);
	void change(int up,int down);
	void request(int up,int down,int left, int right);
	void quant(void);
	void link(int up,int down,int d);
	void linkC(int up,int down,int d);
	void delink(int up,int down);
	void turning(int XSrcSize,int Turn,int cx,int cy,int xc,int yc,int xside,int yside);
	void scaling(int XSrcSize,int cx,int cy,int xc,int yc,int xside,int yside);
	//void scaling_old(int XSrcSize,int cx,int cy,int xc,int yc,int xside,int yside);
#ifndef _SURMAP_
	void scaling_3D(DBM& A,int H,int focus,int cx,int cy,int xc,int yc,int xside,int yside,int Turn);
	void SlopTurnSkip(int Turn,int Slop,int H,int F,int cx,int cy,int xc,int yc,int XDstSize,int YDstSize);
#endif
	//void draw_voxel(int TurnAngle,int SlopeAngle,int TurnSecX,int CX,int CY,int xc,int yc,int xside,int yside);

	void render(int up,int down);
	void screenRender(void);
	void increase(int up,int down);

	void updownSetup(void);
	void lockMem(void);

#ifdef _SURMAP_
	void flush(void);
	void refresh(void);
	void sssUpdate(void);
	void sssKill(void);
	void readLine(int n,uchar* p);
	void sssReserve(void);
	void sssRestore(void);
	void squeeze(void);
	
	void openMirror(void);
	void closeMirror(void);
#endif

	inline uchar* use(void);
	inline void unuse(int i);

	inline uchar* use_c(void);
	inline void unuse_c(int i);

	void analyzeINI(const char* name);
	void fileLoad(void);

	};

void vMapPrepare(const char* name,int nWorld = 0);

extern vrtMap* vMap;

inline int getDistX(int v0,int v1)
{
	int d = v0 - v1;
	int ad = abs(d);
	int dd = H_SIZE - ad;
	if(ad <= dd) return d;
	return d < 0 ? d + H_SIZE : d - H_SIZE;
}

inline int getDistY(int v0,int v1)
{
	int d = v0 - v1;
	int ad = abs(d);
	int dd = V_SIZE - ad;
	if(ad <= dd) return d;
	return d < 0 ? d + V_SIZE : d - V_SIZE;
}

inline int getDelta(int v0,int v1)
{
	return YCYCL(v0 - v1 + V_SIZE);
}

inline int getDeltaX(int v0,int v1)
{
	return XCYCL(v0 - v1 + H_SIZE);
}

inline double getDistX(double v0,double v1)
{
	double d = v0 - v1;
	double ad = fabs(d);
	double dd = H_SIZE - ad;
	if(ad <= dd) return d;
	return d < 0 ? d + H_SIZE : d - H_SIZE;
}

inline double getDistY(double v0,double v1)
{
	double d = v0 - v1;
	double ad = fabs(d);
	double dd = V_SIZE - ad;
	if(ad <= dd) return d;
	return d < 0 ? d + V_SIZE : d - V_SIZE;
}

const int MLNAMELEN = 15;

#endif