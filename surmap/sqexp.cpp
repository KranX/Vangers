
#include "../src/global.h"

#ifdef _SURMAP_
#define SESSION
#endif

#include "sqint.h"

#include "../src/common.h"

#include "../src/terra/vmap.h"
#include "impass.h"
#include "../src/terra/world.h"
#include "../src/terra/render.h"
#include "../src/units/moveland.h"

#include "sqexp.h"
#include "tools.h"
#include "track.h"

#include "port.h"
#define itoa port_itoa
#include "missed.h"

#ifdef _NT
#include "..\root\win32f.h"
#endif

#define FLOOD_TERRAIN	 0
#define DRIFT_TERRAIN	 1
#define TOOLZER 	 2
#define GEONET		 3
#define LBM_MAX 	 4

#define TM_LIFT 	 0
#define TM_INTERPOLATE	 1

/* ----------------------------- EXTERN SECTION ---------------------------- */
extern sqFont sysfont;
extern int Quit;
extern int Regen;
extern int NoiseLevel,NoiseAmp,ShapeNumber,ShapeAmp;
extern int sqTrackBuild;
extern int sssUpdateLog;
extern int S3Dmode,S3Dlevel,S3Dinverse,S3DnoiseLevel,S3DnoiseAmp,S3Dside;
extern MobileLocation* mlobj;
extern int Redraw;
extern int VLstatus;
extern TntValoc* TntVLobj;
extern MLCloneValoc* MLCVLobj;
extern SensorValoc* SnsVLobj;
extern DangerValoc* DngVLobj;
extern int VLediting;
extern int TntZall;
extern int SnsZall;
extern int DngZall;
extern int TunnelProof;
/* --------------------------- PROTOTYPE SECTION --------------------------- */
void ImpassReset(void);
void do_left_buttion_off( int, int );
void do_right_buttion_off( int mX, int mY);
void WORLD_Flood(int x0,int y0,int ttype);
void WORLD_Drift(int x0,int y0,uchar level,int ttype);
void deltaZone(int x,int y,int rad,int smth,int dh,int smode,int eql);
void GeoSetZone(int x,int y,int rad,int level,int delta);
void setSmallCrater(int x,int y);
void BeginSecondLayerEditing(void);
void CancelSecondLayerEditing(void);
void AcceptSecondLayer(void);
void ShowHiddenLayer(void);
void S3Dload(char* name);
void MLbegin(void);
void MLadd(void);
void MLaccept(void);
void MLcancel(void);
void MLshow(int n);
void MLdelete(int n);
void MLreset(void);
void MLreload(void);
void MLedit(int n);
void MLgo(int n);
void MLacceptframe(void);
void MLrestoreframe(void);
void MLacceptediting(void);
void MLdeleteframe(void);
void MLinsertframe(void);
char* getTitle(void);
void PalettePrepare(void);
void VLcancel(void);
void VLsave(void);
void VLstart(int n);
void VLlbpress(int mX,int mY);
int VLrbpress(int mX,int mY);
/* --------------------------- DEFINITION SECTION -------------------------- */
iGameMap* curGMap;

int TrackStatus = 0;
int TrackBuild = 0;
int ColorBase = 0;
int RenderMode = 1;
int LastBranch = 0;
int ReBuildWide = 0;
int MaterSection = 2;
int WideSection = 20;

int PrmToggle;

int LBM_mode[2] = { TOOLZER, 0};
int FloodTerrainType = 2;
int FloodLevel = 50;
int DriftTerrainType = 3;
int DriftLevel = 50;
int DriftPower = 2;
int DriftNoise = 7;

int ToolzerRadius = 32;
int ToolzerSmooth = 5;
int ToolzerDelta = 1;
int ToolzerSMode;
int ToolzerEquDelta;

int GeonetRadius = 32;
int GeonetLevel = 128;
int GeonetDelta = 0;

int CurrentTerrain = -1, ActiveTerrain[TERRAIN_MAX] = { 
													1,1,1,1,1,1,1,1 
#ifdef TERRAIN16
													,1,1,1,1,1,1,1,1 
#endif
													};
int MiniLevel,MaxiLevel = 255;
int MosaicToggle;

int GroupToggle = 0;
int NodeHeight,NodeSlope;
int TerrainMode;

int iMainMenu::copt;
int iPrmMenu::copt;
int iLayerMenu::copt;
int iInputForm::copt;
int S3Dcopt;
int curBmpIndex;

int LayerStatus = UP_LAYER;
int TunnelHeight = 64;
int RenderingLayer;
int LayerMode;
int ExclusiveLayerMode;
int SecondLayerPriority;

int MLstatus,MLprocess;

static const char* ibmFName = "LEVEL.IBM";

ibmFile ibmObj = { 1024,1024,
				  320,240,
				  800,600,
				  250,200,
				  0,
				  nullptr,
				  0,
				  0,
				  0 };

uchar markBMP[7][7] = {
			0,0,0,COL1,0,0,0,
			0,0,0,COL1,0,0,0,
			0,0,0,COL1,0,0,0,
			COL1,COL1,COL1,0,COL1,COL1,COL1,
			0,0,0,COL1,0,0,0,
			0,0,0,COL1,0,0,0,
			0,0,0,COL1,0,0,0 };

static const char* RUS(const char* s,int back = 0)
{
        printf("RUS function is not supported\n");
	//@caiiiycuk TODO: write converter that used internal buffer to avoid memory leak
	return strdup(s);
//	static char buffer[512];
//	if(!back)
//		KDWIN::CharToOemBuff(s,buffer,strlen(s) + 1);
//	else
//		KDWIN::OemToCharBuff(s,buffer,strlen(s) + 1);
//	return buffer;
}

void sqStateSave(XStream& ff)
{
	ff	< LBM_mode[0] < LBM_mode[1]
		< FloodTerrainType
		< FloodLevel
		< DriftTerrainType
		< DriftLevel
		< ToolzerRadius
		< ToolzerSmooth
		< ToolzerDelta
		< ToolzerSMode
		< ToolzerEquDelta
		< GeonetRadius
		< GeonetLevel
		< GeonetDelta
		< CurrentTerrain;
	for(int i = 0;i < TERRAIN_MAX;i++) ff < ActiveTerrain[i];
	ff	< MiniLevel
		< MaxiLevel
		< MIN_WIDE
		< H_MAIN
		< WATERLINE
		< ROAD_MATERIAL
		< ROAD_FORCING
		< DriftPower
		< DriftNoise
		< TunnelHeight
		< ExclusiveLayerMode
		< SecondLayerPriority;

	ff	< S3Dcopt
		< S3Dmode
		< S3Dlevel
		< S3Dinverse
		< S3DnoiseLevel
		< S3DnoiseAmp
		< S3Dside
		< mosaicBMP.mode
		< mosaicBMP.force
		< mosaicBMP.copt
		< mosaicBMP.xoffset
		< mosaicBMP.yoffset
		< TunnelProof;
	for(int i = 0;i < 8;i++) ff < MosaicTypes[i];
}

void sqStateLoad(XStream& ff)
{
	ff	> LBM_mode[0] > LBM_mode[1]
		> FloodTerrainType
		> FloodLevel
		> DriftTerrainType
		> DriftLevel
		> ToolzerRadius
		> ToolzerSmooth
		> ToolzerDelta
		> ToolzerSMode
		> ToolzerEquDelta
		> GeonetRadius
		> GeonetLevel
		> GeonetDelta
		> CurrentTerrain;
	for(int i = 0;i < TERRAIN_MAX;i++) ff > ActiveTerrain[i];
	ff	> MiniLevel
		> MaxiLevel
		> MIN_WIDE
		> H_MAIN
		> WATERLINE
		> ROAD_MATERIAL
		> ROAD_FORCING
		> DriftPower
		> DriftNoise
		> TunnelHeight
		> ExclusiveLayerMode
		> SecondLayerPriority;

	ff	> S3Dcopt
		> S3Dmode
		> S3Dlevel
		> S3Dinverse
		> S3DnoiseLevel
		> S3DnoiseAmp
		> S3Dside
		> mosaicBMP.mode
		> mosaicBMP.force
		> mosaicBMP.copt
		> mosaicBMP.xoffset
		> mosaicBMP.yoffset
		> TunnelProof;
	for(int i = 0;i < 8;i++) ff > MosaicTypes[i];
}

iMainMenu::iMainMenu(sqElem* _owner,int _x,int _y)
: sqInputBox(_owner,_x,_y,10,10,&sysfont,"Main Menu")
{
	const int num = 13;

	x0 = _x; y0 = _y;

	*this + (menu = new sqPopupMenu(this,7,22,num,&sysfont,0,2));

	*menu * new sqMenuBar((uchar*)"Goto...",menu);
	*menu * new sqMenuBar((uchar*)"Mode Menu",menu);
	*menu * new sqMenuBar((uchar*)"Impass Menu",menu);
	*menu * new sqMenuBar((uchar*)"Layer Menu",menu);
	*menu * new sqMenuBar((uchar*)"BitMap Tool Menu",menu);
	*menu * new sqMenuBar((uchar*)"3D Shape Tool",menu);
	*menu * new sqMenuBar((uchar*)"Mobile Locations Menu",menu);
	*menu * new sqMenuBar((uchar*)"Valoc Management Menu",menu);
	*menu * new sqMenuBar((uchar*)"Load Another World",menu);
	*menu * new sqMenuBar((uchar*)"Quit (without saving)",menu);
	*menu * new sqMenuBar((uchar*)"Update session",menu);
	*menu * new sqMenuBar((uchar*)"Kill session (Undo)",menu);
	*menu * new sqMenuBar((uchar*)"Exit (with saving)",menu);

	sy = 30 + font -> sy*num;
	sx = menu -> len*8 + 16;
	x = _x - sx/2; y = _y - sy/2;
	if(x < getX() + 4) x = getX() + 4;
	if(y < getY() + 2) y = getY() + 2;
	if(x + sx + 2 >= getRX()) x = getRX() - sx - 2;
	if(y + sy + 4 >= getRY()) y = getRY() - sy - 4;
	set();

	menu -> setpointer(menu -> getbar(copt),0);

	QuantObj = obj = tail;
	tail -> activate(0);
	sqInputBox::draw();
}

void iMainMenu::message(int code,sqElem* object)
{
	switch(code){
		case M_CHOICE:
		case M_SETOPTION:
			switch(menu -> getptr(menu -> pointer)){
				case 0:
					sqE -> put(E_GOTOFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
					break;
				case 1:
					sqE -> put(E_PRMMENU,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
					break;
				case 2:
					sqE -> put(E_IMPCHOICE,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
					break;
				case 3:
					sqE -> put(E_LAYERMENU,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
					break;
				case 4:
					sqE -> put(E_BMPCHOICE,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
					break;
				case 5:
					sqE -> put(E_FORM3D,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
					break;
				case 6:
					sqE -> put(E_MOBILOCMENU,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
					break;
				case 7:
					sqE -> put(E_VALOCMENU,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
					break;
				case 8:
					sqE -> put(E_WORLDCHOICE,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
					break;
				case 9:
					MLreset();
					Quit = XT_TERMINATE_ID;
#ifdef SESSION
					sssUpdateLog = 0;
#endif
					break;
				case 10:
					MLreset();
					vMap -> flush();
#ifdef SESSION
					vMap -> sssUpdate();
#endif
					SaveVPR();
					break;
				case 11:
					MLreset();
#ifdef SESSION
					vMap -> sssKill();
#endif
					vMap -> refresh();
					LoadVPR();
					break;
				case 12:
					MLreset();
					MLsave();
					Quit = XT_TERMINATE_ID;
					break;
				}
			copt = menu -> getptr(menu -> pointer);
			close();
			return;
		}
	sqInputBox::message(code,object);
}

void iMainMenu::flush(void)
{
	XGR_Flush(getX() + x,getY() + y,sx,sy);
}

struct PrmFormat {
	int mode;
	const char* string;
	int* prm;
	int min,max;
	};

#ifdef TERRAIN16
const int PRMMAX0 = 26 + 8;
#else
const int PRMMAX0 = 26;
#endif
const int PRMMAX1 = 7;

PrmFormat PrmItem0[PRMMAX0] = {
		{-1,"Current Terrain ",       &CurrentTerrain,  -1,TERRAIN_MAX - 1 },
		{-1,"Terrain Filter  [0] ",    &ActiveTerrain[0], 0,1            },
		{-1,"Terrain Filter  [1] ",    &ActiveTerrain[1], 0,1            },
		{-1,"Terrain Filter  [2] ",    &ActiveTerrain[2], 0,1            },
		{-1,"Terrain Filter  [3] ",    &ActiveTerrain[3], 0,1            },
		{-1,"Terrain Filter  [4] ",    &ActiveTerrain[4], 0,1            },
		{-1,"Terrain Filter  [5] ",    &ActiveTerrain[5], 0,1            },
		{-1,"Terrain Filter  [6] ",    &ActiveTerrain[6], 0,1            },
		{-1,"Terrain Filter  [7] ",    &ActiveTerrain[7], 0,1            },
#ifdef TERRAIN16
		{-1,"Terrain Filter  [8] ",      &ActiveTerrain[8], 0,1            },
		{-1,"Terrain Filter  [9] ",      &ActiveTerrain[9], 0,1            },
		{-1,"Terrain Filter [10] ",    &ActiveTerrain[10], 0,1            },
		{-1,"Terrain Filter [11] ",    &ActiveTerrain[11], 0,1            },
		{-1,"Terrain Filter [12] ",    &ActiveTerrain[12], 0,1            },
		{-1,"Terrain Filter [13] ",    &ActiveTerrain[13], 0,1            },
		{-1,"Terrain Filter [14] ",    &ActiveTerrain[14], 0,1            },
		{-1,"Terrain Filter [15] ",    &ActiveTerrain[15], 0,1            },
#endif
		{-1,"MIN Level Filter   ",    &MiniLevel,        0,255          },
		{-1,"MAX Level Filter   ",    &MaxiLevel,        0,255          },
		{-1,"Mosaic On/Off ",     &MosaicToggle,  0,1          },

		{ 0,"Flood by Terrain Type ", &FloodTerrainType, 0,TERRAIN_MAX - 1            },
		{ 0,"Flood Level ",           &FloodLevel,       0,255          },
		{ 1,"Drift by Terrain Type ", &DriftTerrainType, 0,TERRAIN_MAX - 1            },
		{ 1,"Drift Level ",           &DriftLevel,       0,255          },
		{ 1,"Drift Power ",           &DriftPower,       0,16           },
		{ 1,"Drift Noise ",           &DriftNoise,       0,64           },
		{ 2,"Toolzer Radius ",        &ToolzerRadius,    1,MAX_RADIUS   },
		{ 2,"Toolzer Smooth Zone ",   &ToolzerSmooth,    0,10           },
		{ 2,"Toolzer Smooth Mode ",   &ToolzerSMode,     0,2            },
		{ 2,"Toolzer Delta ",         &ToolzerDelta,     -64,64         },
		{ 2,"Toolzer Equ-Delta ",     &ToolzerEquDelta,  0,255          },
		{ 3,"GeoNet Radius ",         &GeonetRadius,     0,MAX_RADIUS   },
		{ 3,"GeoNet Level ",          &GeonetLevel,      0,255          },
		{ 3,"GeoNet Delta ",          &GeonetDelta,      -255,255       }
		};

PrmFormat PrmItem1[PRMMAX1] = {
		{-2,"Lift The Branch ",       NULL,              0,0            },
		{-2,"Interpolate The Branch ",NULL,              0,0            },
		{-1,"Track Width ",           &MIN_WIDE,         1,255          },
		{-1,"Track Height ",          &H_MAIN,           0,255          },
		{-1,"Water Height ",          &WATERLINE,        0,255          },
		{-1,"Road Material ",         &ROAD_MATERIAL,    0,TERRAIN_MAX - 1            },
		{-1,"Road Forcing ",          &ROAD_FORCING,     -255,255       }
		};
PrmFormat* PrmItemPtr;

static const char* MModeStr[LBM_MAX] = {
				"FLOOD",
				"DRIFT",
				"TOOLZER",
				"GEONET"
				};
static XBuffer mmbuf(64);
char* GetMMode(void)
{
	register int i;
	mmbuf.init();

	mmbuf < "(" <= *(PrmItem0[0].prm) < " [";
	for(i = 0;i < TERRAIN_MAX;i++)
		mmbuf <= *(PrmItem0[1 + i].prm);
	mmbuf < "]) ";

	mmbuf < MModeStr[LBM_mode[0]];
	for(i = 1 + TERRAIN_MAX;i < PRMMAX0;i++)
		if(PrmItem0[i].prm && PrmItem0[i].mode == LBM_mode[0])
			mmbuf < " " <= *(PrmItem0[i].prm);

	return mmbuf.GetBuf();
}

iPrmMenu::iPrmMenu(sqElem* _owner,int _x,int _y)
: sqInputBox(_owner,_x,_y,10,10,&sysfont)
{
	x0 = _x; y0 = _y;

	int prmmax,vprm;
	if(TrackStatus){
		PrmToggle = 1;
		PrmItemPtr = PrmItem1;
		vprm = prmmax = PRMMAX1;
		copt = 0;
		}
	else {
		PrmToggle = 0;
		PrmItemPtr = PrmItem0;
		curGMap -> restoreFloodLvl();
#ifdef TERRAIN16
		vprm = (XGR_MAXY - 64 - 16)/16;
		if(vprm > PRMMAX0) vprm = PRMMAX0;
#else
		vprm = PRMMAX0;
#endif
		prmmax = PRMMAX0;
		}

	buf = new XBuffer;

	register int i;

	*this + (menu = new sqPopupMenu(this,4,4,vprm,&sysfont,0,2));

	for(i = 0;i < prmmax;i++){
		buf -> init();
		*buf < PrmItemPtr[i].string;
		if(PrmItemPtr[i].prm) *buf < " " <= *(PrmItemPtr[i].prm) < " ";
		*menu * new sqMenuBar((uchar*)((*buf).GetBuf()),menu,LBM_mode[PrmToggle] == PrmItemPtr[i].mode);
		}

	sy = font -> sy*vprm + 12;
	sx = font -> sx*menu -> len + 12;
	x = _x - sx/2; y = _y - sy/2;
	if(x < getX() + 4) x = getX() + 4;
	if(y < getY() + 2) y = getY() + 2;
	if(x + sx + 2 >= getRX()) x = getRX() - sx - 2;
	if(y + sy + 4 >= getRY()) y = getRY() - sy - 4;
	set();

	menu -> setpointer(menu -> getbar(copt),0);

	QuantObj = obj = tail;
	tail -> activate(0);
	sqInputBox::draw();
}

void iPrmMenu::message(int code,sqElem* object)
{
	switch(code){
		case M_CHOICE:
		case M_SETOPTION:
			copt = menu -> getptr(menu -> pointer);
			if(PrmItemPtr[copt].mode >= 0){
				LBM_mode[PrmToggle] = PrmItemPtr[copt].mode;
				if(!PrmToggle && LBM_mode[0] == FLOOD_TERRAIN) curGMap -> setFloodLvl();
				}
			if(!mosaicBMP.data) MosaicToggle = 0;
			if(PrmItemPtr[copt].mode == -2){
				switch(copt){
					case TM_LIFT:
						if(cBranch) Track.lift_branch(cBranch);
						break;
					case TM_INTERPOLATE:
						if(cBranch) Track.recalc_height_branch(cBranch);
						break;
					}
				}
			close();
			return;
		}
	sqInputBox::message(code,object);
}

void iPrmMenu::flush(void)
{
	XGR_Flush(getX() + x,getY() + y,sx,sy);
}

void iPrmMenu::keytrap(int key)
{
	const int PAUSE = 10;
	static int timer;
	int n;
	switch(key){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			n = menu -> getptr(menu -> pointer);
			if(CLOCK() - timer > PAUSE)
				*(PrmItemPtr[n].prm) = key - '0';
			else
				*(PrmItemPtr[n].prm) = (*(PrmItemPtr[n].prm))*10 + key - '0';
			if(*(PrmItemPtr[n].prm) < PrmItemPtr[n].min) *(PrmItemPtr[n].prm) = PrmItemPtr[n].min;
			if(*(PrmItemPtr[n].prm) > PrmItemPtr[n].max) *(PrmItemPtr[n].prm) = PrmItemPtr[n].max;
			buf -> init();
			(*buf) < PrmItemPtr[n].string <= *(PrmItemPtr[n].prm);
			menu -> pointer -> replace(menu,(uchar*)((*buf).GetBuf()));
			draw();
			timer = CLOCK();
			break;
		case SDLK_MINUS:
			n = menu -> getptr(menu -> pointer);
			*(PrmItemPtr[n].prm) = -(*(PrmItemPtr[n].prm));
			if(*(PrmItemPtr[n].prm) < PrmItemPtr[n].min) *(PrmItemPtr[n].prm) = PrmItemPtr[n].min;
			if(*(PrmItemPtr[n].prm) > PrmItemPtr[n].max) *(PrmItemPtr[n].prm) = PrmItemPtr[n].max;
			buf -> init();
			(*buf) < PrmItemPtr[n].string <= *(PrmItemPtr[n].prm);
			menu -> pointer -> replace(menu,(uchar*)((*buf).GetBuf()));
			draw();
			break;
		case SDLK_LEFT:
			n = menu -> getptr(menu -> pointer);
			if(PrmItemPtr[n].prm){
				buf -> init();
				if(--(*(PrmItemPtr[n].prm)) == PrmItemPtr[n].min - 1) *(PrmItemPtr[n].prm) = PrmItemPtr[n].max;
				(*buf) < PrmItemPtr[n].string <= *(PrmItemPtr[n].prm);
				menu -> pointer -> replace(menu,(uchar*)((*buf).GetBuf()));
				draw();
				}
			break;
		case SDLK_RIGHT:
			n = menu -> getptr(menu -> pointer);
			if(PrmItemPtr[n].prm){
				buf -> init();
				if(++(*(PrmItemPtr[n].prm)) == PrmItemPtr[n].max + 1) *(PrmItemPtr[n].prm) = PrmItemPtr[n].min;
				(*buf) < PrmItemPtr[n].string <= *(PrmItemPtr[n].prm);
				menu -> pointer -> replace(menu,(uchar*)((*buf).GetBuf()));
				draw();
				}
			break;
		default:
			sqInputBox::keytrap(key);
		}
}

iInputForm::iInputForm(sqElem* _owner,int _x,int _y,int _mode)
: sqInputBox(_owner,_x,_y,10,10,&sysfont,"P-Form")
{
	memset(params, 0, sizeof(params));

	int NITEMS = XGR_MAXX <=640 ? 10 : 20;
	const int M = 19;
	int W = XGR_MAXX - (1024 - 950);

	x0 = _x; y0 = _y;
	mode = _mode;
	int yadd = 0,xadd = 41*8,ady;

	char tmpstr[16],*fname;
	int i,max,j;
	XBuffer tb;

	switch(mode){
		case E_OPENFORM:
		case E_SAVEFORM:
			*this + (name = new sqField(this,"Name: ",4,22,0,&sysfont,(uchar*)mapFName,32));
			name -> index = strlen(mapFName);
			break;
		case E_PLACEBMPFORM:
			fname = win32_findfirst("bitmap\\*.bmp");
			if(!fname) return;
			*this + (menu = new sqPopupMenu(this,7,22,NITEMS,&sysfont,0,3));
			i = 0;
			do { i++; *menu * new sqMenuBar((uchar*)fname,menu); }
			while((fname = win32_findnext()) != NULL);
			if(i > NITEMS) i = NITEMS;
			ady = 22 + (yadd = (i - 1)*16);
			yadd += 5*24;
			*this + (params[0] = new sqField(this,"FORCE (0/1):",4,ady + 1*24,0,&sysfont,(uchar*)itoa(placeBMP.force,tmpstr,10),1,T_NUMERIC));
			*this + (params[1] = new sqField(this,"MAP/MAX/MIN/MEAN/ADD (0/1/2/3/4):",4,ady + 2*24,0,&sysfont,(uchar*)itoa(placeBMP.mode,tmpstr,10),1,T_NUMERIC));
			*this + (params[2] = new sqField(this,"Border: ",4,ady + 3*24,0,&sysfont,(uchar*)itoa(placeBMP.border,tmpstr,10),2,T_NUMERIC));
			*this + (params[3] = new sqField(this,"Level: ",4,ady + 4*24,0,&sysfont,(uchar*)itoa(placeBMP.level,tmpstr,10),3,T_NUMERIC));
			*this + (params[4] = new sqField(this,"Size: ",4,ady + 5*24,0,&sysfont,(uchar*)itoa(placeBMP.size,tmpstr,10),4,T_NUMERIC));
			break;
		case E_MOSAICFORM:
			fname = win32_findfirst("bitmap\\mosaic\\*.bmp");
			if(!fname) return;
			*this + (menu = new sqPopupMenu(this,7,22,NITEMS,&sysfont,0,3));
			i = 0;
			do { i++; *menu * new sqMenuBar((uchar*)fname,menu); }
			while((fname = win32_findnext()) != NULL);
			if(mosaicBMP.copt < i) menu -> setpointer(menu -> getbar(mosaicBMP.copt),0);
			if(i > NITEMS) i = NITEMS;
			yadd = MAX((i - 1)*16,7*24);
			ady = 22 + yadd;
			yadd += 3*24;
			*this + (params[1] = new sqField(this,"Relative/Additive/Terrain (0/1/2):",4,ady + 1*24,0,&sysfont,(uchar*)itoa(mosaicBMP.mode,tmpstr,10),1,T_NUMERIC));
			*this + (params[2] = new sqField(this,"Additive amplitude (-255 - 255):",4,ady + 2*24,0,&sysfont,(uchar*)itoa(mosaicBMP.force,tmpstr,10),4,T_NUMERIC));
			*this + (params[3] = new sqField(this,"X-offset:",4,ady + 3*24,0,&sysfont,(uchar*)itoa(mosaicBMP.xoffset,tmpstr,10),4,T_NUMERIC));
			*this + (params[4] = new sqField(this,"Y-offset:",4 + 140,ady + 3*24,0,&sysfont,(uchar*)itoa(mosaicBMP.yoffset,tmpstr,10),4,T_NUMERIC));
			for(i = 0;i < 8;i++){
				tb.init();
				tb < "Type " <= i < ":";
				*this + (params[5 + i] = new sqField(this,tb.GetBuf(),4 + 230,24 + i*24,0,&sysfont,(uchar*)itoa(MosaicTypes[i],tmpstr,10),2,T_NUMERIC));
				}
			break;
		case E_IBMFORM:
			*this + (name = new sqField(this,"Name: ",4,22,0,&sysfont,(uchar*)ibmFName,32));
			name -> index = strlen(ibmFName);
			yadd = 10*24;
			*this + (params[0] = new sqField(this,"Map X Center:",4,22 + 1*24,0,&sysfont,(uchar*)itoa(ibmObj.mapx_center,tmpstr,10),5,T_NUMERIC));
			*this + (params[1] = new sqField(this,"Map Y Center:",4,22 + 2*24,0,&sysfont,(uchar*)itoa(ibmObj.mapy_center,tmpstr,10),5,T_NUMERIC));
			*this + (params[2] = new sqField(this,"X Center:",4,22 + 3*24,0,&sysfont,(uchar*)itoa(ibmObj.x_center,tmpstr,10),4,T_NUMERIC));
			*this + (params[3] = new sqField(this,"Y Center:",4,22 + 4*24,0,&sysfont,(uchar*)itoa(ibmObj.y_center,tmpstr,10),4,T_NUMERIC));
			*this + (params[4] = new sqField(this,"X Size:",4,22 + 5*24,0,&sysfont,(uchar*)itoa(ibmObj.x_size,tmpstr,10),4,T_NUMERIC));
			*this + (params[5] = new sqField(this,"Y Size:",4,22 + 6*24,0,&sysfont,(uchar*)itoa(ibmObj.y_size,tmpstr,10),4,T_NUMERIC));
			*this + (params[6] = new sqField(this,"X Side:",4,22 + 7*24,0,&sysfont,(uchar*)itoa(ibmObj.x_side,tmpstr,10),4,T_NUMERIC));
			*this + (params[7] = new sqField(this,"Y Side:",4,22 + 8*24,0,&sysfont,(uchar*)itoa(ibmObj.y_side,tmpstr,10),4,T_NUMERIC));
			*this + (params[8] = new sqField(this,"Level:",4,22 + 9*24,0,&sysfont,(uchar*)itoa(ibmObj.level,tmpstr,10),3,T_NUMERIC));
			*this + (params[9] = new sqField(this,"Shot Counter:",4,22 + 10*24,0,&sysfont,(uchar*)itoa(ibmObj.counter,tmpstr,10),3,T_NUMERIC));
			break;
		case E_IMPCHOICE:
			yadd = (IMP_MAX - 1)*16;
			*this + (menu = new sqPopupMenu(this,7,22,IMP_MAX,&sysfont,0,3));
			for(i = 0;i < IMP_MAX;i++) *menu * new sqMenuBar((uchar*)ImpItem[i],menu);
			if(copt >= IMP_MAX) copt = IMP_MAX - 1;
			menu -> setpointer(menu -> getbar(copt),0);
			xadd = menu -> len*8 + 16;
			break;
		case E_BMPCHOICE:
			yadd = (3 - 1)*16;
			*this + (menu = new sqPopupMenu(this,7,22,3,&sysfont,0,3));
			*menu * new sqMenuBar((uchar*)"Mosaic Setup",menu);
			*menu * new sqMenuBar((uchar*)"Place BitMap",menu);
			*menu * new sqMenuBar((uchar*)"Accept IBM",menu);
			if(copt >= 3) copt = 2;
			menu -> setpointer(menu -> getbar(copt),0);
			xadd = menu -> len*8 + 16;
			break;
		case E_IMPSPHERE:
			yadd = 10*24;
			*this + (params[0]   = new sqField(this,"X Size:",4,22 + 0*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> sx,tmpstr,10),3,T_NUMERIC));
			*this + (params[1]   = new sqField(this,"Y Size:",4,22 + 1*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> sy,tmpstr,10),3,T_NUMERIC));
			*this + (params[2]   = new sqField(this,"FORCE (0/1):",4,22 + 2*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> force,tmpstr,10),1,T_NUMERIC));
			*this + (params[3]   = new sqField(this,"MAP/MAX/MIN/MEAN/ADD (0/1/2/3/4):",4,22 + 3*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> mode,tmpstr,10),1,T_NUMERIC));
			*this + (params[4]   = new sqField(this,"Level:",4,22 + 4*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> level,tmpstr,10),3,T_NUMERIC));
			*this + (params[5]   = new sqField(this,"Radius (1-100):",4,22 + 5*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> radInt,tmpstr,10),3,T_NUMERIC));
			*this + (params[6]   = new sqField(this,"H:",4,22 + 6*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> h,tmpstr,10),3,T_NUMERIC));
			*this + (params[7]   = new sqField(this,"INVERSE (0/1):",4,22 + 7*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> inverse,tmpstr,10),1,T_NUMERIC));
			*this + (params[8]   = new sqField(this,"PLAIN (0/1)  :",4,22 + 8*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> plain,tmpstr,10),1,T_NUMERIC));
			*this + (params[9]   = new sqField(this,"NOISE LEVEL (0 - 100)  :",4,22 + 9*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> noiseLevel,tmpstr,10),3,T_NUMERIC));
			*this + (params[10] = new sqField(this,"NOISE AMPLITUDE (0 - 255)  :",4,22 + 10*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> noiseAmp,tmpstr,10),3,T_NUMERIC));
			break;
		case E_IMPPOLYGON:
			yadd = 12*24;
			*this + (params[0] = new sqField(this,"X Size:",4,22 + 0*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> sx,tmpstr,10),3,T_NUMERIC));
			*this + (params[1] = new sqField(this,"Y Size:",4,22 + 1*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> sy,tmpstr,10),3,T_NUMERIC));
			*this + (params[2] = new sqField(this,"FORCE (0/1):",4,22 + 2*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> force,tmpstr,10),1,T_NUMERIC));
			*this + (params[3] = new sqField(this,"MAP/MAX/MIN/MEAN/ADD (0/1/2/3/4):",4,22 + 3*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> mode,tmpstr,10),1,T_NUMERIC));
			*this + (params[4] = new sqField(this,"Level:",4,22 + 4*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> level,tmpstr,10),3,T_NUMERIC));
			*this + (params[5] = new sqField(this,"Radius (1-100):",4,22 + 5*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> radInt,tmpstr,10),3,T_NUMERIC));
			*this + (params[6] = new sqField(this,"H:",4,22 + 6*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> h,tmpstr,10),3,T_NUMERIC));
			*this + (params[7] = new sqField(this,"INVERSE (0/1):",4,22 + 7*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> inverse,tmpstr,10),1,T_NUMERIC));
			*this + (params[8] = new sqField(this,"PLAIN (0/1)  :",4,22 + 8*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> plain,tmpstr,10),1,T_NUMERIC));
			*this + (params[9] = new sqField(this,"N:",4,22 + 9*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> n,tmpstr,10),3,T_NUMERIC));
			*this + (params[10] = new sqField(this,"PHASE (0-1000):",4,22 + 10*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> phase,tmpstr,10),4,T_NUMERIC));
			*this + (params[11] = new sqField(this,"NOISE LEVEL (0 - 100)  :",4,22 + 11*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> noiseLevel,tmpstr,10),3,T_NUMERIC));
			*this + (params[12] = new sqField(this,"NOISE AMPLITUDE (0 - 255)  :",4,22 + 12*24,0,&sysfont,(uchar*)itoa(ImpPattern[curImpIndex] -> noiseAmp,tmpstr,10),3,T_NUMERIC));
			break;
		case E_TRACKFORM:
			yadd = 12*24;
			*this + (params[0] = new sqField(this,"Group Toggle (0/1)   :",4,22 + 0*24,0,&sysfont,(uchar*)itoa(GroupToggle,tmpstr,10),1,T_NUMERIC));
			*this + (params[1] = new sqField(this,"Node Height (0-255)  :",4,22 + 1*24,0,&sysfont,(uchar*)itoa(NodeHeight,tmpstr,10),3,T_NUMERIC));
			*this + (params[2] = new sqField(this,"Node Slope (-255-255):",4,22 + 2*24,0,&sysfont,(uchar*)itoa(NodeSlope,tmpstr,10),4,T_NUMERIC));
			*this + (params[3] = new sqField(this,"Noise Level (0-100%) :",4,22 + 3*24,0,&sysfont,(uchar*)itoa(NoiseLevel,tmpstr,10),3,T_NUMERIC));
			*this + (params[4] = new sqField(this,"Noise Amplitude      :",4,22 + 4*24,0,&sysfont,(uchar*)itoa(NoiseAmp,tmpstr,10),2,T_NUMERIC));
			*this + (params[5] = new sqField(this,"Shape Number   :",4,22 + 5*24,0,&sysfont,(uchar*)itoa(ShapeNumber,tmpstr,10),2,T_NUMERIC));
			*this + (params[6] = new sqField(this,"Shape Amplitude:",4,22 + 6*24,0,&sysfont,(uchar*)itoa(ShapeAmp,tmpstr,10),3,T_NUMERIC));
			*this + (params[7] = new sqField(this,"Terrain Mode   :",4,22 + 7*24,0,&sysfont,(uchar*)itoa(TerrainMode,tmpstr,10),1,T_NUMERIC));
			*this + (params[8] = new sqField(this,"Color Index (0-6):",4,22 + 8*24,0,&sysfont,(uchar*)itoa(ColorBase,tmpstr,10),1,T_NUMERIC));
			*this + (params[9] = new sqField(this,"Render Mode (0/1):",4,22 + 9*24,0,&sysfont,(uchar*)itoa(RenderMode,tmpstr,10),1,T_NUMERIC));
			*this + (params[10] = new sqField(this,"New Wide :",4,22 + 10*24,0,&sysfont,(uchar*)itoa(ReBuildWide,tmpstr,10),3,T_NUMERIC));
			*this + (params[11] = new sqField(this,"Material :",4,22 + 11*24,0,&sysfont,(uchar*)itoa(MaterSection,tmpstr,10),3,T_NUMERIC));
			*this + (params[12] = new sqField(this,"Wide :",4,22 + 12*24,0,&sysfont,(uchar*)itoa(WideSection,tmpstr,10),3,T_NUMERIC));
			break;
		case E_GOTOFORM:
			yadd = 1*24;
			tb < "X (0 - " <= map_size_x < ") :";
			*this + (params[0] = new sqField(this,tb.GetBuf(),4,22 + 0*24,0,&sysfont,(uchar*)itoa(curGMap -> CX,tmpstr,10),5,T_NUMERIC));
			tb.init();
			tb < "Y (0 - " <= map_size_y < ") :";
			*this + (params[1] = new sqField(this,tb.GetBuf(),4,22 + 1*24,0,&sysfont,(uchar*)itoa(curGMap -> CY,tmpstr,10),5,T_NUMERIC));
			break;
		case E_FORM3D:
			fname = win32_findfirst("shape3d\\*.c3d");
			if(!fname) return;
			*this + (menu = new sqPopupMenu(this,7,22,NITEMS,&sysfont,0,3));
			i = 0;
			do { i++; *menu * new sqMenuBar((uchar*)fname,menu); }
			while((fname = win32_findnext()) != NULL);
			if(S3Dcopt < i) menu -> setpointer(menu -> getbar(S3Dcopt),0);
			if(i > NITEMS) i = NITEMS;
			ady = 22 + (yadd = (i - 1)*16);
			yadd += 6*24;
			*this + (params[1] = new sqField(this,"MAP/MAX/MIN/MEAN/ADD (0/1/2/3/4):",4,ady + 1*24,0,&sysfont,(uchar*)itoa(S3Dmode,tmpstr,10),1,T_NUMERIC));
			*this + (params[2] = new sqField(this,"Level (0-255):",4,ady + 2*24,0,&sysfont,(uchar*)itoa(S3Dlevel,tmpstr,10),3,T_NUMERIC));
			*this + (params[3] = new sqField(this,"Inverse (0/1):",4,ady + 3*24,0,&sysfont,(uchar*)itoa(S3Dinverse,tmpstr,10),1,T_NUMERIC));
			*this + (params[4] = new sqField(this,"Noise Level (0-100%):",4,ady + 4*24,0,&sysfont,(uchar*)itoa(S3DnoiseLevel,tmpstr,10),3,T_NUMERIC));
			*this + (params[5] = new sqField(this,"Noise Amplitude (0-255):",4,ady + 5*24,0,&sysfont,(uchar*)itoa(S3DnoiseAmp,tmpstr,10),3,T_NUMERIC));
			*this + (params[6] = new sqField(this,"Side - up/down (0/1):",4,ady + 6*24,0,&sysfont,(uchar*)itoa(S3Dside,tmpstr,10),1,T_NUMERIC));
			break;
		case E_MOBILOCMENU:
			i = !MLstatus ? 6 : (MLstatus == 1 ? 3 : 8);
			yadd = (i - 1)*16;
			*this + (menu = new sqPopupMenu(this,7,22,i,&sysfont,0,3));
		switch(MLstatus){
		case 0:
			if(!MLprocess)
				*menu * new sqMenuBar((uchar*)RUS("Разрешить ML-анимацию"),menu);
			else
				*menu * new sqMenuBar((uchar*)RUS("Запретить ML-анимацию"),menu);
			*menu * new sqMenuBar((uchar*)RUS("Создание нового ML-объекта"),menu);
			*menu * new sqMenuBar((uchar*)RUS("Покадровое редактирование существующих ML-объектов..."),menu);
			*menu * new sqMenuBar((uchar*)RUS("Удаление ML-объектов..."),menu);
			*menu * new sqMenuBar((uchar*)RUS("Редактирование параметров у существующих ML-объектов..."),menu);
			*menu * new sqMenuBar((uchar*)RUS("Показать один из существующих ML-объектов..."),menu);
			break;
		case 1:
			*menu * new sqMenuBar((uchar*)RUS("Добавить кадр"),menu);
			*menu * new sqMenuBar((uchar*)RUS("Закончить создание ML-объекта и записать его"),menu);
			*menu * new sqMenuBar((uchar*)RUS("Прекратить создание ML-объекта без сохранения результатов"),menu);
			break;
		case 2:
			*menu * new sqMenuBar((uchar*)RUS("[G] Перейти на один из существующих кадров..."),menu);
			*menu * new sqMenuBar((uchar*)RUS("[A] Запомнить изменения текущего кадра"),menu);
			*menu * new sqMenuBar((uchar*)RUS("[Q] Восстановить кадр в его начальное состояние"),menu);
			*menu * new sqMenuBar((uchar*)RUS("[N] Вставить новый кадр"),menu);
			*menu * new sqMenuBar((uchar*)RUS("[D] Удалить текущий кадр"),menu);
			*menu * new sqMenuBar((uchar*)RUS("Изменить параметры ML-объекта"),menu);
			*menu * new sqMenuBar((uchar*)RUS("Закончить редактирование ML-объекта и сохранить все изменения"),menu);
			*menu * new sqMenuBar((uchar*)RUS("Прекратить редактирование без сохранения всех изменений"),menu);
			break;
		}
			if(copt >= 6) copt = 5;
			menu -> setpointer(menu -> getbar(copt),0);
			xadd = menu -> len*8 + 16;
			break;
		case E_VALOCMENU:
			i = 6;
			yadd = (i - 1)*16;
			*this + (menu = new sqPopupMenu(this,7,22,i,&sysfont,0,3));
			*menu * new sqMenuBar((uchar*)"TNT Valoc Handle",menu);
			*menu * new sqMenuBar((uchar*)"ML-Clone Valoc Handle",menu);
			*menu * new sqMenuBar((uchar*)"Sensor Valoc Handle",menu);
			*menu * new sqMenuBar((uchar*)"Danger Valoc Handle",menu);
			*menu * new sqMenuBar((uchar*)"Cancel Valoc Handle",menu);
			*menu * new sqMenuBar((uchar*)"Save Results",menu);
			if(copt >= i) copt = i - 1;
			menu -> setpointer(menu -> getbar(copt),0);
			xadd = menu -> len*8 + 16;
			break;
		case E_MOBILOCFORM:
			*this + (name = new sqField(this,"Name: ",4,22,0,&sysfont,(uchar*)mlobj -> name,MLNAMELEN));
			name -> index = strlen(mlobj -> name);
			max = mlobj -> maxFrame;
			yadd = (1 + ((max > M) ? M : max))*24;
			xadd = W;
			*this + (params[0] = new sqField(this,"Dry Terrain:",200 + 4,22,0,&sysfont,(uchar*)itoa(mlobj -> DryTerrain,tmpstr,10),1,T_NUMERIC));
			*this + (params[1] = new sqField(this,"Impulse:",340 + 4,22,0,&sysfont,(uchar*)itoa(mlobj -> Impulse,tmpstr,10),3,T_NUMERIC));
			*this + (params[2] = new sqField(this,"Key Phases [0]:0 [1]:",4,22 + 24,0,&sysfont,(uchar*)itoa(mlobj -> KeyPhase[1],tmpstr,10),3,T_NUMERIC));
			for(i = 0;i < MAX_KEYPHASE - 2;i++){
				tb.init();
				tb < "[" <= i + 2 < "]:";
				*this + (params[3 + i] = new sqField(this,tb.GetBuf(),4 + 210 + i*70,22 + 24,0,&sysfont,(uchar*)itoa(mlobj -> KeyPhase[i + 2],tmpstr,10),3,T_NUMERIC));
				}

			for(i = 0,j = 0;i < max;i++,j++){
				if(!(i%M) && i) j = 0;
				tb.init();
				tb < "Prd[" <= i < "]:";
				*this + (params[5 + i] = new sqField(this,tb.GetBuf(),4 + i/M*105,22 + (2 + j)*24,0,&sysfont,(uchar*)itoa(mlobj -> table[i].period,tmpstr,10),3,T_NUMERIC));
				}
			for(i = 0,j = 0;i < max;i++,j++){
				if(!(i%M) && i) j = 0;
				tb.init();
				tb < "Srf[" <= i < "]:";
				*this + (params[5 + max + i] = new sqField(this,tb.GetBuf(),i/M*100 + 4 + W/2,22 + (2 + j)*24,0,&sysfont,(uchar*)itoa(mlobj -> table[i].surfType,tmpstr,10),2,T_NUMERIC));
				}
			break;
		case E_MOBILOCDELETECHOICE:
			yadd = -16;
			*this + (menu = new sqPopupMenu(this,7,22,NITEMS,&sysfont,0,3));
			for(i = 0;i < MLTableSize;i++){
				tb.init();
				tb < "Delete " < MLTable[i] -> name;
				*menu * new sqMenuBar((uchar*)(tb.GetBuf()),menu,MLTable[i] -> inUse);
				}
			yadd += 16*MIN(NITEMS,MLTableSize);
			xadd = menu -> len*8 + 16;
			break;
		case E_MOBILOCSHOWCHOICE:
		case E_MOBILOCEDITCHOICE:
		case E_MOBILOCCHOICE:
			yadd = -16;
			*this + (menu = new sqPopupMenu(this,7,22,NITEMS,&sysfont,0,3));
			for(i = 0;i < MLTableSize;i++)
				*menu * new sqMenuBar((uchar*)MLTable[i] -> name,menu,MLTable[i] -> inUse);
			yadd += 16*MIN(NITEMS,MLTableSize);
			xadd = menu -> len*8 + 16;
			break;
		case E_MOBILOCFRAMECHOICE:
			yadd = -16;
			*this + (menu = new sqPopupMenu(this,7,22,NITEMS,&sysfont,0,3));
			for(i = 0;i < mlobj -> maxFrame;i++){
				tb.init();
				tb <= i < " frame (period " <= mlobj -> table[i].period < ")";
				*menu * new sqMenuBar((uchar*)(tb.GetBuf()),menu);
				}
			yadd += 16*MIN(NITEMS,mlobj -> maxFrame);
			xadd = menu -> len*8 + 16;
			break;
		case E_WORLDCHOICE:
			yadd = (vMap -> maxWorld - 1)*16;
			*this + (menu = new sqPopupMenu(this,7,22,NITEMS,&sysfont,0,3));
			for(i = 0;i < vMap -> maxWorld;i++) *menu * new sqMenuBar((uchar*)vMap -> wTable[i].name,menu);
			xadd = menu -> len*8 + 16;
			break;
		case E_VLTNTFORM:
			yadd = 3*24;
			*this + (params[0] = new sqField(this,"Z:",4,22 + 0*24,0,&sysfont,(uchar*)itoa(TntVLobj -> z,tmpstr,10),3,T_NUMERIC));
			*this + (params[1] = new sqField(this,"Value:",4,22 + 1*24,0,&sysfont,(uchar*)itoa(TntVLobj -> value,tmpstr,10),6,T_NUMERIC));
			*this + (params[2] = new sqField(this,"Radius:",4,22 + 2*24,0,&sysfont,(uchar*)itoa(TntVLobj -> radius,tmpstr,10),6,T_NUMERIC));
			*this + (params[3] = new sqField(this,"Type:",4,22 + 3*24,0,&sysfont,(uchar*)itoa(TntVLobj -> type,tmpstr,10),6,T_NUMERIC));

			*this + (params[4] = new sqField(this,"all-Z:",4 + 140,22 + 0*24,0,&sysfont,(uchar*)itoa(TntZall,tmpstr,10),3,T_NUMERIC));
			break;
		case E_VLMLCFORM:
			yadd = 1*24;
			*this + (params[0] = new sqField(this,"Z:",4,22 + 0*24,0,&sysfont,(uchar*)itoa(MLCVLobj -> z,tmpstr,10),3,T_NUMERIC));
			*this + (params[1] = new sqField(this,"Value:",4,22 + 1*24,0,&sysfont,(uchar*)itoa(MLCVLobj -> value,tmpstr,10),6,T_NUMERIC));
			break;
		case E_VLSNSFORM:
			yadd = 11*24;
			*this + (params[0] = new sqField(this,"Z:",4,22 + 0*24,0,&sysfont,(uchar*)itoa(SnsVLobj -> z,tmpstr,10),3,T_NUMERIC));
			*this + (params[1] = new sqField(this,"ID:",4,22 + 1*24,0,&sysfont,(uchar*)itoa(SnsVLobj -> id,tmpstr,10),9,T_NUMERIC));
			*this + (params[2] = new sqField(this,"Radius:",4,22 + 2*24,0,&sysfont,(uchar*)itoa(SnsVLobj -> radius,tmpstr,10),6,T_NUMERIC));
			*this + (name = new sqField(this,"Name: ",4,22 + 3*24,0,&sysfont,(uchar*)SnsVLobj -> name,32));

			*this + (params[3] = new sqField(this,"all-Z:",4 + 140,22 + 0*24,0,&sysfont,(uchar*)itoa(SnsZall,tmpstr,10),3,T_NUMERIC));
			name -> index = strlen(SnsVLobj -> name);

			*this + (params[4] = new sqField(this,"Z0:",4,22 + 4*24,0,&sysfont,(uchar*)itoa(SnsVLobj -> z0,tmpstr,10),3,T_NUMERIC));
			*this + (params[5] = new sqField(this,"Data0:",4,22 + 5*24,0,&sysfont,(uchar*)itoa(SnsVLobj -> data0,tmpstr,10),3,T_NUMERIC));
			*this + (params[6] = new sqField(this,"Data1:",4,22 + 6*24,0,&sysfont,(uchar*)itoa(SnsVLobj -> data1,tmpstr,10),3,T_NUMERIC));
			*this + (params[7] = new sqField(this,"Data2:",4,22 + 7*24,0,&sysfont,(uchar*)itoa(SnsVLobj -> data2,tmpstr,10),3,T_NUMERIC));
			*this + (params[8] = new sqField(this,"Data3:",4,22 + 8*24,0,&sysfont,(uchar*)itoa(SnsVLobj -> data3,tmpstr,10),3,T_NUMERIC));
			*this + (params[9] = new sqField(this,"Data4:",4,22 + 9*24,0,&sysfont,(uchar*)itoa(SnsVLobj -> data4,tmpstr,10),3,T_NUMERIC));
			*this + (params[10] = new sqField(this,"Data5:",4,22 + 10*24,0,&sysfont,(uchar*)itoa(SnsVLobj -> data5,tmpstr,10),3,T_NUMERIC));
			*this + (params[11] = new sqField(this,"Data6:",4,22 + 11*24,0,&sysfont,(uchar*)itoa(SnsVLobj -> data6,tmpstr,10),3,T_NUMERIC));
			break;
		case E_VLDNGFORM:
			yadd = 2*24;
			*this + (params[0] = new sqField(this,"Z:",4,22 + 0*24,0,&sysfont,(uchar*)itoa(DngVLobj -> z,tmpstr,10),3,T_NUMERIC));
			*this + (params[1] = new sqField(this,"Type:",4,22 + 3*24,0,&sysfont,(uchar*)itoa(DngVLobj -> type,tmpstr,10),6,T_NUMERIC));
			*this + (params[2] = new sqField(this,"Radius:",4,22 + 2*24,0,&sysfont,(uchar*)itoa(DngVLobj -> radius,tmpstr,10),6,T_NUMERIC));

			*this + (params[3] = new sqField(this,"all-Z:",4 + 140,22 + 0*24,0,&sysfont,(uchar*)itoa(DngZall,tmpstr,10),3,T_NUMERIC));
			break;
		}

	sy = 50 + yadd;
	sx = xadd;
	x = _x - sx/2; y = _y - sy/2;
	if(x < getX() + 4) x = getX() + 4;
	if(y < getY() + 2) y = getY() + 2;
	if(x + sx + 2 >= getRX()) x = getRX() - sx - 2;
	if(y + sy + 4 >= getRY()) y = getRY() - sy - 4;
	set();

	obj = tail;
	QuantObj = this;
	if(tail) tail -> activate(0);
	sqInputBox::draw();
}

void iInputForm::message(int code,sqElem* object)
{
	int i,max;
	switch(code){
		case M_CANCEL:
			VLediting = 0;
			return;
		case M_CHOICE:
		case M_SETOPTION:
			switch(mode){
				case E_OPENFORM:
				case E_SAVEFORM:
					mapFName = strdup(name -> getstr());
					break;
				case E_PLACEBMPFORM:
					placeBMP.place((char*)menu -> getbar(menu -> getptr(menu -> pointer)) -> original_data,atoi(params[0] -> getstr()),atoi(params[1] -> getstr()),atoi(params[2] -> getstr()),atoi(params[3] -> getstr()),atoi(params[4] -> getstr()));
					break;
				case E_IBMFORM:
					ibmFName = strdup(name -> getstr());
					ibmObj.save(atoi(params[0] -> getstr()),atoi(params[1] -> getstr()),atoi(params[2] -> getstr()),atoi(params[3] -> getstr()),atoi(params[4] -> getstr()),atoi(params[5] -> getstr()),atoi(params[6] -> getstr()),atoi(params[7] -> getstr()),atoi(params[8] -> getstr()),atoi(params[9] -> getstr()));
					break;
				case E_MOSAICFORM:
					mosaicBMP.copt = menu -> getptr(menu -> pointer);
					mosaicBMP.mode = atoi(params[1] -> getstr());
					mosaicBMP.force = atoi(params[2] -> getstr());
					mosaicBMP.xoffset = atoi(params[3] -> getstr());
					mosaicBMP.yoffset = atoi(params[4] -> getstr());
					for(i = 0;i < 8;i++)
						MosaicTypes[i] = atoi(params[5 + i] -> getstr());
					mosaicBMP.load((char*)menu -> getbar(menu -> getptr(menu -> pointer)) -> original_data);
					break;
				case E_IMPCHOICE:
					switch(curImpIndex = copt = menu -> getptr(menu -> pointer)){
						case IMP_SPHERE:
							sqE -> put(E_IMPSPHERE,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
							break;
						case IMP_POLYGON:
							sqE -> put(E_IMPPOLYGON,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
							break;
						}
					break;
				case E_BMPCHOICE:
					switch(curBmpIndex = copt = menu -> getptr(menu -> pointer)){
						case 0:
							sqE -> put(E_MOSAICFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
							break;
						case 1:
							sqE -> put(E_PLACEBMPFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
							break;
						case 2:
							sqE -> put(E_IBMFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
							break;
						}
					break;
				case E_IMPSPHERE:
					ImpPattern[curImpIndex] -> change(atoi(params[0] -> getstr()),atoi(params[1] -> getstr()),atoi(params[2] -> getstr()),atoi(params[3] -> getstr()),atoi(params[4] -> getstr()),atoi(params[5] -> getstr()),atoi(params[6] -> getstr()),0,0,atoi(params[7] -> getstr()),atoi(params[8] -> getstr()),atoi(params[9] -> getstr()),atoi(params[10] -> getstr()));
					break;
				case E_IMPPOLYGON:
					ImpPattern[curImpIndex] -> change(atoi(params[0] -> getstr()),atoi(params[1] -> getstr()),atoi(params[2] -> getstr()),atoi(params[3] -> getstr()),atoi(params[4] -> getstr()),atoi(params[5] -> getstr()),atoi(params[6] -> getstr()),atoi(params[9] -> getstr()),atoi(params[10] -> getstr()),atoi(params[7] -> getstr()),atoi(params[8] -> getstr()),atoi(params[11] -> getstr()),atoi(params[12] -> getstr()));
					break;
				case E_TRACKFORM:
					GroupToggle = atoi(params[0] -> getstr());
					NodeHeight = atoi(params[1] -> getstr());
					NodeSlope = atoi(params[2] -> getstr());
					NoiseLevel = atoi(params[3] -> getstr());
					NoiseAmp = atoi(params[4] -> getstr());
					ShapeNumber = atoi(params[5] -> getstr());
					ShapeAmp = atoi(params[6] -> getstr());
					TerrainMode = atoi(params[7] -> getstr());
					ColorBase = atoi(params[8] -> getstr());
					RenderMode = atoi(params[9] -> getstr());
					ReBuildWide = atoi(params[10] -> getstr());
					MaterSection = atoi(params[11] -> getstr());
					WideSection = atoi(params[12] -> getstr());

					if ( cBranch  ) {
						Track.set_section_parametr( cBranch, cPoint, NoiseLevel, ShapeNumber, NoiseAmp, ShapeAmp, GroupToggle, NodeHeight, NodeSlope, TerrainMode, ColorBase, RenderMode, WideSection, MaterSection);
						if ( LastBranch ) Track.lift_branch( cBranch );
						if (ReBuildWide) Track.recalc_wide_branch(cBranch,  ReBuildWide);
						ReBuildWide = 0;
						LastBranch = 0;
					}
					else Track.set_node_parametr( cNode, GroupToggle, NoiseLevel, NoiseAmp, NodeHeight, ColorBase, MaterSection, TerrainMode );
					TrackBuild = WAIT;

					break;
				case E_GOTOFORM:
					curGMap -> gotoxy(atoi(params[0] -> getstr()),atoi(params[1] -> getstr()));
					XGR_MouseSetPos(XGR_MAXX/2,XGR_MAXY/2);
//					if(mlobj) mlobj -> setPhase(atoi(params[0] -> getstr()));
					break;
				case E_FORM3D:
					S3Dcopt = menu -> getptr(menu -> pointer);
					S3Dmode = atoi(params[1] -> getstr());
					S3Dlevel = atoi(params[2] -> getstr());
					S3Dinverse = atoi(params[3] -> getstr());
					S3DnoiseLevel = atoi(params[4] -> getstr());
					S3DnoiseAmp = atoi(params[5] -> getstr());
					S3Dside = atoi(params[6] -> getstr());
					S3Dload((char*)menu -> getbar(menu -> getptr(menu -> pointer)) -> original_data);
					break;
				case E_MOBILOCMENU:
					switch(MLstatus){
						case 0:
							switch(copt = menu -> getptr(menu -> pointer)){
								case 0:
									MLreset();
									MLprocess = 1 - MLprocess;
									if(MLprocess && ibmObj.shoting == 2) ibmObj.shoting = 1;
									break;	
								case 1:
									MLbegin();
									break;
								case 2:
									if(MLTableSize) sqE -> put(E_MOBILOCCHOICE,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
									break;
								case 3:
									if(MLTableSize) sqE -> put(E_MOBILOCDELETECHOICE,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
									break;
								case 4:
									if(MLTableSize) sqE -> put(E_MOBILOCEDITCHOICE,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
									break;
								case 5:
									if(MLTableSize) sqE -> put(E_MOBILOCSHOWCHOICE,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
									break;
								}
							break;
						case 1:
							switch(copt = menu -> getptr(menu -> pointer)){
								case 0:
									MLadd();
									break;
								case 1:
									MLaccept();
									if(mlobj -> maxFrame) sqE -> put(E_MOBILOCFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
									break;
								case 2:
									MLcancel();
									break;
								}
							break;
						case 2:
							switch(copt = menu -> getptr(menu -> pointer)){
								case 0:
									sqE -> put(E_MOBILOCFRAMECHOICE,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
									break;
								case 1:
									MLacceptframe();
									break;
								case 2:
									MLrestoreframe();
									break;
								case 3:
									MLinsertframe();
									break;
								case 4:
									MLdeleteframe();
									break;
								case 5:
									sqE -> put(E_MOBILOCFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
									break;
								case 6:
									MLacceptediting();
									break;
								case 7:
									MLcancel();
									break;
								}
						}
					break;
				case E_MOBILOCFRAMECHOICE:
					MLgo(menu -> getptr(menu -> pointer));
					break;
				case E_VALOCMENU:
					switch(copt = menu -> getptr(menu -> pointer)){
						case 0:
						case 1:
						case 2:
						case 3:
							VLstart(copt);
							break;
						case 4:
							VLcancel();
							break;
						case 5:
							VLsave();
							break;
						}
					break;
				case E_MOBILOCFORM:
					mlobj -> removeFile();
					strcpy(mlobj -> name,name -> getstr());
					mlobj -> setDry(atoi(params[0] -> getstr()));
					mlobj -> Impulse = atoi(params[1] -> getstr());
					for(i = 0;i < MAX_KEYPHASE - 1;i++)
						mlobj -> setKeyPhase(i + 1,atoi(params[2 + i] -> getstr()));
					max = mlobj -> maxFrame;
					for(i = 0;i < max;i++)
						mlobj -> table[i].setPeriod(atoi(params[5 + i] -> getstr()));
					for(i = 0;i < max;i++)
						mlobj -> table[i].setSurftype(atoi(params[5 + max + i] -> getstr()));
					mlobj -> save();
					break;
				case E_MOBILOCCHOICE:
					mlobj = MLTable[menu -> getptr(menu -> pointer)];
					MLedit(menu -> getptr(menu -> pointer));
					break;
				case E_MOBILOCSHOWCHOICE:
					if(code == M_SETOPTION){
						menu -> pointer -> status = 1 - menu -> pointer -> status;
						if(menu -> pointer -> status) *menu -> pointer -> data = 4;
						else *menu -> pointer -> data = ' ';
						MLTable[menu -> getptr(menu -> pointer)] -> inUse = 1 - MLTable[menu -> getptr(menu -> pointer)] -> inUse;
						MLTable[menu -> getptr(menu -> pointer)] -> reset();
						draw();
						return;
						}
					else {
						mlobj = MLTable[menu -> getptr(menu -> pointer)];
						MLshow(menu -> getptr(menu -> pointer));
						}
					break;
				case E_MOBILOCDELETECHOICE:
					mlobj = NULL;
					MLdelete(menu -> getptr(menu -> pointer));
					break;
				case E_MOBILOCEDITCHOICE:
					if(code == M_SETOPTION){
						menu -> pointer -> status = 1 - menu -> pointer -> status;
						if(menu -> pointer -> status) *menu -> pointer -> data = 4;
						else *menu -> pointer -> data = ' ';
						MLTable[menu -> getptr(menu -> pointer)] -> inUse = 1 - MLTable[menu -> getptr(menu -> pointer)] -> inUse;
						MLTable[menu -> getptr(menu -> pointer)] -> reset();
						draw();
						return;
						}
					else {
						mlobj = MLTable[menu -> getptr(menu -> pointer)];
						mlobj -> reset();
						sqE -> put(E_MOBILOCFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
						}
					break;
				case E_WORLDCHOICE:
					MLreset();
					vMap -> reload(menu -> getptr(menu -> pointer));
					MLreload();
					PalettePrepare();
					curGMap -> FirstDraw = 1;
					curGMap -> title = getTitle();
					curGMap -> draw();
					Track.load(0);
					Redraw = 1;
					break;
				case E_VLTNTFORM:
					TntVLobj -> accept(atoi(params[0] -> getstr()),atoi(params[1] -> getstr()),atoi(params[2] -> getstr()),atoi(params[3] -> getstr()));
					TntZall = atoi(params[4] -> getstr());
					break;
				case E_VLMLCFORM:
					MLCVLobj -> accept(atoi(params[0] -> getstr()),atoi(params[1] -> getstr()));
					break;
				case E_VLSNSFORM:
					SnsVLobj -> accept(atoi(params[0] -> getstr()),atoi(params[1] -> getstr()),atoi(params[2] -> getstr()),name -> getstr(),atoi(params[4] ->getstr()),atoi(params[5] ->getstr()),atoi(params[6] ->getstr()),atoi(params[7] ->getstr()),atoi(params[8] ->getstr()),atoi(params[9] ->getstr()),atoi(params[10] ->getstr()),atoi(params[11] ->getstr()));
					SnsZall = atoi(params[3] -> getstr());
					break;
				case E_VLDNGFORM:
					DngVLobj -> accept(atoi(params[0] -> getstr()),atoi(params[1] -> getstr()),atoi(params[2] -> getstr()));
					DngZall = atoi(params[3] -> getstr());
					break;
				}
			VLediting = 0;
			close();
			return;
		}
	sqInputBox::message(code,object);
}

void iInputForm::flush(void)
{
	XGR_Flush(getX() + x,getY() + y,sx,sy);
}

void iInputForm::quant(void)
{
	if(mode == E_TRACKFORM)
		switch(sqTrackBuild){
			case SET_HEIGHT:
				Track.show_branch_height( cBranch, cPoint);
				break;
			case SET_HEIGHT_NODE:
				Track.show_node_height( cNode );
				break;
			}

	if (obj) {
		QuantObj = obj;
		obj->quant();
		QuantObj = this;
	}

	flush();
}

void iGameMap::setFloodLvl(void)
{ FloodLvl[CY >> WPART_POWER] = FloodLevel; }

void iGameMap::restoreFloodLvl(void)
{ FloodLevel = FloodLvl[CY >> WPART_POWER]; }

void iGameMap::handler(sqEvent* e)
{
	int mX, mY;
	mX = (e -> x - curGMap -> xc + curGMap -> CX) & clip_mask_x;
	mY = (e -> y - curGMap -> yc + curGMap -> CY) & clip_mask_y;

	if(TrackStatus)
		switch(e -> code){
			case E_LBMUNPRESS:
				do_left_buttion_off(mX,mY);
				break;
			case E_RBMUNPRESS:
				do_right_buttion_off(mX,mY);
				break;
			}
	else
		if(VLstatus)
			switch(e -> code){
				case E_LBMUNPRESS:
					VLlbpress(mX,mY);
					break;
				case E_RBMUNPRESS:
					if(!VLrbpress(mX,mY))
						curGMap -> shift(e -> x - curGMap -> xc,e -> y - curGMap -> yc);
					break;
				}
		else
			switch(e -> code){
				case E_LBMPRESS:
					switch(LBM_mode[0]){
						case FLOOD_TERRAIN:
							WORLD_Flood(mX,mY,FloodTerrainType << TERRAIN_OFFSET);
							vMap -> screenRender();
							break;
						case DRIFT_TERRAIN:
							WORLD_Drift(mX,mY,DriftLevel,DriftTerrainType << TERRAIN_OFFSET);
							vMap -> screenRender();
							break;
						case TOOLZER:
							isreEvent = 1;
							reEvent = E_LBMREEV;
							break;
						case GEONET:
							GeoSetZone(mX,mY,GeonetRadius,GeonetLevel,GeonetDelta);
							vMap -> screenRender();
							break;
						}
					break;
				case E_LBMREEV:
					if(LBM_mode[0] == TOOLZER) deltaZone(mX,mY,ToolzerRadius,ToolzerSmooth,ToolzerDelta,ToolzerSMode,ToolzerEquDelta);
					break;
				case E_LBMUNPRESS:
					if(LBM_mode[0] == TOOLZER) isreEvent = 0;
					break;
				case E_RBMPRESS:
					curGMap -> shift(e -> x - curGMap -> xc,e -> y - curGMap -> yc);
					break;
				}
}

static int checkStage(int* y,int n,int stage)
{
	int minY = map_size_y,maxY = 0;
	register int i,j;
	for(i = 0;i < n;i++){
		j = *y++;
		if(j > maxY) maxY = j;
		if(j < minY) minY = j;
		}
	if(minY > ((stage + 1) << WPART_POWER) || maxY < (stage << WPART_POWER)) return 0;
	return 1;
}

int eNode::linking(int stage)
{ return checkStage(yp,n_branch,stage); }

int eBranch::linking(int stage)
{
	int y[4],n;
	register int i,j,k;

	i = 0;
	rSection* ps = data_all;
	for(j = 0;j < n_section - 1;j++){
		n = data_base[j].point;
		for(k = 0;k < n;i++,k++){
			y[0] = ps -> yr;
			y[3] = ps -> yl;
			ps++;
			y[1] = ps -> yr;
			y[2] = ps -> yl;
			if(checkStage(y,n,stage)) return 1;
			}
		}
	return 0;
}

void sTrack::linking(void)
{
	eNode* l_node = node;
	eBranch* l_branch = branch;
	register int i,j;

	memset(nTails,0,PART_MAX*sizeof(eNode*));
	memset(bTails,0,PART_MAX*sizeof(eBranch*));

	build_all_spline();

	for(j = 0;j < (int)PART_MAX;j++){
		for(i = 0;i < n_branch;i++){
			if(l_branch -> linking(j)){
				l_branch -> next = bTails[j];
				bTails[j] = l_branch;
				}
			l_branch = l_branch -> r;
			}
		for(i = 0;i < n_node;i++){
			if(l_node -> linking(j)){
				l_node -> next = nTails[j];
				nTails[j] = l_node;
				}
			l_node = l_node -> r;
			}
		}
	linkingLog = 1;
}

struct LayFormat {
	int mode;
	const char* string;
	int* prm;
	int min,max;
	};

const int LAYMAX = 9;
const char* LayEditStr[2] = {
	"Begin editing the second layer",
	"Accept the created layer"
	};

LayFormat LayItem[PRMMAX0] = {
		{-1,LayEditStr[0],    NULL, 0,0 	   },
		{-1,"Cancel layer editing",    NULL, 0,0            },
		{-1,"Show hidden layer",    NULL, 0,0            },

		{ 0,"Up-Layer Rendering/Editing", NULL, 0,0            },
		{ 1,"Down-Layer Rendering/Editing", NULL, 0,0            },

		{-1,"Tunnel Proof ",       &TunnelProof,  0,255 },
		{-1,"Tunnel Height ",       &TunnelHeight,  0,MAX_RDELTA },
		{-1,"Exclusive Layer Mode ", &ExclusiveLayerMode, 0,1            },
		{-1,"Second Layer Priority ", &SecondLayerPriority, 0,1            }
		};

iLayerMenu::iLayerMenu(sqElem* _owner,int _x,int _y)
: sqInputBox(_owner,_x,_y,10,10,&sysfont)
{
	x0 = _x; y0 = _y;

	if(LayerStatus == UP_LAYER) LayItem[0].string = LayEditStr[0];
	else LayItem[0].string = LayEditStr[1];

	buf = new XBuffer;

	*this + (menu = new sqPopupMenu(this,4,4,LAYMAX,&sysfont,0,2));

	register int i;
	for(i = 0;i < LAYMAX;i++){
		buf -> init();
		*buf < LayItem[i].string;
		if(LayItem[i].prm) *buf < " " <= *(LayItem[i].prm);
		*menu * new sqMenuBar((uchar*)((*buf).GetBuf()),menu,LayerMode == LayItem[i].mode);
		}

	sy = font -> sy*LAYMAX + 12;
	sx = font -> sx*menu -> len + 12;
	x = _x - sx/2; y = _y - sy/2;
	if(x < getX() + 4) x = getX() + 4;
	if(y < getY() + 2) y = getY() + 2;
	if(x + sx + 2 >= getRX()) x = getRX() - sx - 2;
	if(y + sy + 4 >= getRY()) y = getRY() - sy - 4;
	set();

	menu -> setpointer(menu -> getbar(copt),0);

	QuantObj = obj = tail;
	tail -> activate(0);
	sqInputBox::draw();
}

void iLayerMenu::message(int code,sqElem* object)
{
	switch(code){
		case M_CHOICE:
		case M_SETOPTION:
			copt = menu -> getptr(menu -> pointer);
			if(LayItem[copt].mode >= 0) LayerMode = LayItem[copt].mode;
			switch(menu -> getptr(menu -> pointer)){
				case 0:
					if(LayerStatus == UP_LAYER) BeginSecondLayerEditing();
					else AcceptSecondLayer();
					break;
				case 1:
					if(LayerStatus == DOWN_LAYER) CancelSecondLayerEditing();
					break;
				case 2:
					ShowHiddenLayer();
					break;
				case 3:
					RenderingLayer = UP_LAYER;
					curGMap -> message(M_SETOBJ,curGMap -> knobRender);
					break;
				case 4:
					RenderingLayer = DOWN_LAYER;
					curGMap -> message(M_SETOBJ,curGMap -> knobRender);
					break;
				}
			close();
			return;
		}
	sqInputBox::message(code,object);
}

void iLayerMenu::flush(void)
{
	XGR_Flush(getX() + x,getY() + y,sx,sy);
}

void iLayerMenu::keytrap(int key)
{
	const int PAUSE = 10;
	static int timer;
	int n;
	switch(key){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			n = menu -> getptr(menu -> pointer);
			if(CLOCK() - timer > PAUSE)
				*(LayItem[n].prm) = key - '0';
			else
				*(LayItem[n].prm) = (*(LayItem[n].prm))*10 + key - '0';
			if(*(LayItem[n].prm) < LayItem[n].min) *(LayItem[n].prm) = LayItem[n].min;
			if(*(LayItem[n].prm) > LayItem[n].max) *(LayItem[n].prm) = LayItem[n].max;
			buf -> init();
			(*buf) < LayItem[n].string <= *(LayItem[n].prm);
			menu -> pointer -> replace(menu,(uchar*)((*buf).GetBuf()));
			draw();
			timer = CLOCK();
			break;
		case SDLK_MINUS:
			n = menu -> getptr(menu -> pointer);
			*(LayItem[n].prm) = -(*(LayItem[n].prm));
			if(*(LayItem[n].prm) < LayItem[n].min) *(LayItem[n].prm) = LayItem[n].min;
			if(*(LayItem[n].prm) > LayItem[n].max) *(LayItem[n].prm) = LayItem[n].max;
			buf -> init();
			(*buf) < LayItem[n].string <= *(LayItem[n].prm);
			menu -> pointer -> replace(menu,(uchar*)((*buf).GetBuf()));
			draw();
			break;
		case SDLK_LEFT:
			n = menu -> getptr(menu -> pointer);
			if(LayItem[n].prm){
				buf -> init();
				if(--(*(LayItem[n].prm)) == LayItem[n].min - 1) *(LayItem[n].prm) = LayItem[n].max;
				(*buf) < LayItem[n].string <= *(LayItem[n].prm);
				menu -> pointer -> replace(menu,(uchar*)((*buf).GetBuf()));
				draw();
				}
			break;
		case SDLK_RIGHT:
			n = menu -> getptr(menu -> pointer);
			if(LayItem[n].prm){
				buf -> init();
				if(++(*(LayItem[n].prm)) == LayItem[n].max + 1) *(LayItem[n].prm) = LayItem[n].min;
				(*buf) < LayItem[n].string <= *(LayItem[n].prm);
				menu -> pointer -> replace(menu,(uchar*)((*buf).GetBuf()));
				draw();
				}
			break;
		default:
			sqInputBox::keytrap(key);
		}
}

void ibmFile::save(int _mapx_center,int _mapy_center,int _x_center,int _y_center,int _x_size,int _y_size,int _x_side,int _y_side,int _level,int _counter)
{
	counter = _counter;
	if(counter > 0){
		shoting = 2;
		fcnt = 0;
		}
	else{ 
		counter = 0;
		shoting = 0;
		}

	mapx_center = _mapx_center; mapy_center = _mapy_center;
	x_center = _x_center; y_center = _y_center;
	x_size = _x_size; y_size = _y_size;
	x_side = _x_side; y_side = _y_side;
	level = _level;

	if(_level == -1){
		XStream fff("BCKGRND.DAT",XS_OUT);
		int x0 = mapx_center - x_size/2;
		int y0 = mapy_center - y_size/2;

		vMap -> change(y0,y0 + y_size);
		for(int i = y0;i < y0 + y_size;i++) fff.write(vMap -> lineTcolor[i] + x0,x_size);
		fff.close();
		return;
		}
	
	XStream ff(ibmFName,XS_OUT);
	ff <  mapx_center < mapy_center < x_center < y_center < x_size < y_size < x_side < y_side;
	
	int x0 = mapx_center - x_size/2;
	int y0 = mapy_center - y_size/2;

	vMap -> change(y0,y0 + y_size);
	register int i,j;
	for(i = y0;i < y0 + y_size;i++) ff.write(vMap -> lineTcolor[i] + x0,x_size);

	int sx = 2*x_side + 1;
	int sy = 2*y_side + 1;
	uchar* p = new uchar[sx];
	uchar* pa;
	uchar* pc;
	for(i = y0 + y_center - y_side;i <= y0 + y_center + y_side;i++){
		pa = vMap -> lineT[i] + x0 + x_center - x_side;
		pc = vMap -> lineTcolor[i] + x0 + x_center - x_side;
		memset(p,0,sx);	
		for(j = 0;j < sx;j++)
			if(pa[j] > level) p[j] = pc[j];
			else pc[j] = 0;
		ff.write(p,sx);	
		}
	
	delete[] p;
	ff.close();
}

void ibmFile::quantSave(void)
{
	XBuffer buf;
	buf < "screen" <= fcnt++ < ".BMP";
	XStream fff(buf.GetBuf(),XS_OUT);
	fff < (short)x_size < (short)y_size;
	
	int x0 = mapx_center - x_size/2;
	int y0 = mapy_center - y_size/2;

	vMap -> change(y0,y0 + y_size);
	for(int i = y0;i < y0 + y_size;i++) fff.write(vMap -> lineTcolor[i] + x0,x_size);
	fff.close();

	if(!--counter) shoting = 0;
}
