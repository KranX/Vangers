#include "../global.h"
#include "../runtime.h"

#include "../3d/3d_math.h"
#include "../3d/3dgraph.h"
#include "../3d/3dobject.h"
#include "../3d/parser.h"

#include "../common.h"
#include "../sqexp.h"
#include "../backg.h"

#if !(defined(__unix__) || defined(__APPLE__))
static WIN32_FIND_DATA FFdata;
static HANDLE FFh;
char* win32_findnext(void)
{
	if(FindNextFile(FFh,&FFdata) == TRUE){
		if(FFdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) return win32_findnext();
		return FFdata.cFileName;
		}
	else {
		FindClose(FFh);
		return NULL;
		}
}

char* win32_findfirst(char* mask)
{
	FFh = FindFirstFile(mask,&FFdata);
	if(FFh == INVALID_HANDLE_VALUE) return NULL;
	if(FFdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) return win32_findnext();
	return FFdata.cFileName;
}
#endif

#include "../particle/particle.h"
#include "../particle/partmap.h"
#include "../particle/df.h"
#include "../particle/light.h"

#include "../actint/item_api.h"
#include "uvsapi.h"
#include "../network.h"

#include "../terra/vmap.h"
#include "../terra/world.h"
#include "../terra/render.h"

#include "../uvs/univang.h"

#include "../dast/poly3d.h"
#include "../sound/hsound.h"

#include "../rle.h"
#include "track.h"
#include "hobj.h"
#include "effect.h"
#include "moveland.h"
#include "items.h"
#include "sensor.h"
#include "mechos.h"

#include <iostream>


/* ----------------------------- EXTERN SECTION ---------------------------- */
extern int ViewX,ViewY;
extern iGameMap* curGMap;
extern int MLstatus,MLprocess;
extern std::string path_to_world; 
/* --------------------------- PROTOTYPE SECTION --------------------------- */
char* GetTargetName(const char* name);
/* --------------------------- DEFINITION SECTION -------------------------- */
MobileLocation** MLTable;
int MLTableSize;
const char MLsign[] = "ML3";
const char* VLCsign[4] = { "VLT1","VLM1","VLS1","VLD1" };

#ifdef _ROAD_

MobileLocation* MLobjBug[8];
MobileLocation* MLobjExp[4];
MobileLocation* MLTnt[2];

const char* MLTntName[WORLD_MAX][2] = {
	{"fgrib0","fgrib1"},
	{"ggrib0","ggrib1"},
	{"None","ngrib1"},
	{"None","None"},
	{"None","None"},
	{"None","None"},
	{"None","None"},
	{"None","None"},
	{"None","None"},
	{"None","None"}
};

#endif

#ifdef _SURMAP_
BaseValoc* TntValoc::tail;
BaseValoc* MLCloneValoc::tail;
BaseValoc* SensorValoc::tail;
BaseValoc* DangerValoc::tail;
TntValoc* TntTable;
MLCloneValoc* MLCTable;
SensorValoc* SnsTable;
DangerValoc* DngTable;
int MLvisi;
int* isMLFConverted;
#endif

const char* VLfilenames[] = { "tnttable","mlctable","snstable","dngtable" };

int TntTableSize,MLCTableSize,SnsTableSize,DngTableSize;

char SmoothTerrainMask[TERRAIN_MAX];

#ifndef _SURMAP_

uchar DestroyTerrainTable[TERRAIN_MAX];

dastPoly3D DollyPoint(Vector(0,0,0),40,0);

uchar DestroyMechosTable[TERRAIN_MAX];
uchar DestroyMoleTable[TERRAIN_MAX];

int aciGroundPressingEnabled = 0;

void LocalMapProcess::Init(Parser& in)
{
	int i,j,k;

	UnitList::Init(in);
	DestroySmoothInit();

	in.search_name("NumMapPoint");
	NumMapPoint = in.get_int();
	MapPointStorage.Init(NumMapPoint);
	MapPointData = new MapPointType[NumMapPoint];
	for(i = 0;i < NumMapPoint;i++) MapPointData[i].Init(&MapPointStorage);

	in.search_name("NumTrace");
	NumTrace = in.get_int();
	TraceStorage.Init(NumTrace);
	TraceData = new MapTraceType[NumTrace];
	for(i = 0;i < NumTrace;i++) TraceData[i].Init(&TraceStorage);

//---------------------------------------- Map Process ---------------------------------------------

	Total = 0;

	MaxUnit = new int[MAX_MAP_OBJECT];
	UnitStorage = new StorageType[MAX_MAP_OBJECT];

	in.search_name("NumMapProcess");
	for(i = 0;i < MAX_MAP_OBJECT;i++){
		MaxUnit[i] = in.get_int();
//		if(CurrentWorld != WORLD_KHOX && i == MAP_FIRETERROR_TYPE) MaxUnit[i] = 1;
		Total += MaxUnit[i];
	};

	UnitData = new GeneralObject*[Total];
	k = 0;

	memset(UnitData,0,Total*sizeof(GeneralObject*));

	for(i = 0;i < MAX_MAP_OBJECT;i++){
		UnitStorage[i].Init(MaxUnit[i]);
		for(j = 0;j < MaxUnit[i];j++){
			switch(i){
				case MAP_LAVA_SPOT:
					UnitData[k] = new MapLavaSpot;
					break;
				case MAP_LIGHT_POINT:
					UnitData[k] = new LightPoint;
					((LightPoint*)UnitData[k]) -> MemInit();
					break;
				case MAP_LANDSLIDE_TYPE:
					UnitData[k] = new LandSlideType;
					break;
				case MAP_LANDHOLE_TYPE:			   
					UnitData[k] = new MapLandHole;
					break;
				case MAP_ACID_TYPE:
					UnitData[k] = new MapAcidSpot;
					break;
			};
			UnitData[k]->Init(&UnitStorage[i]);
			UnitStorage[i].Deactive(UnitData[k]);
			k++;
		};
	};
};

void LocalMapProcess::Free(void)
{
	int i;

	for(i = 0;i < Total;i++){
		UnitData[i]->Free();
		delete UnitData[i];
	};

	for(i = 0;i < MAX_MAP_OBJECT;i++) UnitStorage[i].Free();

	delete[] UnitData;
	delete[] MaxUnit;
	delete[] UnitStorage;
 
	MapPointStorage.Free();
	TraceStorage.Free();

	delete[] MapPointData;
	delete[] TraceData;
	DestroySmoothFree();
};

void LocalMapProcess::Open(Parser& in)
{
	int i,j;
	int volume;

	UnitList::Open(in);

	in.search_name("NumDustType");
	NumDustType = in.get_int();
	DustStorage = new StorageType[NumDustType];
	Dust = new ParticleProcess[NumDustType];
	DustData = new MapPointType*[NumDustType];

	for(i = 0;i < NumDustType;i++){
		in.search_name("DustVolume");
		volume = in.get_int();
		in.search_name("DustFirstColor");
		Dust[i].init(1500,in.get_int(),1);
		DustData[i] = new MapPointType[volume];
		DustStorage[i].Init(volume);
		for(j = 0;j < volume;j++) DustData[i][j].Init(&DustStorage[i]);
	};

/*	for(i = 0;i < TERRAIN_MAX;i++){
		in.search_name("TerrainDustID");
		DustID[i] = in.get_int();
	};*/

	for(i = 0;i < TERRAIN_MAX;i++){
		in.search_name("DestroyTerrain");
		DestroyTerrainTable[i] = (uchar)(in.get_int());
		DestroyMechosTable[i] = (uchar)(in.get_int());
		DestroyMoleTable[i] = (uchar)(in.get_int());
	};

	MapPointStorage.ObjectPointer = 0;
	TraceStorage.ObjectPointer = 0;

	for(i = 0;i < Total;i++) UnitData[i]->Open();
};

void LocalMapProcess::Close(void)
{
	int i;

	GeneralObject* p;
	p = Tail;
	while(p){
		if(p->Storage) p->Storage->Deactive(p);
		DisconnectTypeList(p);
		p = p->NextTypeList;
	};
	for(i = 0;i < Total;i++) UnitData[i]->Close();
	for(i = 0;i < MAX_MAP_OBJECT;i++) UnitStorage[i].Check();

	for(i = 0;i < NumDustType;i++){
		Dust[i].finit();
		DustStorage[i].Free();
		delete[] DustData[i];
	};

	delete[] DustData;
	delete[] Dust;
	delete[] DustStorage;
};

void LocalMapProcess::Quant(void)
{
	int i;
	MapPointType* p;
	MapTraceType* t;
	GeneralObject* n;
	GeneralObject* nn;

	while((t = (MapTraceType*)(TraceStorage.GetAll())) != NULL){
		if(t->UpFlag) DrawMechosWheelUp(t->x0,t->y0,t->x1,t->y1,8,3,-1,t->track_nx,t->track_ny,3);
		else DrawMechosWheelDown(t->x0,t->y0,t->x1,t->y1,8,3,-1,t->track_nx,t->track_ny,3);		
	};
	
	if(ActD.Active && aciGroundPressingEnabled)
		ActD.Active->ground_pressing();

	DollyPoint.make_catch_dolly();

	while((p = (MapPointType*)(MapPointStorage.GetAll())) != NULL){
		switch(p->Type){
			case MAP_POINT_CRATER01:
//				MLobjExp[0] -> put(p->R_curr.x,p->R_curr.y);
//				for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 1;

				if(CurrentWorld != WORLD_NECROSS) SmoothTerrainMask[0] = 1;
				else SmoothTerrainMask[0] = 0;
				SmoothTerrainMask[1] = 1;
				SmoothTerrainMask[2] = 1;
				SmoothTerrainMask[3] = 1;
				SmoothTerrainMask[6] = 1;

				SmoothTerrainMask[4] = 0;
				SmoothTerrainMask[5] = 0;				
				SmoothTerrainMask[7] = 0;
				DestroySpot(p->R_curr.x,p->R_curr.y,10,83,512,1);
				DestroySpot(p->R_curr.x,p->R_curr.y,8,83,-(1 << 11),1);
				break;
			case MAP_POINT_CRATER02:
//				MLobjExp[1] -> put(p->R_curr.x,p->R_curr.y);
//				for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 1;

				if(CurrentWorld != WORLD_NECROSS) SmoothTerrainMask[0] = 1;
				else SmoothTerrainMask[0] = 0;
				SmoothTerrainMask[1] = 1;
				SmoothTerrainMask[2] = 1;
				SmoothTerrainMask[3] = 1;
				SmoothTerrainMask[6] = 1;

				SmoothTerrainMask[4] = 0;
				SmoothTerrainMask[5] = 0;
				SmoothTerrainMask[7] = 0;
				DestroySpot(p->R_curr.x,p->R_curr.y,20,83,512,1);
				DestroySpot(p->R_curr.x,p->R_curr.y,15,83,-(1 << 11),1);
				break;
			case MAP_POINT_CRATER03:
				MLobjBug[rPI(-(p->Angle))*8/PIx2] -> put(p->R_curr.x,p->R_curr.y);
				break;
			case MAP_POINT_CRATER04:
				MLobjExp[2] -> put(p->R_curr.x,p->R_curr.y);
				break;
			case MAP_POINT_CRATER05:
//				MLobjExp[3] -> put(p->R_curr.x,p->R_curr.y);
//				for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 1;
				if(CurrentWorld != WORLD_NECROSS) SmoothTerrainMask[0] = 1;
				else SmoothTerrainMask[0] = 0;
				SmoothTerrainMask[1] = 1;
				SmoothTerrainMask[2] = 1;
				SmoothTerrainMask[3] = 1;
				SmoothTerrainMask[6] = 1;

				SmoothTerrainMask[4] = 0;
				SmoothTerrainMask[5] = 0;
				SmoothTerrainMask[7] = 0;
//				void DestroySpot(int x0,int y0,int Radius,uchar terrain,int delta,uchar rfactor);
				DestroySpot(p->R_curr.x,p->R_curr.y,5,83,-64,4);
				break;
			case MAP_POINT_CRATER06:
				if(CurrentWorld == WORLD_GLORX) DestroyBarellSmooth(p->R_curr.x,p->R_curr.y,p->Angle,p->Angle / 2,32 - RND(64),83,3,p->R_curr.z,3);
				else DestroyBarellSmooth(p->R_curr.x,p->R_curr.y,p->Angle,p->Angle / 2,32 - RND(64),7,3,p->R_curr.z,3);
				break;
			case MAP_POINT_CRATER07:
//				for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 1;
				if(CurrentWorld != WORLD_NECROSS) SmoothTerrainMask[0] = 1;
				else SmoothTerrainMask[0] = 0;
				SmoothTerrainMask[1] = 1;
				SmoothTerrainMask[2] = 1;
				SmoothTerrainMask[3] = 1;
				SmoothTerrainMask[6] = 1;

				SmoothTerrainMask[4] = 0;
				SmoothTerrainMask[5] = 0;
				SmoothTerrainMask[7] = 0;
				DestroySpot(p->R_curr.x,p->R_curr.y,15,3,-128,4);
				break;
			case MAP_POINT_CRATER08:
//				for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 1;
				if(CurrentWorld != WORLD_NECROSS) SmoothTerrainMask[0] = 1;
				else SmoothTerrainMask[0] = 0;
				SmoothTerrainMask[1] = 1;
				SmoothTerrainMask[2] = 1;
				SmoothTerrainMask[3] = 1;
				SmoothTerrainMask[6] = 1;
				SmoothTerrainMask[7] = 1;

				SmoothTerrainMask[4] = 0;
				SmoothTerrainMask[5] = 0;
				DestroySmoothCheck(p->R_curr.x,p->R_curr.y,20);
				break;
			case MAP_POINT_CRATER09:
//				DestroySmooth(p->R_curr.x,p->R_curr.y,35,20,0,-20,83,4,2);
				DestroySmooth(p->R_curr.x,p->R_curr.y,35,20,0,-63,83,4,4);
				break;
			case MAP_POINT_CRATER10:
				DestroySmooth(p->R_curr.x,p->R_curr.y,35,20,0,-512,83,4,2);
				break;
			case MAP_POINT_CRATER11:
//				for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 1;
				if(CurrentWorld != WORLD_NECROSS) SmoothTerrainMask[0] = 1;
				else  SmoothTerrainMask[0] = 0;
				SmoothTerrainMask[1] = 1;
				SmoothTerrainMask[2] = 1;
				SmoothTerrainMask[3] = 1;
				SmoothTerrainMask[6] = 1;

				SmoothTerrainMask[4] = 0;
				SmoothTerrainMask[5] = 0;
				SmoothTerrainMask[7] = 0;
				switch(CurrentWorld){
					case WORLD_KHOX:
						DestroySpot(p->R_curr.x,p->R_curr.y,20,7,-64,4);
					case WORLD_THREALL:
					case WORLD_BOOZEENA:
					case WORLD_FOSTRAL:
					case WORLD_GLORX:
					case WORLD_NECROSS:
						DestroySpot(p->R_curr.x,p->R_curr.y,5,7,-64,4);
						break;
				};
				break;
			case MAP_POINT_CRATER12:
//				for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 0;
				memset(SmoothTerrainMask,0,TERRAIN_MAX);

				SmoothTerrainMask[6] = 1;
				DestroySpot(p->R_curr.x,p->R_curr.y,35,5,0,0);
				break;
			case MAP_POINT_SHOW_HEAD:
//				for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 0;
				memset(SmoothTerrainMask,0,TERRAIN_MAX);

				SmoothTerrainMask[1] = 1;
				DestroySpot(p->R_curr.x,p->R_curr.y,20,6,64,0);
				break;
			case MAP_POINT_HIDE_HEAD:
//				for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 0;
				memset(SmoothTerrainMask,0,TERRAIN_MAX);

				SmoothTerrainMask[6] = 1;
				DestroySpot(p->R_curr.x,p->R_curr.y,20,1,64,0);
				break;
		};
	};

	n = Tail;
	while(n){
		nn = n->NextTypeList;
		n->Quant();
//		n = n->NextTypeList;
		n = nn;
	};

	n = Tail;
	while(n){
		if(n->Status & SOBJ_DISCONNECT){
			if(n->Storage) n->Storage->Deactive(n);
			DisconnectTypeList(n);		
		};
		n = n->NextTypeList;
	};

	for(i = 0;i < NumDustType;i++){
		Dust[i].quant1();
		while((p = (MapPointType*)(DustStorage[i].GetAll())) != NULL) Dust[i].set_hot_spot(p->R_curr.x,p->R_curr.y,100,p->R_curr.z);
		Dust[i].quant2();
	};
};


void LocalMapProcess::CreateDust(const Vector& v,int type)
{
	DustData[type][DustStorage[type].ObjectPointer].Create(v);
	DustStorage[type].Deactive(&DustData[type][DustStorage[type].ObjectPointer]);
};

void LocalMapProcess::CreateTrace(int _x0,int _y0,int _x1,int _y1,int track_nx,int track_ny,char f)
{
	TraceData[TraceStorage.ObjectPointer].Create(_x0,_y0,_x1,_y1,track_nx,track_ny,f);
	TraceStorage.Deactive(&TraceData[TraceStorage.ObjectPointer]);
};


/*void LocalMapProcess::CreateCrater(Vector& v,int type)
{
	MapPointData[MapPointStorage.ObjectPointer].Create(v,type);
	MapPointStorage.Deactive(&MapPointData[MapPointStorage.ObjectPointer]);
};*/

void LocalMapProcess::CreateCrater(Vector& v,int type,int angle)
{
	MapPointData[MapPointStorage.ObjectPointer].Create(v,type,angle);
	MapPointStorage.Deactive(&MapPointData[MapPointStorage.ObjectPointer]);
};

void LocalMapProcess::CreateLavaSpot(Vector& v,int fRadius,int fDelta,int radius1,int delta1,int radius2,int delta2,int phase1,int phase2,uchar terrain,uchar md,uchar rf,uchar render,uchar clip_t)
{
	MapLavaSpot* p;
	p = (MapLavaSpot*)(UnitStorage[MAP_LAVA_SPOT].Active());
	if(p){
		p->CreateSpot(v,fRadius,fDelta,radius1,delta1,radius2,delta2,phase1,phase2,terrain,md,rf,render,clip_t);
		ConnectTypeList(p);
	};
};

LightPoint* LocalMapProcess::CreateLight(int x,int y,int z, int r,int e,int mode)
{
	LightPoint* p;
	p = (LightPoint*)(UnitStorage[MAP_LIGHT_POINT].Active());
	if(p){
		p->CreateLight(r,e,mode);
		p->set_position(x,y,z);
		ConnectTypeList(p);
	};
	return p;
};

void LocalMapProcess::CreateLandSlide(int* x,int* y,int time)
{
//zmod 1.17
//	if(NetworkON) return;
	LandSlideType* p;
	p = (LandSlideType*)(UnitStorage[MAP_LANDSLIDE_TYPE].Active());
	if(p){
		p->CreateLandSlide(x,y,time);
		ConnectTypeList(p);		
	};
};

void LocalMapProcess::CreateMapHole(Vector v,int rMax,int l1,int l2,int l3)
{
	MapLandHole* p;
	p = (MapLandHole*)(UnitStorage[MAP_LANDHOLE_TYPE].Active());
	if(p){
		p->CreateLandHole(v,rMax,l1,l2,l3);
		ConnectTypeList(p);
	};
};

void LocalMapProcess::CreateAcidSpot(Vector v,int fR,int lR,int fD,int lD,int lT)
{
	MapAcidSpot* p;
	p = (MapAcidSpot*)(UnitStorage[MAP_ACID_TYPE].Active());
	if(p){
		p->CreateAcid(v,fR,lR,fD,lD,lT);
		ConnectTypeList(p);
	};	
};

void MapLavaSpot::CreateSpot(Vector& v,int fRadius,int fDelta,int radius1,int delta1,int radius2,int delta2,int phase1,int phase2,uchar terrain,uchar md,uchar rf,uchar render,uchar clip_t)
{
	R_curr = v;
	Terrain = terrain;

	Phase = 0;

	MaxPhase1 = phase1 * GAME_TIME_COEFF;
	MaxPhase2 = phase2 * GAME_TIME_COEFF;

	LastRadius = Radius = fRadius << 8;
	LastDelta = Delta = fDelta << 8;

	dRadius1 = ((radius1 << 8) - LastRadius) / MaxPhase1;
	dDelta1 = ((delta1 << 8) - LastDelta) / MaxPhase1;

	dRadius2 = ((radius2 - radius1) << 8) / MaxPhase2;
	dDelta2 = ((delta2 - delta1) << 8) / MaxPhase2;

	Delay = 0;
	mDelay = md * GAME_TIME_COEFF;

	Status = 0;
	ID = ID_EXPLOSION;
	rFactor = rf;
	RenderFlag = render;
	ClipTerrain = clip_t;
};

void MapLavaSpot::Quant(void) // Map Lava spot is underground terminator!
{
//	int i;

	if(ClipTerrain == 83)
		memset(SmoothTerrainMask,1,TERRAIN_MAX);
//		for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 1;
	else{
//		for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 0;
		memset(SmoothTerrainMask,0,TERRAIN_MAX);
		SmoothTerrainMask[ClipTerrain] = 1;
	};	

	if(Delay >= mDelay){		
		if(Phase) xRestoreDestroySpot(R_curr.x,R_curr.y,LastRadius >> 8,Terrain,LastDelta >> 8,0);
		else xRestoreDestroySpot(R_curr.x,R_curr.y,LastRadius >> 8,Terrain,LastDelta >> 8,rFactor);

		if(Phase < MaxPhase1){
			xDestroySpot(R_curr.x,R_curr.y,Radius >> 8,Terrain,Delta >> 8,0);
			if(RenderFlag){
				if(Radius > LastRadius) RadialRender(R_curr.x,R_curr.y,Radius >> 8);
				else RadialRender(R_curr.x,R_curr.y,LastRadius >> 8);
			};
			LastRadius = Radius;
			LastDelta = Delta;
			Radius += dRadius1;
			Delta += dDelta1;
		}else{
			if(Phase < MaxPhase2){
				xDestroySpot(R_curr.x,R_curr.y,Radius >> 8,Terrain,Delta >> 8,0);
				if(RenderFlag){
					if(Radius > LastRadius) RadialRender(R_curr.x,R_curr.y,Radius >> 8);
					else RadialRender(R_curr.x,R_curr.y,LastRadius >> 8);
				};
				LastRadius = Radius;
				LastDelta = Delta;
				Radius += dRadius2;
				Delta += dDelta2;
			}else{
				if(RenderFlag) RadialRender(R_curr.x,R_curr.y,LastRadius >> 8);
				Status |= SOBJ_DISCONNECT;
			};
		};
		Phase++;
		Delay = 0;
	};
	Delay++;
};

#endif

static char SignBuf[3];

void MLverify(XStream& ff)
{
	ff.read(SignBuf,3);
	if(memcmp(SignBuf,MLsign,3)) ErrH.Abort("ML-VOT invalid format");
};

static int ML_FRAME_SIZE = 0;
static uchar *ML_FRAME_DELTA = NULL;
static uchar *ML_FRAME_TERRAIN = NULL;

extern int NumSkipLocation[WORLD_MAX];
extern int RealNumLocation[WORLD_MAX];
extern char** SkipLocationName[WORLD_MAX];
extern int* NumLocationData[WORLD_MAX];

void MLload(void)
{
	int i,j,t;
	XBuffer buf;
	
#ifdef _ROAD_
	int k;

	for(i = 0;i < NumSkipLocation[CurrentWorld];i++)
		NumLocationData[CurrentWorld][i] = i;

	for(i = 0;i < NumSkipLocation[CurrentWorld];i++){
		j = RND(NumSkipLocation[CurrentWorld]);
		k = RND(NumSkipLocation[CurrentWorld]);
		if(j != k){
			t = NumLocationData[CurrentWorld][j];
			NumLocationData[CurrentWorld][j] = NumLocationData[CurrentWorld][k];
			NumLocationData[CurrentWorld][k] = t;
		};
	};

	for(i = 0;i < 8;i++){
		buf.init();
		buf < "resource/mlvot/bugtrl" <= i < ".vot";
		(MLobjBug[i] = new MobileLocation) -> load(buf.GetBuf(),1);
		}
	for(i = 0;i < 4;i++){
		buf.init();
		buf < "resource/mlvot/exptrl" <= i < ".vot";
		(MLobjExp[i] = new MobileLocation) -> load(buf.GetBuf(),1);
		}	
	for(i = 0;i < 2;i++){
		buf.init();
		if(strcmp(MLTntName[CurrentWorld][i],"None")){
			buf < "resource/mlvot/" < MLTntName[CurrentWorld][i] < ".vot";
			(MLTnt[i] = new MobileLocation) -> load(buf.GetBuf(),1);
		}else MLTnt[i] = 0;
	};
#endif
MLTableSize = 0;
#if !(defined(__unix__) || defined(__APPLE__))
	buf.init();
	buf < "data.vot/*.vot";
	char* fn = win32_findfirst(GetTargetName(buf.GetBuf()));
	while(fn){
		MLTableSize++;
		fn = win32_findnext();
		}
#else
	std::string tmp = path_to_world+"data.vot/";
	struct dirent **namelist;
	int n;
	n = scandir(tmp.c_str(), &namelist, 0, alphasort); 
	if (n < 0) 
		perror("scandir"); 
	else { 
		while(n--) { 
			std::string name = namelist[n]->d_name;
			if (name.find(".vot")!=std::string::npos)
				MLTableSize++;
			free(namelist[n]); 
		} 
	free(namelist); 
	} 
	

#endif
	
#ifdef _ROAD_
#if !(defined(__unix__) || defined(__APPLE__))
	MLTableSize -= NumSkipLocation[CurrentWorld] - RealNumLocation[CurrentWorld];
#endif
#endif
	
	MLTable = new MobileLocation*[MLTableSize];
	i = 0;
#if !(defined(__unix__) || defined(__APPLE__))
	fn = win32_findfirst(GetTargetName(buf.GetBuf()));
	while(fn){
		t = 1;
#ifdef _ROAD_
		for(j = 0;j < NumSkipLocation[CurrentWorld];j++){
			if(!strcmp(fn,SkipLocationName[CurrentWorld][NumLocationData[CurrentWorld][j]])){
				if(j >= RealNumLocation[CurrentWorld])
					t = 0;
				break;
				}
			}
#endif
		if(t){
			(MLTable[i] = new MobileLocation) -> load(fn);
			 i++;
			}
		fn = win32_findnext();
	};
#else
	struct dirent **namelist2;
	n = scandir(tmp.c_str(), &namelist2, 0, alphasort); 
	if (n < 0) 
		perror("scandir"); 
	else { 
		while(n--) { 
			std::string name = namelist2[n]->d_name;
			if (name.find(".vot")!=std::string::npos)
				{
				(MLTable[i] = new MobileLocation) -> load(namelist2[n]->d_name);
				i++;
				}
			else
				free(namelist2[n]); 
		} 
	//free(namelist2); 
	} 
#endif
	VLload();
	ML_FRAME_DELTA = new uchar[ML_FRAME_SIZE];
	ML_FRAME_TERRAIN = new uchar[ML_FRAME_SIZE];
};

#ifdef _ROAD_
void MLReset(void)
{
	for(int i = 0;i < MLTableSize;i++) MLTable[i]->reset();
};
#endif

void MLfree(void)
{
	int i;
	if (ML_FRAME_DELTA) {
		delete[] ML_FRAME_DELTA; 
		ML_FRAME_DELTA = NULL;
	}
	if (ML_FRAME_TERRAIN) {
		delete[] ML_FRAME_TERRAIN;
		ML_FRAME_TERRAIN = NULL;
	}

	for(i = 0;i < MLTableSize;i++) {
		delete MLTable[i];
	}
	delete[] MLTable;
	MLTableSize = 0;

#ifdef _ROAD_	
	for(i = 0;i < 8;i++)
		delete MLobjBug[i];
	for(i = 0;i < 4;i++)
		delete MLobjExp[i];
	for(i = 0;i < 2;i++)
		if(MLTnt[i]){
			delete MLTnt[i];
			MLTnt[i] = NULL;
			}
#endif
#ifdef _SURMAP_
	TntValoc::tail = NULL;
	MLCloneValoc::tail = NULL;
	SensorValoc::tail = NULL;
	DangerValoc::tail = NULL;

	delete TntTable;
	delete MLCTable;
	delete SnsTable;
	delete DngTable;

	TntTableSize = 0;
	MLCTableSize = 0;
	SnsTableSize = 0;
	DngTableSize = 0;
#endif

#ifdef _ROAD_
	ValocTableFree();
#endif
};


void MLreload(void)
{
	int i;
	delete[] ML_FRAME_DELTA;
	ML_FRAME_DELTA = NULL;
	delete[] ML_FRAME_TERRAIN;
	ML_FRAME_TERRAIN = NULL;

	for(i = 0;i < MLTableSize;i++) {
		delete MLTable[i];
	}
	delete[] MLTable;

#ifdef _ROAD_
	for(i = 0;i < 8;i++)
		delete MLobjBug[i];
	for(i = 0;i < 4;i++)
		delete MLobjExp[i];
	for(i = 0;i < 2;i++)
		if(MLTnt[i]){
			delete MLTnt[i];
			MLTnt[i] = NULL;
			}
#endif

	MLTableSize = 0;	
	
#ifdef _SURMAP_
	TntValoc::tail = NULL;
	MLCloneValoc::tail = NULL;
	SensorValoc::tail = NULL;
	DangerValoc::tail = NULL;

	delete TntTable;
	delete MLCTable;
	delete SnsTable;
	delete DngTable;

	TntTableSize = 0;
	MLCTableSize = 0;
	SnsTableSize = 0;
	DngTableSize = 0;
#endif

#ifdef _ROAD_
	ValocTableFree();
#endif
	MLload();
//	VLload();
};

void MLsave(void)
{
#ifdef _SURMAP_
	for(int i = 0;i < MLTableSize;i++) MLTable[i] -> save();
#endif
}

void MobileLocation::load(char* fname,int direct)
{

	XStream ff;
	if(!direct){
		XBuffer buf;
		buf < "data.vot/" < fname;
		//HACK
		ff.open(GetTargetName(buf.GetBuf()),XS_IN);
#ifdef _SURMAP_
		fileName = strdup(buf.GetBuf());
#endif
		}
	else
		ff.open(fname,XS_IN);

	
	MLverify(ff);
	ff.read(name,MLNAMELEN + 1);
	int i;
	ff > maxFrame > DryTerrain > Impulse;
	ff.seek(1,XS_CUR);
	uchar t; ff > t; mode = t;
	ff.seek(2,XS_CUR);
	for(i = 1;i < MAX_KEYPHASE;i++) ff > KeyPhase[i];
	ff.seek(4,XS_CUR);
	table = new MLFrame[maxFrame];
	memset(steps = new int[maxFrame],0,maxFrame*sizeof(int));
	isAlt = 0;
	for(i = 0;i < maxFrame;i++){
		table[i].load(ff,mode);
		if(table[i].sx > altSx) altSx = table[i].sx;
		if(table[i].sy > altSy) altSy = table[i].sy;
		if(table[i].period > 1) isAlt = 1;
		}
#ifdef _ROAD_
	if(isAlt)
#endif
		alt = new MLAtype[altSx*altSy];
	calcBounds();
#ifdef _ROAD_
	cStage = -1;
	maxStage = 0;
	goPh = -1;
	NetSetPhase = 0;
	NetGoPhase = 0;
	for(i = 0;i < maxFrame;i++) maxStage += table[i].period;
#endif

#ifdef _SURMAP_
/*
	const int cInd = -1;
	for(i = 0;i < maxFrame;i++){
		if(i == cInd) continue;
//		table[i].cut(table[cInd].x0 - 0,table[cInd].y0 + 0,table[cInd].sx + 0,table[cInd].sy - 0);
		table[i].cut(390,120,650 - 390,340 - 120);
		}
	save(0);
	ErrH.Abort("CUT READY");
*/
#endif

}

MobileLocation* MobileLocation::cloning(int _dx,int _dy, const char* _name)
{
	MobileLocation* ml = new MobileLocation;
	*ml = *this;
	if(isAlt) ml -> alt = new MLAtype[altSx*altSy];
	ml -> isClone = 1;
	memset(ml -> steps = new int[maxFrame],0,maxFrame*sizeof(int));
	ml -> dx = _dx;
	ml -> dy = _dy;
	memcpy(ml -> name,_name,strlen(_name) > MLNAMELEN ? MLNAMELEN : strlen(_name) + 1);
	return ml;
}

#ifdef _SURMAP_
void MobileLocation::removeFile(void)
{
	remove(GetTargetName(fileName));
}

void MobileLocation::erase(void)
{
	removeFile();
	save(1);
}

void MobileLocation::save(int reserve)
{
	int i;
	XBuffer buf;
	XStream ff(0);

	buf < "data.vot/";
	if(reserve) buf < "reserved/";
	buf < name < ".vot";
	if(!reserve)
		if(ff.open(GetTargetName(buf.GetBuf()),XS_IN)){
			ff.close();
			XCon < "\nVOT file < " < name < "> already exist. Overwriting...";
			win32_MainWinMinimize();
			}
	if(!ff.open(GetTargetName(buf.GetBuf()),XS_OUT)){
		XCon < "\nCan't create VOT file < " < buf.GetBuf() < ">. Data may be lost...";
		win32_MainWinMinimize();
		return;
		}
	ff.write(MLsign,3);
	ff.write(name,MLNAMELEN + 1);
	ff < maxFrame < DryTerrain < Impulse;
	ff < (uchar)0 < (uchar)mode < (short)0;
	for(i = 1;i < MAX_KEYPHASE;i++) ff < KeyPhase[i];
	ff < (int)0;
	for(i = 0;i < maxFrame;i++) table[i].save(ff);
	ff.close();

	if(!reserve){
		::free(fileName);
		fileName = strdup(buf.GetBuf());
		}
}
#endif

void MLFrame::load(XStream& ff,int mode)
{
	ff > x0 > y0 > sx > sy > period > surfType;
	ff > csd > cst;
#ifdef _SURMAP_
	if(csd || cst)
		ErrH.Abort("Compressed ML not supported!");
#endif
	ff.seek(2*4,XS_CUR);
	sz = sx*sy;

	if(sz > ML_FRAME_SIZE)  ML_FRAME_SIZE = sz + 2;

	c_terrain = c_delta = NULL;
	
	if (!csd){
		delta = new uchar[sz + 2];
		memset(delta,'\0',sz + 2);
		ff.read(delta,sz);
#ifdef ML_COMPRESS_ON
		RLE_ANALISE(delta,sz,c_delta);
		delete[] delta; delta = NULL;
#endif
		}
	else {
		delta = NULL;
		c_delta = new uchar[csd];
		ff.read(c_delta,csd);
		}

	if(surfType >= TERRAIN_MAX){
		if (!cst){
			terrain = new uchar[sz + 2];
			memset(terrain,'\0',sz + 2);
			ff.read(terrain,sz);
#ifdef ML_COMPRESS_ON
			RLE_ANALISE(terrain,sz,c_terrain);
			delete[] terrain; terrain = NULL;
#endif
			}
		else {
			terrain = NULL;
			c_terrain = new uchar[cst];
			ff.read(c_terrain,cst);
			}
		}
	if(mode == MLM_RELATIVE){
		ss = sz/32 + 1;
		signBits = new unsigned[ss];
		ff.read(signBits,ss*4);
		}
}

#ifdef _SURMAP_
void MLFrame::save(XStream& ff)
{
	ff < x0 < y0 < sx < sy < period < surfType;
	ff < (int)0 < (int)0 < (int)0 < (int)0;
	ff.write(delta,sz);
	if(surfType >= TERRAIN_MAX){
		if(terrain)
			ff.write(terrain,sz);
		else
			surfType = -1;
		}
	else if(terrain){ delete terrain; terrain = NULL; }
	if(signBits) ff.write(signBits,ss*4);
}
#endif

MobileLocation::~MobileLocation(void)
{
	if(!isClone){
		for(int i = 0;i < maxFrame;i++){
			if(table[i].delta) delete table[i].delta;
			if(table[i].terrain) delete table[i].terrain;
			if(table[i].c_delta) delete[] table[i].c_delta;
			if(table[i].c_terrain) delete[] table[i].c_terrain;
			if(table[i].signBits) delete[] table[i].signBits;
			}
		if(table) delete[] table;
		}
	if(isAlt && alt) delete[] alt;
	if(steps) delete[] steps;
}

#ifdef _ROAD_
void MobileLocation::checkQuant(void)
{
	int checked = table[cFrame].check(dy);
	if(checked && frozen){
		steps[cFrame] = 0;
		cFrame = 0;
		frozen = 0;
		cStage = -1;
		setPhase(0,1);
		goPh = 0;
	}else{
		if(!checked && !frozen){
			steps[cFrame] = 0;			
			cFrame = 0;
			frozen = 1;
			cStage = -1;
			goPh = 0;
		};
	};
};
#endif

#ifdef _ROAD_
extern int StartMainQuantFlag;
#endif

int MobileLocation::quant(int render,int skipVZ,int skipCheck)
{
#ifdef _SURMAP_
	if(!inUse) return 0;
	int xsd = curGMap -> xside;
	int ysd = curGMap -> yside;
	if(!skipVZ){
		if(!(getDistX(x0 + dx,ViewX) < xsd && getDistX(ViewX,XCYCL(x0 + dx + altSx - 1)) < xsd)) return 0;
		if(!(getDistY(y0 + dy,ViewY) < ysd && getDistY(ViewY,YCYCL(y0 + dy + altSy - 1)) < ysd)) return 0;
	}
#endif

	int checked;
	if(!skipCheck){
		checked = table[cFrame].check(dy);
		if(checked && frozen){
			steps[cFrame] = 0;
			cFrame = 0;
			frozen = 0;
#ifdef _ROAD_
			cStage = -1;
			setPhase(0,1);
			goPh = 0;
#endif
			return 0;
			}
		else {
			if(!checked && !frozen){
				steps[cFrame] = 0;
				cFrame = 0;
				frozen = 1;
#ifdef _ROAD_		
				cStage = -1;
				goPh = 0;
//				vMap->delink(YCYCL(y0 - altSy),YCYCL(y0 + altSy));
#endif
				return 0;
				}
			if(frozen) return 0;
			}
		}

#ifdef _ROAD_
//	if(!StartMainQuantFlag) return 0;
#endif

	if(getCurPhase() == goPh)
		return 0;

#ifdef _ROAD_		
	cStage++;
#endif	

#ifdef _SURMAP_
	if(mode == MLM_RELATIVE || mode == MLM_REL2ABS){
		if(mode == MLM_REL2ABS) isMLFConverted = isConverted + cFrame;
#else
	if(mode == MLM_RELATIVE){
#endif		
		if(table[cFrame].quant(steps[cFrame],dx,dy,alt,DryTerrain,render,fastMode)){
			if(++cFrame == maxFrame){
				cStage = 0;
				steps[cFrame = 0] = 0;
				}
			if(!render) return 1;
			if(!skipCheck && table[cFrame].check(dy)) 
#ifdef _SURMAP_
				if(mode == MLM_REL2ABS) isMLFConverted = isConverted + cFrame;
#endif		
				table[cFrame].quant(steps[cFrame],dx,dy,alt,DryTerrain,render,fastMode);
			}
		}
	else {
		if(table[cFrame].quantAbs(steps[cFrame],dx,dy,alt,DryTerrain,render,fastMode)){
			if(++cFrame == maxFrame){
				cStage = 0;
				steps[cFrame = 0] = 0;
				}
			if(!render) return 1;
			if(!skipCheck && table[cFrame].check(dy)){
				table[cFrame].quantAbs(steps[cFrame],dx,dy,alt,DryTerrain,render,fastMode);
				}
			}
		}
#ifdef _SURMAP_
	if(mode == MLM_REL2ABS){
		for(int i = 0;i < maxFrame;i++) if(!isConverted[i]) return 1;
		mode = MLM_ABSOLUTE;
		isMLFConverted = NULL;
		if(alt){ delete alt; alt = NULL; }
		save();
		}
#endif
	return 1;
}

#ifdef _SURMAP_
#define VISI_OPERATION	if(MLvisi) if(*pd) *(vMap -> lineTcolor[y] + x) = 255; else *(vMap -> lineTcolor[y] + x) = 253;
#else
#define VISI_OPERATION
#endif

int MLFrame::quant(int& step,int dx,int dy,MLAtype* alt,int dry,int render,int fastMode)
{
#ifdef _SURMAP_
	if(isMLFConverted) period = 1;
#endif
	if(!step && period > 1 && !fastMode) start(dx,dy,alt);
	step++;
	if(step > period || (step > 1 && fastMode)){
		step = 0;
		return 1;
		}
	if(fastMode == 2) return 0;

	uchar** lt = vMap -> lineT;
	uchar* pa0,*pf0;
	unsigned* sb = signBits;
	unsigned mask = 1;
	MLAtype* pt = alt;
	int dl;

#ifdef ML_COMPRESS_ON
	uchar* pd = ML_FRAME_DELTA;
	RLE_UNCODE(pd, sz, c_delta);
#else
	uchar* pd = delta;
	if(csd){
		pd = ML_FRAME_DELTA;
		RLE_UNCODE(pd,sz,c_delta);	
		}
#endif

	int i,j,x,y,v;
	uchar* pr;
	if(step == period || fastMode){
#ifdef ML_COMPRESS_ON
		if(c_terrain){
			pr = ML_FRAME_TERRAIN;
			RLE_UNCODE(pr,sz,c_terrain);
			}
		else pr = NULL;
#else
		if(cst){
			pr = ML_FRAME_TERRAIN;
			RLE_UNCODE(pr,sz,c_terrain);
			}
		pr = terrain;
#endif
		}
	else pr =  NULL;

	int sft = (step == period || fastMode) ? surfType : -1;
#ifdef _SURMAP_
	uchar* pd0 = pd;
	uchar* pr0 = pr;
#endif
	if(period > 1 && !fastMode)
		if(!pr)
			if(sft == -1){
				for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
					pa0 = lt[y];
					pf0 = pa0 + H_SIZE;
					for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pd++,pt++){
						VISI_OPERATION
						dl = *pd;
						if(dl){
							v = (int)*(pa0 + x) - (*pt >> MLPREC);
							if(*sb & mask) dl = -dl;
							v += ((*pt += ((dl << MLPREC)/period)) >> MLPREC);
							if(v < 1){ v = 0; *pt = 0; }
							SET_UP_ALT(pf0 + x,v,pa0,x);
							}
						if(!(mask <<= 1)){ sb++; mask = 1; }
						}
					}
				}
			else {
				int type = sft << TERRAIN_OFFSET;
				for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
					pa0 = lt[y];
					pf0 = pa0 + H_SIZE;
					for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pd++,pt++){
						VISI_OPERATION
						dl = *pd;
						if(dl){
							v = (int)*(pa0 + x) - (*pt >> MLPREC);
							if(*sb & mask) dl = -dl;
							v += ((*pt += ((dl << MLPREC)/period)) >> MLPREC);
							if(v < 1){ v = 0; *pt = 0; }
							SET_UP_ALT(pf0 + x,v,pa0,x);
							SET_REAL_TERRAIN(pf0 + x,type,x);
							}
						if(!(mask <<= 1)){ sb++; mask = 1; }
						}
					}
				}
		else 
			for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
				pa0 = lt[y];
				pf0 = pa0 + H_SIZE;
				for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pd++,pt++,pr++){
					VISI_OPERATION
					dl = *pd;
					if(dl){
						v = (int)*(pa0 + x) - (*pt >> MLPREC);
						if(*sb & mask) dl = -dl;
						v += ((*pt += ((dl << MLPREC)/period)) >> MLPREC);
						if(v < 1){ v = 0; *pt = 0; }
						SET_UP_ALT(pf0 + x,v,pa0,x);
						SET_REAL_TERRAIN(pf0 + x,*pr,x);
						}
					if(!(mask <<= 1)){ sb++; mask = 1; }
					}
				}
	else
		if(!pr)
			if(sft == -1){
				for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
					pa0 = lt[y];
					pf0 = pa0 + H_SIZE;
					for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pd++){
						VISI_OPERATION
						dl = *pd;
						if(dl){
							if(*sb & mask) dl = -dl;
							v = (int)*(pa0 + x) + dl;
							if(v < 0) v = 0;
							SET_UP_ALT(pf0 + x,v,pa0,x);
							}
						if(!(mask <<= 1)){ sb++; mask = 1; }
						}
					}
				}
			else {
				int type = sft << TERRAIN_OFFSET;
				for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
					pa0 = lt[y];
					pf0 = pa0 + H_SIZE;
					for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pd++){
						VISI_OPERATION
						dl = *pd;
						if(dl){
							if(*sb & mask) dl = -dl;
							v = (int)*(pa0 + x) + dl;
							if(v < 0) v = 0;
							SET_UP_ALT(pf0 + x,v,pa0,x);
							SET_REAL_TERRAIN(pf0 + x,type,x);
							}
						if(!(mask <<= 1)){ sb++; mask = 1; }
						}
					}
				}
		else 
			for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
				pa0 = lt[y];
				pf0 = pa0 + H_SIZE;
				for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pd++,pr++){
					VISI_OPERATION
					dl = *pd;
					if(dl){
						if(*sb & mask) dl = -dl;
						v = (int)*(pa0 + x) + dl;
						if(v < 0) v = 0;
						SET_UP_ALT(pf0 + x,v,pa0,x);
						SET_REAL_TERRAIN(pf0 + x,*pr,x);
						}
					if(!(mask <<= 1)){ sb++; mask = 1; }
					}
				}
#ifdef _SURMAP_
	if(isMLFConverted){
		*isMLFConverted = 1;
		if(signBits){ delete signBits; signBits = NULL; }
		pd = pd0;
		pr = pr0;
		if(!pr)
			for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
				pa0 = lt[y];
				pf0 = pa0 + H_SIZE;
				for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pd++)
					if(*pd) *pd = *(pa0 + x);
				}
		else 
			for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
				pa0 = lt[y];
				pf0 = pa0 + H_SIZE;
				for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pd++,pr++)
					if(*pd){
						*pd = *(pa0 + x);
						*pr = GET_TERRAIN_TYPE(*(pf0 + x));
						}
				}
		}
	if(!MLvisi)
#endif
		if(render){
			regSet(x0 + dx,y0 + dy,x0 + dx + sx - 1,y0 + dy + sy - 1,dry << TERRAIN_OFFSET,0);
			Render(dx,dy);
			}

	return 0;
}

int MLFrame::quantAbs(int& step,int dx,int dy,MLAtype* alt,int dry,int render,int fastMode)
{
	step++;
	if(step > 1){
		step = 0;
		return 1;
		}
	if(fastMode == 2) return 0;

	uchar** lt = vMap -> lineT;
	uchar* pa0,*pf0;
#ifdef ML_COMPRESS_ON
	uchar* pd = ML_FRAME_DELTA;
#else
	uchar* pd = delta;
#endif

#ifdef ML_COMPRESS_ON
	RLE_UNCODE(pd, sz, c_delta);
#else
	if (csd){
		pd = ML_FRAME_DELTA;
		RLE_UNCODE(pd, sz, c_delta);	
		}
#endif

	int i,j,x,y;
	uchar* pr;
	if(step == 1 || fastMode) {
#ifdef ML_COMPRESS_ON
		if ( c_terrain ){
			pr = ML_FRAME_TERRAIN;
			RLE_UNCODE(pr, sz, c_terrain);
			}
		else pr = NULL;
#else
		if(cst){
			pr = ML_FRAME_TERRAIN;
			RLE_UNCODE(pr, sz, c_terrain);
			}
		pr = terrain;
#endif
		}
	else pr =  NULL;

	int sft = (step == 1 || fastMode) ? surfType : -1;
	if(!pr)
		if(sft == -1){
			for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
				pa0 = lt[y];
				pf0 = pa0 + H_SIZE;
				for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pd++){
					VISI_OPERATION
					if(*pd) *(pa0 + x) = *pd;
					}
				}
			}
		else {
			int type = sft << TERRAIN_OFFSET;
			for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
				pa0 = lt[y];
				pf0 = pa0 + H_SIZE;
				for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pd++){
					VISI_OPERATION
					if(*pd){
						*(pa0 + x) = *pd;
						SET_TERRAIN(*(pf0 + x),type);
						}
					}
				}
			}
	else 
		for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
			pa0 = lt[y];
			pf0 = pa0 + H_SIZE;
			for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pd++,pr++){
				VISI_OPERATION
				if(*pd){
					*(pa0 + x) = *pd;
					SET_TERRAIN(*(pf0 + x),*pr);
					}
				}
			}
#ifdef _SURMAP_
	if(!MLvisi)
#endif
		if(render){
			regSet(x0 + dx,y0 + dy,x0 + dx + sx - 1,y0 + dy + sy - 1,dry << TERRAIN_OFFSET,0);
			Render(dx,dy);
			}

	return 0;
}

void MLFrame::start(int dx,int dy,MLAtype* alt)
{
	uchar** lt = vMap -> lineT;
	uchar* pa0;
	MLAtype* pt = alt;

	int i,j,x,y;
	for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
		pa0 = lt[y];
		for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pt++)
			*pt = (((MLAtype)*(pa0 + x)) << MLPREC) + (1 << (MLPREC - 1));
		}
}

int MLFrame::check(int dy)
{
	uchar** lt = vMap -> lineT;
	int j,y;
	for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1))
		if(!lt[y]) return 0;
	return 1;
}

void MobileLocation::calcBounds(void)
{
	x0 = table[0].x0;
	y0 = table[0].y0;
	x1 = XCYCL(x0 + table[0].sx - 1);
	y1 = YCYCL(y0 + table[0].sy - 1);
	for(int i = 1;i < maxFrame;i++){
		if(getDistX(x0,table[i].x0) > 0) x0 = table[i].x0;
		if(getDistY(y0,table[i].y0) > 0) y0 = table[i].y0;
		if(getDistX(XCYCL(table[i].x0 + table[i].sx - 1),x1) > 0) x1 = XCYCL(table[i].x0 + table[i].sx - 1);
		if(getDistY(YCYCL(table[i].y0 + table[i].sy - 1),y1) > 0) y1 = YCYCL(table[i].y0 + table[i].sy - 1);
		}
}

int MLquant(void)
{
#ifdef _SURMAP_
	if(MLstatus || !MLprocess) return 0;
#endif
	int ret = 0;
	for(int i = 0;i < MLTableSize;i++) ret += MLTable[i] -> quant();
	return ret;
}

#ifdef _ROAD_
void MLCheckQuant(void)
{
	for(int i = 0;i < MLTableSize;i++)
		{
		MLTable[i] -> checkQuant();
		}
}

char MobileLocation::GetVisible(void)
{
	if(abs(getDistX(x0 + dx,ViewX)) - altSx < TurnSideX &&
	    abs(getDistY(y0 + dy,ViewY)) - altSy < TurnSideY) return 1;
	else return 0;
}
#endif

void MobileLocation::put(int x,int y)
{	
	table[0].put(x,y,alt);
}

void MLFrame::put(int x,int y,MLAtype* alt)
{
	x0 = XCYCL(x - sx/2);
	y0 = YCYCL(y - sy/2);
	int step = 0;
#ifdef _SURMAP_
	quant(step,0,0,alt,1);	
#else 
	if(check(0)) DestroyQuant(0,0,alt,1);
#endif
}

void MobileLocation::setPhase(int nPhase,int noChange)
{
	if(nPhase < 0 || nPhase > maxFrame - 1) return;
	if(cFrame == nPhase && !steps[cFrame]) return;
	int val = goPh;
	goPh = -1;
	int log = 1;
	int f = cFrame;
#ifdef _ROAD_
	int skipCheck = 1;
	fastMode = 1 + noChange;
#else
	int skipCheck = 0;
#endif
	while(log && steps[f]) log = quant(0,1,skipCheck);
	fastMode = 1 + noChange;
	while(log && (cFrame != nPhase || steps[cFrame])) log = quant(0,1,skipCheck);
	fastMode = 0;
	if(log && !noChange){
		regSet(x0 + dx,y0 + dy,x1 + dx,y1 + dy,DryTerrain << TERRAIN_OFFSET,0);
		regRender(x0 + dx,y0 + dy,x1 + dx,y1 + dy,0);
		}
	goPh = val;
}

void MobileLocation::goPhase(int nPhase)
{
#ifdef _SURMAP_
	if(nPhase > maxFrame - 1) nPhase = maxFrame - 1;
#endif
	goPh = nPhase;
};

const int MaxAddDanger[WORLD_MAX] = {0,11,0,0,0 ,0,0,0,0,0};
extern int NumAddDanger;

void VLload(void)
{
	const char* errm = "Wrong VLC format";
	XBuffer buf;
	XStream ff(0);
	char sign[8];
	int i;

	buf < "data.vot/" < VLfilenames[0] < ".vlc";
	if(ff.open(GetTargetName(buf.GetBuf()),XS_IN)){
		ff.read(sign,strlen(VLCsign[0]));
		if(memcmp(sign,VLCsign[0],strlen(VLCsign[0]))) ErrH.Abort(errm);
		ff > TntTableSize;
#ifdef _SURMAP_
		TntTable = new TntValoc[TntTableSize];
		for(i = 0;i < TntTableSize;i++){
			TntTable[i].load(ff);
			TntTable[i].link();
		}
#else
	#ifdef NEW_TNT
		TntObjectData = new TntCreature*[TntTableSize];
		for(i = 0;i < TntTableSize;i++){
			TntObjectData[i] = new TntCreature;
			TntObjectData[i]->Load(ff);
		};
	#else
		TntObjectData = new TntStaticObject*[TntTableSize];
		for(i = 0;i < TntTableSize;i++){
			TntObjectData[i] = new TntStaticObject;
			TntObjectData[i]->Load(ff);
		};
	#endif
#endif
		ff.close();
		}

	buf.init();
	buf < "data.vot/" < VLfilenames[1] < ".vlc";
	if(ff.open(GetTargetName(buf.GetBuf()),XS_IN)){
		ff.read(sign,strlen(VLCsign[1]));
		if(memcmp(sign,VLCsign[1],strlen(VLCsign[1]))) ErrH.Abort(errm);
		ff > MLCTableSize;
#ifdef _SURMAP_
		MLCTable = new MLCloneValoc[MLCTableSize];
		for(i = 0;i < MLCTableSize;i++){
			MLCTable[i].load(ff);
			MLCTable[i].link();
		};
#else
#endif
		ff.close();
		}
	

	buf.init();
	buf < "data.vot/" < VLfilenames[2] < ".vlc";
	if(ff.open(GetTargetName(buf.GetBuf()),XS_IN)){
		ff.read(sign,strlen(VLCsign[2]));
		if(memcmp(sign,VLCsign[2],strlen(VLCsign[2]))) ErrH.Abort(errm);
		ff > SnsTableSize;
/*#ifndef _NT
		SnsTableSize = 0;
#endif*/

#ifdef _SURMAP_
		SnsTable = new SensorValoc[SnsTableSize];
		for(i = 0;i < SnsTableSize;i++){
			SnsTable[i].load(ff);
			SnsTable[i].link();
		};
#else
//#ifdef _NT	
		SensorObjectData = new SensorDataType*[SnsTableSize];
		for(i = 0;i < SnsTableSize;i++){
			SensorObjectData[i] = new SensorDataType;
			SensorObjectData[i]->CreateSensor(ff,i);
		};

//------------------------------------------------------------------------------
/*		XStream fff;
		buf.init();
		buf < "DATA.VOT\\" < VLfilenames[2] < ".VLC";
		fff.open(GetTargetName(buf.GetBuf()),XS_OUT);
		fff.write(sign,strlen(VLCsign[2]));
		fff < SnsTableSize;
		for(i = 0;i < SnsTableSize;i++)
			SensorObjectData[i]->SaveSensor(fff);
		fff.close();*/
//------------------------------------------------------------------------------
//#endif
#endif
		ff.close();
		}	      

#ifdef _ROAD_
	NumAddDanger = MaxAddDanger[CurrentWorld];
#endif
	buf.init();
	buf < "data.vot/" < VLfilenames[3] < ".vlc";
	if(ff.open(GetTargetName(buf.GetBuf()),XS_IN)){
		ff.read(sign,strlen(VLCsign[3]));
		if(memcmp(sign,VLCsign[3],strlen(VLCsign[3]))) ErrH.Abort(errm);
		ff > DngTableSize;
#ifdef _SURMAP_
		DngTable = new DangerValoc[DngTableSize];
		for(i = 0;i < DngTableSize;i++){
			DngTable[i].load(ff);
			DngTable[i].link();
			}
#else

		DangerObjectData = new DangerDataType*[DngTableSize + NumAddDanger];
		for(i = 0;i < DngTableSize;i++){
			DangerObjectData[i] = new DangerDataType;
			DangerObjectData[i]->CreateDanger(ff);
		};

		for(i=0; i<NumAddDanger; i++) {
			DangerObjectData[DngTableSize + i] = new DangerDataType;
			DangerObjectData[DngTableSize + i]->R_curr = Vector(0, 0, 0);
		}

		NumAddDanger = DngTableSize;
		DngTableSize += MaxAddDanger[CurrentWorld];
#endif
		ff.close();
		}      
	
	
}

#ifdef _SURMAP_
void TntValoc::load(XStream& ff)
{
	ff > x > y > z;
	ff > value > radius > type;
}

void MLCloneValoc::load(XStream& ff)
{
	ff > x > y > z;
	ff > value;
};

void SensorValoc::load(XStream& ff)
{
	ff > x > y > z;
	ff > id > radius > nameLen;
	if(nameLen){
		name = new char[nameLen + 1];
		ff.read(name,nameLen);
		name[nameLen] = '\0';
		}
	else
		name = "";

	ff > z0;
	ff > data0;
	ff > data1;
	ff > data2;
	ff > data3;
	ff > data4;
	ff > data5;
	ff > data6;
//	ff.seek(8*sizeof(int),XS_CUR);
}

void DangerValoc::load(XStream& ff)
{
	ff > x > y > z;
	ff > type > radius;
	ff.seek(4*sizeof(int),XS_CUR);
}
#endif

#ifndef _SURMAP_

const int TerrainDestroyTable[TERRAIN_MAX] = {100,100,5,5,0,0,10,10};

int MLFrame::DestroyQuant(int dx,int dy,MLAtype* alt,int dry)
{
	uchar** lt = vMap -> lineT;
	uchar* pa0,*pf0;
	uchar* pd = delta;
	unsigned* sb = signBits;
	unsigned mask = 1;
	int dl;

#ifdef ML_COMPRESS_ON
	pd = ML_FRAME_DELTA;
	RLE_UNCODE(pd, sz, c_delta);
#else
	if (csd){
		pd = ML_FRAME_DELTA;
		RLE_UNCODE(pd, sz, c_delta);
	} 
#endif

	int i,j,x,y,v;
	for(j = 0,y = YCYCL(y0 + dy);j < sy;j++,y = YCYCL(y + 1)){
		pa0 = lt[y];
		pf0 = pa0 + H_SIZE;
		for(i = 0,x = XCYCL(x0 + dx);i < sx;i++,x = XCYCL(x + 1),pd++){
			dl = *pd;
			if(dl && (int)RND(100) < TerrainDestroyTable[GET_TERRAIN(*(pf0 + x))]){
				if(*sb & mask) dl = -dl;
				v = (int)*(pa0 + x) + dl;
				if(v < 0) v = 0;
				SET_UP_ALT(pf0 + x,v,pa0,x);
				}
			if(!(mask <<= 1)){ sb++; mask = 1; }
			}
		}
	regSet(x0 + dx,y0 + dy,x0 + dx + sx - 1,y0 + dy + sy - 1,dry << TERRAIN_OFFSET,0);
	Render(dx,dy);

	return 0;
};


const int MAX_DESTROY_SIDE = 2*MAX_DESTROY_RADIUS + 1;

int* RadiusDestroyX[MAX_DESTROY_RADIUS];
int* RadiusDestroyY[MAX_DESTROY_RADIUS];
int NumDestroyRadius[MAX_DESTROY_RADIUS];
short *NumDestroyRadiusTable;

const int ALT_PROTECTED_VALUE = 2;
const int ALT_PROTECTED_OFFSET = 256*ALT_PROTECTED_VALUE / 2 - 128;
uchar* AltProtectedTable;

void DestroySmoothInit(void)
{
	NumDestroyRadiusTable = new short[MAX_DESTROY_SIDE*MAX_DESTROY_SIDE];

	int i,j,r,ind;
	short* p = NumDestroyRadiusTable;
	int* xheap;
	int* yheap;

	for(j = -MAX_DESTROY_RADIUS;j <= MAX_DESTROY_RADIUS;j++){
		for(i = -MAX_DESTROY_RADIUS;i <= MAX_DESTROY_RADIUS;i++){
			r = (int)sqrt(i*(double)i + j*(double)j);
			if(r >= MAX_DESTROY_RADIUS){
				*p = r;
			}	else{
				*p = r;
				NumDestroyRadius[r]++;
			};
			p++;
		};
	};

	for(ind = 0;ind < MAX_DESTROY_RADIUS;ind++){
		RadiusDestroyX[ind] = new int[NumDestroyRadius[ind]];
		RadiusDestroyY[ind] = new int[NumDestroyRadius[ind]];
		xheap = RadiusDestroyX[ind];
		yheap = RadiusDestroyY[ind];
		for(p = NumDestroyRadiusTable,r = 0,j = -MAX_DESTROY_RADIUS;j <= MAX_DESTROY_RADIUS;j++){
			for(i = -MAX_DESTROY_RADIUS;i <= MAX_DESTROY_RADIUS;i++){
				if(*p == ind){
					xheap[r] = i;
					yheap[r] = j;
					r++;
				};
				p++;
			};
		};
	};

	AltProtectedTable = new uchar[256*ALT_PROTECTED_VALUE];
	for(i = 0;i < 256*ALT_PROTECTED_VALUE;i++){
		if(i < ALT_PROTECTED_OFFSET) AltProtectedTable[i] = 0;
		else{
			if(i > ALT_PROTECTED_OFFSET + 255) AltProtectedTable[i] = 255;
			else AltProtectedTable[i] = i - ALT_PROTECTED_OFFSET;
		};
	};
};

void DestroySmoothFree(void)
{
	int i;
	for(i = 0;i < MAX_DESTROY_RADIUS;i++){
		NumDestroyRadius[i] = 0;
		delete[] RadiusDestroyX[i];
		delete[] RadiusDestroyY[i];
	};
	delete[] AltProtectedTable;
	delete[] NumDestroyRadiusTable;
};

int DestroyBarellSmoothWithCheck(int x0,int y0,int SmoothRadius,int SpotRadius,int Delta,uchar terrain,int Num,int z0,uchar rfactor)
{
	int Phi = RND(PI/2);
	uchar* pa0,*pf0;
	int i,j,n;

	uchar** lt = vMap -> lineT;

	int rx,ry;
	int* dx,*dy;
	int h;

	for(i = y0 - SmoothRadius;i <= y0 + SmoothRadius;i++) if(!lt[YCYCL(i)]) return 0;

//	for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 0;
	memset(SmoothTerrainMask,0,TERRAIN_MAX);
	if(terrain != 83) SmoothTerrainMask[terrain] = 1;

	h = 2*SmoothRadius / 3;
	if(h >= MAX_DESTROY_RADIUS)
		ErrH.Abort("Bad Clear Radius");

	for(i = 0;i < h;i++){
		dx = RadiusDestroyX[i];
		dy = RadiusDestroyY[i];
		for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
			rx = XCYCL((*dx) + x0);
			ry = YCYCL((*dy) + y0);
			pa0 = lt[ry] + rx;
			pf0 = pa0 + H_SIZE;
			if(SmoothTerrainMask[GET_TERRAIN(*pf0)]){
				if(*pf0 & DOUBLE_LEVEL){
					if(rx & 1) 
						*pa0 = AltProtectedTable[z0 + (((*pa0) - z0) >> 6) + ALT_PROTECTED_OFFSET];
				}else 
					*pa0 = AltProtectedTable[z0 + (((*pa0) - z0) >> 6) + ALT_PROTECTED_OFFSET];
			};
		};
	};

//	for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 1;
	memset(SmoothTerrainMask,1,TERRAIN_MAX);
	SmoothTerrainMask[4] = 0;
	SmoothTerrainMask[5] = 0;

	if(Delta > 0){
		for(i = 0;i < Num;i++){
			n = RND(SmoothRadius - SpotRadius);
			rx = XCYCL(x0 + ((CO[Phi] * n) >> 16));
			ry = YCYCL(y0 + ((SI[Phi] * n) >> 16));
			DestroyBarellSpot(rx,ry,SpotRadius,terrain,RND(Delta),rfactor);
			Phi = rPI(Phi + 2*PI / Num);
		};
	}else{
		for(i = 0;i < Num;i++){
			n = RND(SmoothRadius - SpotRadius);
			rx = XCYCL(x0 + ((CO[Phi] * n) >> 16));
			ry = YCYCL(y0 + ((SI[Phi] * n) >> 16));
			DestroyBarellSpot(rx,ry,SpotRadius,terrain,-(int)(RND(-Delta)),rfactor);
			Phi = rPI(Phi + 2*PI / Num);
		};
	};


	DestroySmoothPut(x0,y0,SmoothRadius);


	int x1,y1;
	rx = XCYCL(x0 - SmoothRadius);
	ry = YCYCL(y0 - SmoothRadius);

	x1 = XCYCL(x0 + SmoothRadius);
	y1 = YCYCL(y0 + SmoothRadius);

#ifdef _ROAD_
	if(CurrentWorld != WORLD_NECROSS)
#endif
		regSet(rx,ry,x1,y1,1,0);
	regRender(rx,ry,x1,y1);

	return 1;
};


void DestroyBarellSmooth(int x0,int y0,int SmoothRadius,int SpotRadius,int Delta,uchar terrain,int Num,int z0,uchar rfactor)
{
	int Phi = RND(PI/2);
	uchar* pa0,*pf0;
	int i,j,n;

	uchar** lt = vMap -> lineT;

	int rx,ry;
	int* dx,*dy;
	int h;

	for(i = y0 - SmoothRadius;i <= y0 + SmoothRadius;i++) if(!lt[YCYCL(i)]) return;

	if(terrain != 83){		
//		for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 0;
		memset(SmoothTerrainMask,0,TERRAIN_MAX);
		SmoothTerrainMask[terrain] = 1;		
	}else{	
//		for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 1;
		memset(SmoothTerrainMask,1,TERRAIN_MAX);
	};

	h = 2*SmoothRadius / 3;

	if(h >= MAX_DESTROY_RADIUS)
		ErrH.Abort("Bad Clear Radius");

	for(i = 0;i < h;i++){
		dx = RadiusDestroyX[i];
		dy = RadiusDestroyY[i];
		for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
			rx = XCYCL((*dx) + x0);
			ry = YCYCL((*dy) + y0);
			pa0 = lt[ry] + rx;
			pf0 = pa0 + H_SIZE;
			if(SmoothTerrainMask[GET_TERRAIN(*pf0)]){
				if(*pf0 & DOUBLE_LEVEL){
					if(rx & 1) 
						*pa0 = AltProtectedTable[z0 + (((*pa0) - z0) >> 6) + ALT_PROTECTED_OFFSET];
				}else 
					*pa0 = AltProtectedTable[z0 + (((*pa0) - z0) >> 6) + ALT_PROTECTED_OFFSET];
			};
		};
	};

//	for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 1;

	memset(SmoothTerrainMask,1,TERRAIN_MAX);
	SmoothTerrainMask[4] = 0;
	SmoothTerrainMask[5] = 0;

	if(Delta > 0){
		for(i = 0;i < Num;i++){
			n = RND(SmoothRadius - SpotRadius);
			rx = XCYCL(x0 + ((CO[Phi] * n) >> 16));
			ry = YCYCL(y0 + ((SI[Phi] * n) >> 16));
			DestroyBarellSpot(rx,ry,SpotRadius,terrain,RND(Delta),rfactor);
			Phi = rPI(Phi + 2*PI / Num);
		};
	}else{
		for(i = 0;i < Num;i++){
			n = RND(SmoothRadius - SpotRadius);
			rx = XCYCL(x0 + ((CO[Phi] * n) >> 16));
			ry = YCYCL(y0 + ((SI[Phi] * n) >> 16));
			DestroyBarellSpot(rx,ry,SpotRadius,terrain,-(int)(RND(-Delta)),rfactor);
			Phi = rPI(Phi + 2*PI / Num);
		};
	};

	DestroySmoothPut(x0,y0,SmoothRadius);

	int x1,y1;
	rx = XCYCL(x0 - SmoothRadius);
	ry = YCYCL(y0 - SmoothRadius);

	x1 = XCYCL(x0 + SmoothRadius);
	y1 = YCYCL(y0 + SmoothRadius);

#ifdef _ROAD_
	if(CurrentWorld != WORLD_NECROSS)
#endif
		regSet(rx,ry,x1,y1,1,0);
	regRender(rx,ry,x1,y1,0);
};

void ClearBarell(int x0,int y0,int radius,uchar terrain,int z0)
{
	uchar* pa0,*pf0;
	int i,j;

	uchar** lt = vMap -> lineT;

	int rx,ry;
	int* dx,*dy;

	for(i = y0 - radius;i <= y0 + radius;i++) if(!lt[YCYCL(i)]) return;

	if(terrain != 83){
//		for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 0;
		memset(SmoothTerrainMask,0,TERRAIN_MAX);
		SmoothTerrainMask[terrain] = 1;		
	}else{	
//		for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 1;
		memset(SmoothTerrainMask,1,TERRAIN_MAX);
	};

	if(radius >= MAX_DESTROY_RADIUS)
		ErrH.Abort("Bad Clear Radius");

	for(i = 0;i < radius;i++){
		dx = RadiusDestroyX[i];
		dy = RadiusDestroyY[i];
		for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
			rx = XCYCL((*dx) + x0);
			ry = YCYCL((*dy) + y0);
			pa0 = lt[ry] + rx;
			pf0 = pa0 + H_SIZE;
			if(SmoothTerrainMask[GET_TERRAIN(*pf0)]){
				if(*pf0 & DOUBLE_LEVEL){
					if(rx & 1) 
						*pa0 = AltProtectedTable[z0 + (((*pa0) - z0) >> 6) + ALT_PROTECTED_OFFSET];
				}else 
					*pa0 = AltProtectedTable[z0 + (((*pa0) - z0) >> 6) + ALT_PROTECTED_OFFSET];
			};
		};
	};

	int x1,y1;
	rx = XCYCL(x0 - radius);
	ry = YCYCL(y0 - radius);

	x1 = XCYCL(x0 + radius);
	y1 = YCYCL(y0 + radius);

#ifdef _ROAD_
	if(CurrentWorld != WORLD_NECROSS)
#endif
		regSet(rx,ry,x1,y1,1,0);
	regRender(rx,ry,x1,y1);
};


void DestroySmooth(int x0,int y0,int SmoothRadius,int SpotRadius,int Delta,int cDelta,uchar terrain,int Num,uchar rfactor)
{
	int Phi = RND(PI/2);
	int i,n,rx,ry;

	uchar** lt = vMap -> lineT;
	for(i = y0 - SmoothRadius;i <= y0 + SmoothRadius;i++) if(!lt[YCYCL(i)]) return;

//	for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 1;
	memset(SmoothTerrainMask,1,TERRAIN_MAX);
//	if(CurrentWorld == WORLD_NECROSS) SmoothTerrainMask[0] = 0;
	SmoothTerrainMask[4] = 0;
	SmoothTerrainMask[5] = 0;

	if(Delta > 0){
		for(i = 0;i < Num;i++){
			n = RND(SmoothRadius - SpotRadius);
			rx = XCYCL(x0 + ((CO[Phi] * n) >> 16));
			ry = YCYCL(y0 + ((SI[Phi] * n) >> 16));
			DestroyBarellSpot(rx,ry,SpotRadius,terrain,cDelta + RND(Delta),rfactor);
			Phi = rPI(Phi + 2*PI / Num);
		};
	}else{
		for(i = 0;i < Num;i++){
			n = RND(SmoothRadius - SpotRadius);
			rx = XCYCL(x0 + ((CO[Phi] * n) >> 16));
			ry = YCYCL(y0 + ((SI[Phi] * n) >> 16));
			DestroyBarellSpot(rx,ry,SpotRadius,terrain,cDelta - RND(-Delta),rfactor);
			Phi = rPI(Phi + 2*PI / Num);
		};
	};

	DestroySmoothPut(x0,y0,SmoothRadius);

	int x1,y1;
	rx = XCYCL(x0 - SmoothRadius);
	ry = YCYCL(y0 - SmoothRadius);

	x1 = XCYCL(x0 + SmoothRadius);
	y1 = YCYCL(y0 + SmoothRadius);

#ifdef _ROAD_
	if(CurrentWorld != WORLD_NECROSS)
#endif
		regSet(rx,ry,x1,y1,1,0);
	regRender(rx,ry,x1,y1);
};

void DestroySpot(int x0,int y0,int Radius,uchar terrain,int delta,uchar rfactor)
{
	uchar* pa0,*pf0;
	int i,j;
	uchar** lt = vMap -> lineT;
	int rx,ry;
	uchar t;
	int r;
	int* dx,*dy;
	int rrad;

	for(i = y0 - Radius;i <= y0 + Radius;i++) if(!lt[YCYCL(i)]) return;

	switch(rfactor){
		case 0:
			rrad = 0;
			break;
		case 1:
			rrad = Radius >> 3;
			break;
		case 2:
			rrad = Radius >> 2;
			break;
		case 3:
			rrad = Radius >> 1;
			break;
		case 4:
			rrad = Radius;
			break;
		case 5:
			rrad = Radius << 1;
			break;
		case 6:
			rrad = Radius << 2;
			break;
		case 7:
			rrad = Radius << 3;
			break;
		case 8:
			rrad = Radius << 4;
			break;
	};


	if(terrain == 83){
		for(i = 0;i < Radius;i++){
			dx = RadiusDestroyX[i];
			dy = RadiusDestroyY[i];
			r = (Radius - i) * delta >> 8;
			for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
				if((int)RND(rrad) < Radius - i){
					rx = XCYCL((*dx) + x0);
					ry = YCYCL((*dy) + y0);
					pa0 = lt[ry] + rx;
					pf0 = pa0 + H_SIZE;
					if(SmoothTerrainMask[GET_TERRAIN(*pf0)]){
						if(*pf0 & DOUBLE_LEVEL){
							if(rx & 1) 
								*pa0 = AltProtectedTable[(*pa0) + r + ALT_PROTECTED_OFFSET];
						}else 
							*pa0 = AltProtectedTable[(*pa0) + r + ALT_PROTECTED_OFFSET];
					};
				};
			};
		};
	}else{
		t = terrain <<TERRAIN_OFFSET;
		for(i = 0;i < Radius;i++){
			dx = RadiusDestroyX[i];
			dy = RadiusDestroyY[i];
			r = (Radius - i) * delta >> 8;
			for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
				if((int)RND(rrad) < Radius - i){
					rx = XCYCL((*dx) + x0);
					ry = YCYCL((*dy) + y0);
					pa0 = lt[ry] + rx;
					pf0 = pa0 + H_SIZE;
					
					if(SmoothTerrainMask[GET_TERRAIN(*pf0)]){
						if(*pf0 & DOUBLE_LEVEL){
							if(rx & 1){
								*pa0 = AltProtectedTable[(*pa0) + r + ALT_PROTECTED_OFFSET];
								SET_TERRAIN(*pf0,t);
							};
						}else{
							*pa0 = AltProtectedTable[(*pa0) + r + ALT_PROTECTED_OFFSET];
							SET_TERRAIN(*pf0,t);
						};
					};
				};
			};
		};
	};

	int x1,y1;
	rx = XCYCL(x0 - Radius);
	ry = YCYCL(y0 - Radius);

	x1 = XCYCL(x0 + Radius);
	y1 = YCYCL(y0 + Radius);

#ifdef _ROAD_
	if(CurrentWorld != WORLD_NECROSS)
#endif
		regSet(rx,ry,x1,y1,1,0);
	regRender(rx,ry,x1,y1);
};

void DestroyBarellSpot(int x0,int y0,int Radius,uchar terrain,int delta,uchar rfactor)
{
	uchar* pa0,*pf0;
	int i,j;
	uchar** lt = vMap -> lineT;
	int rx,ry;
	uchar t;
	int r;
	int* dx,*dy;
	int rrad;

	switch(rfactor){
		case 0:
			rrad = 0;
			break;
		case 1:
			rrad = Radius >> 3;
			break;
		case 2:
			rrad = Radius >> 2;
			break;
		case 3:
			rrad = Radius >> 1;
			break;
		case 4:
			rrad = Radius;
			break;
		case 5:
			rrad = Radius << 1;
			break;
		case 6:
			rrad = Radius << 2;
			break;
		case 7:
			rrad = Radius << 3;
			break;
		case 8:
			rrad = Radius << 4;
			break;
	};

	if(Radius > MAX_DESTROY_RADIUS)
		ErrH.Abort("Bad Spot Radius");

	if(terrain == 83){
		for(i = 0;i < Radius;i++){
			dx = RadiusDestroyX[i];
			dy = RadiusDestroyY[i];
			r = (Radius - i) * delta >> 8;
			for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
				if((int)RND(rrad) < Radius - i){
					rx = XCYCL((*dx) + x0);
					ry = YCYCL((*dy) + y0);
					pa0 = lt[ry] + rx;
					pf0 = pa0 + H_SIZE;
					if(SmoothTerrainMask[GET_TERRAIN(*pf0)]){
						if(*pf0 & DOUBLE_LEVEL){
							if(rx & 1) *pa0 = AltProtectedTable[(*pa0) + r + ALT_PROTECTED_OFFSET];
						}else 
							*pa0 = AltProtectedTable[(*pa0) + r + ALT_PROTECTED_OFFSET];						
					};
				};
			};
		};
	}else{
		t = terrain <<TERRAIN_OFFSET;
		for(i = 0;i < Radius;i++){
			dx = RadiusDestroyX[i];
			dy = RadiusDestroyY[i];
			r = (Radius - i) * delta >> 8;
			for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
				if((int)RND(rrad) < Radius - i){
					rx = XCYCL((*dx) + x0);
					ry = YCYCL((*dy) + y0);
					pa0 = lt[ry] + rx;
					pf0 = pa0 + H_SIZE;
					
					if(SmoothTerrainMask[GET_TERRAIN(*pf0)]){
						if(*pf0 & DOUBLE_LEVEL){
							if(rx & 1){
								*pa0 = AltProtectedTable[(*pa0) + r + ALT_PROTECTED_OFFSET];
								SET_TERRAIN(*pf0,t);
							};
						}else{
							*pa0 = AltProtectedTable[(*pa0) + r + ALT_PROTECTED_OFFSET];
							SET_TERRAIN(*pf0,t);
						};
					};
				};
			};
		};
	};
};


/*void DestroySmoothCheck(int x0,int y0, int z0, int Radius)
{
	uchar _zr = z0;
	z0 -= 50;
	uchar _z0 = (z0 > 0) ? (z0): (0);
	int i,j;
	uchar** lt = vMap -> lineT;
	short *pR = NumDestroyRadiusTable + (MAX_DESTROY_RADIUS - Radius)*(MAX_DESTROY_SIDE-1);
	int side = (2*Radius+3);
	int _side = (2*Radius+1);
	uchar *pt = new uchar[side*side];
	uchar *ptt = new uchar[side*side];
	uchar *ph = new uchar[side*side];
	uchar *_pt = pt, *_ph = ph, *_ptt;
	int down = 0;

	for(i = y0 - Radius - 1;i <= y0 + Radius+1;i++) if(!lt[YCYCL(i)]){
		delete pt;
		delete ph;
		return;
	}

	_ph = ph + side;
	_pt = pt + side - 1;
	uchar last = *(_pt + 2);
	
	for( i = y0 - Radius;i <= y0 + Radius; i++){
		uchar *llt = lt[( i) & clip_mask_y];
		int _x0 = (x0 - Radius) & clip_mask_x;
		_pt += 2;

		if ( _x0 + _side >=  map_size_x){
			int d =  map_size_x - _x0;
			memcpy( _ph, llt + _x0 + H_SIZE, d);
			memcpy( _ph + d, llt + H_SIZE,  side - d);
			_ph += side;

			for( j = _x0; j < map_size_x; j++){
				if (llt[j + H_SIZE] & DOUBLE_LEVEL ) {
					uchar _h = llt[j | 0x00000001];
					uchar _l = llt[j &(~ 0x00000001)];
					last = *_pt++ = (((_h+_l)>>1) >_zr) ? _l : _h;
				} else if (llt[j] >= _z0  )
					last = *_pt++ = llt[j];
				else
					*_pt++ = last;
			}

			for( j = 0; j < _side - d; j++){
				if (llt[j + H_SIZE] & DOUBLE_LEVEL ){ 
					uchar _h = llt[j | 0x00000001];
					uchar _l = llt[j &(~ 0x00000001)];
					last = *_pt++ = (((_h+_l)>>1) >_zr) ? _l : _h;
				} else if (llt[j] >= _z0  )
					last = *_pt++ = llt[j];
				else
					*_pt++ = last;
			}
			
		} else{
			memcpy(_ph, llt + _x0 + H_SIZE, side);
			_ph += side;
			for( j = _x0; j < _x0 + _side; j++){
				if (llt[j + H_SIZE] & DOUBLE_LEVEL ) {
					uchar _h = llt[j | 0x00000001];
					uchar _l = llt[j &(~ 0x00000001)];
					last = *_pt++ = (((_h+_l)>>1) >_zr) ? _l : _h;
				} else if (llt[j] >= _z0  )
					last = *_pt++ = llt[j];
				else
					*_pt++ = last;
			}
		}
	}//  end for

	_pt = pt + side - 1;
	_ph = ph + side - 1;
	_ptt = ptt;

	int offset = ((MAX_DESTROY_RADIUS - Radius)<<1);

	for( i = 0; i < _side; i++){
		_ph += 2;
		_pt  += 2;
		pR += offset;
		for(j = 0; j < _side; j++, _pt++, _ph++){
			int _r = *pR++;
			int l = Radius - _r;

			if(((int)(RND(Radius)) < l ) && (SmoothTerrainMask[GET_TERRAIN(*ph)])){
				unsigned short t;

				t = *(_pt-1);
				t += *(_pt+1);
				t += *(_pt - side);
				t += *(_pt + side);
				
				*_ptt++ = t >> 2;
			} else {
				*_ptt++ = *_pt;
			}//  if RND;
		}//  for j;
	}//  for i;

	
	_ptt = ptt;
	for( i = y0 - Radius;i <= y0 + Radius; i++){
		uchar *llt = lt[( i) & clip_mask_y];
		int _x0 = (x0 - Radius ) & clip_mask_x;

		if ( _x0 + _side >=  map_size_x){
			int d =  map_size_x - _x0;

			for( j = _x0; j < map_size_x; j++){
				if (llt[j + H_SIZE] & DOUBLE_LEVEL ) {
					int _h = j | 0x00000001;
					int _l = j &(~ 0x00000001);

					if (((llt[_h] + llt[_l])>>1) > _zr)
						llt[_l] = *_ptt++;
					else
						llt[_h] = *_ptt++;
				} else  if (llt[j] >= _z0)
					llt[j] = *_ptt++;
				else
					_ptt++;
			}

			for( j = 0; j < _side - d; j++){
				if (llt[j + H_SIZE] & DOUBLE_LEVEL ) {
					int _h = j | 0x00000001;
					int _l = j &(~ 0x00000001);

					if (((llt[_h] + llt[_l])>>1) > _zr)
						llt[_l] = *_ptt++;
					else
						llt[_h] = *_ptt++;

				} else  if (llt[j] >= _z0)
					llt[j] = *_ptt++;
				else
					_ptt++;
			}
		} else{
			for( j = _x0; j < _x0 + _side; j++){
				if (llt[j + H_SIZE] & DOUBLE_LEVEL ) {
					int _h = j | 0x00000001;
					int _l = j &(~ 0x00000001);

					if (((llt[_h] + llt[_l])>>1) > _zr)
						llt[_l] = *_ptt++;
					else
						llt[_h] = *_ptt++;

				} else  if (llt[j] >= _z0)
					llt[j] = *_ptt++;
				else
					_ptt++;
			}
		}
	}//  end for i

	int rx = x0 - Radius;
	int ry = y0 - Radius;

	int x1 = x0 + Radius;
	int y1 = y0 + Radius;

	regSet(rx,ry,x1,y1,1,0);
 	regRender(rx,ry,x1,y1);

	delete pt;
	delete ph;
	delete ptt;
};*/


void DestroySmoothCheck(int x0,int y0,int Radius)
{
	uchar *pf0,*pf1,*pac,*pc0;
	uchar v,a;
	int i,j;
	unsigned int t;
	uchar** lt = vMap -> lineT;
	int rx,ry;
	int x1,y1;
	int xx,yy;
	unsigned int l;

	int* dx,*dy;

	for(i = y0 - Radius;i <= y0 + Radius;i++) if(!lt[YCYCL(i)]) return;

	for(i = Radius - 1;i >= 0;i--){
		dx = RadiusDestroyX[i];
		dy = RadiusDestroyY[i];
		l = Radius - i;
		for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
			if(RND(Radius) < l ){
				int hrx = (rx = XCYCL((*dx) + x0)) + H_SIZE;
				ry = YCYCL((*dy) + y0);

				pf0 = (pc0 = lt[ry]) + H_SIZE;
				a = *(pf0 + rx);

				if(SmoothTerrainMask[GET_TERRAIN(a)]){
					if(a & DOUBLE_LEVEL){
						if(rx ^ 1){
							v = *(pac = pc0 + rx);
							
							pf1 = pf0 + (xx = XCYCL(rx + 2));
							if((*pf1) & DOUBLE_LEVEL) t = *(pc0 + xx);
							else t = v;

							pf1 = pf0 + (xx = XCYCL(rx - 2));
							if((*pf1) & DOUBLE_LEVEL) t += *(pc0 + xx);
							else t += v;

							pf1= lt[yy = YCYCL(ry - 1)] + hrx;
							if((*pf1) & DOUBLE_LEVEL) t += *(lt[yy] + rx);
							else t += v;
		
							pf1= lt[yy = YCYCL(ry + 1)] + hrx;
							if((*pf1) & DOUBLE_LEVEL) t += *(lt[yy] + rx);
							else t += v;

							*pac = t >> 2;
						}// if rx;
					}else{
						v = *(pac = pc0 + rx);
						
						pf1 = pf0 + (xx = XCYCL(rx + 1));
						if((*pf1) & DOUBLE_LEVEL) t = v;
						else t = *(pc0 + xx);
						
						pf1 = pf0 + (xx = XCYCL(rx - 1));
						if((*pf1) & DOUBLE_LEVEL) t += v;
						else t += *(pc0 + xx);

						pf1= lt[yy = YCYCL(ry - 1)] + hrx;
						if((*pf1) & DOUBLE_LEVEL) t += v;
						else t += *(lt[yy] + rx);
	
						pf1= lt[yy = YCYCL(ry + 1)] + hrx;
						if((*pf1) & DOUBLE_LEVEL) t += v;
						else t += *(lt[yy] + rx);

						*pac = t >> 2;
					}//  if DOUBLE;
				}//  if Smooth;
			}//  if RND;
		}//  for j;
	}//  for i;

	rx = XCYCL(x0 - Radius);
	ry = YCYCL(y0 - Radius);

	x1 = XCYCL(x0 + Radius);
	y1 = YCYCL(y0 + Radius);

#ifdef _ROAD_
	if(CurrentWorld != WORLD_NECROSS)
#endif
		regSet(rx,ry,x1,y1,1,0);
	regRender(rx,ry,x1,y1);
};

void DestroySmoothPut(int x0,int y0,int Radius)
{
	uchar *pf0,*pf1,*pac,*pc0;
	uchar v,a;
	int i,j;
	unsigned int t;
	uchar** lt = vMap -> lineT;
	int rx,ry;
	int xx,yy;
	unsigned int l;

	int* dx,*dy;

	if(Radius >= MAX_DESTROY_RADIUS)
		ErrH.Abort("Bad Smooth Radius");

	for(i = Radius - 1;i >= 0;i--){
		dx = RadiusDestroyX[i];
		dy = RadiusDestroyY[i];
		l = Radius - i;
		for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
			if(RND(Radius) < l){
				int hrx = (rx = XCYCL((*dx) + x0)) + H_SIZE;
				ry = YCYCL((*dy) + y0);

				pf0 = (pc0 = lt[ry]) + H_SIZE;
				a = *(pf0 + rx);

				if(SmoothTerrainMask[GET_TERRAIN(a)]){
					if(a & DOUBLE_LEVEL){
						if(rx ^ 1){
							v = *(pac = pc0 + rx);
							
							pf1 = pf0 + (xx = XCYCL(rx + 2));
							if((*pf1) & DOUBLE_LEVEL) t = *(pc0 + xx);
							else t = v;

							pf1 = pf0 + (xx = XCYCL(rx - 2));
							if((*pf1) & DOUBLE_LEVEL) t += *(pc0 + xx);
							else t += v;

							pf1= lt[yy = YCYCL(ry - 1)] + hrx;
							if((*pf1) & DOUBLE_LEVEL) t += *(lt[yy] + rx);
							else t += v;
		
							pf1= lt[yy = YCYCL(ry + 1)] + hrx;
							if((*pf1) & DOUBLE_LEVEL) t += *(lt[yy] + rx);
							else t += v;

							*pac = t >> 2;
						};
					}else{
						v = *(pac = pc0 + rx);
						
						pf1 = pf0 + (xx = XCYCL(rx + 1));
						if((*pf1) & DOUBLE_LEVEL) t = v;
						else t = *(pc0 + xx);
						
						pf1 = pf0 + (xx = XCYCL(rx - 1));
						if((*pf1) & DOUBLE_LEVEL) t += v;
						else t += *(pc0 + xx);

						pf1= lt[yy = YCYCL(ry - 1)] + hrx;
						if((*pf1) & DOUBLE_LEVEL) t += v;
						else t += *(lt[yy] + rx);
	
						pf1= lt[yy = YCYCL(ry + 1)] + hrx;
						if((*pf1) & DOUBLE_LEVEL) t += v;
						else t += *(lt[yy] + rx);

						*pac = t >> 2;
					};
				};
			};
		};
	};
};

void xRestoreDestroySpot(int x0,int y0,int Radius,uchar terrain,int delta,uchar rfactor)
{
	uchar* pa0,*pf0;
	int i,j;
	uchar** lt = vMap -> lineT;
	int rx,ry;
	int r;
	int* dx,*dy;
	int rrad;

	int phase,sphase; 
	if(!Radius) return;

	for(i = y0 - Radius;i <= y0 + Radius;i++) if(!lt[YCYCL(i)]) return;

	switch(rfactor){
		case 0:
			rrad = 0;
			break;
		case 1:
			rrad = Radius >> 3;
			break;
		case 2:
			rrad = Radius >> 2;
			break;
		case 3:
			rrad = Radius >> 1;
			break;
		case 4:
			rrad = Radius;
			break;
		case 5:
			rrad = Radius << 1;
			break;
		case 6:
			rrad = Radius << 2;
			break;
		case 7:
			rrad = Radius << 3;
			break;
		case 8:
			rrad = Radius << 4;
			break;
	};

	phase = PI / 2;
	sphase = PI / (2 * Radius);

	for(i = 0;i < Radius;i++){
		dx = RadiusDestroyX[i];
		dy = RadiusDestroyY[i];
		r = SI[rPI(phase)]*delta >> 16;
		for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
			if((int)RND(rrad) < Radius - i){
				rx = XCYCL((*dx) + x0);
				ry = YCYCL((*dy) + y0);
				pa0 = lt[ry] + rx;
				pf0 = pa0 + H_SIZE;
				if(SmoothTerrainMask[GET_TERRAIN(*pf0)]){
					if(*pf0 & DOUBLE_LEVEL){
						if(rx & 1) 
//							*pa0 -= r;
							*pa0 = AltProtectedTable[(*pa0) - r + ALT_PROTECTED_OFFSET];
					}else 
//						*pa0 -= r;
						*pa0 = AltProtectedTable[(*pa0) - r + ALT_PROTECTED_OFFSET];
				};
			};
		};
		phase -= sphase;
	};
};


void xDestroySpot(int x0,int y0,int Radius,uchar terrain,int delta,uchar rfactor)
{
	uchar* pa0,*pf0;
	int i,j;
	uchar** lt = vMap -> lineT;
	int rx,ry;
	uchar t;
	int r;
	int* dx,*dy;
	int rrad;

	int phase,sphase;
	if(!Radius) return;

	for(i = y0 - Radius;i <= y0 + Radius;i++) if(!lt[YCYCL(i)]) return;

	switch(rfactor){
		case 0:
			rrad = 0;
			break;
		case 1:
			rrad = Radius >> 3;
			break;
		case 2:
			rrad = Radius >> 2;
			break;
		case 3:
			rrad = Radius >> 1;
			break;
		case 4:
			rrad = Radius;
			break;
		case 5:
			rrad = Radius << 1;
			break;
		case 6:
			rrad = Radius << 2;
			break;
		case 7:
			rrad = Radius << 3;
			break;
		case 8:
			rrad = Radius << 4;
			break;
	};

	phase = PI / 2;
	sphase = PI / (2 * Radius);

	if(terrain == 83){
		for(i = 0;i < Radius;i++){
			dx = RadiusDestroyX[i];
			dy = RadiusDestroyY[i];
			r = SI[rPI(phase)]*delta >> 16;
			for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
				if((int)RND(rrad) < Radius - i){
					rx = XCYCL((*dx) + x0);
					ry = YCYCL((*dy) + y0);
					pa0 = lt[ry] + rx;
					pf0 = pa0 + H_SIZE;
					if(SmoothTerrainMask[GET_TERRAIN(*pf0)]){
						if(*pf0 & DOUBLE_LEVEL){
							if(rx & 1) 
//								*pa0 += r;							
								*pa0 = AltProtectedTable[(*pa0) + r + ALT_PROTECTED_OFFSET];
						}else 
//							*pa0 += r;
							*pa0 = AltProtectedTable[(*pa0) + r + ALT_PROTECTED_OFFSET];
					};
				};
			};
			phase -= sphase;
		};
	}else{
		t = terrain <<TERRAIN_OFFSET;
		for(i = 0;i < Radius;i++){
			dx = RadiusDestroyX[i];
			dy = RadiusDestroyY[i];
			r = SI[rPI(phase)]*delta >> 16;
			for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
				if((int)RND(rrad) < Radius - i){
					rx = XCYCL((*dx) + x0);
					ry = YCYCL((*dy) + y0);
					pa0 = lt[ry] + rx;
					pf0 = pa0 + H_SIZE;
					
					if(SmoothTerrainMask[GET_TERRAIN(*pf0)]){
						if(*pf0 & DOUBLE_LEVEL){
							if(rx & 1){
//								*pa0 += r;
								*pa0 = AltProtectedTable[*pa0 + r + ALT_PROTECTED_OFFSET];
								SET_TERRAIN(*pf0,t);
							};
						}else{
//							*pa0 += r;
							*pa0 = AltProtectedTable[*pa0 + r + ALT_PROTECTED_OFFSET];
							SET_TERRAIN(*pf0,t);
						};
					};
				};
			};
			phase -= sphase;
		};
	};
};

void RadialRender(int x0,int y0,int radius)
{
	int i;
	int x1,y1;
	int rx,ry;

	uchar** lt = vMap -> lineT;

	for(i = y0 - radius;i <= y0 + radius;i++) if(!lt[YCYCL(i)]) return;

	rx = XCYCL(x0 - radius);
	ry = YCYCL(y0 - radius);

	x1 = XCYCL(x0 + radius);
	y1 = YCYCL(y0 + radius);

#ifdef _ROAD_
	if(CurrentWorld != WORLD_NECROSS)
#endif
		regSet(rx,ry,x1,y1,1,0);
	regRender(rx,ry,x1,y1);
};


/*int LandSlide(int x0,int y0,int Radius,int delta)
{
	uchar *pf0,*pf1,*pac,*pc0,*plc;
	uchar v;
	int t,i,j,t1;
	uchar** lt = vMap -> lineT;
	int rx,ry;
	int x1,y1;
	int ax,bx;
	int l,xx,yy,l2;
	int* dx,*dy;

	for(i = y0 - Radius;i <= y0 + Radius;i++) if(!lt[YCYCL(i)]) return 0;

	int count = 0;

	for(i = 0;i < Radius;i++){
		dx = RadiusDestroyX[i];
		dy = RadiusDestroyY[i];
		l = Radius - i;
		for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
			if((int)RND(Radius) < l){
				rx = XCYCL((*dx) + x0);
				ry = YCYCL((*dy) + y0);

				pf0 = (pc0 = lt[ry]) + H_SIZE;
//				a = *(pf0 + rx);
				v = *(pac = pc0 + rx);
//				t1 = v;

				if(v > delta){
					pf1 = pf0 + (xx = XCYCL(rx + 1));
					t1 = t = v - *(pc0 + xx);
					
					pf1 = pf0 + (xx = XCYCL(rx - 1));
					t = v - *(pc0 + xx);
					if(t < t1) t1 = t;

					pf1= lt[yy = YCYCL(ry - 1)] + rx + H_SIZE;
					t = v - *(lt[yy] + rx);
					if(t < t1) t1 = t;

					pf1= lt[yy = YCYCL(ry + 1)] + rx + H_SIZE;
					t = v - *(lt[yy] + rx);
					if(t < t1) t1 = t;

//					if(t1 > v) *pac = delta;
//					else  if(t1 < v) *pac = AltProtectedTable[ALT_PROTECTED_OFFSET + v + 8*(t1 - v)/9];

					if(t1 <= -1) *pac = delta;

//					else *pac = AltProtectedTable[v - (t >> 2) + ALT_PROTECTED_OFFSET];
//					*pac = t >> 2;
				};
			};
		};
	};

	for(i = 0;i < Radius;i++){
		dx = RadiusDestroyX[i];
		dy = RadiusDestroyY[i];
		l = Radius - i;
		for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
			if((int)RND(Radius) < l){
				rx = XCYCL((*dx) + x0);
				ry = YCYCL((*dy) + y0);

				pf0 = (pc0 = lt[ry]) + H_SIZE;
				if(((*(pf0 + rx)) & DOUBLE_LEVEL) && (rx & 1)){
//					a = *(pf0 + rx);
					v = *(pac = pc0 + rx);
					if(v < delta){
						*pac = *(pc0 + XCYCL(rx - 1));
						plc = pf0 + XCYCL(rx - 1);
						*plc &= ~DOUBLE_LEVEL;
						*(pf0 + XCYCL(rx - 1)) = *(plc);
						count++;
					};

					t1 = 0;

					pf1 = pf0 + (xx = XCYCL(rx + 2));
					if((*pf1) & DOUBLE_LEVEL){
						t = v - *(pc0 + xx);
						if(t < t1) t1 = t;
					};
					
					pf1 = pf0 + (xx = XCYCL(rx - 2));
					if((*pf1) & DOUBLE_LEVEL){
						t = v - *(pc0 + xx);
						if(t < t1) t1 = t;
					}

					pf1= lt[yy = YCYCL(ry - 1)] + rx + H_SIZE;
					if((*pf1) & DOUBLE_LEVEL){
						t = v - *(lt[yy] + rx);
						if(t < t1) t1 = t;
					}

					pf1= lt[yy = YCYCL(ry + 1)] + rx + H_SIZE;
					if((*pf1) & DOUBLE_LEVEL){						
						t = v - *(lt[yy] + rx);
						if(t < t1) t1 = t;
					};

					if(t1 < -2){
						*pac = *(pc0 + XCYCL(rx - 1));
						plc = pf0 + XCYCL(rx - 1);
						*plc &= ~DOUBLE_LEVEL;
						*(pf0 + XCYCL(rx - 1)) = *(plc);
					};
				};

			};
		};
	};

	if(!count) return 0;

	rx = XCYCL(x0 - Radius);
	ry = YCYCL(y0 - Radius);

	x1 = XCYCL(x0 + Radius);
	y1 = YCYCL(y0 + Radius);

	if(rx < x1){
		ax = x1 - rx;
		bx = TOR_XSIZE - ax;
		if(ax > bx){
			regSet(x1,ry,rx,y1,1,0);
			regRender(x1,ry,rx,y1);
		}else{
			regSet(rx,ry,x1,y1,1,0);
 			regRender(rx,ry,x1,y1);
		};
	}else{
		ax = rx - x1;
		bx = TOR_XSIZE - ax;
		if(ax > bx){
			regSet(rx,ry,x1,y1,1,0);
			regRender(rx,ry,x1,y1);
		}else{
			regSet(x1,ry,rx,y1,1,0);
			regRender(x1,ry,rx,y1);
		};
	};
	return count;
};
*/

/*int LandSlide(int x0,int y0,int Radius,int z0)
{
	uchar *pf0,*pac,*pc0,*plc;
	int t,i,j;
	uchar** lt = vMap -> lineT;
	int rx,ry;
	int x1,y1;
	int ax,bx;
	int* dx,*dy;
	int z,kz;

	for(i = y0 - Radius;i <= y0 + Radius;i++) if(!lt[YCYCL(i)]) return 0;

	int count = 0;
	kz = -(z0 << 5) / Radius;
	z = z0 << 8;
	for(i = 0;i < Radius;i++){
		dx = RadiusDestroyX[i];
		dy = RadiusDestroyY[i];
		for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
			rx = XCYCL((*dx) + x0);
			ry = YCYCL((*dy) + y0);
			pf0 = (pc0 = lt[ry]) + H_SIZE;
			if(((*(pf0 + rx)) & DOUBLE_LEVEL) && (rx & 1)){
				t = *(pac = pc0 + rx);
				if(t < z0){
					*pac = *(pc0 + XCYCL(rx - 1));
					plc = pf0 + XCYCL(rx - 1);
					*plc &= ~DOUBLE_LEVEL;
					*(pf0 + XCYCL(rx - 1)) = *(plc);
					count++;
				};
			};
		};
		z += kz;
	};

	if(!count) return 0;

	rx = XCYCL(x0 - Radius);
	ry = YCYCL(y0 - Radius);

	x1 = XCYCL(x0 + Radius);
	y1 = YCYCL(y0 + Radius);

	if(rx < x1){
		ax = x1 - rx;
		bx = TOR_XSIZE - ax;
		if(ax > bx){
			regSet(x1,ry,rx,y1,1,0);
			regRender(x1,ry,rx,y1);
		}else{
			regSet(rx,ry,x1,y1,1,0);
 			regRender(rx,ry,x1,y1);
		};
	}else{
		ax = rx - x1;
		bx = TOR_XSIZE - ax;
		if(ax > bx){
			regSet(rx,ry,x1,y1,1,0);
			regRender(rx,ry,x1,y1);
		}else{
			regSet(x1,ry,rx,y1,1,0);
			regRender(x1,ry,rx,y1);
		};
	};
	return count;
};

int LandSlideMove(int x0,int y0,int Radius,int z0,int delta)
{
	uchar *pf0,*pac,*pc0,*plc;
	int t0,i,j,t1,t2;
	uchar** lt = vMap -> lineT;
	int rx,ry;
	int x1,y1;
	int ax,bx;
	int* dx,*dy;
	int xx;
//	int z,kz;

	for(i = y0 - Radius;i <= y0 + Radius;i++) if(!lt[YCYCL(i)]) return 0;

	int count = 0;

//	kz = -(z0 << 2) / Radius;
//	z = z0 << 8;

	for(i = 0;i < Radius;i++){
		dx = RadiusDestroyX[i];
		dy = RadiusDestroyY[i];
		for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
			rx = XCYCL((*dx) + x0);
			ry = YCYCL((*dy) + y0);
			pf0 = (pc0 = lt[ry]) + H_SIZE;
			if(((*(pf0 + rx)) & DOUBLE_LEVEL) && (rx & 1)){
				t0 = *(pac = pc0 + rx);
				if(t0 < z0){
					xx = rx - 1;					
					t2 = *(pc0 + xx);
					t1 = (((GET_DELTA(*(pf0 + xx)) << 2) + GET_DELTA(*(pf0 + rx)) + 1) << DELTA_SHIFT);
					if(t0 > t2 + t1 + delta){
//						t1 -= delta;
						(*pac) -= delta;
//						SET_DELTA(*(pf0 + xx),(t1 & 12) >> 2);
//						SET_DELTA(*(pf0 + rx),(t1 & 3));
						*(pf0 + xx) &= ~DELTA_MASK;
						*(pf0 + rx) &= ~DELTA_MASK;
						count++;
					}else{
						*(pc0 + xx) = t0;
						*(pf0 + rx) &= ~(DELTA_MASK | DOUBLE_LEVEL);
						*(pf0 + xx) = *(pf0 + rx);
//						*(pf0 + xx) &= ~(DELTA_MASK | DOUBLE_LEVEL);
						count++;
					};
				};
			};
	 	};
//		z += kz;
	};

	if(!count) return 0;

	rx = XCYCL(x0 - Radius);
	ry = YCYCL(y0 - Radius);

	x1 = XCYCL(x0 + Radius);
	y1 = YCYCL(y0 + Radius);

	if(rx < x1){
		ax = x1 - rx;
		bx = TOR_XSIZE - ax;
		if(ax > bx){
			regSet(x1,ry,rx,y1,1,0);
			regRender(x1,ry,rx,y1);
		}else{
			regSet(rx,ry,x1,y1,1,0);
 			regRender(rx,ry,x1,y1);
		};
	}else{
		ax = rx - x1;
		bx = TOR_XSIZE - ax;
		if(ax > bx){
			regSet(rx,ry,x1,y1,1,0);
			regRender(rx,ry,x1,y1);
		}else{
			regSet(x1,ry,rx,y1,1,0);
			regRender(x1,ry,rx,y1);
		};
	};
	return count;
};*/

int LandSlideProcess(int* fx,int* fy,int pow,int z0,int delta)
{
	int xl,xr;
	int kl,kr;
	int al,bl,ar,br;
	int d;
	int where;
	int Y;
	int i;
	int fv,rv,lv;
	int n,n_clip;
	int cnt;
	
	cnt = 0;

	n = 1 << pow;
	n_clip = n - 1;
	Y = fy[0];
	fv = 0;
	for(i = 1;i < n;i++){
		if(fy[i] < Y){
			Y = fy[i];
			fv = i;
		};
	};

	rv = (fv - 1) & n_clip;
	lv = (fv + 1) & n_clip;

	xl = fx[fv];
	al = fx[lv] - xl;
	bl = fy[lv] - Y;
	ar = fx[rv] - xl;
	br = fy[rv] - Y;
	xl = (xl << 16) + (1 << 15);
	xr = xl;

	if(bl == 0) kl = al;
	else kl = (al << 16) / bl;
	if(br == 0) kr = ar;
	else kr = (ar << 16) / br;

	if(bl < 0) bl = -bl;
	if(br < 0) br = -br;


	while(1){
		if(bl > br){
			d = br;
			where = 0;
		}else{
			d = bl;
			where = 1;
		};

		while(d--){
//			LandSlideLine(int x0,int y0,int z0,int l,int delta);
			cnt += LandSlideLine(xl >> 16,Y,z0,xr >> 16,delta);
			xl += kl;
			xr += kr;
			Y++;
		};

		if(where){
			if(lv == rv) return cnt;
			fv = lv;

			lv = (fv + 1) & n_clip;

			br -= bl;
			xl = fx[fv];
			al = fx[lv] - xl;
			bl = fy[lv] - Y;
			xl = (xl << 16) + (1 << 15);

			if(bl == 0) kl = al;
			else kl = (al << 16) / bl;
			if(al < 0) al = -al;
		}else{
			if(rv == lv) return cnt;
			fv = rv;

			rv = (rv - 1) & n_clip;

			bl -= br;
			xr = fx[fv];
			ar = fx[rv] - xr;
			br = fy[rv] - Y;
			xr = (xr << 16) + (1 << 15);

			if(br == 0) kr = ar;
			else kr = (ar << 16) / br;
			if(ar < 0) ar = -ar;
		};
	};
};

int LandSlideLine(int x0,int y0,int z0,int x1,int delta)
{
	uchar *pf,*pf0,*pc0,*pc;
	int f;
	int i,l;
	uchar* lt;
	uchar a0,a1,f0,f1;
	int r1,r2;
	int cnt;

	y0 = YCYCL(y0);
	lt = vMap -> lineT[y0];
	if(!lt) return 0;

	cnt = 0;
	if(x0 & 1) x0--;
	x0 = XCYCL(x0);
	if(!(x1 & 1)) x1++;
	x1 = XCYCL(x1);

	r1 = x1 - x0;
	r2 = TOR_XSIZE - r1;
	if(r1 > r2) l = r2;
	else l = r1;

	pc0 = lt + x0;
	pf0 = pc0 + H_SIZE;

	f = x0;
	for(i = 0;i < l;i++){
		if(f & 1){
			f1 = *pf0;
			if(f1 & DOUBLE_LEVEL){
				a1 = *pc0;
				if(a1 < z0){
					*pc0 = (a0 + RND(delta));
					*pc = (a0 + RND(delta));
					f1 &= ~(DELTA_MASK | DOUBLE_LEVEL);
					*pf = *pf0 = f1;

					if(RND(5000) < 5)
						EffD.CreateFireBall(Vector(XCYCL(x0 + i),y0,z0 + 20),DT_FIRE_BALL05,NULL,1 << 8,0);

					cnt++;
				};
			};
		}else{
			f0 = *pf0;
			if(f0 & DOUBLE_LEVEL){
				a0 = *pc0;
				pf = pf0;
				pc = pc0;
			};
		};

		if(f >= clip_mask_x){
			f = 0;
			pc0 = lt;
			pf0 = pc0 + H_SIZE;
		}else{
			f++;
			pf0++;
			pc0++;
		};
	};

	int nx,ny;

		nx = XCYCL(x0 + 10 - RND(20));
		ny = YCYCL(y0 + 10 - RND(20));
		lt = vMap -> lineT[ny];
		if(!lt) return cnt;
		pc0 = lt + nx;
		pf0 = pc0 + H_SIZE;
		if((*pf0) & DOUBLE_LEVEL){
			if(nx & 1) nx = XCYCL(nx + 1);
			a0 = *pc0;
			if(a0 > delta && a0 < 255 - delta)
				*pc0 = a0 + (delta >> 1) - RND(delta);
		};

		nx = XCYCL(x1 + 10 - RND(20));
		ny = YCYCL(y0 + 10 - RND(20));
		lt = vMap -> lineT[ny];
		if(!lt) return cnt;
		pc0 = lt + nx;
		pf0 = pc0 + H_SIZE;
		if((*pf0) & DOUBLE_LEVEL){
			if(nx & 1) nx = XCYCL(nx + 1);
			a0 = *pc0;
			if(a0 > delta && a0 < 255 - delta)
				*pc0 = a0 + (delta >> 1) - RND(delta);
		};

	return cnt;
};

void MapCircleProcess(int x0,int y0,int z0,int r0,int mode,uchar terrain)
{
	uchar *pc,*pf0,*pf,*ph;
	int j;
	uchar** lt = vMap -> lineT;
	int rx,ry;													     
	int* dx,*dy;
	uchar delta0,delta1;
	uchar t1;
	int z;

	x0 |= 1;
	for(j = y0 - r0;j <= y0 + r0;j++) if(!lt[YCYCL(j)]) return;
	if(r0 < 0 || r0 > MAX_DESTROY_RADIUS) return;
	delta0 = 32 & 3;
	delta1 = (32 & 12) >> 2;
	if(mode){
		dx = RadiusDestroyX[r0];
		dy = RadiusDestroyY[r0];
		for(j = 0;j < NumDestroyRadius[r0];j++,dx++,dy++){
			rx = XCYCL((*dx) + x0);
			ry = YCYCL((*dy) + y0);
			pf0 =  lt[ry] + H_SIZE;
			pf = pf0 + rx;
			if(((*pf) & DOUBLE_LEVEL) && !(rx & 1)){
				*pf &= ~(DELTA_MASK | DOUBLE_LEVEL);
				*(pf + 1) = *pf;
				ph = pf - H_SIZE;
				if((z = (int)*ph + *(ph + 1) - z0) >= 0){
					*(ph + 1) = z;
					SET_DELTA(*pf,1);
					}
				else
					SET_DELTA(*pf,2);
			};
		};
	}else{
		t1 = terrain << TERRAIN_OFFSET;
		dx = RadiusDestroyX[r0];
		dy = RadiusDestroyY[r0];
		for(j = 0;j < NumDestroyRadius[r0];j++,dx++,dy++){
			rx = XCYCL((*dx) + x0);
			ry = YCYCL((*dy) + y0);
			pf0 = lt[ry] + H_SIZE;
			pf = pf0 + rx;
			if(!(*pf & DOUBLE_LEVEL) && !(rx & 1) && *pf & DELTA_MASK){
				pc = pf + 1;
				*pf |= DOUBLE_LEVEL;
				SET_DELTA(*pf,delta1);
				SET_DELTA(*pc,delta0);
				SET_TERRAIN(*pc,t1);
				*pc |= DOUBLE_LEVEL;
				pc -= H_SIZE;
				*pc = z0 + *pc - *(pf - H_SIZE);
			};
		};
	};	
};

int GetLandAlt(int x0,int y0,int Radius)	       
{
/*	uchar *pc0,*pf0;
	int t,i,j,t1;
	uchar** lt = vMap -> lineT;
	int rx,ry;
	int* dx,*dy;

	for(i = y0 - Radius;i <= y0 + Radius;i++) if(!lt[YCYCL(i)]) return -1;
	t1 = 255;
	for(i = 0;i < Radius;i++){
		if(RND(Radius) < i){
			dx = RadiusDestroyX[i];
			dy = RadiusDestroyY[i];
			for(j = 0;j < NumDestroyRadius[i];j++,dx++,dy++){
				rx = XCYCL((*dx) + x0);
				ry = YCYCL((*dy) + y0);
				pf0 = (pc0 = lt[ry]) + H_SIZE;
				if(((*(pf0 + rx)) & DOUBLE_LEVEL) && (rx & 1)){
					t = *(pc0 + rx);
					if(t < t1) t1 = t;
				};
			};
		};
	};
	return t1;*/

	uchar *pf0,*pf;
	uchar** lt;
	lt = vMap -> lineT;
	int t,i,j,t1;

	t1 = 255;
	for(i = y0 - Radius;i <= y0 + Radius;i++) if(!lt[YCYCL(i)]) return -1;

	if(!(x0 & 1)) x0 = XCYCL(x0 + 1);
	for(i = -Radius;i < Radius;i++){
		pf0 = lt[YCYCL(y0 + i)] + H_SIZE;
		for(j = -Radius;j < Radius;j += 2){
			pf = pf0 + XCYCL(x0 + j);
			if((*pf) & DOUBLE_LEVEL){
				t = *(pf - H_SIZE);
				if(t < t1) t1 = t;
			};
		};
	};
	return t1;
};


void MakeSecondLevel(int x0,int y0,uchar z0,int Radius)	       
{
	uchar *pf0,*pf1,*pf;
	uchar** lt;
	lt = vMap -> lineT;
	int t,i,j;
	int d = 32;

	for(i = y0 - Radius;i <= y0 + Radius;i++) if(!lt[YCYCL(i)]) return;

	for(i = -Radius;i < Radius;i++){
		pf1 = NULL;
		pf0 = lt[YCYCL(y0 + i)] + H_SIZE;
		for(j = -Radius;j < Radius;j++){			
			t = XCYCL(x0 + j);
			pf = pf0 + t;
			if(t & 1){
				if(/*GET_TERRAIN(*pf) == 1 || */pf1){
					*(pf) |= (DOUBLE_LEVEL);
					SET_DELTA(*pf,(d & 3));
				};
			}else{
//				if(GET_TERRAIN(*pf) == 1){
					*(pf - H_SIZE) = z0;
					*(pf) |= (DOUBLE_LEVEL);
					SET_DELTA(*pf,(d & 12) >> 2);
					pf1 = pf;
//				}else pf1 = NULL;
			};
		};
	};
	RadialRender(x0,y0,Radius);
};


void LandSlideType::CreateLandSlide(int* _xx,int* _yy,int _Time)
{
	int i,a;
	Vector vCheck;
	x = 0;
	y = 0;
	for(i = 0;i < 4;i++){
		cX[i] = _xx[i];
		cY[i] = _yy[i];
		x += cX[i];
		y += cY[i];
	};
	x /= 4;
	y /= 4;
	cycleTor(x,y);

	SOUND_CRASH();

	Radius = 0;
	for(i = 0;i < 4;i++){
		vCheck = Vector(getDistX(x,cX[i]),getDistY(y,cY[i]),0);
		a = vCheck.vabs();
		if(a > Radius) Radius = a;
	};	
	z = GetLandAlt(x,y,Radius);
	//Time = _Time;
	Time = 80 * GAME_TIME_COEFF;
	Status = 0;
	ID = ID_MOBILE_LOCATION;	
	sZ = z << 16;
	dZ = ((255 << 16) - sZ) / Time;
};

static char Mask_for_crash[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,0,1,1,0,0,0,0,
	0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,0,1,0,0,0,0,
	0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,0,0,0,
	0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
	0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
	0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,0,

	0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,
	0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
	0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,0,
	0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,0,
	0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
	0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
	0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
	0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,0,0,0,0,
	0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

};

void LandSlideType::makeLittelNoise(void){
	int firtst_time = 68 * GAME_TIME_COEFF;
	int end_time = 55 * GAME_TIME_COEFF;
	//int for_one = 12;
	int for_one = 30;
	int size = 6;
	int i, j, s;
	int xc, yc;
	int rnd_quant = 4;
	uchar** lt = vMap -> lineT;
	int dast;

	if (Time < firtst_time && Time > end_time) {
		size = 30;
		rnd_quant = 8;
	} else if (Time == end_time){
		cX[0] += ((cX[2] - cX[0])>>4);
		cX[2] += ((cX[0] - cX[2])>>4);

		cX[1] += ((cX[3] - cX[1])>>4);
		cX[3] += ((cX[1] - cX[3])>>4);

		cY[0] += ((cY[2] - cY[0])>>4);
		cY[2] += ((cY[0] - cY[2])>>4);

		cY[1] += ((cY[3] - cY[1])>>4);
		cY[3] += ((cY[1] - cY[3])>>4);
													 
		LandSlideProcess(cX,cY,2,250,10);
		RadialRender(x,y,Radius);
		Time = 0;
		return;
	} else if (Time < end_time)return;

	for( i = 0; i < for_one; i++){ 
		int rnd1 = RND(259), rnd2 = RND(259);
		char* mask = Mask_for_crash;
		int k = RND(4);
		
		if (Time > firtst_time) {
			xc = cX[k] + (( ((cX[(k+1)&3] - cX[k])*rnd1) + ((cX[(k-1)&3] - cX[k])*rnd2))>>8);
			yc = cY[k] + (( ((cY[(k+1)&3] - cY[k])*rnd1) + ((cY[(k-1)&3] - cY[k])*rnd2))>>8);
		} else {
			xc = cX[k] + (( ( ((cX[(k+1)&3] - cX[k])*rnd1) + ((cX[(k-1)&3] - cX[k])*rnd2))>>11)<<3);
			yc = cY[k] + (( ( ((cY[(k+1)&3] - cY[k])*rnd1) + ((cY[(k-1)&3] - cY[k])*rnd2))>>11)<<3);
		}

		if ((lt[yc] ) && (lt[yc+size-1])){

			dast = 0;
			for( j = 0; j < size; j++){
				uchar *llt = lt[yc+j];
				for( s = 0; s < size; s++){
					int _x = (xc+s)&clip_mask_x;
					if((llt[_x + H_SIZE] & DOUBLE_LEVEL) && (_x & 1) && ((RND(10) > rnd_quant ) || *mask) ){
						llt[_x + H_SIZE] &= ~(DELTA_MASK | DOUBLE_LEVEL);
						llt[_x - 1 + H_SIZE] &= ~(DELTA_MASK | DOUBLE_LEVEL);
						llt[_x ] = llt[_x-1] + RND(2);
						llt[_x-1] +=  RND(2);
						dast++;
					}//  end if
					mask++;
				}//  end for s
			}//  end for j

			if ( dast ){
				if (Time < firtst_time && Time > end_time && dast > (size<<2) && RND(2))
						EffD.CreateFireBall(Vector(XCYCL(xc + (size>>1)),yc + (size>>1), 200),DT_FIRE_BALL05,NULL,3 << 6,0);//4 <<1
				else if ( Time < (77 * GAME_TIME_COEFF) && Time > firtst_time && !RND(8))
						EffD.CreateFireBall(Vector(XCYCL(xc + (size>>1)),yc + (size>>1), 200),DT_FIRE_BALL05,NULL, 3 << 5,0);
				//else
				dastPutSpriteOnMapAlt( XCYCL(xc + (size>>1)), yc + (size>>1), dastResource->data[RND(dastResource->n)], dastResource->x_size, dastResource->y_size, 1<<14);
				regSet(xc,yc,xc + size -1, yc + size - 1,1,0);
				regRender(xc,yc,xc + size - 1,yc + size - 1 );				
			}
		}//  end if
	}//  end for i
}

void LandSlideType::Quant(void)
{
	makeLittelNoise();
	/*if(LandSlideProcess(cX,cY,2,sZ >> 16,10))
		RadialRender(x,y,Radius);*/
	sZ += dZ;
	if(Time-- <= 0) Status |= SOBJ_DISCONNECT;
};

// Necross road animated hole
void MapLandHole::CreateLandHole(Vector v,int rMax,int l1,int l2,int l3)
{
	maxRadius = rMax * GAME_TIME_COEFF;
	Time = 0;
	LifeTime = l1;
	R_curr = v;
	Status = 0;
	ID = ID_MOBILE_LOCATION;	
	cRadius = 0;
	Mode = 0;
};

// Necross road animated hole animation frame
void MapLandHole::Quant(void)
{
	if(!vMap->lineT[R_curr.y]) Status |= SOBJ_DISCONNECT;
	switch(Mode){
		case 0:
			MapCircleProcess(R_curr.x,R_curr.y,R_curr.z,(int)round(cRadius / GAME_TIME_COEFF),1,2);
			RadialRender(R_curr.x,R_curr.y,(int)round(cRadius / GAME_TIME_COEFF) + 1);
			if(cRadius >= maxRadius) Mode = 1;
			else cRadius++;
			if(cRadius == 0 && ActD.Active)
				SOUND_TEAR(getDistX(ActD.Active->R_curr.x,R_curr.x))
			break;
		case 1:
			if(Time > LifeTime) Mode = 2;
			else Time++;
			break;
		case 2:
			MapCircleProcess(R_curr.x,R_curr.y,R_curr.z,(int)round(cRadius / GAME_TIME_COEFF),0,2);
			if(cRadius == maxRadius && ActD.Active)
				SOUND_TEAR(getDistX(ActD.Active->R_curr.x,R_curr.x))
			RadialRender(R_curr.x,R_curr.y,(int)round(cRadius / GAME_TIME_COEFF) + 1);
			if(cRadius <= 0) Status |= SOBJ_DISCONNECT;
			else cRadius--;
			break;
	};
};

void MapAcidSpot::CreateAcid(Vector v,int fRad,int lRad,int fDelta,int lDelta,int lTime)
{
	R_curr = v;
	ID = ID_MOBILE_LOCATION;
	Status = 0;
	cycleTor(R_curr.x,R_curr.y);

	Time = lTime;
	Radius = fRad << 8;
	dRadius = ((lRad << 8) - Radius) / lTime;	
	Delta = fDelta << 8;
	dDelta = ((lDelta << 8) - Delta) / lTime;	
};

void MapAcidSpot::Quant(void)
{
//	int i;
//	for(i = 0;i < TERRAIN_MAX;i++) SmoothTerrainMask[i] = 0;
	memset(SmoothTerrainMask,0,TERRAIN_MAX);
	SmoothTerrainMask[4] = 1;

	if(Time >= 0){
		DestroySpot(R_curr.x,R_curr.y,Radius >> 8,83,Delta >> 8,4);
		DestroySmoothCheck(R_curr.x,R_curr.y,Radius >> 8);
		Radius += dRadius;
		Delta += dDelta;
		Time--;
	}else Status |= SOBJ_DISCONNECT;
};

int MobileLocation::QuickCheck(void)
{ 
	uchar** lt = vMap -> lineT;
	int j,y;
	for(j = 0,y = YCYCL(y0 + dy);j < altSy;j++,y = YCYCL(y + 1))
		if(!lt[y]) return 0;
	return 1;
};

int MobileLocation::NetQuickCheck(int x83,int y83,int r83)
{ 
	uchar** lt = vMap -> lineT;
	int j,y;

	if(abs(getDistX(x83,ViewX)) - r83 < TurnSideX && abs(getDistY(y83,ViewY)) - r83 < TurnSideY && lt[YCYCL(y83 - r83)] && lt[YCYCL(y83 + r83)]){
		for(j = 0,y = YCYCL(y0 + dy);j < altSy;j++,y = YCYCL(y + 1))
			if(!lt[y]) return 0;
		return 1;
	};
	return 0;
};


void LandSlideType::makeFastlNoise(void){
	int size = 30;
	int i, j, s, l;
	int xc, yc;
	int rnd_quant = 4;
	uchar** lt = vMap -> lineT;

	for( i = 0; i < 4; i++){ 
		char* mask = Mask_for_crash;
		
		for( l = 0; l < 16; l++){
			if (!RND(3)){
				xc = (l*cX[i] + (16-l)*cX[(i+1)&3])>>4;
				yc = (l*cY[i] + (16-l)*cY[(i+1)&3])>>4;
				if ((lt[yc] ) && (lt[yc+size-1])){

					for( j = 0; j < size; j++){
						uchar *llt = lt[yc+j];
						for( s = 0; s < size; s++){
							int _x = (xc+s)&clip_mask_x;
							if((llt[_x + H_SIZE] & DOUBLE_LEVEL) && (_x & 1) && ((RND(10) > rnd_quant ) || *mask) ){
								llt[_x + H_SIZE] &= ~(DELTA_MASK | DOUBLE_LEVEL);
								llt[_x - 1 + H_SIZE] &= ~(DELTA_MASK | DOUBLE_LEVEL);
								llt[_x ] = llt[_x-1] + RND(2);
								llt[_x-1] +=  RND(2);
							}//  end if
							mask++;
						}//  end for s
					}//  end for j
				}//  end if
			}//end if
		}//  end for l
	}//  end for i

/*		
	dastPutSpriteOnMapAlt( XCYCL(xc + (size>>1)), yc + (size>>1), dastResource->data[RND(dastResource->n)], dastResource->x_size, dastResource->y_size, 1<<14);
   */
	LandSlideProcess(cX,cY,2,250,10);
	RadialRender(x,y,Radius);
}
#endif
