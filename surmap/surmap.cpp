#include "../src/global.h"

//#define SCREENSHOT

#ifdef _NT
#include "..\root\win32f.h"
#endif
#include "runtime.h"

#include "sqint.h"

#include "../src/common.h"
#include "sqexp.h"
#include "tools.h"
#include "track.h"

#include "impass.h"
#include "../src/terra/vmap.h"
#include "../src/terra/world.h"
#include "../src/terra/render.h"
#include "../src/units/moveland.h"
#include "../src/palette.h"

#include "../src/3d/3d_math.h"
#include "../src/3d/3dgraph.h"
#include "../src/3d/3dobject.h"

#include "missed.h"

#ifndef DBGCHECK
#define DBGCHECK
#endif

#define RANDOMIZE

struct RGBcolor{ uchar r,g,b; };

/* ----------------------------- EXTERN SECTION ---------------------------- */
extern uchar markBMP[7][7];
extern int sssInuse;
extern int RenderingLayer;
extern int MLstatus;
extern int MLvisi;
extern int LayerStatus;
extern int ViewX;
extern int ViewY;
extern int SlopeAngle;
extern int height_of_object;
extern int thickness_of_object;

extern int TrackBuild;
extern int Track_show_all;
extern int TrackStatus;

extern int LandBounded;
extern int DeltalH;
extern int DeltamH;
extern int catch_log;

extern ibmFile ibmObj;
extern int VisiRegR;
extern MobileLocation* mlobj;
extern int MLstatus,MLprocess;

extern int VLstatus;
extern int DEFAULT_TERRAIN;

/* --------------------------- PROTOTYPE SECTION --------------------------- */
void ComlineAnalyze(int argc,char** argv);
void restore(void);
int sqrint(int x);
unsigned RND(unsigned m);
void showF(int x0,int y0,int x1,int y1);
void showCell(void);
void showRadar(void);
void costab(void);
void statistic(void);
void cycleTor(int& x,int& y);
void PalettePrepare(void);
void KeyCenter(int key);
char* GetMMode(void);
void sqStateSave(XStream& ff);
void sqStateLoad(XStream& ff);
int shape_control(int code);
void view_shape(void);
int MLquant(void);
void MLadd(void);
void analyzeColors(void);
void graph3d_init();
void calc_screen(int zoom_percent);
void shotMake(void);
void loadState(void);
char* getVLname(void);
void VLshow(void);
void VLnext(void);
void ClipboardOperation(int slot,int save,int render = 1);
void MLacceptframe(void);
void MLrestoreframe(void);
void MLinsertframe(void);
void MLdeleteframe(void);
void MLnextframe(void);
void MLprevframe(void);
void ClipboardInit(void);

extern "C" {
	int distance(int,int);
	};
/* --------------------------- DEFINITION SECTION -------------------------- */
const int DELTA = 16;

int XSIDE;
int YSIDE;
int XSIZE,YSIZE;

XBuffer msg_buf(1);

int ErrHExcept = 1;
int RAM16;
int MainRqCounter,AuxRqCounter;
int Quit;
int debugging;
XBuffer dbuf;

unsigned char* palbuf,*palbufA,*palbufC,*palbufOrg;
sqFont sysfont;
int* SI;
int* CO;

RGBcolor RGBpal[256];

unsigned RNDVAL = 83;
unsigned realRNDVAL = 83;
int Geo = 1;
int Redraw = 1;
int gen = 1;
int Regen;

int RestoreLog,CGenLog,BorderLog,MobilityLog,InitLog,SSSLog,ROLog,EncodeLog,DirectLog,ExclusiveLog = 1,ShotLog,GlassLog,CutvmpLog,WHLog;
int ForcedCompressed,ForcedUncompressed;
int ConvertLog;
int WaterPrm = -1;
int videoMode = 2;

const char* mapFName = "wrlds.dat";
int ColorShow = 1;
int DepthShow = 0;

sTrack Track;

int MIN_WIDE = 30;
int H_MAIN = 16;

int MouseMode = 1;

int GridLog;
int ShowVoxel = 0;
int WorldPrm = -1;
int ZoomLog;
int ShotPrm = V_SIZE - 1,GlassPrm;
int StartshotPrm;
int MLstepLog,keyP;

int prevWorld,preCX,preCY;

int paliterMode;

int emode;

extern int __internal_argc;
extern char **__internal_argv;

int xtInitApplication(void)
{
	XCon < "SURMAP Editor v3.03-Win32/DirectX by K-D LAB (C)1996-97. All Rights Reserved.\n";
	XCon < "Compilation: DATE: " < __DATE__ < " TIME: " < __TIME__ < "\n\n";
#ifdef TERRAIN16
	XCon < "-TERRAIN MAX 16 VERSION-\n\n";
#endif

#ifdef _NT
	if(!win32_SingleCheck()) ErrH.Abort("Only one uniVang Instance available...");
	win32_SetPriorityProcess(NORMAL_PRIORITY_CLASS);
	XCon < "Priority: " <= win32_GetProcessPriority() < "\n";

#ifdef _DEBUG
	win32_debugSet();
#endif
#endif

	ComlineAnalyze(__internal_argc, __internal_argv);
	//@caiiiycuk: should we ever fix this?
	//ErrH.SetRestore(restore);
	//ErrH.SetFlags(XERR_CTRLBRK);

	XKey.init(NULL,NULL);

#ifdef RANDOMIZE
	RNDVAL = clock();
	realRNDVAL = clock();
#else
	RNDVAL = 83;
	realRNDVAL = 838383;
#endif

	loadState();

	XCon < "\nPrecalculating...";
	costab();
	ImpassPrepare();
	landPrepare();

	XCon < "\nMAP loading:";
	vMapPrepare(mapFName,prevWorld);
	vMap->reload(prevWorld);
	XCon < "\nMESH: " <= MESH < "\n";

	PalettePrepare();

	XStream ffo("SURMAP.FNT",XS_IN);
	void* fo = new char[ffo.size()];
	ffo.read(fo,ffo.size());
	ffo.close();
	sysfont.init(fo);

	emode = ExclusiveLog ? XGR_EXCLUSIVE : 0;

	switch(videoMode){
		case 0:
			if(XGR_Init(640,480,emode)) ErrH.Abort("Error video initialization");
			break;
		case 1:
			if(XGR_Init(800,600,emode)) ErrH.Abort("Error video initialization");
			break;
		case 2:
			if(XGR_Init(1280,720,emode)) ErrH.Abort("Error video initialization");
			break;
		}

	XSIDE = XGR_MAXX/2 - 4;
	YSIDE = (XGR_MAXY - 10 - 22 - 16)/2;
	XSIZE = 2*XSIDE;
	YSIZE = 2*YSIDE + 1;

	XGR_Fill(0);

	XKey.finit();
	XKey.init(KeyCenter,NULL);

	graph3d_init();
	calc_screen(100);

	Track.load(0);

	sqInit(256 - 8);
	sqSetPalette((char*)palbuf);

	KeyTrapObj = sqScr;
	sqScr -> draw();
	sqE -> put(E_MAINMAP,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);

	xtCreateRuntimeObjectTable(RTO_MAX_ID);
	SurmapRTO* surmapObj = new SurmapRTO;
	xtRegisterRuntimeObject(surmapObj);

//	XGR_MouseShow();
	XGR_SetPal(palbuf,0,256);
	return RTO_SURMAP_ID;
}

int SurmapRTO::Quant(void)
{
	sqQuant();
	if (TrackStatus) Track.curr_build();
	if(Redraw){
		XGR_Flush(0,0,XGR_MAXX,XGR_MAXY);
		Redraw = 0;
		}
//	XGR_MouseRedraw();
	static int cnt = CLOCK();
	if(paliterMode && ColorShow && CLOCK() != cnt){
		cnt = CLOCK();
		pal_iter();
		}
	int ret = Quit;
	Quit = 0;
	return ret;
}

void xtDoneApplication(void)
{
	vMap -> flush();
	vMap -> finit();
	
	restore();
}

void restore(void)
{
	XStream ff("state.dat",XS_OUT);
	if(curGMap) ff < curGMap -> CX < curGMap -> CY;
	else ff < (int)0 < (int)0;
	sqStateSave(ff);
	ff < vMap -> cWorld;
	ff.close();
}

void PalettePrepare(void)
{
	int first = 0;
	if(!palbufC){
		first = 1;
		palbufC = new unsigned char[768];
		palbufA = new unsigned char[768];
		palbufOrg = new unsigned char[768];
		palbuf = palbufA;
		memset(palbuf,0,768);
		for(int i = 0;i < 64;i++){
			palbuf[(i + 1)*3 + 0] = i;
			palbuf[(i + 1)*3 + 1] = i;
			palbuf[(i + 1)*3 + 2] = i;
			}
		}

	XStream fpal(GetTargetName(vMap -> paletteName),XS_IN);
	fpal.read(palbufC,768);
	palbufC[0] = palbufC[1] = palbufC[2] = 0;
//	palbufC[3*BEGCOLOR[1]] = palbufC[3*BEGCOLOR[1] + 1] = palbufC[3*BEGCOLOR[1] + 2] = 1;
	fpal.close();
/*
	for(int i = 0;i < 10;i++){
		palbufC[3*(BEGCOLOR[7] + i)] = 7 + i*6;
		palbufC[3*(BEGCOLOR[7] + i) + 1] = 0;
		palbufC[3*(BEGCOLOR[7] + i) + 2] = 0;

		palbufC[3*(BEGCOLOR[7] + 10 + i)] = 7 + 9*6;
		palbufC[3*(BEGCOLOR[7] + 10 + i) + 1] = 7 + i*6;
		palbufC[3*(BEGCOLOR[7] + 10 + i) + 2] = 0;

		palbufC[3*(ENDCOLOR[7] - i)] = 7 + i*6;
		palbufC[3*(ENDCOLOR[7] - i) + 1] = 7 + i*6;
		palbufC[3*(ENDCOLOR[7] - i) + 2] = 0;
		}
*/

	for(int i = 0;i < TERRAIN_MAX;i++){
		palbufC[3*BEGCOLOR[i] + 0] >>= 1;
		palbufC[3*BEGCOLOR[i] + 1] >>= 1;
		palbufC[3*BEGCOLOR[i] + 2] >>= 1;
		}
	
	palbuf = palbufC;
	memcpy(palbufOrg,palbuf,768);

#ifdef _DEBUG
	memcpy(RGBpal,palbuf,768);
//	XStream fp("pals",XS_OUT);
//	for(int i = 0;i < 128;i++){
//		if(!(i%16)) fp < "\r\n";
//		fp <= i < "\t\t" <= RGBpal[i].r < " " <= RGBpal[i].g < " " <= RGBpal[i].b < "\r\n";
//		}
//	for(i = 0;i < 31;i++){
//		palbuf[(i + 1)*3 + 0] = 2*i;
//		palbuf[(i + 1)*3 + 1] = 2*i;
//		palbuf[(i + 1)*3 + 2] = i;
//		}
//	for(int i = 0;i < 32;i++){
//		palbuf[(i + 32)*3 + 0] = 10;
//		palbuf[(i + 32)*3 + 1] = i;
//		palbuf[(i + 32)*3 + 2] = 0;
//		}
#endif
	if(!first){
		sqSetPalette((char*)palbuf);
		XGR_SetPal(palbuf,0,256);
		}
}

void ComlineAnalyze(int argc,char** argv)
{
	int i,j;
	for(i = 1;i < argc;i++)
		if(argv[i][0] == '/'){
			j = 0;
			while(argv[i][j] == '/' || argv[i][j] == '-'){
				switch(argv[i][j + 1]){
					case '?':
						XCon < "  Using: SURMAP.EXE [PRM] [Options]\n";
						XCon < "Options:\n";
						XCon < "\t/In\t\t - force new VMP initing with n as MAP_POWER_Y\n";
						XCon < "\t/C\t\t - color rendering\n";
						XCon < "\t/R\t\t - original landscape from current geonet restoring\n";
						XCon < "\t/B\t\t - white debug world border placing\n";
						XCon < "\t/S\t\t - last session updating\n";
						XCon < "\t/O\t\t - read-only working\n";
						XCon < "\t/D\t\t - direct writing (w/o sessions)\n";
						XCon < "\t/L\t\t - low memory requesting\n";
						XCon < "\t/Wn\t\t - global setting water level to <n>\n";
						XCon < "\t/Mn\t\t - set mesh parameter to <n>\n";
						XCon < "\t/Gn\t\t - set world number to <n>\n";
						XCon < "\t/Tn\t\t - write to flat picture (.BMP) n lines\n";
						XCon < "\t/Un\t\t - start line of shoting\n";
						XCon < "\t/Nn\t\t - glass handling, remove glass if(n) \n";
						XCon < "\t/E\t\t - update CUTVMP world from current by CUTVMP()\n";
						XCon < "\t/H\t\t - world common handling\n";
						XCon < "\t/J\t\t - default terrain type\n";
						XCon < "\t/1\t\t - SVGA 800x600\n";
						XCon < "\t/2\t\t - SVGA 1024x768\n";
						ErrH.Exit();
					case 'r':
					case 'R':
//						RestoreLog = 1;
						break;
					case 'c':
					case 'C':
						CGenLog = 1;
						break;
					case 'b':
					case 'B':
						BorderLog = 1;
						break;
					case 'i':
					case 'I':
						InitLog = 1;
						break;
					case 's':
					case 'S':
						SSSLog = 1;
						break;
					case 'o':
					case 'O':
						ROLog = 1;
						break;
					case 'd':
					case 'D':
						DirectLog = 1;
						break;
					case 'W':
					case 'w':
						WaterPrm = atoi(argv[i] + j + 2);
						j += strlen(argv[i] + j + 2);
						break;
					case 'G':
					case 'g':
						WorldPrm = atoi(argv[i] + j + 2);
						j += strlen(argv[i] + j + 2);
						break;
					case 'A':
					case 'a':
						ExclusiveLog = 0;
						break;
					case 'H':
					case 'h':
						WHLog = 1;
						break;
					case '1':
						videoMode = 1;
						break;
					case '2':
						videoMode = 2;
						break;
					case '#':
						ConvertLog = 1;
						break;
					case 'T':
					case 't':
						ShotLog = 1;
						ShotPrm = atoi(argv[i] + j + 2);
						j += strlen(argv[i] + j + 2);
						break;
					case 'U':
					case 'u':
						StartshotPrm = atoi(argv[i] + j + 2);
						j += strlen(argv[i] + j + 2);
						break;
					case 'N':
					case 'n':
						GlassLog = 1;
						GlassPrm = atoi(argv[i] + j + 2);
						j += strlen(argv[i] + j + 2);
						break;
					case 'E':
					case 'e':
						CutvmpLog = 1;
						break;
					case '-':
						ForcedCompressed = 1;
						break;
					case '=':
						ForcedUncompressed = 1;
						break;
				}
				j+=2;
				}
			}
		else
			mapFName = strupr(strdup(argv[i]));

	if(RestoreLog && CGenLog) ErrH.Abort("Disable concurrent parameters");
}

void costab(void)
{
	SI = new int[PIx2];
	CO = new int[PIx2];
	for(int i = 0;i < PIx2;i++){
		SI[i] = (int)(UNIT*sin(((double)i)/PIx2*M_PI*2));
		CO[i] = (int)(UNIT*cos(((double)i)/PIx2*M_PI*2));
		}
}

void KeyCenter(int key)
{
	if(key == SDLK_ESCAPE){
		sqKey -> top = sqKey -> bottom;
		sqKey -> put(key);
		}
	else
		if(key != SDLK_LSHIFT && key != SDLK_LCTRL)
			sqKey -> put(key);
		else
			if(key == SDLK_LCTRL) keyP = 1;
}

void photo(void)
{
	static int cnt = 0;
	const int SX = XGR_MAXX;
	const int SY = XGR_MAXY;

	XBuffer buf;
	buf < "Photo_" <= cnt++ < ".bmp";
	XStream ff(buf.GetBuf(), XS_OUT);
	ff < (short)SX < (short)SY;

	vMap -> change(ViewY - SY/2,ViewY + SY/2);

	uchar* line = new uchar[SX],*p;

	register int i,j,sx,sy = SY;
	for(j = YCYCL(ViewY - SY/2);sy--;j = YCYCL(j + 1)){
		p = line; sx = SX;
		for(i = XCYCL(ViewX - SX/2);sx--;i = XCYCL(i + 1))
			*p++ = *(vMap -> lineTcolor[j] + i);
		ff.write(line,SX);
		}
	ff.close();
	delete[] line;
}

void sqScreen::draw(int self)
{
	XGR_Fill(SQ_SYSCOLOR + 3);

#ifndef SCREENSHOT
	for(int i = 0;i < 256;i++) XGR_LineTo(5 + i,XGR_MAXY - 16,14,3,i);
#endif

	sqElem::draw();
	Redraw = 1;
}

void sqScreen::keytrap(int key)
{
	switch(key){
		case SDLK_RETURN:
			sqE -> put(E_MAINMENU,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
			break;
		case SDLK_F12:
			DBGCHECK
			break;
		case SDLK_F11:
//			shotMake();
			photo();
			break;
		}
}

char* getTitle(void)
{
	static XBuffer buf;
	buf.init();
	buf < "<SURMAP Editor (C) by K-D Lab> Current World: [" < vMap -> wTable[vMap -> cWorld].name < "]";
	return buf.GetBuf();
}

void sqScreen::handler(sqEvent* e)
{
	switch(e -> code){
		case E_MAINMAP:
			*this + (curGMap = new iGameMap(this,e -> x,e -> y,XSIDE,YSIDE,getTitle()));
			Redraw = 1;
			break;
		case E_MAINMENU:
			*curGMap + new iMainMenu(curGMap,e -> x,e -> y);
			Redraw = 1;
			break;
		case E_PRMMENU:
			*curGMap + new iPrmMenu(curGMap,e -> x,e -> y);
			Redraw = 1;
			break;
		case E_LAYERMENU:
			*curGMap + new iLayerMenu(curGMap,e -> x,e -> y);
			Redraw = 1;
			break;
		case E_REFRESH:
			curGMap -> draw(0);
			Redraw = 1;
			break;
		case E_MUFLER:
			break;
		default:
			if(e -> code >= E_MAINMAP)
				*curGMap + new iInputForm(curGMap,e -> x,e -> y,e -> code);
			Redraw = 1;
			break;
		}
}

void GotoXY(int x,int y)
{
	curGMap -> gotoxy(x,y);
}

void iGameMap::keytrap(int key)
{
	if(shape_control(key)) return;

	int xm,ym;
	switch(key){
		case SDLK_ESCAPE:
			if ( TrackBuild == SET_HEIGHT || TrackBuild == SET_HEIGHT_NODE)
				TrackBuild = WAIT;
			break;
		case SDLK_LEFTBRACKET:
			curGMap -> change(-18,-10);
			break;
		case SDLK_RIGHTBRACKET:
			curGMap -> change(18,10);
			break;
		case SDLK_DELETE:
			if(!MLstatus)
				if(XKey.Pressed(SDLK_LSHIFT)) CX--;
				else if(XKey.Pressed(SDLK_LCTRL)) CX -= 10;
				else CX -= DELTA;
			else {
				LayerStatus = UP_LAYER;
				vMap -> refresh();
				LayerStatus = DOWN_LAYER;
				}
			break;
		//@caiiiycuk: was VK_NEXT
	    case SDLK_RETURN2:
			if(!MLstatus)
				if(XKey.Pressed(SDLK_LSHIFT)) CX++;
				else if(XKey.Pressed(SDLK_LCTRL)) CX += 10;
				else CX += DELTA;
			break;
		case SDLK_HOME:
			if(!MLstatus)
				if(XKey.Pressed(SDLK_LSHIFT)) CY--;
				else if(XKey.Pressed(SDLK_LCTRL)) CY -= 10;
				else CY -= DELTA;
			break;
		case SDLK_END:
			if(!MLstatus)
				if(XKey.Pressed(SDLK_LSHIFT)) CY++;
				else if(XKey.Pressed(SDLK_LCTRL)) CY += 10;
				else CY += DELTA;
			break;
		case SDLK_LEFT:
			if(MLstatus == 2){
				MLprevframe();
				break;
				}
			if(ShowVoxel){
				TurnAngle += 32;
				break;
				}
			if(TrackBuild == SET_HEIGHT){
				DeltalH = -1;
				if(XKey.Pressed(SDLK_LCTRL)) DeltalH = -10;
				}
			break;
		case SDLK_RIGHT:
			if(MLstatus == 2){
				MLnextframe();
				break;
				}
			if(ShowVoxel){
				TurnAngle -= 32;
				break;
				}
			if(TrackBuild == SET_HEIGHT){
				if(XKey.Pressed(SDLK_LCTRL)) DeltalH = 10;
				else DeltalH = 1;
				}
			break;
		case SDLK_UP:
			if(ShowVoxel){
				SlopeAngle += 16;
				break;
				}
			if ( TrackBuild == SET_HEIGHT || TrackBuild == SET_HEIGHT_NODE){
				if(XKey.Pressed(SDLK_LCTRL)) DeltamH = 10;
				else DeltamH = 1;
				}
			break;
		case SDLK_DOWN:
			if(ShowVoxel){
				SlopeAngle -= 16;
				break;
				}
			if(TrackBuild == SET_HEIGHT || TrackBuild == SET_HEIGHT_NODE){
				DeltamH = -1;
				if(XKey.Pressed(SDLK_LCTRL)) DeltamH = -10;
				}
			break;
		case SDLK_MINUS:
		case SDLK_KP_MINUS:
			if(XKey.Pressed(SDLK_LCTRL))
				TurnSecX += 50;
			else
				TurnSecX += 5;
			break;
		case SDLK_EQUALS:
		case SDLK_KP_PLUS:
			if(XKey.Pressed(SDLK_LCTRL))
				TurnSecX -= 50;
			else
				TurnSecX -= 5;
			break;
		case SDLK_INSERT:
			if(MLstatus){
				MLadd();
				LayerStatus = UP_LAYER;
				vMap -> refresh();
				LayerStatus = DOWN_LAYER;
				}
			break;
		case '1':
			if(TrackStatus)
				if(!XKey.Pressed(SDLK_LSHIFT)) Track.save(1);
				else Track.load(1);
			break;
		case '2':
			if(TrackStatus)
				if(!XKey.Pressed(SDLK_LSHIFT)) Track.save(2);
				else Track.load(2);
			break;
		case '3':
			if(TrackStatus)
				if(!XKey.Pressed(SDLK_LSHIFT)) Track.save(3);
				else Track.load(3);
			break;
		case '4':
			if(TrackStatus)
				if(!XKey.Pressed(SDLK_LSHIFT)) Track.save(4);
				else Track.load(4);
			break;
		case '5':
			if(TrackStatus)
				if(!XKey.Pressed(SDLK_LSHIFT)) Track.save(5);
				else Track.load(5);
			break;
		case 'F':
			if(prmFlag & PRM_INFO)
				prmFlag ^= PRM_INFO;
			else
				prmFlag |= PRM_INFO;
			break;
		case SDLK_SPACE:
			if(TrackStatus)
				message(M_SETOBJ,knobSpline);
			else
				if(VLstatus)
					VLnext();
				else {
					xm = XGR_MouseObj.PosX;
					ym = XGR_MouseObj.PosY;
					ImpPattern[curImpIndex] -> put((xm - xc + CX) & clip_mask_x,(ym - yc + CY) & clip_mask_y);
					}
			break;
		case 'S':
			if(TrackStatus)
				Track.save(0);
			else
				sqE -> put(E_FORM3D,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
			break;
		case '9':
			Track_show_all ^= 1;
			break;
		case SDLK_F1:
		case SDLK_F2:
		case SDLK_F3:
		case SDLK_F4:
		case SDLK_F5:
		case SDLK_F6:
		case SDLK_F7:
		case SDLK_F8:
		case SDLK_F9:
		case SDLK_F10:
			if(XKey.Pressed(SDLK_LCTRL))
				ClipboardOperation(key - SDLK_F1,1);
			else
				ClipboardOperation(key - SDLK_F1,0);
			break;
		case 'T':
			message(M_SETOBJ,knobTrack);
			break;
		case 'R':
			message(M_SETOBJ,knobRender);
			break;
		case 'M':
			sqE -> put(E_PRMMENU,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
			break;
		case 'O':
			if(XKey.Pressed(SDLK_LCTRL))
				MLvisi = 1 - MLvisi;
			else
				sqE -> put(E_MOBILOCMENU,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
			break;
		case 'C':
			sqE -> put(E_VALOCMENU,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
			break;
		case SDLK_SLASH:
			VisiRegR = 1 - VisiRegR;
			break;
		case 'E':
			paliterMode = 1 - paliterMode;
			break;
		case SDLK_TAB:
			GridLog = 1 - GridLog;
			break;
		case 'B':
			if(TrackStatus){
				message(M_SETOBJ,knobBuild);
				message(M_SETOBJ,knobRender);
				}
			else
				sqE -> put(E_BMPCHOICE,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
			break;
		case 'I':
			if(TrackStatus)
				message(M_SETOBJ,knobLinking);
			else
				sqE -> put(E_IMPCHOICE,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
			break;
		case 'L':
			if(TrackStatus){
				if(cBranch) Track.lift_branch(cBranch);
				}
			else
				LandBounded = 1 - LandBounded;
			break;
		case 'Y':
			sqE -> put(E_LAYERMENU,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
			break;
//		case 'V':
//			ShowVoxel = 1 - ShowVoxel;
//			break;
		case 'Z':
			ZoomLog = 1 - ZoomLog;
			if(ZoomLog)
				if(XKey.Pressed(SDLK_LSHIFT))
					TurnSecX = curGMap -> xsize/8;
				else
#ifdef POSTER
					TurnSecX = 2*H_SIZE;
#else
					TurnSecX = H_SIZE;
#endif
			else
				TurnSecX = curGMap -> xsize;
			break;
		case SDLK_BACKQUOTE:
			vMap -> squeeze();
			break;
		case '0':
			analyzeColors();
			break;
		case 'P':
			MLstepLog = 1 - MLstepLog;
			keyP = 0;
			break;
		case 'G':
			if(MLstatus == 2) sqE -> put(E_MOBILOCFRAMECHOICE,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
			break;
		case 'A':
			if(MLstatus == 2) MLacceptframe();
			break;
		case 'Q':
			if(MLstatus == 2) MLrestoreframe();
			break;
		case 'N':
			if(MLstatus == 2) MLinsertframe();
			break;
		case 'D':
			if(MLstatus == 2) MLdeleteframe();
			break;
		case 'V':
			void MakeAbsML(void);
			MakeAbsML();
			break;
		default:
			owner -> keytrap(key);
			return;
		}
}

iGameMap::iGameMap(sqElem* _owner,int _x,int _y,int _xside,int _yside,char* _title)
: sqInputBox(_owner,_x,_y,10,10,&sysfont,_title)
{
	xside = _xside;
	yside = _yside;
	xsize = 2*xside + 1;
	ysize = 2*yside + 1;

	int syh = font -> sy + 2;

	sy = syh + ysize + 1;
	sx = xsize + 2;
	_xI = _x; _yI = _y;
	x = _x - sx/2; y = _y - (sy - syh)/2 - syh;

	*this + (knob3D = new sqTextButton(this,sx - 50 + 8,24,"[2D]",&sysfont));
	*this + (knobAlt = new sqTextButton(this,sx - 50,24 + 24,"[COL]",&sysfont));
	*this + (knobTrack = new sqTextButton(this,sx - 50 - 6*8,24 + 2*24,"[Track OFF]",&sysfont));

	*this + (knobRender = new sqTextButton(this,sx - 50 - 3*8,sy - 30,"(Render)",&sysfont));

	set();

	xc = getX() + x + sx/2;
	yc = getY() + y + syh + (sy - syh)/2 - 1;

	UcutLeft = xc - xside;
	UcutRight = xc + xside;
	VcutUp = yc - yside;
	VcutDown = yc + yside;
	XGR_SetClip(UcutLeft,VcutUp,UcutRight,VcutDown);

	isreEvent = reEvent = 0;

	reset();

	KeyTrapObj = QuantObj = this;
	obj = NULL;		// tail
//	draw();
}

void iGameMap::change(int Dx,int Dy)
{
	if(xside + Dx > XSIDE) return;

	xside += Dx;
	yside += Dy;
	xsize = 2*xside + 1;
	ysize = 2*yside + 1;

	int syh = font -> sy + 2;

	sy = syh + ysize + 1;
	sx = xsize + 2;
	x = _xI - sx/2; y = _yI - (sy - syh)/2 - syh;

	set();

	xc = getX() + x + sx/2 - 1;
	yc = getY() + y + syh + (sy - syh)/2 - 1;

	UcutLeft = xc - xside;
	UcutRight = xc + xside;
	VcutUp = yc - yside;
	VcutDown = yc + yside;
	XGR_SetClip(UcutLeft,VcutUp,UcutRight,VcutDown);

	knob3D -> x += 2*Dx;
	knob3D -> set();
	knobAlt -> x += 2*Dx;
	knobAlt -> set();
	knobTrack -> x += 2*Dx;
	knobTrack -> set();
	knobRender -> x += 2*Dx;
	knobRender -> y += 2*Dy;
	knobRender -> set();

	if(TrackStatus){
		knobSpline -> x += 2*Dx;
		knobSpline -> set();
		knobBuild -> x += 2*Dx;
		knobBuild -> y += 2*Dy;
		knobBuild -> set();
		knobBuildAll -> x += 2*Dx;
		knobBuildAll -> y += 2*Dy;
		knobBuildAll -> set();
		knobLinking -> x += 2*Dx;
		knobLinking -> y += 2*Dy;
		knobLinking -> set();
		}

	TurnSecX = xsize;

	FirstDraw = 1;
	sqScr -> draw();
}

void iGameMap::draw(int self)
{
	static XBuffer status;

	if(FirstDraw){
		sqInputBox::draw(FirstDraw);
		FirstDraw = 0;
		}

	CX += DX; CY += DY;
	cycleTor(CX,CY);

//	if(ShowVoxel)
//		vMap -> draw_voxel(TurnAngle,SlopeAngle,TurnSecX,CX,CY,xc,yc,xside,yside);
//	else
		if(TurnAngle)
			vMap -> turning(TurnSecX,TurnAngle,CX,CY,xc,yc,xside,yside);
		else {
			vMap -> scaling(TurnSecX,CX,CY,xc,yc,xside,yside);
			if(GridLog && TurnSecX == xsize){
				int i = (CY - yside) & clip_mask_y;
				int im = (CY + yside) & clip_mask_y;
				for(;i != im;i = (i + 1) & clip_mask_y)
					if(!(i%part_map_size_y)){
						XGR_LineTo(xc - xside,yc + getDistY(i,CY),xsize,2,COL2);
						status.init();
						status <= i/part_map_size_y;
						sysfont.draw(xc - xside + 3,yc + getDistY(i,CY) + 3,(unsigned char*)(status.GetBuf()),COL2,-1);
						}
				if(CX < xside || CX > (int)map_size_x - xside)
					if(CX < xside)
						XGR_LineTo(xc - CX,yc - yside,ysize,3,COL2);
					else
						XGR_LineTo(xc + map_size_x - CX,yc - yside,ysize,3,COL2);
				}
			}

	if(TrackStatus)
		TurnAngle = 0;
	else
		TurnAngle = rPI(TurnAngle + TurnD);

	ViewX = CX;
	ViewY = CY;
	view_shape();

	if(prmFlag & PRM_INFO){
		status.init();
		int u,d,t,dl = IS_DOUBLE(*(vMap -> lineT[CY] + H_SIZE + CX));
		u = GETUPALT(CX,CY);
		t = GETWIDTH(CX,CY);
		status < "x:" <= CX < " y:" <= CY < " col: " <= *(vMap -> lineTcolor[CY] + CX);
		status < " type: " <= GETTERRAIN(CX,CY);
		if(dl) status < "/" <= GETDOWNTERRAIN(CX,CY);
		if(!dl)
			status < " alt:"<= u;
		else {
			d = GETDOWNALT(CX,CY);
			status < " d:" <= d;
			status < " u:"<= u;
			status < " t:" <= t;
			status < " w:" <= (u - d - t);
			}
		if(sssInuse) status < " ";
		if(LandBounded) status < " ";
		sysfont.draw(xc - xside + 3,yc - yside + 3,(unsigned char*)(status.GetBuf()),COL1,-1);
		status.init();
		status < GetMMode();
		sysfont.draw(xc - xside + 3,yc - yside + 3 + 20,(unsigned char*)(status.GetBuf()),COL1,-1);
		int off = 0;
		if(catch_log){
			status.init();
			status < "3D object editing...";
			sysfont.draw(xc - xside + 3,yc - yside + 3 + 40,(unsigned char*)(status.GetBuf()),COL1,-1);
			off += 20;
			}
		if(mlobj && (!MLstatus || MLstatus == 2)){
			status.init();
			status < "ML<" < mlobj -> name < ">, Phase:" <= mlobj -> getCurPhase() < " Step: " <= mlobj -> steps[mlobj -> cFrame] - 1 < /*" Stage: " <= mlobj -> getStage() < */" Frame: " <= mlobj -> cFrame;
			sysfont.draw(xc - xside + 3,yc - yside + 3 + 40 + off,(unsigned char*)(status.GetBuf()),COL1,-1);
			off += 20;
			}
		if(VLstatus){
			status.init();
			status < getVLname() < "Valocs Handling...";
			sysfont.draw(xc - xside + 3,yc - yside + 3 + 40 + off,(unsigned char*)(status.GetBuf()),COL1,-1);
			}
		}

	if(TrackStatus)
		Track.show();
	else {
		Track.show_all_track();
		XGR_PutSpr(xc - 3,yc - 3,7,7,markBMP,0);
	}

	if(VLstatus) VLshow();

	sqInputBox::draw(0);
}

void iGameMap::quant(void)
{
	int dx = 0,dy = 0;
	int xm,ym;
	xm = XGR_MouseObj.PosX;
	ym = XGR_MouseObj.PosY;
	if(xm < xc - xside + 8) dx = -DELTA;
	else if(xm > xc + xside - 8) dx = DELTA;
	if(ym < yc - yside + 8) dy = -DELTA;
	else if(ym > yc + yside - 8) dy = DELTA;

	if(dx || dy){
		if(XKey.Pressed(SDLK_LCTRL)) dx *= 3, dy *= 3;
		shift(dx,dy);
		sqE -> put(E_MUFLER,E_COMMON,0,0);
		}
	else {
		if(!MLstepLog || keyP){
			keyP = 0;
			if(MLquant()){
				sqE -> put(E_MUFLER,E_COMMON,0,0);
				if(ibmObj.shoting == 1) ibmObj.quantSave();
				}
			}
		}

	if(isreEvent)
		switch(reEvent){
			case E_LBMREEV:
				sqE -> put(E_LBMREEV,E_REGION,xm,ym);
				break;
			}

	sqInputBox::quant();
}

void iGameMap::flush()
{
	XGR_Flush(xc - xside,yc - yside,xsize,ysize);
}

void iGameMap::message(int code,sqElem* object)
{
	switch(code){
		case M_SETOBJ:
			if(object == knob3D){
				DepthShow = 1 - DepthShow;
				knob3D -> text[1] = DepthShow ? '3' : '2';
				}
			if(object == knobAlt){
				ColorShow = 1 - ColorShow;
				if(ColorShow){
					palbuf = palbufC;
					memcpy(knobAlt -> text + 1,"COL",3);
					}
				else {
					memcpy(knobAlt -> text + 1,"ALT",3);
					palbuf = palbufA;
					}
				XGR_SetPal(palbuf,0,250);
				sqE -> put(E_MUFLER,E_COMMON,0,0);
				}
			if(object == knobRender)
				vMap -> screenRender();
			if(object == knobTrack){
				TrackStatus = 1 - TrackStatus;
				if(TrackStatus){
					memcpy(knobTrack -> text + 7,"ON ",3);
					*this + (knobSpline = new sqTextButton(this,sx - 50 - 4*8,24 + 3*24,"[Splines]",&sysfont));
					*this + (knobBuild = new sqTextButton(this,sx - 50 - 2*8,sy - 30 - 3*24,"(Build)",&sysfont));
					*this + (knobBuildAll = new sqTextButton(this,sx - 50 - 6*8,sy - 30 - 2*24,"(Build All)",&sysfont));
					*this + (knobLinking = new sqTextButton(this,sx - 50 - 4*8,sy - 30 - 1*24,"(Linking)",&sysfont));
					}
				else {
					memcpy(knobTrack -> text + 7,"OFF",3);
					*this - knobSpline;
					*this - knobBuild;
					*this - knobBuildAll;
					*this - knobLinking;
					obj = NULL;
					}
				}
			if(object == knobSpline) Track.build_all_spline();
			if(object == knobBuild) Track.render(1);
			if(object == knobBuildAll) Track.render(0);
			if(object == knobLinking) Track.linking();
			break;
		}
	sqInputBox::message(code,object);
}

void loadState(void)
{
	XStream ff(0);
	if(ff.open("state.dat",XS_IN)){
		ff > preCX > preCY;
		sqStateLoad(ff);
		ff > prevWorld;
		ff.close();
		}
	if(WorldPrm != -1) prevWorld = WorldPrm;
}

void iGameMap::reset(void)
{
	CX = preCX;
	CY = preCY;

	DX = DY = 0;
	TurnAngle = 0;
	TurnD = 0;
	TurnSecX = xsize;
	prmFlag = PRM_INFO;
	FirstDraw = 1;
	ViewY = CY;

	if(MAP_POWER_Y <= 11)
		vMap -> accept(0,V_SIZE - 1);
	else
		vMap -> accept(CY - yside - 1,CY + yside + 1);
	MLload();
}

void iGameMap::gotoxy(int x,int y)
{
	if(!MLstatus){
		CX = x; CY = y;
		cycleTor(CX,CY);
		}
}

void PrintError(char* error,char* msg)
{
	XCon < "--------------------------------\n";
	XCon < error < "\n";
	XCon < msg < "\n";
	XCon < "--------------------------------\n";
}

void analyzeColors(void)
{
	int x,y;
	int y0 = vMap -> upLine;
	int y1 = vMap -> downLine;
	uchar* pc;

	int* table = new int[256];
	memset(table,0,256*4);

	for(y = y0;y != y1;y = YCYCL(y + 1)){
		pc = vMap -> lineTcolor[y];
		for(x = 0;x < H_SIZE;x++,pc++) table[*pc]++;
		}

	XStream ff("colors",XS_OUT);
	int ind = 0;
	for(x = 0;x < 256;x++){
		if(x == BEGCOLOR[ind])
			ff < "\r\n-------- " <= ind++ < "-------------\r\n";
		ff <= x < ": " <= table[x] < "\r\n";
		}

	delete[] table;
}

#ifdef SCREENSHOT
extern KDWIN::LOGPALETTE  *g_pe;
void shotMake(void)
{
	static int curShotNumber;
	static char palette[768];
	static XBuffer buf;
	static XStream ff;

	char* p = palette;
	for(int i = 0;i < 256;i++){
		*p++ = g_pe->palPalEntry[i].peRed >> 2;
		*p++ = g_pe->palPalEntry[i].peGreen >> 2;
		*p++ = g_pe->palPalEntry[i].peBlue >> 2;
		}

	buf.init();
	buf < "\\STORAGE\\SCR";
	if(curShotNumber < 1000) buf < "0";
	if(curShotNumber < 100) buf < "0";
	if(curShotNumber < 10) buf < "0";
	buf <= curShotNumber++ < ".BMP";
	ff.open((char*)buf,XS_OUT);
	ff < (short)XGR_MAXX < (short)XGR_MAXY;
	ff.write(VS(_video)->_video,XGR_MAXX*XGR_MAXY);
	ff.close();
	*((char*)buf + strlen((char*)buf) - 3) = '\0';
	strcat((char*)buf,"PAL");
	ff.open((char*)buf,XS_OUT);
	ff.write(palette,768);
	ff.close();
}
#endif

struct PrmFile {
	char* buf;
	int len;
	int index;

	void init(char const* name);
	char* getAtom(void);
	void finit(void){ delete buf; }
	};

void PrmFile::init(char const* name)
{
	XStream ff(name,XS_IN);
	buf = new char[len = ff.size()];
	ff.read(buf,len);
	ff.close();

	int i = 0,mode = 1;
	char c;
	while(i < len){
		c = buf[i];
		if(c == '\"'){
			buf[i++] = '\0';
			while(i < len){
				if(buf[i] == '\"') break;
				i++;
				}
			if(i == len) ErrH.Abort("Wrong PRM format, second <\"> is absent");
			else buf[i] = '\0';
			}
		else
			if(c == ' ' || c == '\t' || c == '\r' || c == ',' || c == '\n') buf[i] = '\0';
		i++;
		}
	i = 0;
	while(i < len){
		c = buf[i];
		if(mode){ if(c == '/' && buf[i + 1] == '*'){ buf[i] = '\0'; mode = 0; } }
		else {
			if(c == '*' && buf[i + 1] == '/'){ buf[i] = buf[i + 1] = '\0'; i++; mode = 1; }
			else if(c != '\n') buf[i] = '\0';
			}
		i++;
		}
	index = 0;
}

char* PrmFile::getAtom(void)
{
	char* p = buf + index;
	while(index < len && !(*p)) p++, index++;
	if(index == len) return NULL;
	char* ret = p;
	while(index < len && *p) p++, index++;
	return ret;
}

void Object::StartMoleProcess(void){}
int Object::UsingCopterig(int decr_8){ return 0; }
int Object::UsingCrotrig(int decr_8){ return 0; }
int Object::UsingCutterig(int decr_8){ return 0; }
