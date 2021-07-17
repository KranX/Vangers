#include "../global.h"
#include "../lang.h"

#define LOWLEVEL_OUTPUT
//#define SAVE_ABILITY

//#include "..\win32f.h"


#include "iworld.h"
#include "ivmap.h"
#include "../terra/splay.h"

#include <iostream>

/* ----------------------------- EXTERN SECTION ---------------------------- */
extern int* SI;
extern int* CO;
extern unsigned storeRNDVAL,storerealRNDVAL;
extern int RestoreLog,CGenLog,MobilityLog,InitLog,SSSLog,ROLog,EncodeLog,DirectLog;
extern char* aci_ivMapName;
extern int actIntLog;

/* --------------------------- PROTOTYPE SECTION --------------------------- */
void iWORLD_colcalc(int y0,int y1);
void iLINE_render(int y);
void ipal_iter_init(void);
/* --------------------------- DEFINITION SECTION -------------------------- */
const int CLR_MAX_SIDE = 255;
const int CLR_MAX = 2*CLR_MAX_SIDE + 1;
const uchar SHADOW_CORRECTION = 1;
const uchar MAX_ALT = 255;
const uchar H_CORRECTION = 1;

const int MATERIAL_MAX = 1;
const int TERRAIN_MATERIAL[TERRAIN_MAX] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
const double TERRAIN_DXKOEF[MATERIAL_MAX] = { 1.0 };
const double TERRAIN_SDKOEF[MATERIAL_MAX] = { 1.0 };
const double TERRAIN_JJKOEF[MATERIAL_MAX] = { 1.0 };

uchar ilightCLRmaterial[MATERIAL_MAX][CLR_MAX];
uchar* ilightCLR[TERRAIN_MAX];
uchar ipalCLR[TERRAIN_MAX][2*256];

static uchar iBEGCOLOR[TERRAIN_MAX];
static uchar iENDCOLOR[TERRAIN_MAX];

static uint iMAP_POWER_Y;
static uint imap_size_y;
uint iclip_mask_y;

static uint iV_POWER;
static uint iV_SIZE;

static uchar* ishadowParent;

static int iPAL_MAX;
static int iPAL_WAVE_TERRAIN;
static int iPAL_TERRAIN[TERRAIN_MAX];
static int iPAL_SPEED[TERRAIN_MAX];
static int iPAL_AMPL[TERRAIN_MAX];
static int iPAL_RED[TERRAIN_MAX];
static int iPAL_GREEN[TERRAIN_MAX];
static int iPAL_BLUE[TERRAIN_MAX];

static uchar* ipalbuf,*ipalbufOrg;

ivrtMap* ivMap;

void iRenderPrepare(void)
{
	int j,ind,v;
	if(!ishadowParent)
		memset(ishadowParent = new uchar[4*map_size_x],0,4*map_size_x);

	const double dx = 8.0;
	const double sd = 256.0/(double)SHADOWDEEP;
	double jj,DX,SD;
	int colnum;
	for(ind = 0;ind < MATERIAL_MAX;ind++){
		DX = TERRAIN_DXKOEF[ind]*dx;
		SD = TERRAIN_SDKOEF[ind]*sd;
		for(j = -CLR_MAX_SIDE;j <= CLR_MAX_SIDE;j++){
			jj = TERRAIN_JJKOEF[ind]*(double)j;
			v = round(255.0*(DX*SD - jj)/sqrt((1.0 + SD*SD)*(DX*DX + jj*jj)));
			if(v < 0) v = 0;
			ilightCLRmaterial[ind][CLR_MAX_SIDE + j] = v;
			}
		}
	for(ind = 0;ind < TERRAIN_MAX;ind++){
		ilightCLR[ind] = ilightCLRmaterial[TERRAIN_MATERIAL[ind]];
		memset(ipalCLR[ind],iBEGCOLOR[ind],256);
		colnum = iENDCOLOR[ind] - iBEGCOLOR[ind];
		for(j = 0;j < 256;j++)
			ipalCLR[ind][256 + j] = iBEGCOLOR[ind] + round((double)j*(double)colnum/255.0);
		}
}

void iYSetup(void)
{
	imap_size_y = 1 << iMAP_POWER_Y;
	iclip_mask_y = imap_size_y - 1;
	iV_POWER = iMAP_POWER_Y;
	iV_SIZE = imap_size_y;
}

void ivMapPrepare(void)
{
	if(!ivMap) ivMap = new ivrtMap;
	if(!actIntLog){
		if(lang() != RUSSIAN)
			ivMap -> load("resource/iscreen/mainmenu.ini");
		else
			ivMap -> load("resource/iscreen/mainmenu2.ini");
		}
	else {
		ivMap -> load(aci_ivMapName);
		}

	ivMap -> init();
}

ivrtMap::ivrtMap(void)
: fmap(0){}

void ivrtMap::init(void)
{
	freeNodes = new ivrtNode[MAX_LINE];
	dHeap = new uchar[MAX_LINE*H2_SIZE];
	freeNodes_c = new ivrtNode[MAX_LINE];
	dHeap_c = new uchar[MAX_LINE*H_SIZE];

	memset(lineT = new uchar*[iV_SIZE],0,iV_SIZE*sizeof(uchar*));
	memset(lineTcolor = new uchar*[iV_SIZE],0,iV_SIZE*sizeof(uchar*));

	freeMax = freeMax_c = MAX_LINE;
	freeTail = freeTail_c = 0;

	ivrtNode* p;
	int i;
	for(i = 0,p = freeNodes;i < MAX_LINE - 1;i++,p++) p -> next = p + 1;
	p -> next = freeNodes;
	for(i = 0,p = freeNodes_c;i < MAX_LINE - 1;i++,p++) p -> next = p + 1;
	p -> next = freeNodes_c;
}

void ivrtMap::finit(void)
{
#ifdef SAVE_ABILITY
	int max = iYCYCL(downLine + 1);
	int i = upLine;
	do {
		if(lineT[i]){
			fmap.seek(foffset + i*H2_SIZE,XS_BEG);
			fmap.write(lineT[i],H2_SIZE);
			}
		if(++i == iV_SIZE) i = 0;
	} while(i != max);
#endif

	delete[] freeNodes;
	delete[] freeNodes_c;
	delete[] dHeap;
	delete[] dHeap_c;
	delete[] lineT;
	delete[] lineTcolor;
	if(isCompressed){
		delete[] inbuf;
		delete[] st_table;
		delete[] sz_table;
		}
	free(fileName);
	free(paletteName);
	free(iniName);
	free(fname);

	fmap.close();
}

void ivrtMap::analyzeINI(const char* name)
{
	const char* version = "1.4";

	std::string secGlobal = "global parameters:";
	std::string secStorage = "Storage:";
	std::string secRender = "Rendering Parameters:";
	std::string secPalette = "Dynamic Palette:";

	XStream ff((char*)name,XS_IN);
	ff.close();
	
	dictionary *dict_name = iniparser_load(name);
	//iniparser_dump(dict_name, stdout);
	int val = atoi(iniparser_getstring(dict_name,"global parameters:Map Power X", NULL));
	if(val != MAP_POWER_X) ErrH.Abort("Incorrect X-Size");
	
	iMAP_POWER_Y = atoi(iniparser_getstring(dict_name,"global parameters:Map Power Y", NULL));
	
	if(strcmp(version,iniparser_getstring(dict_name,"Storage:Version", NULL))) ErrH.Abort("Incorrect Storage Version");
	isCompressed = atoi(iniparser_getstring(dict_name,"Storage:Compressed Format Using", NULL));
	fileName = strdup(iniparser_getstring(dict_name,"Storage:File Name", NULL));
	paletteName = strdup(iniparser_getstring(dict_name,"Storage:Palette File", NULL));

	int tmax = atoi(iniparser_getstring(dict_name,"Rendering Parameters:Terrain Max", NULL));
	if((!tmax && TERRAIN_MAX != 8) || (tmax && tmax != TERRAIN_MAX)) ErrH.Abort("Incorrect Terrain Max");
	int i;
	{
		char* p = iniparser_getstring(dict_name,"Rendering Parameters:Begin Colors", NULL);
		XBuffer buf(p,128);
		for(i = 0;i < TERRAIN_MAX;i++) 
			buf >= iBEGCOLOR[i];
	}
	{
		char* p = iniparser_getstring(dict_name,"Rendering Parameters:End Colors", NULL);
		XBuffer buf(p,128);
		for(i = 0;i < TERRAIN_MAX;i++)
			buf >= iENDCOLOR[i];
	}
	ipal_iter_init();
	iPAL_MAX = iniparser_getint(dict_name,"Dynamic Palette:Terrain Number", 0);
	iPAL_WAVE_TERRAIN = iniparser_getint(dict_name,"Dynamic Palette:Wave Terrain", 0);
	if(iPAL_MAX > 0){
		{
			char* pp = iniparser_getstring(dict_name,"Dynamic Palette:Terrains", NULL);
			XBuffer b(pp,128);
			for(i = 0;i < iPAL_MAX;i++)
				b >= iPAL_TERRAIN[i];
		}
		{
			char* pp = iniparser_getstring(dict_name,"Dynamic Palette:Speeds", NULL);
			XBuffer b(pp,128);
			for(i = 0;i < iPAL_MAX;i++)
				b >= iPAL_SPEED[i];
		}
		{
			char *pp = iniparser_getstring(dict_name,"Dynamic Palette:Amplitudes", NULL);
			XBuffer b(pp,128);
			for(i = 0;i < iPAL_MAX;i++)
				b >= iPAL_AMPL[i];
		}
		{
			char *pp = iniparser_getstring(dict_name,"Dynamic Palette:Red", NULL);
			XBuffer b(pp,128);
			for(i = 0;i < iPAL_MAX;i++)
				b >= iPAL_RED[i];
		}
		{
			char *pp = iniparser_getstring(dict_name,"Dynamic Palette:Green", NULL);
			XBuffer b(pp,128);
			for(i = 0;i < iPAL_MAX;i++)
				b >= iPAL_GREEN[i];
		}
		{
			char *pp = iniparser_getstring(dict_name,"Dynamic Palette:Blue", NULL);
			XBuffer b(pp,128);
			for(i = 0;i < iPAL_MAX;i++)
				b >= iPAL_BLUE[i];
		}
	}
	
	iniparser_freedict(dict_name);
}

#if (defined(__unix__) || defined(__APPLE__))
#define _MAX_DIR   800
#define _MAX_DRIVE   5
#define _MAX_EXT   35
#define _MAX_FNAME   160
#define _MAX_PATH   1000 
#endif

char* iGetMergedName(char* name,char* path)
{
	/*static char buf [_MAX_PATH];
	char path_buffer[_MAX_PATH];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	//NEED_SEE!!!
	//if(_fullpath(path_buffer,path,_MAX_PATH) == NULL) ErrH.Abort("Invalid input name");
	
	realpath (path, path_buffer);
	//_splitpath(path_buffer,drive,dir,fname,NULL);
	
	strcpy(buf,drive);
	strcat(buf,dir);
	strcat(buf,name);*/
	std::string out = path;
	int st = out.find_last_of('/');
	out = out.substr(0, st+1);
	out+= name;
	//VNG_DEBUG()<<"iGetMergedName:"<<out<<std::endl;
	return strdup(out.c_str());
}

void ivrtMap::fileLoad(void)
{
	analyzeINI(iniName);
	iYSetup();

	XBuffer buf;
	buf < fileName < (isCompressed ? ".vmc" : ".vmp");
	std::string sbuf=strdup(iGetMergedName(buf.GetBuf(),iniName)), sbuf2;
	int startR = sbuf.find("reso");
	sbuf2 = sbuf.substr(startR, sbuf.size()-startR);
	fname = strdup(sbuf2.c_str());
}

void ivrtMap::load(const char* name)
{
	iniName = strdup(name);

	fileLoad();

#ifndef SAVE_ABILITY
	if(!fmap.open(fname,XS_IN))
		ErrH.Abort("iVMP not found");
#else
	if(!fmap.open(fname,XS_IN | XS_OUT))
		ErrH.Abort("iVMP not found");
#endif

	if(isCompressed){
		inbuf = new uchar[H2_SIZE];
		st_table = new int[iV_SIZE];
		sz_table = new short[iV_SIZE];
		for(uint i = 0;i < iV_SIZE;i++){
			fmap > st_table[i];
			fmap > sz_table[i];
			}
		InitSplay(fmap);
		}
	else {
		st_table = NULL;
		sz_table = NULL;
//		if(fmap.size() != (int)map_size_x*(int)imap_size_y*2) ErrH.Abort("Incorrect ivmp-file size");
		}

	offset = 0;
	foffset = fmap.tell();

	iRenderPrepare();
}

void ivrtMap::accept(int up,int down)
{
	up = iYCYCL(up);
	down = iYCYCL(down);

	int max = iYCYCL(down + 1);
	int i = up;
	uchar* p;
	int off;
	if(!isCompressed)
		do {
			freeMax--;
			p = use();
			off = i*H2_SIZE;
			if(off != offset){
				offset = off + H2_SIZE;
				fmap.seek(foffset + off,XS_BEG);
				}
			else
				offset += H2_SIZE;
			fmap.read(p,H2_SIZE);
			lineT[i] = p;
			lineTcolor[i] = use_c();
			iLINE_render(i);
			i = iYCYCL(i + 1);
		} while(i != max);
	else
		do {
			freeMax--;
			p = use();
			fmap.seek(st_table[i],XS_BEG);
			fmap.read(inbuf,sz_table[i]);
			ExpandBuffer(inbuf,p);
			lineT[i] = p;
			lineTcolor[i] = use_c();
			iLINE_render(i);
			i = iYCYCL(i + 1);
		} while(i != max);

	upLine = up;
	downLine = down;
}

inline uchar* ivrtMap::use(void)
{
	uchar* p = dHeap + freeTail*H2_SIZE;
	freeTail = (freeNodes + freeTail) -> next - freeNodes;
	return p;
}

inline uchar* ivrtMap::use_c(void)
{
	uchar* p = dHeap_c + freeTail_c*H_SIZE;
	freeTail_c = (freeNodes_c + freeTail_c) -> next - freeNodes_c;
	return p;
}

void iLINE_render(int y)
{
	uchar* pa,*pc,*pf,*pa0,*pc0,*pf0;
	uchar type,lxVal,rxVal;

	pa0 = pa = ivMap -> lineT[y];
	pf0 = pf = pa0 + H_SIZE;
	pc0 = pc = ivMap -> lineTcolor[y];
	for(unsigned int x = 0;x < map_size_x;x++,pa++,pc++,pf++){
		if(*pf & DOUBLE_LEVEL){
			x++; pc++; pf++; pa++;
			type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			lxVal = *pa;
			rxVal = *(pa0 + iXCYCL(x + 2));
			if(*pf & SHADOW_MASK)
				*(pc - 1) = *pc = ipalCLR[type][256 + ((ilightCLR[type][255 - (lxVal - rxVal)] - ((255 - *pa) >> H_CORRECTION)) >> SHADOW_CORRECTION)];
			else
				*(pc - 1) = *pc = ipalCLR[type][256 + ilightCLR[type][255 - (lxVal - rxVal)] - ((255 - *pa) >> H_CORRECTION)];
			}
		else {
			type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			lxVal = *(pa0 + iXCYCL(x - 1));
			rxVal = *(pa + 1);
			if(*pf & SHADOW_MASK)
				*pc = ipalCLR[type][256 + ((ilightCLR[type][255 - (lxVal - rxVal)] - ((255 - *pa) >> H_CORRECTION)) >> SHADOW_CORRECTION)];
			else
				*pc = ipalCLR[type][256 + ilightCLR[type][255 - (lxVal - rxVal)] - ((255 - *pa) >> H_CORRECTION)];

			lxVal = *pa;
			x++; pa++; pc++; pf++;

			type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			rxVal = *(pa0 + iXCYCL(x + 1));
			if(*pf & SHADOW_MASK)
				*pc = ipalCLR[type][256 + ((ilightCLR[type][255 - (lxVal - rxVal)] - ((255 - *pa) >> H_CORRECTION)) >> SHADOW_CORRECTION)];
			else
				*pc = ipalCLR[type][256 + ilightCLR[type][255 - (lxVal - rxVal)] - ((255 - *pa) >> H_CORRECTION)];
			}
		}
}

/* ----------------------------- SIDEREND.CPP ---------------------------*/

#define MEMSET(a,b,c) memset(a,b,c)

typedef unsigned char BYTE;

const int POSPOWER = 8;
const int POSVAL = 1 << POSPOWER;

inline void iPreStage(int& LastStep,int HiX,BYTE* pa0,int& hC,BYTE* pmask)
{
	int MaskShift = LastStep >> POSPOWER;
	int x = iXCYCL(HiX + 1);
	BYTE* pa = pa0 + x;
	BYTE* pf = pa + H_SIZE;

	while(hC - MaskShift < MAX_ALT){
		if(!x){
			pa = pa0;
			pf = pa + H_SIZE;
			}
		if(*pf & DOUBLE_LEVEL){
			const int dh = *pa + MaskShift;

			LastStep -= SHADOWDEEP;
			MaskShift = LastStep >> POSPOWER;

			pa++;
			const int h = *pa + MaskShift;
			if(hC < dh){
				MEMSET(pmask + hC,1,dh - hC);
				hC = dh;
				}
			MEMSET(pmask + h - SS_WIDTH,1,SS_WIDTH + 1);
			pa++;
			}
		else {
			int h = *pa + MaskShift;
			if(hC < h){
				MEMSET(pmask + hC,1,h - hC);
				hC = h;
				}
			LastStep -= SHADOWDEEP;
			MaskShift = LastStep >> POSPOWER;

			pa++;
			h = *pa + MaskShift;
			if(hC < h){
				MEMSET(pmask + hC,1,h - hC);
				hC = h;
				}
			pa++;
			}
		LastStep -= SHADOWDEEP;
		MaskShift = LastStep >> POSPOWER;

		x = iXCYCL(x + 2);
		pf += 2;
		}
	hC -= MAX_ALT;
}

inline void iMainStage(BYTE* pa0,int& hC,BYTE* pc0,int SizeX,int& x,BYTE*& grid,int& MaxAlt)
{
	BYTE* pa = pa0 + x;
	BYTE* pf = pa + H_SIZE;
	BYTE* pc = pc0 + x;

	BYTE* palCLRlast;
	BYTE* lightCLRlast;
	BYTE typeC = 0xFF;
	int i;
	for(i = 0;i < SizeX;i += 2){
		if(*pf & DOUBLE_LEVEL){
			const BYTE lxVal = *pa;
			const BYTE rxVal = *(pa0 + iXCYCL(x + 2));
			const BYTE h = *pa;
			const BYTE dh = *(pa - 1);
			const BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			const BYTE level = h;
			if(type != typeC){
				typeC = type;
				palCLRlast = ipalCLR[type];
				lightCLRlast = ilightCLR[type];
				}

			grid += 3;
			hC -= 3;
			MaxAlt -= 3;

			if(*(grid + level)){
				*pf |= SHADOW_MASK;
				*(pf - 1) |= SHADOW_MASK;
				*(pc - 1) = *pc = palCLRlast[256 + ((lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)) >> SHADOW_CORRECTION)];
				}
			else {
				*pf &= ~SHADOW_MASK;
				*(pf - 1) &= ~SHADOW_MASK;
				*(pc - 1) = *pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
				}
			pa--; pf--; pc--;
			if(dh > hC){
				MEMSET(grid + hC,1,dh - hC);
				hC = dh;
				}
			MEMSET(grid + h - SS_WIDTH,1,SS_WIDTH + 1);
			if(h > MaxAlt) MaxAlt = h;
			}
		else {
			BYTE lxVal = *(pa - 1);
			BYTE rxVal = *(pa0 + iXCYCL(x + 1));
			BYTE h = *pa;
			BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			BYTE level = h;
			grid += 1;
			hC -= 1;
			MaxAlt -= 1;
			if(type != typeC){
				typeC = type;
				palCLRlast = ipalCLR[type];
				lightCLRlast = ilightCLR[type];
				}

			if(*(grid + level)){
				*pf |= SHADOW_MASK;
				*pc = palCLRlast[256 + ((lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)) >> SHADOW_CORRECTION)];
				}
			else {
				*pf &= ~SHADOW_MASK;
				*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
				}
			if(h > hC){
				MEMSET(grid + hC,1,h - hC);
				hC = h;
				}
			if(h > MaxAlt) MaxAlt = h;

			pa--; pf--; pc--;
			rxVal = h;
			h = lxVal;
			lxVal = *(pa0 + iXCYCL(x - 2));
			grid += 2;
			hC -= 2;
			MaxAlt -= 2;
			type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
			level = h;
			if(type != typeC){
				typeC = type;
				palCLRlast = ipalCLR[type];
				lightCLRlast = ilightCLR[type];
				}

			if(*(grid + level)){
				*pf |= SHADOW_MASK;
				*pc = palCLRlast[256 + ((lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)) >> SHADOW_CORRECTION)];
				}
			else {
				*pf &= ~SHADOW_MASK;
				*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
				}
			if(h > hC){
				MEMSET(grid + hC,1,h - hC);
				hC = h;
				}
			if(h > MaxAlt)
				MaxAlt = h;
			}
		if(x == 1){
			x = H_SIZE - 1;
			pa = pa0 + x;
			pf = pa + H_SIZE;
			pc = pc0 + x;
			}
		else {
			x -= 2;
			pa--; pf--; pc--;
			}
		}
}

void iregRender(int LowX,int LowY,int HiX,int HiY)
{
	LowX = iXCYCL(LowX);
	HiX = iXCYCL(HiX);
	LowY = iYCYCL(LowY);
	HiY = iYCYCL(HiY);

	LowX &= ~1;
	HiX |= 1;

	int SizeY = (LowY == HiY) ? iV_SIZE : iYCYCL(HiY - LowY);
	int SizeX = (0 == iXCYCL(HiX - LowX)) ? H_SIZE : iXCYCL(HiX - LowX);

	int BackScanLen = 0;
	int j;
	for(j = 0;j < SizeY;j++){
		const int y = iYCYCL(j + LowY);
		BYTE* pa0 = ivMap -> lineT[y];
		BYTE* pc0 = ivMap -> lineTcolor[y];
		if(!pa0) continue;

		MEMSET(ishadowParent,0,(((SizeX*SHADOWDEEP) >> POSPOWER) + 4*MAX_ALT));
		int hC = MAX_ALT;
		int LastStep = (H_SIZE - 1 - HiX) * SHADOWDEEP;
		LastStep -= ((LastStep >> POSPOWER) - MAX_ALT) << POSPOWER;

		//препроход
		iPreStage(LastStep,HiX,pa0,hC,ishadowParent + MAX_ALT);
		//основной проход
		int x = HiX;
		BYTE* grid = ishadowParent + MAX_ALT + MAX_ALT;
		int MaxAlt = 0;
		iMainStage(pa0,hC,pc0,SizeX,x,grid,MaxAlt);

		//пост проход
		{
		x |= 1;
		BYTE* pa = pa0 + x;
		BYTE* pf = pa + H_SIZE;
		BYTE* pc = pc0 + x;
		BYTE* palCLRlast;
		BYTE* lightCLRlast;
		BYTE typeC = 0xFF;
		int MaxPossibleAlt = MAX_ALT;
		unsigned bNeedScan = 1;
		while(bNeedScan && MaxPossibleAlt >= 0){
			bNeedScan = 0;
			if(*pf & DOUBLE_LEVEL){
				const BYTE lxVal = *pa;
				const BYTE rxVal = *(pa0 + iXCYCL(x + 2));
				const BYTE h = *pa;
				const BYTE dh = *(pa - 1);
				const BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				const BYTE level = h;
				if(type != typeC){
					typeC = type;
					palCLRlast = ipalCLR[type];
					lightCLRlast = ilightCLR[type];
					}

				grid += 3;
				hC -= 3;
				MaxAlt -= 3;
				MaxPossibleAlt -= 3;
				if(dh < MaxAlt || (*pf & SHADOW_MASK)) bNeedScan = 1;
				if(*(grid + level)){
					*pf |= SHADOW_MASK;
					*(pf - 1) |= SHADOW_MASK;
					*(pc - 1) = *pc = palCLRlast[256 + ((lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)) >> SHADOW_CORRECTION)];
					}
				else {
					*pf &= ~SHADOW_MASK;
					*(pf - 1) &= ~SHADOW_MASK;
					*(pc - 1) = *pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
					}
				pa--; pf--; pc--;
				if(*pf & SHADOW_MASK) bNeedScan = 1;
				if(dh > hC){
					MEMSET(grid + hC,1,dh - hC);
					hC = dh;
					}
				MEMSET(grid + h - SS_WIDTH,1,SS_WIDTH + 1);
				}
			else {
				BYTE lxVal = *(pa - 1);
				BYTE rxVal = *(pa0 + iXCYCL(x + 1));
				BYTE h = *pa;
				BYTE type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				BYTE level = h;
				grid += 1;
				hC -= 1;
				MaxAlt -= 1;
				MaxPossibleAlt -= 1;
				if(type != typeC){
					typeC = type;
					palCLRlast = ipalCLR[type];
					lightCLRlast = ilightCLR[type];
					}

				if(h <= MaxAlt || (*pf & SHADOW_MASK)) bNeedScan = 1;
				if(*(grid + level)){
					*pf |= SHADOW_MASK;
					*pc = palCLRlast[256 + ((lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)) >> SHADOW_CORRECTION)];
					}
				else {
					*pf &= ~SHADOW_MASK;
					*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
					}
				if(h > hC){
					MEMSET(grid + hC,1,h - hC);
					hC = h;
					}

				pa--; pf--; pc--;
				rxVal = h;
				h = lxVal;
				lxVal = *(pa0 + iXCYCL(x - 2));
				grid += 2;
				hC -= 2;
				MaxAlt -= 2;
				MaxPossibleAlt -= 2;
				type = (*pf & TERRAIN_MASK) >> TERRAIN_OFFSET;
				level = h;
				if(type != typeC){
					typeC = type;
					palCLRlast = ipalCLR[type];
					lightCLRlast = ilightCLR[type];
					}

				if(h <= MaxAlt || (*pf & SHADOW_MASK)) bNeedScan = 1;
				if(*(grid + level)){
					*pf |= SHADOW_MASK;
					*pc = palCLRlast[256 + ((lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)) >> SHADOW_CORRECTION)];
					}
				else {
					*pf &= ~SHADOW_MASK;
					*pc = palCLRlast[256 + lightCLRlast[255 - (lxVal - rxVal)] - ((255 - h) >> H_CORRECTION)];
					}
				if(h > hC){
					MEMSET(grid + hC,1,h - hC);
					hC = h;
					}
				}
			if(x == 1){
				x = H_SIZE - 1;
				pa = pa0 + x;
				pf = pa + H_SIZE;
				pc = pc0 + x;
				}
			else {
				x = iXCYCL(x - 2);
				pa--; pf--; pc--;
				}
			}
		const int CurrScanLenght = iXCYCL(LowX - x);
		if(CurrScanLenght > BackScanLen) BackScanLen = CurrScanLenght;
		}
		}
}

/* ------------------ PALETTE.CPP ----------------------*/

void ipal_iter0(void)
{
	if(iPAL_WAVE_TERRAIN == -1 || iPAL_WAVE_TERRAIN >= TERRAIN_MAX) return;
	const int DSIZE = 8;
	const int PRD = 64;
	const int DATA[DSIZE] = { 1,3,5,6,5,3,2,1 };
	const int BEG = iBEGCOLOR[iPAL_WAVE_TERRAIN];
	const int SZ = iENDCOLOR[iPAL_WAVE_TERRAIN] - BEG;

	static int off = 0;
	static int cnt = PRD;
	static int add = 1;

	int i;
	uchar* p = ipalbuf + 3*(BEG + 1);
	uchar* po = ipalbufOrg + 3*(BEG + 1);
	memcpy(p,po,3*SZ);
	if(off > 0) p += 3*off, po += 3*off;

	for(i = 0;i < DSIZE;i++)
		if(off + i >= 0 && off + i < SZ){
			*p += DATA[i];
			if(*p > 63) *p = 63;
			p++;
			*p += DATA[i];
			if(*p > 63) *p = 63;
			p++;
			*p += DATA[i];
			if(*p > 63) *p = 63;
			p++;
			}
	if(--cnt) off = off + add;
	else {
		cnt = PRD;
		add = realRND(2) ? 1 : -1;
		if(add > 0) off = -DSIZE;
		else off = SZ - 1;
		}
}

void ipal_iter1(void)
{
	if(iPAL_WAVE_TERRAIN == -1 || iPAL_WAVE_TERRAIN >= TERRAIN_MAX) return;
	const int DSIZE = 10;
	const int PRD = 150;
	const int DATA[DSIZE] = { 1,3,5,7,10,8,6,4,2,1 };
	const int BEG = iBEGCOLOR[iPAL_WAVE_TERRAIN];
	const int SZ = iENDCOLOR[iPAL_WAVE_TERRAIN] - BEG;

	static int off = 0;
	static int cnt = PRD;
	static int add = 1;

	int i;
	uchar* p = ipalbuf + 3*(iBEGCOLOR[iPAL_WAVE_TERRAIN] + 1);
	uchar* po = ipalbufOrg + 3*(iBEGCOLOR[iPAL_WAVE_TERRAIN] + 1);
	memcpy(p,po,3*SZ);
	if(off > 0) p += 3*off, po += 3*off;

	for(i = 0;i < DSIZE;i++)
		if(off + i >= 0 && off + i < SZ){
			*p += DATA[i];
			if(*p > 63) *p = 63;
			p++;
			*p += DATA[i];
			if(*p > 63) *p = 63;
			p++;
			*p += DATA[i];
			if(*p > 63) *p = 63;
			p++;
			}
	if(--cnt) off = off + add;
	else {
		cnt = PRD;
		add = realRND(2) ? 1 : -1;
		if(add > 0) off = -DSIZE;
		else off = SZ - 1;
		}
//	XGR_SetPal(ipalbuf,BEG,SZ);
}

static int ioffset[TERRAIN_MAX];

void ipal_iter2(void)
{
	int add,i,j;
	uchar *p,*po;
	for(int ind = 0;ind < iPAL_MAX;ind++){
		ioffset[ind] = rPI(ioffset[ind] + iPAL_SPEED[ind]);
		add = iPAL_AMPL[ind]*SI[ioffset[ind]]/UNIT;
		p = ipalbuf + 3*iBEGCOLOR[iPAL_TERRAIN[ind]];
		po = ipalbufOrg + 3*iBEGCOLOR[iPAL_TERRAIN[ind]];
		for(i = iBEGCOLOR[iPAL_TERRAIN[ind]];i <= iENDCOLOR[iPAL_TERRAIN[ind]];i++){
			if(iPAL_RED[ind]){
				j = *po + add; if(j > 63) j = 63; else if(j < 0) j = 0;
				*p++ = j; po++;
				}
			else *p++ = *po++;
			if(iPAL_GREEN[ind]){
				j = *po + add; if(j > 63) j = 63; else if(j < 0) j = 0;
				*p++ = j; po++;
				}
			else *p++ = *po++;
			if(iPAL_BLUE[ind]){
				j = *po + add; if(j > 63) j = 63; else if(j < 0) j = 0;
				*p++ = j; po++;
				}
			else *p++ = *po++;
			}
//		XGR_SetPal(ipalbuf,iBEGCOLOR[iPAL_TERRAIN[ind]],iENDCOLOR[iPAL_TERRAIN[ind]] - iBEGCOLOR[iPAL_TERRAIN[ind]] + 1);
		}
}

void ipal_init(uchar* p)
{
	if(!ipalbuf){
		ipalbuf = new uchar[768];
		ipalbufOrg = new uchar[768];
		}
	memcpy(ipalbuf,p,768);
	memcpy(ipalbufOrg,p,768);
}

void ipal_iter_init(void)
{
	iPAL_MAX = 0;
	iPAL_WAVE_TERRAIN = -1;
	for(int i = 0;i < TERRAIN_MAX;i++) ioffset[i] = 0;
}

void ipal_iter(int reduced)
{
	ipal_iter0();
	ipal_iter1();
	ipal_iter2();
	XGR_SetPal(ipalbuf,0,reduced ? 128 : 256);
}
