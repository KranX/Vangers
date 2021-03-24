#include "../global.h"
//#include <iniparser.h>

//#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>



#include "../3d/3d_math.h"
#include "../3d/3dgraph.h"

#include "vmap.h"
#include "world.h"
#include "render.h"
//#include "..\win32f.h"
//#include "cpuid.h"
#include "../palette.h"

#include "../zmod_common.h"

#include <iostream>


#ifdef _ROAD_
//#define FILEMAPPING
#endif

//#define LOWLEVEL_OUTPUT
#define DEPTH(u,y)	int(double(u)*double(ysize/2)/double(ysize/2 + y))

#ifdef _SURMAP_
#include "surmap/missed.h"
#define SESSION
#endif

/* ----------------------------- EXTERN SECTION ---------------------------- */
extern int* SI;
extern int* CO;
extern int RestoreLog,CGenLog,MobilityLog,InitLog,SSSLog,ROLog,EncodeLog,DirectLog,ShotLog,GlassLog,CutvmpLog,WHLog,ExclusiveLog;
extern int ForcedCompressed,ForcedUncompressed;
extern int ConvertLog;
extern int Verbose;
extern int DepthShow;
extern int alt_show;
extern int Quit;
extern int ViewY;
extern int MainRqCounter,AuxRqCounter;
extern int debug_view;

extern int NOISE_AMPL;
extern int DEFAULT_TERRAIN;

extern int TotalDrawFlag;
extern int RAM16;
extern const char* mapFName;

extern int NetworkON;
extern int zGameBirthTime;
extern int CurrentWorld;

/* --------------------------- PROTOTYPE SECTION --------------------------- */
void restore(void);
int LayerWrite(int n,uchar* p);
int LayerRead(int n,uchar** p);
void CUTVMP(void);
void LINE_render(int y);
void worldFree(void);
void RenderPrepare(void);
void InitSplay(XStream&);
void ExpandBuffer(unsigned char* InBuffer,unsigned char* OutBuffer);
void LoadPRM(void);
void CompressMap( char* name, int X_SHIFT, int Y_SHIFT );
void ClipboardInit(void);

#ifdef __HIGHC__
#define invturn _invturn
#define scale _scale
#define pscale _pscale
#endif

struct PrmFile {
	char* buf;
	int len;
	int index;

	void init(const char* name);
	char* getAtom(void);
	void finit(void) {
		delete[] buf;
	}
};
/* --------------------------- DEFINITION SECTION -------------------------- */
uint MAP_POWER_Y;
uint map_size_y;
uint clip_mask_y;
uint TOR_YSIZE;
uint V_POWER;
uint V_SIZE;
uint net_size;
uint PART_MAX;

vrtMap* vMap;
int KeepON = 0;

#ifdef _SURMAP_
#if defined(EXTSCREEN) || defined(POSTER) || defined(ACTINT)
int MAX_LINE = V_SIZE + 8;
#else
int MAX_LINE = 3000;
#endif
#else
int MAX_LINE = 3000; // 2050;
#endif

#ifdef SESSION
static const char* sssFName = "SESSION.V__";
static const char* sssFNameT = "SESSION.V_T";
static const char* sssFNameVPR = "SESSION.V_P";
static uchar* linebuf;
int sssUpdateLog = 1;
#endif

#ifdef _SURMAP_
int sssInuse;
int lastUpRq,lastDownRq;
#endif

int* FloodLvl;

static char* iniName;
static int MainQuantToggle = 1;
static int LockSuccess;

#ifdef FILEMAPPING
static uchar* mappingPtr;
#endif

uchar** SkipLineTable;
int preViewY;
std::string path_to_world; 

char* GetTargetName(const char* name)
{
	static int first = 1;
	static char namebuf[256];
	//std::cout<<"name:"<<name<<" mapFName:"<<mapFName<<std::endl;
	if(!name) { 
		first = 1;
		return NULL; 
	}
	if(first) {
		first = 0;
		path_to_world = name;
		int st = path_to_world.find_last_of('/');
		path_to_world = path_to_world.substr(0, st+1);
		return strdup(name);
		}
	
	std::string path;
	path = path_to_world + name;
	strcpy(namebuf, path.c_str());
	return namebuf;
}

int GetKeepSpace(void)
{ 
	PrmFile pFile;
	pFile.init(mapFName);
	int maxWorld = atoi(pFile.getAtom());
	if(maxWorld < 1) ErrH.Abort("Empty world list");
	int i;
	char* atom;
	char* buf = new char[256];
	XStream ff(0);
	int size = 0;
	for(i = 0;i < maxWorld;i++){
		atom = pFile.getAtom();
		atom = pFile.getAtom();
		if(!atom) ErrH.Abort("Null world path");
		GetTargetName(NULL);
		strcpy(buf,GetTargetName(atom));
		dictionary *dict_name = iniparser_load(buf);
		strcpy(buf,iniparser_getstring(dict_name,"Storage:File Name", NULL));
		iniparser_freedict(dict_name);
		strcat(buf,".vmt");
		if(ff.open(GetTargetName(buf))){
			size += ff.size();
			ff.close();
			}
		}
	delete[] buf;
	pFile.finit();
	GetTargetName(NULL);
	return size/1024/1024;
}

void KeepCleanUp(void)
{ 
	PrmFile pFile;
	pFile.init(mapFName);
	int maxWorld = atoi(pFile.getAtom());
	int i;
	char* atom;
	char* buf = new char[256];
	XStream ff(0);
	int size = 0;
	for(i = 0;i < maxWorld;i++){
		atom = pFile.getAtom();
		atom = pFile.getAtom();
		GetTargetName(NULL);
		strcpy(buf,GetTargetName(atom));
		dictionary *dict_name = iniparser_load(buf);
		strcpy(buf, iniparser_getstring(dict_name,"Storage:File Name", NULL));
		strcat(buf,".vmt");
		if(ff.open(GetTargetName(buf))){
			ff.close();
			remove(GetTargetName(buf));
			}
		}
	delete[] buf;
	pFile.finit();
	GetTargetName(NULL);
}

void YSetup(void)
{
	map_size_y = 1 << MAP_POWER_Y;
	clip_mask_y = map_size_y - 1;
	TOR_YSIZE = TOR_POWER*map_size_y;
	V_POWER = MAP_POWER_Y;
	V_SIZE = map_size_y;
#ifdef _SURMAP_
	MAX_LINE = V_SIZE + 2;
#endif

	QUANT = 1 << POWER;
	part_map_size_y = 1 << WPART_POWER;
	part_map_size = map_size_x << WPART_POWER;
	PART_MAX = map_size_y >> WPART_POWER;
	net_size = map_size_x*map_size_y/QUANT/QUANT;
	if(FloodLvl)
		delete FloodLvl;
	FloodLvl = new int[PART_MAX];
	//std::cout<<"Map size x:"<<map_size_x<<" y:"<<map_size_y<<std::endl;
	//std::cout<<"YSetup - V_POWER:"<<V_POWER<<" V_SIZE/map_size_y:"<<V_SIZE<<" map_size_x:"<<map_size_x<<std::endl;

#ifdef _SURMAP_
	worldPrepare();
#endif
}

void vMapInit(void)
{
	if(RAM16) MAX_LINE = 900;

	vMap -> init();
	Verbose = 0;

#ifdef SESSION
	linebuf = new uchar[H2_SIZE];
#endif
}

void vMapPrepare(const char* name,int nWorld)
{
#ifdef _ROAD_
	if(NetworkON)
#endif
		KeepON = 0;
	
	vMap = new vrtMap;
	vMap -> load(name,nWorld);

#ifdef _SURMAP_
	vMapInit();
#endif
}

vrtMap::~vrtMap(void)
{
	delete[] FloodLvl; FloodLvl = NULL;
	free(fileName);
	free(paletteName);
	free(fname);
	free(pname[0]);
	if(KeepON) free(kname);
	GetTargetName(NULL);

	if(isCompressed) {
#ifndef FILEMAPPING
		if(inbuf) { 
			delete[] inbuf; 
			inbuf = NULL; 
		}
#endif
		if(st_table) { 
			delete[] st_table; 
			delete[] sz_table; 
			st_table = NULL; 
			sz_table = NULL; 
		}
	}

	release();
	finit();

	delete[] dHeap; dHeap = NULL;
	delete[] dHeap_c; dHeap_c = NULL;

	delete[] lineT; lineT = NULL;
	delete[] lineTcolor;  lineTcolor = NULL;
	delete[] SkipLineTable; SkipLineTable = NULL;
	delete[] changedT; changedT = NULL;
	if(KeepON){ delete keepT; keepT = NULL; }

	delete[] freeNodes; freeNodes = NULL;
	delete[] freeNodes_c; freeNodes_c = NULL;
}

vrtMap::vrtMap(void)
: fmap(0), kmap(0)
{
	pFile = new PrmFile;
	cWorld = 0;
	inbuf = NULL;
	st_table = NULL; sz_table = NULL;

	lineT = NULL; lineTcolor = NULL;
	dHeap = NULL; dHeap_c = NULL;
	freeNodes = NULL; freeNodes_c = NULL;
	wTable = NULL;
	fileName = NULL; paletteName = NULL; scenarioName = NULL; fname = kname = NULL;
	pname[0] = NULL; pname[1] = NULL;
	changedT = NULL;
	keepT = NULL;
	on = false;
}

void vrtMap::lockHeap(void)
{
/*
#ifdef _ROAD_
	int ret1 = 0;
	int ret2 = 0;
	if(ExclusiveLog){
		ret1 = win32_VLock(dHeap,MAX_LINE*H2_SIZE);
		ret2 = win32_VLock(dHeap_c,MAX_LINE*H_SIZE);
		if(ret1 || ret2){
			XCon < "\nCan't lock RAM\n";
			XCon < "Error1 " <= ret1 < ":\n";
			XCon < win32_errmsg(ret1) < "\n";
			XCon < "Error2 " <= ret2 < ":\n";
			XCon < win32_errmsg(ret2) < "\n";
			}
		else
			LockSuccess = 1;
		}
#endif
*/
}

void vrtMap::allocHeap(void)
{
	dHeap = new uchar[MAX_LINE*H2_SIZE];
	dHeap_c = new uchar[MAX_LINE*H_SIZE];
}

void vrtMap::release(void)
{
#ifdef _ROAD_
	if(ExclusiveLog && LockSuccess){
		//WHAT is IT?
		//win32_VUnLock(dHeap,MAX_LINE*H2_SIZE);
		//win32_VUnLock(dHeap_c,MAX_LINE*H_SIZE);
		LockSuccess = 0;
		}
#endif
	fmap.close();
	if(KeepON && on){ 
		for(uint i = 0;i < V_SIZE;i++)
			if(lineT[i] && changedT[i]){
				keepT[i] = 1;
				kmap.seek(i*H2_SIZE,XS_BEG);
				kmap.write(lineT[i],H2_SIZE);
				}
		kmap.seek(V_SIZE*H2_SIZE,XS_BEG);
		kmap.write(keepT,V_SIZE);
		kmap.close();
		}
	on = false;
}

void vrtMap::init(void)
{
	UI_OR_GAME=0;
	//std::cout<<"vrtMap::init "<<"V_SIZE:"<<V_SIZE<<std::endl;
	memset(lineT = new uchar*[V_SIZE],0,V_SIZE*sizeof(uchar*));
	memset(lineTcolor = new uchar*[V_SIZE],0,V_SIZE*sizeof(uchar*));
	memset(SkipLineTable = new uchar*[V_SIZE],0,V_SIZE*sizeof(uchar*));
	memset(changedT = new uchar[V_SIZE],0,V_SIZE);

	freeNodes = new vrtNode[MAX_LINE];
	freeNodes_c = new vrtNode[MAX_LINE];

	allocHeap();

	freeMax = freeMax_c = MAX_LINE;
	freeTail = freeTail_c = 0;

	vrtNode* p;
	int i;
	for(i = 0,p = freeNodes;i < MAX_LINE - 1;i++,p++) p -> next = p + 1;
	p -> next = freeNodes;
	for(i = 0,p = freeNodes_c;i < MAX_LINE - 1;i++,p++) p -> next = p + 1;
	p -> next = freeNodes_c;

#ifdef SESSION
	if(!DirectLog){
		sssT = new int[V_SIZE];
		if(SSSLog){
			fsss.open(sssFNameT,XS_IN);
			fsss.read(sssT,V_SIZE*sizeof(int));
			fsss.close();
			fsss.open(sssFName,XS_IN);
			sssUpdate();
			fsss.close();
			LoadVPR(1);
			}
		else {
			memset(sssT,0xFF,V_SIZE*sizeof(int));
			if(!ROLog){
				fsss.open(sssFName,XS_OUT);
				fsss.close();
				}
			}
		if(!ROLog) fsss.open(sssFName,XS_IN|XS_OUT);
		}
#endif
	upLine = downLine = 0;
}

#ifdef _SURMAP_
#ifdef SESSION
void vrtMap::sssUpdate(void)
{
	if(DirectLog) return;
	if(Verbose) XCon < "\nSession updating...";
	uint i,j;
	int* p = sssT;
	for(i = 0,j = 0;i < V_SIZE;i++,p++,j += H2_SIZE)
		if(*p != -1){
			fsss.seek(*p,XS_BEG);
			fsss.read(linebuf,H2_SIZE);
			fmap.seek(foffset + j,XS_BEG);
			fmap.write(linebuf,H2_SIZE);
			offset = j + H2_SIZE;
			*p = -1;
			}

	remove(sssFNameT);
	fsss.close();
	fsss.open(sssFName,XS_OUT);
	fsss.close();
	fsss.open(sssFName,XS_IN|XS_OUT);
	sssInuse = 0;
}

void vrtMap::sssKill(void)
{
	if(DirectLog) return;
	uint i,j;
	int* p = sssT;
	for(i = 0,j = 0;i < V_SIZE;i++) *p++ = -1;
	if(!ROLog){
		remove(sssFNameT);
		fsss.close();
		fsss.open(sssFName,XS_OUT);
		fsss.close();
		fsss.open(sssFName,XS_IN|XS_OUT);
		}
	sssInuse = 0;
}

void vrtMap::sssReserve(void)
{
	int max = YCYCL(downLine + 1);
	int i = upLine;
	if(MAP_POWER_Y <= 11 && !RAM16) i = max = 0;
	do {
		changedT[i] = 1;
		i = YCYCL(i + 1);
	} while(i != max);
	flush();
	i = upLine;
	if(MAP_POWER_Y <= 11 && !RAM16) i = 0;
	do {
		changedT[i] = 1;
		i = YCYCL(i + 1);
	} while(i != max);
}

void vrtMap::sssRestore(void)
{
	int max = YCYCL(downLine + 1);
	int i = upLine;
	if(MAP_POWER_Y <= 11 && !RAM16) i = max = 0;
	do {
		readLine(i,lineT[i]);
		LINE_render(i);
		i = YCYCL(i + 1);
	} while(i != max);
}

void vrtMap::squeeze(void)
{
	if(MAP_POWER_Y > 10){
		if(getDistY(lastUpRq,upLine) > 0){
			delink(upLine,lastUpRq - 1);
			upLine = lastUpRq;
			}
		if(getDistY(downLine,lastDownRq) > 0){
			delink(lastDownRq + 1,downLine);
			downLine = lastDownRq;
			}
		}
}
#endif

void vrtMap::openMirror(void)
{
#ifdef _SURMAP_ROUGH_
	printf("WARN: openMirror is not emplimented\n");
#else
	if(!fmap.open(fname,XS_IN | XS_OUT)) ErrH.Abort("VMP not found");

	offset = 0;
	foffset = fmap.tell();

	dHeap = (uchar*)win32_VMPMirrorON(fmap.gethandler());
	if(!dHeap) ErrH.Abort("VMP Mirror Error");
	dHeap += foffset;
	uchar* p = dHeap;
	memset(lineT = new uchar*[V_SIZE],0,V_SIZE*sizeof(uchar*));
	memset(lineTcolor = new uchar*[V_SIZE],0,V_SIZE*sizeof(uchar*));
	for(uint i = 0;i < V_SIZE;i++,p += H2_SIZE) lineT[i] = p;
#endif
}

void vrtMap::closeMirror(void)
{
#ifndef _SURMAP_ROUGH_
	win32_VMPMirrorOFF(dHeap);
	delete lineT;
	delete lineTcolor;
	fmap.close();
#endif
}
#endif

void vrtMap::finit(void)
{
#ifdef SESSION
	if(!DirectLog){
		if(sssUpdateLog){
			sssUpdate();
			SaveVPR();
			}
		else {
			fsss.close();
			if(!ROLog){
				fsss.open(sssFNameT,XS_OUT);
				fsss.write(sssT,V_SIZE*sizeof(int));
				SaveVPR(1);
				}
			}
		fsss.close();
		}
#endif
#ifdef FILEMAPPING
	win32_FileMirror(0,0,1);
#endif
}

#ifdef _SURMAP_
void ConvertProcedure(char* name)
{
	XStream fin(name,XS_IN);
	char* outname = strdup(name);
	memcpy(outname + strlen(name) - 3,"CNV",3);

	XStream fout(outname,XS_OUT);

	fin.seek(512,XS_BEG);

	const int xsize = H_SIZE;
	const int ysize = V_SIZE;

	uchar* buf = new uchar[xsize];

	XCon < "\n";
	int i;
	for(i = 0;i < ysize;i++){
		fin.read(buf,xsize);
		fout.write(buf,xsize);
		fin.seek(xsize,XS_CUR);
		fin.read(buf,xsize);
		fout.write(buf,xsize);
		XCon < "\r" <= i;
		}
	XCon < "\n";

	delete[] buf;
	fin.close();
	fout.close();
}
#endif

void vrtMap::analyzeINI(const char* name)
{
	const char* version = "1.4";

	const char* secGlobal = "Global Parameters";
	const char* secStorage = "Storage";
	const char* secRender = "Rendering Parameters";
	const char* secPalette = "Dynamic Palette";
	const char* secCreation = "Creation Parameters";

	

	if(iniName)
		free(iniName);
	//HACK
	/*std::string sbuf = strdup(name), sbuf2;
	int startR = sbuf.find("TheChain");
	sbuf2 = sbuf.substr(startR, sbuf.size()-startR);*/
	iniName = strdup(name);

	XStream ff((char*)iniName,XS_IN);
	ff.close();
	std::cout<<"vrtMap::analyzeINI "<<name<<std::endl;
	dictionary *dict_name = iniparser_load(name);
	int val = atoi(iniparser_getstring(dict_name,"Global Parameters:Map Power X", NULL));
	if(val != MAP_POWER_X) ErrH.Abort("Incorrect X-Size");
	
	MAP_POWER_Y = atoi(iniparser_getstring(dict_name,"Global Parameters:Map Power Y", NULL));
	POWER = atoi(iniparser_getstring(dict_name,"Global Parameters:GeoNet Power", NULL));
	WPART_POWER = atoi(iniparser_getstring(dict_name,"Global Parameters:Section Size Power", NULL));
	MINSQUARE = 1 << atoi(iniparser_getstring(dict_name,"Global Parameters:Minimal Square Power", NULL));

	if(strcmp(version,iniparser_getstring(dict_name,"Storage:Version", NULL))) ErrH.Abort("Incorrect Storage Version");
	isCompressed = atoi(iniparser_getstring(dict_name,"Storage:Compressed Format Using", NULL));
#ifdef _SURMAP_
	if(ForcedCompressed) isCompressed = 1;
	if(ForcedUncompressed) isCompressed = 0;
#endif

	fileName = strdup(iniparser_getstring(dict_name,"Storage:File Name", NULL));
	paletteName = strdup(iniparser_getstring(dict_name,"Storage:Palette File", NULL));

	int tmax = iniparser_getint(dict_name,"Rendering Parameters:Terrain Max", 0);
	if((!tmax && TERRAIN_MAX != 8) || (tmax && tmax != TERRAIN_MAX)) ErrH.Abort("Incorrect Terrain Max");
	
	int i;
	{
		char* p = iniparser_getstring(dict_name,"Rendering Parameters:Begin Colors", NULL);
		XBuffer buf(p,128);
		for(i = 0;i < TERRAIN_MAX;i++)
			buf >= BEGCOLOR[i];
	}
	{
		char* p = iniparser_getstring(dict_name,"Rendering Parameters:End Colors", NULL);
		XBuffer buf(p,128);
		for(i = 0;i < TERRAIN_MAX;i++)
			buf >= ENDCOLOR[i];
	}

	pal_iter_init();
	PAL_MAX = iniparser_getint(dict_name,"Dynamic Palette:Terrain Number", 0);
	PAL_WAVE_TERRAIN = atoi(iniparser_getstring(dict_name,"Dynamic Palette:Wave Terrain", NULL));
	if(PAL_MAX > 0){
		{
			char* pp = iniparser_getstring(dict_name,"Dynamic Palette:Terrains", NULL);
			XBuffer b(pp,128);
			for(i = 0;i < PAL_MAX;i++)
				b >= PAL_TERRAIN[i];
		}
		{
			char* pp = iniparser_getstring(dict_name,"Dynamic Palette:Speeds", NULL);
			XBuffer b(pp,128);
			for(i = 0;i < PAL_MAX;i++)
				b >= PAL_SPEED[i];
		}
		{
			char* pp = iniparser_getstring(dict_name,"Dynamic Palette:Amplitudes", NULL);
			XBuffer b(pp,128);
			for(i = 0;i < PAL_MAX;i++)
				b >= PAL_AMPL[i];
		}
		{
			char* pp = iniparser_getstring(dict_name,"Dynamic Palette:Red", NULL);
			XBuffer b(pp,128);
			for(i = 0;i < PAL_MAX;i++)
				b >= PAL_RED[i];
		}
		{
			char* pp = iniparser_getstring(dict_name,"Dynamic Palette:Green", NULL);
			XBuffer b(pp,128);
			for(i = 0;i < PAL_MAX;i++) 
				b >= PAL_GREEN[i];
		}
		{
			char* pp = iniparser_getstring(dict_name,"Dynamic Palette:Blue", NULL);
			XBuffer b(pp,128);
			for(i = 0;i < PAL_MAX;i++) 
				b >= PAL_BLUE[i];
		}
		}

#ifdef _SURMAP_
	scenarioName = strdup(iniparser_getstring_surmap(dict_name,secCreation,"Build Scenario File"));
	MESH = atoi(iniparser_getstring_surmap(dict_name,secCreation,"Mesh Value"));
	NOISE_AMPL = atoi(iniparser_getstring_surmap(dict_name,secCreation,"Noise Amplitude"));
	DEFAULT_TERRAIN = atoi(iniparser_getstring_surmap(dict_name,secCreation,"Default Terrain Type")) << TERRAIN_OFFSET;
#endif
	iniparser_freedict(dict_name);
}

void vrtMap::fileLoad(void)
{
	analyzeINI(GetTargetName(wTable[cWorld].fname));
	YSetup();

	XBuffer buf;
	buf < fileName < (isCompressed ? ".vmc" : ".vmp");
	
	fname = strdup(GetTargetName(buf.GetBuf()));
	pname[0] = strdup(fname);
	memcpy(pname[0] + strlen(pname[0]) - 3,"vpr",3);
	if(KeepON) {
		kname = strdup(fname);
		memcpy(kname + strlen(kname) - 3,"vmt",3);
	}

	if(isCompressed){
		std::cout<<"Compressed format detected..."<<std::endl;
	}
}

//Удалил не используемый код - смотреть старые версии
void vrtMap::load(const char* name,int nWorld)
{
	on = true;
	pFile -> init(name);
	maxWorld = atoi(pFile -> getAtom());
	if(maxWorld < 1) {
		ErrH.Abort("Empty world list");
	}
	int i;
	char* atom;
	wTable = new vrtWorld[maxWorld];
	for(i = 0;i < maxWorld;i++) {
		atom = pFile -> getAtom();
		if(!atom) ErrH.Abort("Null world name");
		wTable[i].name = strdup(atom);
		atom = pFile -> getAtom();
		if(!atom) ErrH.Abort("Null world path");
		wTable[i].fname = strdup(atom);
	}
	pFile -> finit();

	if(nWorld >= maxWorld || nWorld < 0) {
		nWorld = 0;
	}

	cWorld = nWorld;

	fileLoad();

	if(!fmap.open(fname,XS_IN|XS_NOSHARING)) {
		ErrH.Abort("WorldData not found");
	}
	
	if(KeepON) {
		std::cout<<"KeepON"<<std::endl;
		memset(keepT = new uchar[V_SIZE],0,V_SIZE);
		int need = 0;
		if(kmap.open(kname,XS_IN)) {
			if(kmap.size() != (int)(V_SIZE*H2_SIZE + V_SIZE),XS_BEG) { 
				kmap.close(); need = 1;
			} else {
				kmap.seek(V_SIZE*H2_SIZE,XS_BEG);
				kmap.read(keepT,V_SIZE);
			}
		} else {
			need = 1;
		}
		if(need) {
			kmap.open(kname,XS_OUT|XS_NOSHARING/*|XS_NOBUFFERING*/);
			kmap.seek(V_SIZE*H2_SIZE,XS_BEG);
			kmap.write(keepT,V_SIZE);
			kmap.close();
			kmap.open(kname,XS_OUT|XS_NOSHARING);
		}
		kmap.close();
		//std::cout<<"KMAP2!!!!"<<std::endl;
		kmap.open(kname,XS_IN|XS_OUT|XS_NOSHARING/*|XS_NOBUFFERING*/);
	}

	if(st_table) {
		delete st_table;
		delete sz_table;
		st_table = NULL;
		sz_table = NULL; 
	}
	if(isCompressed) {
		inbuf = new uchar[H2_SIZE]; // возможно не нужен
		st_table = new int[V_SIZE];
		sz_table = new short[V_SIZE];
		std::cout<<"Get compressed tables for V_SIZE:"<<V_SIZE<<std::endl;
		for(uint i = 0;i < V_SIZE;i++) {
			fmap > st_table[i];
			fmap > sz_table[i];
			if(sz_table[i] > H2_SIZE)
				ErrH.Abort("Wrong compression");
		}
		InitSplay(fmap);
	}
	else {
		if(fmap.size() != (int)map_size_x*(int)map_size_y*2) {
			ErrH.Abort("Incorrect vmp-file size");
		}
	}

	offset = 0;
	foffset = fmap.tell();

	LoadVPR();
	RenderPrepare();

	//dump_terrain();
}

void vrtMap::reload(int nWorld)
{
	on  = true;
	if(nWorld >= maxWorld || nWorld < 0) ErrH.Abort("World Index out of range");
#ifdef _SURMAP_
	// TODO: delete this block
	// if(cWorld == nWorld) return;
#endif
	cWorld = nWorld;
	uint old_v_size = V_SIZE;

#ifdef _SURMAP_
	worldFree();
#endif
	delete[] FloodLvl;
	FloodLvl = NULL;
	free(fileName);
	free(paletteName);
	free(fname);
	free(pname[0]);
	GetTargetName(NULL);

	fileLoad();

#ifdef _ROAD_
#ifdef FILEMAPPING
	win32_FileMirror(0,0,1);
#endif
#endif

	if(V_SIZE != old_v_size){
		delete[] lineT;
		delete[] lineTcolor;
		delete[] SkipLineTable;
		delete[] changedT;
		memset(lineT = new uchar*[V_SIZE],0,V_SIZE*sizeof(uchar*));
		memset(lineTcolor = new uchar*[V_SIZE],0,V_SIZE*sizeof(uchar*));
		memset(SkipLineTable = new uchar*[V_SIZE],0,V_SIZE*sizeof(uchar*));
		memset(changedT = new uchar[V_SIZE],0,V_SIZE);
		if(KeepON){ delete keepT; memset(keepT = new uchar[V_SIZE],0,V_SIZE); }
		}
	else {
		memset(lineT,0,V_SIZE*sizeof(uchar*));
		memset(lineTcolor,0,V_SIZE*sizeof(uchar*));
		memset(SkipLineTable,0,V_SIZE*sizeof(uchar*));
		memset(changedT,0,V_SIZE);
		if(KeepON){ memset(keepT,0,V_SIZE); }
		}

	int i;
#ifdef _SURMAP_
	int exist = 1;
	if(!fmap.open(fname,XS_IN)) exist = 0;
	else fmap.close();
	if(!exist) ErrH.Abort("Can't load world,XERR_USER,-1,fname");

	if(ROLog)
		fmap.open(fname,XS_IN);
	else
		fmap.open(fname,XS_IN | XS_OUT);
#else
	if(!fmap.open(fname,XS_IN|XS_NOSHARING)) ErrH.Abort("WorldData not found");
	if(KeepON){
		int need = 0;
		if(kmap.open(kname,XS_IN)){
			if(kmap.size() != (int)(V_SIZE*H2_SIZE + V_SIZE),XS_BEG){
				kmap.close(); need = 1;
				}
			else {
				kmap.seek(V_SIZE*H2_SIZE,XS_BEG);
				kmap.read(keepT,V_SIZE);
				}
			}
		else need = 1;
		if(need){
			/*NEED?int h = _open(kname,_O_BINARY|_O_CREAT|_O_WRONLY,_S_IREAD|_S_IWRITE);
			if(_chsize(h,V_SIZE*H2_SIZE)){
				_close(h);
				remove(kname);
#if defined(RUSSIAN_VERSION) && !defined(GERMAN_VERSION)
				ErrH.Abort("Недостаточно места на жестком диске для работы в режиме сохранения всех изменеий!");
#else
				ErrH.Abort("Can't create KEEP File. Please, free more disk space for this feature working.");
#endif
				}
			_close(h);*/
			kmap.open(kname,XS_OUT|XS_NOSHARING/*|XS_NOBUFFERING*/);
			kmap.seek(V_SIZE*H2_SIZE,XS_BEG);
			kmap.write(keepT,V_SIZE);
			}
		kmap.close();
		//std::cout<<"KMAP1!!!!"<<std::endl;
		kmap.open(kname,XS_IN|XS_OUT|XS_NOSHARING/*|XS_NOBUFFERING*/);
		}
#endif

#ifdef FILEMAPPING
	mappingPtr = (uchar*)win32_FileMirror(fmap.gethandler(),fmap.size());
	if(!mappingPtr) ErrH.Abort("File Mapping Error");
#endif

	if(isCompressed){
#ifndef FILEMAPPING
		if(!inbuf) inbuf = new uchar[H2_SIZE];
#endif
		if(V_SIZE != old_v_size || !st_table){
			if(st_table) { 
				delete[] st_table;
				delete[] sz_table; 
			}
			st_table = new int[V_SIZE];
			sz_table = new short[V_SIZE];
			}
		for(i = 0;i < (int)V_SIZE;i++){
			fmap > st_table[i];
			fmap > sz_table[i];
			if(sz_table[i] > H2_SIZE)
				ErrH.Abort("Wrong compression");
			}
		InitSplay(fmap);
		}
	else
		if(fmap.size() != (int)map_size_x*(int)map_size_y*2) ErrH.Abort("Incorrect vmp-file size");

	offset = 0;
	foffset = fmap.tell();

	freeMax = freeMax_c = MAX_LINE;
	freeTail = freeTail_c = 0;

	vrtNode* p;
	for(i = 0,p = freeNodes;i < MAX_LINE - 1;i++,p++) p -> next = p + 1;
	p -> next = freeNodes;
	for(i = 0,p = freeNodes_c;i < MAX_LINE - 1;i++,p++) p -> next = p + 1;
	p -> next = freeNodes_c;

#ifdef SESSION
	if(!DirectLog){
		delete[] sssT;
		memset(sssT = new int[V_SIZE],0xFF,V_SIZE*sizeof(int));
		if(!ROLog){
			fsss.open(sssFName,XS_OUT);
			fsss.close();
			fsss.open(sssFName,XS_IN|XS_OUT);
			}
		}
#endif

	LoadVPR();
	RenderPrepare();

#ifdef _ROAD_
	if(MAP_POWER_Y <= 11)
		accept(0, V_SIZE - 1);
	else
		accept(ViewY - 100, ViewY + 100);
#else
	if(MAP_POWER_Y <= 11)
		accept(0,V_SIZE - 1);
	else {
		upLine = 1;
		downLine = 0;
		}
#endif
}

void vrtMap::increase(int up,int down)
{
	if(MAP_POWER_Y <= 11 && !RAM16) return;

	up = YCYCL(up);
	down = YCYCL(down);

	int d = getDistY(upLine,up);
	if(d > 0){
		if(isCompressed) linkC(upLine - 1,up,-1);
		else link(upLine - 1,up,-1);
		upLine = up;
		}

	d = getDistY(downLine,down);
	if(d < 0){
		if(isCompressed) linkC(downLine + 1,down,1);
		else link(downLine + 1,down,1);
		downLine = down;
		}
}


void vrtMap::dump_terrain() {
	std::cout<<"Start dump terrain H2_SIZE:"<<H2_SIZE<<" H_SIZE:"<<H_SIZE<<" V_SIZE:"<<V_SIZE<<std::endl;
	unsigned char *line_buffer = new unsigned char[H2_SIZE];
	unsigned int i = 0, i2 = 0;

	SDL_Palette *gray_pal = SDL_AllocPalette(256);
	for (i2 = 0; i2 < 256; i2++) {
		SDL_Color color = {(Uint8)i2, (Uint8)i2, (Uint8)i2, 255};
		SDL_SetPaletteColors(gray_pal, &color, i2, 1);
	}

	SDL_Surface *surface = SDL_CreateRGBSurface(0, H_SIZE, V_SIZE, 8, 0, 0, 0, 0);
	SDL_SetSurfacePalette(surface, gray_pal);
	SDL_Surface *surface2 = SDL_CreateRGBSurface(0, H_SIZE, V_SIZE, 8, 0, 0, 0, 0);
	SDL_SetSurfacePalette(surface2, gray_pal);
	unsigned char *sur_pos = (unsigned char *)surface->pixels;
	unsigned char *sur_pos2 = (unsigned char *)surface2->pixels;

	std::ofstream test_file;
	test_file.open("test_map.map", std::ios::binary);
	do {
			memset(line_buffer, 0, map_size_x);
			if(KeepON && keepT[i]){
				std::cout<<"KeepON && keepT[i]"<<std::endl;
				kmap.seek(i*H2_SIZE,XS_BEG);
				kmap.read(line_buffer, H2_SIZE);
			} else {
				fmap.seek(st_table[i], XS_BEG);
				fmap.read(inbuf, sz_table[i]);
				ExpandBuffer(inbuf,line_buffer);
			}
			test_file.write((const char*)line_buffer, H2_SIZE);
			//memcpy(sur_pos, line_buffer, H_SIZE);
			//First level
			for (i2=0;i2<H_SIZE;i2++) {
				if (*(line_buffer + H_SIZE + i2) & DOUBLE_LEVEL) {
					sur_pos[i2+1] = sur_pos[i2] = line_buffer[i2];
					i2++;
				} else {
					sur_pos[i2] = line_buffer[i2];
				}
			}
			//Doublelevel
			for (i2=0;i2<H_SIZE;i2++) {
				if (*(line_buffer + H_SIZE + i2) & DOUBLE_LEVEL) {
					i2++;
					sur_pos2[i2-1] = sur_pos2[i2] = line_buffer[i2];
				} else {
					sur_pos2[i2] = 0;
				}
			}
			sur_pos += H_SIZE;
			sur_pos2 += H_SIZE;
			i = YCYCL(i + 1);
	} while(i != V_SIZE-1);

	test_file.close();
	SDL_SaveBMP(surface, "test_map_h1.bmp");
	SDL_SaveBMP(surface2, "test_map_h2.bmp");
	SDL_FreeSurface(surface);
	SDL_FreeSurface(surface2);
	//delete[] line_buffer;
}

/*Функция загрузки карт высот и т.д.
Удалил закомментированный код, смотреть в svn.*/
void vrtMap::accept(int up,int down) 
{
	// std::cout<<"vrtMap::accept up:"<<up<<" down:"<<down<<std::endl;
	up = YCYCL(up);
	down = YCYCL(down);

	int max = YCYCL(down + 1);
	int i = up;
	uchar* p;
	int off;

	if(!isCompressed)
		do {
			freeMax--;
			off = i*H2_SIZE;
			p = use();
			if(KeepON && keepT[i]){
				kmap.seek(off,XS_BEG);
				kmap.read(p,H2_SIZE);
				}
			else {
				if(off != offset){
					offset = off + H2_SIZE;
					fmap.seek(foffset + off,XS_BEG);
					}
				else
					offset += H2_SIZE;
				fmap.read(p,H2_SIZE);
				}
			lineT[i] = p; //znfo lineT plain //загрузка
			lineTcolor[i] = use_c();
			LINE_render(i);
			i = YCYCL(i + 1);
		} while(i != max);
	else
		do {
			freeMax--;
#ifdef FILEMAPPING
			lineT[i] = p = use();
			lineTcolor[i] = use_c();
			ExpandBuffer(mappingPtr + st_table[i],p);
#else
			p = use();
			if(KeepON && keepT[i]){
				kmap.seek(i*H2_SIZE,XS_BEG);
				kmap.read(p,H2_SIZE);
			} else {
				fmap.seek(st_table[i],XS_BEG);
				fmap.read(inbuf,sz_table[i]);
				ExpandBuffer(inbuf,p);
			}

			lineT[i] = p; //znfo lineT compressed //загрузка
			lineTcolor[i] = use_c();
#endif
			LINE_render(i);
			i = YCYCL(i + 1);
		} while(i != max);

	upLine = up;
	downLine = down;
	preViewY = ViewY;
}

void vrtMap::lockMem(void)
{
	const int M2 = MAX_LINE*H2_SIZE;
	const int M = MAX_LINE*H_SIZE;

	uchar* p,t;
	int i,size;
	for(i = 0;i < 2;i++){
		p = dHeap;
		size = M2;
		while(size--){ t = *p; *p = 0; *p++ = t; }
		}
	for(i = 0;i < 2;i++){
		p = dHeap_c;
		size = M;
		while(size--){ t = *p; *p = 0; *p++ = t; }
		}
}

void vrtMap::another(int up,int down)
{
	up = YCYCL(up);
	down = YCYCL(down);

	delink(upLine,downLine);
	if(isCompressed) linkC(up,down,1);
	else link(up,down,1);

	upLine = up;
	downLine = down;
	preViewY = ViewY;
}

void vrtMap::change(int up,int down)
{
	int du,dd,req;
	up = YCYCL(up);
	down = YCYCL(down);

#ifdef _SURMAP_
	lastUpRq = up;
	lastDownRq = down;

	if(getDistY(up,downLine) > 0 || getDistY(upLine,down) > 0){
		another(up,down);
		return;
		}
#endif
	du = getDistY(upLine, up);
	dd = getDistY(downLine, down);
	req = MAX(du, -dd);
	// std::cout<<"vrtMap::change du:"<<du<<" dd:"<<dd<<" req:"<<req<<" up:"<<up<<" down:"<<down<<" upLine:"<<upLine<<" downLine:"<<downLine<<std::endl;
	if (up > down) {
		std::cout<<"vrtMap::change oposite order for request terrain"<<std::endl;
	}
	if (req > 0 && freeMax <= req + 1) {
		if (du < 0) {
			delink(upLine, upLine + req - 1);
			upLine = YCYCL(upLine + req);
		}
		if (dd > 0) {
			delink(downLine - req + 1, downLine);
			downLine = YCYCL(downLine - req);
		}
	}
	if (req < 0 && du < 0 && freeMax <= abs(du) + 1) {
		delink(upLine, up - 1);
		upLine = YCYCL(up);
	}
	if (req < 0 && dd > 0 && freeMax <= abs(dd) + 1) {
		delink(down + 1, downLine);
		downLine = YCYCL(down);
	}
	if (du > 0) {
		if (isCompressed) {
			linkC(up, upLine - 1, 1);
		} else {
			link(up, upLine - 1, 1);
		}
		upLine = up;
	}
	if (dd < 0) {
		if (isCompressed) {
			linkC(downLine + 1, down, 1);
		} else {
			link(downLine + 1, down, 1);
		}
		downLine = down;
	}
}

void vrtMap::updownSetup(void)
{
	uchar** lt = isCompressed ? lineTcolor : lineT;
	int i = ViewY;
	while(lt[i]) i = YCYCL(i - 1);
	upLine = YCYCL(i + 1);
	i = ViewY;
	while(lt[i]) i = YCYCL(i + 1);
	downLine = YCYCL(i - 1);
}

void vrtMap::request(int up,int down,int left, int right)
{
#ifdef _SURMAP_
	if(MAP_POWER_Y > 10)
#endif
		change(up,down);
}

void vrtMap::quant(void)
{
	const int D = 1;

	int center = YCYCL(upLine + getDelta(downLine,upLine)/2);
	int dl = getDistY(center,ViewY);

	int ytop = upLine;
	int ybottom = downLine;

	if(abs(dl) > D){
		dl /= 4;
		ytop -= dl;
		ybottom -= dl;
		}
	else {
		ytop -= D;
		ybottom += D;
		}

	ytop = YCYCL(ytop);
	ybottom = YCYCL(ybottom);

	while(getDelta(ybottom,ytop) >= MAX_LINE - 4){
		ytop = YCYCL(ytop + 1);
		ybottom = YCYCL(ybottom - 1);
		}

	change(ytop,ybottom);
}

inline uchar* vrtMap::use(void)
{
	uchar* p = dHeap + freeTail*H2_SIZE;
	freeTail = (freeNodes + freeTail) -> next - freeNodes;
	return p;
}

inline void vrtMap::unuse(int i)
{
	int m = (lineT[i] - dHeap)/H2_SIZE;
	(freeNodes + m) -> next = freeNodes + freeTail;
	freeTail = m;
}

inline uchar* vrtMap::use_c(void)
{
	uchar* p = dHeap_c + freeTail_c*H_SIZE;
	freeTail_c = (freeNodes_c + freeTail_c) -> next - freeNodes_c;
	return p;
}

inline void vrtMap::unuse_c(int i)
{
	int m = (lineTcolor[i] - dHeap_c)/H_SIZE;
	(freeNodes_c + m) -> next = freeNodes_c + freeTail_c;
	freeTail_c = m;
}

void vrtMap::link(int up, int down, int d)
{
	//std::cout<<"vrtMap::link"<<std::endl;
	if(MAP_POWER_Y <= 11 && !RAM16) return;

	up = YCYCL(up);
	down = YCYCL(down);
	std::cout<<"vrtMap::link up:"<<up<<" down:"<<down<<std::endl;

	uchar* p;
	int max = YCYCL(down + d), off;
	int i = up;
	do {
		if (!lineTcolor[i]) {
			if(freeMax <= 1) {
				std::cout<<"We have no more free space in terrain buffer"<<std::endl;
				return;
//				XBuffer buf;
//				buf < "up: " <= upLine < " down: " <= downLine < " size: " <= getDelta(downLine,upLine);
//				ErrH.Abort("Line Buffer overflow",XERR_USER,-1,buf.GetBuf());
			}
			freeMax--;
			lineTcolor[i] = (p = use());
#ifdef SESSION
			if(!LayerRead(i,&p))
				if(!DirectLog && sssT[i] != -1){
					fsss.seek(sssT[i],XS_BEG);
					fsss.read(p,H2_SIZE);
					lineT[i] = p;
					lineTcolor[i] = use_c();
					LINE_render(i);
					}
				else {
#endif
					off = i*H2_SIZE;
					if (KeepON && keepT[i]) {
						kmap.seek(off, XS_BEG);
						kmap.read(p, H2_SIZE);
					} else {
						if (off != offset) {
							offset = off + H2_SIZE;
							fmap.seek(foffset + off,XS_BEG);
						} else {
							offset += H2_SIZE;
						}
						fmap.read(p, H2_SIZE);
					}
					lineT[i] = p; //znfo lineT plain //догрузка
					lineTcolor[i] = use_c();
					LINE_render(i);
#ifdef SESSION
					}
#endif
			}
		i = YCYCL(i + d);
	} while(i != max);
}

void vrtMap::linkC(int up,int down,int d)
{
	if(MAP_POWER_Y <= 11 && !RAM16) {
		std::cout<<"vrtMap::linkC MAP_POWER_Y <= 11"<<std::endl;
		return;
	}
	up = YCYCL(up);
	down = YCYCL(down);
	if (up > down) {
		std::cout<<"vrtMap::change Wrong order for request terrain"<<std::endl;
	}
	// std::cout<<"vrtMap::linkC up:"<<up<<" down:"<<down<<std::endl;


	uchar* p;
	int max = YCYCL(down + d);
	int i = up;
	do {
		if(!lineTcolor[i]) {
			if(freeMax <= 1) {
				std::cout<<"We have no more free space in terrain buffer"<<std::endl;
				return;
//				XBuffer buf;
//				buf < "up: " <= upLine < " down: " <= downLine < " size: " <= getDelta(downLine,upLine);
//				ErrH.Abort("Line Buffer overflow",XERR_USER,-1,buf.GetBuf());
			}
			freeMax--;
#ifdef FILEMAPPING
			lineT[i] = p = use();
			lineTcolor[i] = use_c();
			ExpandBuffer(mappingPtr + st_table[i],p);
			LINE_render(i);
#else
			p = use();
			if(KeepON && keepT[i]) {
				kmap.seek(i*H2_SIZE,XS_BEG);
				kmap.read(p,H2_SIZE);
			} else {
				fmap.seek(st_table[i],XS_BEG);
				fmap.read(inbuf,sz_table[i]);
				ExpandBuffer(inbuf,p);
			}

//ZMOD 1.18 DYNAMIC WATER (there is one more in upper)
//ZMOD 1.20 fix
if (NetworkON && zMod_flood_level_delta!=0) {
	uchar* pa,*pc,*pf,*pa0,*pc0,*pf0;
	uchar type,lxVal,rxVal;
	pa0 = pa = p;
	pf0 = pf = pa0 + H_SIZE;

	if (zMod_flood_level_delta>0) { //если уровень повысился
		for(int x = 0;x < H_SIZE; x++, pa++, pf++)
			if (*(pa0 + x) <= FloodLEVEL) //заливаем все что ниже уровня
				*pf &= ~TERRAIN_MASK & ~OBJSHADOW; 
	
	} else { //если уровень понизился
		for(int x = 0;x < H_SIZE; x++, pa++, pf++)
			if (*(pa0 + x) > FloodLEVEL) //осушаем все что выше уровня 
				if(GET_TERRAIN(*pf) == WATER_TERRAIN_INDEX) //и только существующую воду
					*pf |= MAIN_TERRAIN;

	}
}

/*
	//znfo ficha DRY (there is one more in upper)
	uchar* pf = p + H_SIZE;
	for (int x=0;x<H_SIZE; x++,pf++) {
		if(GET_TERRAIN(*pf) == WATER_TERRAIN_INDEX) {
			*pf |= MAIN_TERRAIN_INDEX << TERRAIN_OFFSET;
			}
		}
*/

			lineT[i] = p; //znfo lineT compressed //догрузка
			lineTcolor[i] = use_c();
			LINE_render(i);
#endif
			}
		i = YCYCL(i + d);
	} while(i != max);
}

void vrtMap::delink(int up, int down)
{
	static int keeped = 0;
	if(MAP_POWER_Y <= 11 && !RAM16) return;
	up = YCYCL(up);
	down = YCYCL(down);
	// std::cout<<"vrtMap::delink up:"<<up<<" down:"<<down<<std::endl;

	int max = YCYCL(down + 1);
	int i = up;
	uchar* p;
	do {
		if(lineT[i]){
			freeMax++;
			p = lineT[i];
#ifdef _ROAD_
			if(KeepON)
#endif
			if(changedT[i]){
				changedT[i] = 0;
#ifdef _ROAD_
				keepT[i] = 1;
				if(p){
					kmap.seek(i*H2_SIZE,XS_BEG);
					kmap.write(p,H2_SIZE);
					if(++keeped > 1024){
						keeped = 0;
						kmap.seek(V_SIZE*H2_SIZE,XS_BEG);
						kmap.write(keepT,V_SIZE);
						}
					}
#else
				if(!LayerWrite(i,lineT[i]))
					if(!DirectLog){
						sssInuse = 1;
						if(sssT[i] == -1){
							fsss.seek(0,XS_END);
							sssT[i] = fsss.tell();
							fsss.write(lineT[i],H2_SIZE);
							}
						else {
							fsss.seek(sssT[i],XS_BEG);
							fsss.write(lineT[i],H2_SIZE);
							}
						}
					else {
						int m = i*H2_SIZE;
						if(m != offset){
							offset = m + H2_SIZE;
							fmap.seek(foffset + m,XS_BEG);
							}
						else
							offset += H2_SIZE;
						fmap.write(lineT[i],H2_SIZE);
						}
#endif
				}
			unuse_c(i);
			unuse(i);
			lineT[i] = NULL;
			lineTcolor[i] = NULL;
			}
		i = YCYCL(i + 1);
	} while(i != max);
}

#ifdef _SURMAP_
void vrtMap::refresh(void)
{
	if(isCompressed) return;

	int off;
	int max = YCYCL(downLine + 1);
	int i = upLine;
	if(MAP_POWER_Y <= 11 && !RAM16) i = max = 0;
	do {
		if(lineT[i]){
#ifdef SESSION
			if(!LayerRead(i,&lineT[i]))
				if(!DirectLog && sssT[i] != -1){
					fsss.seek(sssT[i],XS_BEG);
					fsss.read(lineT[i],H2_SIZE);
					}
				else {
#endif
					off = i*H2_SIZE;
					if(off != offset){
						offset = off + H2_SIZE;
						fmap.seek(foffset + off,XS_BEG);
						}
					else
						offset += H2_SIZE;
					fmap.read(lineT[i],H2_SIZE);
#ifdef SESSION
					}
#endif
			LINE_render(i);
			}
		i = YCYCL(i + 1);
	} while(i != max);
}

void vrtMap::flush(void)
{
	int max = YCYCL(downLine + 1);
	int m;
	int i = upLine;
	if(MAP_POWER_Y <= 11 && !RAM16) i = max = 0;
	do {
		if(lineT[i] && changedT[i]){
			changedT[i] = 0;
			if(!LayerWrite(i,lineT[i]))
				if(!DirectLog){
					sssInuse = 1;
					if(sssT[i] == -1){
						fsss.seek(0,XS_END);
						sssT[i] = fsss.tell();
						fsss.write(lineT[i],H2_SIZE);
						}
					else {
						fsss.seek(sssT[i],XS_BEG);
						fsss.write(lineT[i],H2_SIZE);
						}
					}
				else {
					m = i*H2_SIZE;
					if(m != offset){
						offset = m + H2_SIZE;
						fmap.seek(foffset + m,XS_BEG);
						}
					else
						offset += H2_SIZE;
					fmap.write(lineT[i],H2_SIZE);
					}
			}
		i = YCYCL(i + 1);
	} while(i != max);
}

void vrtMap::screenRender(void)
{
	//std::cout<<"vrtMap::screenRender"<<std::endl;
	int max = YCYCL(downLine + 1);
	int i = upLine;
	if(MAP_POWER_Y <= 11 && !RAM16) i = max = 0;
	do {
		changedT[i] = 1;
		i = YCYCL(i + 1);
	} while(i != max);

	if(MAP_POWER_Y <= 11 && !RAM16)
		WORLD_colcalc(0,V_SIZE - 1);
	else
		WORLD_colcalc(YCYCL(upLine + 1),downLine);
}

void vrtMap::readLine(int n,uchar* p)
{
	int off = n*H2_SIZE;
	if(off != offset){
		offset = off + H2_SIZE;
		fmap.seek(foffset + off,XS_BEG);
		}
	else
		offset += H2_SIZE;
	fmap.read(p,H2_SIZE);
}
#endif

#ifndef _SURMAP_
void LoadVPR(int ind)
{
	XStream ff(vMap -> pname[ind],XS_IN);
	if(ff.size() != (int)(2*4 + (1 + 4 + 4)*4 + 2*net_size + 2*POWER*4 + 2*PART_MAX*POWER*4 + PART_MAX*4)) ErrH.Abort("Incorrect VPR size");

	ff.seek(2*4 + 1*4,XS_BEG);

	ff.seek(2*4*4,XS_CUR);

	ff.seek(2*net_size + 2*POWER*sizeof(int) + 2*PART_MAX*POWER*sizeof(int),XS_CUR);
	ff.read(FloodLvl,PART_MAX*sizeof(unsigned));

	//ZMOD 1.18 DYNAMIC FLOOD LEVEL
	//ZMOD 1.21 fix
	zMod_flood_level_delta = 0;
	if (NetworkON) {
		// network cycled life of univang.
		// 1172609523 eq 2007-02-27 23:52 - starting point.
		double t = ((double)zGameBirthTime-1172609523.) / (60.*60.*24.);

		double period = 0.;
		double high_period = sin(M_PI/4);
		double low_period = -high_period;

		int dynamic_level = 0;
		double phase = 0.;
		double high_level = 0.3;
		double low_level = 0.3;
		double new_level = FloodLvl[0];

		switch(CurrentWorld){
			case WORLD_FOSTRAL:
				period = 4.;
				dynamic_level = 1;
				high_level = 0.8;
				low_level = 0.6;
				break;
			case WORLD_GLORX:
				period = 3.;
				dynamic_level = 1;
				high_level = 0.6;
				low_level = 0.6;
				break;
			case WORLD_NECROSS:
				period = 5.;
				dynamic_level = 1;
				high_level = 0.8;
				low_level = 0.6;
				break;
			case WORLD_XPLO:
				period = 6.;
				break;
			case WORLD_BOOZEENA:
				period = 8.;
				break;
			case WORLD_WEEXOW:
				period = 7.;
				dynamic_level = 1;
				break;
			case WORLD_THREALL:
				period = 10.;
				dynamic_level = 1;
				high_level = 0.6;
				low_level = 0.6;
				break;
			case WORLD_ARKONOY:
				period = 6.;
				phase = M_PI;
				break;
			default:
				period = 10.;
		};

		zMod_cycle = sin(t*2.*M_PI / period + phase);
		
		if (dynamic_level) {
			if (zMod_cycle > high_period) {
				zMod_flood_level_delta = SIGN(zMod_cycle)*(1+cos(t*2.*M_PI)) / 2;
				new_level = FloodLvl[0] * (1 + zMod_flood_level_delta*high_level);
				if (new_level>255) new_level=255.;
				FloodLvl[0] = (int)round(new_level);
			} else if (zMod_cycle < low_period) {
				zMod_flood_level_delta = SIGN(zMod_cycle)*(1+cos(t*2.*M_PI)) / 2;
				new_level = FloodLvl[0] * (1 + zMod_flood_level_delta*low_level);
				if (new_level<0) new_level=0.;
				FloodLvl[0] = (int)round(new_level);
			}
		}
	}

	ff.close();
}
#endif


//Посути первичная и основная функция рендринга
void vrtMap::scaling(int XSrcSize,int cx,int cy,int xc,int yc,int xside,int yside)
{
	char* vp = (char*)XGR_VIDEOBUF + (yc - yside)*XGR_MAXX + (xc - xside);
	int xsize = 2*xside;
	int ysize = 2*yside;
	int XADD = XGR_MAXX - xsize;

	cx = XCYCL(cx);
	cy = YCYCL(cy);

	int YSrcSize = ysize*XSrcSize/xsize;

	int k_xscr_x = (XSrcSize << 16)/xsize;
	int k_yscr_y = (YSrcSize << 16)/ysize;

	int tfx = (cx << 16) - (XSrcSize << 15) + (1 << 15);
	int x0 = tfx >> 16;
	int x1 = x0 + XSrcSize;
	int tfy = (cy << 16) - (YSrcSize << 15) + (1 << 15);
	int y0 = tfy >> 16;
	int y1 = y0 + YSrcSize;

	request(MIN(y0,y1) - MAX_RADIUS/2,MAX(y0,y1) + MAX_RADIUS/2,MIN(x0,x1) - 4,MAX(x0,x1) + 4);

#if defined(_ROAD_) && defined(_DEBUG)
	if(!TotalDrawFlag) return;
#endif

	int i,j,fx,fy;
	uchar** lt = lineT;
	uchar** ltc = lineTcolor;
	int YSrc;
	uchar* data;
	debug_view = 10;
		if(DepthShow)
			//@caiiiycuk: there is something wrong with clipping in XCYCL & YCYCL
			// in depth mode there are out of bounds mem reads. I think it's because
			// clip_mask is not calculated correctly, but why???
			for(i = 0;i < ysize;i++){
				YSrc = (ysize - 2*i)*XSrcSize/xsize;
				fx = (cx << 16) - DEPTH((XSrcSize << 0x10)/2,i) + (1 << 15);
				fy = (cy << 16) - DEPTH((YSrc << 0x10)/2,i) + (1 << 15);
				k_xscr_x = DEPTH((XSrcSize << 0x10)/xsize,i);
				data = ltc[YCYCL(fy >> 16)];
				for(j = 0;j < xsize;j++){
					*vp++ = *(data + XCYCL(fx >> 16));
					fx += k_xscr_x;
					}
				vp += XADD;
				}
		else
#if defined(_ROAD_) && defined(_DEBUG)
		   if(debug_view){
			static unsigned char old_pal[256*3];
			static unsigned char new_pal[256*3];
			if(debug_view == 1){
				XGR_GetPal(old_pal);
				int i,j = 64*3;
				for(i = 0;i < 64;i++)
					new_pal[j++] = new_pal[j++] = new_pal[j++] = i;
				for(i = 0;i < 64;i++){
					new_pal[j++] = new_pal[j++] = i;
					j++;
					}
				debug_view = 0;
				XGR_SetPal(new_pal,0,255);
				debug_view = 2;
				}
			if(debug_view == 5){
				debug_view = 0;
				XGR_SetPal(old_pal,0,255);
				debug_view = -3;
				 }
// 			std::cout<<"DOUBLE_LEVEL:"<<(unsigned int)DOUBLE_LEVEL<<" OBJSHADOW:"<<(unsigned int)OBJSHADOW<<std::endl;
			unsigned char* p;
			for(i = 0;i < ysize;i++){
				fx = tfx;
				fy = tfy;
				data = lt[YCYCL(fy >> 16)];
				for(j = 0;j < xsize;j++,vp++){
					p = data + XCYCL(fx >> 16);
// 					std::cout<<"D:"<<(unsigned int)*vp<<" H_SIZE:"<<H_SIZE<<" *(p + H_SIZE):"<<(unsigned int)*(p + H_SIZE)<<std::endl;
					switch(debug_view){
						case 10:
							*vp = (*p >> 2);
							break;
						case 2: //SHOW_HEIGHTS
							*vp = ((*(p + H_SIZE) & DOUBLE_LEVEL) ? 64 : 128) + (*p >> 2);
							break;
						case 3: //Floor
							*vp = 64 + (*((unsigned char*)((size_t)p & ~1)) >> 2);
							break;
						case 4: //Ceiling
							*vp = 128 + (*((unsigned char*)((size_t)p | 1)) >> 2);
							break;

						case -1: //SHADOW Bits
							*vp = *(p + H_SIZE) & OBJSHADOW ? 255 : *(ltc[YCYCL(fy >> 16)] + XCYCL(fx >> 16));
							break;
						case -2: //DOUBLE LEVEL Bits
							*vp = *(p + H_SIZE) & DOUBLE_LEVEL ? 255 : *(ltc[YCYCL(fy >> 16)] + XCYCL(fx >> 16));
							break;
						case -3: //!DOUBLE LEVEL Bits
							*vp = !(*(p + H_SIZE) & DOUBLE_LEVEL) ? 255 : *(ltc[YCYCL(fy >> 16)] + XCYCL(fx >> 16));
							break;
						case 0:
							*vp = *(ltc[YCYCL(fy >> 16)] + XCYCL(fx >> 16));
							break;
						}
					fx += k_xscr_x;
					}
				tfy += k_yscr_y;
				vp += XADD;
				}
			return;
			}
		else
#endif
		if(k_xscr_x == 1 << 16){
			fx = XCYCL(tfx >> 16);
			if(fx + xsize < H_SIZE)
				for(i = 0;i < ysize;i++){
					fy = tfy;
					data = ltc[YCYCL(fy >> 16)] + fx;
					memcpy(vp,data,xsize);
					tfy += k_yscr_y;
					vp += xsize + XADD;
					}
			else {
				int sz1 = H_SIZE - fx;
				int sz2 = fx + xsize - H_SIZE;
				for(i = 0;i < ysize;i++){
					fy = tfy;
					data = ltc[YCYCL(fy >> 16)];
					memcpy(vp,data + fx,sz1);
					memcpy(vp + sz1,data,sz2);

					tfy += k_yscr_y;
					vp += xsize + XADD;
					}
				}
			return;
			}
#ifdef LOWLEVEL_OUTPUT
			scale(xsize,ysize,tfx,tfy,ltc,vp,k_xscr_x,k_yscr_y,XADD);
#else
			for(i = 0; i < ysize; i++) {
				fx = tfx;
				fy = tfy;
				data = ltc[YCYCL(fy >> 16)];
				for(j = 0;j < xsize;j++,vp++) {
					*vp = *(data + XCYCL(fx >> 16));
					fx += k_xscr_x;
				}
				tfy += k_yscr_y;
				vp += XADD;
			}
#endif
}

static int* LineTable = 0;
static int LineTableLenght;
void calcLineTable(int curr_lenght,int k_vu,int base_step,int up_step)
{
	if(!LineTable){
		LineTableLenght = 1024;
		if(!(LineTableLenght & 1))
			LineTableLenght++;
		memset(LineTable = new int[LineTableLenght],0,sizeof(int)*LineTableLenght);
		}
	int fv = (-k_vu)*curr_lenght/2 + (1 << 15);
	int old_v,v;
	int v_diff;

	int* c = LineTable;
	old_v = (fv >> 16)*up_step;
	for(int u = 0;u <= curr_lenght;u++){
		fv += k_vu;
		v = (fv >> 16)*up_step;
		v_diff = v - old_v + base_step;
		old_v = v;
		*c++ = v_diff;
		}
}

#define GetCornerX(x,y) (((x)*cosTurn - (y)*sinTurn)/4)
#define GetCornerY(x,y) (((x)*sinTurn + (y)*cosTurn)/4)
#define GetCornerX1(x,y) (((x)*cosTurn - (y)*sinTurn)/2 + (1 << 15));
#define GetCornerY1(x,y) (((x)*sinTurn + (y)*cosTurn)/2 + (1 << 15));

#define CURR_COLOR *d

#ifndef TURN_TEST
	#define OptionalParameter 0
#endif

void vrtMap::turning(int XSrcSize,int Turn,int cx,int cy,int xc,int yc,int XDstSize,int YDstSize)
{
	// std::cout<<"vrtMap::turning XSrcSize:"<<XSrcSize<<" Turn:"<<Turn<<" cx:"<<cx<<" cy:"<<cy<<" xc:"<<xc<<" yc:"<<yc<<" XDstSize:"<<XDstSize<<" YDstSize:"<<YDstSize<<std::endl;


//#define SLOW_FLOAT_TURNING
#define SLOW_INT_TURNING
//#define OLD_TURNING

#if defined SLOW_FLOAT_TURNING
	// NOTE(amdmi3): this stock implementation is fast, but
	// buggy, this shit is still to reverse-enginner and rewrite
	//
	// My dump FPU implementation is noteceably slower, but should
	// be ok as a temporary fix; maybe this can be faster if
	// implemented as fixed point, but this likely is cache issue
	// as well (should read by lines!)

	// XXX: this also still lacks zoom support
	int YSrcSize = YDstSize*XSrcSize/XDstSize;
	float sina = sin((float)Turn/(float)PI*M_PI);
	float cosa = cos((float)Turn/(float)PI*M_PI);

	int edge_y1 = (int)(cy - sina*XSrcSize - cosa*YSrcSize);
	int edge_y2 = (int)(cy + sina*XSrcSize + cosa*YSrcSize);
	int edge_y3 = (int)(cy + sina*XSrcSize - cosa*YSrcSize);
	int edge_y4 = (int)(cy - sina*XSrcSize + cosa*YSrcSize);

	request(MIN(MIN(MIN(edge_y1,edge_y2),edge_y3),edge_y4) - MAX_RADIUS/2,
			MAX(MAX(MAX(edge_y1,edge_y2),edge_y3),edge_y4) + MAX_RADIUS/2, 0, 0);

	int x, y;
	for (y = -YDstSize; y < YDstSize; y++) {
		for (x = -XDstSize; x < XDstSize; x++) {
			char *dst = (char*)XGR_VIDEOBUF + (yc + y)*XGR_MAXX + (xc + x);

			float dx = (float)x*cosa - (float)y*sina;
			float dy = (float)x*sina + (float)y*cosa;

			int srcx = cx + (int)dx;
			int srcy = cy + (int)dy;

			uchar *srcline = vMap->lineTcolor[(srcy) & clip_mask_y];
			uchar *src = &srcline[(srcx) & clip_mask_x];

			*dst = *src;
		}
	}
#endif
	// NOTE(stalkeg): rewrite function of AMDmi3 with fixed point
	// and bit shift + normal scaling.
#if defined SLOW_INT_TURNING
	int YSrcSize = YDstSize*XSrcSize/XDstSize;
	int sina = sinTurn = SI[rPI(Turn)];
	int cosa = cosTurn = CO[rPI(Turn)];
	int fx,tfx,tfy;
	tfx = (cx << 16) - (XSrcSize*cosTurn - YSrcSize*sinTurn)/2 + (1 << 15);
	int vv0 = XSrcSize*sinTurn;
	int vv1 = YSrcSize*cosTurn;
	int v0 = (vv0 + vv1) >> 1;
	int v1 = (vv0 - vv1) >> 1;
	int vcy = cy << 16;
	
	tfy = vcy - v0 + (1 << 15);
	int y0 = tfy >> 16;
	int y1 = (vcy + v0) >> 16;
	int y2 = (vcy - v1) >> 16;
	int y3 = (vcy + v1) >> 16;

	request(MIN(MIN(MIN(y0,y1),y2),y3) - MAX_RADIUS/2,
			MAX(MAX(MAX(y0,y1),y2),y3) + MAX_RADIUS/2,0,0);
	
	int x, y, srcx, srcy;
	
	char *dst;
	uchar *src, *srcline;
	float d_size;
	d_size = ((float)YSrcSize)/((float)YDstSize*2);
	if (d_size!=1) {
		XSrcSize*=0.5;
		YSrcSize*=0.5;
		float src_x, src_y;
		for (y = -YDstSize, src_y = -YSrcSize; y < YDstSize; ++y, src_y += d_size) {
			for (x = -XDstSize, src_x = -XSrcSize; x < XDstSize; ++x, src_x += d_size) {
				dst = (char*)XGR_VIDEOBUF + (yc + y)*XGR_MAXX + (xc + x);

				srcx = cx + ((((int)src_x)*cosa - ((int)src_y)*sina)>>16);
				srcy = cy + ((((int)src_x)*sina + ((int)src_y)*cosa)>>16);

				srcline = vMap->lineTcolor[(srcy) & clip_mask_y];
				src = &srcline[(srcx) & clip_mask_x];

				*dst = *src;
			}
		}
	} else {
		for (y = -YDstSize; y < YDstSize; ++y) {
			for (x = -XDstSize; x < XDstSize; ++x) {
				dst = (char*)XGR_VIDEOBUF + (yc + y)*XGR_MAXX + (xc + x);

				srcx = cx + ((x*cosa - y*sina)>>16);
				srcy = cy + ((x*sina + y*cosa)>>16);

				srcline = vMap->lineTcolor[(srcy) & clip_mask_y];
				src = &srcline[(srcx) & clip_mask_x];

				*dst = *src;
			}
		}
	}
#endif
#if defined OLD_TURNING
	char* vp = (char*)XGR_VIDEOBUF + (yc - YDstSize)*XGR_MAXX + (xc - XDstSize);
	char* vpp;
	XDstSize = 2*XDstSize;
	YDstSize = 2*YDstSize;
#ifndef TURN_TEST
	cx = cx & clip_mask_x;
	cy = cy & clip_mask_y;
#endif
	int YSrcSize = YDstSize*XSrcSize/XDstSize;
	int sinTurn,cosTurn;
	int k_base_x,k_base_y;
	int k_up_x,k_up_y;
	int k_x,k_vu;
	int fx,tfx,tfy;
	int tmp;
	int i0, i;
	int* tpTmp;

	sinTurn = SI[rPI(Turn)];
	cosTurn = CO[rPI(Turn)];

#ifndef TURN_TEST
	tfx = (cx << 16) - (XSrcSize*cosTurn - YSrcSize*sinTurn)/2 + (1 << 15);
	int vv0 = XSrcSize*sinTurn;
	int vv1 = YSrcSize*cosTurn;
	int v0 = vv0 + vv1 >> 1;
	int v1 = vv0 - vv1 >> 1;
	int vcy = cy << 16;
	tfy = vcy - v0 + (1 << 15);

	int y0 = tfy >> 16;
	int y1 = (vcy + v0) >> 16;
	int y2 = (vcy - v1) >> 16;
	int y3 = (vcy + v1) >> 16;

	request(MIN(MIN(MIN(y0,y1),y2),y3) - MAX_RADIUS/2,MAX(MAX(MAX(y0,y1),y2),y3) + MAX_RADIUS/2,0,0);
	uchar** lt = vMap -> lineTcolor;
#else
	uchar** lt = TextureDataTable;
#endif

	int ParallelScanLines,ScreenDirectScan,TextureDirectScan;
	ParallelScanLines = abs(cosTurn) > abs(sinTurn);
	if(!ParallelScanLines){
		tmp = XDstSize;
		XDstSize = YDstSize;
		YDstSize = tmp;
		tmp = XSrcSize;
		XSrcSize = YSrcSize;
		YSrcSize = tmp;
//		  Turn = 3*PIx2/4 - Turn + PIx2;
//		  Turn %= PIx2;

//		  sinTurn = SI[rPI(Turn)];
//		  cosTurn = CO[rPI(Turn)];
		tmp = -sinTurn;
		sinTurn = -cosTurn;
		cosTurn = tmp;
		}

	k_base_x = cosTurn*XSrcSize/XDstSize;
	k_base_y = sinTurn*YSrcSize/YDstSize;
	k_up_x = -sinTurn*XSrcSize/XDstSize;
	k_up_y = cosTurn*YSrcSize/YDstSize;
//	  if(TestMode)
//		  k_x = int((double)XSrcSize/(double)cosTurn/(double)XDstSize*(double)0x10000*(double)0x10000);
//	  else
	k_x = ((XSrcSize << 16 + 5)/cosTurn << 16 - 5)/XDstSize;
	k_vu = -(sinTurn << 14)/(cosTurn >> 2);

	ScreenDirectScan = (sinTurn > 0) == (cosTurn > 0);
	TextureDirectScan = (sinTurn > 0) == (cosTurn > 0);
	if(ParallelScanLines){
		if(TextureDirectScan){
			tfx = GetCornerX(-2*XSrcSize + 1,-2*YSrcSize + 1);
			tfy = GetCornerY(-2*XSrcSize + 1,-2*YSrcSize + 1);
			}
		else{
			tfx = GetCornerX(2*XSrcSize - 1,-2*YSrcSize + 1);
			tfy = GetCornerY(2*XSrcSize - 1,-2*YSrcSize + 1);
			k_x = -k_x;
			}
		}
	else{
		if(TextureDirectScan){
			tfx = GetCornerX(-2*XSrcSize + 1,-2*YSrcSize + 1);
			tfy = GetCornerY(2*XSrcSize - 1,2*YSrcSize - 1);
			k_base_y = -k_base_y;
			k_up_y = -k_up_y;
			}
		else{
			tfx = GetCornerX(2*XSrcSize - 1,-2*YSrcSize + 1);
			tfy = GetCornerY(-2*XSrcSize + 1,2*YSrcSize - 1);
			k_base_y = -k_base_y;
			k_up_y = -k_up_y;
			k_x = -k_x;
			}
		}
	if(!ScreenDirectScan){
		k_base_x = - k_base_x;
		k_base_y = - k_base_y;
		}
	tfx += cx << 16;
	tfy += cy << 16;
	uchar* d;
	int u1,vb,ub;
	int base_step = ParallelScanLines ? 1 : XGR_MAXX;
	int up_step = ParallelScanLines ? XGR_MAXX : 1;
	if(!ScreenDirectScan)
		base_step = -base_step;

	calcLineTable(XDstSize,k_vu < 0 ? k_vu : -k_vu,base_step,up_step);
	int* tp;
	tpTmp = LineTable;
	if(ScreenDirectScan){
		u1 = 1;
		while(u1 < XDstSize && *tpTmp == base_step){
			tpTmp++;
			u1++;
			}
		}
	else{
		u1 = XDstSize - 1;
		while(u1 > 0 && *tpTmp == base_step){
			tpTmp++;
			u1--;
			}
		}
	for(vb = 0;vb < YDstSize;vb++){

		tp = LineTable + 0;
		if(ScreenDirectScan){
			if(ParallelScanLines)
				vpp = vp + (0) + (vb)*XGR_MAXX;
			else
				vpp = vp + (vb) + (0)*XGR_MAXX;
			}
		else{
			if(ParallelScanLines)
				vpp = vp + (XDstSize - 1) + (vb)*XGR_MAXX;
			else
				vpp = vp + (vb) + (XDstSize - 1)*XGR_MAXX;
			}
		if(ScreenDirectScan)
			i0 = u1;
		else
			i0 = XDstSize - 1 - u1 + 1;

		fx = tfx;
		int x = fx >> 16;
//		d = lt[YCYCL(tfy >> 16)] + XCYCL(fx >> 16);
		d = lt[YCYCL(tfy >> 16)];
//		  k_x2 = k_x & 0xFFFF;
//		  d_const = k_x >> 16;
#ifdef LOWLEVEL_OUTPUT
		if(d) invturn(d, vpp, fx, k_x, tp, i0);
//		  invturn(d, x, vpp, fx, k_x2, d_const, tp, i0);
		tp += i0;
#else
		for(i = i0;i > 0;i--){
			*vpp = *(d + (x & clip_mask_x));
			int prev_fx = fx;
			fx += k_x;
			//x += d_const + (prev_fx >> 16 != fx >> 16);
			x += (prev_fx >> 16 != fx >> 16);
			vpp += *tp++;
			}
#endif
		if(ScreenDirectScan){
			tpTmp = tp;
			if(u1 < XDstSize)
				u1++;
			while(u1 < XDstSize && *tpTmp == base_step){
				tpTmp++;
				u1++;
				}
			}
		else{
			tpTmp = tp;
			if(u1 > 0)
				u1--;
			while(u1 > 0 && *tpTmp == base_step){
				tpTmp++;
				u1--;
				}
			}
		tfx += k_up_x;
		tfy += k_up_y;
		}
	tfx -= k_up_x;
	tfy -= k_up_y;
	if(ScreenDirectScan){
		ub = 0;
		while(ub < XDstSize - 1){
			tpTmp = LineTable + ub;
			do{
				ub++;
				tfx += k_base_x;
				tfy += k_base_y;
				}while(*tpTmp++ == base_step);

			tp = LineTable + ub;
			if(ParallelScanLines)
				vpp = vp + (ub) + (YDstSize - 1 + OptionalParameter)*XGR_MAXX;
			else
				vpp = vp + (YDstSize - 1 + OptionalParameter) + (ub)*XGR_MAXX;
			i0 = u1 - ub;

			fx = tfx;
			int x = fx >> 16;
//			d = lt[YCYCL(tfy >> 16)] + XCYCL(fx >> 16);
			d = lt[YCYCL(tfy >> 16)];
//			  k_x2 = k_x & 0xFFFF;
//			  d_const = k_x >> 16;
#ifdef LOWLEVEL_OUTPUT
			if(d) invturn(d, vpp, fx, k_x, tp, i0);
//			  invturn(d, x, vpp, fx, k_x2, d_const, tp, i0);
			tp += i0;
#else
			for(i = i0;i > 0;i--){
				*vpp = *(d + (x & clip_mask_x));
				int prev_fx = fx;
				fx += k_x;
				//x += d_const + (prev_fx >> 16 != fx >> 16);
				x += (prev_fx >> 16 != fx >> 16);
				vpp += *tp++;
				}
#endif

			tpTmp = tp;
			if(u1 < XDstSize)
				u1++;
			while(u1 < XDstSize && *tpTmp == base_step){
				tpTmp++;
				u1++;
				}
			}
		}
	else{
		ub = XDstSize - 1;
		while(ub >= 0){
			tpTmp = LineTable + XDstSize - ub - 1;
			do{
				ub--;
				tfx += k_base_x;
				tfy += k_base_y;
				}while(*tpTmp++ == base_step);

			tp = LineTable + XDstSize - ub - 1;
			if(ParallelScanLines)
				vpp = vp + (ub) + (YDstSize - 1 + OptionalParameter)*XGR_MAXX;
			else
				vpp = vp + (YDstSize - 1 + OptionalParameter) + (ub)*XGR_MAXX;
			i0 = ub - u1 + 1;

			fx = tfx;
			int x = fx >> 16;
//			d = lt[YCYCL(tfy >> 16)] + XCYCL(fx >> 16);
			d = lt[YCYCL(tfy >> 16)];
//			  k_x2 = k_x & 0xFFFF;
//			  d_const = k_x >> 16;
#ifdef LOWLEVEL_OUTPUT
			if(d) invturn(d, vpp, fx, k_x, tp, i0);
//			  invturn(d, x, vpp, fx, k_x2, d_const, tp, i0);
			tp += i0;
#else
			for(i = i0;i > 0;i--){
				*vpp = *(d + (x & clip_mask_x));
				int prev_fx = fx;
				fx += k_x;
				//x += d_const + (prev_fx >> 16 != fx >> 16);
				x += (prev_fx >> 16 != fx >> 16);
				vpp += *tp++;
				}
#endif

			tpTmp = tp;
			if(u1 > 0)
				u1--;
			while(u1 > 0 && *tpTmp == base_step){
				tpTmp++;
				u1--;
				}
			}
		}
#endif
}

#ifndef _SURMAP_
void vrtMap::scaling_3D(DBM& A,int H,int focus,int cx,int cy,int xc,int yc,int xside,int yside,int Turn)
{
	char* vp = (char*)XGR_VIDEOBUF + (yc - yside)*XGR_MAXX + (xc - xside);
	int xsize = 2*xside;
	int ysize = 2*yside;
	int XADD = XGR_MAXX - xsize;

	cx = (cx & clip_mask_x) << 16;
	cy = (cy & clip_mask_y) << 16;

	double ai = Ha*xside;
	double aj = Va*yside;
	double bi = Hb*xside;
	double bj = Vb*yside;
	double ci = Hc*xside;
	double cj = Vc*yside;

	int y0 = ((int)round((bi + bj)/(Oc + ci + cj)) + cy) >> 16;
	int y1 = ((int)round((bi - bj)/(Oc + ci - cj)) + cy) >> 16;
	int y2 = ((int)round((-bi + bj)/(Oc - ci + cj)) + cy) >> 16;
	int y3 = ((int)round((-bi - bj)/(Oc - ci - cj)) + cy) >> 16;

	request(MIN(MIN(MIN(y0,y1),y2),y3) - MAX_RADIUS/2,MAX(MAX(MAX(y0,y1),y2),y3) + MAX_RADIUS/2,0,0);

	double al = -ai - aj;
	double bl = -bi - bj;
	double cl = Oc - ci - cj;

	double ar = ai - aj;
	double br = bi - bj;

	double xsize_inv = 1/(double)xsize;
	double cl_inv;

	int k_xscr_x,k_xscr_y;
	int i,j,fx,fy;
	uchar** lt = lineTcolor;
	uchar* data;

	if(Turn)
		for(i = 0;i < ysize;i++){
			cl_inv = 1/cl;
			fx = round(al*cl_inv);
			fy = round(bl*cl_inv);
			cl_inv *= xsize_inv;
			k_xscr_x = round((ar - al)*cl_inv);
			k_xscr_y = round((br - bl)*cl_inv);
			fx += cx;
			fy += cy;
			for(j = 0;j < xsize;j++){
				*vp++ = *(lt[(fy >> 16) & clip_mask_y] + ((fx >> 16) & clip_mask_x));
				fx += k_xscr_x;
				fy += k_xscr_y;
				}

			al += Va;
			bl += Vb;
			cl += Vc;

			ar += Va;
			br += Vb;

			vp += XADD;
			}
	else
		for(i = 0;i < ysize;i++){
			cl_inv = 1/cl;
			fx = round(al*cl_inv);
			fy = round(bl*cl_inv);
			cl_inv *= xsize_inv;
			k_xscr_x = round((ar - al)*cl_inv);
			fx += cx;
			fy += cy;
			data = lt[(fy >> 16) & clip_mask_y];
#ifdef LOWLEVEL_OUTPUT
			pscale(xsize,fx,vp,k_xscr_x,data);
			vp += xsize;
#else
			for(j = 0;j < xsize;j++){
				*vp++ = *(data + ((fx >> 16) & clip_mask_x));
				fx += k_xscr_x;
				}
#endif
			al += Va;
			bl += Vb;
			cl += Vc;

			ar += Va;
			br += Vb;

			vp += XADD;
			}
}
#endif

#ifdef _ROAD_
int PerpSlopTurn(int Turn,int Slop,int H,int F,int cx,int cy,int xc,int yc,int XDstSize,int YDstSize);
void vrtMap::SlopTurnSkip(int Turn,int Slop,int H,int F,int cx,int cy,int xc,int yc,int XDstSize,int YDstSize)
{
	PerpSlopTurn(Turn,Slop,H,F,cx,cy,xc,yc,XDstSize,YDstSize);
}
#endif

