#include "../global.h"
#include "../lang.h"

#include "../zmod_client.h"

//#include "..\win32f.h"
#include "../runtime.h"

#include "../3d/3d_math.h"
#include "../3d/3dgraph.h"
#include "../3d/3dobject.h"
#include "../3d/parser.h"

#include "../common.h"
#include "../sqexp.h"
#include "../backg.h"

#include "../terra/vmap.h"
#include "../terra/world.h"
#include "../terra/render.h"

#include "../iscreen/controls.h"
#include "../iscreen/iscreen_options.h"

#include "../actint/item_api.h"
#include "uvsapi.h"
#include "../network.h"

#include "../uvs/univang.h"

#include "../dast/poly3d.h"

#include "../particle/particle.h"
#include "../particle/partmap.h"
#include "../particle/df.h"
#include "../particle/light.h"

#include "track.h"
#include "hobj.h"
#include "moveland.h"
#include "items.h"
#include "sensor.h"
#include "effect.h"
#include "mechos.h"
#include "compas.h"
#include "../sound/hsound.h"

#include "../uvs/diagen.h"
#include "magnum.h"

#include "../actint/credits.h"

#define INSECTOIDS

int DbgCheckEnable;

int AUTOMATIC_WORLD_INDEX = WORLD_NECROSS;
//#define AUTOMATIC_CHANGE_WORLD

#define DUST
#define AUTOMAT

extern iGameMap* curGMap;
extern int frame;
extern uchar* palbufOrg;
extern uchar* FireColorTable;
extern int multi_analysis;
extern int multi_draw;
extern int RAM16;
extern int GameQuantReturnValue;

int mechosCameraOffsetX = 0;
extern int aciWorldIndex;

extern int light_modulation;
//XStream ggg;


int test_block(unsigned char* ptr, int size);
void camera_impulse(int amplitude_8);

int vSetVangerFlag = -1;

int* SpeedLenTable;
int* SpeedAngleTable;
int* RudderLenTable;
int* RudderAngleTable;

extern TrackType HideTrack;

int NumHumanModel = 1;
int AdvancedView = 0;

#ifdef TEST_TRACK
Vector vGoTest;
int CurrentLinkStatus = 0;
LinkType* TestPrevLink = NULL;
int TestLinkTime = 1;
int TestLevelFlag = 0;
#endif

#ifdef ITEM_LOG
extern XStream xLog;
#endif

#ifdef SAVE_AI_STATUS
XStream AiStatusLog("ai_log.log",XS_OUT);
void SaveAiStaus(char* p,VangerUnit* v,int time);
#endif

void DrawCircle(int x, int y, int R, int color);

int ChangeEnergy,ChangeArmor;

CompasObject CompasObj;

extern int aciCurJumpCount;
extern int aciMaxJumpCount;

const int FLY_DEVICE_DEFAULT = 22;
const int MOLE_DEVICE_DEFAULT = 23;
const int SWIM_DEVICE_DEFAULT = 21;

void aPutStr(int x,int y,int font,int color,unsigned char* str,int bsx, unsigned char* buf,int space);
void calc_view_factors();

void uvsChangeCycle(void);

Vector vInsectTarget;

extern int aciProtractorEvent;
extern int aciMechMessiahEvent;
extern int uvsKronActive;

extern int  MAX_TABUTASK;

int UnitGlobalTime;

LandQuake SkyQuake;
int SkyQuakeEnable;
LandQuake SkyQuake2;
int SkyQuakeEnable2;
LandQuake SkyQuake3;
int SkyQuakeEnable3;

extern int aciTeleportEvent;

const int SKY_QUAKE_RADIUS = 300;
const int SKY_QUAKE_DELTA = 15;
const int SKY_QUAKE_TIME = 10;
const int SKY_QUAKE_DELAY = 25;

const int SIGNATOR_DELAY = 70;

extern int Dead,Quit;

int aiReadEvent,aiWriteEvent;
int aiNumGlobalEvent[2][AI_EVENT_MAX];
aiUnitEvent aiGlobalEventData[2][AI_EVENT_MAX][AI_GLOBAL_EVENT_MAX];

int TotalVangerSpeed;

extern int max_jump_power;

extern int aciCurLuck;
extern int aciCurDominance;

int aiStatusLastEscave,aiStatusEscaveEmpty;
int aiStatusPodishCount,aiStatusIncubatorCount;
int aiStatusPodishEmptyCount,aiStatusIncubatorEmptyCount;
int aiStatusEmptyCount,aiStatusEmptyTime;
int aiStatusPrevY;

aiFactorType aiFactorData[AI_FACTOR_NUM];

extern uvsTabuTaskType **TabuTable;

int aiHotBugData00;
int aiHotBugData01;
int aiHotBugData02;
int aiHotBugData03;

int aiHotBugAdd00;
int aiHotBugAdd01;
int aiHotBugAdd02;
int aiHotBugAdd03;

void aOutText32clip(int x,int y,int color,void* text,int font,int hspace,int vspace);

void FIRE_ALL_WEAPONS()
{
	// TODO(amdmi3): this should handle both ctrl's actually
	ActD.keyhandler(SDL_SCANCODE_RCTRL);
}

char CheckAddLink(LinkType* l,BranchType* b,int s)
{
// l - Next Link
/*	l += s;
	if((int)(l) >= (int)(b->Link + b->NumLink)) return 0;
	return 1;*/
	if(((uintptr_t)(l) + s*sizeof(LinkType)) >= (uintptr_t)(b->Link + b->NumLink)) return 0;
	return 1;
};

char CheckDecLink(LinkType* l,BranchType* b,int s)
{
// l - Prev Link
/*	l -= s;
	if(int(l) < (int)(b->Link)) return 0;
	return 1;*/
	if(((uintptr_t)(l) - s*sizeof(LinkType)) < (uintptr_t)(b->Link)) return 0;
	return 1;
};

int GetAngle(int x0,int y0,int x1,int y1,int x2,int y2)
{
	int nx,ny,dx,dy;
	nx = getDistY(y2,y1);
	ny = -getDistX(x2,x1);

	dx = getDistX(x0,x1);
	dy = getDistY(y0,y1);
	return dx*nx + dy*ny;
};

char CheckInNode(Vector& v,NodeType* n)
{
	char i;
	char nn;
	int bx,by,pbx,pby;
	int* dx;
	int* dy;

	nn = n->NumBranch;
	dx = n->BorderX;
	dy = n->BorderY;
	pbx = *dx;
	pby = *dy;
	if(GetAngle(v.x,v.y,dx[nn - 1],dy[nn - 1],pbx,pby) < 0) return 0;
	for(i = 1;i < nn;i++){
		dx++;
		dy++;
		bx = *dx;
		by = *dy;
		if(GetAngle(v.x,v.y,pbx,pby,bx,by) < 0) return 0;
		pbx = bx;
		pby = by;
	};
	return 1;
};

char CheckInLink(Vector& v,LinkType* pl,LinkType* nl)
{
	if(GetAngle(v.x,v.y,nl->xl,nl->yl,nl->xr,nl->yr) < 0) return 0; // NEXT
	if(GetAngle(v.x,v.y,pl->xr,pl->yr,pl->xl,pl->yl) < 0) return 0; // PREV
	if(GetAngle(v.x,v.y,pl->xl,pl->yl,nl->xl,nl->yl) < 0) return 0; // LEFT
	if(GetAngle(v.x,v.y,nl->xr,nl->yr,pl->xr,pl->yr) < 0) return 0; // RIGHT
	return 1;
};

char CheckInBranch(Vector& v,LinkType* pl,LinkType* nl,BranchType* b)
{
	char i;
	for(i = 0;i < CHECK_BRANCH_DELTA;i++){
		if(GetAngle(v.x,v.y,nl->xl,nl->yl,nl->xr,nl->yr) < 0){
			nl++;
			if(nl >= b->Link + b->NumLink) return 0;
			else pl++;
		}else{
			if(GetAngle(v.x,v.y,pl->xr,pl->yr,pl->xl,pl->yl) < 0){
				nl--;
				if(nl <= b->Link) return 0;
				else pl--;
			}else{
				if(GetAngle(v.x,v.y,pl->xl,pl->yl,nl->xl,nl->yl) < 0) return 0;
				else{
					if(GetAngle(v.x,v.y,nl->xr,nl->yr,pl->xr,pl->yr) < 0) return 0;
					else return 1;
				};
			};
		};
	};
	return 0;
};

int LinkDist(int x,int y,int x1,int y1,int x2,int y2)
{
	int d,a;
	Vector vBorder;
	Vector vLink;
	vLink = Vector(getDistX(x1,x),getDistY(y1,y),0);
	vBorder = Vector(getDistX(x2,x1),getDistY(y2,y1),0);
	d = vLink.vabs();
	a = rPI(vBorder.psi() - vLink.psi());
	return round(d * fabs(Sin(a)));
};

void LinkDist(int x,int y,int x1,int y1,int x2,int y2,int radius,Vector& v)
{
	int d,l,r;

	Vector vBorder;
	Vector vLink;
	vLink = Vector(getDistX(x1,x),getDistY(y1,y),0);
	vBorder = Vector(getDistX(x2,x1),getDistY(y2,y1),0);
	d = round((vLink.vabs() * fabs(Sin(vBorder.psi() - vLink.psi())))) - radius;

	if(d < MAX_TOUCH_BORDER){
		vLink = Vector(vBorder.y,-vBorder.x,0);
		r = vLink.vabs();
		if(r > 5){
			l = (MAX_TOUCH_VECTOR - (d * MAX_DELTA_VECTOR >> 15)) / 2;
			v += vLink * l / r;
		};
	};
};

void AxisLen(Vector vA0,Vector vA1,Vector vC,Vector& vR)
{
	Vector vTrack,vCheck;
	int dAp,dC,dA;
	vTrack = Vector(getDistX(vA1.x,vA0.x),getDistY(vA1.y,vA0.y),vA1.z - vA0.z);
	vCheck = Vector(getDistX(vC.x,vA0.x),getDistY(vC.y,vA0.y),vC.z - vA0.z);
	dAp = vTrack.vabs();
	dC = vCheck.vabs();
	vCheck.x = vCheck.x * vTrack.x;
	vCheck.y = vCheck.y * vTrack.y;
	vCheck.z = vCheck.z * vTrack.z;
	dA = (vCheck.x + vCheck.y + vCheck.z)/dAp;
	vR = vA0 + (dA * vTrack / dAp);
	cycleTor(vR.x,vR.y);
	/*line_trace(Vector(vA0.x,vA0.y,159),Vector(vA1.x,vA1.y,159));
	line_trace(Vector(vA0.x,vA0.y,159),Vector(vC.x,vC.y,159));
	line_trace(Vector(vR.x,vR.y,159),Vector(vC.x,vC.y,159));*/
};

/*void PointDist(int x,int y,int z,int x1,int y1,int z1,int r,int radius,char dir,Vector& vd,Vector& v)
{
	int l,d,m;
	int r;
	Vector vLink;

	if(abs(z - z1) < radius){
		vLink = Vector(getDistX(x,x1),getDistY(y,y1),0);
//		r = vLink.vabs();
		d = round(r * fabs(Sin(vd.psi() - vLink.psi())));

		if(r < radius){
			l = MAX_TOUCH_VECTOR - (r * MAX_TOUCH_VECTOR/radius);
			v += vLink * l / r;
			m = MAX_TOUCH_VECTOR - l;
		}else m = MAX_TOUCH_VECTOR;

		if(r < MAX_TOUCH_TARGET){
			if(dir) vLink = Vector(vd.y,-vd.x,0);
			else vLink = Vector(-vd.y,vd.x,0);
			l = m - d * m / MAX_TOUCH_TARGET;
			d = m - r * m / MAX_TOUCH_TARGET;
			v += vLink * l * d / (vLink.vabs() * m);
		};
	};
};*/

void PointDist(int dx,int dy,int dz,int r,int radius,char dir,Vector& vd,Vector& v)
{
	int l,d,m;
//	int r;
	Vector vLink;

	if(abs(dz) < radius){
		vLink = Vector(dx,dy,0);
//		r = vLink.vabs();
		d = round(r * fabs(Sin(vd.psi() - vLink.psi())));

		if(r < radius){
			l = MAX_TOUCH_VECTOR - (r * MAX_TOUCH_VECTOR/radius);
			v += vLink * l / r;
			m = MAX_TOUCH_VECTOR - l;
		}else m = MAX_TOUCH_VECTOR;

		if(r < MAX_TOUCH_TARGET){
			if(dir) vLink = Vector(vd.y,-vd.x,0);
			else vLink = Vector(-vd.y,vd.x,0);
			l = m - d * m / MAX_TOUCH_TARGET;
			d = m - r * m / MAX_TOUCH_TARGET;
			v += vLink * l * d / (vLink.vabs() * m);
		};
	};
};

void LenDist(int x,int y,int z,int x1,int y1,int z1,int dist,int radius,Vector& v)
{
	int l;
	Vector vLink;
	if(abs(z - z1) < radius){
		vLink = Vector(getDistX(x,x1),getDistY(y,y1),0);
		if(dist < radius && dist > 0){
			l = MAX_TOUCH_VECTOR - (dist * MAX_TOUCH_VECTOR/radius);
			v += vLink * l / dist;
		};
	};
};

char GetNodeDist(TrackLinkType* p1,TrackLinkType* p2)
{
	NodeType* n;
	char i;

	if(p1->PointStatus & TRK_BRANCH_MASK){
		if(p2->PointStatus & TRK_BRANCH_MASK){
			if(p1->pBranch == p2->pBranch) return 0;
			else if(p1->pBranch->pBeg == p2->pBranch->pBeg || p1->pBranch->pBeg == p2->pBranch->pEnd ||
				   p1->pBranch->pEnd == p2->pBranch->pBeg || p1->pBranch->pEnd == p2->pBranch->pEnd) return 2;
		}else if(p1->pBranch->pBeg == p2->pNode || p1->pBranch->pEnd == p2->pNode) return 1;
	}else{
		if(p2->PointStatus & TRK_BRANCH_MASK){
			if(p2->pBranch->pBeg == p1->pNode || p2->pBranch->pEnd == p1->pNode) return 1;
		}else{
			n = p1->pNode;
			if(n == p2->pNode) return 0;
			for(i = 0;i < n->NumBranch;i++){
				if(n->Status[i]){
					if(n->pBranches[i]->pEnd == p2->pNode) return 2;
				}else{
					if(n->pBranches[i]->pBeg == p2->pNode) return 2;
				};
			};
		};
	};
	return 3;
};

void ActionUnit::InitEnvironment(void)
{
	collision_object = 0;
	PrevVisibility = Visibility;	
	if(Status & SOBJ_ACTIVE){
		Visibility = VISIBLE;
		return;
	};	
	int xt = getDistX(R_curr.x,ViewX);
	int yt = getDistY(R_curr.y,ViewY);
	if(DepthShow){
		double xx = A_g2s.a[0]*xt + A_g2s.a[1]*yt;
		double yy = A_g2s.a[3]*xt + A_g2s.a[4]*yt;
		double zz = A_g2s.a[6]*xt + A_g2s.a[7]*yt + ViewZ;
		if(zz <= 0){
			Visibility = UNVISIBLE;
			return;
			}
		zz = focus_flt/zz;
		xt = round(xx*zz);
		yt = round(yy*zz);
		if(abs(xt) - (radius << 1) < curGMap -> xside && abs(yt) - (radius << 1) < curGMap -> yside)
			Visibility = VISIBLE;
		else
			Visibility = UNVISIBLE;
		return;
		}
	else
		if(TurnAngle){
			int xx = round(A_g2s.a[0]*xt + A_g2s.a[1]*yt);
			yt = round(A_g2s.a[3]*xt + A_g2s.a[4]*yt);
			xt = xx;
			}

	if(abs(xt) - (radius << 1) < TurnSideX && abs(yt) - (radius << 1) < TurnSideY)
		Visibility = VISIBLE;
	else
		Visibility = UNVISIBLE;
};

void ActionUnit::Init(StorageType* s)
{
	BaseObject::Init(s);

	Status = SOBJ_DISCONNECT;
	ViewRadius = 300;
};

void ActionUnit::Free(void){ };
void ActionUnit::Open(void){ };

void ActionUnit::Close(void)
{
};

const char MECHOS_WATER_WAVE = 0;

void ActionUnit::CreateActionUnit(int nmodel/*Object& _model*/,int _status,const Vector& _v,int _angle,int put_mode)
{
	R_curr = _v;
	Angle = _angle;
	Status = _status;
	nModel = nmodel;
	Object::operator = (ModelD.ActiveModel(nModel));
	cycleTor(R_curr.x,R_curr.y);

	PrevVisibility = UNVISIBLE;

	set_3D(put_mode,R_curr.x,R_curr.y,R_curr.z,0,-Angle,0);

	ActionUnit::InitEnvironment();

	if(Status & SOBJ_ACTIVE) set_active(1);
	else set_active(0);

	MovMat = RotMat = DBM(Angle,Z_AXIS);
	Speed = 0;

	vUp = Vector(ymax_real,0,0)*MovMat;
	vDown = -vUp;

	vDirect =  Vector(0,0,0);

	EnvirAngle = EnvirLen = 0;

	SpeedDir = 1;
	MaxHideSpeed = MaxSpeed = 50;

	MaxVelocity = 200;
	CurrSpeed = 0;

	NumCalcUnit = 0;

	Count = 0;
	DeltaTraction = 128;

	wProcess = &(EffD.DeformData[MECHOS_WATER_WAVE]);
	WavePhase = 0;

	MoleInProcess = 0;
};

void ActionUnit::MixVector(void)
{
	int a;
	
	CalcTrackVector(vEnvir,EnvirLen,a,EnvirAngle);
	if(SpeedDir > 0){
		MoveAngle = (RudderAngleTable[a]*RudderLenTable[EnvirLen]) >> 20;
		DeltaSpeed = MaxVelocity - (2 * MaxVelocity * SpeedAngleTable[EnvirAngle] * SpeedLenTable[EnvirLen] >> 20);
	}else{
		MoveAngle  = (RudderAngleTable[rPI(a + PI)]*RudderLenTable[EnvirLen]) >> 20;
		DeltaSpeed = (2 * MaxVelocity * SpeedAngleTable[PI - EnvirAngle] * SpeedLenTable[EnvirLen] >> 20) - MaxVelocity;
	};
};

void ActionUnit::CalcTrackVector(Vector& v,int& len,int& angle1,int& angle2)
{
	len = v.vabs();
	if(len >= MAX_TOUCH_VECTOR) len = MAX_TOUCH_VECTOR - 1;

//	if(len < 0 || len > MAX_TOUCH_VECTOR) ErrH.Abort("Bad Lenght Vector");
	if(len){
		angle2 = angle1 = rPI(v.psi() - Angle);
//		if(angle1 < 0 || angle1 >= PI * 2) ErrH.Abort("Bad Angle2 Vector");
		if(angle2 > PI) angle2 = abs(angle2 - 2*PI);
	}else angle1 = angle2 = 0;
//	if(angle2 < 0 || angle2 > PI) ErrH.Abort("Bad Angle Vector");
};

void ActionUnit::Action(void)
{
	int d;
	Vector vCheck;

	if(!SpeedDir){
		brake_on();
		return;
	};

	if(SpeedDir > 0){
		MoveAngle = rPI(MoveAngle - Angle);
		CurrSpeed += DeltaSpeed;
	}else{
		MoveAngle = rPI(MoveAngle - Angle + PI);
		CurrSpeed += DeltaSpeed;
	};

	if(MoveAngle > PI) MoveAngle -= 2*PI;

	if(MoveAngle > 0){
		if(MoveAngle > MECHOS_ROT_DELTA) MoveAngle = MECHOS_ROT_DELTA;
	}else{
		if(MoveAngle < -MECHOS_ROT_DELTA) MoveAngle = -MECHOS_ROT_DELTA;
	};

	if(abs(MoveAngle) < PI / 10) controls(CONTROLS::TURBO_QUANT,83);

	if(CurrSpeed > MaxSpeed) CurrSpeed = MaxSpeed;
	if(CurrSpeed < -MaxSpeed) CurrSpeed = -MaxSpeed;

	d = CurrSpeed - Speed;

/*	if(d > 0){
		if(d > CurrSpeed) brake_on();
		if((traction += DeltaTraction) > 256) traction = 256;
	}else{
		if(d < CurrSpeed) brake_on();
		if((traction -= DeltaTraction) < -256) traction = -256;
	};

	if(Speed >= 0) rudder = -MoveAngle;
	else rudder = MoveAngle;*/

	if(d > 0){
		if(d > CurrSpeed) controls(CONTROLS::BRAKE_QUANT,83);
		else controls(CONTROLS::TRACTION_INCREASE,83);
	}else{
		if(d < CurrSpeed) controls(CONTROLS::BRAKE_QUANT,83);
		controls(CONTROLS::TRACTION_DECREASE,83);
	};

	if(Speed >= 0) controls(CONTROLS::STEER_BY_ANGLE,-MoveAngle);
	else controls(CONTROLS::STEER_BY_ANGLE,MoveAngle);
};

void ActionUnit::HideAction(void)
{	
	dynamic_state = WHEELS_TOUCH;
	if(SpeedDir == 0) return;
	else{
		if(SpeedDir > 0){
			CurrSpeed = MaxHideSpeed / 2 + RND(MaxHideSpeed/2);
			MoveAngle = rPI(MoveAngle - Angle);
		}else{
			MoveAngle = rPI(MoveAngle - Angle + PI);
			CurrSpeed = -MaxHideSpeed / 2 - RND(3*MaxHideSpeed/2);
		};
	};

	if(MoveAngle > PI) MoveAngle -= 2*PI;

	if(MoveAngle > 0){
		if(MoveAngle > MECHOS_ROT_DELTA) MoveAngle = MECHOS_ROT_DELTA;
	}else{
		if(MoveAngle < -MECHOS_ROT_DELTA) MoveAngle = -MECHOS_ROT_DELTA;
	};

	Angle = rPI(Angle + MoveAngle);
	vDirect = Vector(CurrSpeed,0,0) * DBM(Angle,Z_AXIS);

	R_curr.x += vDirect.x;
	R_curr.y += vDirect.y;
	cycleTor(R_curr.x,R_curr.y);
};

void ActionUnit::Quant(void)
{
	MoveAngle = 0;
	DeltaSpeed = 0;

	if(Visibility == VISIBLE){
		if(PrevVisibility == UNVISIBLE) Hide2Show();
		analysis();

		RotMat = A_l2g*DBM(PI/2,Z_AXIS);
		Angle = rPI((int)RTOG(atan2(RotMat.a[1],RotMat.a[0])));
		MovMat = DBM(Angle,Z_AXIS);
		Speed = (int)V.y;

		cycleTor(R_curr.x,R_curr.y);
		MapLevel = GetMapLevel(R_curr);
	}else{
		MovMat = RotMat = DBM(Angle,Z_AXIS);
		Speed = CurrSpeed;
		MapLevel = 0;
	};
	vUp = Vector(ymax_real,0,0)*MovMat;
	vDown = -vUp;
};

void ActionUnit::DrawQuant(void)
{
	draw();
	if(dynamic_state & TOUCH_OF_WATER){
		if(wProcess->CheckOffset(WavePhase)) WavePhase = 0;
		wProcess->Deform(R_scr.x,R_scr.y,WavePhase,0);
	};
};

void ActionUnit::Touch(GeneralObject *obj)
{
};

void VangerUnit::BulletCollision(int pow,GeneralObject* p)
{
	if(ActD.Active && Visibility == VISIBLE){
		if(p)SOUND_HIT(getDistX(ActD.Active->R_curr.x,R_curr.x))
		else{
			if(Status & SOBJ_ACTIVE) SOUND_DAMAGE()
		};
	};

	if(NetworkON && !(Status & SOBJ_ACTIVE)) return;	
	int pa = Armor;	
	int s;
	if(original_scale_size - scale_size)
		pow = round((double)(pow) * 2. * original_scale_size / (original_scale_size - scale_size));

	Energy -= pow;
	if(Energy < 0){
		Armor += Energy;
		Energy = 0;
	};
	
	if(pa > 0 && Armor <= 0 && p && NetworkON && p->ID == ID_VANGER){
		switch(my_server_data.GameType){
			case VAN_WAR:
				if(!(my_server_data.Van_War.TeamMode) || ((VangerUnit*)(p))->uvsPoint->Pmechos->color != uvsPoint->Pmechos->color)
					NetDestroyID = GET_STATION(p->NetID);
				break;
			case PASSEMBLOSS:
				NetDestroyID = GET_STATION(p->NetID);
				break;
			case MECHOSOMA:
				if(!(my_server_data.Mechosoma.TeamMode) || ((VangerUnit*)(p))->uvsPoint->Pmechos->color != uvsPoint->Pmechos->color)
					NetDestroyID = GET_STATION(p->NetID);
				break;
		};
	};

	if(Status & SOBJ_ACTIVE){
		if(Armor < 0)
			Armor = 0;
	}else{
		if(p && p->ID == ID_VANGER && (p->Status & SOBJ_ACTIVE)){
			s = 0;
			if(!(TabuUse & TABUTASK_COUNT_OTHER)){
				switch(VangerRaceStatus){
					case VANGER_RACE_ELR:
						s |= UVS_KRON_FLAG::ELEREC;
						break;
					case VANGER_RACE_PPS:
						s |= UVS_KRON_FLAG::PIP;
						break;
					case VANGER_RACE_KRW:
						s |= UVS_KRON_FLAG::KERN;
						break;
					case VANGER_RACE_ZLP:
						s |= UVS_KRON_FLAG::ZEEXL;
						break;
				};

				if(PowerFlag & VANGER_POWER_RUFFA_GUN)
					s |= UVS_KRON_FLAG::RAFFA;

				if(uvsPoint->Pmechos->color != uvsPoint->Pmechos->color)
					s |= UVS_KRON_FLAG::ALIEN;

				switch(uvsPoint->Pmechos->color){
					case 0:
						s |= UVS_KRON_FLAG::ELIPOD;
						break;
					case 1:
						s |= UVS_KRON_FLAG::BEEBO;
						break;
					case 2:
						s |= UVS_KRON_FLAG::ZEEX;
						break;
					default:
						break;
				};
				TabuUse |= TABUTASK_COUNT_OTHER;
			};

			if((helicopter || R_curr.z > 300) && !(TabuUse & TABUTSAK_COUNT_FLY)){
				s |= UVS_KRON_FLAG::FLY;
				TabuUse |= TABUTSAK_COUNT_FLY;
			};
			
			if(VangerRaceStatus != VANGER_RACE_NONE && !(TabuUse & TABUTSAK_COUNT_RITUAL)){
				s |= UVS_KRON_FLAG::RITUAL;
				TabuUse |= TABUTSAK_COUNT_RITUAL;
			};

			if(CheckSpeetle() && !(TabuUse & TABUTASK_COUNT_SPEETLE)){
				s |= UVS_KRON_FLAG::SPETLE;
				TabuUse |= TABUTASK_COUNT_SPEETLE;
			};

			if(s) uvsCheckKronIventTabuTask(UVS_KRON_EVENT::SHOT,s);

			if(Armor <= 0 && pa > 0){
				PlayerDestroyFlag = 1;
				GamerResult.vanger_kill++;
				uvsPoint->KillStatic();
			};			
		};
	};	
};

void VangerUnit::DestroyCollision(int l_16,Object* p)
{
	int pa;
	if(NetworkON && !(Status & SOBJ_ACTIVE)) return;

	if((Status & SOBJ_ACTIVE) && !(TabuUse & TABUTASK_COUNT_DAMAGE)){
		uvsCheckKronIventTabuTask(UVS_KRON_EVENT::DAMAGE,1);
		TabuUse |= TABUTASK_COUNT_DAMAGE;
	};

	pa = Armor;
	if((Armor -= l_16) < 0)
		Armor = 0;

	if(pa > 0 && Armor <= 0 && p){
		//NetDestroyID = GET_STATION(p->NetID);
		if(NetworkON && p->ID == ID_VANGER){
			switch(my_server_data.GameType){
				case VAN_WAR:
					if(!(my_server_data.Van_War.TeamMode) || ((VangerUnit*)(p))->uvsPoint->Pmechos->color != uvsPoint->Pmechos->color)
						NetDestroyID = GET_STATION(p->NetID);
					break;
				case PASSEMBLOSS:
					NetDestroyID = GET_STATION(p->NetID);
					break;
				case MECHOSOMA:
					if(!(my_server_data.Mechosoma.TeamMode) || ((VangerUnit*)(p))->uvsPoint->Pmechos->color != uvsPoint->Pmechos->color)
						NetDestroyID = GET_STATION(p->NetID);
					break;
			};
		};

		if(p->Status & SOBJ_ACTIVE){			
			PlayerDestroyFlag = 1;
			GamerResult.vanger_kill++;
			uvsPoint->KillStatic();		
		};
	};
};
											
void TrackUnit::GetBranch(void)
{
	switch(PointStatus){
		case TRK_IN_BRANCH:
			CheckPosition = 0;
			ChangeLink();
			break;
		case TRK_IN_NODE:
			CheckPosition = 0;
			if(!CheckInNode(vPoint,pNode)){
				if(CheckInBranch(vPoint,pPrevLink,pNextLink,pBranch)) PointStatus = TRK_IN_BRANCH;
				else PointStatus = TRK_OUT_NODE;
			};
			break;
		case TRK_END_BRANCH:
		case TRK_OUT_NODE:
		case TRK_OUT_BRANCH:
			if(ActiveGetBranch()){
				CheckPosition++;
				if(CheckPosition > MaxCheckPosition){
					CheckPosition = 0;
					GetInside();
				};
			};
			break;
	};
};

void TrackUnit::HideGetBranch(void)
{
	HideChangeLink(MoveDir);
};

void TrackUnit::WayInit(void)
{
	int i;

	for(i = 0;i < HideTrack.NumNode;i++) CalcWayDist[i] = 0;
	if(OtherFlag & MECHOS_TARGET_MOVE){
		if(TargetPoint.PointStatus & TRK_BRANCH_MASK){
			if(TargetPoint.pBranch -> pBeg) CalcWayDist[TargetPoint.pBranch -> pBeg -> index] = 1;
			if(TargetPoint.pBranch -> pEnd) CalcWayDist[TargetPoint.pBranch -> pEnd -> index] = 1;
		}else CalcWayDist[TargetPoint.pNode -> index] = 1;
		OtherFlag |= MECHOS_CALC_WAY;
	};
};

void VangerUnit::Destroy(void)
{
	WaterParticleObject* wp;
	int i;
	if(Visibility == VISIBLE){
		if(!BeebonationFlag){
			if(dynamic_state & TOUCH_OF_AIR){
				switch(DestroyPhase){
					case 0:
						if(Status & SOBJ_ACTIVE)
							camera_impulse(128);
						EffD.CreateExplosion(R_curr + vUp,EFF_EXPLOSION01,this,1 << 15,0);
						break;
					case 3:
						if(ActD.Active)
							SOUND_EXPLOSION(getDistX(ActD.Active->R_curr.x,R_curr.x))
						else
							SOUND_EXPLOSION(0)
						if(DestroyClass > 1)
							EffD.CreateExplosion(R_curr + vDown,EFF_EXPLOSION01,this,1 << 15,0);						
						break;
					case 4:
						switch(DestroyClass){
							case 0:
								EffD.CreateExplosion(R_curr,EFF_EXPLOSION03,this,1 << 15,0);
								break;
							case 1:
								EffD.CreateExplosion(R_curr,EFF_EXPLOSION03,this,2 << 15,0);
								break;
							case 2:
								EffD.CreateExplosion(R_curr,EFF_EXPLOSION03,this,2 << 15,1 << 10);
								break;
							case 3:
								EffD.CreateExplosion(R_curr,EFF_EXPLOSION03,this,3 << 15,1 << 10);
								break;
						};
						break;
					case 10:
						EffD.CreateExplosion(R_curr + vDown,EFF_EXPLOSION01,this,1 << 14,0);
						DestroyEnvironment();
						break;
					case 11:
						if(DestroyClass > 1)
							EffD.CreateExplosion(R_curr + vUp,EFF_EXPLOSION01,this,3 << 15,0);
						Explosion(64,nModel);
						break;
					case 17:
						if(DestroyClass > 0)
							EffD.CreateExplosion(R_curr + vDown,EFF_EXPLOSION01,this,1 << 14,0);
						break;
					case 18:						
						if(DestroyClass > 2)
							EffD.CreateExplosion(R_curr + vUp,EFF_EXPLOSION01,this,1 << 15,1 << 11);
						break;
					case 21:
						if(DestroyClass > 1)
							EffD.CreateExplosion(R_curr + vUp,EFF_EXPLOSION01,this,1 << 14,0);
						break;
					case 22:
						if(DestroyClass > 0)
							EffD.CreateExplosion(R_curr + vUp,EFF_EXPLOSION01,this,1 << 15,0);
						break;
					case 24:
						if(DestroyClass > 2)
							EffD.CreateExplosion(R_curr,EFF_EXPLOSION03,this,1 << 13,3 << 10);
						break;
					default:
						if(dynamic_state & (GROUND_COLLISION | WHEELS_TOUCH))
							MapD.CreateCrater(R_curr,MAP_POINT_CRATER09);
						break;
				};
			}else{
				if(DestroyPhase & 3){
					wp = (WaterParticleObject*)(EffD.GetObject(EFF_PARTICLE04));
					if(wp){
						wp->CreateParticle(20,5,3 << 8,radius,5,28,5,Vector(R_curr.x + radius - RND(2*radius),R_curr.y + radius - RND(2*radius),R_curr.z));
						EffD.ConnectObject(wp);
					};

					EffD.CreateDeform(R_curr + Vector(PASSING_WAVE_RADIUS - realRND(PASSING_WAVE_RADIUS2),PASSING_WAVE_RADIUS - realRND(PASSING_WAVE_RADIUS2),83),DEFORM_WATER_ONLY,PASSING_WAVE_PROCESS);
					if(DestroyPhase == 11){
						Explosion(64, nModel);
						DestroyEnvironment();
						if(ActD.Active)
							SOUND_EXPLOSION(getDistX(ActD.Active->R_curr.x,R_curr.x))
					};

					if(RND(15) < 5 && (dynamic_state & (GROUND_COLLISION | WHEELS_TOUCH)))
						MapD.CreateCrater(R_curr,MAP_POINT_CRATER09);
				};
			};
		};
	};

	if(DestroyPhase++ == 27){
		if(Status & SOBJ_ACTIVE){
			if(NetworkON){
				NetStatisticUpdate(NET_STATISTICS_DEATH);
				send_player_body(my_player_body);
				
				ExternalMode = EXTERNAL_MODE_EARTH_PREPARE;
				ExternalTime = 10;
				PalCD.Set(CPAL_HIDE_PASSAGE,ExternalTime);

				switch(my_server_data.GameType){
					case MECHOSOMA:
						ChangeWorldConstraction = CurrentWorld;
						break;
					case VAN_WAR:
						if(my_server_data.Van_War.Nascency == -1) ChangeWorldConstraction = RND(3);
						else ChangeWorldConstraction = my_server_data.Van_War.Nascency;
						break;
					case PASSEMBLOSS:
						if(UsedCheckNum == 0) ChangeWorldConstraction = GloryPlaceData[0].World;
						else{
							for(i = UsedCheckNum - 1;i >= 0;i--){
								if(GloryPlaceData[i].World  < MAIN_WORLD_MAX - 1){
									ChangeWorldConstraction = GloryPlaceData[i].World;
									break;
								};
							};
						};
						break;
				};

				ExternalLock = 1;
				ExternalDraw = 1;
				ExternalObject = NULL;
				switch_analysis(0);
			}else{
				if(GetStuffObject(this,ACI_SPUMMY)/* && CurrentWorld < MAIN_WORLD_MAX - 1*/){
					ActD.SpummyRunner = 1;
					GamerResult.earth_unable = 1;
//					if(CurrentWorld != WORLD_FOSTRAL)
						ChangeWorldConstraction = WORLD_FOSTRAL;

					ExternalMode = EXTERNAL_MODE_LIGHT;
					ExternalTime = 3;
		//			StartHidePassage(ExternalTime);
					PalCD.Set(CPAL_PASSAGE_TO,ExternalTime);
					ExternalLock = 1;
					ExternalDraw = 0;
					ExternalObject = NULL;
					switch_analysis(0);
				}else{
					Status |= SOBJ_DISCONNECT;
//					ObjectDestroy(this);
					uvsPoint -> destroy(PlayerDestroyFlag);
					GameOverID = GAME_OVER_EXPLOSION;
					GameQuantReturnValue = RTO_LOADING3_ID;
				};
			};
		}else{	
			if(!NetworkON) uvsPoint -> destroy(PlayerDestroyFlag);
			Status |= SOBJ_DISCONNECT;			
		};
	};
};

/*int TrackUnit::TraceWay(void)
{
	WayNodeType* n1;

	int* pdist;
	int td;

	int i,t;
	char j;
	int ff,fm;
	int nprocess;

	nprocess = 0;

	n1 = WayData;
	pdist = WayDist;
	for(i = 0;i < HideTrack.NumNode;i++,n1++,pdist++){
		if(n1->NumBranch > 0){
			ff = 0;
			fm = 0xfffffff;
			for(j = 0;j < n1->NumBranch;j++){
				td = WayDist[n1->Index[j]];
				if(td > 0){
					ff++;
					t = td + n1->LenBranch[j];
					if(t < fm) fm = t;
				};
			};

			if(ff > 0 && (*pdist == 0 || *pdist > fm)){
				*pdist = fm;
				nprocess++;
			};
		};
	};
	return nprocess;
};*/

int TrackUnit::TraceWay(void)
{
	NodeType* n;

	int* pdist;
	int td;

	int i,t;
	int j;
	int ff,fm;
	int nprocess;

	int* nInd;
	int* bInd;

	nprocess = 0;

	n = HideTrack.node;

	pdist = CalcWayDist;

	for(i = 0;i < HideTrack.NumNode;i++,n++,pdist++){
		if(n->NumBranch == 0) ErrH.Abort("Null Node");
		ff = 0;
		fm = 0xfffffff;
		nInd = n->WayNodeIndex;
		bInd = n->WayBranchIndex;
		for(j = 0;j < n->NumWayNode;j++,nInd++,bInd++){
			td = CalcWayDist[*nInd];
			if(td > 0){
				ff++;
				t = td + TrackDist[*bInd];
				if(t < fm) fm = t;
			};
		};

		if(ff > 0 && (*pdist == 0 || *pdist > fm)){
			*pdist = fm;
			nprocess++;
		};
	};
	return nprocess;
};


void TrackUnit::GetNode(TrackLinkType* tp,char& dir)
{
	NodeType * n;
	BranchType* b;
	int cDist = 0,nn,td;
	int i;

	if(TargetPoint.PointStatus & TRK_BRANCH_MASK){
		if(tp->pNode == TargetPoint.pBranch->pBeg){
			tp->pBranch = TargetPoint.pBranch;
			tp->pPrevLink = TargetPoint.pBranch->Link;
			tp->pNextLink = tp->pPrevLink + 1;
			dir = 1;
			return;
		}else{
			if(tp->pNode == TargetPoint.pBranch->pEnd){
				tp->pBranch = TargetPoint.pBranch;
				tp->pNextLink = TargetPoint.pBranch->Link + TargetPoint.pBranch->NumLink - 1;
				tp->pPrevLink = tp->pNextLink - 1;
				dir = 0;
				return;
			};
		};
	}else{
		n = tp->pNode;
		if(n == TargetPoint.pNode){
			dir = -1;
			return;
		};

		nn = -1;
		for(i = 0;i < n->NumBranch;i++){
			b = n->pBranches[i];
			if(n->Status[i]){
				if(b->pEnd == TargetPoint.pNode){
					nn = i;
					break;
				};
			}else{
				if(b->pBeg == TargetPoint.pNode){
					nn = i;
					break;
				};
			};
		};

		if(nn != -1){
			if(nn <= -1 || nn >= n->NumBranch) ErrH.Abort("Bad Short Way");
			tp->pBranch = n->pBranches[nn];
			if(n->Status[nn]){
				tp->pPrevLink = tp->pBranch->Link;
				tp->pNextLink = tp->pPrevLink + 1;
				dir = 1;
			}else{
				tp->pNextLink = tp->pBranch->Link + tp->pBranch->NumLink - 1;
				tp->pPrevLink = tp->pNextLink - 1;
				dir = 0;
			};
			return;
		};
	};
	
	n = tp->pNode;

	nn = -1;

	for(i = 0;i < n->NumWayNode;i++){
		td = WayDist[n->WayNodeIndex[i]] + TrackDist[n->WayBranchIndex[i]];
		if(td <= cDist || nn == -1){
			nn = i;
			cDist = td;
		};		
	};

	tp->pBranch = &(HideTrack.branch[n->WayBranchIndex[nn]]);

	if(tp->pBranch->pBeg == n){
		tp->pPrevLink = tp->pBranch->Link;
		tp->pNextLink = tp->pPrevLink + 1;
		dir = 1;
	}else{
		if(tp->pBranch->pEnd == n){
			tp->pNextLink = tp->pBranch->Link + tp->pBranch->NumLink - 1;
			tp->pPrevLink = tp->pNextLink - 1;
			dir = 0;
		}else ErrH.Abort("Error make WayTable");
	};
};

/*void TrackUnit::GetNode(TrackLinkType* tp,char& dir)
{
	NodeType * n;
	BranchType* b;
	int cDist,nn,td;
	int i;

	if(TargetPoint.PointStatus & TRK_BRANCH_MASK){
		if(tp->pNode == TargetPoint.pBranch->pBeg){
			tp->pBranch = TargetPoint.pBranch;
			tp->pPrevLink = TargetPoint.pBranch->Link;
			tp->pNextLink = tp->pPrevLink + 1;
			dir = 1;
			return;
		}else{
			if(tp->pNode == TargetPoint.pBranch->pEnd){
				tp->pBranch = TargetPoint.pBranch;
				tp->pNextLink = TargetPoint.pBranch->Link + TargetPoint.pBranch->NumLink - 1;
				tp->pPrevLink = tp->pNextLink - 1;
				dir = 0;
				return;
			};
		};
	}else{
		n = tp->pNode;
		if(n == TargetPoint.pNode){
			dir = -1;
			return;
		};

		cDist = WayDist[n->index];
		nn = -1;

		for(i = 0;i < n->NumBranch;i++){
			b = n->pBranches[i];
			if(n->Status[i]){
				if(b->pEnd == TargetPoint.pNode){
					nn = i;
					break;
				};
			}else{
				if(b->pBeg == TargetPoint.pNode){
					nn = i;
					break;
				};
			};
		};

		if(nn != -1){
			if(nn <= -1 || nn >= n->NumBranch) ErrH.Abort("Bad Short Way");
			tp->pBranch = n->pBranches[nn];
			if(n->Status[nn]){
				tp->pPrevLink = tp->pBranch->Link;
				tp->pNextLink = tp->pPrevLink + 1;
				dir = 1;
			}else{
				tp->pNextLink = tp->pBranch->Link + tp->pBranch->NumLink - 1;
				tp->pPrevLink = tp->pNextLink - 1;
				dir = 0;
			};
			return;
		};
	};
	
	n = tp->pNode;
	cDist = WayDist[n->index];
	if(cDist > 0){
		nn = -1;

		for(i = 0;i < n->NumWayNode;i++){
			if(WayDist[n->WayNodeIndex[i]] <= cDist){
				nn = i;
				break;
			};		
		};

		if(nn == -1) nn = RND(n->NumWayNode);
		else{
			for(i = nn + 1;i < n->NumWayNode;i++){
				if(WayDist[n->WayNodeIndex[i]] <= cDist && !RND(2))
					nn = i;				
			};
		};		

		if(nn == -1 || nn >= n->NumWayNode)
			ErrH.Abort("Bad Find Node");
	}else nn = 0;

	tp->pBranch = &(HideTrack.branch[n->WayBranchIndex[nn]]);

	if(tp->pBranch->pBeg == n){
		tp->pPrevLink = tp->pBranch->Link;
		tp->pNextLink = tp->pPrevLink + 1;
		dir = 1;
	}else{
		if(tp->pBranch->pEnd == n){
			tp->pNextLink = tp->pBranch->Link + tp->pBranch->NumLink - 1;
			tp->pPrevLink = tp->pNextLink - 1;
			dir = 0;
		}else ErrH.Abort("Error make WayTable");
	};	
};*/

void TrackUnit::GetInside(void)
{
	int i,j;
	Vector vTrack;

	LinkType* ml;
	LinkType* pml;
	BranchType* mb;
	NodeType* nn;

	Vector vCheck = R_curr;

	mb = HideTrack.branch;
	for(i = 0;i < HideTrack.NumBranch;i++,mb++){
		if(R_curr.y > mb->Top && R_curr.y < mb->Bottom){
			pml = mb->Link;
			ml = pml + 1;
			for(j = 1;j < mb->NumLink;j++){
				if(CheckInLink(vCheck,pml,ml) && ml->z < R_curr.z){
					pBranch = mb;
					pNextLink = ml;
					pPrevLink = pml;
					PointStatus = TRK_IN_BRANCH;
					OtherFlag |= MECHOS_RECALC_FRONT;
					return;
				};
				pml = ml;
				ml++;
			};
		};
	};

	nn = HideTrack.node;
	for(i = 0;i < HideTrack.NumNode;i++,nn++){
		if(R_curr.y > nn->Top && R_curr.y < nn->Bottom){
			if(CheckInNode(vCheck,nn) && nn->z < R_curr.z){
				pNode = nn;
				PointStatus = TRK_IN_NODE;
				OtherFlag |= MECHOS_RECALC_FRONT;
				return;
			};
		};
	};
	PointStatus = TRK_OUT_BRANCH;
};

void TrackLinkType::ChangeLink(void)
{
//	int aaa = 0;
	char vLog = 0;
	do{
		if(GetAngle(vPoint.x,vPoint.y,pNextLink->xl,pNextLink->yl,pNextLink->xr,pNextLink->yr) < 0 && vLog != 2){
			pNextLink++;
			if(pNextLink >= pBranch->Link + pBranch->NumLink){
				pNextLink--;
				if(pBranch -> pEnd){
					PointStatus = TRK_IN_NODE;
					pNode = pBranch -> pEnd;
				}else PointStatus = TRK_END_BRANCH;
			}else{
				vLog = 1;
				PointStatus = TRK_CHANGE_LINK;
				pPrevLink++;
			};
		}else{
			if(GetAngle(vPoint.x,vPoint.y,pPrevLink->xr,pPrevLink->yr,pPrevLink->xl,pPrevLink->yl) < 0 && vLog != 1){
				pNextLink--;
				if(pNextLink <= pBranch->Link){
					pNextLink++;
					if(pBranch -> pBeg){
						PointStatus = TRK_IN_NODE;
						pNode = pBranch -> pBeg;
					}else PointStatus = TRK_END_BRANCH;
				}else{
					vLog = 2;
					PointStatus = TRK_CHANGE_LINK;
					pPrevLink--;
				};
			 }else{
				if(GetAngle(vPoint.x,vPoint.y,pPrevLink->xl,pPrevLink->yl,pNextLink->xl,pNextLink->yl) < 0)
					PointStatus = TRK_OUT_BRANCH;
				else{
					if(GetAngle(vPoint.x,vPoint.y,pNextLink->xr,pNextLink->yr,pPrevLink->xr,pPrevLink->yr) < 0)
						PointStatus = TRK_OUT_BRANCH;
					else PointStatus = TRK_IN_BRANCH;
				};
			};
		};
//--------------------------------------------------------
//		if(aaa++ > 10000) ErrH.Abort("Change Link Overflow");
//--------------------------------------------------------
	}while(PointStatus == TRK_CHANGE_LINK);
};

void TrackLinkType::HideChangeLink(char dir)
{
//	int aaa = 0;
	if(dir){
		do{
			if(GetAngle(vPoint.x,vPoint.y,pNextLink->xl,pNextLink->yl,pNextLink->xr,pNextLink->yr) < 0){
				pNextLink++;
				if(pNextLink >= pBranch->Link + pBranch->NumLink){
					pNextLink--;
					if(pBranch -> pEnd){
						PointStatus = TRK_IN_NODE;
						pNode = pBranch -> pEnd;
					}else PointStatus = TRK_END_BRANCH;
				}else{
					PointStatus = TRK_CHANGE_LINK;
					pPrevLink++;
				};
			}else PointStatus = TRK_IN_BRANCH;
//--------------------------------------------------------
//		if(aaa++ > 10000) ErrH.Abort("Change Link Overflow");
//--------------------------------------------------------
		}while(PointStatus == TRK_CHANGE_LINK);
	}else{
		do{
			if(GetAngle(vPoint.x,vPoint.y,pPrevLink->xr,pPrevLink->yr,pPrevLink->xl,pPrevLink->yl) < 0){
				pNextLink--;
				if(pNextLink <= pBranch->Link){
					pNextLink++;
					if(pBranch -> pBeg){
						PointStatus = TRK_IN_NODE;
						pNode = pBranch -> pBeg;
					}else PointStatus = TRK_END_BRANCH;
				}else{
					PointStatus = TRK_CHANGE_LINK;
					pPrevLink--;
				};
			 }else PointStatus = TRK_IN_BRANCH;
//--------------------------------------------------------
//		if(aaa++ > 10000) ErrH.Abort("Change Link Overflow");
//--------------------------------------------------------
		}while(PointStatus == TRK_CHANGE_LINK);
	};
};

char TrackUnit::TestAnalysis(TrackLinkType& tp)
{
	char i;
	LinkType* lp;
	LinkType* ln;
	BranchType* b;

	tp.ChangeLink();
	switch(tp.PointStatus){
		case TRK_IN_NODE:
			if(CheckInNode(tp.vPoint,tp.pNode)) return 1;
			else{
				for(i = 0;i < tp.pNode->NumBranch;i++){
					b = tp.pNode->pBranches[i];
					if(tp.pNode->Status[i]){
						lp = b->Link;
						ln = lp + 1;
					}else{
						ln = b->Link + b->NumLink - 1;
						lp = ln - 1;
					};
					if(CheckInBranch(tp.vPoint,lp,ln,b)){
						tp.pBranch = b;
						tp.pPrevLink = lp;
						tp.pNextLink = ln;
						tp.PointStatus = TRK_IN_BRANCH;
						return 1;
					};
				};
				tp.PointStatus = TRK_OUT_NODE;
				return 0;
			};
			break;
		case TRK_IN_BRANCH:
			return 1;
	};
	return 0;
};

char TrackUnit::ActiveGetBranch(void)
{
	char i;
	LinkType* lp;
	LinkType* ln;
	BranchType* b;

	ChangeLink();

	if(PointStatus == TRK_IN_BRANCH) return 0;

	if(pNode){
		if(PointStatus & TRK_NODE_MASK){
			if(CheckInNode(vPoint,pNode)){
				PointStatus = TRK_IN_NODE;
				OtherFlag |= MECHOS_RECALC_FRONT;
				return 0;
			}else PointStatus = TRK_OUT_NODE;
		};

		for(i = 0;i < pNode->NumBranch;i++){
			b = pNode->pBranches[i];
			if(b != pBranch){
				if(pNode->Status[i]){
					lp = b->Link;
					ln = lp + 1;
				}else{
					ln = b->Link + b->NumLink - 1;
					lp = ln - 1;
				};
				if(CheckInBranch(vPoint,lp,ln,b)){
					pBranch = b;
					pPrevLink = lp;
					pNextLink = ln;
					PointStatus = TRK_IN_BRANCH;
					OtherFlag |= MECHOS_RECALC_FRONT;
					return 0;
				};
			};
		};
	};
	return 1;

/*	if(PointStatus & TRK_NODE_MASK){
		if(CheckInNode(vPoint,pNode)){
			PointStatus = TRK_IN_NODE;
			OtherFlag |= MECHOS_RECALC_FRONT;
			return;
		}else{
			for(i = 0;i < pNode->NumBranch;i++){
				b = pNode->pBranches[i];
				if(pNode->Status[i]){
					lp = b->Link;
					ln = lp + 1;
				}else{
					ln = b->Link + b->NumLink - 1;
					lp = ln - 1;
				};
				if(CheckInBranch(vPoint,lp,ln,b)){
					pBranch = b;
					pPrevLink = lp;
					pNextLink = ln;
					PointStatus = TRK_IN_BRANCH;
					OtherFlag |= MECHOS_RECALC_FRONT;
					return;
				};
			};
			PointStatus = TRK_OUT_NODE;
		};
	};*/
};


char TrackUnit::Check2Position(int fx,int fy,int num)
{
	int dx,dy;
	int tx,ty;
	int i;

	tx = R_curr.x << 15;
	ty = R_curr.y << 15;
	dx = (getDistX(fx,R_curr.x) << 15) / num;
	dy = (getDistY(fy,R_curr.y) << 15) / num;

	TestPoint.PointStatus = TRK_IN_BRANCH;
	TestPoint.pNextLink = pNextLink;
	TestPoint.pPrevLink = pPrevLink;
	TestPoint.pBranch = pBranch;
	TestPoint.pNode = pNode;

	for(i = 0;i < num;i++){
		TestPoint.vPoint.x = tx >> 15;
		TestPoint.vPoint.y = ty >> 15;
		if(!TestAnalysis(TestPoint)) return 0;
		tx += dx;
		ty += dy;
	};
	return 1;
};

void TrackLinkType::AddLink(char s)
{
	pNextLink += s;
	if((long long)(pNextLink) >= (long long)(pBranch->Link + pBranch->NumLink)){
		pNextLink = pBranch->Link + pBranch->NumLink - 1;
		if(pBranch -> pEnd){
			PointStatus = TRK_WAIT_NODE;
			pNode = pBranch -> pEnd;
		}else PointStatus = TRK_END_BRANCH;
	}else PointStatus = TRK_IN_BRANCH;
	pPrevLink = pNextLink - 1;
};

void TrackLinkType::DecLink(char s)
{
	pPrevLink -= s;
	if((long long)(pPrevLink) < (long long)(pBranch->Link)){
		pPrevLink = pBranch->Link;
		if(pBranch -> pBeg){
			PointStatus = TRK_WAIT_NODE;
			pNode = pBranch -> pBeg;
		}else PointStatus = TRK_END_BRANCH;
	}else PointStatus = TRK_IN_BRANCH;
	pNextLink = pPrevLink + 1;
};

TrackLinkType::TrackLinkType(void)
{
	PointStatus = 0;
	vPoint = Vector(0,0,0);
	pNextLink = NULL;
	pPrevLink = NULL;
	pBranch = NULL;
	pNode = NULL;
};

char TrackUnit::GetDirect(TrackLinkType* tp)
{
	TrackLinkType t;
	char d;

	if(tp->PointStatus & TRK_BRANCH_MASK){
		if(NoWayDirect != -1) return NoWayDirect;
		if(MoveDir){
			if(tp->pBranch->pEnd){				
				t.pNode = tp->pBranch->pEnd;
				GetNode(&t,d);
				if(t.pBranch == tp->pBranch) return 0;
			}else{
				if(tp->pBranch->pBeg){
					t.pNode = tp->pBranch->pBeg;
					GetNode(&t,d);
					if(t.pBranch != tp->pBranch) return 0;
				}else ErrH.Abort("Bad Node's Link to Branch");
			};
			return 1;
		}else{
			if(tp->pBranch->pBeg){				
				t.pNode = tp->pBranch->pBeg;
				GetNode(&t,d);
				if(t.pBranch == tp->pBranch) return 1;
			}else{
				if(tp->pBranch->pEnd){
					t.pNode = tp->pBranch->pEnd;
					GetNode(&t,d);
					if(t.pBranch != tp->pBranch) return 1;
				}else ErrH.Abort("Bad Node's Link to Branch");
			};
			return 0;
		};
	};
	return -1;
};

void TrackUnit::MixVector(void)
{
//	vEnvir = vEnvir * EnvirReaction;
//	vEnvir += vTarget * TargetReaction;
//	vEnvir /= TOTAL_REACTION;

	if(NumCalcUnit) vEnvir += vTarget / NumCalcUnit;
	ActionUnit::MixVector();
};

void TrackUnit::CreateTrackUnit(void)
{
	int i;

//	MobilityType = _type;
	
	WayDist = WayDistData;
	CalcWayDist = WayDistData + HideTrack.NumNode;
	memset(WayDistData,0,sizeof(int)*HideTrack.NumNode*2);

	TargetAngle = TargetLen = 0;
	EnvirReaction = 128;
	TargetReaction = TOTAL_REACTION - EnvirReaction;

	MoveDir = 1;

	PointStatus = TRK_IN_BRANCH;
	pNextLink = NULL;
	pBranch = NULL;
	HideTrack.GetFirstPosition(R_curr.x,R_curr.y,pNextLink,pBranch);

//	if(pBranch == NULL || pNextLink == NULL)
//		ErrH.Abort("Bad Track Position");

	if(pNextLink == pBranch->Link){
		pPrevLink = pNextLink;
		pNextLink++;
	}else pPrevLink = pNextLink - 1;

//	WayData = ActD.WayMap[MobilityType];
//	MobilityValue = ActD.MaxMobility[MobilityType];
//	HideTrack.InitTrace(MaxNoise,0,WayData,0);
	if(pBranch->pBeg) pNode = pBranch->pBeg;
	else pNode = pBranch->pEnd;

	FrontDir = -1;
	FrontPoint.PointStatus = TRK_IN_BRANCH;
	FrontPoint.pNextLink = pNextLink;
	FrontPoint.pPrevLink = pPrevLink;
	FrontPoint.pBranch = pBranch;
	FrontPoint.pNode = pNode;

	PrevBranch = pBranch;

	OtherFlag = MECHOS_RECALC_FRONT | MECHOS_TARGET_MOVE;

	CheckWayCount = MaxWayCount = 50;
	

#ifdef TEST_TRACK
	CheckPosition = MaxCheckPosition = 50;
#else
	CheckPosition = MaxCheckPosition = 15;
#endif

	DestroyPhase = 0;

	vTarget = Vector(0,0,0);

	for(i =0;i < n_wheels;i++){
		PrevWheelX[i] = 0;
		PrevWheelY[i] = 0;
		PrevWheelFlag[i] = 0;
	};

	NullTime = 0;

/*	PrevWheelX[0] =  PrevWheelX[1] = 0;
	PrevWheelY[0] = PrevWheelY[1] = 0;
	PrevWheelFlag[0] = PrevWheelFlag[1] = 0;
	nLeftWheel = -1;
	nRightWheel = -1;
	for(i = 0;i < n_wheels;i++){
		if(!wheels[i].steer){
			if(nLeftWheel == -1) nLeftWheel = i;
			else if(nRightWheel == -1) nRightWheel = i;
				else break;
		};
	};*/

	vEnvir = Vector(0,0,0);
	vDirect = Vector(0,0,0);	

	aiMoveMode = AI_MOVE_TRACK;
	aiMoveFunction = AI_MOVE_FUNCTION_WHEEL;

	gRnd = 0xFFFFFFFF;
};

void TrackUnit::Init(StorageType* s)
{
	ActionUnit::Init(s);
};

void TrackUnit::Free(void)
{
};

void TrackUnit::Open(void)
{
	WayDistData = new int[HideTrack.NumNode * 2];
	TrackDist = new int[HideTrack.NumBranch];
};

void TrackUnit::Close(void)
{
	delete[] WayDistData;
	delete[] TrackDist;
};

void TrackUnit::TrackQuant(void)
{
	if((OtherFlag & MECHOS_RECALC_FRONT) && SpeedDir != 0){
		if(PointStatus & TRK_BRANCH_MASK){
			if(MoveDir){
				if(pBranch->pBeg) pNode = pBranch->pBeg;
				else{
					if(pBranch->pEnd) pNode = pBranch->pEnd;
					else ErrH.Abort("Bad Link to Node");
				};
			}else{
				if(pBranch->pEnd) pNode = pBranch->pEnd;
				else{
					if(pBranch->pBeg) pNode = pBranch->pBeg;
					else ErrH.Abort("Bad Link to Node");
				};
			};
			FrontPoint.PointStatus = TRK_IN_BRANCH;
			MoveDir = GetDirect(this);
		}else{
			pBranch = pNode->pBranches[0];
			if(pBranch->pBeg == pNode){
				pPrevLink = pBranch->Link;
				pNextLink = pPrevLink + 1;
				MoveDir = 0;
			}else{
				pNextLink = pBranch->Link + pBranch->NumLink - 1;
				pPrevLink = pNextLink - 1;
				MoveDir = 1;
			};
			FrontPoint.PointStatus = TRK_IN_NODE;
		};
		if(SpeedDir != 0) OtherFlag &= ~MECHOS_RECALC_FRONT;
	};
};

void TrackUnit::DirectQuant(void)
{
	int d;
	if(SpeedDir != 0){
//		if(MoveDir == -1) ErrH.Abort("Bad Move Direction");
//		if(pNode != pBranch->pBeg && pNode != pBranch->pEnd) ErrH.Abort("Bad Branch Link");
		switch(FrontPoint.PointStatus){
//-----------------------------------------------------------------------------------------------
			case TRK_IN_BRANCH:
				FrontPoint.pNextLink = pNextLink;
				FrontPoint.pPrevLink = pPrevLink;
				FrontPoint.pBranch = pBranch;
				FrontPoint.pNode = pNode;

				if(SpeedDir > 0){
					if(MoveDir) FrontPoint.AddLink(radius / 12);
					else FrontPoint.DecLink(radius / 12);
				}else{
					if(MoveDir) FrontPoint.DecLink(radius / 12);
					else FrontPoint.AddLink(radius / 12);
				};

				if((TargetPoint.PointStatus & TRK_BRANCH_MASK) && pBranch == TargetPoint.pBranch){
					if(TargetPoint.pNextLink <= FrontPoint.pNextLink){
						if(TargetPoint.pNextLink >= pNextLink){
							FrontPoint.pNextLink = TargetPoint.pNextLink;
							FrontPoint.pPrevLink = TargetPoint.pPrevLink;
							FrontPoint.PointStatus = TRK_TARGET_LINK;
						}else{
							MoveDir = 0;
							FrontPoint.PointStatus = TRK_IN_BRANCH;
						};
					}else{
						if(TargetPoint.pNextLink <= pNextLink){
							FrontPoint.pNextLink = TargetPoint.pNextLink;
							FrontPoint.pPrevLink = TargetPoint.pPrevLink;
							FrontPoint.PointStatus = TRK_TARGET_LINK;
						}else{
							MoveDir = 1;
							FrontPoint.PointStatus = TRK_IN_BRANCH;
						};
					};
				}else{
					if(FrontPoint.PointStatus == TRK_WAIT_NODE){
						PrevBranch = pBranch;
						GetNode(&FrontPoint,FrontDir);
						NoWayDirect = -1;
						if(SpeedDir == 0) FrontPoint.PointStatus = TRK_IN_BRANCH;
						else{
							if(FrontDir == -1) FrontPoint.PointStatus = TRK_TARGET_NODE;
							else
								if(FrontPoint.pBranch == pBranch){
									MoveDir ^= 1;
									FrontPoint.PointStatus = TRK_IN_BRANCH;
								};
						};
					};
				};

				break;
			case TRK_WAIT_NODE:
				if(PointStatus & TRK_NODE_MASK){
					if(pNode == FrontPoint.pNode){
						MoveDir = FrontDir;
						pNextLink = FrontPoint.pNextLink;
						pPrevLink = FrontPoint.pPrevLink;
						pBranch = FrontPoint.pBranch;

						if(SpeedDir > 0){
							if(MoveDir) FrontPoint.AddLink(radius / 12);
							else FrontPoint.DecLink(radius / 12);
						}else{
							if(MoveDir) FrontPoint.DecLink(radius / 12);
							else FrontPoint.AddLink(radius / 12);
						};
						FrontPoint.PointStatus = TRK_WAIT_BRANCH;
					}else FrontPoint.PointStatus = TRK_IN_NODE;
				}else{
//					if(pBranch != PrevBranch)
//						ErrH.Abort("Bad Prev Branch");

					if(SpeedDir > 0){
						if(MoveDir){
							if(CheckAddLink(pNextLink,pBranch,radius / 12)) FrontPoint.PointStatus = TRK_IN_BRANCH;
						}else{
							if(CheckDecLink(pPrevLink,pBranch,radius / 12)) FrontPoint.PointStatus = TRK_IN_BRANCH;
						};
					}else{
						if(MoveDir){
							if(CheckDecLink(pPrevLink,pBranch,radius / 12)) FrontPoint.PointStatus = TRK_IN_BRANCH;
						}else{
							if(CheckAddLink(pNextLink,pBranch,radius / 12)) FrontPoint.PointStatus = TRK_IN_BRANCH;
						};
					};
				};
				break;
			case TRK_WAIT_BRANCH:
				if(PointStatus & TRK_BRANCH_MASK){
					FrontPoint.PointStatus = TRK_IN_BRANCH;
//					if(pBranch != FrontPoint.pBranch) ErrH.Abort("Error in Wait Branch");
				};
				break;
//-----------------------------------------------------------------------------------------------
			case TRK_IN_NODE:
				FrontPoint.pNextLink = pNextLink;
				FrontPoint.pPrevLink = pPrevLink;
				FrontPoint.pBranch = pBranch;
				FrontPoint.pNode = pNode;
				GetNode(&FrontPoint,FrontDir);
				NoWayDirect = -1;
				if(SpeedDir == 0) FrontPoint.PointStatus = TRK_IN_NODE;
				else{
					if(FrontDir == -1) FrontPoint.PointStatus = TRK_TARGET_NODE;
					else{
						MoveDir = FrontDir;
						pNextLink = FrontPoint.pNextLink;
						pPrevLink = FrontPoint.pPrevLink;
						pBranch = FrontPoint.pBranch;

						if(SpeedDir > 0){
							if(MoveDir) FrontPoint.AddLink(radius / 12);
							else FrontPoint.DecLink(radius / 12);
						}else{
							if(MoveDir) FrontPoint.DecLink(radius / 12);
							else FrontPoint.AddLink(radius / 12);
						};
						FrontPoint.PointStatus = TRK_WAIT_BRANCH;
					};
				};
				break;
//-----------------------------------------------------------------------------------------------
			case TRK_TARGET_LINK:
				if(PointStatus & TRK_BRANCH_MASK){
					if(pBranch == FrontPoint.pBranch){
						d = (int)(pNextLink - FrontPoint.pNextLink);
						if(abs(d) > radius / 15) FrontPoint.PointStatus = TRK_IN_BRANCH;
					}else{
						OtherFlag |= MECHOS_RECALC_FRONT;
						FrontPoint.PointStatus = TRK_IN_BRANCH;
					};
				}else FrontPoint.PointStatus = TRK_IN_NODE;
				break;
			case TRK_TARGET_NODE:
				if(PointStatus & TRK_NODE_MASK){
					if(FrontPoint.pNode != pNode) FrontPoint.PointStatus = TRK_IN_NODE;
				}else FrontPoint.PointStatus = TRK_IN_BRANCH;
				break;
//-----------------------------------------------------------------------------------------------
			case TRK_END_BRANCH:
				MoveDir ^= 1;
				FrontPoint.PointStatus = TRK_IN_BRANCH;
				break;
		};

		switch(FrontPoint.PointStatus){
			case TRK_WAIT_NODE:
			case TRK_END_BRANCH:
			case TRK_WAIT_BRANCH:
			case TRK_TARGET_LINK:
			case TRK_IN_BRANCH:
				if(SpeedDir > 0){
					if(MoveDir) vDirect = Vector(getDistX(FrontPoint.pNextLink->x,XCYCL(R_curr.x + vUp.x)),getDistY(FrontPoint.pNextLink->y,R_curr.y + vUp.y),0);
					else vDirect = Vector(getDistX(FrontPoint.pPrevLink->x,XCYCL(R_curr.x + vUp.x)),getDistY(FrontPoint.pPrevLink->y,R_curr.y + vUp.y),0);
				}else{
					if(MoveDir) vDirect = Vector(getDistX(FrontPoint.pNextLink->x,XCYCL(R_curr.x + vDown.x)),getDistY(FrontPoint.pNextLink->y,R_curr.y + vDown.y),0);
					else vDirect = Vector(getDistX(FrontPoint.pPrevLink->x,XCYCL(R_curr.x + vDown.x)),getDistY(FrontPoint.pPrevLink->y,R_curr.y + vDown.y),0);
				};
				break;
			case TRK_IN_NODE:
			case TRK_TARGET_NODE:
				if(SpeedDir > 0) vDirect = Vector(getDistX(FrontPoint.pNode->x,XCYCL(R_curr.x + vUp.x)),getDistY(FrontPoint.pNode->y,R_curr.y + vUp.y),0);
				else vDirect = Vector(getDistX(FrontPoint.pNode->x,XCYCL(R_curr.x + vDown.x)),getDistY(FrontPoint.pNode->y,R_curr.y + vDown.y),0);
				break;
		};
	};
};

void TrackUnit::GetTrackVector(void)
{
	int tx,ty;
	Vector vCheck;

	vEnvir = vCheck = Vector(0,0,0);	

	if(PointStatus & TRK_BRANCH_MASK){
		tx = (pPrevLink->x + pNextLink->x) >> 1;
		ty = (pPrevLink->y + pNextLink->y) >> 1;

		LinkDist(tx,ty,pPrevLink->xl,pPrevLink->yl,pNextLink->xl,pNextLink->yl,radius,vCheck);
		LinkDist(tx,ty,pNextLink->xr,pNextLink->yr,pPrevLink->xr,pPrevLink->yr,radius,vCheck);
		LinkDist(vPoint.x,vPoint.y,pPrevLink->xl,pPrevLink->yl,pNextLink->xl,pNextLink->yl,radius,vEnvir);
		LinkDist(vPoint.x,vPoint.y,pNextLink->xr,pNextLink->yr,pPrevLink->xr,pPrevLink->yr,radius,vEnvir);

		vEnvir -= vCheck;
//		vEnvir /= 2;
	};
};

void TrackUnit::Quant(void)
{
	int i;
	for(i = 0;i < n_wheels;i++) PrevWheelFlag[i] = 0;

	ActionUnit::Quant();
	vPoint = R_curr;
	cycleTor(vPoint.x,vPoint.y);

	for(i = 0;i < n_wheels;i++) if(!PrevWheelFlag[i]) PrevWheelY[i] = 0;
//	if(!PrevWheelFlag[0]) PrevWheelY[0] = 0;
//	if(!PrevWheelFlag[1]) PrevWheelY[1] = 0;
};

void ModelDispatcher::Init(Parser& in)
{
	int i;
	char* n;
	double max_size;
	int size;

	in.search_name("NumModel");
	MaxModel = in.get_int();

	in.search_name("MaxSize");
	max_size = in.get_double();

	Data = new Object[MaxModel];
	NameData = new char*[MaxModel];

	for(i = 0;i < MaxModel;i++){
		in.search_name("ModelNum");
		if(i != in.get_int())
			ErrH.Abort("Bad Model Num");

		in.search_name("Name");
		n = in.get_name();
		in.search_name("Size");
		size = (int)(in.get_double()*256./max_size);
		Data[i].ID = ID_VANGER;
		Data[i].load(n,size);

		in.search_name("NameID");
		n = in.get_name();
		NameData[i] = new char[strlen(n) + 1];
		strcpy(NameData[i],n);
	};
/*
//Test CRC
	extern double k_distance_to_force;
	if(test_block((unsigned char*)&k_distance_to_force,20))
		ErrH.Abort("test");
*/
};

Object& ModelDispatcher::ActiveModel(char id)
{
	return Data[id];
};

int ModelDispatcher::FindModel(const char* name)
{
	int i;
	for(i = 0;i < MaxModel;i++) 
		if(!strcmp(name,NameData[i])) 
			return i;
	ErrH.Abort("Model Not Found",XERR_USER,0,name);
	return 0;
};

void ModelDispatcher::Free(void)
{
	int i;
	for(i = 0;i < MaxModel;i++){
		Data[i].free();
		delete[] NameData[i];
	};
	delete[] NameData;
	delete[] Data;
};

void ActionDispatcher::Init(Parser& in)
{
	int i,j,k;

	UnitList::Init(in);

	Total = 0;

	MaxUnit = new int[MAX_ACTION_UNIT];
	UnitStorage = new StorageType[MAX_ACTION_UNIT];

	in.search_name("NumActionUnit");
	for(i = 0;i < MAX_ACTION_UNIT;i++){
		MaxUnit[i] = in.get_int();
		Total += MaxUnit[i];
	};	
	
	UnitData = new GeneralObject*[Total];
	k = 0;
	for(i = 0;i < MAX_ACTION_UNIT;i++){
		UnitStorage[i].Init(MaxUnit[i]);
		for(j = 0;j < MaxUnit[i];j++){
			switch(i){
				case ACTION_VANGER:
					UnitData[k] = new VangerUnit;
					break;
			};
			UnitData[k]->Init(&UnitStorage[i]);
			UnitStorage[i].Deactive(UnitData[k]);
			k++;
		};
	};

/*	in.search_name("NumMobilityType");
	NumMobilityType = in.get_int();
	WayMap = new WayNodeType*[NumMobilityType];
	MaxMobility = new short[NumMobilityType];
	for(i = 0;i < NumMobilityType;i++){
		in.search_name("MobilityDelta");
		MaxMobility[i] = (int)in.get_int();
	};*/

	Active = NULL;
	pfActive = NULL;
	mfActive = NULL;
//----------------------------Initialize Mechos Control Table---------------------------------------------------

	SpeedLenTable = new int[MAX_TOUCH_VECTOR];
	SpeedAngleTable = new int[PI];
	RudderLenTable = new int[MAX_TOUCH_VECTOR];
	RudderAngleTable = new int[PI * 2];

	for(i = 0;i < MAX_TOUCH_VECTOR;i++){
//		SpeedLenTable[i] = (int)(pow((double)(i)/128.,1./3.) * 1024.);
//		RudderLenTable[i] = (int)(pow((double)(i)/128.,3./5.) * 1024.);
		SpeedLenTable[i] = (int)(pow((double)(i)/255.,1./3.) * 1024.);
		RudderLenTable[i] = (int)(pow((double)(i)/255.,3./5.) * 1024.);
	};

//	for(i = 0;i < PI;i++) SpeedAngleTable[i] = (int)(pow((double)(i)/(double)(PI),.5) * 1024.);
	for(i = 0;i < PI;i++) SpeedAngleTable[i] = (i*1024)/PI;
	for(i = 0;i < PI*2;i++) RudderAngleTable[i] = (int)(sin((double)(i)*M_PI/(double)(PI)) * 1024.) * PI / 2;

	//ggg.open("lst.aaa",XS_OUT);

	dastCreateResourceSign("resource/bml/sign.fnt");
	SignEngine = new dastPoly3D(Vector(83,83,83),0);

	LightData.Init(NULL);
	LightData.MemInit();
	LightData.CreateLight(63,12,LIGHT_TYPE::TOR | LIGHT_TYPE::DYNAMIC);

	SpobsDestroy = 0;
	ThreallDestroy = 0;
	LuckyFunction = 0;
	SpobsDestroyActive = 0;
	ThreallDestroyActive = 0;
	SpummyRunner = 0;

	PromptPodishFreeVisit = 0;
	PromptIncubatorFreeVisit = 0;
	PromptChangeCycleCount = 0;
	PromptCurrentWay = 0;
	PromptPodishCount = 0;
	PromptIncubatorCount = 0;
	PromptPrevY = 0;
	PromptPrevTime = 0;
	NetEnterOffset = 0;
	GlobalFuryLevel = 0;

	aiHotBugData00 = RND(0xffffffff);
	aiHotBugData01 = RND(0xffffffff);
	aiHotBugData02 = RND(0xffffffff);
	aiHotBugData03 = RND(0xffffffff);

	aiHotBugAdd00 = RND(0xffffffff);
	aiHotBugAdd01 = RND(0xffffffff);
	aiHotBugAdd02 = RND(0xffffffff);
	aiHotBugAdd03 = RND(0xffffffff);
};

void ActionDispatcher::Free(void)
{
	int i;

	dastDeleteResourceSign();
	delete SignEngine;

	delete[] SpeedAngleTable;
	delete[] RudderAngleTable;
	delete[] SpeedLenTable;
	delete[] RudderLenTable;

//	delete WayMap;
//	delete MaxMobility;

	for(i = 0;i < Total;i++){
		UnitData[i]->Free();
		delete UnitData[i];
	};

	for(i = 0;i < MAX_ACTION_UNIT;i++) {
		UnitStorage[i].Free();
	}
	delete[] UnitData;
	delete[] MaxUnit;
	delete[] UnitStorage;

//	ggg.close();
};

void ActionDispatcher::Open(Parser& in)
{
	int i;

	UnitList::Open(in);
/*	for(i = 0;i < NumMobilityType;i++){
		WayMap[i] = new WayNodeType[HideTrack.NumNode];
		for(j = 0;j < HideTrack.NumNode;j++){
			WayMap[i][j].Link = &HideTrack.node[j];
			WayMap[i][j].NumBranch = 0;
			WayMap[i][j].Index = new int[HideTrack.node[j].NumBranch];
			WayMap[i][j].LenBranch = new int[HideTrack.node[j].NumBranch];
			WayMap[i][j].BranchData = new BranchType*[HideTrack.node[j].NumBranch];
			WayMap[i][j].NodeData = new WayNodeType*[HideTrack.node[j].NumBranch];
		};
		HideTrack.InitTrace(MaxMobility[i],0,WayMap[i],0);
	};*/


	switch(CurrentWorld){
		case WORLD_FOSTRAL:
			FlyTaskFirstY = 14700;
			FlyTaskLastY = 1781;
			FlyTaskDirect = 0;
			break;
		case WORLD_GLORX:
			FlyTaskFirstY = 2600;
			FlyTaskLastY = 14830;
			FlyTaskDirect = 1;
			break;
		case WORLD_NECROSS:
			FlyTaskFirstY = 2800;
			FlyTaskLastY = 15500;
			FlyTaskDirect = 1;
			break;
		default:
			FlyTaskFirstY = -1;
			FlyTaskLastY = -1;
			FlyTaskDirect = 0;
			break;
	};
	FlyTaskEnable = 2;

	for(i = 0;i < Total;i++) UnitData[i]->Open();

	Active = NULL;
	
	pfActive = NULL;
	mfActive = NULL;

	fTail = NULL;
	vInsectTarget = Vector(-1,-1,-1);
	SkyQuakeEnable = 0;
	SkyQuakeEnable2 = 0;
	SkyQuakeEnable3 = 0;

	NetFunctionProtractor = NetFunctionMessiah = 0; 	

	FunctionSpobsDestroyActive = 0;
	FunctionThreallDestroyActive = 0;

	if(ThreallDestroyActive)
		aciActivateItemFunction(ACI_PROTRACTOR,ACI_PROTRACTOR_EVENT8);
	else
		aciDeactivateItemFunction(ACI_PROTRACTOR,ACI_PROTRACTOR_EVENT8);

	if(SpobsDestroyActive)
		aciActivateItemFunction(ACI_MECHANIC_MESSIAH,ACI_MECH_MESSIAH_EVENT6);
	else 
		aciDeactivateItemFunction(ACI_MECHANIC_MESSIAH,ACI_MECH_MESSIAH_EVENT6);

	aiReadEvent = 0;
	aiWriteEvent = 1;
	for(i = 0;i < AI_EVENT_MAX;i++)
		aiNumGlobalEvent[aiReadEvent][i] = aiNumGlobalEvent[aiWriteEvent][i] = 0;

	BigZek = NULL;
	DoorEnable = 1;
	CameraModifier = 0;
	PassageTouchEnable = 0;
//	camera_reset();	

	NumVisibleVanger = Total;
	ProtractorLight = NULL;
};

void ActionDispatcher::Close(void)
{
	int i;
	VangerUnit* p;
	VangerFunctionType* n;
	VangerFunctionType* nn;

	n = fTail;
	while(n){
		nn = n->Next;
		DeleteFunction(n);
		delete n;
		n = nn;
	};
	
	if(ProtractorLight){
		ProtractorLight->Destroy();
		ProtractorLight = NULL;
	};

	if(NetworkON){
		p = (VangerUnit*)Tail;
		while(p){	
			if(p->Status & SOBJ_ACTIVE){
				my_player_body.NetID = 0;
				send_player_body(my_player_body);
				NETWORK_OUT_STREAM.delete_object(p->NetID);
				NETWORK_OUT_STREAM.end_body();
				NETWORK_OUT_STREAM.delete_object(p->ShellNetID);
				NETWORK_OUT_STREAM.end_body();
				p->DeleteHandler();
			}else{
				p->ClearItemList();
				p->uvsPoint->destroy();
			};

			if(p->pNetPlayer){
				p->pNetPlayer->CreatePlayerFlag = 0;
				p->pNetPlayer->uvsPoint = NULL;
				for(i = 0;i < MAX_ACTIVE_SLOT;i++){
					p->pNetPlayer->SlotNetID[i] = 0;
					p->pNetPlayer->SlotStuffNetID[i] = 0;
					p->pNetPlayer->SlotFireCount[i] = 0;
				};
			};
			
			DeleteUnit(p);
			ObjectDestroy(p);
			p = (VangerUnit*)(p->NextTypeList);
		};	
	}else{
		p = (VangerUnit*)Tail;
		while(p){			
			p->DeleteHandler();
			DeleteUnit(p);
			ObjectDestroy(p);
			p = (VangerUnit*)(p->NextTypeList);
		};
	};

	for(i = 0;i < MAX_ACTION_UNIT;i++) UnitStorage[i].Check();
	for(i = 0;i < Total;i++) UnitData[i]->Close();
};

extern int view_lock;
extern int aciTextEndFlag;
char* dgGetThreallPhrase(void);
XBuffer RaceTxtBuff(1024);

int uvsWorldToCross(int fromWID, int toWID);

void ActionDispatcher::Quant(void)
{
	VangerUnit* p;
//	FishWarrior* f;
	Vector v1,v2;
	int i,lp_log;
	char* ThreallText;

	PlayerData* pd;
	int n_score[10],n_total,n_position,n_current;
	int c_score[10],n2_score[10];
	int drop_log;
	uvsPassage* pass;	

	static unsigned int FragCnt = 0;

	BigZek = NULL;	

	pfActive = NULL;
	mfActive = NULL;

	NumVisibleVanger = 1;

	aiReadEvent = aiWriteEvent;
	aiWriteEvent = 1 - aiReadEvent;
	for(i = 0;i < AI_EVENT_MAX;i++)
		aiNumGlobalEvent[aiWriteEvent][i] = 0;

	if(Active && Active->ID == ID_VANGER){
		for(i = 0;i < MAX_ACTIVE_SLOT;i++){
			if(Slot[i] && Slot[i]->ActIntBuffer.type == ACI_RADAR_DEVICE){
				LocatorNum = 0;
				p = (VangerUnit*)Tail;
				v1 = Active->R_curr;
				lp_log = 1;
				while(p){
					if(p != Active && p->Visibility == VISIBLE && p->ExternalMode == EXTERNAL_MODE_NORMAL && p->ExternalDraw && !(p->Status & SOBJ_WAIT_CONFIRMATION)){
						LocatorData[LocatorNum] = p;
						LocatorNum++;
						if(LocatorPoint == p) lp_log = 0;
					};
					p = (VangerUnit*)(p->NextTypeList);
				};
				if(lp_log) ChangeLocator();
				break;
			};
		};

		if(i == MAX_ACTIVE_SLOT)
			LocatorPoint = NULL;

		if(LocatorPoint){
			LightData.set_position(LocatorPoint->R_curr.x,LocatorPoint->R_curr.y,LocatorPoint->R_curr.z);
			LightData.Quant();
		};

		int my;
		StuffObject* s;

		WorldSeedNum = 0;
		aciRacingFlag = 0;
		BigZek = NULL;

		if(Active && Active->uvsPoint->Pworld && Active->uvsPoint->Pworld->gIndex == CurrentWorld){
			if(NetworkON){
				drop_log = 0;
				RaceTxtBuff.init();
				aciRacingFlag = 1;
				switch(my_server_data.GameType){
					case VAN_WAR:
						strcpy(aciCurRaceType,"VWR");
						if(my_server_data.Van_War.TeamMode){
							memset(n_score,0,sizeof(int)*10);
							memset(c_score,0,sizeof(int)*10);
							pd = players_list.first();
							while(pd){
								if(pd->status == GAMING_STATUS){
									n_score[pd->body.color] += pd->body.kills - pd->body.deaths;
									c_score[pd->body.color] += pd->body.beebos;
								};
								pd = (PlayerData*)pd -> next;
							};
							n_position = 1;
							n_total = 10;
							for(i = 0;i < 10;i++){
								if(n_score[i] >= my_server_data.Van_War.MaxKills) drop_log = 1;
								if(i != my_player_body.color){
									if(n_score[i] > n_score[my_player_body.color]){
										n_position++;
									}else{
										if(n_score[i] == n_score[my_player_body.color] && c_score[i] > c_score[my_player_body.color])
											n_position++;
									};
								};
							};
							RaceTxtBuff <= n_position < " TEAM";
						}else{
							pd = players_list.first();
							n_position = 1;
							n_total = 0;
							n_current = my_player_body.kills - my_player_body.deaths;
							while(pd){
								if(pd->status == GAMING_STATUS){
									if(pd->body.kills >= my_server_data.Van_War.MaxKills) drop_log = 1;
									if(pd->client_ID != GlobalStationID){
										if((pd->body.kills - pd->body.deaths) > n_current)
											n_position++;
										else{
											if((pd->body.kills - pd->body.deaths) == n_current && pd->body.beebos > my_player_body.beebos)
												n_position++;
										};
									};
									n_total++;
								};
								pd = (PlayerData*)pd -> next;
							};
							RaceTxtBuff <= n_position < "|" <= n_total;
						};

						//std::cout<<"age_of_current_game: "<<age_of_current_game()<<" Van_War.MaxTime:"<<my_server_data.Van_War.MaxTime*60<<std::endl;
						if(age_of_current_game() >= my_server_data.Van_War.MaxTime*60 || drop_log){
							GameOverID = GAME_OVER_NETWORK;
							ActD.Active->Status |= SOBJ_DISCONNECT;
							GameQuantReturnValue = RTO_LOADING3_ID;
						};
						break;
					case PASSEMBLOSS:
						strcpy(aciCurRaceType,"PMB");
						pd = players_list.first();
						n_position = 1;
						n_total = 0;
						n_current = my_player_body.PassemblossStat.CheckpointLighting;
						while(pd){
							if(pd->status == GAMING_STATUS){
								if(pd->client_ID != GlobalStationID){
									if(pd->body.PassemblossStat.CheckpointLighting > n_current)
										n_position++;
									else{
										if(pd->body.PassemblossStat.CheckpointLighting == n_current){
											if(pd->body.PassemblossStat.MaxTime < my_player_body.PassemblossStat.MaxTime)
												n_position++;
											else{
												if(pd->body.PassemblossStat.MaxTime == my_player_body.PassemblossStat.MaxTime && UsedCheckNum < GloryPlaceNum){
													if(pd->body.world == CurrentWorld){
														if(GloryPlaceData[UsedCheckNum].World == CurrentWorld){
															if(CurrentWorld < MAIN_WORLD_MAX - 1){
																if(abs(pd->y - GloryPlaceData[UsedCheckNum].R_curr.y) < abs(ActD.Active->R_curr.y - GloryPlaceData[UsedCheckNum].R_curr.y))
																	n_position++;
															}else{
																if(abs(getDistY(pd->y,GloryPlaceData[UsedCheckNum].R_curr.y)) < abs(getDistY(ActD.Active->R_curr.y,GloryPlaceData[UsedCheckNum].R_curr.y)))
																	n_position++;
															};
														}else{
															pass = GetPassage(CurrentWorld,GloryPlaceData[UsedCheckNum].World);
															if(pass){
																if(CurrentWorld < MAIN_WORLD_MAX - 1){
																	if(abs(pd->y - pass->pos_y) < abs(ActD.Active->R_curr.y - pass->pos_y))
																		n_position++;
																}else{
																	if(abs(getDistY(pd->y,pass->pos_y)) < abs(getDistY(ActD.Active->R_curr.y,pass->pos_y)))
																		n_position++;
																};
															};
														};
													}else{
														if(uvsWorldToCross(pd->body.world,GloryPlaceData[UsedCheckNum].World) < uvsWorldToCross(CurrentWorld,GloryPlaceData[UsedCheckNum].World))
															n_position++;
													};
												};
											};
										};
									};
								};
								n_total++;
							};
							pd = (PlayerData*)pd -> next;
						};
						RaceTxtBuff <= n_position < "|" <= n_total;
						break;
					case MECHOSOMA:
						strcpy(aciCurRaceType,"MHS");
						if(my_server_data.Mechosoma.TeamMode){
							memset(n_score,0,sizeof(int)*10);
							memset(c_score,0,sizeof(int)*10);
							memset(n2_score,0,sizeof(int)*10);
							pd = players_list.first();
							while(pd){
								if(pd->status == GAMING_STATUS){
									n_score[pd->body.color] += pd->body.MechosomaStat.ItemCount1;
									n2_score[pd->body.color] += pd->body.MechosomaStat.ItemCount2;
									c_score[pd->body.color] += pd->body.beebos;
								};
								pd = (PlayerData*)pd -> next;
							};

							n_position = 1;
							n_total = 10;
							for(i = 0;i < 10;i++){
								if(i != my_player_body.color){
									if((n_score[i] + n2_score[i]) > (n_score[my_player_body.color] + n2_score[my_player_body.color])){
										n_position++;
									}else{
										if((n_score[i] + n2_score[i]) == (n_score[my_player_body.color] + n2_score[my_player_body.color]) && c_score[i] > c_score[my_player_body.color])
											n_position++;
									};
								};
							};
							RaceTxtBuff <= n_position < " TEAM";
							if(n_score[my_player_body.color] >= my_server_data.Mechosoma.ProductQuantity1 &&  n2_score[my_player_body.color] >= my_server_data.Mechosoma.ProductQuantity2){
								GameOverID = GAME_OVER_NETWORK;
								ActD.Active->Status |= SOBJ_DISCONNECT;
								GameQuantReturnValue = RTO_LOADING3_ID;
							};
						}else{
							pd = players_list.first();
							n_position = 1;
							n_total = 0;
							n_current = my_player_body.MechosomaStat.ItemCount1 + my_player_body.MechosomaStat.ItemCount2;
							while(pd){
								if(pd->status == GAMING_STATUS){
									if(pd->client_ID != GlobalStationID){
										if((pd->body.MechosomaStat.ItemCount1 + pd->body.MechosomaStat.ItemCount2) > n_current)
											n_position++;
										else{
											if((pd->body.MechosomaStat.ItemCount1 + pd->body.MechosomaStat.ItemCount2) == n_current){
												if(pd->body.MechosomaStat.MinTransitTime < my_player_body.MechosomaStat.MinTransitTime)
													n_position++;
												else{
													if(pd->body.MechosomaStat.MinTransitTime == my_player_body.MechosomaStat.MinTransitTime && pd->body.beebos > my_player_body.beebos)
														n_position++;
												};
											};
										};
									};
									n_total++;
								};
								pd = (PlayerData*)pd -> next;
							};
							RaceTxtBuff <= n_position < "|" <= n_total;
						};
						break;
				};			
				strcpy(aciCurRaceInfo,RaceTxtBuff.GetBuf());				
			}else{
				switch(CurrentWorld){
					case WORLD_FOSTRAL:								
						if(uvsCurrentCycle == 1 && Active->VangerRaceStatus == VANGER_RACE_ELR){
							RaceTxtBuff.init();
							my = 1;
							i = uvsGameRaceFinish();
							p = (VangerUnit*)(ActD.Tail);
							while(p){
								if(p->VangerRaceStatus == VANGER_RACE_ELR){
									if(p != Active && p->R_curr.y > Active->R_curr.y) my++;
									i++;
								};
								p = (VangerUnit*)(p->NextTypeList);
							};
							strcpy(aciCurRaceType,"ELR");
							RaceTxtBuff <= (my + uvsGameRaceFinish()) < "|" <= /*uvsGameRaceNumber()*/i;
							strcpy(aciCurRaceInfo,RaceTxtBuff.GetBuf());
							aciRacingFlag = 1;
						};
						break;
					case WORLD_GLORX:				
						if(uvsCurrentCycle == 1){
							Active->VangerRaceStatus = VANGER_RACE_PPS;
							RaceTxtBuff.init();
							my = 0;
							s = (StuffObject*)(ItemD.Tail);
							while(s){
								if(s->ActIntBuffer.type == ACI_PIPETKA)
									my += s->ActIntBuffer.data1;
								s = (StuffObject*)(s->NextTypeList);
							};
							WorldSeedNum = my;
							strcpy(aciCurRaceType,"PPS");
							aciRacingFlag = 1;
							i = 0;
							s = (StuffObject*)(Active->DeviceData);
							while(s){
								if(s->ActIntBuffer.type == ACI_PIPETKA)
									i += s->ActIntBuffer.data1;
								s = (StuffObject*)(s->NextDeviceList);
							};
							RaceTxtBuff  <= my < "|" <= i;
							strcpy(aciCurRaceInfo,RaceTxtBuff.GetBuf());
						}else{
							if(uvsCurrentCycle == 2){
								Active->VangerRaceStatus = VANGER_RACE_KRW;
								RaceTxtBuff.init();
								my = 0;
								s = (StuffObject*)(ItemD.Tail);
								while(s){
									if(s->ActIntBuffer.type == ACI_KERNOBOO)
										my += s->ActIntBuffer.data1;
									s = (StuffObject*)(s->NextTypeList);
								};
								WorldSeedNum = my;
								strcpy(aciCurRaceType,"KRW");
								aciRacingFlag = 1;
								i = 0;
								s = (StuffObject*)(Active->DeviceData);
								while(s){
									if(s->ActIntBuffer.type == ACI_KERNOBOO)
										i += s->ActIntBuffer.data1;
									s = (StuffObject*)(s->NextDeviceList);
								};
								RaceTxtBuff <= my < "|" <= i;
								strcpy(aciCurRaceInfo,RaceTxtBuff.GetBuf());
							};
						};				
						break;
					case WORLD_NECROSS:
						if(uvsCurrentCycle == 1){
							i = 0;
							RaceTxtBuff.init();
							p = (VangerUnit*)(ActD.Tail);
							while(p){
		//						if(p->uvsPoint->status == UVS_VANGER_STATUS::RACE_HUNTER){
									BigZek = GetStuffObject(p,ACI_WEEZYK);
									if(BigZek) break;
		//						};
								 p = (VangerUnit*)(p->NextTypeList);
							};

							if(!BigZek){
								BigZek = (StuffObject*)(ItemD.Tail);
								while(BigZek){
									if(BigZek->ActIntBuffer.type == ACI_WEEZYK) break;
									BigZek = (StuffObject*)(BigZek->NextTypeList);
								};
							};

							if(BigZek){
								strcpy(aciCurRaceType,"ZKL");
								if(BigZek->Owner){
									BigZek->Owner->VangerRaceStatus = VANGER_RACE_ZLP;
									my = 1;
									p = (VangerUnit*)(ActD.Tail);
									while(p){
										if(p->VangerRaceStatus == VANGER_RACE_ZLP){
											if(p != BigZek->Owner && p->R_curr.y > BigZek->Owner->R_curr.y) my++;
											i++;
										};
										p = (VangerUnit*)(p->NextTypeList);
									};
									RaceTxtBuff <= my < "|" <= /*uvsGameRaceNumber()*/i;
								}else{
//									RaceTxtBuff < "?|" <= uvsGameRaceNumber();
									my = 1;
									p = (VangerUnit*)(ActD.Tail);
									while(p){
										if(p->VangerRaceStatus == VANGER_RACE_ZLP){
											if(p->R_curr.y > BigZek->R_curr.y) my++;
											i++;
										};
										p = (VangerUnit*)(p->NextTypeList);
									};
									RaceTxtBuff <= my < "?|" <= /*uvsGameRaceNumber()*/i;
								};
								aciRacingFlag = 1;
								strcpy(aciCurRaceInfo,RaceTxtBuff.GetBuf());
							};
						};
						break;
				};
			};
		};
		aciUpdateCurCredits(aiGetHotBug());
//		aciCurCredits = HotBug;
		aciMaxEnergy = (Active->MaxEnergy >> 16) + 1;
		aciMaxArmor = (Active->MaxArmor	>> 16) + 1;
		aciCurEnergy = Active->Energy >> 16;
		aciCurArmor = Active->Armor >> 16;

		aiCutLuck = GamerResult.luck + GamerResult.add_luck;
		aiCutDominance = GamerResult.dominance + GamerResult.add_dominance;
		if(aiCutLuck > 100) aiCutLuck = 100;
		else if(aiCutLuck < 0) aiCutLuck = 0;
		if(aiCutDominance > 100) aiCutDominance = 100;
		else if(aiCutDominance < -100) aiCutDominance = -100;

		aciCurLuck = aiCutLuck;
		aciCurDominance = aiCutDominance;

		aciCurJumpCount = Active->PassageCount;
		aciMaxJumpCount = Active->MaxPassageCount + 1;

		Active->InitEnvironment();
		Active->Quant();
		SignEngine->quant_make_sign();

//		if(NetworkON && (int)(SDL_GetTicks() - FragCnt) > 0){
//			events_out.send_simple_query(PLAYERS_POSITIONS_QUERY);
//			FragCnt = SDL_GetTicks() + 10*1000;
//		};
	};

	ActD.CameraQuant();

	if(Active && Active->ID == ID_VANGER)
		Active -> update_coord(1);

	p = (VangerUnit*)Tail;
	while(p){
//		if(p->ID != ID_VANGER)
//			ErrH.Abort("Bad Vanger List");

		if(!(p->Status & SOBJ_ACTIVE))
			p->InitEnvironment();
		p = (VangerUnit*)(p->NextTypeList);
	};

	NumResolve = 0;
	p = (VangerUnit*)Tail;
	while(p){
//		if(p->ID != ID_VANGER)
//			ErrH.Abort("Bad Vanger List");

		if(!(p->Status & SOBJ_ACTIVE)){
			p->Quant();
			NumResolve += p->aiResolveFind.Num;
			NumResolve += p->aiResolveAttack.Num;
		};
		p = (VangerUnit*)(p->NextTypeList);
	};

	if(NetworkON){
		p = (VangerUnit*)Tail;
		while(p){
			if(p->Status & SOBJ_DISCONNECT){
				if(p->Status & SOBJ_ACTIVE){
					my_player_body.NetID = 0;
					send_player_body(my_player_body);
					NETWORK_OUT_STREAM.delete_object(p->NetID);
					NETWORK_OUT_STREAM.end_body();
					NETWORK_OUT_STREAM.delete_object(p->ShellNetID);
					NETWORK_OUT_STREAM.end_body();
				}else{
					if(p->pNetPlayer){
						p->pNetPlayer->CreatePlayerFlag = 3;
						p->pNetPlayer->uvsPoint = NULL;
						for(i = 0;i < MAX_ACTIVE_SLOT;i++){
							p->pNetPlayer->SlotNetID[i] = 0;
							p->pNetPlayer->SlotStuffNetID[i] = 0;
							p->pNetPlayer->SlotFireCount[i] = 0;
						};
					};
					p->ClearItemList();
					p->uvsPoint->destroy();
				};
				DeleteUnit(p);
				ObjectDestroy(p);
			};
			p = (VangerUnit*)(p->NextTypeList);
		};
	}else{
		p = (VangerUnit*)Tail;
		while(p){
			if(p->Status & SOBJ_DISCONNECT){				
				DeleteUnit(p);
				ObjectDestroy(p);
			};
			p = (VangerUnit*)(p->NextTypeList);
		};
	};
	FunctionQuant();
	PromptQuant();

	if(ThreallMessageProcess){
		if(aciTextEndFlag){
			aciTextEndFlag = 0;
			ThreallText = dgGetThreallPhrase();
			if(ThreallText) aciPrepareText(ThreallText);
		};
	};

/*
//Test CRC
	extern double k_distance_to_force;	
	if(test_block((unsigned char*)&k_distance_to_force,20))
		ErrH.Abort("test");
*/
};

void ActionDispatcher::ChangeLocator(void)
{
	if(LocatorNum){
		if(LocatorOffset == -1)	LocatorOffset = 0;
		else LocatorOffset = (LocatorOffset + 1) % LocatorNum;
		LocatorPoint = LocatorData[LocatorOffset];
	}else{
		LocatorPoint = NULL;
		LocatorOffset = -1;
	};
};

int Object::test_objects_collision()
{
	return 0;
};

void ActionUnit::Hide2Show(void)
{
	 set_3D(SET_3D_CHOOSE_LEVEL,R_curr.x,R_curr.y,R_curr.z + zmax,0,-Angle,Speed);
};

void TrackUnit::Hide2Show(void)
{
/*	if(pBranch){
		if(pNextLink){
			if(pNextLink->Level == 1) set_3D(SET_3D_TO_THE_UPPER_LEVEL ,R_curr.x,R_curr.y,pNextLink->z,radius / 2,-Angle,Speed);
			else{
				if(pNextLink->Level == 0) set_3D(SET_3D_TO_THE_LOWER_LEVEL,R_curr.x,R_curr.y,pNextLink->z,radius / 2,-Angle,Speed);
				else ErrH.Abort("Bad Level");
			};
		}else ErrH.Abort("Bad Link in Hide2Show");
	}else{
		if(pNode){
			if(pNode->Level == 1) set_3D(SET_3D_TO_THE_UPPER_LEVEL ,R_curr.x,R_curr.y,pNode->z,radius / 2,-Angle,Speed);
			else{
				if(pNode->Level == 0) set_3D(SET_3D_TO_THE_LOWER_LEVEL,R_curr.x,R_curr.y,pNode->z,radius / 2,-Angle,Speed);
				else ErrH.Abort("Bad Level");
			};
		}else ErrH.Abort("Bad TrackInfo in Hide2Show");
	};*/

	if(pBranch){
		if(pNextLink) set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,pNextLink->z + zmax,0,-Angle,Speed);
//		else ErrH.Abort("Bad Link in Hide2Show");
	}else{
		if(pNode) set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,pNode->z + zmax,0,-Angle,Speed);
//		else ErrH.Abort("Bad TrackInfo in Hide2Show");
	};
};

void VangerUnit::Hide2Show(void)
{
	if(aiMoveMode == AI_MOVE_TRACK){	
		if(pBranch){
			if(pNextLink) set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,pNextLink->z + zmax,0,-Angle,Speed);
//			else ErrH.Abort("Bad Link in Hide2Show");
		}else{
			if(pNode) set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,pNode->z + zmax,0,-Angle,Speed);
//			else ErrH.Abort("Bad TrackInfo in Hide2Show");
		};
	}else set_3D(SET_3D_CHOOSE_LEVEL,R_curr.x,R_curr.y,R_curr.z + zmax,0,-Angle,Speed);
};


int InsectUnit::test_objects_collision()
{
	int log = 0;	
	BaseObject* p;

	p = (BaseObject*)(ActD.Tail);
	while(p && p != this){
		if(p->Visibility == VISIBLE)
			log += test_object_to_baseobject(p);
		p = (BaseObject*)(p->NextTypeList);
	};
	return 0;
};

int VangerUnit::test_objects_collision()
{
	int log = 0;	

	DangerDataType* st;
	int y0,y1;
	int d,l,i;
	Vector vCheck;
	BaseObject* p;

	if(ExternalMode != EXTERNAL_MODE_NORMAL || !ExternalDraw)
		return 0;
	
	p = (BaseObject*)(BulletD.Tail);
	while(p){
		if(p->Visibility == VISIBLE){
//			if(p->ID != ID_BULLET)
//				ErrH.Abort("Big BUG with memory");
			if(((BulletObject*)(p))->ShowID == BULLET_SHOW_TYPE_ID::CRATER){
				i = radius + p->radius;
				l = getDistY(p->R_curr.y,R_curr.y);
				if(l < i){
					d = getDistX(p->R_curr.x,R_curr.x);
					if(d < i && R_curr.z <= p->R_curr.z + i && (d*d + l*l) < i*i){
						Touch(p);
						p->Touch(this);
					};
				};
			}else log += test_object_to_baseobject(p);
		};
		p = (BaseObject*)(p->NextTypeList);
	};

//	if(mole_on || SensorEnable) return 0;

	if(!mole_on && !SensorEnable){
		p = (BaseObject*)(ActD.Tail);
		while(p && p != this){
			if(p->Visibility == VISIBLE)
				log += test_object_to_baseobject(p);
			p = (BaseObject*)(p->NextTypeList);
		};
	};
	
	if(!NetworkON || (Status & SOBJ_ACTIVE)){
		p = (BaseObject*)(ItemD.Tail);
		while(p){
			if(p->Visibility == VISIBLE && (!mole_on || !(((Object*)(p))->analysis_off)))
				log += test_object_to_baseobject(p);
			p = (BaseObject*)(p->NextTypeList);
		};
	};

	if(mole_on || SensorEnable) return 0;

	p = (BaseObject*)(JumpD.Tail);
	while(p){
		if(p->Visibility == VISIBLE)
			log += test_object_to_baseobject(p);
		p = (BaseObject*)(p->NextTypeList);
	};

	p = (BaseObject*)(FishD.Tail);
	while(p){
		if(p->Visibility == VISIBLE)
			log += test_object_to_baseobject(p);
		p = (BaseObject*)(p->NextTypeList);
	};

	p = (BaseObject*)(HordeSourceD.Tail);
	while(p){
		if(p->Visibility == VISIBLE)
			log += test_object_to_baseobject(p);
		p = (BaseObject*)(p->NextTypeList);
	};

	p = (BaseObject*)(HordeD.Tail);
	while(p){
		if(p->Visibility == VISIBLE)
			log += test_object_to_baseobject(p);
		p = (BaseObject*)(p->NextTypeList);
	};

	p = (BaseObject*)(ClefD.Tail);
	while(p){
		if(p->Visibility == VISIBLE)
			log += test_object_to_baseobject(p);
		p = (BaseObject*)(p->NextTypeList);
	};

	p = (BaseObject*)(DebrisD.Tail);
	while(p){
		if(p->Visibility == VISIBLE)
			log += test_object_to_baseobject(p);
		p = (BaseObject*)(p->NextTypeList);
	};

	p = (BaseObject*)(InsectD.Tail);
	while(p){
		if(p->Visibility == VISIBLE)
			log += test_object_to_baseobject(p);
		p = (BaseObject*)(p->NextTypeList);
	};

/*	n = MapD.Tail;
	while(n){
		if(n->ID == ID_FIELD_SOURCE && ((BaseObject*)(n))->Visibility == VISIBLE)
			log += test_object_to_baseobject((BaseObject*)(n));
		n = n->NextTypeList;
	};*/

	closest_field_object = NULL;
	d = 0xfffffff;
	y0 = R_curr.y - radius - MAX_SENSOR_RADIUS;
	y1 = R_curr.y + radius + MAX_SENSOR_RADIUS;
	i = FindFirstStatic(y0,y1,(StaticObject**)DangerSortedData,DngTableSize);

	if(i > -1){
		do{
			st = DangerSortedData[i];
			if(st->Enable && R_curr.z < st->R_curr.z + radius){
				vCheck.x = getDistX(st->R_curr.x,R_curr.x);
				vCheck.y = getDistY(st->R_curr.y,R_curr.y);
				vCheck.z = 0;
				l = vCheck.vabs();
				if(l < (st->radius + radius) && l < d){
					d = l;
					closest_field_object = st;
				};
			};
			i++;
		}while(i < DngTableSize && DangerSortedData[i]->R_curr.y < y1);
	};

	if(Status & SOBJ_ACTIVE){
		if(closest_field_object){
			switch(closest_field_object->Type){		
				case DangerTypeList::FASTSAND:
				case DangerTypeList::SWAMP:
					if(ActD.Active)
						SOUND_SWAMP(getDistX(ActD.Active->R_curr.x,R_curr.x))
					if(!(closest_field_object->TabuUse)){
						uvsCheckKronIventTabuTask(UVS_KRON_EVENT::SWAMP,1);
						closest_field_object->TabuUse = 1;
					};
					break;
				case DangerTypeList::WHIRLPOOL:	
					if(ActD.Active)
						SOUND_SWAMP(getDistX(ActD.Active->R_curr.x,R_curr.x))
					if(!(closest_field_object->TabuUse)){
						uvsCheckKronIventTabuTask(UVS_KRON_EVENT::WHIRLPOOL,1);
						closest_field_object->TabuUse = 1;
					};
					break;
				case DangerTypeList::TRAIN:					
					break;
			};
		};
	};
	return 0;
};

void ActionDispatcher::keyhandler(int key)
{
	if(Active){
		if(Active->Status & SOBJ_DISCONNECT) Active = NULL;
		else Active->keyhandler(key);
	};
};

void ActionDispatcher::DeleteUnit(ActionUnit* p)
{	
	GameD.DisconnectBaseList(p);
	DisconnectTypeList(p);
	p->Storage->Deactive(p);
};

VangerUnit* ActionDispatcher::GetNextVanger(ActionUnit* p)
{
	VangerUnit* n;
	n = (VangerUnit*)(p->NextTypeList);
	while(1){
		if(!n)	n = (VangerUnit*)(Tail);
		if(n->ID == ID_VANGER) return n;
		n = (VangerUnit*)(n->NextTypeList);
	};
};

// !!! The same params are in the road.cpp
#define SLOPE_MAX	Pi/6
#define MAX_ZOOM	384
#define MAX_ZOOM_16  320
#define MIN_ZOOM	128

double camera_mi;
double camera_miz;
double camera_mit;
double camera_mii;
double camera_mis;

double camera_drag;
double camera_dragz;
double camera_dragt;
double camera_dragi;
double camera_drags;

double camera_vz_min;
double camera_vt_min;
double camera_vs_min;

int camera_turn_impulse = Pi;

int camera_zmin = 400;
int camera_slope_min = 0;
int camera_zmax = 800;
double camera_vmax = 20;

double camera_x=0;
double camera_vx=0;
double camera_y=0;
double camera_vy=0;
double camera_z=0;
double camera_vz=0;
double camera_t=0;
double camera_vt=0;
double camera_vi=0;
double camera_s=0;
double camera_vs=0;
int stop_camera = 0;
int camera_moving_log = 0;
int camera_X_prev;
int camera_Y_prev;

int camera_moving_xy_enable = 0;
int camera_moving_z_enable = 0;
int camera_slope_enable = 0;
int camera_rotate_enable = 0;

int time_vibration = 0;
int max_time_vibration;
double A_vibration;
double alpha_vibration;
double oscillar_vibration;

void start_vibration()
{
	time_vibration = 1;
}
void camera_reset() {
	camera_x=0;
	camera_vx=0;
	camera_y=0;
	camera_vy=0;
	camera_z=0;
	camera_vz=0;
	camera_t=0;
	camera_vt=0;
	camera_vi=0;
	camera_s=0;
	camera_vs=0;
	
	camera_moving_log = 0;

	camera_moving_xy_enable = camera_moving_z_enable = iGetOptionValue(iCAMERA_SCALE);
	camera_slope_enable = iGetOptionValue(iCAMERA_SLOPE);
	camera_rotate_enable = iGetOptionValue(iCAMERA_TURN);
}

void camera_quant(int X,int Y,int Turn,double V_abs) {
	if(stop_camera)
		return;

	int t,dx,dy;

	if(!camera_moving_xy_enable) {
		ViewX = X;
		ViewY = Y;
	}
	dx = getDistX(X,ViewX);
	dy = getDistY(Y,ViewY);
	if(dx > 400 || dy > 400){
		ViewX = X;
		ViewY = Y;
	} else {
		camera_vx += (double)dx*camera_mi * XTCORE_FRAME_NORMAL;
		camera_vx *= camera_drag;
		camera_x += camera_vx * XTCORE_FRAME_NORMAL;
		ViewX += (t = round(camera_x));
		camera_x -= t;

		camera_vy += (double)dy*camera_mi * XTCORE_FRAME_NORMAL;
		camera_vy *= camera_drag;
		camera_y += camera_vy * XTCORE_FRAME_NORMAL;
		ViewY += (t = round(camera_y));
		camera_y -= t;
	}
	if(time_vibration) {
		static int phase_vibration;
		double k = A_vibration*exp(-alpha_vibration*time_vibration);
		double t = oscillar_vibration*(time_vibration + phase_vibration);
		ViewX += round(k*sin(1.276*t)+.8*sin(2.878*t)+0.3*sin(23.876*t)) * XTCORE_FRAME_NORMAL;
		ViewY += round(k*sin(0.981*t)+.8*sin(2.98*t)+0.3*sin(20.82*t)) * XTCORE_FRAME_NORMAL;
		if(time_vibration++ > max_time_vibration) {
			time_vibration = 0;
			phase_vibration = realRND(1367339);
		}
	}

	camera_X_prev = X;
	camera_Y_prev = Y;

	int TurnSecX_old = TurnSecX;
	int z;
	if (camera_moving_z_enable) {
		auto v = V_abs < camera_vmax ? V_abs : camera_vmax;
		auto z_max = curGMap->xsize * 1.38;

		z = camera_zmin + (z_max - camera_zmin) * v / camera_vmax;
	} else {
		z = camera_zmin;
	}

	camera_vz += (double)(z - TurnSecX)*camera_miz * XTCORE_FRAME_NORMAL;
	//camera_vz -= camera_vz*camera_dragz;
	camera_vz *= camera_dragz*pow(0.97,camera_vz_min/(fabs(camera_vz) + 1e-10));
	camera_z += camera_vz * XTCORE_FRAME_NORMAL;
	TurnSecX += (t = round(camera_z));
	camera_z -= t;
	if(TurnSecX > (t = curGMap -> xsize*(RAM16 ? MAX_ZOOM_16 : MAX_ZOOM) >> 8))
		TurnSecX = t;
	if(TurnSecX < (t = curGMap -> xsize*MIN_ZOOM >> 8))
		TurnSecX = t;
	if(RAM16 && (SlopeAngle || TurnAngle) && TurnSecX > curGMap -> xsize)
		TurnSecX = TurnSecX_old;
	if(abs(TurnSecX - curGMap -> xsize) < 4)
		TurnSecX = curGMap -> xsize;

	int s = camera_slope_enable ? (V_abs < camera_vmax ? camera_slope_min + (-SLOPE_MAX - camera_slope_min)*V_abs/camera_vmax : -SLOPE_MAX)
					: camera_slope_min;
	camera_vs += (double)(s - SlopeAngle)*camera_mis * XTCORE_FRAME_NORMAL;
	camera_vs *= camera_drags*pow(0.97,camera_vs_min/(fabs(camera_vs) + 1e-10));
	camera_s += camera_vs * XTCORE_FRAME_NORMAL;
	SlopeAngle += (t = round(camera_s));
	if(SlopeAngle < -SLOPE_MAX)
		SlopeAngle = -SLOPE_MAX;
	camera_s -= t;
	if(RAM16 && (TurnSecX > curGMap -> xsize || TurnAngle) && SlopeAngle)
		SlopeAngle  = 0;
	if(abs(DistPi(SlopeAngle,0)) < 8)
		SlopeAngle = 0;

	
	camera_vt += (double)DistPi(camera_rotate_enable ? Turn : 0,TurnAngle)*camera_mit * XTCORE_FRAME_NORMAL;
	camera_vt *= camera_dragt*pow(0.97,camera_vt_min/(fabs(camera_vt) + 1e-10));
	camera_vi *= camera_dragi*pow(0.97,camera_vt_min/(fabs(camera_vi) + 1e-10));
	camera_t += camera_vt*XTCORE_FRAME_NORMAL + camera_vi;
	TurnAngle += (t = round(camera_t));
	camera_t -= t;
	if(RAM16 && (TurnSecX > curGMap -> xsize || SlopeAngle) && TurnAngle)
		TurnAngle  = 0;
	if(abs(DistPi(TurnAngle,0)) < 8)
		TurnAngle = 0;

	calc_view_factors();
}

void camera_impulse(int amplitude_8)
{
	camera_vt += (camera_turn_impulse - (int)RND(2*camera_turn_impulse))*amplitude_8 >> 8;
}
void camera_quant()
{
	int t, t2;
	camera_x += camera_vx;
	ViewX += (t = round(camera_x)) * XTCORE_FRAME_NORMAL;
	camera_x -= t;

	camera_y += camera_vy;
	ViewY += (t = round(camera_y)) * XTCORE_FRAME_NORMAL;
	camera_y -= t;

	camera_z += camera_vz;
	TurnSecX += (t = round(camera_z)) * XTCORE_FRAME_NORMAL;
	if (TurnSecX < MIN_ZOOM) {
		TurnSecX = MIN_ZOOM;
	}
	camera_z -= t;

	camera_s += camera_vs;
	SlopeAngle += (t = round(camera_s)) * XTCORE_FRAME_NORMAL;
	if(SlopeAngle < -SLOPE_MAX)
		SlopeAngle = -SLOPE_MAX;
	camera_s -= t;

	camera_t += camera_vt;
	TurnAngle += (t = round(camera_t)) * XTCORE_FRAME_NORMAL;
	camera_t -= t;

	calc_view_factors();
}

void camera_direct(int X,int Y,int zoom,int Turn,int Slope,int num_frames)
{
	// Where zoom is between  MIN_ZOOM and MAX_ZOOM
	double time = (double)num_frames * XTCORE_FRAME_NORMAL;
	camera_moving_log = num_frames;
	camera_vx = (double)getDistX(X, ViewX) / time;
	camera_vy = (double)getDistY(Y, ViewY) / time;

	camera_vz = (double)((curGMap -> xsize*zoom >> 8) - TurnSecX)/time;

	camera_vs = (double)(-Slope - SlopeAngle) / time;

	camera_vt += (double)DistPi(Turn,TurnAngle) / time;
}

extern int NewWorldX;
extern int NewWorldY;
//extern XStream fout;
void ActionDispatcher::CameraQuant(void)
{
	if(camera_moving_log-- > 0){
		camera_quant();
		if(!camera_moving_log)
			camera_reset();
//		fout < "camera_quant(): " <= ViewX < "\t" <= ViewY < "\n";
		return;
		}
	if(Active && !(Active->Status & SOBJ_DISCONNECT)){
		int Turn = Active -> psi;
		//if(Active -> traction < 0)
		//	Turn = rPI(Turn + PI);
		double turnf = 1.5 * M_PI - GTOR(TurnAngle);
		double cdx = 0, cdy = 0;
		if (camera_rotate_enable) {
			cdx = -mechosCameraOffsetX * cos(M_PI / 2 - turnf);
			cdy = mechosCameraOffsetX * sin(M_PI / 2 - turnf);
		} else {
			cdx = mechosCameraOffsetX;
			cdy = 0;
		}

		camera_quant(Active->R_curr.x + cdx, Active->R_curr.y + cdy, Turn, Active->V.vabs());
//		fout < "camera_quant(x,y,t,v): " <= ViewX < "\t" <= ViewY < "\n";
		}
}

const int INSECT_RADIUS = 2500;
const int INSECT_RADIUS2 = INSECT_RADIUS * 2;

void InsectUnit::CreateInsect(void)
{
	MaxSpeed = 5;
	MaxHideSpeed = 3;
	Target = R_curr + Vector(INSECT_RADIUS - RND(INSECT_RADIUS2),INSECT_RADIUS - RND(INSECT_RADIUS2),0);
	if(!RND(InsectD.NumInsect[2]*INSECT_PRICE_DATA[2])) BeebType = 2;
	else{
		if(!RND(InsectD.NumInsect[1]*INSECT_PRICE_DATA[1])) BeebType = 1;
		else BeebType = 0;
	};
	InsectD.NumInsect[BeebType]++;
	switch(BeebType){
		case 0:
			set_body_color(COLORS_IDS::MATERIAL_1);
			break;			
		case 1:
			set_body_color(COLORS_IDS::MATERIAL_2);
			break;			
		case 2:
			set_body_color(COLORS_IDS::MATERIAL_4);
			break;
	};
};

void InsectUnit::Init(void)
{
	ID = ID_INSECT;
	ActionUnit::Init(NULL);
};

void InsectUnit::Quant(void)
{
	ActionUnit::Quant();
	int dx,dy;
	int d;

	if(Status & SOBJ_AUTOMAT){
		vDirect.z = 0;

		if(vInsectTarget.y == -1){
			if(ActD.Active) Target = ActD.Active->R_curr;
			else Target = R_curr;
		}else Target = vInsectTarget;

		dx = getDistX(Target.x,R_curr.x);
		dy = getDistY(Target.y,R_curr.y);

/*		if(dx == 0 && dy == 0){
			if(ActD.Active) Target = Vector(ActD.Active->R_curr.x + INSECT_RADIUS - RND(INSECT_RADIUS2),ActD.Active->R_curr.y + INSECT_RADIUS - RND(INSECT_RADIUS2),0);
			else  Target = R_curr + Vector(INSECT_RADIUS - RND(INSECT_RADIUS2),INSECT_RADIUS - RND(INSECT_RADIUS2),0);
			cycleTor(Target.x,Target.y);
		};*/

		d = (int)(sqrt(dx*(double)dx + dy*(double)dy));
		d <<= 2;
		if(d){
			vDirect.x += dx * MaxSpeed / d * XTCORE_FRAME_NORMAL;
			vDirect.y += dy * MaxSpeed / d * XTCORE_FRAME_NORMAL;
		};

		if(NumCalcUnit) vDirect /= NumCalcUnit;

		MoveAngle = vDirect.psi();
		DeltaSpeed = vDirect.vabs();

		if(Visibility == VISIBLE) Action();
		else HideAction();
	};
};

void InsectUnit::InitEnvironment(void)
{
	ActionUnit* p;
	int dx,dy;
	int d,r,f;

	ActionUnit::InitEnvironment();

	if(Visibility == UNVISIBLE) R_curr.z = 80;

	vDirect = Vector(0,0,0);
	NumCalcUnit = 0;

//	if(Visibility != VISIBLE) return;

	if(vInsectTarget.y == -1){
		p = (ActionUnit*)(ActD.Tail);
		while(p){
			dy = getDistY(R_curr.y,p->R_curr.y);
			if(abs(dy) < ViewRadius){
				dx = getDistX(R_curr.x,p->R_curr.x);
				if(abs(dx) < ViewRadius){
					d = (int)(sqrt(dx*(double)dx + dy*(double)dy));
					if(d){
						r = p->radius * 5;
						if(d < r && r > 0){
							f = MaxSpeed - MaxSpeed * d / r;
							vDirect.x += dx * f / d;
							vDirect.y += dy * f / d;
							NumCalcUnit++;
						};
					};
				};
			};
			p = (ActionUnit*)(p->NextTypeList);
		};

		p = (ActionUnit*)(InsectD.Tail);
		while(p){
			if(p != this/* && p->Visibility == VISIBLE*/){			
				dy = getDistY(R_curr.y,p->R_curr.y);
				if(abs(dy) < ViewRadius){
					dx = getDistX(R_curr.x,p->R_curr.x);
					if(abs(dx) < ViewRadius){
						d = (int)(sqrt(dx*(double)dx + dy*(double)dy));
						if(d){
							r = p->radius * 5;
							if(d < r && r > 0){
								f = MaxSpeed - MaxSpeed * d / r;
								vDirect.x += dx * f / d;
								vDirect.y += dy * f / d;
								NumCalcUnit++;
							};
						};
					};
				};
			};
			p = (ActionUnit*)(p->NextTypeList);
		};
	};
};

void InsectUnit::Touch(GeneralObject* p)
{
	switch(p->ID){
		case ID_VANGER:
			if(p->Status & SOBJ_ACTIVE){
				SOUND_BEEB_DEATH(getDistX(ActD.Active->R_curr.x,R_curr.x));
//				ActD.HotBug += INSECT_PRICE_DATA[BeebType];
				aiPutHotBug(aiGetHotBug() + INSECT_PRICE_DATA[BeebType]);
				if(BeebType == 2) uvsCheckKronIventTabuTask(UVS_KRON_EVENT::GOLD_BEEB,1);
				else uvsCheckKronIventTabuTask(UVS_KRON_EVENT::BEEB,1);
				if(NetworkON){
					my_player_body.beebos = aiGetHotBug();
					send_player_body(my_player_body);
				};
			};
			MapD.CreateCrater(R_curr,MAP_POINT_CRATER03,Angle);
			R_curr.x = clip_mask_x/2 - RND(clip_mask_x);
			R_curr.y = clip_mask_y/2 - RND(clip_mask_y);
			cycleTor(R_curr.x,R_curr.y);
			set_3D(SET_3D_CHOOSE_LEVEL,R_curr.x,R_curr.y,R_curr.z,0,-Angle,0);
		case ID_BULLET:
		case ID_JUMPBALL:
			MapD.CreateCrater(R_curr,MAP_POINT_CRATER03,Angle);
			R_curr.x = clip_mask_x/2 - RND(clip_mask_x);
			R_curr.y = clip_mask_y/2 - RND(clip_mask_y);
			cycleTor(R_curr.x,R_curr.y);
			set_3D(SET_3D_CHOOSE_LEVEL,R_curr.x,R_curr.y,R_curr.z,0,-Angle,0);
			break;
	};
};

void VangerUnit::DrawQuant(void)
{
	WaterParticleObject* p;
	int i;

	if(!ExternalDraw || (Status & SOBJ_WAIT_CONFIRMATION)) return;
	TrackUnit::DrawQuant();	
	Vector vCheck;

	if(ExternalMode == EXTERNAL_MODE_NORMAL){
		if(Status & SOBJ_ACTIVE){
			if(!mole_on){
				if(dynamic_state & TOUCH_OF_AIR){
					if(CurrentWorld != WORLD_KHOX) AddOxigenResource();
				}else{
					if(!(dynamic_state & TOUCH_OF_WATER))
						UseOxigenResource();
				};
			};

			if(dynamic_state & TOUCH_OF_WATER){
				if(CurrentWorld == WORLD_THREALL){
					if(NetworkON) BulletCollision((MaxEnergy + MaxArmor) / 150,NULL);
					else BulletCollision((MaxEnergy + MaxArmor) / 20,NULL);
				}else{
					if(!(dynamic_state & TOUCH_OF_AIR)){
						ChargeWeapon(this,ACI_MACHOTINE_GUN_LIGHT,1);
						ChargeWeapon(this,ACI_MACHOTINE_GUN_HEAVY,1);
						UseOxigenResource();
						if(Speed){
							p = (WaterParticleObject*)(EffD.GetObject(EFF_PARTICLE03));
							if(p){
								p->CreateParticle(30,5,1 << 7,radius,5,28,5,R_curr);
								EffD.ConnectObject(p);
							};
						};
					};
				};
			};
		}else{
			if(dynamic_state & TOUCH_OF_WATER){
				if(CurrentWorld == WORLD_THREALL)
					BulletCollision(Energy / 20,NULL);
				else{
					if(Speed){
						p = (WaterParticleObject*)(EffD.GetObject(EFF_PARTICLE03));
						if(p){
							p->CreateParticle(30,5,1 << 7,radius,5,28,5,R_curr);
							EffD.ConnectObject(p);
						};
					};
				};
			};
		};
	};

	PlayerData* pl;
	int cclx,ccly,ccrx,ccry,ccm;

	if(NetworkON && (Status & SOBJ_AUTOMAT)){
		pl = players_list.first();
		while(pl){
			if(uvsPoint == ((uvsVanger*)(pl->uvsPoint)) && pl->name){
				ccm = XGR_Obj.ClipMode;
				XGR_SetClipMode(XGR_CLIP_ALL);
				cclx = XGR_Obj.clipLeft;
				ccly = XGR_Obj.clipTop;
				ccrx = XGR_Obj.clipRight;
				ccry = XGR_Obj.clipBottom;
				XGR_SetClip(UcutLeft,VcutUp,UcutRight,VcutDown);
				aOutText32clip(R_scr.x,R_scr.y,165 | (2 << 16),pl->name,0,0,0);
				XGR_SetClip(cclx,ccly,ccrx,ccry);
				XGR_SetClipMode(ccm);
				break;
			};
			pl = (PlayerData*)pl -> next;
		};
	};

#ifdef UNIT_DEBUG_VIEW

#ifdef TEST_TRACK	

	if(Status & SOBJ_ACTIVE){
		int cc1,cc2;
		cc1 = 111;
		cc2 = 111;

		if(PointStatus & TRK_BRANCH_MASK){
			cc2 = 111;
			if(PointStatus & TRK_IN_MASK) cc1 = 175;
			else cc1 = 111;
		}else{
			cc1 = 111;
			if(PointStatus & TRK_IN_MASK) cc2 = 175;
			else cc2 = 111;
		};

		if(TestLevelFlag){
			if(pNode){
				if(pNode->TrackCount){
					/*line_trace(Vector(pNode->BorderX[0],pNode->BorderY[0],159),Vector(pNode->BorderX[pNode->NumBranch - 1],pNode->BorderY[pNode->NumBranch - 1],159));
					for(i = 1;i < pNode->NumBranch;i++){
						line_trace(Vector(pNode->BorderX[i],pNode->BorderY[i],159),Vector(pNode->BorderX[i - 1],pNode->BorderY[i - 1],159));
						line_trace(Vector(pNode->BorderX[i],pNode->BorderY[i],159),Vector(pNode->x,pNode->y,159));
					};*/
				}else{
					/*line_trace(Vector(pNode->BorderX[0],pNode->BorderY[0],cc2),Vector(pNode->BorderX[pNode->NumBranch - 1],pNode->BorderY[pNode->NumBranch - 1],cc2));
					for(i = 1;i < pNode->NumBranch;i++){
						line_trace(Vector(pNode->BorderX[i],pNode->BorderY[i],cc2),Vector(pNode->BorderX[i - 1],pNode->BorderY[i - 1],cc2));
						line_trace(Vector(pNode->BorderX[i],pNode->BorderY[i],cc2),Vector(pNode->x,pNode->y,cc2));
					};*/
				};
			};

			for(i = 0;i < pBranch->NumLink;i++){
				if(pBranch->Link[i].TrackCount){
					putMapPixel(pBranch->Link[i].xr,pBranch->Link[i].yr,159);
					putMapPixel(pBranch->Link[i].xl,pBranch->Link[i].yl,159);
				}else{
					putMapPixel(pBranch->Link[i].xr,pBranch->Link[i].yr,cc1);
					putMapPixel(pBranch->Link[i].xl,pBranch->Link[i].yl,cc1);
				};
			};
		}else{
			for(i = 0;i < pBranch->NumLink;i++){
				if(pBranch->Link[i].Level){
					putMapPixel(pBranch->Link[i].xr,pBranch->Link[i].yr,175);
					putMapPixel(pBranch->Link[i].xl,pBranch->Link[i].yl,175);
				}else{
					putMapPixel(pBranch->Link[i].xr,pBranch->Link[i].yr,159);
					putMapPixel(pBranch->Link[i].xl,pBranch->Link[i].yl,159);
				};
		//		putMapPixel(pBranch->Link[i].x,pBranch->Link[i].y,175);
			};

			if(pNode){
				if(pNode->Level){
					/*line_trace(Vector(pNode->BorderX[0],pNode->BorderY[0],175),Vector(pNode->BorderX[pNode->NumBranch - 1],pNode->BorderY[pNode->NumBranch - 1],175));
					for(i = 1;i < pNode->NumBranch;i++){
						line_trace(Vector(pNode->BorderX[i],pNode->BorderY[i],175),Vector(pNode->BorderX[i - 1],pNode->BorderY[i - 1],175));
						line_trace(Vector(pNode->BorderX[i],pNode->BorderY[i],175),Vector(pNode->x,pNode->y,175));
					};*/
				}else{
					/*line_trace(Vector(pNode->BorderX[0],pNode->BorderY[0],159),Vector(pNode->BorderX[pNode->NumBranch - 1],pNode->BorderY[pNode->NumBranch - 1],159));
					for(i = 1;i < pNode->NumBranch;i++){
						line_trace(Vector(pNode->BorderX[i],pNode->BorderY[i],159),Vector(pNode->BorderX[i - 1],pNode->BorderY[i - 1],159));
						line_trace(Vector(pNode->BorderX[i],pNode->BorderY[i],159),Vector(pNode->x,pNode->y,159));
					};*/
				};
			};
		};

		//line_trace(Vector(pNextLink->xl,pNextLink->yl,195),Vector(pNextLink->xr,pNextLink->yr,210));
		//line_trace(Vector(pPrevLink->xl,pPrevLink->yl,195),Vector(pPrevLink->xr,pPrevLink->yr,210));
	};

#else
/*	VangerUnit* van;
	if(Status & SOBJ_ACTIVE){
		vCheck = Vector(300,0,0)*RotMat;
		vCheck += R_curr;
		cycleTor(vCheck.x,vCheck.y);
		line_trace(Vector(R_curr.x,R_curr.y,159),Vector(vCheck.x,vCheck.y,159));
		van = (VangerUnit*)(ActD.Tail);	
		while(van){
			if(van->Visibility == VISIBLE && !(van->Status & SOBJ_ACTIVE)){
				if(TouchSphere(R_curr,vCheck,van->R_curr,van->radius,i))
					line_trace(Vector(XCYCL(van->R_curr.x - radius),van->R_curr.y,159),Vector(XCYCL(van->R_curr.x + radius),van->R_curr.y,175));
			};
			van = (VangerUnit*)(van->NextTypeList);
		};
	};*/


	for(i = 0;i < pBranch->NumLink;i++){
		if(pBranch->Link[i].Level){
			putMapPixel(pBranch->Link[i].xr,pBranch->Link[i].yr,175);
			putMapPixel(pBranch->Link[i].xl,pBranch->Link[i].yl,175);
		}else{
			putMapPixel(pBranch->Link[i].xr,pBranch->Link[i].yr,159);
			putMapPixel(pBranch->Link[i].xl,pBranch->Link[i].yl,159);
		};
//		putMapPixel(pBranch->Link[i].x,pBranch->Link[i].y,175);
	};

	if(PointStatus == TRK_IN_NODE){
		if(pNode->Level){
			line_trace(Vector(pNode->BorderX[0],pNode->BorderY[0],175),Vector(pNode->BorderX[pNode->NumBranch - 1],pNode->BorderY[pNode->NumBranch - 1],175));
			for(i = 1;i < pNode->NumBranch;i++){
				line_trace(Vector(pNode->BorderX[i],pNode->BorderY[i],175),Vector(pNode->BorderX[i - 1],pNode->BorderY[i - 1],175));
				line_trace(Vector(pNode->BorderX[i],pNode->BorderY[i],175),Vector(pNode->x,pNode->y,175));
			};
		}else{
			line_trace(Vector(pNode->BorderX[0],pNode->BorderY[0],159),Vector(pNode->BorderX[pNode->NumBranch - 1],pNode->BorderY[pNode->NumBranch - 1],159));
			for(i = 1;i < pNode->NumBranch;i++){
				line_trace(Vector(pNode->BorderX[i],pNode->BorderY[i],159),Vector(pNode->BorderX[i - 1],pNode->BorderY[i - 1],159));
				line_trace(Vector(pNode->BorderX[i],pNode->BorderY[i],159),Vector(pNode->x,pNode->y,159));
			};
		};
	};

	line_trace(Vector(R_curr.x,R_curr.y,170),Vector(R_curr.x + vTarget.x / 2,R_curr.y + vTarget.y / 2,170));

	if(SpeedDir == 0 || (OtherFlag & MECHOS_CALC_WAY)){
		line_trace(Vector(FrontPoint.pNextLink->xl,FrontPoint.pNextLink->yl,183),Vector(FrontPoint.pNextLink->xr,FrontPoint.pNextLink->yr,159));
		line_trace(Vector(FrontPoint.pPrevLink->xl,FrontPoint.pPrevLink->yl,183),Vector(FrontPoint.pPrevLink->xr,FrontPoint.pPrevLink->yr,159));
	}else{
		line_trace(Vector(FrontPoint.pNextLink->xl,FrontPoint.pNextLink->yl,183),Vector(FrontPoint.pNextLink->xr,FrontPoint.pNextLink->yr,183));
		line_trace(Vector(FrontPoint.pPrevLink->xl,FrontPoint.pPrevLink->yl,183),Vector(FrontPoint.pPrevLink->xr,FrontPoint.pPrevLink->yr,183));
	};

	line_trace(Vector(pNextLink->xl,pNextLink->yl,195),Vector(pNextLink->xr,pNextLink->yr,210));
	line_trace(Vector(pPrevLink->xl,pPrevLink->yl,195),Vector(pPrevLink->xr,pPrevLink->yr,210));
#endif

#endif
};

void aci_LocationQuant(void);

void VangerUnit::MapQuant(void)
{	
	if((mole_on == 256 && (dynamic_state & TOUCH_OF_AIR)) || Molerizator)
		MolePoint1->make_first_mole();

	if(MoleInProcess)
		MoleProcessQuant();
	
	if(LastMole == 256 && mole_on != 256){
		MolePoint2->set(R_curr,40,radius < 40 ? radius : 40);
		SensorEnable = 20;
	}else{
		if(SensorEnable > 0)
			SensorEnable--;
	};
	MolePoint2->make_first_mole();
	LastMole = mole_on;
};

void Object::StartMoleProcess(void)
{
};

void VangerUnit::StartMoleProcess(void)
{
	MolePoint1->set(R_curr,40,radius < 40 ? radius : 40);
};


extern int uvsGamerWaitGame;

void VangerUnit::Quant(void)
{
	int cX[4],cY[4];
	int cAlpha;
	Vector vCheck;
	int i;
	StuffObject* p;

	if(Status & SOBJ_WAIT_CONFIRMATION) return;	
	RuffaGunTime++;
	
	SensorQuant();

	UseDeviceMask = 0;
	CoptePoint = NULL;
	SwimPoint = NULL;
	MolePoint = NULL;

	if(VangerRaceStatus != VANGER_RACE_ZLP) VangerRaceStatus = VANGER_RACE_NONE;

	p = DeviceData;
	while(p){
		switch(p->ActIntBuffer.type){
			case ACI_COPTE_RIG:
				if(PowerFlag & VANGER_POWER_FLY && CurrentWorld != WORLD_KHOX){
					UseDeviceMask |= DEVICE_MASK_COPTE;
					CoptePoint = p;
				};
				break;
			case ACI_CUTTE_RIG:
				if(PowerFlag & VANGER_POWER_FLY){
					UseDeviceMask |= DEVICE_MASK_SWIM;
					SwimPoint = p;
				};
				break;
			case ACI_CROT_RIG:
				if(PowerFlag & VANGER_POWER_FLY){
					UseDeviceMask |= DEVICE_MASK_MOLE;
					MolePoint = p;
				};
				break;			
			case ACI_ELEECH:
				if(CurrentWorld == WORLD_FOSTRAL) VangerRaceStatus = VANGER_RACE_ELR;
				break;
			case ACI_KERNOBOO:
				if(CurrentWorld == WORLD_GLORX) VangerRaceStatus = VANGER_RACE_KRW;
				break;
			case ACI_PIPETKA:
				if(CurrentWorld == WORLD_GLORX)VangerRaceStatus = VANGER_RACE_PPS;
				break;
			case ACI_WEEZYK:
				if(CurrentWorld == WORLD_NECROSS) VangerRaceStatus = VANGER_RACE_ZLP;
				break;
			case ACI_PROTRACTOR:
				ActD.pfActive = this;
				break;
			case ACI_MECHANIC_MESSIAH:
				ActD.mfActive = this;
				break;			
		};
		p = p->NextDeviceList;
	};	

	if((Status & SOBJ_ACTIVE) && CurrentWorld == WORLD_NECROSS && uvsCurrentCycle == 1 && uvsGamerWaitGame)
		VangerRaceStatus = VANGER_RACE_ZLP;

	if(uvsPoint->status == UVS_VANGER_STATUS::RACE_HUNTER)
		VangerRaceStatus = VANGER_RACE_ZLP;

	TrackUnit::Quant();
	
	if(Status & SOBJ_ACTIVE){
		switch(ActD.FlyTaskEnable){
			case 0:
				if(ActD.FlyTaskDirect){
					if(R_curr.y >= ActD.FlyTaskFirstY){
						if(!(dynamic_state & TOUCH_OF_GROUND)) ActD.FlyTaskEnable = 1;
						else ActD.FlyTaskEnable = 2;
					};
				}else{
					if(R_curr.y <= ActD.FlyTaskFirstY){					
						if(!(dynamic_state & TOUCH_OF_GROUND)) ActD.FlyTaskEnable = 1;
						else ActD.FlyTaskEnable = 2;
					};
				};
				break;
			case 1:
				if(dynamic_state & TOUCH_OF_GROUND)
					ActD.FlyTaskEnable = 2;
				else{
					if(ActD.FlyTaskDirect){
						if(R_curr.y > ActD.FlyTaskLastY){
							uvsCheckKronIventTabuTask(UVS_KRON_EVENT::FLY_ONLINE,0);
							ActD.FlyTaskEnable = 2;
						};				
					}else{
						if(R_curr.y < ActD.FlyTaskLastY){
							uvsCheckKronIventTabuTask(UVS_KRON_EVENT::FLY_ONLINE,0);
							ActD.FlyTaskEnable = 2;
						};
					};
				};
				break;
			case 2:
				if(ActD.FlyTaskDirect){
					if(R_curr.y <= ActD.FlyTaskFirstY)
						ActD.FlyTaskEnable = 0;
				}else{
					if(R_curr.y >= ActD.FlyTaskFirstY) 
						ActD.FlyTaskEnable = 0;
				};
				break;
		};
	};

	if(NetworkON && !(Status & SOBJ_ACTIVE)){
/*		if(aciWorldIndex != -1){
			uvsPoint -> Pworld = WorldTable[aciWorldIndex];
			aciWorldIndex = -1;
			EffD.CreateRingOfLord(EFF_PARTICLE06,R_curr + Vector(0,0,80),radius*2,200,111,111,radius << 7);
			ExternalLock = 1;
			ExternalObject = NULL;

			ExternalMode = EXTERNAL_MODE_FREE_IN;
			ExternalTime = ROTOR_PROCESS_LIFE_TIME;
			CreateParticleRotor(R_curr,83);
			ExternalDraw = 0;
			switch_analysis(1);
		};*/
		TerrainQuant();

		CheckPosition++;
		if(ActiveGetBranch()){
			if(CheckPosition > MaxCheckPosition){
				CheckPosition = 0;
				HideTrack.GetPosition(this);
			};
		};

		if(PointStatus & TRK_BRANCH_MASK){
			if(MoveDir){
				if(pNode != pBranch->pBeg && pNode != pBranch->pEnd){
					if(pBranch->pBeg) pNode = pBranch->pBeg;
					else{
						if(pBranch->pEnd) pNode = pBranch->pEnd;
						else ErrH.Abort("Bad Link to Node");
					};
				};
			}else{
				if(pNode != pBranch->pBeg && pNode != pBranch->pEnd){
					if(pBranch->pEnd) pNode = pBranch->pEnd;
					else{
						if(pBranch->pBeg) pNode = pBranch->pBeg;
						else ErrH.Abort("Bad Link to Node");
					};
				};
			};
		}else{
			if(pBranch->pBeg != pNode && pBranch->pEnd != pNode){
				pBranch = pNode->pBranches[0];
				if(pBranch->pBeg == pNode){
					pPrevLink = pBranch->Link;
					pNextLink = pPrevLink + 1;
				}else{
					pNextLink = pBranch->Link + pBranch->NumLink - 1;
					pPrevLink = pNextLink - 1;
				};
			};
		};

		GetTrackVector();
		if(CurrentWorld == WORLD_GLORX && !NetworkON){
			if(NullTime > 80){
				cAlpha = RND(PI*2);
				for(i = 0;i < 4;i++){
					vCheck = Vector(radius*2,0,0)*DBM(cAlpha,Z_AXIS);
					cX[i] = R_curr.x + vCheck.x;
					cY[i] = R_curr.y + vCheck.y;
					cAlpha -= PI/2 + PI/8 - RND(PI/6);
				};
				MapD.CreateLandSlide(cX,cY,50);
				NullTime = -NullTime;
			};
		};
	}else{
		if(Status & SOBJ_AUTOMAT){
			if(Visibility == VISIBLE){
				TerrainQuant();
				if(aiMoveMode == AI_MOVE_POINT){
					CheckPosition++;
					if(ActiveGetBranch()){
						if(CheckPosition > MaxCheckPosition){
							CheckPosition = 0;
							HideTrack.GetPosition(this);
						};
					};
				}else GetBranch();
				GetTrackVector();
				if(aiActionID != AI_ACTION_BRAKE) TargetAnalysis();
				else SpeedDir = 0;
				Action();			
			}else{
	//			HideGetBranch();
				if(aiMoveMode == AI_MOVE_POINT){
					CheckPosition++;
					if(ActiveGetBranch()){
						if(CheckPosition > MaxCheckPosition){
							CheckPosition = 0;
							HideTrack.GetPosition(this);
						};
					};
				}else GetBranch();
				if(aiActionID != AI_ACTION_BRAKE) TargetAnalysis();
				else SpeedDir = 0;
				HideAction();
			};
		}else{
			if(aciWorldIndex != -1){
				if(!(Status & SOBJ_AUTOMAT)){
					uvsPoint -> Pworld = WorldTable[aciWorldIndex];				
					EffD.CreateRingOfLord(EFF_PARTICLE06,R_curr + Vector(0,0,80),radius*2,200,111,111,radius << 7);
		//			ExternalMode = EXTERNAL_MODE_SIGN_IN;
		//			ExternalTime = 40;
					ExternalLock = 1;
					ExternalObject = NULL;

					ExternalMode = EXTERNAL_MODE_FREE_IN;
					ExternalTime = ROTOR_PROCESS_LIFE_TIME;
					CreateParticleRotor(R_curr,83);
					PalCD.Set(CPAL_HIDE_PASSAGE,ExternalTime);
					ExternalDraw = 0;
					switch_analysis(1);
					StopCDTRACK();
					NetFunction83Time = NetGlobalTime;
					ShellUpdateFlag = 1;
				};
				aciWorldIndex = -1;
			};

			if(aciTeleportEvent != 0){
				if(!(Status & SOBJ_AUTOMAT)){
					ExternalTime = CHANGE_VANGER_TIME;
					ExternalLock = 1;
					ExternalObject = NULL;
					ExternalMode = EXTERNAL_MODE_IN_VANGER;
					vSetVangerFlag = aciTeleportEvent - TELEPORT_ESCAVE_ID;
		//			Go2Universe();
		//			Status |= SOBJ_DISCONNECT;					
					PalCD.Set(CPAL_HIDE_PASSAGE,ExternalTime);
					switch_analysis(1);				
					for(i = 0;i < 5;i++)
						EffD.CreateParticleGenerator(R_curr,R_curr,Vector(5,0,0)*DBM((int)(RND(PI*2)),Z_AXIS));
					SOUND_BOOT_START();
				};
				aciTeleportEvent = 0;
			};

			if(ExternalMode == EXTERNAL_MODE_NORMAL && ActD.PassageTouchEnable > 0)
				ActD.PassageTouchEnable--;

			TerrainQuant();

			CheckPosition++;
			if(ActiveGetBranch()){
				if(CheckPosition > MaxCheckPosition){
					CheckPosition = 0;
					HideTrack.GetPosition(this);
				};
			};

			if(PointStatus & TRK_BRANCH_MASK){
				if(MoveDir){
					if(pNode != pBranch->pBeg && pNode != pBranch->pEnd){
						if(pBranch->pBeg) pNode = pBranch->pBeg;
						else{
							if(pBranch->pEnd) pNode = pBranch->pEnd;
							else ErrH.Abort("Bad Link to Node");
						};
					};
				}else{
					if(pNode != pBranch->pBeg && pNode != pBranch->pEnd){
						if(pBranch->pEnd) pNode = pBranch->pEnd;
						else{
							if(pBranch->pBeg) pNode = pBranch->pBeg;
							else ErrH.Abort("Bad Link to Node");
						};
					};
				};
			}else{
				if(pBranch->pBeg != pNode && pBranch->pEnd != pNode){
					pBranch = pNode->pBranches[0];
					if(pBranch->pBeg == pNode){
						pPrevLink = pBranch->Link;
						pNextLink = pPrevLink + 1;
					}else{
						pNextLink = pBranch->Link + pBranch->NumLink - 1;
						pPrevLink = pNextLink - 1;
					};
				};
			};
			GetTrackVector();
			if(CurrentWorld == WORLD_GLORX && !NetworkON){
				if(NullTime > 80){
					cAlpha = RND(PI*2);
					for(i = 0;i < 4;i++){
						vCheck = Vector(radius*2,0,0)*DBM(cAlpha,Z_AXIS);
						cX[i] = R_curr.x + vCheck.x;
						cY[i] = R_curr.y + vCheck.y;
						//cycleTor(cX[i],cY[i]);
						cAlpha -= PI/2 + PI/8 - RND(PI/6);
					};
					/*cX[2] = cX[1] + cX[3] - cX[0];
					cY[2] = cY[1] + cY[3] - cY[0];*/
					MapD.CreateLandSlide(cX,cY,50);
					NullTime = -NullTime;
				};
			};
		};
	};
	
	if(!NetworkON && CurrentWorld == WORLD_KHOX && ExternalMode == EXTERNAL_MODE_NORMAL && !PalIterLock && PalCD.PalEnable){
		if(KhoxPoison < 0){
			UseOxigenResource();
		}else{
			for(i = 0;i < 128*3;i++)
				palbufOrg[i] = palbuf[i] = (uchar)(KhoxPoison*(int)(palbufSrc[i]) / MaxKhoxPoison);
			 if(CheckPointCount < MAX_KHOX_CHECKPOINT) KhoxPoison -= 3 << 6;
		};
	};	

	ItemQuant();
	if(uvsPoint) uvsPoint->update(R_curr.x,R_curr.y);

	if(NetworkON && (Status & SOBJ_ACTIVE)){
		if(my_server_data.GameType == PASSEMBLOSS && UsedCheckNum >= GloryPlaceNum && ExternalMode == EXTERNAL_MODE_NORMAL){
			ExternalLock = 1;
			ExternalObject = NULL;

			ExternalMode = EXTERNAL_MODE_FREE_IN;
			ExternalTime = ROTOR_PROCESS_LIFE_TIME;
			CreateParticleRotor(R_curr,83);
			PalCD.Set(CPAL_HIDE_PASSAGE,ExternalTime);
			ExternalDraw = 0;
			switch_analysis(1);
			StopCDTRACK();
			NetFunction83Time = NetGlobalTime;
			ShellUpdateFlag = 1;
		};

//		static unsigned int last_send_set_position = 0;
//		static prevViewY = 0; 
//		if(abs(getDistY(prevViewY,ViewY)) > 10 && IS_PAST(last_send_set_position + average_lag)){
//			NETWORK_OUT_STREAM.set_position(ViewX,ViewY,round(2.*(fabs(curGMap -> xsize*sinTurnInvFlt) + fabs(curGMap -> ysize*cosTurnInvFlt))*0.5));
//			prevViewY = ViewY;
//			last_send_set_position = SDL_GetTicks();
//			}

		static int need_to_send_vanger = 0;
		if(prev_controls != current_controls || dynamic_state & ITS_MOVING || IS_PAST(last_send_time + 2000))
			need_to_send_vanger = 1;
		if(need_to_send_vanger && IS_PAST(last_send_time + average_lag)){
			last_send_time = SDL_GetTicks();
			NETWORK_OUT_STREAM.update_object(NetID,R_curr.x,R_curr.y);
			int scr_size = round((fabs(curGMap -> xsize*sinTurnInvFlt) + fabs(curGMap -> ysize*cosTurnInvFlt))*.5);
			NETWORK_OUT_STREAM < (unsigned char)(scr_size < 255 ? scr_size : 255);
			Send();
			NETWORK_OUT_STREAM.end_body();
			need_to_send_vanger = 0;

			if(lag_averaging_t0.tell() < 20){
				//zmod
				z_time_init();
				NETWORK_OUT_STREAM.begin_event(SERVER_TIME_QUERY);
				NETWORK_OUT_STREAM.end_body();
				lag_averaging_t0.put(SDL_GetTicks());
				}
			}	
	
		if(abs(Energy - NetworkEnergy) > (MaxEnergy >> 4)){
			NetworkEnergy = Energy;
			ShellUpdateFlag = 1;
		}else{
			if(Armor == 0 || abs(Armor - NetworkArmor) > (MaxArmor >> 4)){
				NetworkArmor = Armor;
				ShellUpdateFlag = 1;
			};
		};

		if(VangerChanger){
			if(VangerChanger->VangerChanger){
				if(VangerChanger->VangerChanger != this){
					VangerChanger = NULL;
					VangerChangerCount = 0;
					NetChanger = 0;
					ShellUpdateFlag = 1;
				}else{
					if(NetChanger == GET_STATION(VangerChanger->NetID) && VangerChanger->NetChanger == GlobalStationID && VangerChangerCount == 2)
						VangerChangerCount = 1;
				};
			}else{
				if(NetChanger == 0xff){
					VangerChanger = NULL;
					VangerChangerCount = 0;
					NetChanger = 0;
					ShellUpdateFlag = 1;
				};
			};		
		};

/*		if(VangerChanger && VangerChanger->VangerChanger){
			if(VangerChanger->VangerChanger != this){
				VangerChanger = NULL;
				VangerChangerCount = 0;
				NetChanger = 0;
				ShellUpdateFlag = 1;
			}else{
				if(VangerChangerCount == 2)
					VangerChangerCount = 1;
			};
		};*/

		if(ShellUpdateFlag){
			ShellUpdateFlag = 0;
			NETWORK_OUT_STREAM.update_object(ShellNetID,0,0);
			ShellUpdate();
			NETWORK_OUT_STREAM.end_body();
		};
	};
};

void VangerUnit::ShellUpdate(void)
{
	unsigned char t;

/*	t = 0;
	if(ActD.NetActiveProtractor){
		t |= ((ActD.NetProtractor & 7) - 1) | 64;
		ActD.NetActiveProtractor = 0;
	};

	if(ActD.NetActiveMessiah){
		t |= (((ActD.NetMechMessiah & 7) - 1) << 3) | 128;
		ActD.NetActiveMessiah = 0;
	};

	NETWORK_OUT_STREAM < (unsigned char)(t);*/

	NetFunction = (ActD.NetFunctionProtractor | ActD.NetFunctionMessiah);
	NETWORK_OUT_STREAM < (unsigned char)(NetFunction);
	
	t = (unsigned char)((15 * Energy / MaxEnergy) | ((15 * Armor / MaxArmor) << 4));
	if(Armor > 0) t |= 16;

	NETWORK_OUT_STREAM < (unsigned char)(t);
	NETWORK_OUT_STREAM < (unsigned char)(NetDestroyID);
	NETWORK_OUT_STREAM < (unsigned char)(NetChanger);
	NETWORK_OUT_STREAM < (unsigned int)(NetRuffaGunTime);
	NETWORK_OUT_STREAM < (unsigned int)(NetProtractorFunctionTime);
	NETWORK_OUT_STREAM < (unsigned int)(NetMessiahFunctionTime);
	NETWORK_OUT_STREAM < (unsigned int)(NetFunction83Time);
};

void VangerUnit::TerrainQuant(void)
{
	int i,y0,y1;
	Vector vCheck;
	StaticObject* st;
	int l;

	if(CurrentWorld == 2){
		if(check_terrain_type(7)){
			y0 = R_curr.y - EXPLOSION_BARELL_RADIUS;
			y1 = R_curr.y + EXPLOSION_BARELL_RADIUS;
			i = FindFirstStatic(y0,y1,(StaticObject**)TntSortedData,TntTableSize);

			if(i > -1){
				do{
					st = TntSortedData[i];
					if(st->Enable){
						vCheck.x = getDistX(st->R_curr.x,R_curr.x);
//						l = radius + st->radius;
						l = radius + st->radius;
						if(abs(vCheck.x) < l){
							vCheck.y = getDistY(st->R_curr.y,R_curr.y);
							vCheck.z = st->R_curr.z - R_curr.z;
							if(vCheck.abs2() < l*l){
								st->Touch(this);
								Touch(st);
							};
						};
					};
					i++;
				}while(i < TntTableSize && TntSortedData[i]->R_curr.y < y1);
			};
		};
	};
};

void VangerUnit::InitEnvironment(void)
{
	StaticObject* st;
	int y0,y1;
	int l,i;

	Vector vTrack;
	Vector vOffset;	
	Vector vCheck;
	GunSlot* s;
	BulletObject* g;

	aiUnitResolve* p;
	aiUnitResolve* pp;

	if(Status & SOBJ_WAIT_CONFIRMATION) return;
	nDoorFlag = DoorFlag;
	ActionUnit::InitEnvironment();
	vTarget = Vector(0,0,0);
	NumCalcUnit = 0;

	if(NetworkON){
		if(VangerChanger){			
			if(Status & SOBJ_ACTIVE){
				switch(VangerChangerCount){
					case 3:
						OutCarNator();
						VangerChangerCount = 2;
						break;
					case 1:
						if(NetChanger == 0xff){
							if(VangerChanger->NetChanger == 0xff){
								VangerChanger->VangerChanger = NULL;
								VangerChanger->VangerChangerCount = 0;
								VangerChanger = NULL;
								VangerChangerCount = 0;
								NetChanger = 0;
								ShellUpdateFlag = 1;
							};
						}else{
							ChangeVangerProcess();
							NetChanger = 0xff;
							ShellUpdateFlag = 1;
						};
						break;
				};
			};
		};
		
		if(pNetPlayer && pNetPlayer->body.CarIndex != uvsPoint->Pmechos->type && !(Status & SOBJ_ACTIVE))
			ChangeVangerProcess();
	}else{
		if(VangerChanger && VangerChangerCount > 0){
			VangerChangerCount--;
			switch(VangerChangerCount){
				case 1:
					OutCarNator();
					break;
				case 0:
					ChangeVangerProcess();
					VangerChanger = NULL;
					break;
			};		
		};
	};

	if(!NetworkON || (Status & SOBJ_ACTIVE)){
		ExternalLastSensor = ExternalSensor;
		ExternalSensor = NULL;

		if(Visibility == VISIBLE){
			if(Status & SOBJ_AUTOMAT){
				if(aiActionID == AI_ACTION_BRAKE){
					y0 = R_curr.y - radius - MAX_SENSOR_RADIUS;
					y1 = R_curr.y + radius + MAX_SENSOR_RADIUS;
					i = FindFirstStatic(y0,y1,(StaticObject**)SensorSortedData,SnsTableSize);

					if(i > -1){
						do{
							st = SensorSortedData[i];
							vCheck.x = getDistX(st->R_curr.x,R_curr.x);
							l = radius + st->radius;
							if(abs(vCheck.x) < l){
								if(R_curr.z > ((SensorDataType*)(st))->z0 - radius && R_curr.z < ((SensorDataType*)(st))->z1 + radius){
									vCheck.y = getDistY(st->R_curr.y,R_curr.y);
									if((vCheck.x*vCheck.x + vCheck.y*vCheck.y) < l*l)
										StopTouchSensor((SensorDataType*)st);
								};
							};
							i++;
						}while(i < SnsTableSize && SensorSortedData[i]->R_curr.y < y1);
					};

					if(TouchKeyObjectFlag)	TouchKeyObject = NULL;
					else TouchKeyObjectFlag = 1;
				}else{
					ResolveGenerator();
					
					NoWayHandler();

					p = (aiUnitResolve*)(aiResolveFind.Tail);
					while(p){
						if(!(p->rStatus & AI_RESOLVE_STATUS_DISCONNECT))
							ResolveHandlerFind(p);
						p = (aiUnitResolve*)(p->Next);				
					};

					p = (aiUnitResolve*)(aiResolveAttack.Tail);
					while(p){
						if(!(p->rStatus & AI_RESOLVE_STATUS_DISCONNECT))
							ResolveHandlerAttack(p);
						p = (aiUnitResolve*)(p->Next);
					};

					p = (aiUnitResolve*)(aiResolveFind.Tail);
					while(p){
						pp = (aiUnitResolve*)(p->Next);
						if(p->rStatus & AI_RESOLVE_STATUS_DISCONNECT){
							if(p == aiLocalTarget) aiLocalTarget = NULL;
							aiResolveFind.Disconnect(p);
							delete p;
						};
						p = pp;
					};

					p = (aiUnitResolve*)(aiResolveAttack.Tail);
					while(p){
						pp = (aiUnitResolve*)(p->Next);
						if(p->rStatus & AI_RESOLVE_STATUS_DISCONNECT){
							if(p == aiLocalTarget) aiLocalTarget = NULL;
							aiResolveAttack.Disconnect(p);
							delete p;
						};
						p = pp;
					};

					if(!mole_on && !SensorEnable){
						y0 = R_curr.y - radius - MAX_SENSOR_RADIUS;
						y1 = R_curr.y + radius + MAX_SENSOR_RADIUS;
						i = FindFirstStatic(y0,y1,(StaticObject**)SensorSortedData,SnsTableSize);

						if(i > -1){
							do{
								st = SensorSortedData[i];
								vCheck.x = getDistX(st->R_curr.x,R_curr.x);
								l = radius + st->radius;
								if(abs(vCheck.x) < l){
									if(R_curr.z > ((SensorDataType*)(st))->z0 - radius && R_curr.z < ((SensorDataType*)(st))->z1 + radius){
										vCheck.y = getDistY(st->R_curr.y,R_curr.y);
										if((vCheck.x*vCheck.x + vCheck.y*vCheck.y) < l*l)
											AutomaticTouchSensor((SensorDataType*)st);
									};
								};
								i++;
							}while(i < SnsTableSize && SensorSortedData[i]->R_curr.y < y1);
						};
					};

					if(TouchKeyObjectFlag)	TouchKeyObject = NULL;
					else TouchKeyObjectFlag = 1;

					if(aiStatus & AI_STATUS_TNT){
						if(PowerFlag & VANGER_POWER_RUFFA_GUN){
							if(RuffaGunTime > (RUFFA_GUN_WAIT * WeaponWaitTime >> 8)){
								RuffaGunTime = 0;
								g = BulletD.CreateBullet();
								vCheck = Vector(64,0,0)*RotMat;
								g->CreateBullet(Vector(R_curr.x,R_curr.y,R_curr.z),
									Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),NULL,&GameBulletData[WD_BULLET_RUFFA],this,Speed);
								if(ActD.Active)
									SOUND_RAFFA_SHOT(getDistX(ActD.Active->R_curr.x,R_curr.x));
							};
						}else{
							s = NULL;
							l = -1;

							for(i = 0;i < ItemMatrix->NumSlot;i++){
								if(GunSlotData[ItemMatrix->nSlot[i]].pData && (GunSlotData[ItemMatrix->nSlot[i]].pData->BulletMode & BULLET_CONTROL_MODE::FLY) && (GunSlotData[ItemMatrix->nSlot[i]].pData->Power < l || l == -1)){
									s = &GunSlotData[ItemMatrix->nSlot[i]];
									l = GunSlotData[ItemMatrix->nSlot[i]].pData->Power;
								};
							};
							if(s) s->Fire();
						};
					};
				};
			}else{
				y0 = R_curr.y - radius - MAX_SENSOR_RADIUS;
				y1 = R_curr.y + radius + MAX_SENSOR_RADIUS;

				if(mole_on) i = -1;
				else i = FindFirstStatic(y0,y1,(StaticObject**)SensorSortedData,SnsTableSize);

				switch(CurrentWorld){
					case WORLD_HMOK:
						if(ActD.hMokKeyEnable && i > -1){
							do{
								st = SensorSortedData[i];
								if(st->Enable){
									vCheck.x = getDistX(st->R_curr.x,R_curr.x);
									l = radius + st->radius;
									if(abs(vCheck.x) < l){
										if(R_curr.z  > ((SensorDataType*)(st))->z0 - radius && R_curr.z < ((SensorDataType*)(st))->z1 + radius){
											vCheck.y = getDistY(st->R_curr.y,R_curr.y);
											if((vCheck.x*vCheck.x + vCheck.y*vCheck.y) < l*l){
												st->Touch(this);
												TouchSensor((SensorDataType*)st);
											};
										};
									};
								};
								i++;
							}while(i < SnsTableSize && SensorSortedData[i]->R_curr.y < y1);
						};
						break;
					case WORLD_KHOX:
						if(i > -1){
							do{
								st = SensorSortedData[i];
								if(st->Enable){
									vCheck.x = getDistX(st->R_curr.x,R_curr.x);
									l = radius + st->radius;
									if(abs(vCheck.x) < l){
										if(R_curr.z > ((SensorDataType*)(st))->z0 - radius && R_curr.z  < ((SensorDataType*)(st))->z1 + radius){
											if(!strcmp(((SensorDataType*)(st))->Name,"UniDoor")){
												if(CheckPointCount >= MAX_KHOX_CHECKPOINT){
													vCheck.y = getDistY(st->R_curr.y,R_curr.y);
													if((vCheck.x*vCheck.x + vCheck.y*vCheck.y) < l*l){
														st->Touch(this);
														TouchSensor((SensorDataType*)st);
													};
												};
											}else{
												vCheck.y = getDistY(st->R_curr.y,R_curr.y);
												if((vCheck.x*vCheck.x + vCheck.y*vCheck.y) < l*l){
													st->Touch(this);
													TouchSensor((SensorDataType*)st);
												};
											};
										};
									};
								};
								i++;
							}while(i < SnsTableSize && SensorSortedData[i]->R_curr.y < y1);
						};
						break;
					case WORLD_XPLO:
						ActD.SpobsEntrance = 0;
						if(i > -1){
							do{
								st = SensorSortedData[i];
								if(st->Enable){
									vCheck.x = getDistX(st->R_curr.x,R_curr.x);
									l = radius + st->radius;
									if(abs(vCheck.x) < l){
										if(R_curr.z > ((SensorDataType*)(st))->z0 - radius && R_curr.z < ((SensorDataType*)(st))->z1 + radius){
											if(!strcmp(((SensorDataType*)(st))->Name,"SpotDoor1") || !strcmp(((SensorDataType*)(st))->Name,"SpotDoor2")){
												if(ActD.XploKeyEnable){
													vCheck.y = getDistY(st->R_curr.y,R_curr.y);
													if((vCheck.x*vCheck.x + vCheck.y*vCheck.y) < l*l){
														st->Touch(this);
														TouchSensor((SensorDataType*)st);
														ActD.SpobsEntrance = 1;
													};
												};
												aiMessageQueue.Send(AI_MESSAGE_SPOBS,Speed);//aiMessageData[AI_MESSAGE_SPOBS].Send(Speed);
											}else{
												 if(strcmp(((SensorDataType*)(st))->Name,"Spot1") || (!(ActD.FunctionSpobsDestroyActive) && !(ActD.SpobsDestroy) && ActD.XploKeyEnable)){
													vCheck.y = getDistY(st->R_curr.y,R_curr.y);
													if((vCheck.x*vCheck.x + vCheck.y*vCheck.y) < l*l){
														if(((SensorDataType*)(st))->data6) aiMessageTouch(Speed,((SensorDataType*)(st))->data6 - 1);
														st->Touch(this);
														TouchSensor((SensorDataType*)st);
													};
												};
											};
										};
									};
								};
								i++;
							}while(i < SnsTableSize && SensorSortedData[i]->R_curr.y < y1);
						};

						if(ActD.SpobsEntrance){
							if(!ActD.CameraModifier){
								camera_direct(1650,820,1 << 8,0,0,20);
								ActD.CameraModifier = 20;
							}else{								
								if(ActD.CameraModifier == 1){
									camera_direct(1650,820,1 << 8,0,0,1);
								}else ActD.CameraModifier--;
							};
						}else{
							if(ActD.CameraModifier){
								camera_direct(R_curr.x,R_curr.y,1 << 8,0,0,20);
								ActD.CameraModifier = 0;
							};
						};
						break;
					case WORLD_THREALL:						
						if(i > -1){
							do{
								st = SensorSortedData[i];
								if(st->Enable){
									vCheck.x = getDistX(st->R_curr.x,R_curr.x);
									l = radius + st->radius;
									if(abs(vCheck.x) < l){
										if(R_curr.z > ((SensorDataType*)(st))->z0 - radius && R_curr.z < ((SensorDataType*)(st))->z1 + radius){
											if(!strcmp(((SensorDataType*)(st))->Name,"SIGN")){
												if(!NetworkON && ActD.Active && ActD.Active->ExternalMode == EXTERNAL_MODE_NORMAL && ActD.Active->ExternalDraw && !ActD.ThreallDestroy){
													vCheck.y = getDistY(st->R_curr.y,R_curr.y);
													if((vCheck.x*vCheck.x + vCheck.y*vCheck.y) < l*l){
														st->Touch(this);
														TouchSensor((SensorDataType*)st);
													};
												};
											}else{
												vCheck.y = getDistY(st->R_curr.y,R_curr.y);
												if((vCheck.x*vCheck.x + vCheck.y*vCheck.y) < l*l){
													if(((SensorDataType*)(st))->data6) aiMessageTouch(Speed,((SensorDataType*)(st))->data6 - 1);
													st->Touch(this);
													TouchSensor((SensorDataType*)st);
												};
											};
										};
									};
								};
								i++;
							}while(i < SnsTableSize && SensorSortedData[i]->R_curr.y < y1);
						};
						break;
					default:
						if(i > -1){
							do{
								st = SensorSortedData[i];
								vCheck.x = getDistX(st->R_curr.x,R_curr.x);
								l = radius + st->radius;
								if(abs(vCheck.x) < l){
									if(R_curr.z > ((SensorDataType*)(st))->z0 - radius && R_curr.z < ((SensorDataType*)(st))->z1 + radius){
										vCheck.y = getDistY(st->R_curr.y,R_curr.y);
										if((vCheck.x*vCheck.x + vCheck.y*vCheck.y) < l*l){
											if(((SensorDataType*)(st))->data6) aiMessageTouch(Speed,((SensorDataType*)(st))->data6 - 1);
											if(st->Enable){											
												st->Touch(this);
												TouchSensor((SensorDataType*)st);
											};
										};
									};
								};
								i++;
							}while(i < SnsTableSize && SensorSortedData[i]->R_curr.y < y1);
						};
						break;
				};
			};
		}else{
			if(aiActionID != AI_ACTION_BRAKE){
				ResolveGenerator();
				
				NoWayHandler();

				p = (aiUnitResolve*)(aiResolveFind.Tail);
				while(p){
					if(!(p->rStatus & AI_RESOLVE_STATUS_DISCONNECT))
						ResolveHandlerFind(p);
					p = (aiUnitResolve*)(p->Next);				
				};

				p = (aiUnitResolve*)(aiResolveAttack.Tail);
				while(p){
					if(!(p->rStatus & AI_RESOLVE_STATUS_DISCONNECT))
						ResolveHandlerAttack(p);
					p = (aiUnitResolve*)(p->Next);
				};

				p = (aiUnitResolve*)(aiResolveFind.Tail);
				while(p){
					pp = (aiUnitResolve*)(p->Next);
					if(p->rStatus & AI_RESOLVE_STATUS_DISCONNECT){
						if(p == aiLocalTarget) aiLocalTarget = NULL;
						aiResolveFind.Disconnect(p);				
						delete p;
					};
					p = pp;
				};

				p = (aiUnitResolve*)(aiResolveAttack.Tail);
				while(p){
					pp = (aiUnitResolve*)(p->Next);
					if(p->rStatus & AI_RESOLVE_STATUS_DISCONNECT){
						if(p == aiLocalTarget) aiLocalTarget = NULL;
						aiResolveAttack.Disconnect(p);
						delete p;
					};
					p = pp;
				};
			};

			y0 = R_curr.y - radius - MAX_SENSOR_RADIUS;
			y1 = R_curr.y + radius + MAX_SENSOR_RADIUS;
			i = FindFirstStatic(y0,y1,(StaticObject**)KeySensorSorted,NumKeySensor);

			if(i > -1){
				do{
					st = KeySensorSorted[i];
					if(st->Enable){
						vCheck.x = getDistX(st->R_curr.x,R_curr.x);
						l = radius + st->radius;
						if(abs(vCheck.x) < l){
							vCheck.y = getDistY(st->R_curr.y,R_curr.y);
							if((vCheck.x*vCheck.x + vCheck.y*vCheck.y) < l*l){
								st->Touch(this);
								if(ExternalSensor){
									if(ExternalSensor == ExternalObject && ExternalObject != st) ExternalSensor = (SensorDataType*)(st);
								}else ExternalSensor = (SensorDataType*)(st);
							};
						};
					};
					i++;
				}while(i < NumKeySensor && KeySensorSorted[i]->R_curr.y < y1);
			};
		};
	}else{
		if(Visibility == VISIBLE){
			ActD.NumVisibleVanger++;

			y0 = R_curr.y - radius - MAX_SENSOR_RADIUS;
			y1 = R_curr.y + radius + MAX_SENSOR_RADIUS;
			i = FindFirstStatic(y0,y1,(StaticObject**)SensorSortedData,SnsTableSize);

			if(i > -1){
				do{
					st = SensorSortedData[i];
					if(st->Enable){
						vCheck.x = getDistX(st->R_curr.x,R_curr.x);
						l = radius + st->radius;
						if(abs(vCheck.x) < l){
							if(R_curr.z > ((SensorDataType*)(st))->z0 - radius && R_curr.z < ((SensorDataType*)(st))->z1 + radius){
								vCheck.y = getDistY(st->R_curr.y,R_curr.y);
								if((vCheck.x*vCheck.x + vCheck.y*vCheck.y) < l*l)
									st->Touch(this);
							};
						};
					};
					i++;
				}while(i < SnsTableSize && SensorSortedData[i]->R_curr.y < y1);
			};
		};
	};

/*	if(Visibility == VISIBLE){
		p = (ActionUnit*)(ActD.Tail);
		if(Status & SOBJ_AUTOMAT){
			while(p){
				if(p != this && p->Visibility == VISIBLE){
					dy = getDistY(p->R_curr.y,R_curr.y);
					if(abs(dy) < ViewRadius){
						dx = getDistX(p->R_curr.x,R_curr.x);
						if(abs(dx) < ViewRadius){
							switch(p->ID){
								case ID_VANGER:
									r = (int)(sqrt(dx*(double)dx + dy*(double)dy));
									if(r > 0){
										PointDist(-dx,-dy,R_curr.z - p->R_curr.z,r,2*radius + p->radius,1,vUp,vTarget);
										NumCalcUnit++;
									};
									break;
							};
						};
					};
				};
				p = (ActionUnit*)(p->NextTypeList);
			};
		};
	};*/
};

void VangerUnit::AutomaticTouchSensor(SensorDataType* p) //znfo !!!
{
	int etype;
	DoorEngine* d;

	if(p->Enable){
		p->Touch(this);
		switch(p->SensorType){
			case SensorTypeList::SPOT:
			case SensorTypeList::ESCAVE:
			case SensorTypeList::PASSAGE:
			case SensorTypeList::TRAIN:
			case SensorTypeList::TRAP:
				if(!(Status & SOBJ_ACTIVE)){
					if(ExternalSensor){
						if(ExternalSensor == ExternalObject && ExternalObject != p) ExternalSensor = p;
					}else ExternalSensor = p;
				};
				break;
			case SensorTypeList::IMPULSE:
				impulse(p->vData,p->Power,0);				
				break;
			case SensorTypeList::SENSOR:
				if(!(Status & SOBJ_ACTIVE)){
					if(p->Owner){
						if(p->Owner->Type == EngineTypeList::DOOR){
							d = (DoorEngine*)(p->Owner);
							if(d){
								if(aiLocalTarget && aiLocalTarget->Type == UNIT_ORDER_ENTER && FindDoor(d,etype) == (aiLocalTarget->Obj).EnterT){
									switch(etype){
										case EngineTypeList::IMPULSE_ESCAVE:
										case EngineTypeList::ELEVATOR:
											if(d->Mode == EngineModeList::WAIT && !d->ProcessFlag)
												d->OpenDoor();
											break;
										case EngineTypeList::ESCAVE:
											if(TouchKeyObject && TouchKeyObject->SensorType == SensorTypeList::ESCAVE && d->Mode == EngineModeList::OPEN && !d->ProcessFlag)
												d->CloseDoor();
											break;
									};
								};
							};
						};
					};
				};
				break;
			case SensorTypeList::ARMOR_UPDATE:
				Armor = MaxArmor;
				LowArmor = 0;
				break;
			case SensorTypeList::FIRE_UPDATE:				
				ChargeWeapon(this,ACI_GHORB_GEAR_LIGHT,1);
				ChargeWeapon(this,ACI_GHORB_GEAR_HEAVY,1);			
				break;
		};
	}else{
		if(p->SensorType == SensorTypeList::SPOT || p->SensorType == SensorTypeList::ESCAVE || p->SensorType == SensorTypeList::PASSAGE){
			TouchKeyObject = p;
			TouchKeyObjectFlag = 0;
		};
	};
};


void VangerUnit::StopTouchSensor(SensorDataType* p)
{
	if(p->Enable){
		p->Touch(this);
		switch(p->SensorType){
			case SensorTypeList::SPOT:
			case SensorTypeList::ESCAVE:
			case SensorTypeList::PASSAGE:
			case SensorTypeList::TRAIN:
			case SensorTypeList::TRAP:
				if(ExternalSensor){
					if(ExternalSensor == ExternalObject && ExternalObject != p) ExternalSensor = p;
				}else ExternalSensor = p;
				break;
			case SensorTypeList::IMPULSE:
				impulse(p->vData,p->Power,0);
				break;
		};
	}else{
		if(p->SensorType == SensorTypeList::SPOT || p->SensorType == SensorTypeList::ESCAVE || p->SensorType == SensorTypeList::PASSAGE){
			TouchKeyObject = p;
			TouchKeyObjectFlag = 0;
		};
	};
};

void VangerUnit::Touch(GeneralObject* p)
{
	if(p->ID == ID_VANGER && (!NetworkON || (Status & SOBJ_ACTIVE))){
		PUT_GLOBAL_EVENT(AI_EVENT_COLLISION,ID_VANGER,p,this);
		if(BeebonationFlag){
			Energy = 0;
			Armor = 0;
			ExternalDraw = 0;
		};
	};
};

void VangerUnit::TouchSensor(SensorDataType* p)
{
	DoorEngine* d;
	Vector vCheck;
	TiristorEngine* t;
	int i,s,r_log;
	switch(p->SensorType){
		case SensorTypeList::PASSAGE:
		case SensorTypeList::EARTH_PASSAGE:
		case SensorTypeList::ESCAVE:		
		case SensorTypeList::SPOT:
		case SensorTypeList::TRAP:
		case SensorTypeList::TRAIN:
			if(ActD.PassageTouchEnable) ActD.PassageTouchEnable = MAX_PASSAGE_DELAY;
			if(!BeebonationFlag){
				if(ExternalSensor){
					if(ExternalSensor == ExternalObject && ExternalObject != p) ExternalSensor = p;
				}else ExternalSensor = p;
			};
			break;
		case SensorTypeList::IMPULSE:
//			impulse(Vector(32 - RND(64),32 - RND(64),RND(64)),20,0);
			impulse(p->vData,p->Power,0);
			if(abs(PrevImpuseFrame - frame) > 5){
				SOUND_KIDPUSH();
				PrevImpuseFrame = frame;
			};
			break;
		case SensorTypeList::ARMOR_UPDATE:
			if(Armor < MaxArmor){
				Armor = MaxArmor;
				aiMessageQueue.Send(AI_MESSAGE_ARMOR,Speed,2,0);//aiMessageData[AI_MESSAGE_ARMOR].Send(Speed,2,0);
				SOUND_CHARGE_REPARE();
			};
			aiMessageQueue.Send(AI_MESSAGE_ARMOR,Speed,1);//aiMessageData[AI_MESSAGE_ARMOR].Send(Speed,1);
//			if(Armor < (MaxArmor - MaxArmor / 32)){
//				aiMessageData[AI_MESSAGE_ARMOR].Send(Speed,2,0);
//				Armor += MaxArmor / 32;
//				SOUND_CHARGE();
//			};
//			aiMessageData[AI_MESSAGE_ARMOR].Send(Speed,1);
			break;
		case SensorTypeList::RANDOMIZE_UPDATE:
			aiMessageQueue.Send(AI_MESSAGE_RANDOM_UPDATE,Speed,1);//aiMessageData[AI_MESSAGE_RANDOM_UPDATE].Send(Speed,1);
			if(RandomUpdate != frame - 1){
				r_log = 0;
				while(!r_log){
					switch(RND(6)){
						case 0:
							s = ChargeWeapon(this,ACI_GHORB_GEAR_LIGHT,1);
							s += ChargeWeapon(this,ACI_GHORB_GEAR_HEAVY,1);
							if(s) r_log = 1;
							break;
						case 1:
							s = ChargeWeapon(this,ACI_GHORB_GEAR_LIGHT,0);
							s += ChargeWeapon(this,ACI_GHORB_GEAR_HEAVY,0);
							if(s) r_log = 1;
							break;
						case 2:						
							if(Energy < MaxEnergy - 1){
								aiMessageQueue.Send(AI_MESSAGE_ENERGY,Speed,1,0);//aiMessageData[AI_MESSAGE_ENERGY].Send(Speed,1,0);
								Energy = MaxEnergy - 1;
								r_log = 1;
								uvsCheckKronIventTabuTask(UVS_KRON_EVENT::RECHARGE_SHEILD,1);
								SOUND_CHARGE_ENERGE();
							};	
							break;
						case 3:
							if(Energy > 0){
								aiMessageQueue.Send(AI_MESSAGE_DENERGY,Speed,1,0);//aiMessageData[AI_MESSAGE_DENERGY].Send(Speed,1,0);
								Energy = 0;								
								r_log = 1;
								SOUND_DISCHARGE();
							};
							break;
						case 4:
							s = ChargeDevice(this,ACI_EMPTY_COPTE_RIG,1);			
							s += ChargeDevice(this,ACI_EMPTY_CROT_RIG,1);
							s += ChargeDevice(this,ACI_EMPTY_CUTTE_RIG,1);
							s += ChargeDevice(this,ACI_COPTE_RIG,1);
							s += ChargeDevice(this,ACI_CROT_RIG,1);
							s += ChargeDevice(this,ACI_CUTTE_RIG,1);
							if(s)  r_log = 1;
							break;
						case 5:
//							s = ChargeDevice(this,ACI_EMPTY_COPTE_RIG,0);
//							s += ChargeDevice(this,ACI_EMPTY_CROT_RIG,0);
//							s += ChargeDevice(this,ACI_EMPTY_CUTTE_RIG,0);
							s = ChargeDevice(this,ACI_COPTE_RIG,0);
							s += ChargeDevice(this,ACI_CROT_RIG,0);
							s += ChargeDevice(this,ACI_CUTTE_RIG,0);
							if(s) r_log = 1;
							break;
					};
				};
			};
			RandomUpdate = frame;
			break;
		case SensorTypeList::OXIGEN_UPDATE:
			if(p->data5 == CheckPointCount){
				KhoxPoison = MaxKhoxPoison;
				p->Enable = 0;
				MapD.CreateCrater(p->R_curr,MAP_POINT_CRATER12);
				CheckPointCount++;
				OxigenResource = MaxOxigenResource;
				SOUND_CHECKPOINT() 
			};
			break;
		case SensorTypeList::FLY_UPDATE:
			aiMessageQueue.Send(AI_MESSAGE_COPTER,Speed,1);//aiMessageData[AI_MESSAGE_COPTER].Send(Speed,1);
			if(RandomUpdate != frame - 1){	
				ChargeDevice(this,ACI_EMPTY_COPTE_RIG,1);			
				ChargeDevice(this,ACI_EMPTY_CROT_RIG,1);
				ChargeDevice(this,ACI_EMPTY_CUTTE_RIG,1);
				ChargeDevice(this,ACI_COPTE_RIG,1);
				ChargeDevice(this,ACI_CROT_RIG,1);
				ChargeDevice(this,ACI_CUTTE_RIG,1);
				RandomUpdate = frame;
			};
			break;
		case SensorTypeList::FIRE_UPDATE:			
			aiMessageQueue.Send(AI_MESSAGE_GHORB,Speed,1);//aiMessageData[AI_MESSAGE_GHORB].Send(Speed,1);
			ChargeWeapon(this,ACI_GHORB_GEAR_LIGHT,1);
			ChargeWeapon(this,ACI_GHORB_GEAR_HEAVY,1);
//			ActD.AddFireResource();
			break;
		case SensorTypeList::KEY_UPDATE:
			aiMessageQueue.Send(AI_MESSAGE_SPIRAL,Speed,1);//aiMessageData[AI_MESSAGE_SPIRAL].Send(Speed,1);
			if(PassageCount < MaxPassageCount){
				aiMessageQueue.Send(AI_MESSAGE_SPIRAL,Speed,2,0);//aiMessageData[AI_MESSAGE_SPIRAL].Send(Speed,2,0);
				aciWorldLinksON();
				PassageCount = MaxPassageCount;
				SOUND_CHARGE_SPIRAL();
			};
			break;
		case SensorTypeList::SENSOR:
			if(DoorFlag){
				if(p->Owner){
					if(p->Owner->Type == EngineTypeList::DOOR){
						d = (DoorEngine*)(p->Owner);
						if(d->Luck > 0){
							if((int)(RND(d->Luck)) <= aiCutLuck){
								DoorFlag = 0;
								if(d->Mode == EngineModeList::OPEN)
									d->CloseDoor();
								else
									d->OpenDoor();
							}else{
								if(d->Luck > 0){
									for(i = 0;i < d->NumSensor;i++)
										d->SensorLink[i]->Enable = 0;
								};
							};
						}else{
							if(d->Luck >= -aiCutLuck){
								DoorFlag = 0;
								if(d->Mode == EngineModeList::OPEN)
									d->CloseDoor();
								else
									d->OpenDoor();
							};
						};
					}else{
						if(p->Owner->Type == EngineTypeList::TIRISTOR){
							DoorFlag = 0;
							t = (TiristorEngine*)(p->Owner);
							if(t->Luck > 0){
								if((int)(RND(t->Luck)) <= aiCutLuck){									
									t->OpenDoor();
								}else{
									if(t->Luck > 0){
										for(i = 0;i < t->NumSensor;i++)
											t->SensorLink[i]->Enable = 0;
									};
								};
							}else{
							 	if(t->Luck >= -aiCutLuck){	
									if(t->Mode != EngineModeList::OPEN){
										t->OpenDoor();
									};
								};
							};
						};
					};
				};
			};
			break;
	};
};

void VangerUnit::SensorQuant(void)
{
	Vector vCheck,vTrack;
	int d,a,l;
	int i;

#ifdef TEST_TRACK
	double ddz,dda,ddb;
	DBV ddv;
#endif

	uvsWorld* w1;
	uvsWorld* w2;

	if(!ExternalLock && ExternalLastSensor != ExternalSensor){
		if(ExternalSensor){
			switch(ExternalSensor->SensorType){
				case SensorTypeList::SPOT:
					if(Status & SOBJ_AUTOMAT){
						if(aiLocalTarget && aiLocalTarget->Type == UNIT_ORDER_ENTER && (aiLocalTarget->Obj).EnterT == ((EnterEngine*)(ExternalSensor->Owner))->Owner){
							ExternalMode = EXTERNAL_MODE_SPOT_IN;
							ExternalLock = 1;
							if(Visibility == VISIBLE){
								ExternalTime = ExternalSensor->Owner->DeactiveTime;
//								SOUND_ENTRANCE();
							}else ExternalTime = 0;
							ExternalObject = ExternalSensor;
							switch_analysis(1);
						};
					}else{
						ExternalMode = EXTERNAL_MODE_SPOT_IN;
						ExternalLock = 1;
						if(Visibility == VISIBLE){
//							SOUND_ENTRANCE();
							ExternalTime = ExternalSensor->Owner->DeactiveTime;
						}else ExternalTime = 0;
						ExternalObject = ExternalSensor;
						switch_analysis(1);						
//						aciSendEvent2actint(ACI_LOCK_INTERFACE,NULL);
					};
					break;
				case SensorTypeList::ESCAVE:
					if(Status & SOBJ_AUTOMAT){
						if(aiLocalTarget && aiLocalTarget->Type == UNIT_ORDER_ENTER && (aiLocalTarget->Obj).EnterT == ((EnterEngine*)(ExternalSensor->Owner))->Owner){
							if(Visibility == VISIBLE){
								ExternalTime = ExternalSensor->Owner->DeactiveTime;
//								SOUND_ENTRANCE();
							}else ExternalTime = 0;
							ExternalMode = EXTERNAL_MODE_ESCAVE_IN;
							ExternalLock = 1;
							ExternalObject = ExternalSensor;
							switch_analysis(1);
						};
					}else{
						if(Visibility == VISIBLE){
							ExternalTime = ExternalSensor->Owner->DeactiveTime;
//							SOUND_ENTRANCE();
						}else ExternalTime = 0;
						ExternalMode = EXTERNAL_MODE_ESCAVE_IN;
						ExternalLock = 1;
						ExternalObject = ExternalSensor;
						switch_analysis(1);
//						aciSendEvent2actint(ACI_LOCK_INTERFACE,NULL);
					};
					break;
				case SensorTypeList::PASSAGE:
					if(Status & SOBJ_AUTOMAT){
						if(aiLocalTarget && aiLocalTarget->Type == UNIT_ORDER_PASSAGE && ((aiLocalTarget->Obj).PassageT)->ActionLink == ExternalSensor){

//							if(MainOrderID != UVS_TARGET::PASSAGE)
//								ErrH.Abort("Bad Target Sesion");
//							SensorDebugFlag |= 4;

							ExternalMode = EXTERNAL_MODE_PASS_IN;
							if(Visibility == VISIBLE){
								ExternalTime = ExternalSensor->Owner->DeactiveTime;
								CreateParticleRotor(ExternalSensor->R_curr,ExternalSensor->radius);
								if(Status & SOBJ_ACTIVE){
									camera_direct(ExternalSensor->R_curr.x,ExternalSensor->R_curr.y,1 << 7,0,0,ExternalTime + 1);
//									StartHidePassage(ExternalTime);
									PalCD.Set(CPAL_HIDE_PASSAGE,ExternalTime);
								};
							}else ExternalTime = 0;

							ExternalLock = 1;
							ExternalDraw = 0;
							ExternalObject = ExternalSensor;
							switch_analysis(1);
						};
					}else{
						if(aciWorldLinkExist(((PassageEngine*)(ExternalSensor->Owner))->uvsPort->Pworld->gIndex,((PassageEngine*)(ExternalSensor->Owner))->uvsPort->Poutput->gIndex)){
							if(!ActD.PassageTouchEnable){
								PassageCount--;
								aciSendEvent2actint(ACI_LOCK_INTERFACE,NULL);
								ExternalMode = EXTERNAL_MODE_PASS_IN;
								if(Visibility == VISIBLE){
									ExternalTime = ExternalSensor->Owner->DeactiveTime;
									CreateParticleRotor(ExternalSensor->R_curr,ExternalSensor->radius);
									if(Status & SOBJ_ACTIVE){
										camera_direct(ExternalSensor->R_curr.x,ExternalSensor->R_curr.y,1 << 7,0,0,ExternalTime + 1);
										PalCD.Set(CPAL_HIDE_PASSAGE,ExternalTime);
									};
								}else ExternalTime = 0;
								ExternalLock = 1;
								ExternalDraw = 0;
								ExternalObject = ExternalSensor;
								switch_analysis(1);
								StopCDTRACK();

								NetFunction83Time = NetGlobalTime;
								ShellUpdateFlag = 1;
								if(CurrentWorld == WORLD_ARKONOY && (PowerFlag & VANGER_POWER_RUFFA_GUN))
									uvsCheckKronIventTabuTask(UVS_KRON_EVENT::ARK_NOY,0);
							};
						}else{
							impulse(Vector(32 - RND(64),32 - RND(64),64),30,0);
							SOUND_FAILEDPASS() 
						};
					};
					break;
				case SensorTypeList::TRAIN:
				case SensorTypeList::TRAP:
					if(ExternalSensor->Index == 1){
						if(((TrainEngine*)(ExternalSensor->Owner))->LockFlag & DOOR_CLOSE_LOCK) break;
						ExternalObject = ((TrainEngine*)(ExternalSensor->Owner))->TrainLink[0];
					}else{
						if(((TrainEngine*)(ExternalSensor->Owner))->LockFlag & DOOR_OPEN_LOCK) break;
						ExternalObject = ((TrainEngine*)(ExternalSensor->Owner))->TrainLink[1];
					};
					ExternalMode = EXTERNAL_MODE_MOVE;
					ExternalLock = 1;
					ExternalTime = ExternalSensor->Owner->ActiveTime;
					ExternalTime2 = ExternalSensor->Owner->DeactiveTime;
					ExternalDraw = 1;
					switch_analysis(1);					
					break;
				case SensorTypeList::EARTH_PASSAGE:
					if(GamerResult.earth_unable && aciCurJumpCount > 0){
						ExternalMode = EXTERNAL_MODE_EARTH_IN;
						aciSendEvent2actint(ACI_LOCK_INTERFACE,NULL);
						if(Visibility == VISIBLE){
							ExternalTime = ROTOR_PROCESS_LIFE_TIME;
							CreateParticleRotor(ExternalSensor->R_curr,ExternalSensor->radius);
							if(Status & SOBJ_ACTIVE){
								camera_direct(ExternalSensor->R_curr.x,ExternalSensor->R_curr.y,1 << 7,0,0,ExternalTime + 1);
								PalCD.Set(CPAL_HIDE_PASSAGE,ExternalTime);
		//						StartHidePassage(ExternalTime);
							};
						}else ExternalTime = 0;
						ExternalLock = 1;
						ExternalDraw = 0;
						ExternalObject = ExternalSensor;
						switch_analysis(1);
						StopCDTRACK();
					}else impulse(Vector(32 - RND(64),32 - RND(64),RND(64)),RND(30),RND(10));					
					break;				
			};
		};
	};

	switch(ExternalMode){
#ifdef TEST_TRACK
		case EXTERNAL_MODE_TEST:
			vCheck = Vector(getDistX(pNextLink->x,pPrevLink->x),pNextLink->y - pPrevLink->y,0);
			if(Speed < 0/* || (!(dynamic_state & TRACTION_WHEEL_TOUCH) && !(dynamic_state & STEER_WHEEL_TOUCH))*/)
				break;
			if(PointStatus == TRK_IN_NODE){
				pNode->TrackCount++;
				pNode->Level = test_whether_there_is_not_something_above();
			}else{
				if(PointStatus == TRK_IN_BRANCH){
					if(pPrevLink != TestPrevLink){
						TestLinkTime = 1;
						TestPrevLink = pPrevLink;
					}else TestLinkTime++;

					pNextLink->TrackCount++;
					pPrevLink->TrackCount++;
					section_prepare(pPrevLink->xl,pPrevLink->yl,pNextLink->xl,pNextLink->yl,pNextLink->xr,pNextLink->yr,pPrevLink->xr,pPrevLink->yr,R_curr.z,
					dda,ddb,ddz,ddv);

					pNextLink->z += (int)(ddz);
					pPrevLink->z += (int)(ddz);
					pNextLink->Speed += Speed;
					pPrevLink->Speed += Speed;
					pNextLink->z >>= 1;
					pPrevLink->z >>= 1;
					pNextLink->Speed >>= 1;
					pPrevLink->Speed >>= 1;
					pNextLink->Level = test_whether_there_is_not_something_above();
					pPrevLink->Level = pNextLink->Level;

					pPrevLink->Time = TestLinkTime;
//					pPrevLink->Status = CurrentLinkStatus;
//					pNextLink->Status = CurrentLinkStatus;
				};
			};
			break;
		case EXTERNAL_MODE_TEST_MOVE:
			if(ExternalTime <= 0){
				ExternalMode = EXTERNAL_MODE_TEST;
				switch_analysis(0);
				ExternalLock = 0;
				ExternalDraw = 1;
				vCheck = Vector(getDistX(pNextLink->x,pPrevLink->x),pNextLink->y - pPrevLink->y,0);
				set_3D(SET_3D_CHOOSE_LEVEL,vGoTest.x,vGoTest.y,vGoTest.z,radius / 2,-vCheck.psi(),0);
			}else{
				vCheck = Vector(getDistX(vGoTest.x,R_curr.x),vGoTest.y - R_curr.y,vGoTest.z - R_curr.z);
				d = vCheck.vabs();
				l = d / ExternalTime;
				vCheck *= l;
				vCheck /= d;
				R_curr += vCheck;
				cycleTor(R_curr.x,R_curr.y);
				set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,0,Speed);
				ExternalTime--;
			};
			break;
#endif
		case EXTERNAL_MODE_MOVE:
			if(ExternalTime <= 0){
				ExternalMode = EXTERNAL_MODE_NORMAL;
				ExternalLock = 0;
				ExternalDraw = 1;
				switch_analysis(0);
				if(Status & SOBJ_ACTIVE && !(((TrainEngine*)(ExternalObject->Owner))->TabuUse)){
					uvsCheckKronIventTabuTask(UVS_KRON_EVENT::CRYPT,1);
					((TrainEngine*)(ExternalObject->Owner))->TabuUse = 1;
				};
				ExternalSensor = ExternalObject;
			}else{
				vCheck = Vector(getDistX(ExternalObject->R_curr.x,R_curr.x),ExternalObject->R_curr.y - R_curr.y,ExternalObject->R_curr.z - R_curr.z);
				d = vCheck.vabs();
				a = vCheck.psi();
				l = d / ExternalTime;
				if(Speed < 0) Speed = 0;
				if(ExternalTime2 > 0){
					Speed += (l - Speed) / ExternalTime2;
					Angle = rPI(Angle + (a - Angle) / ExternalTime2);
					ExternalTime2--;
				}else{
					Speed = l;
					Angle = a;
				};
				vCheck *= Speed;
				vCheck /= d;
				R_curr += vCheck;
				cycleTor(R_curr.x,R_curr.y);
				if(Visibility == VISIBLE) set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,-Angle,Speed);
				ExternalTime--;
			};
			break;
		case EXTERNAL_MODE_TRANSPORTER:
			if(ExternalTime <= 0){
				ExternalMode = EXTERNAL_MODE_NORMAL;
				ExternalLock = 0;
				ExternalDraw = 1;
				switch_analysis(0);
				ExternalSensor = ExternalObject;
			}else{
				vCheck = Vector(getDistX(ExternalObject->R_curr.x,R_curr.x),ExternalObject->R_curr.y - R_curr.y,ExternalObject->R_curr.z - R_curr.z);
				d = vCheck.vabs();
				a = vCheck.psi();
				l = d / ExternalTime;
				if(Speed < 0) Speed = 0;
				if(ExternalTime2 > 0){
					Speed += (l - Speed) / ExternalTime2;
					Angle = rPI(Angle + (a - Angle) / ExternalTime2);
					ExternalTime2--;
				}else{
					Speed = l;
					Angle = a;
				};
				vCheck *= Speed;
				vCheck /= d;
				R_curr += vCheck;
				cycleTor(R_curr.x,R_curr.y);
				if(Visibility == VISIBLE) set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,-Angle,Speed);
				ExternalTime--;
			};
			break;
		case EXTERNAL_MODE_PASS_OUT:
			ExternalTime--;
			if(Visibility == VISIBLE && ActD.Active) SOUND_PASSAGE(getDistX(ActD.Active->R_curr.x,R_curr.x));
			if(ExternalTime <= 0){
				ExternalMode = EXTERNAL_MODE_NORMAL;
				ExternalLock = 0;
				ExternalDraw = 1;
				switch_analysis(0);				
				ExternalSensor = ExternalObject;
				if(Status & SOBJ_ACTIVE)
					aciSendEvent2actint(ACI_UNLOCK_INTERFACE,NULL);
			};
			break;
		case EXTERNAL_MODE_PASS_IMPULSE:
			ExternalTime--;
			if(Visibility == VISIBLE && ActD.Active) SOUND_PASSAGE(getDistX(ActD.Active->R_curr.x,R_curr.x));
			if(ExternalTime == EXTERNAL_IMPULSE_TIME){				
				ExternalDraw = 1;
				switch_analysis(0);
				if(Status & SOBJ_ACTIVE)
					aciSendEvent2actint(ACI_UNLOCK_INTERFACE,NULL);
			}else{
				if(ExternalTime <= 0){
					ExternalMode = EXTERNAL_MODE_NORMAL;
					ExternalLock = 0;
					ExternalSensor = ExternalObject;
				}else{
					if(ExternalTime < EXTERNAL_IMPULSE_TIME)
						impulse(ExternalAngle,8 * ExternalTime / EXTERNAL_IMPULSE_TIME);
				};
			};
			break;
		case EXTERNAL_MODE_ESCAVE_OUT:
			ExternalTime--;
			if(ExternalTime <= 0){
				ExternalMode = EXTERNAL_MODE_NORMAL;
				ExternalLock = 0;
				switch_analysis(0);
//				set_3D(SET_3D_CHOOSE_LEVEL,R_curr.x,R_curr.y,R_curr.z,0,-Angle,0);
				ExternalSensor = ExternalObject;
//				if(Status & SOBJ_ACTIVE)
//					aciSendEvent2actint(ACI_UNLOCK_INTERFACE,NULL);
			};
			break;
		case EXTERNAL_MODE_SPOT_OUT:
			ExternalTime--;
			if(ExternalTime <= 0){
				ExternalMode = EXTERNAL_MODE_NORMAL;
				ExternalLock = 0;
				switch_analysis(0);
				ExternalSensor = ExternalObject;
//				if(Status & SOBJ_ACTIVE)
//					aciSendEvent2actint(ACI_UNLOCK_INTERFACE,NULL);
			};
			break;
		case EXTERNAL_MODE_SPOT_IN:
			ExternalTime--;
			if(ExternalTime <= 0){
				ExternalMode = EXTERNAL_MODE_NORMAL;
				ExternalLock = 0;
				uvsPoint -> Pspot = (uvsSpot*)(((EnterEngine*)(ExternalObject->Owner))->Owner->Owner);
				uvsPoint -> Pescave = NULL;
				Go2Universe();
				uvsPoint -> Event(UVS_EVENT::SPOT_ARRIVAL);
				Status |= SOBJ_DISCONNECT;
				if(Status & SOBJ_ACTIVE){
					ChangeArmor = Armor;
					ChangeEnergy = Energy;
				};
			};
			break;
		case EXTERNAL_MODE_ESCAVE_IN:
			ExternalTime--;
			if(ExternalTime <= 0){
				ExternalMode = EXTERNAL_MODE_NORMAL;
				ExternalLock = 0;
				uvsPoint -> Pescave = (uvsEscave*)(((EnterEngine*)(ExternalObject->Owner))->Owner->Owner);
				uvsPoint -> Pspot = NULL;
				Go2Universe();
				uvsPoint -> Event(UVS_EVENT::ESCAVE_ARRIVAL);
				Status |= SOBJ_DISCONNECT;
				if(Status & SOBJ_ACTIVE){
					ChangeArmor = Armor;
					ChangeEnergy = Energy;
				};
			};
			break;
		case EXTERNAL_MODE_DARK:
			ExternalTime--;
			if(ExternalTime <= 0){
				ExternalTime = 10;
				PalCD.Set(CPAL_HIDE_PASSAGE,ExternalTime);
				ExternalMode = EXTERNAL_MODE_EARTH_PREPARE;
			};
			break;
		case EXTERNAL_MODE_LIGHT:
			ExternalTime--;
			if(ExternalTime <= 0){
				ExternalTime = 5;
				PalCD.Set(CPAL_PASSAGE_FROM,ExternalTime);
				ExternalMode = EXTERNAL_MODE_DARK;
				for(i = 0;i < 5;i++)
					EffD.CreateParticleGenerator(R_curr,R_curr,Vector(5,0,0)*DBM((int)(RND(PI*2)),Z_AXIS));
			};
			break;
		case EXTERNAL_MODE_EARTH_PREPARE:
			ExternalTime--;
			if(ExternalTime <= 0){
				ExternalMode = EXTERNAL_MODE_NORMAL;
				ExternalLock = 0;
				if(ChangeWorldConstraction != -1 && ChangeWorldConstraction != CurrentWorld){
					uvsPoint -> Pworld = WorldTable[ChangeWorldConstraction];
					uvsPoint -> Pescave = uvsPoint->Pworld->escT[0];
					uvsPoint -> Pspot = NULL;
					uvsPoint -> Event(UVS_EVENT::GO_NEW_ESCAVE);
				}else{
					 uvsPoint -> Pescave = uvsPoint->Pworld->escT[0];
	   				 if(uvsPoint -> Pescave){
						uvsPoint -> Pspot = NULL;
						uvsPoint -> Event(UVS_EVENT::ESCAVE_ARRIVAL);
					}else{
						uvsPoint -> Pescave = NULL;
						uvsPoint -> Pspot = uvsPoint->Pworld->sptT[0];
						uvsPoint -> Event(UVS_EVENT::SPOT_ARRIVAL);
					};
				};				
				Go2Universe();
				ChangeWorldConstraction = -1;
				Status |= SOBJ_DISCONNECT;
			};
			break;
		case EXTERNAL_MODE_PASS_IN:
			ExternalTime--;

			if(Visibility == VISIBLE && ActD.Active) {
				SOUND_PASSAGE(getDistX(ActD.Active->R_curr.x,R_curr.x));
			}
			if(ExternalTime & 3) {
				EffD.CreateDeform(
					ExternalObject->R_curr + Vector(
						PASSING_WAVE_RADIUS - realRND(PASSING_WAVE_RADIUS2),
						PASSING_WAVE_RADIUS - realRND(PASSING_WAVE_RADIUS2),
						83
					),
					1,
					PASSING_WAVE_PROCESS
				);
			}

			if(!(Status & SOBJ_ACTIVE)){
				if(ExternalTime <= 0){
					ExternalMode = EXTERNAL_MODE_NORMAL;
					ExternalLock = 0;
					Go2Universe();
					if(!NetworkON){
						uvsPoint -> Ppassage = ((PassageEngine*)(ExternalObject->Owner))->uvsPort;
						uvsPoint -> Event(UVS_EVENT::PASSAGE_ARRIVAL);
					};
					Status |= SOBJ_DISCONNECT;
				};
			}else{
				if(ExternalTime <= 0){
					ExternalMode = EXTERNAL_MODE_NORMAL;
					ExternalLock = 0;
					uvsPoint -> Ppassage = ((PassageEngine*)(ExternalObject->Owner))->uvsPort;

					w1 = ((PassageEngine*)(ExternalObject->Owner))->uvsPort->Poutput;
					w2 = ((PassageEngine*)(ExternalObject->Owner))->uvsPort->Pworld;
					for(i = 0;i < w1->pssTmax;i++){
						if(w1->pssT[i]->Poutput == w2){
							NewWorldX = w1->pssT[i]->pos_x;
							NewWorldY = w1->pssT[i]->pos_y;
						};
					};
					Go2Universe();

					ChangeArmor = Armor;
					ChangeEnergy = Energy;

					uvsPoint -> Event(UVS_EVENT::PASSAGE_ARRIVAL);
					Status |= SOBJ_DISCONNECT;
				};
			};
			break;
		case EXTERNAL_MODE_SIGN_IN:
			ExternalTime--;
			if(ExternalTime <= 0){
				ExternalMode = EXTERNAL_MODE_FREE_IN;
				ExternalTime = ROTOR_PROCESS_LIFE_TIME;
				CreateParticleRotor(R_curr,83);
//				StartHidePassage(ExternalTime);
				PalCD.Set(CPAL_HIDE_PASSAGE,ExternalTime);
//				ExternalLock = 1;
				ExternalDraw = 0;
//				ExternalObject = NULL;
				switch_analysis(1);
			};
			break;
		case EXTERNAL_MODE_FREE_IN:
			ExternalTime--;
			if(Visibility == VISIBLE && ActD.Active) SOUND_PASSAGE(getDistX(ActD.Active->R_curr.x,R_curr.x));
			if(ExternalTime <= 0){
				ExternalMode = EXTERNAL_MODE_NORMAL;
				ExternalLock = 0;
				Go2Universe();

				if(NetworkON && my_server_data.GameType == PASSEMBLOSS && UsedCheckNum >= GloryPlaceNum){
					GameOverID = GAME_OVER_NETWORK;
					Status |= SOBJ_DISCONNECT;
					GameQuantReturnValue = RTO_LOADING3_ID;
				}else{
					uvsPoint -> Event(UVS_EVENT::GO_NEW_WORLD);
					NewWorldX = ViewX;
					NewWorldY = ViewY;
					Status |= SOBJ_DISCONNECT;
					ChangeArmor = Armor;
					ChangeEnergy = Energy;
				};
			};
			break;
		case EXTERNAL_MODE_EARTH_IN:
			if(Visibility == VISIBLE && ActD.Active) SOUND_PASSAGE(getDistX(ActD.Active->R_curr.x,R_curr.x));
			if(ExternalTime & 3) EffD.CreateDeform(R_curr + Vector(PASSING_WAVE_RADIUS - realRND(PASSING_WAVE_RADIUS2),PASSING_WAVE_RADIUS - realRND(PASSING_WAVE_RADIUS2),83),1,PASSING_WAVE_PROCESS);
			ExternalTime--;
			if(ExternalTime <= 0){
//				ExternalMode = EXTERNAL_MODE_NORMAL;
//				ExternalLock = 0;
				uvsPoint -> Pescave = NULL;
				uvsPoint -> Pspot = NULL;
				uvsPoint -> Ppassage = NULL;				
				Go2Universe();

				if(ActD.LuckyFunction) 
					GameOverID = GAME_OVER_LUCKY;
				else{
					if(ActD.ThreallDestroy){
						if(ActD.SpobsDestroy) GameOverID = GAME_OVER_ALL_LOCKED;
						else GameOverID = GAME_OVER_INFERNAL_LOCKED;
					}else{
						if(ActD.SpobsDestroy) GameOverID = GAME_OVER_SPOBS_LOCKED;
						else GameOverID = GAME_OVER_LUCKY;
					};
				};
//				if(!NetworkON) uvsPoint -> destroy(PlayerDestroyFlag);
				Status |= SOBJ_DISCONNECT;
				GameQuantReturnValue = RTO_LOADING3_ID;
			};
			break;
		case EXTERNAL_MODE_IN_VANGER:  // teleport from mehos 
			ExternalTime--;
			aciSendEvent2actint(ACI_LOCK_INTERFACE,NULL); // disable right menu and full screen
			if(ExternalTime <= 0){
				SOUND_BOOT_STOP();
				Go2Universe();
				Status |= SOBJ_DISCONNECT;
			};			
			break;
		case EXTERNAL_MODE_OUT_VANGER:  // loading into mechos
			ExternalTime--;
			if(ExternalTime <= 0){
				SOUND_BOOT_STOP();
				ExternalMode = EXTERNAL_MODE_NORMAL;
				ExternalLock = 0;
				ExternalDraw = 1;
				switch_analysis(0);
				ExternalSensor = ExternalObject;
				aciSendEvent2actint(ACI_UNLOCK_INTERFACE,NULL); // enable right menu 
			};
			break;
	};
	DoorFlag = 0;
};

void VangerUnit::Action(void)
{		
	int a;
	int d;
	Vector vCheck;

	jump_power = 0;

	switch(aiMoveMode){
		case AI_MOVE_TRACK:
			if(MixVectorEnable){
				MixVector();
				MoveAngle = rPI(vDirect.psi() + MoveAngle);
			}else{
				MoveAngle = vDirect.psi();
				DeltaSpeed = MaxVelocity;
			};		

			switch(aiMoveFunction){
				case AI_MOVE_FUNCTION_WHEEL:
					if(aiRelaxTime > aiRelaxTimeMax && !(aiStatus & AI_STATUS_TARGET)){
						a = rPI(Angle - MoveAngle);
						if(a > PI) a -= 2*PI;

						if((dynamic_state & GROUND_COLLISION) && !(dynamic_state & TRACTION_WHEEL_TOUCH)){
							if(a > 0) controls(CONTROLS::LEFT_SIDE_IMPULSE,20);
							else controls(CONTROLS::RIGHT_SIDE_IMPULSE,20);
						};

						if(WallCollisionTime){
							if(NoWayEnable == AI_NO_WAY_NONE){
								if(!RND(5)) CalcImpulse();
								else CalcForce();								
							};
						}else{
							if(DeltaTractionTime) MixVectorEnable = 0;
							else MixVectorEnable = 1;
							if(aiRelaxTime > 2*aiRelaxTimeMax && NoWayEnable == AI_NO_WAY_NONE){
 								if(!RND(5)) CalcImpulse();
								else CalcForce();								
							};
						};
					}else MixVectorEnable = 1;
					break;
				case AI_MOVE_FUNCTION_IMPULSE:
				case AI_MOVE_FUNCTION_FLY:
					aiMoveFunction = AI_MOVE_FUNCTION_WHEEL;
					break;				
			};
			break;
		case AI_MOVE_POINT:
			MoveAngle = vDirect.psi();
			DeltaSpeed = MaxVelocity;

			switch(aiMoveFunction){
				case AI_MOVE_FUNCTION_WHEEL:
					if(aiStatus & AI_STATUS_WHEEL){
						if(aiRelaxTime > aiRelaxTimeMax){
							a = rPI(Angle - MoveAngle);
							if(a > PI) a -= 2*PI;
							if((dynamic_state & GROUND_COLLISION) && !(dynamic_state & TRACTION_WHEEL_TOUCH)){
								if(a > 0) controls(CONTROLS::LEFT_SIDE_IMPULSE,20);
								else controls(CONTROLS::RIGHT_SIDE_IMPULSE,20);
							};
						};
						if(!(aiStatus & AI_STATUS_TARGET) || WallCollisionTime){
							if(aiStatus & AI_STATUS_FLY) aiMoveFunction = AI_MOVE_FUNCTION_FLY;
							else aiMoveFunction = AI_MOVE_FUNCTION_IMPULSE;
						};
					}else{
						if(aiStatus & AI_STATUS_FLY) aiMoveFunction = AI_MOVE_FUNCTION_FLY;
						else aiMoveFunction = AI_MOVE_FUNCTION_IMPULSE;
					};
					break;
				case AI_MOVE_FUNCTION_FLY:
					if((aiStatus & AI_STATUS_TARGET) || !(aiStatus & AI_STATUS_FLY)){
						aiMoveFunction = AI_MOVE_FUNCTION_WHEEL;
						controls(CONTROLS::HELICOPTER_DOWN,20);
					}else{
						if(R_curr.z < 350)
							controls(CONTROLS::HELICOPTER_UP,20);

						if(aiRelaxTime > aiRelaxTimeMax*5){
							if(aiStatus & AI_MOVE_FUNCTION_IMPULSE) aiMoveFunction = AI_MOVE_FUNCTION_IMPULSE;
							else  aiMoveFunction = AI_MOVE_FUNCTION_WHEEL;
						}else{
							if((dynamic_state & GROUND_COLLISION) && R_curr.z < 300){
								jump_power = aiRelaxTime * (max_jump_power - 1) / (aiRelaxTimeMax*5);
								controls(CONTROLS::JUMP_USING_ACCUMULATED_POWER,20);
							};
						};
					};
					break;
				case AI_MOVE_FUNCTION_IMPULSE:
					if(!(aiStatus & AI_STATUS_IMPULSE) || (aiStatus & AI_STATUS_TARGET)){
						aiMoveFunction = AI_MOVE_FUNCTION_WHEEL;
						aiReactionCheckTime = 0;
						aiReactionMode = 0;
						aiReactionTime = 0;
					}else{
						a = rPI(Angle - MoveAngle);
						if(a > PI) a -= 2*PI;
						switch(aiReactionMode){
							case 0:
								if(dynamic_state & TOUCH_OF_GROUND){
									vCheck = Vector(getDistX(TargetPoint.vPoint.x,R_curr.x),getDistY(TargetPoint.vPoint.y,R_curr.y),0);
									d = vCheck.vabs();
									if(d < aiMaxJumpRadius)	jump_power = d * (max_jump_power - 1) / aiMaxJumpRadius;
									else jump_power = max_jump_power - 1;
									controls(CONTROLS::JUMP_USING_ACCUMULATED_POWER,20);
									aiReactionMode = 1;
									aiRelaxTime = 0;
									aiReactionCheckTime = 0;
								};
								break;
							case 1:
								if(aiRelaxTime > aiRelaxTimeMax || WallCollisionTime){
									aiReactionMode = 2;
									aiReactionCheckTime = 0;
								}else{
									if(aiReactionCheckTime > aiReactionCheckTimeMax){
										aiMoveFunction = AI_MOVE_FUNCTION_WHEEL;
										aiReactionMode = 1;
										aiRelaxTime = 0;
										aiReactionCheckTime = 0;
									}else{
										aiReactionCheckTime++;
										if((dynamic_state & GROUND_COLLISION) && !(dynamic_state & TRACTION_WHEEL_TOUCH)){
											if(a > 0) controls(CONTROLS::LEFT_SIDE_IMPULSE,20);
											else controls(CONTROLS::RIGHT_SIDE_IMPULSE,20);
										};
									};
								};
								break;
							case 2:
								if((dynamic_state & GROUND_COLLISION) && !(dynamic_state & TRACTION_WHEEL_TOUCH)){
									if(a > 0) controls(CONTROLS::LEFT_SIDE_IMPULSE,20);
									else controls(CONTROLS::RIGHT_SIDE_IMPULSE,20);
								};
								if(abs(a) < PI / 8 || aiReactionCheckTime > 2*aiRelaxTimeMax) aiReactionMode = 0;
								else{
									aiReactionCheckTime++;
									if(aiReactionCheckTime < aiRelaxTimeMax)
										DeltaSpeed = -DeltaSpeed*4;
								};
								break;
						};
					};
					break;
			};
			break;
	};

	TrackUnit::Action();
	if(!(frame % MAX_TRACTION_CHECK)){
		TractionValue = TractionStat / MAX_TRACTION_CHECK;
		TractionStat = 0;
	};
	TractionStat += CurrSpeed;
};

void VangerUnit::HideAction(void)
{
	DeltaSpeed = MaxHideSpeed;
	if(RND(100) < 20) DeltaSpeed = 1 - RND(2);
	MoveAngle = vDirect.psi();
	TrackUnit::HideAction();
};

void VangerUnit::Init(StorageType* s)
{
	ID = ID_VANGER;
	uvsUnitType::Init();
	TrackUnit::Init(s);
};

void VangerUnit::Free(void)
{
	uvsUnitType::Free();
};

void VangerUnit::CreateVangerUnit(void)
{
	if(NetworkON){
		if(Status & SOBJ_ACTIVE){
			NetID = CREATE_NET_ID(NID_VANGER);
			ShellNetID = (NetID & (~(63 << 16))) | NID_SHELL;
			uvsPoint->Pmechos->color = aciGetPlayerColor();
		}else{
			NetID = 0;
			ShellNetID = 0;
		};
	};

	ShellUpdateFlag = 0;
	switch(uvsPoint->Pmechos->color){
		case 0:
			set_body_color(COLORS_IDS::BODY_GREEN);
			aiModifier = AI_MODIFIER_ELEEPOD;
			break;
		case 1:
			set_body_color(COLORS_IDS::BODY_RED);			
			aiModifier = AI_MODIFIER_BEEBOORAT;
			break;
		case 2:
			set_body_color(COLORS_IDS::BODY_BLUE);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 3:
			set_body_color(COLORS_IDS::BODY_YELLOW);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 4:
			set_body_color(COLORS_IDS::BODY_CRIMSON);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 5:
			set_body_color(COLORS_IDS::BODY_GRAY);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 6:
			set_body_color(COLORS_IDS::ROTTEN_ITEM);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 7:
			set_body_color(COLORS_IDS::MATERIAL_3);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 8:
			set_body_color(COLORS_IDS::MATERIAL_1);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 9:
			set_body_color(COLORS_IDS::MATERIAL_0);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
	};

//	FeatureEnable = 0;

	DoorFlag = 0;
	ExternalMode = EXTERNAL_MODE_NORMAL;
	ExternalTime = 0;
	ExternalLock = 0;
	ExternalDraw = 1;
	ExternalObject = ExternalLastSensor = ExternalSensor = NULL;
	ExternalTime2 = 0;
	ExternalAngle = 0;
	Go2World();

	TouchKeyObject = NULL;
	TouchKeyObjectFlag = 1;

	dynamic_state = WHEELS_TOUCH;

	CheckPointCount = 0;

	MaxKhoxPoison = KhoxPoison = 300 << 8;

	VangerChanger = NULL;
	VangerChangerCount = 0;
	MechosChangerType = 0;
	
	vChangerPosition = Vector(0,0,0);
	VangerChangerAngle = 0;
	VangerChangerArmor = 0;
	VangerChangerEnergy = 0;
	VangerChangerColor = 0;

	RandomUpdate = -1;
	LastMole = 0;

	CoptePoint = NULL;
	SwimPoint = NULL;
	MolePoint = NULL;	

#ifdef AUTOMATIC_CHANGE_WORLD
		if(Status & SOBJ_ACTIVE){
			aciWorldIndex = AUTOMATIC_WORLD_INDEX;
			AUTOMATIC_WORLD_INDEX = -1;
		};
#endif
	
	if(Status & SOBJ_ACTIVE){
		aciUpdateCurCredits(aiGetHotBug());
//		aciCurCredits = ActD.HotBug;
		aciMaxEnergy = (MaxEnergy >> 16) + 1;
		aciMaxArmor = (MaxArmor	>> 16) + 1;
		aciCurEnergy = Energy >> 16;
		aciCurArmor = Armor >> 16;
		aciCurJumpCount = PassageCount;
		aciMaxJumpCount = MaxPassageCount + 1;
	};

	InitAI();
	RuffaGunTime = 0;		
	SensorEnable = 0;
	VangerRaceStatus = VANGER_RACE_NONE;
	NetDestroyID = 0xff;
	NetChanger = 0;
	NetFunction = 0;
	PlayerDestroyFlag = 0;
	TabuUse = 0;	
	NetworkArmor = Armor;
	NetworkEnergy = Energy;
	NetRuffaGunTime = 0;
	PrevImpuseFrame = 0;	
	PrevNetFunction83Time = NetFunction83Time = NetProtractorFunctionTime = NetMessiahFunctionTime = 0;
};

void VangerUnit::AddFree(void)
{
	int i;
	Vector vCheck;	
	if(Status & SOBJ_ACTIVE){
		if(vSetVangerFlag != -1){
			ExternalMode = EXTERNAL_MODE_OUT_VANGER;
			ExternalObject = NULL;
			ExternalTime = CHANGE_VANGER_TIME;
			ExternalLock = 0;
			ExternalDraw = 1;
			switch_analysis(1);
			vSetVangerFlag = -1;
			PalCD.Set(CPAL_SHOW_PASSAGE,ExternalTime);			
			for(i = 0;i < 5;i++){
				vCheck = Vector(300,0,0)*DBM((int)(RND(PI*2)),Z_AXIS);
				vCheck += R_curr;
				cycleTor(vCheck.x,vCheck.y);
				EffD.CreateParticleGenerator(vCheck,R_curr,Vector(5,0,0)*DBM((int)(RND(PI*2)),Z_AXIS),PG_STYLE_REINCAR);
			};
			set_3D(SET_3D_TO_THE_UPPER_LEVEL,R_curr.x,R_curr.y,R_curr.z,0,-Angle,0);
			SOUND_BOOT_START();
		}else{
			ExternalMode = EXTERNAL_MODE_PASS_OUT;
			ExternalObject = NULL;
			ExternalTime = ROTOR_PROCESS_LIFE_TIME;
			ExternalLock = 1;
			ExternalDraw = 0;
			switch_analysis(1);
			CreateParticleMechos(R_curr,500);
			if(!NetworkON || !PalIterLock) PalCD.Set(CPAL_SHOW_PASSAGE,ExternalTime);
			if(ChangeArmor != -1 && ChangeEnergy != -1){
				Energy = ChangeEnergy;
				Armor = ChangeArmor;
			};
			set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,255,0,-Angle,0);
		};
	}else{
		if(NetworkON && Visibility == VISIBLE && pNetPlayer && GetDistTime(NetGlobalTime,pNetPlayer->body.BirthTime) < 256*5 + 256*ROTOR_PROCESS_LIFE_TIME / 20){
			ExternalMode = EXTERNAL_MODE_PASS_OUT;
			ExternalObject = NULL;
			ExternalTime = ROTOR_PROCESS_LIFE_TIME;
			ExternalLock = 1;
			ExternalDraw = 0;
			switch_analysis(1);
			CreateParticleMechos(R_curr,500);
			set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,255,0,-Angle,0);
		}else{
			ExternalObject = NULL;
			ExternalMode = EXTERNAL_MODE_NORMAL;
			ExternalTime = 0;
			ExternalLock = 0;
			ExternalDraw = 1;
			switch_analysis(0);
		};
	};
};

void VangerUnit::AddPassage(SensorDataType* p)
{
	if(Status & SOBJ_ACTIVE){
		ExternalMode = EXTERNAL_MODE_PASS_OUT;
		ExternalObject = p;
		ExternalTime = p->Owner->ActiveTime;
		ExternalLock = 1;
		ExternalDraw = 0;
		switch_analysis(1);
		CreateParticleMechos(ExternalObject->R_curr,500);
		ActD.PassageTouchEnable = MAX_PASSAGE_DELAY;
		PalCD.Set(CPAL_SHOW_PASSAGE,ExternalTime);
		camera_direct(R_curr.x,R_curr.y,1 << 8,0,0,ExternalTime + 1);
		if(ChangeArmor != -1 && ChangeEnergy != -1){
			Energy = ChangeEnergy;
			Armor = ChangeArmor;
		};
	}else{
		if(NetworkON){
			if(Visibility == VISIBLE && pNetPlayer && GetDistTime(NetGlobalTime,pNetPlayer->body.BirthTime) < 256*5 + 256*ROTOR_PROCESS_LIFE_TIME/20){
				ExternalMode = EXTERNAL_MODE_PASS_OUT;
				ExternalObject = p;
				ExternalTime = p->Owner->ActiveTime;
				ExternalLock = 1;
				ExternalDraw = 0;
				switch_analysis(1);
				CreateParticleMechos(ExternalObject->R_curr,500);
			}else{
				ExternalObject = NULL;
				ExternalMode = EXTERNAL_MODE_NORMAL;
				ExternalTime = 0;
				ExternalLock = 0;
				ExternalDraw = 1;
				switch_analysis(0);
			};
		}else{
			if(Visibility == VISIBLE){
				ExternalMode = EXTERNAL_MODE_PASS_OUT;
				ExternalObject = p;
				ExternalTime = p->Owner->ActiveTime;
				ExternalLock = 1;
				ExternalDraw = 0;
				switch_analysis(1);
				CreateParticleMechos(ExternalObject->R_curr,500);
			}else{
				ExternalObject = NULL;
				ExternalMode = EXTERNAL_MODE_NORMAL;
				ExternalTime = 0;
				ExternalLock = 0;
				ExternalDraw = 1;
				switch_analysis(0);
			};
		};
	};

/*	if(Visibility == VISIBLE || (Status & SOBJ_ACTIVE)){
//		ExternalMode = EXTERNAL_MODE_PASS_IMPULSE;
		ExternalMode = EXTERNAL_MODE_PASS_OUT;
		ExternalObject = p;
//		ExternalTime = p->Owner->ActiveTime + EXTERNAL_IMPULSE_TIME;
		ExternalTime = p->Owner->ActiveTime;
		ExternalLock = 1;
		ExternalDraw = 0;
//		ExternalAngle = RND(2*PI);
		switch_analysis(1);
		CreateParticleMechos(ExternalObject->R_curr,500);
		if(Status & SOBJ_ACTIVE){
			ActD.PassageTouchEnable = MAX_PASSAGE_DELAY;
			PalCD.Set(CPAL_SHOW_PASSAGE,ExternalTime);
			camera_direct(R_curr.x,R_curr.y,1 << 8,0,0,ExternalTime);
			if(ChangeArmor != -1 && ChangeEnergy != -1){
				Energy = ChangeEnergy;
				Armor = ChangeArmor;
			};
		};
	}else{
		ExternalObject = NULL;
		ExternalMode = EXTERNAL_MODE_NORMAL;
		ExternalTime = 0;
		ExternalLock = 0;
		ExternalDraw = 1;
		switch_analysis(0);
	};*/
};

void VangerUnit::AddEscave(SensorDataType* p)
{
	ExternalMode = EXTERNAL_MODE_ESCAVE_OUT;
	ExternalObject = p;
	ExternalTime = p->Owner->ActiveTime;
	ExternalLock = 1;
	ExternalDraw = 1;
	switch_analysis(1);
	if(Status & SOBJ_ACTIVE){
		if(ChangeArmor != -1 && ChangeEnergy != -1){
			Energy = ChangeEnergy;
			Armor = ChangeArmor;
		};
	};
//	if(Visibility == VISIBLE)
//		SOUND_ENTRANCE();
};

void VangerUnit::AddSpot(SensorDataType* p)
{
	ExternalMode = EXTERNAL_MODE_SPOT_OUT;
	ExternalObject = p;
	ExternalTime = p->Owner->ActiveTime;
	ExternalLock = 1;
	ExternalDraw = 1;
	switch_analysis(1);

	if(Status & SOBJ_ACTIVE){
		if(ChangeArmor != -1 && ChangeEnergy != -1){
			Energy = ChangeEnergy;
			Armor = ChangeArmor;
		};
	};

//	if(Visibility == VISIBLE)
//		SOUND_ENTRANCE();
};

extern int preViewY;


VangerUnit* addVanger(uvsVanger* p,int x,int y,int Human)
{	
	VangerUnit* n;

#ifdef TEST_TRACK
	if(!Human) return NULL;
#endif

//	if(p->Pworld->gIndex != CurrentWorld)
//		ErrH.Abort("Bad AddFree");

//	if(!(p->Pmechos))
//		ErrH.Abort("Add pMechos Error");

	n = (VangerUnit*)(ActD.GetObject(ACTION_VANGER));
	if(n){
		n->CreateUnitType(p);
		if(Human){
			if(GeneralMapReload){
				if(GeneralLoadReleaseFlag) vMap->release();
				ViewX = x;
				preViewY = ViewY = y;
				vMap->reload(CurrentWorld);
				GeneralMapReload = 0;
#ifndef NEW_TNT
				RestoreFlagBarell();
				RestoreBarell();
#endif
			};
			MLCheckQuant();

			ActD.CreateActive(n);
			NumHumanModel = ModelD.FindModel(uvsMechosTable[p->Pmechos->type]->name);
			n->CreateActionUnit(NumHumanModel,SOBJ_ACTIVE,Vector(x,y,128),0,SET_3D_DIRECT_PLACE);
		}else n->CreateActionUnit(ModelD.FindModel(uvsMechosTable[p->Pmechos->type]->name),SOBJ_AUTOMAT,Vector(x,y,128),0,SET_3D_DIRECT_PLACE);
		n->CreateTrackUnit();
		n->CreateVangerUnit();
		
		if(NetworkON){
			if(Human){
				n->NetCreateVanger(NULL,NULL,NULL);
				n->AddFree();
			}else n->NetCreateSlave();
		}else n->AddFree();

		ActD.ConnectObject(n);
	};
	return n;
};

VangerUnit* addVanger(uvsVanger* p,uvsPassage* origin,int Human)
{
	VangerUnit* n;
	SensorDataType* s;

#ifdef TEST_TRACK
	if(!Human) return NULL;
#endif

//	if(origin->Pworld->gIndex != CurrentWorld)
//		ErrH.Abort("Bad AddInPassage");

//	if(p->Pworld->gIndex != CurrentWorld)
//		ErrH.Abort("Bad AddFree");

//	if(origin->locked)
//		ErrH.Abort("Bad locked Passage");

//	if(!(p->Pmechos))
//		ErrH.Abort("Add pMechos Error");

	n = (VangerUnit*)(ActD.GetObject(ACTION_VANGER));
	if(n){
		n->CreateUnitType(p);
		s = ((origin->unitPtr).PassageT)->ActionLink;

		if(Human){
			ActD.CreateActive(n);
			NumHumanModel = ModelD.FindModel(uvsMechosTable[p->Pmechos->type]->name);
			n->CreateActionUnit(NumHumanModel,SOBJ_ACTIVE,s->R_curr,0,SET_3D_DIRECT_PLACE);			
		}else n->CreateActionUnit(ModelD.FindModel(uvsMechosTable[p->Pmechos->type]->name),SOBJ_AUTOMAT,s->R_curr,0,SET_3D_DIRECT_PLACE);

		n->CreateTrackUnit();
		n->CreateVangerUnit();
//		n->AddPassage(s);

		if(NetworkON){
			if(Human){
				n->NetCreateVanger(origin,NULL,NULL);
				n->AddPassage(s);
			}else n->NetCreateSlave();
		}else n->AddPassage(s);

//		if(NetworkON){
//			if(Human) n->NetCreateVanger(origin,NULL,NULL);
//			else n->NetCreateSlave();
//		};
		ActD.ConnectObject(n);
	};
	return n;
};

int iGetOptionValue(int id);
void ActionDispatcher::PromptChangeCycle(void)
{
	if(CurrentWorld == WORLD_FOSTRAL && Active && PromptChangeCycleCount < 7 && !(WorldTable[WORLD_GLORX]->GamerVisit)){
		if(iGetOptionValue(6) || NetworkON) return;
		switch(uvsCurrentCycle){
			case 0:
//				aiMessageData[AI_MESSAGE_20].Send(0,0xff,0);
				aiMessageQueue.Send(AI_MESSAGE_20,0,0xff,0);
				break;
			case 1:
//				aiMessageData[AI_MESSAGE_01].Send(0,0xff,0);
				aiMessageQueue.Send(AI_MESSAGE_01,0,0xff,0);
				break;
			case 2:
//				aiMessageData[AI_MESSAGE_12].Send(0,0xff,0);
				aiMessageQueue.Send(AI_MESSAGE_12,0,0xff,0);
				break;
		};
		PromptChangeCycleCount++;
	};
};

void ActionDispatcher::PromptInit(int ind)
{
	PromptCurrentWay = ind;	
	if(ind) PromptIncubatorFreeVisit++;
	else PromptPodishFreeVisit++;
	PromptPrevY = Active->R_curr.y;
	PromptPrevTime = 0;
};


//Пишем подсказки на экране для миров
void ActionDispatcher::PromptQuant(void)
{
	int r_log = 1;
	
	if(iGetOptionValue(6)) return;
	if(!NetworkON && Active){
		if(Active->Armor < Active->MaxArmor / 2 && ConTimer.sec == 0 && (ConTimer.min & 1))
			aiMessageQueue.Send(AI_MESSAGE_LOW_ARMOR,0,0xff,0);

		switch(CurrentWorld){
			case WORLD_GLORX:
				if(Active->VangerRaceStatus == VANGER_RACE_NONE && !(ConTimer.min % 30) && ConTimer.sec == 0 && !aciWorldLinkExist(WORLD_GLORX,WORLD_NECROSS,0)){
					switch(dgD -> bStatus){
						case 0:
							aiMessageQueue.Send(AI_MESSAGE_GLORX_MOVE_RUB,0,0xff,0);
							break;
						case 1:
							aiMessageQueue.Send(AI_MESSAGE_GLORX_MOVE_PRORUB,0,0xff,0);
							break;
					};
				};
				break;
			case WORLD_NECROSS:
				if(Active->VangerRaceStatus == VANGER_RACE_NONE && ConTimer.min == 0 && ConTimer.sec == 0 && !aciWorldLinkExist(WORLD_GLORX,WORLD_XPLO,0))
					aiMessageQueue.Send(AI_MESSAGE_NECROSS_MOVE,0,0xff,0);
				break;
			case WORLD_FOSTRAL:
				if(Active->VangerRaceStatus == VANGER_RACE_NONE && !WorldTable[WORLD_GLORX]->GamerVisit){
					if(PromptCurrentWay){
						if(PromptIncubatorFreeVisit  < 3 && PromptIncubatorCount < 3){
							if(!(ConTimer.min % 3) && ConTimer.sec == 0 && !GetStuffObject(Active,ACI_NYMBOS)){
								PromptIncubatorCount++;						
								aiMessageQueue.Send(AI_MESSAGE_NYMBOS_LOST,0,0xff,0);//aiMessageData[AI_MESSAGE_NYMBOS_LOST].Send(0,0xff,0);
								return;
							};
						};

						if(PromptIncubatorFreeVisit < 2){
							if(getDistY(PromptPrevY,Active->R_curr.y) > 1000){
								PromptPrevTime = 0;
								PromptPrevY = Active->R_curr.y;
							}else{			
								if(PromptPrevTime > 1500){
									PromptPrevTime = 0;
									PromptPrevTime = 0;
									aiMessageQueue.Send(AI_MESSAGE_INCUBATOR_WAY,0,0xff,0);//aiMessageData[AI_MESSAGE_INCUBATOR_WAY].Send(0,0xff,0);
									if(!GetCompasTarget()){
										if(lang() == RUSSIAN) SelectCompasTarget(rCmpIncubator);
										else SelectCompasTarget(eCmpIncubator);
										aciRefreshTargetsMenu();
									};					
									return;
								}else PromptPrevTime++;
							};
						};
						
						if (PromptIncubatorFreeVisit < 2
							&& (PromptIncubatorCount > 2 || GetStuffObject(Active,ACI_NYMBOS))
							&& !(ConTimer.min % 30) //  show every 30 minutes (1.5 min real)
							&& ConTimer.sec == 0
							&& ConTimer.counter < 36000 ) {  //do not show message if an 10 game hour(30 min real) has passed 
							aiMessageQueue.Send(AI_MESSAGE_CAMERA_HELP, 0, 0xff, 0);
							return;
						}
					}else{
						if(PromptPodishFreeVisit  < 3 && PromptPodishCount < 3){
							if(!(ConTimer.min % 3) && ConTimer.sec == 0 && !GetStuffObject(Active,ACI_PHLEGMA)){
								PromptPodishCount++;
								aiMessageQueue.Send(AI_MESSAGE_PHLEGMA_LOST,0,0xff,0);//aiMessageData[AI_MESSAGE_PHLEGMA_LOST].Send(0,0xff,0);
								return;
							};
						};

						if(PromptPodishFreeVisit < 2){
							if(getDistY(Active->R_curr.y,PromptPrevY) > 1000){
								PromptPrevTime = 0;
								PromptPrevY = Active->R_curr.y;
							}else{
								if(PromptPrevTime > 1500){
									PromptPrevTime = 0;
									aiMessageQueue.Send(AI_MESSAGE_PODISH_WAY,0,0xff,0);//aiMessageData[AI_MESSAGE_PODISH_WAY].Send(0,0xff,0);
									if(!GetCompasTarget()){
										if(lang() == RUSSIAN) SelectCompasTarget(rCmpPodish);
										else SelectCompasTarget(eCmpPodish);
										aciRefreshTargetsMenu();
									};
									return;
								}else PromptPrevTime++;
							};
						};
					};

					if(ConTimer.min == 0 && ConTimer.sec == 0){
						if(aciWorldLinkExist(WORLD_FOSTRAL,WORLD_GLORX)){
							aiMessageQueue.Send(AI_MESSAGE_GLORX_WAY,0,0xff,0);//aiMessageData[AI_MESSAGE_GLORX_WAY].Send(0,0xff,0);
							if(!GetCompasTarget()){
								if(lang() == RUSSIAN) SelectCompasTarget(rCmpPassGlorx);
								else SelectCompasTarget(eCmpPassGlorx);
								aciRefreshTargetsMenu();
							};
						}else{
							if(uvsCurrentCycle == 0){
								if(GamerResult.game_elr_result < 150){
									aiMessageQueue.Send(AI_MESSAGE_ELR_LOW,0,0xff,0);//aiMessageData[AI_MESSAGE_ELR_LOW].Send(0,0xff,0);
									if(!GetCompasTarget()){
										if(lang() == RUSSIAN) SelectCompasTarget(rCmpIncubator);
										else SelectCompasTarget(eCmpIncubator);
										aciRefreshTargetsMenu();
									};
								}else{
									if(GamerResult.game_elr_result < 250){
										aiMessageQueue.Send(AI_MESSAGE_ELR_HI,0,0xff,0);//aiMessageData[AI_MESSAGE_ELR_HI].Send(0,0xff,0);
										if(!GetCompasTarget()){
											if(lang() == RUSSIAN) SelectCompasTarget(rCmpIncubator);
											else SelectCompasTarget(eCmpIncubator);
											aciRefreshTargetsMenu();
										};
									};
								};
							};
						};
					};
				};
				break;
		};		
	};
};


VangerUnit* addVanger(uvsVanger* p,uvsEscave* origin,int Human)
{
	int ang;
	VangerUnit* n;
	SensorDataType* s;

#ifdef TEST_TRACK
	if(!Human) return NULL;
#endif

//	if(origin->Pworld->gIndex != CurrentWorld)
//		ErrH.Abort("Bad AddInEscave");

//	if(p->Pworld->gIndex != CurrentWorld)
//		ErrH.Abort("Bad AddFree");

//	if(origin->locked)
//		ErrH.Abort("Bad Locked Escave");

//	if(!(p->Pmechos))
//		ErrH.Abort("Add pMechos Error");


	n = (VangerUnit*)(ActD.GetObject(ACTION_VANGER));
	if(n){
		
		s = origin->unitPtr.EnterT->GetCenter();
		if(Human){
			if(GeneralMapReload){
				if(GeneralLoadReleaseFlag) vMap->release();
				ViewX = s->R_curr.x;
				preViewY = ViewY = s->R_curr.y;
				vMap->reload(CurrentWorld);
				GeneralMapReload = 0;
#ifndef NEW_TNT
				RestoreFlagBarell();
				RestoreBarell();
#endif				
			};
			MLCheckQuant();
		};

		((origin->unitPtr).EnterT)->ActiveCenter();
		n->CreateUnitType(p);
		if(s->Owner->Type == EngineTypeList::ESCAVE) ang = ((EscaveEngine*)(s->Owner))->StartAngle;
		else ang = RND(PI*2);

		if(Human){
			ActD.CreateActive(n);
			NumHumanModel = ModelD.FindModel(uvsMechosTable[p->Pmechos->type]->name);
			n->CreateActionUnit(NumHumanModel,SOBJ_ACTIVE,s->R_curr,ang,SET_3D_DIRECT_PLACE);			
		}else n->CreateActionUnit(ModelD.FindModel(uvsMechosTable[p->Pmechos->type]->name),SOBJ_AUTOMAT,s->R_curr,ang,SET_3D_DIRECT_PLACE);

		n->CreateTrackUnit();
		n->CreateVangerUnit();
		n->AddEscave(s);
		ActD.ConnectObject(n);
		if(NetworkON){
			if(Human) n->NetCreateVanger(NULL,origin,NULL);
			else n->NetCreateSlave();
		};

		if(Human)
			ActD.PromptInit(1);

	};
	return n;
};

VangerUnit* addVanger(uvsVanger* p,uvsSpot* origin,int Human)
{
	VangerUnit* n;
	SensorDataType* s;

#ifdef TEST_TRACK
	if(!Human) return NULL;
#endif

//	if(origin->Pworld->gIndex != CurrentWorld)
//		ErrH.Abort("Bad AddInSpot");

//	if(origin->locked)
//		ErrH.Abort("Bad locked Spot");

//	if(p->Pworld->gIndex != CurrentWorld)
//		ErrH.Abort("Bad AddFree");

//	if(!(p->Pmechos))
//		ErrH.Abort("Add pMechos Error");

	n = (VangerUnit*)(ActD.GetObject(ACTION_VANGER));
	if(n){
		s = ((origin->unitPtr).EnterT)->GetCenter();
		if(Human){
			if(GeneralMapReload){
				if(GeneralLoadReleaseFlag) vMap->release();

				ViewX = s->R_curr.x;
				preViewY = ViewY = s->R_curr.y;
				vMap->reload(CurrentWorld);
				GeneralMapReload = 0;

#ifndef NEW_TNT
				RestoreFlagBarell();
				RestoreBarell();
#endif
			};
			MLCheckQuant();			
		};
		((origin->unitPtr).EnterT)->ActiveCenter();

		n->CreateUnitType(p);

		if(Human){
			NumHumanModel = ModelD.FindModel(uvsMechosTable[p->Pmechos->type]->name);
			n->CreateActionUnit(NumHumanModel,SOBJ_ACTIVE,s->R_curr,0,SET_3D_DIRECT_PLACE);
			ActD.CreateActive(n);
		}else n->CreateActionUnit(ModelD.FindModel(uvsMechosTable[p->Pmechos->type]->name),SOBJ_AUTOMAT,s->R_curr,0,SET_3D_DIRECT_PLACE);

		n->CreateTrackUnit();
		n->CreateVangerUnit();
		n->AddSpot(s);
		ActD.ConnectObject(n);
		if(NetworkON){
			if(Human) n->NetCreateVanger(NULL,NULL,origin);
			else n->NetCreateSlave();
		};
		if(Human)
			ActD.PromptInit(0);
	};
	return n;
};

extern VangerUnit* actCurrentViewObject;
void uvsChangeCycle(void);

void VangerUnit::NewKeyHandler(void)
{
	Vector vCheck;
	BulletObject* g;
//	VangerUnit* v;
	static int UseVectorFlag = 0;
	int i;
	StuffObject* p;
	StuffObject* n;

	
	if(iKeyPressed(iKEY_USE_GLUEK)){
		p = GetStuffObject(this,ACI_GLUEK);
		if(p){
			(ActD.Active)->Armor += 10 << 16;
			ObjectDestroy(p);
			(ActD.Active)->CheckOutDevice(p);
			ActD.CheckDevice(p);
			p->Storage->Deactive(p);
			(ActD.Active)->DelDevice(p);
			aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
		};
	};

	if(iKeyPressed(iKEY_USE_VECTOR)){
		if(!UseVectorFlag){
			UseVectorFlag++;
			p = NULL;
			for(i = 0;i < MAX_ACTIVE_SLOT;i++){
				if(ActD.Slot[i]){
					p = ActD.Slot[i];
					aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
					if(p->ActIntBuffer.type != ACI_RADAR_DEVICE){
						n = GetStuffObject(this,ACI_RADAR_DEVICE);
						if(n)
							aciSendEvent2actint(ACI_PUT_IN_SLOT,&(n->ActIntBuffer));
					};
					aciSendEvent2actint(ACI_PUT_ITEM,&(p->ActIntBuffer));
					break;
				};
			};
			if(!p){
				n = GetStuffObject(this,ACI_RADAR_DEVICE);
				if(n)
					aciSendEvent2actint(ACI_PUT_IN_SLOT,&(n->ActIntBuffer));
			};	
		};
	}else UseVectorFlag = 0;

	if(iKeyPressed(iKEY_OPEN)){
		if(!ThreallMessageProcess){
			if(ActD.DoorEnable){
				DoorFlag = 1;
				ActD.DoorEnable = 0;
			};
		};
	}else ActD.DoorEnable = 1;

	if(iKeyPressed(iKEY_FIRE_WEAPON4))
		ActD.ActiveAllTerminator();

	if(iKeyPressed(iKEY_FIRE_WEAPON1)){
		ActD.ActiveSlot(0);
#ifdef UNIT_DEBUG_VIEW
//		DBGCHECK;
//		uvsChangeCycle();
//		uvsapiDestroyItem(0,-1);
//		v = (VangerUnit*)(ActD.Tail);
//		while(v){
//			if(v->Status & SOBJ_AUTOMAT) v->CalcImpulse();
//			v = (VangerUnit*)(v->NextTypeList);
//		};
//		if(NumPromptTest >= aiNumMessage) NumPromptTest = 0;
//		else NumPromptTest++;
//		aiMessageData[NumPromptTest].Send();

//		aciWorldIndex = WORLD_NECROSS;
//		aiMessageQueue.Send(AI_MESSAGE_MACHOTINE,Speed,1,0);
//		OpenCrypt(0,0);
//		OpenCrypt(0,1);

/*		int aaa[83],bbb[83];
		for(i = 0;i < 83;i++){
			aaa[i] = RND(9);
			bbb[i] = CheckInMatrix(aaa[i]);
		};

		for(i = 0;i < 83;i++){
			if(CheckOutMatrix(aaa[i]) != bbb[i])
				ErrH.Abort("GHJHGH");
		};*/
//		Armor = Energy = 0;
//		aciWorldIndex = WORLD_THREALL;
//		ShowDominanceMessage(+5);
		
#endif
	};
	
	if(iKeyPressed(iKEY_FIRE_WEAPON2)){
		ActD.ActiveSlot(1);
#ifdef UNIT_DEBUG_VIEW
//		if(NumPromptTest <= 0) NumPromptTest = aiNumMessage - 1;
//		else NumPromptTest--;
//		aiMessageData[NumPromptTest].Send();

//		aciWorldIndex = WORLD_GLORX;
//		aiMessageQueue.Send(0,Speed,1,0);
//		uvsChangeCycle();
//		aciWorldIndex = WORLD_GLORX;
/*		for(i = 0;i < SnsTableSize;i++){
			if(SensorObjectData[i]->SensorType == SensorTypeList::TRAIN){
				if(!(SensorObjectData[i]->Owner))
					t = 83;
				else{
					t = (int)(((TrainEngine*)(SensorObjectData[i]->Owner))->LockFlag);
					if(((TrainEngine*)(SensorObjectData[i]->Owner))->TrainLink[0])
						t = (int)(((TrainEngine*)(SensorObjectData[i]->Owner))->TrainLink[0]);
					else
						t = 83;

					if(((TrainEngine*)(SensorObjectData[i]->Owner))->TrainLink[1])
						t = (int)(((TrainEngine*)(SensorObjectData[i]->Owner))->TrainLink[1]);
					else
						t = 83;
				};
			};
		};*/
//		ShowLuckMessage(-5);
//		Status ^= SOBJ_AUTOMAT;
#endif
	};

	if(iKeyPressed(iKEY_FIRE_WEAPON3)){
		ActD.ActiveSlot(2);
#ifdef UNIT_DEBUG_VIEW
//		aciWorldIndex = WORLD_NECROSS;
//		aiMessageQueue.Send(1,Speed,1,0);
//		PalCD.Set(CPAL_THREALL_TO,50);
//		ShowTaskMessage(10);
//		Status ^= SOBJ_AUTOMAT;
//		PalCD.Set(CPAL_SPOBS_TO,50);
#endif
	};

	if(iKeyPressed(iKEY_FIRE_ALL_WEAPONS)){
		if(PowerFlag & VANGER_POWER_RUFFA_GUN){
			if(RuffaGunTime > (RUFFA_GUN_WAIT * WeaponWaitTime >> 8)){
				RuffaGunTime = 0;
				g = BulletD.CreateBullet();
				vCheck = Vector(64,0,0)*RotMat;
				g->CreateBullet(Vector(R_curr.x,R_curr.y,R_curr.z),
					Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),NULL,&GameBulletData[WD_BULLET_RUFFA],this,Speed);
				if(ActD.Active)
					SOUND_RAFFA_SHOT(getDistX(ActD.Active->R_curr.x,R_curr.x));

				if(NetworkON){
					NetRuffaGunTime = GLOBAL_CLOCK();
					ShellUpdateFlag = 1;
				};
			};
		}else{
			ActD.ActiveAllSlots();
		};
	};	

	if(iKeyPressed(iKEY_CHANGE_TARGET))
		ActD.ChangeLocator();

	if((Status & SOBJ_ACTIVE) && (Status & SOBJ_AUTOMAT) && 
	   (iKeyPressed(iKEY_TURN_WHEELS_LEFT) || iKeyPressed(iKEY_TURN_WHEELS_RIGHT) || iKeyPressed(iKEY_MOVE_FORWARD) || iKeyPressed(iKEY_MOVE_BACKWARD) || iKeyPressed(iKEY_TURN_OVER_LEFT)
	   || iKeyPressed(iKEY_TURN_OVER_RIGHT) || iKeyPressed(iKEY_DEVICE_ON) || iKeyPressed(iKEY_DEVICE_OFF) || iKeyPressed(iKEY_ACTIVATE_KID) || iKeyPressed(iKEY_ACCELERATION) || iKeyPressed(iKEY_OPEN))){
		Status &=~SOBJ_AUTOMAT;
		aiMessageQueue.Send(AI_MESSAGE_AUTOMATIC_OFF,0,0xff,0);
	};
};

void VangerUnit::keyhandler(int key)
{
	Vector vCheck;
	BulletObject* g;
	SDL_Keymod mod;
	
	switch(key){
		case SDL_SCANCODE_1:
			DbgCheckEnable ^= 1;
			ActD.ActiveSlot(0);
			break;
		case SDL_SCANCODE_2:
			ActD.ActiveSlot(1);
			break;
		case SDL_SCANCODE_3:
			ActD.ActiveSlot(2);
			break;
		case SDL_SCANCODE_4:
			ActD.ActiveSlot(3);
			break;
		case SDL_SCANCODE_LCTRL:
		case SDL_SCANCODE_RCTRL:
			if(PowerFlag & VANGER_POWER_RUFFA_GUN){
				if(RuffaGunTime > (RUFFA_GUN_WAIT * WeaponWaitTime >> 8)){
					RuffaGunTime = 0;
					g = BulletD.CreateBullet();
					vCheck = Vector(64,0,0)*RotMat;
					g->CreateBullet(Vector(R_curr.x,R_curr.y,R_curr.z),
						Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),NULL,&GameBulletData[WD_BULLET_RUFFA],this,Speed);
					if(ActD.Active)
						SOUND_MACHOTINE_SHOT(getDistX(ActD.Active->R_curr.x,R_curr.x));
				};
			}else ActD.ActiveAllSlots();
			break;
		case SDL_SCANCODE_TAB:
			ActD.ChangeLocator();
			break;
		case SDL_SCANCODE_SPACE:
			DoorFlag = 1;
			break;
		case SDL_SCANCODE_5:
			start_vibration();
			//Explosion(R_curr,64,nModel);
			camera_impulse(512);
			break;
		case SDL_SCANCODE_7: {
			static int i_model = 0;
			static int models[2] = {0,0};
			mod = SDL_GetModState();
			if(!(mod&KMOD_CTRL)){
				if(!(mod&KMOD_SHIFT)){
					if(++NumHumanModel >= ModelD.MaxModel)
						NumHumanModel = 0;
					}
				else{
					if(--NumHumanModel < 0)
						NumHumanModel =  ModelD.MaxModel - 1;
					}
				models[i_model] = NumHumanModel;
				}
			else
				NumHumanModel = models[i_model = 1 - i_model];
			Object::operator = (ModelD.ActiveModel(NumHumanModel));
			cycleTor(R_curr.x,R_curr.y);
			set_active(1);
			set_3D(SET_3D_TO_THE_UPPER_LEVEL,R_curr.x,R_curr.y,R_curr.z,0,-Angle,Speed);
			}
			break;
	};
};

//--------------------------------------------------------------------------------------------------

void VangerUnit::CreateParticleMechos(const Vector& v,int r, int _type)
{
	TargetParticleObject* p;
	Vector vPos;
	int i,a,da,dr;
	unsigned char color_offset,color_shift;
	if(Visibility == VISIBLE){

		COLORS_VALUE_TABLE[COLORS_IDS::BODY*2] = body_color_offset;
		COLORS_VALUE_TABLE[COLORS_IDS::BODY*2 + 1] = body_color_shift;

		p = (TargetParticleObject*)(EffD.GetObject(EFF_PARTICLE02));
		if(p){
			a = RND(PI*2);
			da = 8 * PI  / model->num_poly;
			if (_type)
				p->CreateParticle(v,_type,0);
			else
				p->CreateParticle(v,ROTOR_PROCESS_LIFE_TIME,0);
			dr = r / 8;

			DBM A = A_l2g*scale_real;
			for(i = 0;i < model->num_poly;i++){
				vPos = Vector(dr + RND(r),0,0)*DBM(a,Z_AXIS);
				vPos += v;

				color_offset = COLORS_VALUE_TABLE[model->polygons[i].color_id*2];
				color_shift = COLORS_VALUE_TABLE[model->polygons[i].color_id*2 + 1];

				p->AddVertex2(vPos + Vector(RND(dr),RND(dr),0),
						Vector(1 - RND(2),1- RND(2),0) + v + (A*Vector(model->polygons[i].middle_x,model->polygons[i].middle_y,model->polygons[i].middle_z)),
						color_offset + (((1 << (7 - color_shift)) - 1) & ~1), _type);

				p->AddVertex2(vPos + Vector(RND(dr),RND(dr),0),
						Vector(1 - RND(2),1- RND(2),0) + v + (A*Vector(model->polygons[i].middle_x,model->polygons[i].middle_y,model->polygons[i].middle_z)),
						color_offset + (((1 << (7 - color_shift)) - 1) & ~1), _type);
				a = rPI(a + da);
			};
			EffD.ConnectObject(p);
		};
	};
};

void VangerUnit::CreateParticleRotor(const Vector& v,int r)
{
	TargetParticleObject* p;
	int i;
	unsigned char color_offset,color_shift;
	if(Visibility == VISIBLE){
		COLORS_VALUE_TABLE[COLORS_IDS::BODY*2] = body_color_offset;
		COLORS_VALUE_TABLE[COLORS_IDS::BODY*2 + 1] = body_color_shift;

		p = (TargetParticleObject*)(EffD.GetObject(EFF_PARTICLE02));
		if(p){
			p->CreateParticle(v,ROTOR_PROCESS_LIFE_TIME,1);

			for(i = 0;i < model->num_poly;i++){

				color_offset = COLORS_VALUE_TABLE[model->polygons[i].color_id*2];
				color_shift = COLORS_VALUE_TABLE[model->polygons[i].color_id*2 + 1];

				p->AddVertex(R_curr + (Vector(model->polygons[i].middle_x,model->polygons[i].middle_y,model->polygons[i].middle_z)*(A_scl)),
						color_offset + (((1 << (7 - color_shift)) - 1) & ~1),
						(2 << 8) + realRND(1 << 8),(3 << 7) + realRND(1 << 7));
				p->AddVertex(R_curr + (Vector(model->polygons[i].middle_x,model->polygons[i].middle_y,model->polygons[i].middle_z)*(A_scl)),
						color_offset + (((1 << (7 - color_shift)) - 1) & ~1),
						(2 << 8) + realRND(1 << 8),(3 << 7) + realRND(1 << 7));
			};


			for(i = 0;i < (p->NumParticle - model->num_poly*2);i++)
				p->AddVertex(v + Vector(r - realRND(r << 1),r - realRND(r << 1),r - realRND(r << 1)),
						FIRE_COLOR_FIRST + realRND(FIRE_PROCESS_COLOR_MAX),
						(15 << 8) + realRND(20 << 8),1 << 8);

			EffD.ConnectObject(p);
		};
	};
};

//---------------------------------------------------------------------------------------------------------------------------------------

/*void VangerUnit::CreateParticleMechos(const Vector& v,int r)
{
	TargetParticleObject* p;
	Vector vPos;
	int i,a,da,dr;
	unsigned char color_offset,color_shift;
	if(Visibility == VISIBLE){
		COLOR_OFFSET_TABLE[COLORS_IDS::BODY] = body_color_offset;
		COLOR_SHIFT_TABLE[COLORS_IDS::BODY] = body_color_shift;

		p = (TargetParticleObject*)(EffD.GetObject(EFF_PARTICLE02));
		if(p){
			a = RND(PI*2);
			da = 8 * PI  / model->num_poly;
			p->CreateParticle(v,ROTOR_PROCESS_LIFE_TIME,0);
			dr = r / 8;

			DBM A = A_l2g*scale_real;
			for(i = 0;i < model->num_poly;i++){
				vPos = Vector(dr + RND(r),0,0)*DBM(a,Z_AXIS);
				vPos += v;

				color_offset = COLOR_OFFSET_TABLE[model->polygons[i].color_id];
				color_shift = COLOR_SHIFT_TABLE[model->polygons[i].color_id];

				p->AddVertex2(vPos + Vector(RND(dr),RND(dr),0),
						Vector(1 - RND(2),1- RND(2),0) + v + (A*Vector(model->polygons[i].middle_x,model->polygons[i].middle_y,model->polygons[i].middle_z)),
						color_offset + (1 << (7 - color_shift)) - 1);

				p->AddVertex2(vPos + Vector(RND(dr),RND(dr),0),
						Vector(1 - RND(2),1- RND(2),0) + v + (A*Vector(model->polygons[i].middle_x,model->polygons[i].middle_y,model->polygons[i].middle_z)),
						color_offset + (1 << (7 - color_shift)) - 1);

				a = rPI(a + da);
			};

			EffD.ConnectObject(p);
		};
	};
};

void VangerUnit::CreateParticleRotor(const Vector& v,int r)
{
	TargetParticleObject* p;
	int i;
	unsigned char color_offset,color_shift;
	if(Visibility == VISIBLE){
		COLOR_OFFSET_TABLE[COLORS_IDS::BODY] = body_color_offset;
		COLOR_SHIFT_TABLE[COLORS_IDS::BODY] = body_color_shift;

		p = (TargetParticleObject*)(EffD.GetObject(EFF_PARTICLE02));
		if(p){
			p->CreateParticle(v,ROTOR_PROCESS_LIFE_TIME,1);

			for(i = 0;i < model->num_poly;i++){

				color_offset = COLOR_OFFSET_TABLE[model->polygons[i].color_id];
				color_shift = COLOR_SHIFT_TABLE[model->polygons[i].color_id];

				p->AddVertex(R_curr + (Vector(model->polygons[i].middle_x,model->polygons[i].middle_y,model->polygons[i].middle_z)*(A_scl)),
						color_offset + (1 << (7 - color_shift)) - 1,
						(2 << 8) + realRND(1 << 8),(3 << 7) + realRND(1 << 7));
				p->AddVertex(R_curr + (Vector(model->polygons[i].middle_x,model->polygons[i].middle_y,model->polygons[i].middle_z)*(A_scl)),
						color_offset + (1 << (7 - color_shift)) - 1,
						(2 << 8) + realRND(1 << 8),(3 << 7) + realRND(1 << 7));
			};


			for(i = 0;i < (p->NumParticle - model->num_poly*2);i++)
				p->AddVertex(v + Vector(r - realRND(r << 1),r - realRND(r << 1),r - realRND(r << 1)),
						FIRE_COLOR_FIRST + realRND(FIRE_PROCESS_COLOR_MAX),
						(15 << 8) + realRND(20 << 8),1 << 8);

			EffD.ConnectObject(p);
		};
	};
};*/

void ActionUnit::DeleteHandler(void)
{
};

void VangerUnit::DeleteHandler(void)
{
	Go2Universe();
};

void uvsUnitType::Init(void)
{
/*	int i;
	for(i = 0;i < UNIT_ORDER_NUM;i++){
		MaxTarget[i] = 1;
		TargetStore[i] = new UnitTargetType[MaxTarget[i]];
	};*/
};

void uvsUnitType::Free(void)
{
/*	int i;
	for(i = 0;i < UNIT_ORDER_NUM;i++)
		delete TargetStore[i];*/
};

void uvsUnitType::CreateUnitType(uvsVanger* p)
{
	uvsMechosType* sc;

	uvsPoint = p;
	uvsPoint->Ppassage = NULL;

	sc = uvsMechosTable[p->Pmechos->type];
	uvsMaxSpeed = sc->MaxSpeed;
	MaxArmor = sc->MaxArmor << 16;
	MaxEnergy = sc->MaxEnergy << 16;
//	dEnergy = MaxEnergy / sc->DeltaEnergy;
	dEnergy = ((MaxEnergy / 10) * sc->DeltaEnergy) / (UnitGlobalTime * 100);
	DropEnergy = MaxEnergy * sc->DropEnergy / 100;
	DelayDrop = sc->DropTime;
	ImpulsePower = (MaxEnergy / 100) * sc->MaxFly;

	NumDevice = 0;
	DeviceData = NULL;
	MaxVolume = 7;
	Volume = 0;
	Energy = MaxEnergy;
	Armor = MaxArmor;
	DropTime = 0;
	OxigenResource = MaxOxigenResource = sc->MaxOxigen;

	PowerFlag = sc->MaxFire;
//	PowerFlag = VANGER_POWER_RUFFA_GUN;
	aiPowerFlag = VANGER_POWER_NONE;

	MaxPassageCount = sc->MaxTeleport;

	PassageCount = uvsPoint->Pmechos->teleport & 0xff;
	
	if(PassageCount > MaxPassageCount) PassageCount = MaxPassageCount;
	if(PassageCount < 0) PassageCount = 0;

	uvsPoint->Pmechos->teleport &= ~0xff;
	uvsPoint->Pmechos->teleport |= PassageCount;

	DestroyClass = sc->MaxDamage;

	switch(uvsPoint->Pmechos->type){
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			ItemMatrix = &UnitMatrixData[UNIT_SUB_MATRIX_NUM + uvsPoint->Pmechos->type];
			break;
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
			ItemMatrix = &UnitMatrixData[23];
			break;
		case 19://Sandoll
			ItemMatrix = &UnitMatrixData[25];
			break;
		case 20://BTR
			ItemMatrix = &UnitMatrixData[26];
			break;
		case 21://Amphyb
			ItemMatrix = &UnitMatrixData[24];
			break;
		case 22://Helicopt
			ItemMatrix = &UnitMatrixData[27];
			break;
		case 23://Worm
			ItemMatrix = &UnitMatrixData[28];
			break;
	};	
};

void uvsUnitType::AddDevice(StuffObject* p)
{
	if(!DeviceData){
		p->NextDeviceList = p->PrevDeviceList = NULL;
		DeviceData = p;
	}else{
		DeviceData -> PrevDeviceList = p;
		p -> NextDeviceList = DeviceData;
		p -> PrevDeviceList = NULL;
		DeviceData = p;
	};
	NumDevice++;
	p->ActIntBuffer.slot = -1;
};

void uvsUnitType::DelDevice(StuffObject* p)
{
	if((!p->PrevDeviceList) && (!p->NextDeviceList)) DeviceData = NULL;
	else{
		if(p->PrevDeviceList) p->PrevDeviceList->NextDeviceList = p->NextDeviceList;
		else DeviceData = (StuffObject*)(p->NextDeviceList);
		if(p->NextDeviceList) p->NextDeviceList->PrevDeviceList = p->PrevDeviceList;
	};
	NumDevice--;
};

void uvsUnitType::Go2World(void)
{
	DeviceData = NULL;
};

void uvsUnitType::Go2Universe(void)
{
	StuffObject* p;
	StuffObject* pp;
	uvsItem* g;

	p = DeviceData;

	while(p){
		pp = p->NextDeviceList;
		ObjectDestroy(p);

		g = new uvsItem(p->uvsDeviceType);

		g->param1 = p->ActIntBuffer.data0;
		g->param2 = p->ActIntBuffer.data1;

		g->pos_x = p->R_curr.x;
		g->pos_y = p->R_curr.y;

		uvsPoint->addItem(g,1);

		p->Storage->Deactive(p);
		DelDevice(p);
		p = pp;
	};
//	if(DeviceData != NULL)
//		ErrH.Abort("Bad clear Device List");
//	uvsPoint = NULL;
};

int uvsMechosType_to_AciInt(int);


//znfo выход в мир
void iChatFinit(void);
void VangerUnit::Go2Universe(void)
{
	StuffObject* p;
	StuffObject* pp;
	uvsItem* g;
	uvsVanger* n;
	VangerUnit* l;
	int i;

#ifdef ITEM_LOG
	if(Status & SOBJ_ACTIVE){
		xLog < "\n\n\n\nGo2Universe:";
		p = DeviceData;
		while(p){
			xLog < "\n\nuvsType " <= (int)(p->uvsDeviceType);
			xLog < "\nActIntType " <= (int)(p->ActIntBuffer.type);
			xLog < "\nActIntData0 " <= (int)(p->ActIntBuffer.data0);
			xLog < "\nActIntData1 " <= (int)(p->ActIntBuffer.data1);
			p = (StuffObject*)(p->NextDeviceList);
		};
	};
#endif
//	ObjectDestroy(this);
	
	FreeList(uvsPoint->Pitem);

	if(Status & SOBJ_ACTIVE){
		if(NetworkON)
			iChatFinit();
		ItemD.LarvaHilator();

		uvsPoint->Pmechos->teleport = PassageCount | (Armor & 0xffffff00);
		aciDropMoveItem();
		if(vSetVangerFlag != -1){
			if(DeviceData){
				p = DeviceData;
				while(p){
					pp = p->NextDeviceList;
					ObjectDestroy(p);
					if(p->uvsDeviceType == UVS_ITEM_TYPE::SECTOR){
						CheckOutDevice(p);
						ActD.CheckDevice(p);
						aciRemoveItem(&(p->ActIntBuffer));
					}else{
						g = new uvsItem(p->uvsDeviceType);
						aciGetItemCoords(&(p->ActIntBuffer),g->pos_x,g->pos_y);
						g->param1 = p->ActIntBuffer.data0;
						g->param2 = p->ActIntBuffer.data1;
						uvsPoint->addItem(g,1);
					};
					p->Storage->Deactive(p);
					DelDevice(p);
					p = pp;
				};
//				if(DeviceData != NULL)
//					ErrH.Abort("Bad Device List");
			};

			if(vSetVangerFlag == 0){
				n = uvsMakeNewGamerInEscave(uvsPoint->Pworld->escT[0]);
				aciNewMechos(uvsMechosType_to_AciInt(n->Pmechos->type));
				if(!aciPutItem(ACI_BOOT_SECTOR))
					ErrH.Abort("Water Closet Droped");
				vSetVangerFlag = -1;
				g = new uvsItem(UVS_ITEM_TYPE::SECTOR);
				aciGetItemCoords(ACI_BOOT_SECTOR,g->pos_x,g->pos_y);
				n->addItem(g,1);
			}else{
				i = 1;
				l = (VangerUnit*)(ActD.Tail);
				while(l){
					if(l->uvsPoint->shape == UVS_VANGER_SHAPE::GAMER_SLAVE){
						if(i == vSetVangerFlag) break;
						else i++;
					};
					l = (VangerUnit*)(l->NextTypeList);
				};

				uvsMakeGamerFromSlave(l->uvsPoint);
				if(!l) ErrH.Abort("Gamer Slave Not Found");
				aciNewMechos(uvsMechosType_to_AciInt(l->uvsPoint->Pmechos->type));
			};
		}else{
			if(ChangeWorldConstraction != -1){
				if(ActD.SpummyRunner){
					p = DeviceData;
					while(p){
						pp = p->NextDeviceList;
						ObjectDestroy(p);							
						CheckOutDevice(p);
						ActD.CheckDevice(p);
						aciRemoveItem(&(p->ActIntBuffer));							
						p->Storage->Deactive(p);
						DelDevice(p);
						p = pp;
					};
				}else{
					if(DeviceData){
						p = DeviceData;
						while(p){
							pp = p->NextDeviceList;
							ObjectDestroy(p);
							g = new uvsItem(p->uvsDeviceType);
							aciGetItemCoords(&(p->ActIntBuffer),g->pos_x,g->pos_y);
							g->param1 = p->ActIntBuffer.data0;
							g->param2 = p->ActIntBuffer.data1;
							uvsPoint->addItem(g,1);
							p->Storage->Deactive(p);
							DelDevice(p);
							p = pp;
						};
					};
				};
				n = uvsMakeNewGamerInEscave(uvsPoint->Pworld->escT[0],0);
				aciNewMechos(uvsMechosType_to_AciInt(n->Pmechos->type));
/*				if(ActD.SpummyRunner){
					if(!aciPutItem(ACI_SPUMMY))
						ErrH.Abort("SPummy Must Die");					
					g = new uvsItem(UVS_ITEM_TYPE::SPUMMY);
					aciGetItemCoords(ACI_SPUMMY,g->pos_x,g->pos_y);
					n->addItem(g,1);
				};*/
			}else{
				p = DeviceData;
				while(p){
					pp = p->NextDeviceList;
					ObjectDestroy(p);
					g = new uvsItem(p->uvsDeviceType);
					aciGetItemCoords(&(p->ActIntBuffer),g->pos_x,g->pos_y);
					g->param1 = p->ActIntBuffer.data0;
					g->param2 = p->ActIntBuffer.data1;
					uvsPoint->addItem(g,1);
					p->Storage->Deactive(p);
					DelDevice(p);
					p = pp;
				};
//				if(DeviceData != NULL)
//					ErrH.Abort("Bad Device List");
				aciKillLinks();
			};
		};
	}else uvsUnitType::Go2Universe();

#ifdef ITEM_LOG
	if(Status & SOBJ_ACTIVE){
		g = (uvsItem*)(uvsPoint->Pitem);
		while(g){
			xLog < "\n\nuvsType " <= ((uvsItem*)(g))->type;
			xLog < "\nuvsParam1 " <= ((uvsItem*)(g))->param1;
			xLog < "\nuvsParam2 " <= ((uvsItem*)(g))->param2;
			g = (uvsItem*)(g->next);
		};
	};
#endif

	for(i = 0;i < ItemMatrix->NumSlot;i++)
		GunSlotData[ItemMatrix->nSlot[i]].CloseSlot();

	aiEvent.Free();
	aiResolveFind.Free();
	aiResolveAttack.Free();

	delete[] ItemMatrixData;
	delete MoleTool;
	delete MolePoint1;
	delete MolePoint2;
};

void VangerUnit::Go2World(void)
{
	int i;
	StuffObject* p;
	listElem* n;
	uvsItem* g;
	
	MoleTool = new dastPoly3D(Vector(0,0,0),Vector(0,0,0),Vector(0,0,0));
	MolePoint1 = new dastPoly3D(Vector(0,0,0),0,0);
	MolePoint2 = new dastPoly3D(Vector(0,0,0),0,0);

	ItemMatrixData = new int[ItemMatrix->FullNum];
	for(i = 0;i < ItemMatrix->FullNum;i++) ItemMatrixData[i] = 0;

	for(i = 0;i < MAX_ACTIVE_SLOT;i++){
		GunSlotData[i].pData = NULL;
		GunSlotData[i].Owner = NULL;
	};

	for(i = 0;i < ItemMatrix->NumSlot;i++)
		GunSlotData[ItemMatrix->nSlot[i]].OpenSlot(ItemMatrix->nSlot[i],this,ItemMatrix->nSlot[i]);

	aiEvent.Init();
	aiResolveFind.Init();
	aiResolveAttack.Init();

#ifdef ITEM_LOG
	if(Status & SOBJ_ACTIVE){
		xLog < "\n\n\nGo2World";
		g = (uvsItem*)(uvsPoint->Pitem);
		while(g){
			xLog < "\n\nuvsType " <= ((uvsItem*)(g))->type;
			xLog < "\nuvsParam1 " <= ((uvsItem*)(g))->param1;
			xLog < "\nuvsParam2 " <= ((uvsItem*)(g))->param2;
			g = (uvsItem*)(g->next);
		};
	};
#endif

	uvsUnitType::Go2World();

	if(vSetVangerFlag > 0 && (Status & SOBJ_ACTIVE)){
		Armor = uvsPoint->Pmechos->teleport & 0xffffff00;
		n = uvsPoint->Pitem;
		while(n){
			p = addDevice(((uvsItem*)(n))->pos_x,((uvsItem*)(n))->pos_y,0,((uvsItem*)(n))->type,((uvsItem*)(n))->param1,((uvsItem*)(n))->param2,this);
			if(!p) 
				ErrH.Abort("Stuff Memory Overflow");

//			if(p->ActIntBuffer.type == ACI_BOOT_SECTOR)
//				ErrH.Abort("Boot Sectot Error");

			if(!aciPutItem(p->ActIntBuffer.type,((uvsItem*)(n))->pos_x,((uvsItem*)(n))->pos_y))
				ErrH.Abort("Bad Put Item");		

			n = n->next;
		};

		p = DeviceData;
		while(p){
			p->ActIntBuffer.slot = -1;
//			aciReInitItem(&p->ActIntBuffer);
			aciReInitItemXY(&p->ActIntBuffer,p->R_curr.x,p->R_curr.y);
			p = p->NextDeviceList;
		};

		g = new uvsItem(UVS_ITEM_TYPE::SECTOR);
		uvsPoint->addItem(g,1);
		if(!aciPutItem(ACI_BOOT_SECTOR))
			ErrH.Abort("Water Closet Droped");
		p = addDevice(0,0,0,g->type,g->param1,g->param2,this);
		p->ActIntBuffer.slot = -1;
		aciReInitItem(&p->ActIntBuffer);
	}else{
		//stalkerg Или тут
		n = uvsPoint->Pitem;
		while(n){
			if(addDevice(((uvsItem*)(n))->pos_x,((uvsItem*)(n))->pos_y,0,((uvsItem*)(n))->type,((uvsItem*)(n))->param1,((uvsItem*)(n))->param2,this) == NULL)
				ErrH.Abort("Stuff Memory Overflow");
			n = n->next;
		};
		//stalkerg Кажется тут проблемма!
		if(Status & SOBJ_ACTIVE){
			p = DeviceData;
			while(p){
				p->ActIntBuffer.slot = -1;
				if(!aciReInitItemXY(&p->ActIntBuffer,p->R_curr.x,p->R_curr.y)){
					if(!aciPutItem(p->ActIntBuffer.type)){
						uvsKronDeleteItem(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
						ObjectDestroy(p);
						CheckOutDevice(p);
						ActD.CheckDevice(p);			
						p->Storage->Deactive(p);
						DelDevice(p);
					};
				};
				p = p->NextDeviceList;
			};
		};
	};

	FreeList(uvsPoint->Pitem);

#ifdef ITEM_LOG
	if(Status & SOBJ_ACTIVE){
		p = DeviceData;
		while(p){
			xLog < "\n\nuvsType " <= (int)(p->uvsDeviceType);
			xLog < "\nActIntType " <= (int)(p->ActIntBuffer.type);
			xLog < "\nActIntData0 " <= (int)(p->ActIntBuffer.data0);
			xLog < "\nActIntData1 " <= (int)(p->ActIntBuffer.data1);
			p = (StuffObject*)(p->NextDeviceList);
		};
	};
#endif
};

char uvsUnitType::CheckInDevice(StuffObject* p)
{
	if(Armor <= 0 || Energy <= DropEnergy) return CHECK_DEVICE_OUT;
	if(PowerFlag & VANGER_POWER_FLY) return CHECK_DEVICE_IN;
//	return CHECK_DEVICE_IN;
	return CHECK_DEVICE_OUT;
};

void uvsUnitType::CheckOutDevice(StuffObject* p)
{
};

char VangerUnit::CheckInDevice(StuffObject* p)
{
	StuffObject* n;
	int id;
	aiUnitResolve* l;

	if(p->uvsDeviceType == UVS_ITEM_TYPE::PROTRACTOR && ActD.ThreallDestroy)
		return CHECK_DEVICE_OUT;

	if(p->uvsDeviceType == UVS_ITEM_TYPE::KERNOBOO || p->uvsDeviceType == UVS_ITEM_TYPE::PIPETKA){
		id = p->uvsDeviceType;
		n = DeviceData;
		while(n){
			if(n->uvsDeviceType == id && n->ActIntBuffer.data1 < MAX_SEED_QUANT){
//				n->ActIntBuffer.data1++;				
				return CHECK_DEVICE_ADD;
			};
			n = n->NextDeviceList;
		};
	};	
	
	if(Armor <= 0 || Energy <= DropEnergy) return CHECK_DEVICE_OUT;
	if(!(Status & SOBJ_ACTIVE)){
		l = aiResolveFind.FindResolve(UNIT_ORDER_STUFF,UnitOrderType(p));
		if(l){
			ResolveFindDestroy(l);
			return CHECK_DEVICE_IN;
		}else{
			if(p->ActIntBuffer.type == ACI_DEGRADATOR || p->ActIntBuffer.type == ACI_AMPUTATOR || p->ActIntBuffer.type == ACI_MECHOSCOPE) return CHECK_DEVICE_IN;
			return CHECK_DEVICE_OUT;
		};
	};

	if(aciCheckItemPickUp(&(p->ActIntBuffer))){
		uvsDomChangeFromItem(p->uvsDeviceType, 1, 1);
		return CHECK_DEVICE_IN;
	};
	return CHECK_DEVICE_OUT;
};

int uvsCheckItemOnLive(int type);
void uvsOnKillItem(void);
void VangerUnit::CheckOutDevice(StuffObject* p)
{
	int i;
	if(Status & SOBJ_ACTIVE)
		uvsDomChangeFromItem(p->uvsDeviceType, 0, 1);

	if(!(Status & SOBJ_ACTIVE)){
		for(i = 0;i < MAX_ACTIVE_SLOT;i++)
			if(GunSlotData[i].pData && GunSlotData[i].ItemData == p) GunSlotData[i].CloseGun();
	};
	
	StuffObject* n;
	switch(p->ActIntBuffer.type){
		case ACI_PHLEGMA:
			i = -1;
			n = DeviceData;
			while(n){
				if(n->ActIntBuffer.type == ACI_PHLEGMA){
					i++;
					if(i == SAFE_STUFF_MAX) break;
				};
				n = n->NextDeviceList;
			};
			
			if(i < SAFE_STUFF_MAX){
				n = DeviceData;
				while(n){
					if(n->ActIntBuffer.type == ACI_WEEZYK && !uvsCheckItemOnLive(UVS_ITEM_TYPE::WEEZYK)){
						n->uvsDeviceType = UVS_ITEM_TYPE::DEAD_WEEZYK;
						n->ActIntBuffer.type = uvsSetItemType(n->uvsDeviceType,n->ActIntBuffer.data0,n->ActIntBuffer.data1);
						if(Status & SOBJ_ACTIVE){
							aciChangeItem(&(n->ActIntBuffer));
							uvsOnKillItem();
						};
					};
					n = n->NextDeviceList;
				};
			};
			break;
		case ACI_WEEZYK:
			if(!uvsCheckItemOnLive(UVS_ITEM_TYPE::WEEZYK)){
				p->uvsDeviceType = UVS_ITEM_TYPE::DEAD_WEEZYK;
				p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
				if(Status & SOBJ_ACTIVE) uvsOnKillItem();
			};
			break;
		case ACI_TOXICK:
			i = -1;
			n = DeviceData;
			while(n){
				if(n->ActIntBuffer.type == ACI_TOXICK){
					i++;
					if(i == SAFE_STUFF_MAX) break;
				};
				n = n->NextDeviceList;
			};

			if(i < SAFE_STUFF_MAX){
				n = DeviceData;
				while(n){
					if(n->ActIntBuffer.type == ACI_ELEECH && !uvsCheckItemOnLive(UVS_ITEM_TYPE::ELEECH)){
						n->uvsDeviceType = UVS_ITEM_TYPE::DEAD_ELEECH;
						n->ActIntBuffer.type = uvsSetItemType(n->uvsDeviceType,n->ActIntBuffer.data0,n->ActIntBuffer.data1);
						if(Status & SOBJ_ACTIVE){
							aciChangeItem(&(n->ActIntBuffer));
							uvsOnKillItem();
						};
					};
					n = n->NextDeviceList;
				};
			};
			break;
		case ACI_ELEECH:
			if(!uvsCheckItemOnLive(UVS_ITEM_TYPE::ELEECH)){
				p->uvsDeviceType = UVS_ITEM_TYPE::DEAD_ELEECH;
				p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
				if(Status & SOBJ_ACTIVE) uvsOnKillItem();
			};
			break;
		case ACI_PROTRACTOR:
			ActD.ClearProtractor();			
			break;
		case ACI_MECHANIC_MESSIAH:
			ActD.ClearMessiah();
			break;
		case ACI_PEELOT:
			if((Status & SOBJ_ACTIVE) && (Status & SOBJ_AUTOMAT)){
				Status &=~SOBJ_AUTOMAT;
				aiMessageQueue.Send(AI_MESSAGE_AUTOMATIC_OFF,0,0xff,0);
			};
			break;
	};
};


void VangerUnit::ItemQuant(void)
{
	StuffObject* p;
	StuffObject* pp;
	actintItemData* n;
	int i;
	
	if(!VangerChanger){
		if(NetworkON && !(Status & SOBJ_ACTIVE)){
			for(i = 0;i < ItemMatrix->NumSlot;i++){
				GunSlotData[ItemMatrix->nSlot[i]].NetStuffQuant();
				if(GunSlotData[ItemMatrix->nSlot[i]].pData)
					GunSlotData[ItemMatrix->nSlot[i]].Quant();
			};
			if(Armor <= 0) Destroy();
		}else{
			if(Armor <= 0){
				if(DeviceData){
					p = DeviceData;
					while(p){
						pp = (StuffObject*)(p->NextDeviceList);
						DestroyItem(p);
						p = pp;
					};
				};
				Destroy();
			}else{
				if(Energy < MaxEnergy){
					Energy += dEnergy;
					if(Energy >= MaxEnergy) Energy = MaxEnergy;
					else{
						DropTime--;
						if(Energy < DropEnergy){
							if(DropTime <= 0){
								DropTime = DelayDrop;
								if(DeviceData){
									if(Status & SOBJ_ACTIVE){
										n = aciGetLast();
										if((StuffObject*)(n->stuffOwner)) DischargeItem((StuffObject*)(n->stuffOwner));
									}else DischargeItem(DeviceData);
								};
							};
						};
					};
				}else Energy = MaxEnergy;

				for(i = 0;i < MAX_ACTIVE_SLOT;i++)
					if(GunSlotData[i].pData)
						GunSlotData[i].Quant();	
			};
		};
		
		if(NetworkON){
			p = DeviceData;
			if(Status & SOBJ_ACTIVE){
				while(p){
					pp = p->NextDeviceList;
					if(p->Status & SOBJ_WAIT_CONFIRMATION) p->NetOwnerQuant();
					else{
						switch(p->ActIntBuffer.type){
							case ACI_GLUEK:
								if(Armor < MaxArmor / 2){
									Armor += 10 << 16;
									ObjectDestroy(p);
									if(Status & SOBJ_ACTIVE){
										aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
										ActD.CheckDevice(p);
									};							
									CheckOutDevice(p);
									p->Storage->Deactive(p);
									DelDevice(p);						
								};
								break;
							default:
								p->DeviceQuant();
								break;
						};
						p = pp;
					};
				};
			}else{
				while(p){
					pp = p->NextDeviceList;
					if(p->Status & SOBJ_WAIT_CONFIRMATION)
						p->NetOwnerQuant();
					p->DeviceQuant();
					p = pp;
				};
			};
		}else{
			p = DeviceData;
			while(p){
				pp = p->NextDeviceList;
				switch(p->ActIntBuffer.type){
					case ACI_GLUEK:
						if(Armor < MaxArmor / 2){
							Armor += 10 << 16;
							ObjectDestroy(p);
							if(Status & SOBJ_ACTIVE){
								aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
								ActD.CheckDevice(p);
							};
							CheckOutDevice(p);
							p->Storage->Deactive(p);
							DelDevice(p);						
						};
						break;
					default:
						p->DeviceQuant();
						break;
				};
				p = pp;
			};
		};
	};
};

void BunchEvent(int type)
{
	GeneralObject* p;
	switch(type){
		case BUNCH_CHANGE_CYCLE:
			if(CurrentWorld >= MAIN_WORLD_MAX - 1) return;
			WorldPalCurrent = uvsCurrentCycle;
			if(WorldPalCurrent >= WorldPalNum) WorldPalCurrent = 0;
//			StartSetColor(100,WorldPalData[WorldPalCurrent]);
			PalCD.Set(CPAL_CHANGE_CYCLE,100,WorldPalData[WorldPalCurrent]);
			p = FarmerD.Tail;
			while(p){
//				if(p->ID != ID_SKYFARMER)
//					ErrH.Abort("Bad SkyFarmer List");
				((SkyFarmerObject*)(p))->skyfarmer_end();
				p = p->NextTypeList;
			};
			ActD.PromptChangeCycle();
			break;
	};
};

int Object::test_whether_there_is_not_something_above()
{
	unsigned char *p0,*p;
	int xx = (int)R.x;
	int yy = (int)R.y;
	int zz = (int)R.z;
	int D = radius;
	int x,y,lh,mh,uh;
	xx -= (xx - D) & 1;
	for(y = -D;y <= D;y++){
		p0 = vMap -> lineT[(y + yy) & clip_mask_y];
		if(!p0)
			continue;
		for(x = -D;x < D;x += 2){
			p = p0 + ((x + xx) & clip_mask_x);
			if((*(p + H_SIZE) & DOUBLE_LEVEL)){
				lh = *p;
				uh = *(p + 1);
				mh = (uh - lh > 130) ? lh + 110 : lh + 48;
				if(zz < mh)
					return 0;
				}
			}
		}
	return 1;
}



#define UPPER_THRESHOULD	32
#define LOWER_THRESHOULD       64
void section_prepare(int xg0,int yg0,int xg1,int yg1,int xg2,int yg2,int xg3,int yg3,int z_mechous,
		       double& A,double& B,double& Z_AVR,DBV& normal)
{
	int xl,xr,al,ar,bl,br,d,where,Y;
	int x1,x2;
	Vector *lfv,*rfv,*ltv,*rtv;
	Vector *curr,*beg,*up,*end;
	unsigned char *p0,*p;

	Vector vertices[4];
	vertices[0].x = xg0;
	vertices[0].y = yg0;
	vertices[1].x = xg1;
	vertices[1].y = yg1;
	vertices[2].x = xg2;
	vertices[2].y = yg2;
	vertices[3].x = xg3;
	vertices[3].y = yg3;

	beg = up = curr = vertices;
	end = vertices + 3;
	curr++;
	for(;curr <= end;curr++)
		if(up -> y > curr -> y)
			up = curr;

	rfv = lfv = up;

	if(up == beg)
		ltv = end;
	else
		ltv = up - 1;

	if(up == end)
		rtv = beg;
	else
		rtv = up + 1;

	Y = lfv -> y; xl = lfv -> x;
	al = ltv -> x - xl; bl = ltv -> y - Y;
	ar = rtv -> x - xl; br = rtv -> y - Y;
	xr = xl = (xl << 16) + (1 << 15);

	if(bl)
		DIV16(al,bl);

	if(br)
		DIV16(ar,br);

	int z,lh,uh;
	int N=0;
	double Sx=0,Sx2=0;
	double Sy=0,Sy2=0;
	double Sz=0;
	double Sxy = 0,Sxz=0,Syz=0;
	while(1){
		if(bl > br){
			d = br;
			where = 0;
			}
		else{
			d = bl;
			where = 1;
			}
		while(d-- > 0){
			p0 = vMap -> lineT[Y & clip_mask_y];
			if(!p0)
				goto cont;

			x1 = xl >> 16;
			x2 = xr >> 16;

			if(x1 > x2)
				SWAP(x1,x2);
			x1 &= ~1;
			x2 |= 1;

			while(x1 < x2){
				p = p0 + (x1 & clip_mask_x);
				if(!(*(p + 2*H_SIZE) & DOUBLE_LEVEL)){
					z = *p;
					if((uh = *(p + 1)) > z)
						z = uh;
					}
				else{
					lh = *p;
					uh = *(p + 1);
					z = z_mechous >= uh ? uh : lh;
					}

				if(z_mechous - z < LOWER_THRESHOULD && z - z_mechous < UPPER_THRESHOULD){
					N++;
					Sy += Y;
					Sy2 += Y*Y;
					Sx += x1;
					Sx2 += x1*x1;

					Sz += z;
					Sxz += x1*z;
					Syz += Y*z;
					}

				x1 += 2;
				}

		cont:
			Y++;
			xl += al;
			xr += ar;
			}

		if(where){
			if(ltv == rtv)
				goto ret;
			lfv = ltv;
			if(ltv == beg)
				ltv = end;
			else
				ltv--;

			br -= bl;

			xl = lfv -> x;
			al = ltv -> x - xl;
			bl = ltv -> y - Y;
			xl = (xl << 16) + (1 << 15);

			if(bl)
				DIV16(al,bl);
			}
		else{
			if(rtv == ltv)
				goto ret;
			rfv = rtv;
			if(rtv == end)
				rtv = beg;
			else
				rtv++;

			bl -= br;

			xr = rfv -> x;
			ar = rtv -> x - xr;
			br = rtv -> y - Y;
			xr = (xr << 16) + (1 << 15);

			if(br)
				DIV16(ar,br);
			}
		}
ret:
	if(!N){
		A = B = Z_AVR = 0;
		normal = DBV();
		return;
		}
	Z_AVR = Sz/N;
	double Det_x = (double)N*Sx2 - (double)Sx*Sx;
	if(fabs(Det_x) < DBL_EPS)
		return;
	double Det_y = (double)N*Sy2 - (double)Sy*Sy;
	if(fabs(Det_y) < DBL_EPS)
		return;
	A = ((double)N*Sxz - (double)Sx*Sz)/Det_x;
	B = ((double)N*Syz - (double)Sy*Sz)/Det_y;
	normal = DBV(-A,-B,1);
	normal.norm(1);
}

void aciActionEvent(void)
{
	if(ActD.Active)
		ActD.Active->DoorFlag = 1;
};

void ActionDispatcher::CreateActive(VangerUnit* p)
{
	int i;
	hMokKeyEnable = 0;
	XploKeyEnable = 0;
	Active = p;

	aiPutHotBug(aciGetCurCredits());
//	HotBug = aciCurCredits;

	aiCutLuck = GamerResult.luck + GamerResult.add_luck;
	aiCutDominance = GamerResult.dominance + GamerResult.add_dominance;
	if(aiCutLuck > 100) aiCutLuck = 100;
	else if(aiCutLuck < 0) aiCutLuck = 0;
	if(aiCutDominance > 100) aiCutDominance = 100;
	else if(aiCutDominance < -100) aiCutDominance = -100;

	for(i = 0;i < MAX_ACTIVE_SLOT;i++)
		Slot[i] = NULL;

	LocatorNum = 0;
	for(i = 0;i < LOCATOR_DATA_SIZE;i++)
		LocatorData[i] = NULL;
	LocatorPoint = NULL;
	LocatorOffset = -1;

	if(Active) DrawResourceMaxValue = MaxOxigenBar;
	else DrawResourceMaxValue = 0;
	DrawResourceValue = 0;
	DrawResourceTime = 0;
	SpummyRunner = 0;	
};

char* uvsGetNameByID(int type, int& ID);
void aciSendEvent2itmdsp(int code,actintItemData* p,int data)
{
	Vector vCheck;
	int i;
	int what,id;
	char* name_from;
	char* name_to;
	if(ActD.Active){
		switch(code){
			case ACI_DROP_ITEM:
				((VangerUnit*)(ActD.Active))->CheckOutDevice((StuffObject*)(p->stuffOwner));
				ActD.CheckDevice((StuffObject*)(p->stuffOwner));
				((StuffObject*)(p->stuffOwner))->DeviceOut(ActD.Active->R_curr + Vector(0,0,ActD.Active->radius*3));
				break;
			case ACI_ACTIVATE_ITEM:
				switch(p->type){
					case ACI_EMPTY_CIRTAINER:
						if(ActD.Active->dynamic_state & (GROUND_COLLISION | WHEELS_TOUCH)){
							ActD.Active->uvsPoint->gatherCirt(ActD.Active->R_curr.x,ActD.Active->R_curr.y,((StuffObject*)(p->stuffOwner))->ActIntBuffer.data0,((StuffObject*)(p->stuffOwner))->ActIntBuffer.data1);
							p->type = uvsSetItemType(((StuffObject*)(p->stuffOwner))->uvsDeviceType,((StuffObject*)(p->stuffOwner))->ActIntBuffer.data0,((StuffObject*)(p->stuffOwner))->ActIntBuffer.data1);
							aciChangeItem(p);
						};
						break;
					case ACI_CIRTAINER:						
						((StuffObject*)(p->stuffOwner))->ActIntBuffer.data0 = 0;
						((StuffObject*)(p->stuffOwner))->ActIntBuffer.data1 = 0;
						p->type = uvsSetItemType(((StuffObject*)(p->stuffOwner))->uvsDeviceType,((StuffObject*)(p->stuffOwner))->ActIntBuffer.data0,((StuffObject*)(p->stuffOwner))->ActIntBuffer.data1);
						aciChangeItem(p);						
						break;
					case ACI_PEELOT:
						if(ActD.Active->Status & SOBJ_AUTOMAT)
							aiMessageQueue.Send(AI_MESSAGE_AUTOMATIC_OFF,0,0xff,0);
						else							
							aiMessageQueue.Send(AI_MESSAGE_AUTOMATIC_ON,0,0xff,0);
						ActD.Active->Status ^= SOBJ_AUTOMAT;
						break;
					case ACI_GLUEK:
						(ActD.Active)->Armor += 10 << 16;
						ObjectDestroy((StuffObject*)(p->stuffOwner));
						(ActD.Active)->CheckOutDevice((StuffObject*)(p->stuffOwner));
						ActD.CheckDevice((StuffObject*)(p->stuffOwner));
						((StuffObject*)(p->stuffOwner))->Storage->Deactive((StuffObject*)(p->stuffOwner));
						(ActD.Active)->DelDevice((StuffObject*)(p->stuffOwner));
						aciSendEvent2actint(ACI_DROP_ITEM,p);
						break;				
					case ACI_TANKACID:
						ObjectDestroy((StuffObject*)(p->stuffOwner));
						for(i = 0;i < 3;i++){
							vCheck = Vector(40,0,0)*DBM(i*PI / 3,Z_AXIS);
							vCheck += ActD.Active->R_curr;
							cycleTor(vCheck.x,vCheck.y);
							MapD.CreateAcidSpot(vCheck,40,60,0,-16,40);	
						};
						MapD.CreateAcidSpot(ActD.Active->R_curr,40,60,0,-16,40);
						(ActD.Active)->CheckOutDevice((StuffObject*)(p->stuffOwner));
						ActD.CheckDevice((StuffObject*)(p->stuffOwner));
						((StuffObject*)(p->stuffOwner))->Storage->Deactive((StuffObject*)(p->stuffOwner));
						(ActD.Active)->DelDevice((StuffObject*)(p->stuffOwner));
						aciSendEvent2actint(ACI_DROP_ITEM,p);					
						SOUND_ACID() 
						break;
					default:
						p->flags |= ACI_ACTIVE;
						ActD.SlotIn(data,(StuffObject*)(p->stuffOwner));
						break;
				};
				break;
			case ACI_DEACTIVATE_ITEM:
				p->flags &= ~ACI_ACTIVE;
				ActD.SlotOut((StuffObject*)(p->stuffOwner));
				break;
			case ACI_SHOW_ITEM_TEXT:
				if(p->type == ACI_TABUTASK_SUCCESSFUL){
					RaceTxtBuff.init();
					if(lang() == RUSSIAN){
//GERMAN
						RaceTxtBuff < uvsGetNameByID(p->data1 & 0xffff,i);
						RaceTxtBuff < rFirstTabuTaskMessage;
						RaceTxtBuff < rSecondTabuTaskMessage;
						RaceTxtBuff <= TabuTable[p->data1 & 0xffff]->cash < "$";
//ENGLISH
/*
						RaceTxtBuff < rFirstTabuTaskMessage;
						RaceTxtBuff < uvsGetNameByID(p->data1 & 0xffff,i);
						RaceTxtBuff < rSecondTabuTaskMessage;
						RaceTxtBuff <= TabuTable[p->data1 & 0xffff]->cash < "$";*/
					}else{
						RaceTxtBuff < uvsGetNameByID(p->data1 & 0xffff,i);
						RaceTxtBuff < FirstTabuTaskMessage;
						RaceTxtBuff < SecondTabuTaskMessage;
						RaceTxtBuff <= TabuTable[p->data1 & 0xffff]->cash < "$";
					};
					aciPrepareText(RaceTxtBuff.address());
				}else{
					if(uvsPrepareItemToDiagen(((StuffObject*)(p->stuffOwner))->uvsDeviceType,p->data0,p->data1,what,id,name_from,name_to))
						aciPrepareText(dgD->getInvText(what,id,name_from,name_to));
				};
				break;
		};
	};
};

void ActionDispatcher::SlotIn(int n,StuffObject* p)
{	
	Slot[n] = p;
	switch(p->StuffType){
		case DEVICE_ID_GUN:
			Active->GunSlotData[n].OpenGun((GunDevice*)(p));
			if(uvsKronActive && !GameD.FirstQuant) uvsCheckKronIventTabuTask(UVS_KRON_EVENT::WEAPON_ON,1);
			break;
	};	
};

void ActionDispatcher::SlotOut(StuffObject* p)
{
	int i;
	for(i = 0;i < MAX_ACTIVE_SLOT;i++)
		if(Active && Slot[i] && p == Slot[i]) break;

	if(i == MAX_ACTIVE_SLOT) 
		return;

	Slot[i]->Deactive();
	Slot[i] = NULL;

	switch(p->StuffType){
		case DEVICE_ID_GUN:
			Active->GunSlotData[i].CloseGun();			
			break;
	};
	p->ActIntBuffer.slot = -1;
};

void ActionDispatcher::CheckDevice(StuffObject* p)
{
	int i;
	for(i = 0;i < MAX_ACTIVE_SLOT;i++){
		if(Slot[i] == p){
			if(p->ActIntBuffer.slot >= 0 && p->ActIntBuffer.slot < MAX_SLOTS)
				Active->GunSlotData[p->ActIntBuffer.slot].CloseGun();
			Slot[i] = NULL;
			p->ActIntBuffer.slot = -1;
			return;
		};
	};
};

void ActionDispatcher::DeactiveAllSlots(void)
{
	int i;
	for(i = 0;i < MAX_ACTIVE_SLOT;i++)
		if(Slot[i]) Slot[i]->Deactive();
};

void ActionDispatcher::ActiveAllSlots(void)
{
	int i;
	for(i = 0;i < MAX_ACTIVE_SLOT;i++)
		ActiveSlot(i);
};

void ActionDispatcher::ActiveAllTerminator(void)
{
	int i;
	for(i = 0;i < MAX_ACTIVE_SLOT;i++)
		ActiveTerminatorSlot(i);
};

void ActionDispatcher::ActiveSlot(int n)
{
	if(Slot[n] && Active && Slot[n]->StuffType == DEVICE_ID_GUN && !isTerminator(Slot[n]->ActIntBuffer.type))
		Active->GunSlotData[n].Fire();
};

void ActionDispatcher::ActiveTerminatorSlot(int n)
{
	if(Slot[n] && Active && Slot[n]->StuffType == DEVICE_ID_GUN && isTerminator(Slot[n]->ActIntBuffer.type))
		Active->GunSlotData[n].Fire();
};

actintItemData::actintItemData(void)
{
	type = -1;
	data0 = data1 = maxData = -1;
	flags = 0;
	slot = -1;
};

actintItemData::actintItemData(int tp)
{
	type = tp;
	data0 = data1 = maxData = -1;

	flags = 0;
	slot = -1;

//	if(tp <= ACI_TERMINATOR) flags |= ACI_WEAPON;
};

StuffObject* GetStuffObject(uvsUnitType* g,int n)
{
	StuffObject* p;
	p = g->DeviceData;
	while(p){
		if(p->ActIntBuffer.type == n) return p;
		p = p->NextDeviceList;
	};
	return NULL;
};

void ActionDispatcher::AddFireResource(void)
{
/*	if(FireResource < MaxFireResource){
		FireResource++;
		SOUND_CHARGE();
	};*/
};

void uvsUnitType::AddOxigenResource(void)
{
	if(OxigenResource < MaxOxigenResource - 10){
		OxigenResource += 10;
		if(ActD.Active->Status & SOBJ_ACTIVE) ActD.DrawResourceValue = OxigenResource;
	};
};

extern int aciCurDvcResource;
extern int aciMaxDvcResource;

void aOutStr(int x,int y,int font,int color,unsigned char* str,int space);


void ActionDispatcher::DrawResource(void)
{
	int i;
	int y0,x0,sx;
	int x1;
	int cclx,ccly,ccrx,ccry;

	cclx = XGR_Obj.clipLeft;
	ccly = XGR_Obj.clipTop;
	ccrx = XGR_Obj.clipRight;
	ccry = XGR_Obj.clipBottom;

	XGR_SetClip(UcutLeft,VcutUp,UcutRight,VcutDown);

	y0 = VcutDown - RES_DRAW_DOWN;
	x0 = UcutRight - RES_DRAW_LEFT - mechosCameraOffsetX;
	x1 = UcutLeft + RES_DRAW_LEFT;
	sx = x0 - x1;

	if(DrawResourceValue > 0){
		XGR_Rectangle(x0 - sx * DrawResourceValue / DrawResourceMaxValue,y0,sx * DrawResourceValue / DrawResourceMaxValue,RES_DRAW_STEP_Y,228,228,XGR_FILLED);
		DrawResourceValue = -DrawResourceValue;
		DrawResourceTime = 130;
	}else{
		if(DrawResourceTime > 0){
			XGR_Rectangle(x0 + sx * DrawResourceValue / DrawResourceMaxValue,y0,-sx * DrawResourceValue / DrawResourceMaxValue,RES_DRAW_STEP_Y,228,228,XGR_FILLED);
			DrawResourceTime -= 128 / 20;
		};
	};

	if(Active){
		aciMaxDvcResource = 1;
		aciCurDvcResource = 0;

		for(i = 0;i < MAX_ACTIVE_SLOT;i++){
			if(Slot[i] && (Slot[i]->ActIntBuffer.type == ACI_COPTE_RIG || Slot[i]->ActIntBuffer.type == ACI_CROT_RIG || Slot[i]->ActIntBuffer.type == ACI_CUTTE_RIG)){
				aciCurDvcResource = Slot[i]->ActIntBuffer.data1;
				aciMaxDvcResource = uvsItemTable[Slot[i]->uvsDeviceType]->param2 + 1;
				break;
			};
		};
	};

	if(SkyQuakeEnable)
		SkyQuakeEnable = SkyQuake.quant(3);

	if(SkyQuakeEnable2){
		SkyQuakeEnable2 = SkyQuake2.quant(5);
		if(!SkyQuakeEnable2 && FunctionSpobsDestroyActive && ActD.Active){
			SkyQuake2.set(ActD.Active->R_scr.x,ActD.Active->R_scr.y,20,SKY_QUAKE_RADIUS,SKY_QUAKE_DELTA);
			SkyQuakeEnable2 = 1;
		};
	};

	if(SkyQuakeEnable3){
		SkyQuake3.set(ScreenCX,ScreenCY,PALLADIUM_RADIUS,PALLADIUM_RADIUS);
		SkyQuakeEnable3 = SkyQuake3.quant(5);
	};

/*     	v = (PhantomUnit*)(PhantomD.Tail);
	while(v){
		if(v->MyName && v->Visibility == VISIBLE)
			aOutStr(v->R_scr.x,v->R_scr.y + v->rmax_screen,4,224,(unsigned char*)(v->MyName),1);
		v = (PhantomUnit*)(v->NextTypeList);
	};*/

	XGR_SetClip(cclx,ccly,ccrx,ccry);

//	const int DCC[4] = {239,143,159,175};
//	for(i = 0;i < TntTableSize;i++){
//		putMapPixel(TntObjectData[i]->R_curr.x,TntObjectData[i]->R_curr.y,DCC[TntObjectData[i]->MasterID]);
//		putMapPixel(TntObjectData[i]->R_curr.x + 1,TntObjectData[i]->R_curr.y + 1,DCC[TntObjectData[i]->MasterID]);
//		putMapPixel(TntObjectData[i]->R_curr.x - 1,TntObjectData[i]->R_curr.y - 1,DCC[TntObjectData[i]->MasterID]);
//		putMapPixel(TntObjectData[i]->R_curr.x + 1,TntObjectData[i]->R_curr.y - 1,DCC[TntObjectData[i]->MasterID]);
//		putMapPixel(TntObjectData[i]->R_curr.x - 1,TntObjectData[i]->R_curr.y + 1,DCC[TntObjectData[i]->MasterID]);
//	};
};

void uvsUnitType::UseOxigenResource(void)
{
	if(OxigenResource > 0){
		OxigenResource--;
		if(ActD.Active && ActD.Active->Status & SOBJ_ACTIVE) ActD.DrawResourceValue = OxigenResource;
	}else{
		Armor = 0;
		Energy = 0;
	};
};

int CheckStartJump(Object* p)
{
	return ((VangerUnit*)(p))->CheckStartJump();
};

int VangerUnit::CheckStartJump(void)
{	
	int st;
	st = (MaxEnergy - ImpulsePower) / max_jump_power;
	if(Energy >  ImpulsePower){
		Energy -= st;
		jump_power++;
		if(jump_power > max_jump_power)
			return 1;
		else
			return 0;
	};
	return 1;
};

const int COMPAS_RIGHT = 80;
const int COMPAS_LEFT = 60;
const int COMPAS_UP = 60;
const int COMPAS_DOWN = 80;

const int MAX_COMPAS_DELTA = 7;
const int MAX_COMPAS_SPEED = 10;

extern listElem* EscaveTail;
extern listElem* SpotTail;
extern listElem* PassageTail;

void CompasObject::Init(void)
{
	FrameCount = 0;
	vMove = Vector(0,0,0);
	CurrentTarget = NULL;
};

void CompasObject::Open(void)
{
	CompasTargetType* p;
	uvsEscave* pe;
	uvsSpot* ps;
	uvsPassage* pp;

	p = TargetData;
	while(p){
		switch(p->ID){
			case CMP_TARGET_ESCAVE:
				p->ID = CMP_OBJECT_ESCAVE;
				pe = (uvsEscave*)EscaveTail;
				while(pe){
					if(!strcmp(pe->name,p->Name)){
						p->Data.EnterT = pe->unitPtr.EnterT;
						break;
					};
					pe = (uvsEscave*)(pe -> next);
				};
				if(pe == NULL) ErrH.Abort("Bad Compas Target Open");
//				p->Data.EnterT = ((p->Data.TargetT)->unitPtr).EnterT;
				break;
			case CMP_TARGET_SPOT:
				p->ID = CMP_OBJECT_SPOT;
				ps = (uvsSpot*)SpotTail;
				while(ps){
					if(!strcmp(ps->name,p->Name)){
						p->Data.EnterT = ps->unitPtr.EnterT;
						break;
					};
					ps = (uvsSpot*)(ps -> next);
				};
				if(ps == NULL) ErrH.Abort("Bad Compas Target Open");
//				p->Data.EnterT = ((p->Data.TargetT)->unitPtr).EnterT;
				break;
			case CMP_TARGET_PASSAGE:
				p->ID = CMP_OBJECT_PASSAGE;
				pp = (uvsPassage*)PassageTail;
				while(pp){
					if(!strcmp(pp->name,p->Name)){
						p->Data.PassageT = pp->unitPtr.PassageT;
						break;
					};
					pp = (uvsPassage*)(pp -> next);
				};
				if(pp == NULL) ErrH.Abort("Bad Compas Target Open");
//				p->Data.PassageT = ((p->Data.TargetT)->unitPtr).PassageT;
				break;
			case CMP_TARGET_SENSOR:
				p->ID = CMP_OBJECT_SENSOR;
				p->Data.SensorT = FindSensor(p->Name);
				break;
		};
		if(!p->Data.SensorT)
			ErrH.Abort("Error in Compas Target Open");
		p = p->Next;
	};
};

void CompasObject::Close(void)
{
	CurrentTarget = NULL;
	CompasTargetType* p;
	CompasTargetType* gp;
	p = TargetData;
	while(p){
		gp = p->Next;
		DeleteTarget(p);
		p = gp;
	};
};

void CompasObject::Free(void)
{
};

void ClearTabutaskTarget(void)
{
	int r_log = 0;
	CompasTargetType* p;
	CompasTargetType* pp;

	p = CompasObj.TargetData;
	while(p){
		pp = p->Next;
		if(p->ID == CMP_OBJECT_VANGER){
			if(p == CompasObj.TargetData)
				SelectCompasTarget(NULL);
			CompasObj.DeleteTarget(p);
			r_log = 1;
		};
		p = pp;
	};
	if(r_log) aciRefreshTargetsMenu();	
};

void ClearPhantomTarget(int id)
{	
	CompasTargetType* p;
	CompasTargetType* pp;

	p = CompasObj.TargetData;
	while(p){
		pp = p->Next;
		if(p->ID == CMP_OBJECT_PHANTOM && (p->Data).ID == id){
			if(CompasObj.CurrentTarget == p) SelectCompasTarget(NULL);
			CompasObj.DeleteTarget(p);
			aciRefreshTargetsMenu();
			return;
		};
		p = pp;
	};
};

void AddPhantomTarget(PlayerData* n)
{	
	CompasTargetType* p;
	p = CompasObj.TargetData;
	while(p){
		if(p->ID == CMP_OBJECT_PHANTOM && (p->Data).ID == n->client_ID)
			break;
		p = p->Next;
	};
	if(!p)
		CompasObj.AddTarget(CMP_OBJECT_PHANTOM,UnitOrderType(n->client_ID),NULL,n->name);
};

void CreateArtefactTarget(StuffObject* n)
{
	CompasTargetType* p;
	p = CompasObj.TargetData;
	while(p){
		if(p->ID == CMP_OBJECT_ITEM && (p->Data).StuffT == n)
			break;
		p = p->Next;
	};

	if(!p){
		if(lang() == RUSSIAN){
			switch(n->ActIntBuffer.type){
				case ACI_PROTRACTOR:
					CompasObj.AddTarget(CMP_OBJECT_ITEM,UnitOrderType(n),NULL,rProtractorCompasTarget);
					break;
				case ACI_MECHANIC_MESSIAH:
					CompasObj.AddTarget(CMP_OBJECT_ITEM,UnitOrderType(n),NULL,rMessiahCompasTarget);
					break;
				case ACI_FUNCTION83:
					CompasObj.AddTarget(CMP_OBJECT_ITEM,UnitOrderType(n),NULL,rFunction83CompasTarget);
					break;
				case ACI_SPUMMY:
					CompasObj.AddTarget(CMP_OBJECT_ITEM,UnitOrderType(n),NULL,rSpummyCompasTarget);
					break;
			};
		}else{
			switch(n->ActIntBuffer.type){
				case ACI_PROTRACTOR:
					CompasObj.AddTarget(CMP_OBJECT_ITEM,UnitOrderType(n),NULL,ProtractorCompasTarget);
					break;
				case ACI_MECHANIC_MESSIAH:
					CompasObj.AddTarget(CMP_OBJECT_ITEM,UnitOrderType(n),NULL,MessiahCompasTarget);
					break;
				case ACI_FUNCTION83:
					CompasObj.AddTarget(CMP_OBJECT_ITEM,UnitOrderType(n),NULL,Function83CompasTarget);
					break;
				case ACI_SPUMMY:
					CompasObj.AddTarget(CMP_OBJECT_ITEM,UnitOrderType(n),NULL,SpummyCompasTarget);
					break;
			};
		};		
	};
};

void DeleteArtefactTarget(StuffObject*n)
{
	CompasTargetType* p;
	p = CompasObj.TargetData;
	while(p){
		if(p->ID == CMP_OBJECT_ITEM && (p->Data).StuffT == n){
			if(CompasObj.CurrentTarget == p) SelectCompasTarget(NULL);
			CompasObj.DeleteTarget(p);			
			aciRefreshTargetsMenu();
			return;
		};			
		p = p->Next;
	};
};

void CreateTabutaskTarget(void)
{
	int i;
	CompasTargetType* n;
	UnitOrderType c;

	for(i = 0;i < MAX_TABUTASK;i++){
		if(TabuTable[i]->status == UVS_TABUTASK_STATUS::ACTIVE){
			switch(TabuTable[i]->target){
				case UVS_TARGET::VANGER:
					c.TargetT = TabuTable[i]->Ptarget;
					n = CompasObj.TargetData;
					while(n){
						if(n->ID == CMP_OBJECT_VANGER){
							if(n->Data.TargetT == c.TargetT)
								break;
						};
						n = n->Next;
					};
					if(!n){		
						RaceTxtBuff.init();
						if(lang() == RUSSIAN) RaceTxtBuff < rTaskCompasTarget <= i;
						else RaceTxtBuff < TaskCompasTarget <= i;

						CompasObj.AddTarget(CMP_OBJECT_VANGER,UnitOrderType(c),NULL,RaceTxtBuff.GetBuf());
						SelectCompasTarget(RaceTxtBuff.GetBuf());
						aciRefreshTargetsMenu();
					};
					break;
			};
		};
	};
};

void CreatePhantomTarget(void)
{
	CompasTargetType* n;
	uvsPassage* pass;

	if(my_server_data.GameType == PASSEMBLOSS && UsedCheckNum < GloryPlaceNum){
		n = CompasObj.TargetData;
		while(n){	
			if(n -> ID == CMP_OBJECT_VECTOR){
				if(GloryPlaceData[UsedCheckNum].World == CurrentWorld){
					(n->Data).vT.x = GloryPlaceData[UsedCheckNum].R_curr.x;
					(n->Data).vT.y = GloryPlaceData[UsedCheckNum].R_curr.y;
					(n->Data).vT.z = GloryPlaceData[UsedCheckNum].R_curr.z;					
				}else{
					pass = GetPassage(CurrentWorld,GloryPlaceData[UsedCheckNum].World);
					if(pass){
						(n->Data).vT.x = pass->unitPtr.PassageT->ActionLink->R_curr.x;
						(n->Data).vT.y = pass->unitPtr.PassageT->ActionLink->R_curr.y;
						(n->Data).vT.z = pass->unitPtr.PassageT->ActionLink->R_curr.z;
					};
				};
				break;
			};
			n = n->Next;
		};

		if(!n){
			if(lang() == RUSSIAN){
				CompasObj.AddTarget(CMP_OBJECT_VECTOR,UnitOrderType(GloryPlaceData[UsedCheckNum].R_curr.x,GloryPlaceData[UsedCheckNum].R_curr.y,GloryPlaceData[UsedCheckNum].R_curr.z),NULL,rCheckPointCompasTarget);
				SelectCompasTarget(rCheckPointCompasTarget);
			}else{
				CompasObj.AddTarget(CMP_OBJECT_VECTOR,UnitOrderType(GloryPlaceData[UsedCheckNum].R_curr.x,GloryPlaceData[UsedCheckNum].R_curr.y,GloryPlaceData[UsedCheckNum].R_curr.z),NULL,CheckPointCompasTarget);
				SelectCompasTarget(CheckPointCompasTarget);
			};
			aciRefreshTargetsMenu();
		};
	};
};

void CompasObject::Quant(void)
{
	int tx,ty;
	int d;
	Vector v;
	Vector R_curr;
	PlayerData* p;
	uvsVanger* tt;
	uvsPassage* pass;
	int dx,dy;	

	if(!CurrentTarget || !ActD.Active || ActD.Active->ExternalMode != EXTERNAL_MODE_NORMAL || !(ActD.Active->ExternalDraw)) return;

	RaceTxtBuff.init();
	switch(CurrentTarget->ID){
		case CMP_OBJECT_ESCAVE:
		case CMP_OBJECT_SPOT:
			R_curr	= (((CurrentTarget->Data).EnterT)->GetEnterCenter(ActD.Active->R_curr)->ActionLink)->R_curr;
			break;
		case CMP_OBJECT_PASSAGE:
			R_curr = ((CurrentTarget->Data).PassageT)->ActionLink->R_curr;
			break;
		case CMP_OBJECT_SENSOR:
			R_curr = ((CurrentTarget->Data).SensorT)->R_curr;
			break;
		case CMP_OBJECT_PHANTOM:
			p = players_list.first();
			while(p){
				if((CurrentTarget->Data).ID == p->client_ID){
					R_curr.x = p->x;
					R_curr.y = p->y;
					R_curr.z = 0;
					RaceTxtBuff < p->name < " : ";
					break;
				};
				p = (PlayerData*)p -> next;
			};
			break;
		case CMP_OBJECT_ITEM:
			if(CurrentTarget->Data.StuffT->Owner){
				R_curr.x = CurrentTarget->Data.StuffT->Owner->R_curr.x;
				R_curr.y = CurrentTarget->Data.StuffT->Owner->R_curr.y;
			}else{
				R_curr.x = CurrentTarget->Data.StuffT->R_curr.x;
				R_curr.y = CurrentTarget->Data.StuffT->R_curr.y;
			};
			R_curr.z = 0;
			break;
		case CMP_OBJECT_VANGER:
			tt = (uvsVanger*)(CurrentTarget->Data.TargetT);
			if(tt ->Pworld->gIndex == CurrentWorld){
				R_curr.x = tt->pos_x;
				R_curr.y = tt->pos_y;
				R_curr.z = 0;
			}else{
				pass = GetPassage(CurrentWorld,tt ->Pworld->gIndex);
				if(pass){
					R_curr.x = pass->unitPtr.PassageT->ActionLink->R_curr.x;
					R_curr.y = pass->unitPtr.PassageT->ActionLink->R_curr.y;
					R_curr.z = pass->unitPtr.PassageT->ActionLink->R_curr.z;
				};
			};
			break;
		case CMP_OBJECT_VECTOR:
			R_curr.x = (CurrentTarget->Data).vT.x;
			R_curr.y = (CurrentTarget->Data).vT.y;
			R_curr.z = (CurrentTarget->Data).vT.z;
			if(NetworkON && my_server_data.GameType == PASSEMBLOSS && UsedCheckNum < GloryPlaceNum){
				if(GloryPlaceData[UsedCheckNum].World == CurrentWorld)
					RaceTxtBuff < "C : ";
				else
					RaceTxtBuff < "P : ";
			};
			break;
		default: return;
	};

//zmod 1.21
	if (CurrentWorld <= 2) {
		v = Vector(getDistX(R_curr.x,x),R_curr.y - y,0);
	} else {
		v = Vector(getDistX(R_curr.x,x),getDistY(R_curr.y,y),0);
	}

	d = v.vabs();
	if(d){
		vMove += v * MAX_COMPAS_DELTA / d;
		d = vMove.vabs();
		if(d) vMove = vMove * (MAX_COMPAS_SPEED * XTCORE_FRAME_NORMAL / d);
	};

	v = Vector(ActD.Active->Speed,0,0)*ActD.Active->RotMat;
	x = XCYCL(x + vMove.x + v.x);
	y = YCYCL(y + vMove.y + v.y);

	G2LQ(Vector(x,y,0), tx, ty);
	if(tx < UcutLeft + COMPAS_LEFT){
		tx = UcutLeft + COMPAS_LEFT;
		vMove.x = 0;
	};

	if(tx > UcutRight - COMPAS_RIGHT - mechosCameraOffsetX * 2){
		tx = UcutRight - COMPAS_RIGHT - mechosCameraOffsetX * 2;
		vMove.x = 0;
	};

	if(ty < VcutUp + COMPAS_UP){
		ty = VcutUp + COMPAS_UP;
		vMove.y = 0;
	};

	if(ty > VcutDown - COMPAS_DOWN){
		ty = VcutDown - COMPAS_DOWN;
		vMove.y = 0;
	};

	EffD.FireBallData[2].Show2(tx,ty,FrameCount);
	EffD.FireBallData[2].CheckOut(FrameCount);

	S2G(tx,ty,x,y);

	int cclx,ccly,ccrx,ccry,ccm;

	ccm = XGR_Obj.ClipMode;
	XGR_SetClipMode(XGR_CLIP_ALL);
	cclx = XGR_Obj.clipLeft;
	ccly = XGR_Obj.clipTop;
	ccrx = XGR_Obj.clipRight;
	ccry = XGR_Obj.clipBottom;
	XGR_SetClip(UcutLeft,VcutUp,UcutRight,VcutDown);

	if(ActD.Active){
		dx = getDistX(R_curr.x,ActD.Active->R_curr.x);
		if(CurrentWorld < MAIN_WORLD_MAX - 1)
			dy = R_curr.y - ActD.Active->R_curr.y;
		else		
			dy = getDistY(R_curr.y,ActD.Active->R_curr.y);
	}else{
		dx = 0;
		dy = 0;
	};

	RaceTxtBuff <= (int)(sqrt(dx*(double)dx + dy*(double)dy)) / 100;
	dx = ((ScreenCX - tx)*(int)(strlen(RaceTxtBuff.address()))*10 + 32) / (int)(curGMap -> xside);
	dy = ((ScreenCY - ty)*18 + 32) / (int)(curGMap -> yside);
	aOutText32clip(tx + dx,ty + dy,165 | (2 << 16),RaceTxtBuff.address(),0,0,0);
/*	if(ty < ScreenCY){
		if(tx < ScreenCX)
			aOutText32clip(tx + 32,ty + 32,165 | (2 << 16),RaceTxtBuff.address(),0,0,0);
		else
		      aOutText32clip(tx - strlen(RaceTxtBuff.address())*10,ty + 32,165 | (2 << 16),RaceTxtBuff.address(),0,0,0);
	}else{
		if(tx < ScreenCX)
			aOutText32clip(tx + 32,ty - 18,165 | (2 << 16),RaceTxtBuff.address(),0,0,0);
		else
			aOutText32clip(tx - strlen(RaceTxtBuff.address())*10,ty - 18,165 | (2 << 16),RaceTxtBuff.address(),0,0,0);
	};*/
	XGR_SetClip(cclx,ccly,ccrx,ccry);
	XGR_SetClipMode(ccm);
};

void CompasObject::AddTarget(int id,UnitOrderType d,char* n1,const char* n2)
{
	CompasTargetType* p;

	p = TargetData;
	while(p){
		if(!strcmp(p->aciName,n2)) return;
		p = p->Next;
	};

	p = new CompasTargetType;

	if(!TargetData){
		p->Next = p->Prev = NULL;
		TargetData = p;
	}else{
		TargetData -> Prev = p;
		p -> Next = TargetData;
		p -> Prev = NULL;
		TargetData = p;
	};
	
	p->ID = id;
	if(n1){
		p->Name = new char[strlen(n1) + 1];
		strcpy(p->Name,n1);
	}else p->Name = NULL;

	if(n2){
		p->aciName = new char[strlen(n2) + 1];
		strcpy(p->aciName,n2);
		aciAdd2Targets(p->aciName);
	}else p->aciName = NULL;

	p->Data.ActionT = d.ActionT;
	p->dFlag = 0;
};

void CompasObject::DeleteTarget(CompasTargetType* p)
{
	if(p->Name) delete[] p->Name;
	if(p->aciName){
		aciDeleteFromTargets(p->aciName);
		delete[] p->aciName;
	};
	if((!p->Prev) && (!p->Next)) TargetData = NULL;
	else{
		if(p->Prev) p->Prev->Next = p->Next;
		else TargetData = p->Next;
		if(p->Next) p->Next->Prev = p->Prev;
	};
	delete p;
};

void AddTarget2Compas(int type,void* point, const char* head)
{
	switch(type){
		case CMP_TARGET_ESCAVE:
			CompasObj.AddTarget(type,(uvsEscave*)(NULL),(char*)(point),head);
			break;
		case CMP_TARGET_SPOT:
			CompasObj.AddTarget(type,(uvsSpot*)(NULL),(char*)(point),head);
			break;
		case CMP_TARGET_PASSAGE:
			CompasObj.AddTarget(type,(uvsPassage*)(NULL),(char*)(point),head);
			break;
		case CMP_TARGET_SENSOR:
			CompasObj.AddTarget(type,(ActionUnit*)(NULL),(char*)(point),head);
			break;
	};
};

void SelectCompasTarget(const char * n)
{
	CompasTargetType* p;
	if(ActD.Active) ActD.Active->aiResolveFind.ClearResolve();
	if(!n){
		CompasObj.CurrentTarget = NULL;
		return;
	};
	p = CompasObj.TargetData;
	while(p){
		if(!strcmp(n,p->aciName)){
			CompasObj.CurrentTarget = p;
			return;
		};
		p = p->Next;
	};
	CompasObj.CurrentTarget = NULL;
};

char* GetCompasTarget(void)
{
	if(CompasObj.CurrentTarget){
		return CompasObj.CurrentTarget->aciName;
	};
	return NULL;
};

//d - расстояние до Object'а
//a - угол между направлением движения MECHOS'a  и Object'ом


void CharacterDataType::GetInternalStuffOrder(int d,int a,int item,StuffObject* s,VangerUnit* p,VangerUnit* n)
{
	float d_inv;

	Aggressia = 0;
	Hunting = 0;

	if(s->ActIntBuffer.type == item) d_inv = 8 / (float)(d);
	else d_inv = 1 / (float)(d);
	Hunting = round(FetishMaina * (float)(4 - GetNodeDist(p,n)) * d_inv);
	Aggressia = round(Hunting * RobberFactor);
};

void CharacterDataType::GetStuffOrder(int d,int a,int item,StuffObject* s)
{
	float d_inv;

	Aggressia = 0;
	Hunting = 0;

	if(s->ActIntBuffer.type == item) d_inv = 8 / (float)(d);
	else d_inv = 1 / (float)(d);

	Hunting = round(FetishMaina * d_inv);
	Aggressia = 0;
};

void CharacterDataType::GetVangerOrder(int d,int a,VangerUnit* p,VangerUnit* n)
{
	float d_inv;

	Aggressia = 0;
	Hunting = 0;

	d_inv = 1 / (float)(d);
	Aggressia = round(KillerInstinct * (float)(4 - GetNodeDist(n,p)) * d_inv);
	Hunting = round(Aggressia * Aggressia2Hunting);
};

extern dastPoly3D terra_moving_tool;

void VangerUnit::MoleProcessQuant(void)
{
	Vector left_border,right_border;
	double dx = xmax_real;
	Vector delta(round(A_l2g[0]*dx),round(A_l2g[1]*dx),round(A_l2g[2]*dx));
	if(sqrt((double)(sqr(R_curr.x - R_prev.x) + sqr(R_curr.y - R_prev.y))) > 2){
		MoleTool->set(R_curr - delta, R_curr + delta,Vector(0,0,0));
		MoleTool->make_mole();
		}
};

void ActionDispatcher::AddFunction(VangerFunctionType* p)
{
	if(!fTail){
		p->Next = p->Prev = NULL;
		fTail = p;
	}else{
		fTail -> Prev = p;
		p -> Next = fTail;
		p -> Prev = NULL;
		fTail = p;
	};
};

void ActionDispatcher::DeleteFunction(VangerFunctionType* p)
{
	if((!p->Prev) && (!p->Next)) fTail = NULL;
	else{
		if(p->Prev) p->Prev->Next = p->Next;
		else fTail = p->Next;
		if(p->Next) p->Next->Prev = p->Prev;
	};
};

void VangerFunctionType::Init(int _ID,Vector _vR,int _Time,int _External)
{
	ID = _ID;
	vR = _vR;
	LifeTime = Time = _Time;
	Next = Prev =NULL;	

	if(_External){
		switch(ID){
			case PROTRACTOR_SCALE_UP:									
			case PROTRACTOR_SCALE_DOWN:
			case PROTRACTOR_BEEBOS_DANCE:			
			case PROTRACTOR_MOLERIZATOR:						
			case PROTRACTOR_PALLADIUM:
			case PROTRACTOR_JESTEROID:
				Time = LifeTime - SIGNATOR_DELAY + 1;
				break;
			case MECHANIC_BEEB_NATION:
			case MECHANIC_UNVISIBLE:			
				Time = LifeTime - SKY_QUAKE_DELAY + 1;
				break;
		};
	}else{
		switch(ID){
			case PROTRACTOR_OPEN_SPODS:
				ActD.SignEngine->set(vR,4);
				SOUND_PROCTRACTOR_START()
				break;
			case PROTRACTOR_SCALE_UP:	
				ActD.SignEngine->set(vR,6);
				SOUND_PROCTRACTOR_START()
				break;
			case PROTRACTOR_SCALE_DOWN:
				ActD.SignEngine->set(vR,6);
				SOUND_PROCTRACTOR_START()
				break;		
			case MECHANIC_BEEB_NATION:
				if(ActD.mfActive == ActD.Active){
					SkyQuake.set(ScreenCX,ScreenCY,200,SKY_QUAKE_RADIUS,SKY_QUAKE_DELTA);
					SkyQuakeEnable = 1;
				};
				SOUND_MESSIAH();
				break;
			case MECHANIC_UNVISIBLE:
				if(ActD.mfActive == ActD.Active){
					SkyQuake.set(ScreenCX,ScreenCY,80,SKY_QUAKE_RADIUS,SKY_QUAKE_DELTA);
					SkyQuakeEnable = 1;
				};
				SOUND_MESSIAH();
				break;
			case MECHANIC_FIRE_GARDEN:
				if(ActD.mfActive == ActD.Active){
					SkyQuake.set(ScreenCX,ScreenCY,50,SKY_QUAKE_RADIUS,SKY_QUAKE_DELTA);
					SkyQuakeEnable = 1;
				};
				SOUND_MESSIAH();
				break;
			case MECHANIC_ITEM_FALL:
				if(ActD.mfActive == ActD.Active){
					SkyQuake.set(ScreenCX,ScreenCY,20,SKY_QUAKE_RADIUS,SKY_QUAKE_DELTA);
					SkyQuakeEnable = 1;
				};
				SOUND_MESSIAH();
				break;
			case PROTRACTOR_BEEBOS_DANCE:
				ActD.SignEngine->set(vR,7);
				SOUND_PROCTRACTOR_START()
				break;
			case PROTRACTOR_MOLERIZATOR:
				ActD.SignEngine->set(vR,9);
				SOUND_PROCTRACTOR_START()
				break;
			case PROTRACTOR_PALLADIUM:
				ActD.SignEngine->set(vR,3);
				SOUND_PROCTRACTOR_START()
				break;
			case PROTRACTOR_PREPASSAGE:
				ActD.SignEngine->set(vR,5);
				SOUND_PROCTRACTOR_START()
				break;
			case PROTRACTOR_JESTEROID:
				ActD.SignEngine->set(vR,8);
				SOUND_PROCTRACTOR_START()
				break;
		};
	};
};

void ActionDispatcher::ClearProtractor(void)
{
	VangerFunctionType* p;
	p = fTail;	
	while(p){
		if(p->ID <= PROTRACTOR_PREPASSAGE){
			p->Time = 0;
			p->Quant();
		};
		p = p->Next;
	};
};

void ActionDispatcher::ClearMessiah(void)
{
	VangerFunctionType* p;
	p = fTail;
	while(p){
		if(p->ID > PROTRACTOR_PREPASSAGE){
			p->Time = 0;
			p->Quant();
		};
		p = p->Next;
	};
};

int ActionDispatcher::NewFunction(int id,int tp)
{
	VangerFunctionType* p;
	p = fTail;
	if(id == PROTRACTOR_PREPASSAGE){
		while(p){
			if(p->ID == PROTRACTOR_PREPASSAGE)
				return 0;
			if(p->ID <= id)
				p->Time = 0;
			if(p->ID == tp)
				return 0;
			p = p->Next;
		};
		if(tp == PROTRACTOR_MOLERIZATOR && CurrentWorld == WORLD_HMOK)
			return 0;
	}else{
		while(p){
			if(p->ID > PROTRACTOR_PREPASSAGE &&  p->ID <= id) p->Time = 0;
			if(p->ID == tp){
				if(ActD.mfActive == ActD.Active){
					SkyQuake2.set(ScreenCX,ScreenCY,20,SKY_QUAKE_RADIUS,SKY_QUAKE_DELTA);
					SkyQuakeEnable2 = 1;
				};
				return 0;
			};
			p = p->Next;
		};
	};
	return 1;
};

void VangerFunctionType::SoundQuant(void)
{
	if(ID <= PROTRACTOR_PREPASSAGE) {
		if(Time <= LifeTime - SIGNATOR_DELAY) {
			SOUND_PROCTRACTOR_STOP();
		}
	};
};

void VangerFunctionType::Quant(void)
{
//	GeneralObject* p;
	BulletObject* n;
	int i,a;
	Vector vCheck,vTrack;
	VangerUnit* g;
	StuffObject* l;	
	StuffObject* ll;
	
	Time--;	
	SoundQuant();
	if(Time <= 0){
		switch(ID){
			case PROTRACTOR_PALLADIUM:
				SOUND_PR_BARRIER();
				SkyQuakeEnable3 = 0;
				break;
			case PROTRACTOR_MOLERIZATOR:
				if(ActD.pfActive){
					ActD.pfActive->Molerizator = 0;
					ActD.pfActive->LastMole = 256;
				};
				break;
			case PROTRACTOR_SCALE_DOWN:
			case PROTRACTOR_SCALE_UP:
				if(ActD.pfActive)
					ActD.pfActive->scale_size = ActD.pfActive->original_scale_size;
				SOUND_PR_RESIZE();
				break;
			case MECHANIC_UNVISIBLE:
				if(ActD.mfActive)
					((VangerUnit*)(ActD.mfActive))->set_draw_mode(NORMAL_DRAW_MODE);
				SOUND_MES_CLOAK();
				break;
			case PROTRACTOR_PREPASSAGE:
				SOUND_PR_FUNCTION_STOP();
				if(ActD.pfActive){
					((VangerUnit*)(ActD.pfActive))->ExternalDraw = 1;
					ActD.pfActive->switch_analysis(0);
				};
				break;
			case MECHANIC_BEEB_NATION:
				if(ActD.mfActive){
					((VangerUnit*)(ActD.mfActive))->convert_to_beeb(NULL);
					ActD.mfActive->BeebonationFlag = 0;
				};
				SOUND_MES_BEEBSOSPY();
				break;
			case PROTRACTOR_BEEBOS_DANCE:
				vInsectTarget = Vector(-1,-1,-1);
				break;
			case PROTRACTOR_OPEN_SPODS:
				if(!ActD.SpobsDestroy) 
					ActD.XploKeyEnable = 1;
				break;
			case MECHANIC_ITEM_FALL:				
				if(ActD.mfActive){
					if(NetworkON){
						g = (VangerUnit*)(ActD.Tail);
						while(g){
							if(g->Visibility == VISIBLE && g->ID == ID_VANGER && g != ActD.mfActive && (g->Status & SOBJ_ACTIVE)){
								l = g->DeviceData;
								while(l){
									ll = (StuffObject*)(l->NextDeviceList);
									g->CheckOutDevice(l);									
									ActD.CheckDevice(l);
									aciRemoveItem(&(l->ActIntBuffer));									
									l->DeviceOut(g->R_curr + Vector(0,0,g->radius*4));
									l = ll;
								};
							};
							g = (VangerUnit*)(g->NextTypeList);
						};
					}else{
						g = (VangerUnit*)(ActD.Tail);
						while(g){
							if(g->Visibility == VISIBLE && g->ID == ID_VANGER && g != ActD.mfActive){
								l = g->DeviceData;
								while(l){
									ll = (StuffObject*)(l->NextDeviceList);
									g->CheckOutDevice(l);
									if(g->Status & SOBJ_ACTIVE){
										ActD.CheckDevice(l);
										aciRemoveItem(&(l->ActIntBuffer));
									};
									l->DeviceOut(g->R_curr + Vector(0,0,g->radius*4));
									l = ll;
								};
							};
							g = (VangerUnit*)(g->NextTypeList);
						};
					};
				};
				break;
		};
		ID = FUNCTION_OFF;
	}else{
		switch(ID){
			case PROTRACTOR_JESTEROID:
				if(ActD.pfActive && Time < LifeTime - SIGNATOR_DELAY){
					if(ActD.pfActive->R_curr.z < 400) ActD.pfActive->impulse(Vector(0,0,64),RND(25),0);
					else ActD.pfActive->impulse(Vector(0,0,0),1,RND(50));
					SoundFlag |= SoundCopterig;
				};
				break;
			case PROTRACTOR_MOLERIZATOR:
				if(ActD.pfActive){
					if(Time < LifeTime - SIGNATOR_DELAY){
						if(!(ActD.pfActive->mole_on)){
							ActD.pfActive->Molerizator = 1;
							if(Time < LifeTime - 2){
								ActD.pfActive->set_3D(SET_3D_DIRECT_PLACE,vR.x,vR.y,-32,0,-((VangerUnit*)(ActD.pfActive))->Angle,0);								
								SoundFlag |= SoundCrotrig;
							};
						};
					}else{
						if(Time == LifeTime - SIGNATOR_DELAY){
							ActD.pfActive->set_3D(SET_3D_DIRECT_PLACE,vR.x,vR.y,-32,0,-((VangerUnit*)(ActD.pfActive))->Angle,0);
							for(i = 0;i < MOLERIZATOR_NUM;i++)
								MapD.CreateDust(Vector(XCYCL(vR.x + MOLERIZATOR_RADIUS - RND(2*MOLERIZATOR_RADIUS)),YCYCL(vR.y + MOLERIZATOR_RADIUS - RND(2*MOLERIZATOR_RADIUS)),0),MAP_DUST_PROCESS);
							ActD.pfActive->StartMoleProcess();
							ActD.pfActive->Molerizator = 1;
						};
					};
				};
				break;
			case PROTRACTOR_SCALE_UP:
				if(ActD.pfActive && Time == LifeTime - SIGNATOR_DELAY){
					ActD.pfActive->scale_size = ActD.pfActive->original_scale_size * 2;
					SOUND_PR_RESIZE();
				};
				break;
			case PROTRACTOR_SCALE_DOWN:
				if(ActD.pfActive && Time == LifeTime - SIGNATOR_DELAY){
					ActD.pfActive->scale_size = ActD.pfActive->original_scale_size / 2;
					SOUND_PR_RESIZE();
				};
				break;		
			case PROTRACTOR_BEEBOS_DANCE:
				if(ActD.pfActive && Time == LifeTime - SIGNATOR_DELAY)
					vInsectTarget = vR;
				break;
			case PROTRACTOR_PREPASSAGE:
				if(ActD.pfActive && Time == LifeTime - SIGNATOR_DELAY){
					SOUND_PR_FUNCTION_START();
					((VangerUnit*)(ActD.pfActive))->ExternalDraw = 0;
					ActD.pfActive->switch_analysis(1);
					ActD.pfActive->CreateParticleMechos(ActD.pfActive->R_curr,300,LifeTime - SIGNATOR_DELAY);
				};
				break;
			case MECHANIC_ITEM_FALL:
				if(ActD.mfActive && Time == LifeTime - SKY_QUAKE_TIME){
					if(ActD.mfActive == ActD.Active){
						SkyQuake2.set(ScreenCX,ScreenCY,20,SKY_QUAKE_RADIUS,SKY_QUAKE_DELTA);
						SkyQuakeEnable2 = 1;
					};
				};
				break;
			case MECHANIC_BEEB_NATION:
				if(ActD.mfActive){
					if(Time == LifeTime - SKY_QUAKE_TIME){
						if(ActD.mfActive == ActD.Active){
							SkyQuake2.set(ScreenCX,ScreenCY,20,SKY_QUAKE_RADIUS,SKY_QUAKE_DELTA);
							SkyQuakeEnable2 = 1;
						};
					}else{
						if(Time == LifeTime - SKY_QUAKE_DELAY){
							SOUND_MES_BEEBSOSPY();
							ActD.mfActive->BeebonationFlag = 1;
							((VangerUnit*)(ActD.mfActive))->convert_to_beeb(&(ModelD.ActiveModel(ModelD.FindModel("Bug"))));
							switch(((VangerUnit*)(ActD.mfActive))->uvsPoint->Pmechos->color){
								case 0:			
									((VangerUnit*)(ActD.mfActive))->set_body_color(COLORS_IDS::BODY_GREEN);
									break;
								case 1:
									((VangerUnit*)(ActD.mfActive))->set_body_color(COLORS_IDS::BODY_RED);			
									break;
								case 2:
									((VangerUnit*)(ActD.mfActive))->set_body_color(COLORS_IDS::BODY_BLUE);
									break;
								case 3:
									((VangerUnit*)(ActD.mfActive))->set_body_color(COLORS_IDS::BODY_YELLOW);
									break;
								case 4:
									((VangerUnit*)(ActD.mfActive))->set_body_color(COLORS_IDS::BODY_CRIMSON);
									break;
								case 5:
									((VangerUnit*)(ActD.mfActive))->set_body_color(COLORS_IDS::BODY_GRAY);
									break;
								case 6:
									((VangerUnit*)(ActD.mfActive))->set_body_color(COLORS_IDS::ROTTEN_ITEM);
									break;
								case 7:
									((VangerUnit*)(ActD.mfActive))->set_body_color(COLORS_IDS::MATERIAL_3);
									break;
								case 8:
									((VangerUnit*)(ActD.mfActive))->set_body_color(COLORS_IDS::MATERIAL_1);
									break;
								case 9:
									((VangerUnit*)(ActD.mfActive))->set_body_color(COLORS_IDS::MATERIAL_0);
									break;
							};
						};
					};
				};
				break;
			case MECHANIC_UNVISIBLE:
				if(ActD.mfActive){
					if(Time == LifeTime - SKY_QUAKE_TIME){
						if(ActD.mfActive == ActD.Active){
							SkyQuake2.set(ScreenCX,ScreenCY,20,SKY_QUAKE_RADIUS,SKY_QUAKE_DELTA);
							SkyQuakeEnable2 = 1;
						};
					}else{
						if(Time == LifeTime - SKY_QUAKE_DELAY){
							SOUND_MES_CLOAK();
							((VangerUnit*)(ActD.mfActive))->set_draw_mode(TRANSPARENCY_DRAW_MODE);
							ObjectDestroy(ActD.mfActive,0);
						};
					};
				};
				break;
			case MECHANIC_FIRE_GARDEN:
				if(ActD.mfActive){
					if(Time > LifeTime - SKY_QUAKE_DELAY){
						if(Time == LifeTime - SKY_QUAKE_TIME){
							if(ActD.mfActive == ActD.Active){
								SkyQuake2.set(ScreenCX,ScreenCY,20,SKY_QUAKE_RADIUS,SKY_QUAKE_DELTA);
								SkyQuakeEnable2 = 1;
							};
						};
					}else{
						SOUND_MES_FIRE();
						a = 2*PI * Time / (LifeTime - SKY_QUAKE_DELAY);
						n = BulletD.CreateBullet();
						vCheck = Vector(ActD.mfActive->radius*4,0,0) * DBM(a,Z_AXIS);
						vCheck += ActD.mfActive->R_curr;
						cycleTor(vCheck.x,vCheck.y);

						vTrack = Vector(ActD.mfActive->radius*3,0,0) * DBM(a,Z_AXIS);
						vTrack += ActD.mfActive->R_curr;					
						cycleTor(vTrack.x,vTrack.y);
						n->CreateBullet(vTrack,vCheck,NULL,&GameBulletData[WD_BULLET_FIRE_GARDEN],NULL);
					};
				};
				break;
			case PROTRACTOR_PALLADIUM:
				if(ActD.pfActive){
					vR = ActD.pfActive->R_curr;
					if(Time == LifeTime - SIGNATOR_DELAY){
						SOUND_PR_BARRIER();
						if(ActD.pfActive == ActD.Active){
//							SkyQuake3.set(ScreenCX,ScreenCY,PALLADIUM_RADIUS,PALLADIUM_RADIUS);
							SkyQuakeEnable3 = 1;
						};
					};
				};
				break;
		};
	};
};

void VangerUnit::SetMechos(int n)
{
	Object::operator = (ModelD.ActiveModel(n));
	set_active(1);
	set_3D(SET_3D_CHOOSE_LEVEL,R_curr.x,R_curr.y,R_curr.z,0,-Angle,Speed);
};

/*void ActionDispatcher::ClearFunction(int type)
{
	VangerFunctionType* p;
	p = fTail;
	while(p){

		p = p->Next;
	};
};*/


void ActionDispatcher::AddCoolFunction(int _ID,Vector _vR,int _Time,int _External)
{
	VangerFunctionType* p;
	p = new VangerFunctionType;
	p->Init(_ID,_vR,_Time,_External);
	AddFunction(p);
};

void ActionDispatcher::FunctionQuant(void)
{
	VangerFunctionType* p;
	VangerFunctionType* pp;
	unsigned char ch;
	int m_flag,p_flag;
	int p_new,m_new;

	m_flag = 0;
	p_flag = 0;
	if(NetworkON){
		if(pfActive){
			ch = pfActive->NetFunction & (64 | 7);
			if(NetFunctionProtractor != ch){
				if(ch & 64){
					aciProtractorEvent = (ch & 7) + 1;					
					if(GetDistTime(NetGlobalTime,pfActive->NetProtractorFunctionTime) > 256*SIGNATOR_DELAY / 20)
						p_flag = 1;
				}else
					NewFunction(PROTRACTOR_PREPASSAGE,(ch & 7) + 1);				
				NetFunctionProtractor = ch;
			};
		};

		if(mfActive){
			ch = mfActive->NetFunction & (128 | (7 << 3));
			if(NetFunctionMessiah != ch){
				if(ch & 128){
					aciMechMessiahEvent = ((ch >> 3) & 7) + 1;					
					if(GetDistTime(NetGlobalTime,mfActive->NetMessiahFunctionTime) > 256*SKY_QUAKE_DELAY / 20)
						m_flag = 1;
				}else
					NewFunction(MECHANIC_BEEB_NATION,((ch >> 3) & 7) + 1);
				NetFunctionMessiah = ch;
			};
		};
	};

	if(FunctionSpobsDestroyActive > 0) FunctionSpobsDestroyActive--;

	if(FunctionThreallDestroyActive > 0)
		FunctionThreallDestroyActive--;

	if(FunctionThreallDestroyActive > 0){
		if(ActD.Active){
			if(!ProtractorLight) ProtractorLight = MapD.CreateLight(ActD.Active->R_curr.x,ActD.Active->R_curr.y,512,ActD.Active->radius*2,63,LIGHT_TYPE::DYNAMIC);
			else ProtractorLight->set_position(ActD.Active->R_curr.x,ActD.Active->R_curr.y,512);
		};
	}else{
		if(ProtractorLight){
			ProtractorLight->Destroy();
			ProtractorLight = NULL;
		};
	};

	if(aciProtractorEvent){
		if(pfActive){			
			FunctionThreallDestroyActive = 0;
			p_new = 0;

			switch(aciProtractorEvent){
				case ACI_PROTRACTOR_EVENT1:
					if(!NetworkON && NewFunction(PROTRACTOR_PREPASSAGE,PROTRACTOR_OPEN_SPODS)){
						AddCoolFunction(PROTRACTOR_OPEN_SPODS,pfActive->R_curr,SIGNATOR_DELAY,p_flag);
						p_new = 1;
					};
					break;
				case ACI_PROTRACTOR_EVENT2:
					if(NewFunction(PROTRACTOR_PREPASSAGE,PROTRACTOR_SCALE_DOWN)){
						AddCoolFunction(PROTRACTOR_SCALE_DOWN,pfActive->R_curr,0xffffff + SIGNATOR_DELAY,p_flag);
						p_new = 1;
					};
					break;
				case ACI_PROTRACTOR_EVENT3:
					if(NewFunction(PROTRACTOR_PREPASSAGE,PROTRACTOR_PALLADIUM)){
						AddCoolFunction(PROTRACTOR_PALLADIUM,pfActive->R_curr,0xffffff + SIGNATOR_DELAY,p_flag);
						p_new = 1;
					};
					break;
				case ACI_PROTRACTOR_EVENT4:
					if(NewFunction(PROTRACTOR_PREPASSAGE,PROTRACTOR_BEEBOS_DANCE)){
						AddCoolFunction(PROTRACTOR_BEEBOS_DANCE,pfActive->R_curr,0xffffff + SIGNATOR_DELAY,p_flag);
						p_new = 1;
					};
					break;
				case ACI_PROTRACTOR_EVENT5:
					if(NewFunction(PROTRACTOR_PREPASSAGE,PROTRACTOR_JESTEROID)){
						AddCoolFunction(PROTRACTOR_JESTEROID,pfActive->R_curr,0xffffff + SIGNATOR_DELAY,p_flag);
						p_new = 1;
					};
					break;
				case ACI_PROTRACTOR_EVENT6:
					if(NewFunction(PROTRACTOR_PREPASSAGE,PROTRACTOR_MOLERIZATOR)){
						AddCoolFunction(PROTRACTOR_MOLERIZATOR,pfActive->R_curr,500 + SIGNATOR_DELAY,p_flag);
						p_new = 1;
					};
					break;
				case ACI_PROTRACTOR_EVENT7:					
					if(NewFunction(PROTRACTOR_PREPASSAGE,PROTRACTOR_PREPASSAGE)){
						if(!p_flag){
							AddCoolFunction(PROTRACTOR_PREPASSAGE,pfActive->R_curr,200 + SIGNATOR_DELAY,p_flag);
							p_new = 1;
						};
					};
					break;
				case ACI_PROTRACTOR_EVENT8:
					if(!NetworkON)
						FunctionThreallDestroyActive = GAME_OVER_EVENT_TIME;
					break;
			};

			if(NetworkON && pfActive == Active){
				NetFunctionProtractor &= ~7;
				if(p_new) NetFunctionProtractor |= 64;
				else NetFunctionProtractor &= ~64;
				NetFunctionProtractor |= (aciProtractorEvent - 1) & 7;
				pfActive->ShellUpdateFlag = 1;
				pfActive->NetProtractorFunctionTime = NetGlobalTime;
			};
		};
		aciProtractorEvent = 0;
	};

	if(aciMechMessiahEvent){
		if(mfActive){	
			FunctionSpobsDestroyActive = 0;
			m_new = 0;

			switch(aciMechMessiahEvent){
				case ACI_MECH_MESSIAH_EVENT1:
					if(NewFunction(MECHANIC_BEEB_NATION,MECHANIC_UNVISIBLE)){
						AddCoolFunction(MECHANIC_UNVISIBLE,mfActive->R_curr,SKY_QUAKE_DELAY + 0xffffff,m_flag);
						m_new = 1;
					};
					break;
				case ACI_MECH_MESSIAH_EVENT2:
					if(!NetworkON && NewFunction(MECHANIC_BEEB_NATION,MECHANIC_GAME_OVER))
						XpeditionOFF(GAME_OVER_LUCKY);
					break;
				case ACI_MECH_MESSIAH_EVENT3:
					if(NewFunction(MECHANIC_BEEB_NATION,MECHANIC_BEEB_NATION)){
						AddCoolFunction(MECHANIC_BEEB_NATION,mfActive->R_curr,SKY_QUAKE_DELAY + 0xffffff,m_flag);
						m_new = 1;
					};
					break;
				case ACI_MECH_MESSIAH_EVENT4:					
					if(NewFunction(MECHANIC_BEEB_NATION,MECHANIC_FIRE_GARDEN)){
						if(!m_flag){
							AddCoolFunction(MECHANIC_FIRE_GARDEN,mfActive->R_curr,SKY_QUAKE_DELAY + 15,m_flag);
							m_new = 1;
						};
					};
					break;
				case ACI_MECH_MESSIAH_EVENT5:					
					if(NewFunction(MECHANIC_BEEB_NATION,MECHANIC_ITEM_FALL)){
						if(!m_flag){
							AddCoolFunction(MECHANIC_ITEM_FALL,mfActive->R_curr,SKY_QUAKE_DELAY,m_flag);
							m_new = 1;
						};
					};
					break;
				case ACI_MECH_MESSIAH_EVENT6:
					if(!NetworkON){
						FunctionSpobsDestroyActive = GAME_OVER_EVENT_TIME;
						if(ActD.Active){
							SkyQuake2.set(ActD.Active->R_scr.x,ActD.Active->R_scr.y,20,SKY_QUAKE_RADIUS,SKY_QUAKE_DELTA);
							SkyQuakeEnable2 = 1;
						};
						SOUND_MESSIAH();
					};
					break;
			};

			if(NetworkON && mfActive == Active){
				NetFunctionMessiah &= ~(7 << 3);
				if(m_new) NetFunctionMessiah |= 128;
				else NetFunctionMessiah &= ~128;
				NetFunctionMessiah |= ((aciMechMessiahEvent - 1) & 7) << 3;
 				mfActive->ShellUpdateFlag = 1;
				mfActive->NetMessiahFunctionTime = NetGlobalTime;
			};
		};
		aciMechMessiahEvent = 0;
	};

	p = fTail;
	while(p){
		p->Quant();
		p = p->Next;
	};

	p = fTail;
	while(p){
		pp = p->Next;
		if(p->ID == FUNCTION_OFF){
			DeleteFunction(p);
			delete p;
		};
		p = pp;
	};
};

void uvsDolly::ActiveQuant(void)
{
	BaseObject* p;
	StuffObject* n;
	int dm,d;
	Vector vCheck;

	pos_x = (pos_x + x_speed) & (Pworld -> x_size - 1);
	pos_y = (pos_y + y_speed) & (Pworld -> y_size - 1);

	dm = -1;
	n = NULL;
	p = (BaseObject*)(ItemD.Tail);
	while(p){
		if(p->ID == ID_STUFF && ((StuffObject*)(p))->ActIntBuffer.type == ACI_EMPTY_CONLARVER && ((StuffObject*)(p))->FindDolly){
			vCheck = Vector(getDistX(p->R_curr.x,pos_x),getDistY(p->R_curr.y,pos_y),0);
			d = vCheck.vabs();
			if(d < dm || dm == -1){
				dm = d;
				n = (StuffObject*)p;
			};
		};
		p = (BaseObject*)(p->NextTypeList);
	};

	if(n && dm < 80){
		vCheck = Vector(getDistX(n->R_curr.x,pos_x),getDistY(n->R_curr.y,pos_y),0);
		vCheck = 2*vCheck * V_SPEED_DOLLY / dm;
		if(dm < n->radius*2){
			n->ActIntBuffer.data0 = uvsCatchDolly(this);
			n->ActIntBuffer.type = ACI_CONLARVER;
			n->set_body_color(COLORS_IDS::YELLOW_CHARGED);
			if(n->LightData){
				n->LightData->Destroy();
				n->LightData = NULL;
			};
		};
		x_speed = vCheck.x;
		y_speed = vCheck.y;

/*		p = (BaseObject*)(ActD.Tail);
		while(p){
			if(p->ID == ID_VANGER)
				((VangerUnit*)(p))->UpdateMainOrder = 1;
			p = (BaseObject*)(p->NextTypeList);
		};*/
	}else{
		if(!RND(128)) setSpeed();
	};

	if(!tail_counter){
		tail_x[tail_cInd] = pos_x;
		tail_y[tail_cInd] = pos_y;
		tail_cInd = (tail_cInd + 1)%DOLLY_TAIL_LEN;
		tail_counter++;
		}
	else if(++tail_counter == DOLLY_TAIL_PERIOD) tail_counter = 0;
};


void InsectList::Init(void)
{
	int i;
	for(i = 0;i < MAX_INSECT_TYPE;i++)
		NumInsect[i] = 0;

	if(CurrentWorld != WORLD_KHOX && CurrentWorld != WORLD_HMOK && CurrentWorld != WORLD_WEEXOW &&  !uvsCurrentWorldUnable){
		Data = new InsectUnit[MAX_INSECT_UNIT];
		for(i = 0;i < MAX_INSECT_UNIT;i++) {
			Data[i].Init();
			Data[i].CreateActionUnit(
				ModelD.FindModel("Bug"),
				SOBJ_AUTOMAT, Vector(RND(clip_mask_x), RND(clip_mask_y), -1),
				0,
				SET_3D_CHOOSE_LEVEL
			);
			Data[i].CreateInsect();
			ConnectTypeList(&Data[i]);
		}
	}else {
		Data = nullptr;
	}
};

void InsectList::FreeUnit(GeneralObject* p)
{
	DisconnectTypeList(p);
};

void InsectList::Free(void)
{
	UnitBaseListType::Free();
	if(Data) delete[] Data;
};

void InsectList::Quant(void)
{
	GeneralObject* p;
	GeneralObject* pp;

	p = Tail;
	while(p){
		((ActionUnit*)(p))->InitEnvironment();
		p = p->NextTypeList;
	};

	p = Tail;
	while(p){
		p->Quant();
		p = p->NextTypeList;
	};

	p = Tail;
	while(p){
		pp = p->NextTypeList;
		if(p->Status & SOBJ_DISCONNECT)
			FreeUnit(p);
		p = pp;
	};
};

int ChargeWeapon(VangerUnit* p,int ind,int sign)
{
	StuffObject* n;
	int c_log = 0;

	n = p->DeviceData;
	while(n){
		if(n->ActIntBuffer.type == ind){
			if(sign > 0){
				if(n->ActIntBuffer.data1 < uvsItemTable[n->uvsDeviceType]->param2){
					c_log = 1;
					n->ActIntBuffer.data1 = uvsItemTable[n->uvsDeviceType]->param2;
				};
			}else{
				if(n->ActIntBuffer.data1 > 0){
					c_log = 1;
					n->ActIntBuffer.data1 = 0;
				};
			};
		};
		n = n->NextDeviceList;
	};
	
	if(p->Status & SOBJ_ACTIVE){
		if(sign){
			if(c_log){				
				switch(ind){
					case ACI_MACHOTINE_GUN_LIGHT:
					case ACI_MACHOTINE_GUN_HEAVY:
						aiMessageQueue.Send(AI_MESSAGE_MACHOTINE,p->Speed,1,0);//aiMessageData[AI_MESSAGE_MACHOTINE].Send(p->Speed,1,0);
						SOUND_CHARGE_MACHOTIN();
						break;
					case ACI_GHORB_GEAR_LIGHT:
					case ACI_GHORB_GEAR_HEAVY:
						aiMessageQueue.Send(AI_MESSAGE_GHORB,p->Speed,2,0);//aiMessageData[AI_MESSAGE_GHORB].Send(p->Speed,2,0);
						SOUND_CHARGE_GHORB();
						break;
				};
			};
		}else{
			if(c_log){				
				switch(ind){
					case ACI_GHORB_GEAR_LIGHT:
					case ACI_GHORB_GEAR_HEAVY:
						aiMessageQueue.Send(AI_MESSAGE_DGHORB,p->Speed,1,0);//aiMessageData[AI_MESSAGE_DGHORB].Send(p->Speed,1,0);
						SOUND_DISCHARGE();
						break;
				};
			};
		};
	};
	return c_log;
};

int ChargeDevice(VangerUnit* p,int ind,int sign)
{
	StuffObject* n;
	int c_log = 0;	

	if(p->Status & SOBJ_ACTIVE){
		n = p->DeviceData;
		while(n){
			if(n->ActIntBuffer.type == ind){
				if(sign > 0){
					if(n->ActIntBuffer.data1 < uvsItemTable[n->uvsDeviceType]->param2){
						c_log = 1;										
						n->ActIntBuffer.data1 = uvsItemTable[n->uvsDeviceType]->param2;
						n->ActIntBuffer.type = uvsSetItemType(n->uvsDeviceType,n->ActIntBuffer.data0,n->ActIntBuffer.data1);
						aciChangeItem(&(n->ActIntBuffer));
					};
				}else{
					if(n->ActIntBuffer.data1 > 0){
						c_log = 1;										
						n->ActIntBuffer.data1 = 0;
						n->ActIntBuffer.type = uvsSetItemType(n->uvsDeviceType,n->ActIntBuffer.data0,n->ActIntBuffer.data1);
						aciChangeItem(&(n->ActIntBuffer));
					};
				};
			};
			n = n->NextDeviceList;
		};
		
		if(sign){
			if(c_log){
				aiMessageQueue.Send(AI_MESSAGE_COPTER,p->Speed,2,0);//aiMessageData[AI_MESSAGE_COPTER].Send(p->Speed,2,0);
				SOUND_CHARGE_RIGS();
			};
		}else{
			if(c_log){
				aiMessageQueue.Send(AI_MESSAGE_DCOPTER,p->Speed,1,0);//aiMessageData[AI_MESSAGE_DCOPTER].Send(p->Speed,1,0);
				SOUND_DISCHARGE();
			};
		};
	}else{
		n = p->DeviceData;
		while(n){
			if(n->ActIntBuffer.type == ind){
				if(sign > 0){
					if(n->ActIntBuffer.data1 < uvsItemTable[n->uvsDeviceType]->param2){
						c_log = 1;										
						n->ActIntBuffer.data1 = uvsItemTable[n->uvsDeviceType]->param2;
						n->ActIntBuffer.type = uvsSetItemType(n->uvsDeviceType,n->ActIntBuffer.data0,n->ActIntBuffer.data1);
					};
				}else{
					if(n->ActIntBuffer.data1 > 0){
						c_log = 1;										
						n->ActIntBuffer.data1 = 0;
						n->ActIntBuffer.type = uvsSetItemType(n->uvsDeviceType,n->ActIntBuffer.data0,n->ActIntBuffer.data1);
					};
				};
			};
			n = n->NextDeviceList;
		};
	};
	return c_log;
};

void GunSlot::OpenSlot(int slot,VangerUnit* own,int ind)
{
	ItemData = NULL;
	pData = NULL;
	Owner = own;
	nSlot = slot;
	Owner->lay_to_slot(nSlot,NULL);	
	StuffNetID = 0;
	FireCount = 0;
	NetFireCount = 0;
	TableIndex = ind;
	if(NetworkON){
		if(Owner->Status & SOBJ_ACTIVE){
			NetID = CREATE_NET_ID(NID_SLOT);
			NETWORK_OUT_STREAM.create_permanent_object(NetID,0,0,0);
			NETWORK_OUT_STREAM < (int)(Owner->NetID);
			NETWORK_OUT_STREAM < (uchar)(TableIndex);
			NETWORK_OUT_STREAM < (int)(StuffNetID);
			NETWORK_OUT_STREAM < (int)(FireCount);
			NETWORK_OUT_STREAM < (int)(0);
			NETWORK_OUT_STREAM.end_body();
		}else NetID = 0;
	};
};

void GunSlot::CloseSlot(void)
{
	Owner->lay_to_slot(nSlot,NULL);
//	pData = NULL;
	if(NetworkON && (Owner->Status & SOBJ_ACTIVE)){
		NETWORK_OUT_STREAM.delete_object(NetID);
		NETWORK_OUT_STREAM.end_body();
	};
};

void GunSlot::OpenGun(GunDevice* p)
{
	ItemData = p;
	pData = ItemData->pData;
	RealSpeed = 0;
	Time = 0;
	GunStatus = GUN_READY;
	ControlFlag = 0;
	TargetObject = NULL;
	aiTargetObject = NULL;
	Owner->lay_to_slot(nSlot,&ModelD.ActiveModel(p->ModelID));
	ItemData->ActIntBuffer.slot = nSlot;
	
	StuffNetID = ItemData->NetDeviceID;
	FireCount = 0;
	NetFireCount = 0;

	if(NetworkON && (Owner->Status & SOBJ_ACTIVE)){
		NETWORK_OUT_STREAM.update_object(NetID,0,0);
		NETWORK_OUT_STREAM < (int)(Owner->NetID);
		NETWORK_OUT_STREAM < (uchar)(TableIndex);
		NETWORK_OUT_STREAM < (int)(StuffNetID);
		NETWORK_OUT_STREAM < (int)(FireCount);
		NETWORK_OUT_STREAM < (int)(0);
		NETWORK_OUT_STREAM.end_body();
	};

	if(Owner->Status & SOBJ_ACTIVE)
		Owner->GetWeaponDelta();
};

void GunSlot::CloseGun(void)
{
	ItemData->ActIntBuffer.slot = -1;
	pData = NULL;
	Owner->lay_to_slot(nSlot,NULL);

	StuffNetID = 0;
	FireCount = 0;
	NetFireCount = 0;
	
	if(Owner->Status & SOBJ_ACTIVE){
		Owner->GetWeaponDelta();
		if(NetworkON && (Owner->Status & SOBJ_ACTIVE)){
			NETWORK_OUT_STREAM.update_object(NetID,0,0);
			NETWORK_OUT_STREAM < (int)(Owner->NetID);
			NETWORK_OUT_STREAM < (uchar)(TableIndex);
			NETWORK_OUT_STREAM < (int)(StuffNetID);
			NETWORK_OUT_STREAM < (int)(FireCount);
			NETWORK_OUT_STREAM < (int)(0);
			NETWORK_OUT_STREAM.end_body();
		};
	};
};

void GunSlot::NetStuffQuant(void)
{
	StuffObject* p;	
	if(StuffNetID){
		if(!pData){
			ItemData = NULL;
			p = Owner->DeviceData;
			while(p){
				if(p->NetDeviceID == StuffNetID){
					ItemData = (GunDevice*)(p);
					break;
				};
				p = p->NextDeviceList;
			};

			if(ItemData){
				pData = ItemData->pData;
				RealSpeed = 0;
				Time = 0;
				GunStatus = GUN_READY;
				ControlFlag = 0;
				TargetObject = NULL;
				aiTargetObject = NULL;
				Owner->lay_to_slot(nSlot,&ModelD.ActiveModel(p->ModelID));
				ItemData->ActIntBuffer.slot = nSlot;
				FireCount = NetFireCount;
			};
		}else{
			if(ItemData->NetDeviceID != StuffNetID){
				ItemData->ActIntBuffer.slot = -1;
				pData = NULL;
				Owner->lay_to_slot(nSlot,NULL);	
			}else{
				if(FireCount < NetFireCount)
					RemoteFire();
			};
		};
	}else{
		if(pData){
			ItemData->ActIntBuffer.slot = -1;
			pData = NULL;
			Owner->lay_to_slot(nSlot,NULL);	
			FireCount = NetFireCount;
		};
	};
};

void GunSlot::NetEvent(unsigned int time)
{
	int t;
	TargetObject = NULL;
	NETWORK_IN_STREAM > StuffNetID;
	NETWORK_IN_STREAM > NetFireCount;
	NETWORK_IN_STREAM > t;

	if(Owner && Owner->Visibility == VISIBLE && !(Owner->Status & SOBJ_WAIT_CONFIRMATION) && GetDistTime(NetGlobalTime,time) < 5*256){
		if(t) aiTargetObject = ActD.GetNetObject(t);
		else aiTargetObject = NULL;
	}else{
		FireCount = NetFireCount;
		aiTargetObject = NULL;
	};
};

void NetSlotEvent(int type,int id)
{
	uchar ch;
	int t;
	VangerUnit* p;
	PlayerData* n;
	
	if(type == UPDATE_OBJECT){
		NETWORK_IN_STREAM > t;
		NETWORK_IN_STREAM > ch;
		p = (VangerUnit*)(ActD.GetNetObject(t));
		if(p){
			p->GunSlotData[ch].NetID = id;
			p->GunSlotData[ch].NetEvent(NETWORK_IN_STREAM.current_time());
		}else{
			n = players_list.first();
			while(n){
				if(n->client_ID == NETWORK_IN_STREAM.current_creator()){
					n->SlotNetID[ch] = id;
					NETWORK_IN_STREAM > t;
					n->SlotStuffNetID[ch] = t;
					NETWORK_IN_STREAM > t;
					n->SlotFireCount[ch] = t;
					NETWORK_IN_STREAM > t;
					break;
				};
				n = (PlayerData*)n -> next;
			};
			if(!n) NETWORK_IN_STREAM.ignore_event();
		};
	}else NETWORK_IN_STREAM.ignore_event();
};

void GunSlot::NetUpdate(void)
{	
	FireCount++;
	NETWORK_OUT_STREAM.update_object(NetID,0,0);
	NETWORK_OUT_STREAM < (int)(Owner->NetID);
	NETWORK_OUT_STREAM < (uchar)(TableIndex);
	NETWORK_OUT_STREAM < (int)(StuffNetID);
	NETWORK_OUT_STREAM < (int)(FireCount);
	if(ActD.LocatorPoint) NETWORK_OUT_STREAM < (int)(ActD.LocatorPoint->NetID);
	else{
		if(TargetObject) NETWORK_OUT_STREAM < (int)(TargetObject->NetID);
		else NETWORK_OUT_STREAM < (int)(0);
	};
	NETWORK_OUT_STREAM.end_body();	
};

void GunSlot::Fire(void)
{
	GeneralObject* p;
	int mlen,len;
	int dx,dy;
	int rx,ry;
	StuffObject* l;
	
	if(GunStatus != GUN_READY || Owner->BeebonationFlag || !(Owner->ExternalDraw)) return;	

	TargetObject = NULL;
	switch(ItemData->ActIntBuffer.type){
		case ACI_MACHOTINE_GUN_HEAVY:
		case ACI_MACHOTINE_GUN_LIGHT:
			if(ItemData->ActIntBuffer.data1 > 0){
				GunStatus = GUN_FIRE;
				ItemData->ActIntBuffer.data1--;

				if(NetworkON) NetUpdate();

				if(ActD.Active)
					SOUND_MACHOTINE_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
			};
			break;
		case ACI_GHORB_GEAR_LIGHT:
			if(ItemData->ActIntBuffer.data1 > 0){
				GunStatus = GUN_FIRE;
				ItemData->ActIntBuffer.data1--;
				if(NetworkON) NetUpdate();
				if(ActD.Active)
					SOUND_GHORB_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
			};			
			break;
		case ACI_GHORB_GEAR_HEAVY:
			if(ItemData->ActIntBuffer.data1 > 0){
				GunStatus = GUN_FIRE;
				ItemData->ActIntBuffer.data1--;
				if(NetworkON) NetUpdate();
				if(ActD.Active)
					SOUND_GHORB_BIG_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
			};			
			break;
		case ACI_SPEETLE_SYSTEM_LIGHT:
		case ACI_SPEETLE_SYSTEM_HEAVY:
			if(ItemData->ActIntBuffer.data1 > 0){
				if(ControlFlag){					
					p = ActD.Tail;
					mlen = -1;
					rx = Owner->R_curr.x;
					ry = Owner->R_curr.y;
					while(p){
						if(((BaseObject*)(p)) -> Visibility == VISIBLE && p != Owner){
							dx = getDistX(p->R_curr.x,rx);
							dy = getDistY(p->R_curr.y,ry);
							len = dx*dx + dy*dy;
							if(len < mlen || mlen == -1){
								TargetObject = p;
								mlen = len;
							};
						};
						p = (Object*)(p->NextTypeList);
					};
				};
				GunStatus = GUN_FIRE;
				ItemData->ActIntBuffer.data1--;
				if(NetworkON) NetUpdate();
				if(ActD.Active)
					SOUND_SPEETLE_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
			}else{
				l = GetStuffObject(Owner,ACI_SPEETLE_SYSTEM_AMMO1);
				if(!l){
					ControlFlag = 0;
					l = GetStuffObject(Owner,ACI_SPEETLE_SYSTEM_AMMO0);
				}else ControlFlag = 1;
				if(l){
					ObjectDestroy(l);
					ItemData->ActIntBuffer.data1 = l->ActIntBuffer.data1;
					if(Owner->ID == ID_VANGER && (Owner->Status & SOBJ_ACTIVE)) aciSendEvent2actint(ACI_DROP_ITEM,&(l->ActIntBuffer));
					l->Storage->Deactive(l);
					Owner->DelDevice(l);
				};
			};			
			break;
		case ACI_BEEBBANOZA_BLOCKADE:
			if(Owner->ID == ID_VANGER && (Owner->Status & SOBJ_ACTIVE)){
				if(aiGetHotBug() > 5){
					aiPutHotBug(aiGetHotBug() - 5);
					GunStatus = GUN_FIRE;
					if(NetworkON){
						NetUpdate();
						my_player_body.beebos = aiGetHotBug();
						send_player_body(my_player_body);
					};
					if(ActD.Active)
						SOUND_BEEBBANOZA_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
				};
			}else{
				GunStatus = GUN_FIRE;
				if(ActD.Active)
					SOUND_BEEBBANOZA_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
			};			
			break;
		case ACI_CRUSTEST_CANNON:
			if(ItemData->ActIntBuffer.data1 > 0){
				GunStatus = GUN_FIRE;
				ItemData->ActIntBuffer.data1--;
				if(NetworkON) NetUpdate();
				if(ActD.Active)
					SOUND_CRUSTEST_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
			}else{
				l = GetStuffObject(Owner,ACI_CRUSTEST_CANNON_AMMO);
				if(l){
					ObjectDestroy(l);
					ItemData->ActIntBuffer.data1 = l->ActIntBuffer.data1;
					if(Owner->ID == ID_VANGER && (Owner->Status & SOBJ_ACTIVE)) aciSendEvent2actint(ACI_DROP_ITEM,&(l->ActIntBuffer));
					l->Storage->Deactive(l);
					Owner->DelDevice(l);
				};
			};			
			break;
		case ACI_TERMINATOR:
			if(Owner->dynamic_state & (GROUND_COLLISION | WHEELS_TOUCH)){
				p = ActD.Tail;
				mlen = -1;
				rx = Owner->R_curr.x;
				ry = Owner->R_curr.y;
				while(p){
					if(((BaseObject*)(p)) -> Visibility == VISIBLE && p != Owner){
						dx = getDistX(p->R_curr.x,rx);
						dy = getDistY(p->R_curr.y,ry);
						len = dx*dx + dy*dy;
						if(len < mlen || mlen == -1){
							TargetObject = p;
							mlen = len;
						};
					};
					p = (Object*)(p->NextTypeList);
				};

				GunStatus = GUN_FIRE;
				ItemData->ActIntBuffer.data1--;
				if(NetworkON) NetUpdate();
				if(ActD.Active)
					SOUND_TERMINATOR_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));

				if(ItemData->ActIntBuffer.data1 <= 0){
					l = Owner->DeviceData;
					while(l){
						if(l->ActIntBuffer.type == ACI_TERMINATOR && l != ItemData)
							break;
						l = l->NextDeviceList;
					};

					if(l){
						ObjectDestroy(l);
						ItemData->ActIntBuffer.data0 = l->ActIntBuffer.data0;
						ItemData->ActIntBuffer.data1 = l->ActIntBuffer.data1;
						if(Owner->ID == ID_VANGER && (Owner->Status & SOBJ_ACTIVE)) aciSendEvent2actint(ACI_DROP_ITEM,&(l->ActIntBuffer));
						l->Storage->Deactive(l);
						Owner->DelDevice(l);
					}else{
						ObjectDestroy(ItemData);
						if(Owner->ID == ID_VANGER && (Owner->Status & SOBJ_ACTIVE)) aciSendEvent2actint(ACI_DROP_ITEM,&(ItemData->ActIntBuffer));
						ItemData->Storage->Deactive(ItemData);
						Owner->DelDevice(ItemData);
						CloseGun();
					};
				};
			};
			break;
		case ACI_TERMINATOR2:
			p = ActD.Tail;
			mlen = -1;
			rx = Owner->R_curr.x;
			ry = Owner->R_curr.y;
			while(p){
				if(((BaseObject*)(p)) -> Visibility == VISIBLE && p != Owner){
					dx = getDistX(p->R_curr.x,rx);
					dy = getDistY(p->R_curr.y,ry);
					len = dx*dx + dy*dy;
					if(len < mlen || mlen == -1){
						TargetObject = p;
						mlen = len;
					};
				};
				p = (Object*)(p->NextTypeList);
			};
			GunStatus = GUN_FIRE;
			ItemData->ActIntBuffer.data1--;
			if(NetworkON) NetUpdate();

			if(ActD.Active)
				SOUND_TERMINATOR_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));

			if(ItemData->ActIntBuffer.data1 <= 0){
				l = Owner->DeviceData;
				while(l){
					if(l->ActIntBuffer.type == ACI_TERMINATOR2 && l != ItemData)
						break;
					l = l->NextDeviceList;
				};

				if(l){
					ObjectDestroy(l);
					ItemData->ActIntBuffer.data0 = l->ActIntBuffer.data0;
					ItemData->ActIntBuffer.data1 = l->ActIntBuffer.data1;
					if(Owner->ID == ID_VANGER && (Owner->Status & SOBJ_ACTIVE)) aciSendEvent2actint(ACI_DROP_ITEM,&(l->ActIntBuffer));
					l->Storage->Deactive(l);
					Owner->DelDevice(l);
				}else{
					ObjectDestroy(ItemData);
					if(Owner->ID == ID_VANGER && (Owner->Status & SOBJ_ACTIVE)) aciSendEvent2actint(ACI_DROP_ITEM,&(ItemData->ActIntBuffer));
					ItemData->Storage->Deactive(ItemData);
					Owner->DelDevice(ItemData);
					CloseGun();
				};
			};
			break;
		case ACI_EMPTY_AMPUTATOR:
			if(ActD.Active)
				SOUND_TERMINATOR_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));				
			GunStatus = GUN_FIRE;
			if(NetworkON) NetUpdate();
			break;
		case ACI_EMPTY_DEGRADATOR:
			if(ActD.Active)
				SOUND_TERMINATOR_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));				
			GunStatus = GUN_FIRE;
			if(NetworkON) NetUpdate();
			break;
		case ACI_EMPTY_MECHOSCOPE:			
			if(ActD.Active)
				SOUND_TERMINATOR_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));				
			GunStatus = GUN_FIRE;
			if(NetworkON) NetUpdate();
			break;
	};
};

const int HYPNOTISE_WEAPON_RADIUS = 100;

void GunSlot::RemoteFire(void)
{
	if(GunStatus != GUN_READY || Owner->BeebonationFlag || !(Owner->ExternalDraw)) return;
	FireCount++;
	TargetObject = NULL;
	GunStatus = GUN_FIRE;

	if(ActD.Active){
		switch(ItemData->ActIntBuffer.type){
			case ACI_MACHOTINE_GUN_HEAVY:
			case ACI_MACHOTINE_GUN_LIGHT:			
				SOUND_MACHOTINE_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
				break;
			case ACI_GHORB_GEAR_LIGHT:
			case ACI_GHORB_GEAR_HEAVY:			
				SOUND_GHORB_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
				break;
			case ACI_SPEETLE_SYSTEM_LIGHT:
			case ACI_SPEETLE_SYSTEM_HEAVY:			
				SOUND_SPEETLE_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
				break;
			case ACI_BEEBBANOZA_BLOCKADE:
				SOUND_BEEBBANOZA_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));			
				break;
			case ACI_CRUSTEST_CANNON:
				SOUND_CRUSTEST_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
				break;
			case ACI_TERMINATOR2:
			case ACI_TERMINATOR:
				SOUND_TERMINATOR_SHOT(getDistX(ActD.Active->R_curr.x,Owner->R_curr.x));
				break;
		};
	};
};

void GunSlot::Quant(void)
{
	BulletObject* b;
	JumpBallObject* g;
	int i;
	Vector vCheck;

	switch(pData->BulletID){
		case BULLET_TYPE_ID::HYPNOTISE:
			if(GunStatus == GUN_FIRE){
				ItemData->Time = pData->LifeTime;
				if(Owner->Status & SOBJ_ACTIVE){
					ActD.CheckDevice(ItemData);
					Owner->CheckOutDevice(ItemData);
					aciRemoveItem(&(ItemData->ActIntBuffer));
//					aciSendEvent2actint(ACI_DROP_ITEM,&(ItemData->ActIntBuffer));
				}else
					Owner->CheckOutDevice(ItemData);

				vCheck = Vector(HYPNOTISE_WEAPON_RADIUS,0,0)*Owner->MovMat;
				vCheck += Owner->R_curr;
				cycleTor(vCheck.x,vCheck.y);				
				ItemData->DeviceOut(Owner->R_curr + Vector(0,0,Owner->radius*4),1,vCheck);
				CloseGun();
				GunStatus = GUN_READY;
			};
			break;
		case BULLET_TYPE_ID::ROCKET:
		case BULLET_TYPE_ID::FIREBALL:
		case BULLET_TYPE_ID::TERMINATOR:
			switch(GunStatus){
				case GUN_FIRE:
					if(Owner->Status & SOBJ_ACTIVE){
						if(ActD.LocatorPoint)
							TargetObject = ActD.LocatorPoint;
					}else{
						if(aiTargetObject)
							TargetObject = aiTargetObject;
						else 
							TargetObject = NULL;
					};
					b = BulletD.CreateBullet();
					Time = 0;
					if(Owner->Speed > 0) RealSpeed = Owner->Speed;
					else RealSpeed = 0;
					mFire = Owner->RotMat;
					vFire = Owner->R_curr;
					if((1 << ItemData->ActIntBuffer.slot) & Owner->slots_existence) vFire += Owner->A_l2g*(Owner->R_slots[ItemData->ActIntBuffer.slot]*Owner->scale_real);
					cycleTor(vFire.x,vFire.y);
					b->Owner = Owner;
					b->CreateBullet(this,pData);
					GunStatus = GUN_WAIT;
					break;
				case GUN_WAIT:
					Time++;
					if(Time > pData->WaitTime) GunStatus = GUN_READY;
					break;
			};
			break;
		case BULLET_TYPE_ID::LASER:
			switch(GunStatus){
				case GUN_FIRE:
					Time = 0;
					if(Owner->Speed > 0) RealSpeed = Owner->Speed;
					else RealSpeed = 0;
					mFire = Owner->RotMat;
					vFire = Owner->R_curr;
					if((1 << ItemData->ActIntBuffer.slot) & Owner->slots_existence) vFire += Owner->A_l2g*(Owner->R_slots[ItemData->ActIntBuffer.slot]*Owner->scale_real);
					cycleTor(vFire.x,vFire.y);
					GunStatus = GUN_WAIT;
					for(i = 0;i < pData->TapeSize;i++){
						b = BulletD.CreateBullet();
						b->Owner = Owner;
						b->CreateBullet(this,pData);
					};
					break;
				case GUN_WAIT:
					Time++;
					if(Time > pData->WaitTime) GunStatus = GUN_READY;
					break;
			};
			break;
		case BULLET_TYPE_ID::HORDE:
			switch(GunStatus){
				case GUN_FIRE:
					Time = 0;
					mFire = Owner->RotMat;
					vFire = Owner->R_curr + Vector(0,0,Owner->radius*2);
					cycleTor(vFire.x,vFire.y);
					GunStatus = GUN_WAIT;
					(HordeD.CreateHorde())->CreateHorde(vFire,20,240,400,Owner);
					break;
				case GUN_WAIT:
					Time++;
					if(Time > pData->WaitTime) GunStatus = GUN_READY;
					break;
			};
			break;
		case BULLET_TYPE_ID::JUMPBALL:
			switch(GunStatus){
				case GUN_FIRE:
					Time = 0;
					mFire = Owner->RotMat;
					vFire = Owner->R_curr + Vector(0,0,Owner->radius*2);
					cycleTor(vFire.x,vFire.y);
					GunStatus = GUN_WAIT;
					if(pData->TargetMode == BULLET_TARGET_MODE::CONTROL){
						for(i = 0;i < pData->TapeSize;i++){
							g = JumpD.CreateBall();
							g->CreateBullet(this,pData);
						};
					}else{
						g = JumpD.CreateBall();
						g->CreateBullet(this,pData);					
					};					
					break;
				case GUN_WAIT:
					Time++;
					if(Time > pData->WaitTime) GunStatus = GUN_READY;
					break;
			};
			break;
	};
};

UnitItemMatrix* UnitMatrixData;

void UnitItemMatrix::Open(Parser& in)
{
	int i,j,k,t;
	in.search_name("UnitMatrixID");
	ID = in.get_int();

	in.search_name("NumID");
	NumID = in.get_int();
	if(NumID > 0){
		DataID = new UnitItemMatrix*[NumID];
		in.search_name("DataID");
		for(i = 0;i < NumID;i++)
			DataID[i] = &UnitMatrixData[in.get_int()];
		in.search_name("NumSlot");
		NumSlot = in.get_int();
		if(NumSlot > 0){
			SlotSize = new int[NumSlot*2];
			nSlot = SlotSize + NumSlot;
			in.search_name("SlotSize");
			for(i = 0;i < NumSlot;i++) SlotSize[i] = in.get_int();
			in.search_name("nSlot");
			for(i = 0;i < NumSlot;i++) nSlot[i] = in.get_int() - 1;

			for(j = 0;j < NumSlot - 1;j++){
				for(k = 0;k < NumSlot - 1;k++){
					if(SlotSize[k] < SlotSize[k + 1]){
						t = SlotSize[k];
						SlotSize[k] = SlotSize[k + 1];
						SlotSize[k + 1] = t;

						t = nSlot[k];
						nSlot[k] = nSlot[k + 1];
						nSlot[k + 1] = t;
					};
				};
			};
		};
	}else DataID = NULL;
};

void UnitItemMatrix::Close(void)
{
	if(NumID > 0){
		delete[] DataID;
		if(NumSlot > 0)
			delete[] SlotSize;
	};
};

void uvsUnitType::ItemQuant(void)
{
};

#ifdef TEST_TRACK

void VangerUnit::DestroyAroundBarrel(void)
{
	StaticObject* st;
	int y0,y1;
	int l,i;
	Vector vCheck;

	y0 = R_curr.y - EXPLOSION_BARELL_RADIUS - 200;
	y1 = R_curr.y + EXPLOSION_BARELL_RADIUS + 200;
	i = FindFirstStatic(y0,y1,(StaticObject**)TntSortedData,TntTableSize);

	if(i > -1){
		do{
			st = TntSortedData[i];
			if(st->Enable){
				vCheck.x = getDistX(st->R_curr.x,R_curr.x);
				l = 200 + EXPLOSION_BARELL_RADIUS;
				if(abs(vCheck.x) < l){
					vCheck.y = getDistY(st->R_curr.y,R_curr.y);
					vCheck.z = st->R_curr.z - R_curr.z;
					if(vCheck.abs2() < l*l) st->Touch(this);
				};
			};
			i++;
		}while(i < TntTableSize && TntSortedData[i]->R_curr.y < y1);
	};
};
#endif

//------------------------------------------------------------------------------------------------------------------------------------

void TrackUnit::MakeTrackDist(void)
{
	int i;
	for(i = 0;i < HideTrack.NumBranch;i++)
		TrackDist[i] = HideTrack.branch[i].Len;	
};

void aiListType::Init(void)
{
	Num = 0;
	Tail = NULL;
};

void aiListType::Free(void)
{
	aiListElement* p;
	aiListElement* pp;
	p = Tail;
	while(p){
		pp = p->Next;
		Disconnect(p);
		delete p;
		p = pp;
	};
};

void aiListType::Connect(aiListElement* p)
{
	if(!Tail){
		p->Next = p->Prev = NULL;
		Tail = p;
	}else{
		Tail -> Prev = p;
		p -> Next = Tail;
		p -> Prev = NULL;
		Tail = p;
	};
	Num++;	
};

void aiListType::Disconnect(aiListElement* p)
{
	if((!p->Prev) && (!p->Next)) Tail = NULL;
	else{
		if(p->Prev) p->Prev->Next = p->Next;
		else Tail = p->Next;
		if(p->Next) p->Next->Prev = p->Prev;
	};
	Num--;
};

void VangerUnit::TargetAnalysis(void)
{		
	Vector vCheck;
	int d;

	if(OtherFlag & MECHOS_CALC_WAY){
		if(!TraceWay()){
			OtherFlag &= ~MECHOS_CALC_WAY;
			OtherFlag |= MECHOS_RECALC_FRONT;
//			SpeedDir = 1;
			memcpy(WayDist,CalcWayDist,sizeof(int)*HideTrack.NumNode);
		};
	};

	vCheck = Vector(getDistX(TargetPoint.vPoint.x,R_curr.x),getDistY(TargetPoint.vPoint.y,R_curr.y),0);
	d = vCheck.vabs();
	if(aiMoveMode == AI_MOVE_TRACK){
		TrackQuant();
		DirectQuant();
	}else{
		if(SpeedDir > 0) vDirect = Vector(getDistX(TargetPoint.vPoint.x,XCYCL(R_curr.x + vUp.x)),getDistY(TargetPoint.vPoint.y,R_curr.y + vUp.y),0);
		else vDirect = Vector(getDistX(TargetPoint.vPoint.x,XCYCL(R_curr.x + vDown.x)),getDistY(TargetPoint.vPoint.y,R_curr.y + vDown.y),0);
		TrackQuant();
	};	
};

void VangerUnit::ResolveGenerator(void)
{
	int i;
	aiUnitEvent* n;
	aiUnitEvent* nn;

	int dx,dy;
	uchar* pa0;
	Vector vCheck,vTrack;
	VangerUnit* v;
	StuffObject* s;
	int q,a;
	SensorDataType* sns;

	aiStatus = AI_STATUS_NONE;
	aiStatus |= AI_STATUS_WHEEL | AI_STATUS_IMPULSE;

	if(dynamic_state & VERTICAL_WALL_COLLISION) WallCollisionTime = MAX_WALL_TIME;
	else if(WallCollisionTime > 0) WallCollisionTime--;

	if(abs(TractionValue) < MAX_TRACTION_CHECK_DELTA) DeltaTractionTime = MAX_TRACTION_CHECK_TIME;
	else if(DeltaTractionTime > 0) DeltaTractionTime--;

	if(aiAlarmTime > 0) aiAlarmTime--;
	if(SpeedFactorTime > 0)	SpeedFactorTime--;

	if(AddAttackTime > 0) AddAttackTime = 0;

	if(aiDamage > 0){
		aiDamage -= (1 << 10);
		if(aiDamage < 0){
			aiDamage = 0;
			aiDamageData = NULL;
		};
	};	

	if(!LowAmmo) CheckAmmo();

	if(!LowArmor && Armor < ArmorAlarm){
		sns = GetNearSensor(R_curr.y,SensorTypeList::ARMOR_UPDATE);
		if(sns && !aiResolveFind.FindResolve(UNIT_ORDER_SENSOR,UnitOrderType(sns))){
			AddFindResolve(UNIT_ORDER_SENSOR,UnitOrderType(sns),0,AI_RESOLVE_STATUS_TARGET)->Time = MaxWayCount;
			LowArmor = 1;
		};
	};

	if(aiActionID == AI_ACTION_FARMER){
		q = SeedNum;
		s = DeviceData;
		SeedNum = 0;
		if(uvsCurrentCycle == 1) { // Election of Castaways
			while(s){
				if(s->ActIntBuffer.type == ACI_PIPETKA)
					SeedNum += s->ActIntBuffer.data1;
				s = s->NextDeviceList;
			};
		}else{
			if(uvsCurrentCycle == 2){ // Heroism
				while(s){
					if(s->ActIntBuffer.type == ACI_KERNOBOO)
						SeedNum += s->ActIntBuffer.data1;
					s = s->NextDeviceList;
				};
			};
		};

		if(FarmerD.Num == 0 && ActD.WorldSeedNum <= MaxSeed)
			SeedNum = MaxSeed;

		if(SeedNum >= MaxSeed && q < MaxSeed && uvsCurrentCycle != 0){ // 0 - Progress
			aiResolveFind.ClearResolve();
			uvsPoint->break_harvest();
			MainOrderInit();
		};
	};

	if(Visibility == VISIBLE && CoptePoint && !aiAlarmTime) aiStatus |= AI_STATUS_FLY;
	
	if((Visibility == VISIBLE && !RND(MAX_CHECK_VISIBLE_FACTOR)) || (Visibility == UNVISIBLE && !RND(MAX_CHECK_UNVISIBLE_FACTOR))){
		n = (aiUnitEvent*)(aiEvent.Tail);
		while(n){
			if(n->Time < aiMaxView) n->Time += aiAddView;
			n->Refresh = 0;
			n = (aiUnitEvent*)(n->Next);
		};

		v = (VangerUnit*)(ActD.Tail);
		vCheck.z = 0;		

		while(v){
			if(v != this && v->ExternalDraw && v->draw_mode == NORMAL_DRAW_MODE){
				vCheck.x = abs(getDistX(v->R_curr.x,R_curr.x));
				vCheck.y = abs(getDistY(v->R_curr.y,R_curr.y));
				if(vCheck.y < aiVangerRadius && vCheck.x < aiVangerRadius)
					AddEvent(AI_EVENT_VANGER,NULL,v);
			};
			v = (VangerUnit*)(v->NextTypeList);
		};

		s = (StuffObject*)(ItemD.Tail);
		while(s){
			if(s->ActIntBuffer.type == aiFirstItem || s->ActIntBuffer.type == aiSecondItem){
				dy = getDistY(s->R_curr.y,R_curr.y);
				if(abs(dy) < aiItemRadius){
					dy= getDistX(s->R_curr.x,R_curr.x);
					if(abs(dy) < aiItemRadius)
						AddEvent(AI_EVENT_ITEM,s,NULL);
				};
			};
			s = (StuffObject*)(s->NextTypeList);
		};
	}else{
		n = (aiUnitEvent*)(aiEvent.Tail);
		while(n){
			if(n->Time < aiMaxView) n->Time += aiAddView;
			n = (aiUnitEvent*)(n->Next);
		};
	};

	dx = getDistX(aiRelaxData.x,R_curr.x);
	dy = getDistY(aiRelaxData.y,R_curr.y);

	if(abs(dx) > aiRealxRadius || abs(dy) > aiRealxRadius){
		aiRelaxTime = 0;
		aiRelaxData = R_curr;
	}else aiRelaxTime++;

	dx = getDistX(TargetPoint.vPoint.x,R_curr.x);
	dy = getDistY(TargetPoint.vPoint.y,R_curr.y);
	if(abs(dy) < radius && abs(dx) < radius)
		aiStatus |= AI_STATUS_TARGET;

	i = 0;
	if(Visibility == VISIBLE){
		vCheck = R_curr;
		vTrack = R_curr + Vector(aiScanDist,0,0)*RotMat;
		if(MapLineTrace(vCheck,vTrack)){
			pa0 = vMap->lineT[vCheck.y];
			if(pa0 && GET_TERRAIN(*(pa0 + vCheck.x + H_SIZE)) == 7)
				i++;
		};

		vCheck = R_curr;
		vTrack = R_curr + Vector(aiScanDist,radius,0)*RotMat;
		if(MapLineTrace(vCheck,vTrack)){
			pa0 = vMap->lineT[vCheck.y];
			if(pa0 && GET_TERRAIN(*(pa0 + vCheck.x + H_SIZE)) == 7)
				i++;
		};

		vCheck = R_curr;
		vTrack = R_curr + Vector(aiScanDist,-radius,0)*RotMat;
		if(MapLineTrace(vCheck,vTrack)){
			pa0 = vMap->lineT[vCheck.y];
			if(pa0 && GET_TERRAIN(*(pa0 + vCheck.x + H_SIZE)) == 7)
				i++;
		};			
		if(i >= 2) aiStatus |= AI_STATUS_TNT;
	};

	n = (aiUnitEvent*)(aiEvent.Tail);
	if(Visibility == VISIBLE){
		while(n){
			nn = (aiUnitEvent*)(n->Next);
			if(!(n->Refresh)){
				aiEvent.Disconnect(n);
				delete n;
			}else{
				if(n->ID == AI_EVENT_VANGER && !RND(MAX_CHECK_VISIBLE_FACTOR)){
					if(n->Time > aiMainLevel)
						n->Time += aiDeltaLevel;

					if(!aiAlarmTime){
						if(GetAllert(n->Subj) < 0)
							aiAlarmTime = AI_MAX_ALARM_TIME;
					}else{
						if(n->Time < aiMaxAlarm)
							n->Time += aiAddAlarm;
					};

					CalcAiFactor(n->Subj,n->Time);
				};
			};
			n = nn;
		};
	}else{
		while(n){
			nn = (aiUnitEvent*)(n->Next);
			if(!(n->Refresh)){
				aiEvent.Disconnect(n);
				delete n;
			}else{
				if(n->ID == AI_EVENT_VANGER && !RND(MAX_CHECK_UNVISIBLE_FACTOR)){
					if(n->Time > aiMainLevel)
						n->Time += aiDeltaLevel;
					if(!aiAlarmTime){
						if(GetAllert(n->Subj) < 0)
							aiAlarmTime = AI_MAX_ALARM_TIME;
					}else{
						if(n->Time < aiMaxAlarm)
							n->Time += aiAddAlarm;					
					};
					CalcAiFactor(n->Subj,n->Time);
				};
			};
			n = nn;
		};
	};

	n = aiGlobalEventData[aiReadEvent][AI_EVENT_DROP];
	for(i = 0;i < aiNumGlobalEvent[aiReadEvent][AI_EVENT_DROP];i++){
		if(n->Refresh){
		};
		n++;
	};

	n = aiGlobalEventData[aiReadEvent][AI_EVENT_CAPTURE];
	for(i = 0;i < aiNumGlobalEvent[aiReadEvent][AI_EVENT_CAPTURE];i++){
		if(n->Refresh){
			if(n->Subj == this){
				if(aiResolveFind.FindResolve(UNIT_ORDER_STUFF,UnitOrderType((StuffObject*)(n->Obj))))
					aiResolveFind.ClearResolve(UNIT_ORDER_STUFF,UnitOrderType((StuffObject*)(n->Obj)));
			};
		};
		n++;
	};
	
	n = aiGlobalEventData[aiReadEvent][AI_EVENT_COLLISION];
	for(i = 0;i < aiNumGlobalEvent[aiReadEvent][AI_EVENT_COLLISION];i++){
		if(n->Refresh){
			switch(n->ID){
				case ID_VANGER:
					if(n->Subj == this && aiRelaxTime > aiRelaxTimeMax){
						vCheck = Vector(getDistX(n->Obj->R_curr.x,R_curr.x),getDistY(n->Obj->R_curr.y,R_curr.y),0);
						a = rPI(vCheck.psi() - Angle);
						if(a > PI) a -= PI*2;
						if(abs(a) < PI / 4) AddEventTime((VangerUnit*)(n->Obj),aiAddFrontCollision,AI_ADD_FRONT_COLLISION);
						else{
							if(abs(a) > 3*PI / 4)
								AddEventTime((VangerUnit*)(n->Obj),aiAddBackCollision,AI_ADD_BACK_COLLISION);
							else
								AddEventTime((VangerUnit*)(n->Obj),aiAddSideCollision,AI_ADD_SIDE_COLLISION);
						};
					};
					break;
				case ID_BULLET:
					if(n->Subj == this)
						AddEventTime((VangerUnit*)(n->Obj),aiAddAttack,AI_ADD_ATTACK);
					break;
			};
		};
		n++;
	};

	aiResolveWayEnable = 0;
	aiLocalTarget = NULL;

	if(!aiResolveFind.Num){
		if(Status & SOBJ_ACTIVE) GamerOrderInit();
		else MainOrderInit();
	};
};

void VangerUnit::GamerOrderInit(void)
{
	if(!CompasObj.CurrentTarget){
		CompasObj.CurrentTarget = CompasObj.TargetData;
		aciRefreshTargetsMenu();
	};
	//stalkerg
	if(!CompasObj.CurrentTarget){
		return;
	}
	//stalkerg тут падает если ты включил Пилот и уже подобрал спумию
	switch(CompasObj.CurrentTarget->ID){
		case CMP_OBJECT_ESCAVE:
		case CMP_OBJECT_SPOT:
			AddFindResolve(UNIT_ORDER_ENTER,CompasObj.CurrentTarget->Data)->Time = MaxWayCount;
			break;
		case CMP_OBJECT_PASSAGE:
			AddFindResolve(UNIT_ORDER_PASSAGE,CompasObj.CurrentTarget->Data)->Time = MaxWayCount;
			break;
		case CMP_OBJECT_SENSOR:
			AddFindResolve(UNIT_ORDER_SENSOR,CompasObj.CurrentTarget->Data)->Time = MaxWayCount;
			break;
	};
};

void VangerUnit::MainOrderInit(void)
{
	uvsTarget* g;
	aiUnitResolve* p;

	if(aiActionID == AI_ACTION_FARMER && SeedNum < MaxSeed){
		(p = AddFindResolve(UNIT_ORDER_VECTOR,UnitOrderType(RND(clip_mask_x),300 + RND(clip_mask_y - 600),0),0,AI_RESOLVE_STATUS_VIEW))->Time = MaxWayCount;
	}else{
		p = NULL;
		switch(uvsPoint->getOrder(g)){
			case UVS_TARGET::ESCAVE:
				(p = AddFindResolve(UNIT_ORDER_ENTER,g->unitPtr))->Time = MaxWayCount;
				break;
			case UVS_TARGET::SPOT:
				(p = AddFindResolve(UNIT_ORDER_ENTER,g->unitPtr))->Time = MaxWayCount;										
				break;
			case UVS_TARGET::PASSAGE:
				(p = AddFindResolve(UNIT_ORDER_PASSAGE,g->unitPtr))->Time = MaxWayCount;
				break;
			case UVS_TARGET::DOLLY:
				(p = AddFindResolve(UNIT_ORDER_DOLL,UnitOrderType((uvsDolly*)(g))))->Time = MaxWayCount;
				break;
			case UVS_TARGET::VANGER:
				if(ActD.Active){
					(p = AddFindResolve(UNIT_ORDER_VANGER,ActD.Active))->Time = MaxWayCount;
					aiResolveAttack.AddResolve(UNIT_ORDER_VANGER,ActD.Active);
				}else
					(p = AddFindResolve(UNIT_ORDER_VECTOR,UnitOrderType(RND(clip_mask_x),300 + RND(clip_mask_y - 600),0),0,AI_RESOLVE_STATUS_VIEW))->Time = MaxWayCount;
				break;
			case UVS_TARGET::ITEM:
				(p = AddFindResolve(UNIT_ORDER_PASSAGE,g->unitPtr))->Time = MaxWayCount;			
				break;				
			default:
				(p = AddFindResolve(UNIT_ORDER_VECTOR,UnitOrderType(RND(clip_mask_x),300 + RND(clip_mask_y - 600),0),0,AI_RESOLVE_STATUS_VIEW))->Time = MaxWayCount;
				break;
		};
	};
};

void VangerUnit::NoWayHandler(void)
{
	Vector vCheck;
	int dx,dy;
	switch(NoWayEnable){
		case AI_NO_WAY_CALC:
			if(!(OtherFlag & MECHOS_CALC_WAY)){
				if(PointStatus & TRK_BRANCH_MASK){
					if(pBranch->pEnd && pBranch->pBeg){
						NoWayDirect = 1 - MoveDir;
						TrackDist[pBranch->index] = 0xffff;
		 				WayInit();
					};
				};
			
				NoWayEnable = AI_NO_WAY_NONE;
//				aiMoveMode = AI_MOVE_TRACK;
			};
			aiResolveWayEnable = 2;
			break;
		case AI_NO_WAY_PROCESS:
			if(Visibility == VISIBLE){
				if(RunTimeProcess > 0){
					RunTimeProcess--;
					dx = getDistX(vRunStartPosition.x,R_curr.x);
					dy = getDistY(vRunStartPosition.y,R_curr.y);
					if((dx*dx + dy*dy) >= RunStartDist){
						NoWayEnable = AI_NO_WAY_NONE;
						aiMoveMode = AI_MOVE_TRACK;
						OtherFlag |= MECHOS_RECALC_FRONT;
					}else{
						if(PointStatus & TRK_IN_MASK){
							if(aiRunBranch != pBranch || aiRunNode != pNode){
								NoWayEnable = AI_NO_WAY_NONE;
								aiMoveMode = AI_MOVE_TRACK;
								OtherFlag |= MECHOS_RECALC_FRONT;
							}else{
								if(TargetPoint.PointStatus & TRK_NODE_MASK){
									if(pNode == TargetPoint.pNode){
										NoWayEnable = AI_NO_WAY_NONE;
										aiMoveMode = AI_MOVE_TRACK;
										OtherFlag |= MECHOS_RECALC_FRONT;
									};
								}else{
									if(pBranch == TargetPoint.pBranch && 
									   (pNextLink == TargetPoint.pPrevLink || pNextLink == TargetPoint.pNextLink || 
									    pPrevLink == TargetPoint.pPrevLink || pPrevLink == TargetPoint.pNextLink)){
										NoWayEnable = AI_NO_WAY_NONE;
										aiMoveMode = AI_MOVE_TRACK;
										OtherFlag |= MECHOS_RECALC_FRONT;
									};
								};				
							};				
						};
					};
				}else{
					NoWayEnable = AI_NO_WAY_NONE;
					aiMoveMode = AI_MOVE_TRACK;
					OtherFlag |= MECHOS_RECALC_FRONT;
				};
			}else{
				if(PointStatus & TRK_IN_MASK){
					if(aiRunBranch != pBranch || aiRunNode != pNode){
						NoWayEnable = AI_NO_WAY_NONE;
						aiMoveMode = AI_MOVE_TRACK;
						OtherFlag |= MECHOS_RECALC_FRONT;
					}else{
						if(TargetPoint.PointStatus & TRK_NODE_MASK){
							if(pNode == TargetPoint.pNode){
								NoWayEnable = AI_NO_WAY_NONE;
								aiMoveMode = AI_MOVE_TRACK;
								OtherFlag |= MECHOS_RECALC_FRONT;
							};
						}else{
							if(pBranch == TargetPoint.pBranch && 
							   (pNextLink == TargetPoint.pPrevLink || pNextLink == TargetPoint.pNextLink || 
							    pPrevLink == TargetPoint.pPrevLink || pPrevLink == TargetPoint.pNextLink)){
								NoWayEnable = AI_NO_WAY_NONE;
								aiMoveMode = AI_MOVE_TRACK;
								OtherFlag |= MECHOS_RECALC_FRONT;
							};
						};				
					};				
				};
			};
			aiResolveWayEnable = 2;
			break;
	};			
};

void VangerUnit::ResolveHandlerAttack(aiUnitResolve* p)
{
	GunSlot* s;
	int dx,dy;
	int i;

	if(p->rStatus & AI_RESOLVE_STATUS_VAR){
		if(p->Level > 0) p->Level--;
		else p->rStatus |= AI_RESOLVE_STATUS_DISCONNECT;
	};

	dx = getDistX(((p->Obj).VangerT)->R_curr.x,R_curr.x);
	dy = getDistY(((p->Obj).VangerT)->R_curr.y,R_curr.y);
	if(dx < ViewRadius && dy < ViewRadius){
		s = NULL;
		if(Visibility == VISIBLE || (p->Obj.VangerT)->Visibility == VISIBLE){
			for(i = 0;i < ItemMatrix->NumSlot;i++){
				s = &GunSlotData[ItemMatrix->nSlot[i]];
				if(s->pData && s->GunStatus == GUN_READY && s->CheckTarget(p->Obj.VangerT))
					s->Fire();
			};
		}else{
			for(i = 0;i < ItemMatrix->NumSlot;i++){
				s = &GunSlotData[ItemMatrix->nSlot[i]];
				if(s->pData)
					p->Obj.VangerT->BulletCollision(s->pData->Power / s->pData->WaitTime,this);
			};
		};
	};
};


void VangerUnit::CheckFind(aiUnitResolve* p,Vector v)
{
	int dx,dy;
	if(p->rStatus & AI_RESOLVE_STATUS_TARGET){
		dy = abs(getDistY(v.y,R_curr.y));
		if(dy < radius * 4){
			dx = abs(getDistX(v.x,R_curr.x));
			if(dx < radius*4) ResolveFindDestroy(p);
		};
	}else{
		if(p->rStatus & AI_RESOLVE_STATUS_VIEW){
			dy = abs(getDistY(v.y,R_curr.y));
			if(dy < ViewRadius){
				dx = abs(getDistX(v.x,R_curr.x));
				if(dx < ViewRadius) ResolveFindDestroy(p);
			};
		};
	};
};

void VangerUnit::ResolveFindDestroy(aiUnitResolve* p)
{
	p->rStatus |= AI_RESOLVE_STATUS_DISCONNECT;
	if(aiMoveMode == AI_MOVE_POINT){
		aiMoveMode = AI_MOVE_TRACK;
		OtherFlag |= MECHOS_RECALC_FRONT;
	};
};

aiUnitResolve* VangerUnit::AddFindResolve(int type,UnitOrderType obj,int level,int status)
{
	aiUnitResolve* p;
	p = aiResolveFind.AddResolve(type,obj,level,status);	
	switch(p->Type){
		case UNIT_ORDER_VANGER:
			TargetPoint.vPoint = (p->Obj.ActionT)->R_curr;
			break;
		case UNIT_ORDER_STUFF:
			if((p->Obj.StuffT)->Owner) TargetPoint.vPoint = ((p->Obj.StuffT)->Owner)->R_curr;
			else TargetPoint.vPoint = (p->Obj.StuffT)->R_curr;
			break;
		case UNIT_ORDER_SENSOR:
			TargetPoint.vPoint = (p->Obj.SensorT)->R_curr;
			break;
		case UNIT_ORDER_PASSAGE:
			TargetPoint.vPoint = (p->Obj.PassageT)->ActionLink->R_curr;
			aiLocalTarget = p;
			break;
		case UNIT_ORDER_ENTER:
			TargetPoint.vPoint = ((p->Obj.EnterT)->GetNearCenter(R_curr))->R_curr;
			aiLocalTarget = p;
			break;
		case UNIT_ORDER_DOLL:
			TargetPoint.vPoint = Vector((p->Obj.DollT)->pos_x,(p->Obj.DollT)->pos_y,0);
			break;
		case UNIT_ORDER_VECTOR:
			TargetPoint.vPoint.x = (p->Obj.vT).x;
			TargetPoint.vPoint.y = (p->Obj.vT).y;
			TargetPoint.vPoint.z = 0;
			break;
	};
	HideTrack.GetPosition(&TargetPoint);
	return p;
};

void VangerUnit::ResolveHandlerFind(aiUnitResolve* p)
{
	int dx,dy;
	StuffObject* c;
	Vector vCheck;

	if(aiResolveWayEnable == 0){
		if(p->rStatus & AI_RESOLVE_STATUS_VAR){
			if(p->Level > 0) p->Level--;
			else ResolveFindDestroy(p);
		};

		switch(p->Type){
			case UNIT_ORDER_VANGER:
				TargetPoint.vPoint = (p->Obj.ActionT)->R_curr;
				CheckFind(p,TargetPoint.vPoint);
				break;
			case UNIT_ORDER_STUFF:
				if((p->Obj.StuffT)->Owner) TargetPoint.vPoint = ((p->Obj.StuffT)->Owner)->R_curr;
				else TargetPoint.vPoint = (p->Obj.StuffT)->R_curr;
				if(p->rStatus & AI_RESOLVE_STATUS_VIEW){
					dy = abs(getDistY(TargetPoint.vPoint.y,R_curr.y));
					if(dy < ViewRadius){
						dx = abs(getDistX(TargetPoint.vPoint.x,R_curr.x));
						if(dx < ViewRadius) ResolveFindDestroy(p);
					};
				}else{
					if(!(p->Obj.StuffT)->Owner && (p->Obj.StuffT)->Visibility == UNVISIBLE && Visibility == UNVISIBLE){
						dx = abs(getDistX(((p->Obj).StuffT)->R_curr.x,R_curr.x));
						dy = abs(getDistY(((p->Obj).StuffT)->R_curr.y,R_curr.y));
						if(dx < radius*4 && dy < radius*4){
							(p->Obj.StuffT)->Touch(this);
							Touch(p->Obj.StuffT);
//							p->rStatus |= AI_RESOLVE_STATUS_DISCONNECT;
						};
					};
				};
				break;
			case UNIT_ORDER_SENSOR:
				TargetPoint.vPoint = (p->Obj.SensorT)->R_curr;
				CheckFind(p,TargetPoint.vPoint);
				break;
			case UNIT_ORDER_PASSAGE:
				TargetPoint.vPoint = (p->Obj.PassageT)->ActionLink->R_curr;
				aiLocalTarget = p;
				CheckFind(p,TargetPoint.vPoint);
				break;
			case UNIT_ORDER_ENTER:
				TargetPoint.vPoint = ((p->Obj.EnterT)->GetNearCenter(R_curr))->R_curr;
				aiLocalTarget = p;
				CheckFind(p,TargetPoint.vPoint);
				break;
			case UNIT_ORDER_DOLL:
				TargetPoint.vPoint = Vector((p->Obj.DollT)->pos_x,(p->Obj.DollT)->pos_y,0);
				vCheck = Vector(getDistX((p->Obj.DollT)->pos_x,R_curr.x),getDistX((p->Obj.DollT)->pos_y,R_curr.y),0);
				if(vCheck.vabs() < (int)(RND(radius*5))){
					c = GetStuffObject(this,ACI_EMPTY_CIRTAINER);
					if(c){
						c->ActIntBuffer.data0 = p->Obj.DollT -> getCirt((p->Obj.DollT)->pos_x,(p->Obj.DollT)->pos_y);
						c->ActIntBuffer.data1 = 1 << (p->Obj.DollT) -> gIndex;									
						c->ActIntBuffer.type = uvsSetItemType(c->uvsDeviceType,c->ActIntBuffer.data0,c->ActIntBuffer.data1);
					};
					uvsPoint->goHome();
					ResolveFindDestroy(p);
				};
				break;
			case UNIT_ORDER_VECTOR:
				TargetPoint.vPoint.x = (p->Obj.vT).x;
				TargetPoint.vPoint.y = (p->Obj.vT).y;
				TargetPoint.vPoint.z = 0;
				CheckFind(p,TargetPoint.vPoint);
				break;
		};

		if(!(OtherFlag & MECHOS_CALC_WAY)){
			p->Time++;
			if(p->Time > MaxWayCount){
				HideTrack.GetPosition(&TargetPoint);
				WayInit();
				p->Time = 0;
			};
		};
		
		switch(aiMoveMode){
			case AI_MOVE_POINT:
				if(!CheckReturn2Track()){
					aiMoveMode = AI_MOVE_TRACK;
					OtherFlag |= MECHOS_RECALC_FRONT;
				};
				break;
			case AI_MOVE_TRACK:
				if(CheckReturn2Track()){
					if(PointStatus & TRK_NODE_MASK){
						if((TargetPoint.PointStatus & TRK_NODE_MASK) && pNode == TargetPoint.pNode)
							aiMoveMode = AI_MOVE_POINT;
					}else{
						if((TargetPoint.PointStatus & TRK_BRANCH_MASK) && pBranch == TargetPoint.pBranch && 
							(pNextLink == TargetPoint.pNextLink || pPrevLink == TargetPoint.pNextLink || pNextLink == TargetPoint.pPrevLink || pPrevLink == TargetPoint.pPrevLink))
								aiMoveMode = AI_MOVE_POINT;
					};
				};
				break;
		};
		aiResolveWayEnable = 1;
	}else{
		switch(p->Type){
			case UNIT_ORDER_VANGER:				
				CheckFind(p,(p->Obj.ActionT)->R_curr);
				break;
			case UNIT_ORDER_STUFF:
				if(p->rStatus & AI_RESOLVE_STATUS_VIEW){
					dy = abs(getDistY((p->Obj.StuffT)->R_curr.y,R_curr.y));
					if(dy < ViewRadius){
						dx = abs(getDistX((p->Obj.StuffT)->R_curr.x,R_curr.x));
						if(dx < ViewRadius) ResolveFindDestroy(p);
					};
				}else{
					if(!(p->Obj.StuffT)->Owner && (p->Obj.StuffT)->Visibility == UNVISIBLE && Visibility == UNVISIBLE){
						dx = abs(getDistX(((p->Obj).StuffT)->R_curr.x,R_curr.x));
						dy = abs(getDistY(((p->Obj).StuffT)->R_curr.y,R_curr.y));
						if(dx < radius*4 && dy < radius*4){
							(p->Obj.StuffT)->Touch(this);
							Touch(p->Obj.StuffT);
							ResolveFindDestroy(p);
						};
					};
				};
				break;
			case UNIT_ORDER_SENSOR:
				CheckFind(p,(p->Obj.SensorT)->R_curr);
				break;
			case UNIT_ORDER_PASSAGE:
				CheckFind(p,(p->Obj.PassageT)->ActionLink->R_curr);
				break;
			case UNIT_ORDER_ENTER:
				CheckFind(p,((p->Obj.EnterT)->GetNearCenter(R_curr))->R_curr);
				break;
			case UNIT_ORDER_DOLL:
				TargetPoint.vPoint = Vector((p->Obj.DollT)->pos_x,(p->Obj.DollT)->pos_y,0);
				vCheck = Vector(getDistX((p->Obj.DollT)->pos_x,R_curr.x),getDistX((p->Obj.DollT)->pos_y,R_curr.y),0);
				if(vCheck.vabs() < (int)(RND(radius*5))){
					c = GetStuffObject(this,ACI_EMPTY_CIRTAINER);
					if(c){
						c->ActIntBuffer.data0 = p->Obj.DollT -> getCirt((p->Obj.DollT)->pos_x,(p->Obj.DollT)->pos_y);
						c->ActIntBuffer.data1 = 1 << (p->Obj.DollT) -> gIndex;									
						c->ActIntBuffer.type = uvsSetItemType(c->uvsDeviceType,c->ActIntBuffer.data0,c->ActIntBuffer.data1);									
					};
					uvsPoint->goHome();
					ResolveFindDestroy(p);
				};
				break;
			case UNIT_ORDER_VECTOR:
				CheckFind(p,Vector(p->Obj.vT.x,p->Obj.vT.y,0));
				break;
		};
	};
};

void VangerUnit::CalcImpulse(void)
{	
	Vector vCheck;	
	int t;
	SpeedFactorTime += 2;
	if(CheckNearTrack()){
		TargetPoint.vPoint = aiNearTrack;
		HideTrack.GetPosition(&TargetPoint);
		aiMoveMode = AI_MOVE_POINT;
		aiResolveWayEnable = 2;
		NoWayEnable = AI_NO_WAY_PROCESS;
		aiRunBranch = pBranch;
		aiRunNode = pNode;
		vRunStartPosition = R_curr;
		t = getDistX(R_curr.x,TargetPoint.vPoint.x);
		RunStartDist = t*t;
		t = getDistY(R_curr.y,TargetPoint.vPoint.y);
		RunStartDist += t*t;
		RunTimeProcess = 4*aiRelaxTimeMax;
	}else{
		GetForceWay(aiMaxJumpRadius * aiMaxJumpRadius / 4,vCheck);
		if(vCheck.x != 0 || vCheck.y != 0){
			TargetPoint.vPoint = vCheck;
			HideTrack.GetPosition(&TargetPoint);
			aiMoveMode = AI_MOVE_POINT;
			aiResolveWayEnable = 2;
			NoWayEnable = AI_NO_WAY_PROCESS;
			aiRunBranch = pBranch;
			aiRunNode = pNode;
			vRunStartPosition = R_curr;
			t = getDistX(R_curr.x,TargetPoint.vPoint.x);
			RunStartDist = t*t;
			t = getDistY(R_curr.y,TargetPoint.vPoint.y);
			RunStartDist += t*t;
			RunTimeProcess = 4*aiRelaxTimeMax;
		};
	};	
};	     

void VangerUnit::CalcForce(void)
{	
	Vector vCheck;	
	int t;	
	GetForceWay(aiMaxJumpRadius * aiMaxJumpRadius / 4,vCheck);
	if(vCheck.x != 0 || vCheck.y != 0){
		TargetPoint.vPoint = vCheck;
		HideTrack.GetPosition(&TargetPoint);
		aiMoveMode = AI_MOVE_POINT;
		aiResolveWayEnable = 2;
		NoWayEnable = AI_NO_WAY_PROCESS;
		aiRunBranch = pBranch;
		aiRunNode = pNode;
		vRunStartPosition = R_curr;
		t = getDistX(R_curr.x,TargetPoint.vPoint.x);
		RunStartDist = t*t;
		t = getDistY(R_curr.y,TargetPoint.vPoint.y);
		RunStartDist += t*t;
		RunTimeProcess = 4*aiRelaxTimeMax;
	}else{
		if(CheckNearTrack()){
			TargetPoint.vPoint = aiNearTrack;
			HideTrack.GetPosition(&TargetPoint);
			aiMoveMode = AI_MOVE_POINT;
			aiResolveWayEnable = 2;
			NoWayEnable = AI_NO_WAY_PROCESS;
			aiRunBranch = pBranch;
			aiRunNode = pNode;
			vRunStartPosition = R_curr;
			t = getDistX(R_curr.x,TargetPoint.vPoint.x);
			RunStartDist = t*t;
			t = getDistY(R_curr.y,TargetPoint.vPoint.y);
			RunStartDist += t*t;
			RunTimeProcess = 4*aiRelaxTimeMax;
		};
	};
};	     


int VangerUnit::CheckReturn2Track(void)
{
	int dx,dy;
	int d1,d2;

	dx = getDistX(R_curr.x,TargetPoint.vPoint.x);
	dy = getDistY(R_curr.y,TargetPoint.vPoint.y);
	d1 = round(sqrt(dx*(double)dx + dy*(double)dy));

	if(PointStatus & TRK_NODE_MASK){
		dx = getDistX(R_curr.x,pNode->x);
		dy = getDistY(R_curr.y,pNode->y);
		d2 = round(sqrt(dx*(double)dx + dy*(double)dy));
	}else{
		dx = getDistX(R_curr.x,pNextLink->x);
		dy = getDistY(R_curr.y,pPrevLink->y);
		d2 = round(sqrt(dx*(double)dx + dy*(double)dy));
	};

	if(TargetPoint.PointStatus & TRK_NODE_MASK){
		if(!TargetPoint.pNode) return 0;
		dx = getDistX(TargetPoint.vPoint.x,TargetPoint.pNode->x);
		dy = getDistY(TargetPoint.vPoint.y,TargetPoint.pNode->y);
		d2 += round(sqrt(dx*(double)dx + dy*(double)dy));
	}else{
		if(!TargetPoint.pNextLink) return 0;
		dx = getDistX(TargetPoint.vPoint.x,TargetPoint.pNextLink->x);
		dy = getDistY(TargetPoint.vPoint.y,TargetPoint.pNextLink->y);
		d2 += round(sqrt(dx*(double)dx + dy*(double)dy));
	};

	if(d1 > d2) return 0;
	return 1;
};

void VangerUnit::GetForceWay(int d2,Vector& v)
{
	LinkType* p;
	int d,md;
	int dx,dy;
	Vector vt;

	vt = Vector(0,0,0);
//	if(!SpeedFactorTime){
//		v = vt;
//		return;
//	};

	md = 0;
	if(MoveDir){
		p = pBranch->Link + pBranch->NumLink - 1;
		while(p != pNextLink && p != pBranch->Link){
			dy = R_curr.y - p->y;
			dx = getDistX(R_curr.x,p->x);
			d = dx*dx + dy*dy;
			if(d < d2 && d > md){
				vt.x = p->x;
				vt.y = p->y;
				md = d;
			};
			p--;
		};
	}else{
		p = pBranch->Link;
		while(p != pNextLink && p != (pBranch->Link + pBranch->NumLink - 1)){
			dy = R_curr.y - p->y;
			dx = getDistX(R_curr.x,p->x);
			d = dx*dx + dy*dy;
			if(d < d2 && d > md){
				vt.x = p->x;
				vt.y = p->y;
				md = d;
			};
			p++;
		};		
	};
	v = vt;
};

int VangerUnit::CheckSpeetle(void)
{
	int i;
	for(i = 0;i < ItemMatrix->NumSlot;i++){
		if(GunSlotData[ItemMatrix->nSlot[i]].pData && (GunSlotData[ItemMatrix->nSlot[i]].ItemData->ActIntBuffer.type == ACI_SPEETLE_SYSTEM_LIGHT || GunSlotData[ItemMatrix->nSlot[i]].ItemData->ActIntBuffer.type == ACI_SPEETLE_SYSTEM_HEAVY))
			return 1;
	};
	return 0;
};

void VangerUnit::CheckAmmo(void)
{
	int i;
	int s1,s2;
	s2 = s1 = 0;

	for(i = 0;i < ItemMatrix->NumSlot;i++){
		if(GunSlotData[ItemMatrix->nSlot[i]].pData){
			if(!GunSlotData[ItemMatrix->nSlot[i]].ItemData->ActIntBuffer.data1)
				s1++;
			s2++;
		};
	};

	if(s1 == s2)
		LowAmmo = 1;
}; 

void VangerUnit::WeaponGenerator(void)
{
	int i;
	StuffObject* p;
	GunDevice* pp;	
	int ms,s,mp,d;
	GunSlot* gs;

	WeaponDelta = 0;
	AttackRadius = 300;

	for(i = 0;i < ItemMatrix->NumSlot;i++){
		gs = &(GunSlotData[ItemMatrix->nSlot[i]]);
		if(gs->pData)
			gs->CloseGun();
	};

	for(i = 0;i < ItemMatrix->NumSlot;i++){
		if(!GunSlotData[ItemMatrix->nSlot[i]].pData){
			s = ItemMatrix->SlotSize[i];
			ms = 0;
			mp = 0;
			pp = NULL;
			p = DeviceData;
			while(p){
				if(p->StuffType == DEVICE_ID_GUN && p->ActIntBuffer.slot == -1 && p->ActIntBuffer.data1 > 0 && 
				   p->SizeID >= ms && ((GunDevice*)(p))->pData->Power >= mp){
					if(p->SizeID > 0){
						if(p->SizeID <= s){
							ms = p->SizeID;
							pp = (GunDevice*)(p);
							mp = pp->pData->Power;
							
						};
					}else{
						if(p->SizeID == 0 && s == 0){
							ms = p->SizeID;
							pp = (GunDevice*)(p);
							mp = pp->pData->Power;							
						};
					};
				};
				p = p->NextDeviceList;
			};
			if(pp){
				GunSlotData[ItemMatrix->nSlot[i]].OpenGun(pp);
				if(pp->pData->WaitTime) WeaponDelta += pp->pData->Power / pp->pData->WaitTime;
				else WeaponDelta += pp->pData->Power;
				d = pp->pData->LifeTime * pp->pData->Speed;
				if(d < AttackRadius) AttackRadius = d;
			};
		};
	};
};

int VangerUnit::CheckNearTrack(void)
{
	int i,j;

	LinkType* ml;
	BranchType* mb;
	NodeType* nn;

	LinkType* cl;
	NodeType* cn;

	int cDist;
	int mDist,bDist,dx;

	aiNearTrack = Vector(0,0,0);

//	if(!SpeedFactorTime)
//		return 0;

	mb = HideTrack.branch;
	cDist = WayDist[pNode->index];
	bDist = 2*aiMaxJumpRadius;
	mDist = 0;

	cl = NULL;
	for(i = 0;i < HideTrack.NumBranch;i++,mb++){
		if((mb->pBeg && WayDist[mb->pBeg->index] < cDist) || (mb->pEnd && WayDist[mb->pEnd->index] < cDist)){
			ml = mb->Link;
			for(j = 0;j < mb->NumLink;j++){
				dx = abs(getDistX(R_curr.x,ml->x)) + abs(R_curr.y - ml->y);
				if(dx < bDist && dx > mDist){
					mDist = dx;
					cl = ml;
				};
				ml++;
			};
		};
	};

	nn = HideTrack.node;
	cn = NULL;
	for(i = 0;i < HideTrack.NumNode;i++,nn++){
		if(WayDist[nn->index] < cDist){
			dx = abs(getDistX(R_curr.x,nn->x)) + abs(R_curr.y - nn->y);
			if(dx < bDist && dx > mDist){
				mDist = dx;
				cn = nn;
			};
		};
	};

	if(cn){
		aiNearTrack = Vector(cn->x,cn->y,cn->z);
		return 1;
	}else{
		if(cl){
			aiNearTrack = Vector(cl->x,cl->y,cl->z);
			return 1;
		};
	};
	return 0;
};

int GunSlot::CheckTarget(ActionUnit* p)
{
	int d,v,a,l;
	Vector vCheck,vStart,vEnd,vTan;	

	if(pData->BulletMode & BULLET_CONTROL_MODE::SPEED) v = pData->Speed + Owner->Speed;
	else v = pData->Speed;

	d = v * pData->LifeTime;
	vStart = Owner->R_curr;

	aiTargetObject = NULL;

	if(Owner->aiMoveFunction == AI_MOVE_FUNCTION_WHEEL && (Owner->PowerFlag & VANGER_POWER_FLY) && GetStuffObject(Owner,ACI_RADAR_DEVICE)){
		if(pData->BulletMode & BULLET_CONTROL_MODE::AIM){
			vEnd = p->R_curr;
			vCheck = Vector(getDistX(vEnd.x,vStart.x),getDistY(vEnd.y,vStart.y),vEnd.z - vStart.z);
			if(vCheck.abs2() > d*d) return 0;
			if(!(pData->BulletMode & BULLET_CONTROL_MODE::FLY) && MapLineTrace(vStart,vEnd)) return 0;
			aiTargetObject = p;
		}else{
			if(pData->BulletMode & BULLET_CONTROL_MODE::TARGET){	
				vEnd = p->R_curr;				
				vCheck = Vector(getDistX(vEnd.x,vStart.x),getDistY(vEnd.y,vStart.y),vEnd.z - vStart.z);
				if(vCheck.abs2() > d*d) return 0;
				vTan = Vector(vCheck.x,vCheck.y,0);
				a = rPI(Owner->Angle - vTan.psi());
				if(a > PI) a -= 2*PI;
				if(abs(a) > PI/4) return 0;
				if(!(pData->BulletMode & BULLET_CONTROL_MODE::FLY) && MapLineTrace(vStart,vEnd)) return 0;
				aiTargetObject = p;
			}else{
				vEnd = Vector(d,0,0)*Owner->RotMat;
				vEnd += Owner->R_curr;
				cycleTor(vEnd.x,vEnd.y);				
				if(!TouchSphere(vStart,vEnd,p->R_curr,p->radius,l)) return 0;
				vTan = p->R_curr;
				if(!(pData->BulletMode & BULLET_CONTROL_MODE::FLY) && MapLineTrace(vStart,vTan)) return 0;
			};
		};
	}else{		
		vEnd = Vector(d,0,0)*Owner->RotMat;
		vEnd += Owner->R_curr;
		cycleTor(vEnd.x,vEnd.y);
		if(!TouchSphere(vStart,vEnd,p->R_curr,p->radius,l)) return 0;
		vTan = p->R_curr;
		if(!(pData->BulletMode & BULLET_CONTROL_MODE::FLY) && MapLineTrace(vStart,vTan)) return 0;
	};
	return 1;
};

aiUnitEvent* VangerUnit::AddEvent(int id,GeneralObject* obj,VangerUnit* subj)
{
	aiUnitEvent* l;

	l = (aiUnitEvent*)(aiEvent.Tail);
	while(l){
		if(l->ID == id && l->Obj == obj && l->Subj == subj){
			l->Refresh = 1;
			return l;
		};
		l = (aiUnitEvent*)(l->Next);
	};

	l = new aiUnitEvent;
	l->ID = id;
	l->Obj = obj;
	l->Subj = subj;
	l->Refresh = 1;
	l->Time = 0;
	aiEvent.Connect(l);

	switch(id){
		case AI_EVENT_ITEM:
			aiResolveFind.AddResolve(UNIT_ORDER_STUFF,UnitOrderType((StuffObject*)(obj)))->Time = MaxWayCount;
			break;
	};
	return l;
};

void VangerUnit::ClearSubjEvent(VangerUnit* subj)
{
	aiUnitEvent* l;
	l = (aiUnitEvent*)(aiEvent.Tail);
	while(l){
		if(l->Subj == subj)
			l->Refresh = 0;
		l = (aiUnitEvent*)(l->Next);
	};
};

void VangerUnit::ClearObjEvent(GeneralObject* obj)
{
	aiUnitEvent* l;
	l = (aiUnitEvent*)(aiEvent.Tail);
	while(l){
		if(l->Obj == obj)
			l->Refresh = 0;
		l = (aiUnitEvent*)(l->Next);
	};
};

void VangerUnit::ClearEvent(GeneralObject* obj)
{
	aiUnitEvent* l;
	l = (aiUnitEvent*)(aiEvent.Tail);
	if(obj->ID == ID_VANGER){
		while(l){
			if(l->Obj == obj || l->Subj == (VangerUnit*)(obj))
				l->Refresh = 0;
			l = (aiUnitEvent*)(l->Next);
		};
	}else{
		while(l){
			if(l->Obj == obj)
				l->Refresh = 0;
			l = (aiUnitEvent*)(l->Next);
		};
	};
};

int VangerUnit::GetAllert(VangerUnit* p)
{
	int i,s;
	GunSlot* gs;
	s = 0;

	for(i = 0;i < p->ItemMatrix->NumSlot;i++){
		gs = &(p->GunSlotData[p->ItemMatrix->nSlot[i]]);
		if(gs->pData){
			if(gs->pData->WaitTime) s += gs->pData->Power / gs->pData->WaitTime;
			else s += gs->pData->Power;
		};
	};

	if(WeaponDelta){
		if(s == 0) s = 1;
		else s = ((Armor + Energy) / s) - ((p->Armor + p->Energy) / WeaponDelta);
	}else{
		if(s) s = -(Armor + Energy) / s;
		else s = Armor - p->Armor;
	};
	return s;
};

void VangerUnit::GetWeaponDelta(void)
{
	int i,s;
	GunSlot* gs;
	s = 0;

	for(i = 0;i < ItemMatrix->NumSlot;i++){
		gs = &(GunSlotData[ItemMatrix->nSlot[i]]);
		if(gs->pData){
			if(gs->pData->WaitTime) s += gs->pData->Power / gs->pData->WaitTime;
			else s += gs->pData->Power;
		};
	};
	WeaponDelta = s;
};

void VangerUnit::VangerDestroyHandler(GeneralObject* p)
{
	if(aiDamageData == p){
		aiDamage = 0;
		aiDamageData = NULL;
	};

	aiResolveFind.ClearResolve(UNIT_ORDER_VANGER,UnitOrderType((ActionUnit*)(p)));
	aiResolveAttack.ClearResolve(UNIT_ORDER_VANGER,UnitOrderType((ActionUnit*)(p)));
	ClearEvent(p);
};

void VangerUnit::StuffDestroyHandler(GeneralObject* p)
{
	aiResolveFind.ClearResolve(UNIT_ORDER_STUFF,UnitOrderType((StuffObject*)(p)));
	ClearEvent(p);
};

void VangerUnit::InitAI(void)
{
	aiActionID = AI_ACTION_AUTO;

	aiStatus = AI_STATUS_NONE;
	
	aiNearTrack = Vector(0,0,0);
	aiLocalTarget = NULL;
	aiResolveWayEnable = 0;	
	NoWayDirect = -1;

	aiScanDist = radius*3;

	aiRunBranch = NULL;
	aiRunNode = NULL;
	vRunStartPosition = Vector(0,0,0);
	RunStartDist = 0;
	RunTimeProcess = 0;

	ViewRadius = 300;

	aiMaxJumpRadius = max_jump_distance;

	uvsMaxSpeed = round((double)(TotalVangerSpeed)*speed_factor);

	MaxHideSpeed = uvsMaxSpeed;
	MakeTrackDist();
	NoWayEnable = AI_NO_WAY_NONE;

	aiReactionCheckTime = 0;
	aiReactionMode = 0;
	aiReactionTime = 0;
	aiReactionCheckTime = 0;

	aiReactionTimeMax = 30;
	aiReactionCheckTimeMax = 50;

	AttackRadius = 300;
	MixVectorEnable = 1;
	
	SeedNum = 0;
	MaxSeed = 5 + RND(10);

	LowAmmo = 0;
	LowArmor = 0;	

	WallCollisionTime = 0;
	DeltaTractionTime = 0;
	TractionValue = 0;
	TractionStat = 0;

	SpeedFactorTime = 0;

	aiAlarmTime = 0;
	aiDamage = 0;
	aiDamageData = NULL;

	BeebonationFlag = 0;

	ArmorAlarm = 2*MaxArmor / 3;

	if((Status & SOBJ_AUTOMAT) && !NetworkON)
		WeaponGenerator();
	else
		GetWeaponDelta();

	AddAttackTime = 0;

	aiRelaxTime = 0;
	aiRealxRadius = radius*4;
	aiRelaxTimeMax = 4 * aiRealxRadius / uvsMaxSpeed;
	aiRelaxData = R_curr;

	switch(uvsPoint->shape){
		case UVS_VANGER_SHAPE::GAMER_SLAVE:
			aiVangerRadius = 400;
			aiItemRadius = 400;

			aiFirstItem = -1;
			aiSecondItem = -1;
			aiActionID = AI_ACTION_BRAKE;

			FactorOpen(&aiFactorData[AI_FACTOR_TRANSPORT]);
			FactorOpen(&aiFactorData[AI_FACTOR_ALARM_NORMAL]);
			FactorOpen(&aiFactorData[AI_FACTOR_TARGET_MONO]);
			FactorOpen(&aiFactorData[AI_FACTOR_OTHER]);
			break;
		case UVS_VANGER_SHAPE::GAMER:
			aiVangerRadius = 400;
			aiItemRadius = 400;

			aiFirstItem = -1;
			aiSecondItem = -1;

			aiActionID = AI_ACTION_AUTO;

			FactorOpen(&aiFactorData[AI_FACTOR_KILLER]);
			FactorOpen(&aiFactorData[AI_FACTOR_ALARM_GAMER]);
			FactorOpen(&aiFactorData[AI_FACTOR_TARGET_MONO]);
			FactorOpen(&aiFactorData[AI_FACTOR_OTHER_GAMER]);
			break;
		case UVS_VANGER_SHAPE::KILLER:
			aiVangerRadius = 400;
			aiItemRadius = 400;

			aiFirstItem = -1;
			aiSecondItem = -1;

			aiActionID = AI_ACTION_RANGER;

			FactorOpen(&aiFactorData[AI_FACTOR_KILLER]);
			FactorOpen(&aiFactorData[AI_FACTOR_ALARM_FREEZER]);
			FactorOpen(&aiFactorData[AI_FACTOR_TARGET_MONO]);
			FactorOpen(&aiFactorData[AI_FACTOR_OTHER]);
			break;
		case UVS_VANGER_SHAPE::THIEF:
			aiVangerRadius = 400;
			aiItemRadius = 400;

			aiFirstItem = -1;
			aiSecondItem = -1;
			aiActionID = AI_ACTION_THIEF;

			FactorOpen(&aiFactorData[AI_FACTOR_THIEF]);
			FactorOpen(&aiFactorData[AI_FACTOR_ALARM_FUNK]);
			FactorOpen(&aiFactorData[AI_FACTOR_TARGET_MONO]);
			FactorOpen(&aiFactorData[AI_FACTOR_OTHER]);
			break;
		default:
			if(PowerFlag & VANGER_POWER_RUFFA_GUN){
				aiVangerRadius = 400;
				aiItemRadius = 400;

				aiFirstItem = -1;
				aiSecondItem = -1;

				aiActionID = AI_ACTION_RACER;

				FactorOpen(&aiFactorData[AI_FACTOR_TRANSPORT]);
				FactorOpen(&aiFactorData[AI_FACTOR_ALARM_FUNK]);
				FactorOpen(&aiFactorData[AI_FACTOR_TARGET_MONO]);
				FactorOpen(&aiFactorData[AI_FACTOR_OTHER]);
			}else{
				switch(uvsPoint->status){
					case UVS_VANGER_STATUS::RACE:
						aiVangerRadius = 400;
						aiItemRadius = 400;

						aiFirstItem = ACI_ELEECH;
						aiSecondItem = -1;

						aiActionID = AI_ACTION_RACER;

						FactorOpen(&aiFactorData[AI_FACTOR_RACER]);
						FactorOpen(&aiFactorData[AI_FACTOR_ALARM_NORMAL]);
						FactorOpen(&aiFactorData[AI_FACTOR_TARGET_MONO]);
						FactorOpen(&aiFactorData[AI_FACTOR_OTHER]);
						break;
					case UVS_VANGER_STATUS::RACE_HUNTER:
						aiVangerRadius = 400;
						aiItemRadius = 400;

						aiFirstItem = ACI_WEEZYK;
						aiSecondItem = -1;
						aiActionID = AI_ACTION_RACER;

						FactorOpen(&aiFactorData[AI_FACTOR_RACER]);
						FactorOpen(&aiFactorData[AI_FACTOR_ALARM_NORMAL]);
						FactorOpen(&aiFactorData[AI_FACTOR_TARGET_MONO]);
						FactorOpen(&aiFactorData[AI_FACTOR_OTHER]);
						break;
					default:
						if(CurrentWorld == WORLD_GLORX && (uvsCurrentCycle == 1 || uvsCurrentCycle == 2) && !RND(3)){
							aiVangerRadius = 400;
							aiItemRadius = 400;

							if(uvsCurrentCycle == 1) aiFirstItem = ACI_PIPETKA;
							else aiFirstItem = ACI_KERNOBOO;
							aiSecondItem = -1;
							aiActionID = AI_ACTION_FARMER;

							FactorOpen(&aiFactorData[AI_FACTOR_RACER]);
							FactorOpen(&aiFactorData[AI_FACTOR_ALARM_NORMAL]);
							FactorOpen(&aiFactorData[AI_FACTOR_TARGET_MONO]);
							FactorOpen(&aiFactorData[AI_FACTOR_OTHER]);
						}else{
							if(uvsPoint->shape ==  UVS_VANGER_SHAPE::RANGER){
								aiVangerRadius = 400;
								aiItemRadius = 400;

								aiFirstItem = -1;
								aiSecondItem = -1;

								aiActionID = AI_ACTION_RANGER;

								FactorOpen(&aiFactorData[AI_FACTOR_RANGER]);
								FactorOpen(&aiFactorData[AI_FACTOR_ALARM_FREEZER]);
								FactorOpen(&aiFactorData[AI_FACTOR_TARGET_FREEZER]);
								FactorOpen(&aiFactorData[AI_FACTOR_OTHER]);
							}else{
								aiVangerRadius = 400;
								aiItemRadius = 400;

								aiFirstItem = -1;
								aiSecondItem = -1;

								aiActionID = AI_ACTION_SLAVE;
								FactorOpen(&aiFactorData[AI_FACTOR_TRANSPORT]);
								FactorOpen(&aiFactorData[AI_FACTOR_ALARM_NORMAL]);
								FactorOpen(&aiFactorData[AI_FACTOR_TARGET_MONO]);
								FactorOpen(&aiFactorData[AI_FACTOR_OTHER]);
							};
						};
						break;
				};	
			};
			break;		
	};
	Molerizator = 0;
	VangerCloneID = 0;	
};

extern uvsVanger* Gamer;
int GetEscaveDist(void)
{
	uvsEscave* p1;
	uvsSpot* p2;
	uvsPassage *p3;
	int md,d;
	Vector vCheck;
	
	md = -1;	

	p1 = (uvsEscave*)EscaveTail;
	while(p1){
		if (((uvsEscave*)p1) -> Pworld -> gIndex == CurrentWorld ){
			vCheck = Vector(getDistX(p1->pos_x,Gamer->pos_x),getDistY(p1->pos_y,Gamer->pos_y),0);
			d = vCheck.vabs();
			if(d < md || md == -1) md = d;
		}
		p1 = (uvsEscave*)(p1 -> next);
	};

	p2 = (uvsSpot*)SpotTail;
	while(p2){
		if (((uvsSpot*)p2) -> Pworld -> gIndex == CurrentWorld ){
			vCheck = Vector(getDistX(p2->pos_x,Gamer->pos_x),getDistY(p2->pos_y,Gamer->pos_y),0);
			d = vCheck.vabs();
			if(d < md || md == -1) md = d;
		}
		p2 = (uvsSpot*)(p2 -> next);
	};

	if ( md == -1 ){
		p3 = (uvsPassage*)PassageTail;
		while(p3){
			if (((uvsPassage*)p3) -> Pworld -> gIndex == CurrentWorld ){
				vCheck = Vector(getDistX(p3->pos_x,Gamer->pos_x),getDistY(p3->pos_y,Gamer->pos_y),0);
				d = vCheck.vabs();
				if(d < md || md == -1) md = d;
			}
			p3 = (uvsPassage*)(p3 -> next);
		};
	}
	
	return md;
};

void aiResolveList::ClearResolve(void)
{
	aiUnitResolve* l;
	l = (aiUnitResolve*)(Tail);
	while(l){
		l->rStatus |= AI_RESOLVE_STATUS_DISCONNECT;
		l = (aiUnitResolve*)(l->Next);
	};
};

void aiResolveList::ClearResolve(int type,UnitOrderType obj)
{
	aiUnitResolve* l;
	switch(type){
		case UNIT_ORDER_VANGER:			
			l = (aiUnitResolve*)(Tail);
			while(l){
				if(l->Obj.VangerT == obj.VangerT)
					l->rStatus |= AI_RESOLVE_STATUS_DISCONNECT;
				l = (aiUnitResolve*)(l->Next);
			};
			break;
		case UNIT_ORDER_STUFF:
			l = (aiUnitResolve*)(Tail);
			while(l){
				if(l->Obj.StuffT == obj.StuffT)
					l->rStatus |= AI_RESOLVE_STATUS_DISCONNECT;
				l = (aiUnitResolve*)(l->Next);
			};
			break;
	};
};

void aiResolveList::ClearResolveForce(UnitOrderType obj)
{
	aiUnitResolve* l;
	l = (aiUnitResolve*)(Tail);
	while(l){
		if(l->Obj.VangerT == obj.VangerT)
			l->rStatus |= AI_RESOLVE_STATUS_DISCONNECT;
		l = (aiUnitResolve*)(l->Next);
	};
};

aiUnitResolve* aiResolveList::FindResolve(int type,UnitOrderType obj)
{	
	aiUnitResolve* l;
	l = NULL;
	switch(type){
		case UNIT_ORDER_VANGER:			
			l = (aiUnitResolve*)(Tail);
			while(l){
				if(l->Obj.VangerT == obj.VangerT)
					break;
				l = (aiUnitResolve*)(l->Next);
			};
			break;
		case UNIT_ORDER_STUFF:
			l = (aiUnitResolve*)(Tail);
			while(l){
				if(l->Obj.StuffT == obj.StuffT)
					break;
				l = (aiUnitResolve*)(l->Next);
			};
			break;
		case UNIT_ORDER_SENSOR:
			l = (aiUnitResolve*)(Tail);
			while(l){
				if(l->Obj.SensorT == obj.SensorT)
					break;
				l = (aiUnitResolve*)(l->Next);
			};
			break;
	};
	return l;
};

aiUnitResolve* aiResolveList::AddResolve(int type,UnitOrderType obj,int level,int status)
{
	aiUnitResolve* l;
	l = FindResolve(type,obj);
	if(l){
		l->Level += level;
		Disconnect(l);
		Connect(l);
		return l;
	};
	l = new aiUnitResolve;	
	l->Type = type;
	l->Obj = obj;
	l->Time = 0;
	l->rStatus = status;
	l->Level = level;
	Connect(l);
	return l;
};

aiUnitResolve* aiResolveList::FastAddResolve(int type,UnitOrderType obj,int level,int status)
{
	aiUnitResolve* l;
	l = new aiUnitResolve;
	l->Type = type;
	l->Obj = obj;
	l->Time = 0;
	l->rStatus = status;
	l->Level = level;
	Connect(l);
	return l;
};

time_t StartGameTime;
time_t  PrevLifeTime;
time_t PrevKillTime;

time_t MinLifeTime;
time_t MaxLifeTime;
time_t MinKillTime;
time_t MaxKillTime;


void NetStatisticInit(void)
{
	my_player_body.kills = 0;
	my_player_body.deaths = 0;
	
	switch(my_server_data.GameType){
		case VAN_WAR:		
			my_player_body.VanVarStat.MaxLiveTime = 0;
			my_player_body.VanVarStat.MinLiveTime = 0;
			my_player_body.VanVarStat.KillFreq = 0;
			my_player_body.VanVarStat.DeathFreq = 0;

			MinLifeTime = 0;
			MaxLifeTime = 0;
			MinKillTime = 0;
			MaxKillTime = 0;
			break;
		case PASSEMBLOSS:
			my_player_body.PassemblossStat.TotalTime = 0;
			my_player_body.PassemblossStat.MinTime = 0;
			my_player_body.PassemblossStat.MaxTime = 0;
			my_player_body.PassemblossStat.CheckpointLighting = 0;
			break;
		case MECHOSOMA:
			my_player_body.MechosomaStat.ItemCount1 = 0;
			my_player_body.MechosomaStat.ItemCount2 = 0;
			my_player_body.MechosomaStat.MaxTransitTime = 0;
			my_player_body.MechosomaStat.MinTransitTime = 0;
			my_player_body.MechosomaStat.SneakCount = 0;
			my_player_body.MechosomaStat.LostCount = 0;  
			break;
	};
};

void NetStatisticGameStart(void)
{
	time(&StartGameTime);
	PrevKillTime = PrevLifeTime = StartGameTime;	
};

void NetStatisticUpdate(int id)
{
	time_t t,d;
	time(&t);
	switch(my_server_data.GameType){
		case VAN_WAR:
			switch(id){
				case NET_STATISTICS_KILL:
					my_player_body.kills++;
					d = labs((long)(t - PrevKillTime));
					if(MinKillTime == 0 || MinKillTime > d) MinKillTime = d;
					if(MaxKillTime == 0 || MaxKillTime < d) MaxKillTime = d;
					PrevKillTime = t;
					if(MinKillTime == MaxKillTime)my_player_body.VanVarStat.KillFreq = MaxKillTime;
					else my_player_body.VanVarStat.KillFreq = (MaxKillTime - MinKillTime) / 2;
					break;
				case NET_STATISTICS_DEATH:
					my_player_body.deaths++;
					d = labs((long)(t - PrevLifeTime));
					if(MinLifeTime == 0 || MinLifeTime > d) MinLifeTime = d;
					if(MaxLifeTime == 0 || MaxLifeTime < d) MaxLifeTime = d;
					PrevLifeTime = t;
					if(MinLifeTime == MaxLifeTime) my_player_body.VanVarStat.DeathFreq = MaxLifeTime;
					else my_player_body.VanVarStat.DeathFreq = (MaxLifeTime - MinLifeTime) / 2;
					break;
			};
			break;
		case PASSEMBLOSS:
			switch(id){
				case NET_STATISTICS_KILL:
					my_player_body.kills++;
					break;
				case NET_STATISTICS_DEATH:
					my_player_body.deaths++;
					break;
				case NET_STATISTICS_CHECKPOINT:
					my_player_body.PassemblossStat.CheckpointLighting = UsedCheckNum;
					my_player_body.PassemblossStat.MaxTime = labs((long)(t - StartGameTime));
					break;
				case NET_STATISTICS_END_RACE:
					my_player_body.PassemblossStat.CheckpointLighting = UsedCheckNum;
					my_player_body.PassemblossStat.TotalTime =
						my_player_body.PassemblossStat.MaxTime = labs((long)(t - StartGameTime));
					break;
			};
			break;
		case MECHOSOMA:
			switch(id){
				case NET_STATISTICS_KILL:
					my_player_body.kills++;
					break;
				case NET_STATISTICS_DEATH:
					my_player_body.deaths++;
					break;
				case NET_STATISTICS_CHECKPOINT:
				case NET_STATISTICS_END_RACE:
					my_player_body.MechosomaStat.MinTransitTime = labs((long)(t - StartGameTime));
					break;
			};
			break;
	};				
};

aiUnitEvent* VangerUnit::FindEvent(VangerUnit* subj,GeneralObject* obj)
{
	aiUnitEvent* l;
	l = (aiUnitEvent*)(aiEvent.Tail);
	while(l){
		if(l->Subj == subj && l->Obj == obj)
			return l;
		l = (aiUnitEvent*)(l->Next);
	};	
	return NULL;
};

void VangerUnit::AddEventTime(VangerUnit* p,int delta,int mode)
{
	aiUnitEvent* l;
	l = (aiUnitEvent*)(aiEvent.Tail);
	while(l){
		if(l->Subj == p){
			switch(mode){
				case AI_ADD_FRONT_COLLISION:
					if(l->Time < aiMaxFrontCollision)
						l->Time += delta;
					break;
				case AI_ADD_BACK_COLLISION:
					if(l->Time < aiMaxBackCollision)
						l->Time += delta;
					break;
				case AI_ADD_SIDE_COLLISION:
					if(l->Time < aiMaxSideCollision)
						l->Time += delta;
					break;
				case AI_ADD_ATTACK:
					if(l->Time < aiMaxAttack)
						l->Time += delta;
					break;
			};
			return;			
		};
		l = (aiUnitEvent*)(l->Next);
	};	
};

void VangerUnit::DestroyEnvironment(void)
{      
	VangerUnit* p;

/*	p = (VangerUnit*)(ActD.Tail);
	while(p){
		if(p->ID != ID_VANGER)
			ErrH.Abort("Bad List Mechos");
		p = (VangerUnit*)(p->NextTypeList);
	};*/

	Vector vCheck;
	int d;
	p = (VangerUnit*)(ActD.Tail);
	int ml;
	ml = 5 * radius;	
	while(p){
//		if(p->ID != ID_VANGER)
//			ErrH.Abort("Bad Vanger List");
		if(p->Visibility == VISIBLE && p != this){
			vCheck = Vector(getDistX(p->R_curr.x,R_curr.x),getDistY(p->R_curr.y,R_curr.y),p->R_curr.z - R_curr.z);
			d = vCheck.vabs();
			if(d < ml){
				p->impulse(vCheck,30*(ml - d) / ml,0);
				p->BulletCollision((MaxEnergy / ml)*(ml - d),NULL);
			};
		};		
		p = (VangerUnit*)(p->NextTypeList);
	};
};

int Object::UsingCopterig(int decr_8)
{
	return 0;
};

int Object::UsingCrotrig(int decr_8)
{ 	
	return 0;
};

int Object::UsingCutterig(int decr_8)
{
	return 0;
};

int VangerUnit::UsingCopterig(int decr_8)
{
	StuffObject* p;
	int i;
	
	
	if(uvsPoint->Pmechos->type == FLY_DEVICE_DEFAULT) return 1;

	if(UseDeviceMask & DEVICE_MASK_COPTE){
		if(Status & SOBJ_ACTIVE){
			if(Status & SOBJ_AUTOMAT) return 0;
			for(i = 0;i < MAX_ACTIVE_SLOT;i++){
				if(ActD.Slot[i] && ActD.Slot[i]->ActIntBuffer.type == ACI_COPTE_RIG){
					if(!decr_8) return 1;
					p = ActD.Slot[i];
					if(p->ActIntBuffer.data1 <= 0){
						p->ActIntBuffer.data1 = 0;
						p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
						aciChangeItem(&(p->ActIntBuffer));
						aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
						aciSendEvent2actint(ACI_PUT_ITEM,&(p->ActIntBuffer));
					}else p->ActIntBuffer.data1--;
					return 1;
				};
			};
			p = GetStuffObject(this,ACI_COPTE_RIG);
			if(p){
				aciSendEvent2actint(ACI_PUT_IN_SLOT,&(p->ActIntBuffer));
				return 1;
			};
		}else{			
			if(aiMoveFunction != AI_MOVE_FUNCTION_FLY) return 0;
			if(!decr_8) return 1;
			if(CoptePoint->ActIntBuffer.data1 <= 0){
				CoptePoint->ActIntBuffer.data1 = 0;
				CoptePoint->ActIntBuffer.type = uvsSetItemType(CoptePoint->uvsDeviceType,CoptePoint->ActIntBuffer.data0,CoptePoint->ActIntBuffer.data1);
			}else{
				CoptePoint->ActIntBuffer.data1--;
				return 1;
			};
		};
	};
	return 0;
};

int VangerUnit::UsingCrotrig(int decr_8)
{ 
	StuffObject* p;
	int i;

	
	if(CurrentWorld == WORLD_HMOK) return 0;
	if(uvsPoint->Pmechos->type == MOLE_DEVICE_DEFAULT) return 1;

	if(UseDeviceMask & DEVICE_MASK_MOLE){
		if(Status & SOBJ_ACTIVE){
			if(Status & SOBJ_AUTOMAT) return 0;
			for(i = 0;i < MAX_ACTIVE_SLOT;i++){
				if(ActD.Slot[i] && ActD.Slot[i]->ActIntBuffer.type == ACI_CROT_RIG){
					if(!decr_8) return 1;
					p = ActD.Slot[i];					
					if(p->ActIntBuffer.data1 <= 0){
						p->ActIntBuffer.data1 = 0;
						p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
						aciChangeItem(&(p->ActIntBuffer));
						aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
						aciSendEvent2actint(ACI_PUT_ITEM,&(p->ActIntBuffer));
					}else p->ActIntBuffer.data1--;
					return 1;
				};
			};			
			p = GetStuffObject(this,ACI_CROT_RIG);
			if(p){
				aciSendEvent2actint(ACI_PUT_IN_SLOT,&(p->ActIntBuffer));
				return 1;
			};
		}else{
			return 0;
/*			if(!decr_8) return 1;
			if(MolePoint->ActIntBuffer.data1 <= 0){
				MolePoint->ActIntBuffer.data1 = 0;
				MolePoint->ActIntBuffer.type = uvsSetItemType(MolePoint->uvsDeviceType,MolePoint->ActIntBuffer.data0,MolePoint->ActIntBuffer.data1);
			}else MolePoint->ActIntBuffer.data1--;
			return 1;*/
		};
	};
	return 0;
};


int VangerUnit::UsingCutterig(int decr_8)
{
	StuffObject* p;
	int i;

	if(uvsPoint->Pmechos->type == SWIM_DEVICE_DEFAULT || PowerFlag & VANGER_POWER_SWIM_ALWAYS) return 1;
	if(UseDeviceMask & DEVICE_MASK_SWIM){
		if(Status & SOBJ_ACTIVE){
			if(Status & SOBJ_AUTOMAT) return 0;
			for(i = 0;i < MAX_ACTIVE_SLOT;i++){
				if(ActD.Slot[i] && ActD.Slot[i]->ActIntBuffer.type == ACI_CUTTE_RIG){
					if(!decr_8) return 1;
					p = ActD.Slot[i];					
					if(p->ActIntBuffer.data1 <= 0){
						p->ActIntBuffer.data1 = 0;
						p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
						aciChangeItem(&(p->ActIntBuffer));
						aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
						aciSendEvent2actint(ACI_PUT_ITEM,&(p->ActIntBuffer));
					}else p->ActIntBuffer.data1--;
					return 1;
				};
			};			
			p = GetStuffObject(this,ACI_CUTTE_RIG);
			if(p){
				aciSendEvent2actint(ACI_PUT_IN_SLOT,&(p->ActIntBuffer));
				return 1;
			};
		}else{
			return 0;
/*			if(!decr_8) return 1;
			if(SwimPoint->ActIntBuffer.data1 <= 0){
				SwimPoint->ActIntBuffer.data1 = 0;
				SwimPoint->ActIntBuffer.type = uvsSetItemType(SwimPoint->uvsDeviceType,SwimPoint->ActIntBuffer.data0,SwimPoint->ActIntBuffer.data1);				
			}else SwimPoint->ActIntBuffer.data1--;
			return 1;*/
		};
	};
	return 0;
};

void VangerUnit::NetCreateVanger(uvsPassage* pp,uvsEscave* pe,uvsSpot* ps)
{
	//zNfo NetCreateVanger

	//NETWORK_OUT_STREAM.set_position(ViewX,ViewY,round(2.*(fabs(curGMap -> xsize*sinTurnInvFlt) + fabs(curGMap -> ysize*cosTurnInvFlt))*0.5));
	NETWORK_OUT_STREAM.create_permanent_object(NetID,R_curr.x,R_curr.y,radius);
	int scr_size = round((fabs(curGMap -> xsize*sinTurnInvFlt) + fabs(curGMap -> ysize*cosTurnInvFlt))*.5);
	NETWORK_OUT_STREAM < (unsigned char)(scr_size < 255 ? scr_size : 255);
	Send();
	NETWORK_OUT_STREAM.end_body();

	NETWORK_OUT_STREAM.create_permanent_object(ShellNetID,0,0,0);
	ShellUpdate();
	NETWORK_OUT_STREAM.end_body();

	my_player_body.beebos = aiGetHotBug();
	my_player_body.world = CurrentWorld;
	my_player_body.NetID = NetID;
	my_player_body.BirthTime = GLOBAL_CLOCK();
	my_player_body.CarIndex = uvsPoint->Pmechos->type;
	my_player_body.color = uvsPoint->Pmechos->color;
	if(!pp && !pe && !ps){
		my_player_body.Data0 = R_curr.x;
		my_player_body.Data1 = R_curr.y;
	}else{
		if(pp) my_player_body.Data0 = pp->Poutput->gIndex;
		else my_player_body.Data0 = -1;

		if(pe) my_player_body.Data1 = pe->TabuTaskID;
		else{
			if(ps) my_player_body.Data1 = ps->TabuTaskID;
			else my_player_body.Data1 = -1;
		};
	};
	send_player_body(my_player_body);
};


void VangerUnit::NetCreateSlave(void)
{	
	DoorFlag = nDoorFlag = 0;	
//	Visibility = VISIBLE;
};

void VangerUnit::InitPlayerPoint(PlayerData* p)
{
	int i;
	uvsPoint->status = UVS_VANGER_STATUS::MOVEMENT;
	NetID = p->body.NetID;
	ShellNetID = (NetID & (~(63 << 16))) | NID_SHELL;
	pNetPlayer = p;
	p->CreatePlayerFlag = 2;
	for(i = 0;i < MAX_ACTIVE_SLOT;i++){
		if(p->SlotNetID[i]){
			GunSlotData[i].NetID = p->SlotNetID[i];
			GunSlotData[i].StuffNetID = p->SlotStuffNetID[i];
			GunSlotData[i].NetFireCount = p->SlotFireCount[i];
			if(Visibility == UNVISIBLE) GunSlotData[i].FireCount = GunSlotData[i].NetFireCount;
		};
	};
	Status |= SOBJ_WAIT_CONFIRMATION;
	AddFree();
	AddPhantomTarget(p);
};

void CheckPlayerList(void)
{
	PlayerData* p;
	VangerUnit* v;

	p = players_list.first();
	while(p){
		if(p->client_ID != GlobalStationID && p->name && p->status == GAMING_STATUS){
			if(p->CreatePlayerFlag == 3 && p->body.NetID == 0)
				p->CreatePlayerFlag = 0;
			if(p->body.world == CurrentWorld && p->body.NetID != 0 && p->body.CarIndex != 255 && p->CreatePlayerFlag == 0)
				NetEvent4Uvs(p);
			if(p->CreatePlayerFlag == 1){
				if(!((uvsVanger*)(p->uvsPoint))->Pescave && !((uvsVanger*)(p->uvsPoint))->Pspot && !((uvsVanger*)(p->uvsPoint))->Ppassage)
					v = addVanger((uvsVanger*)(p->uvsPoint),p->body.Data0,p->body.Data1);
				else 
					v = addVanger((uvsVanger*)(p->uvsPoint),((uvsVanger*)(p->uvsPoint))->Ppassage);
				if(v) v->InitPlayerPoint(p);
			};
		}else{
			if(ActD.Active) ActD.Active->pNetPlayer = p;
		};
		p = (PlayerData*)p -> next;
	};
};

void NetEvent4Uvs(PlayerData* p)
{		
	p->uvsPoint = (void*)(uvsCreateNetVanger(p->body.CarIndex,p->body.color,p->body.Data0,p->body.Data1));	
	p->CreatePlayerFlag = 1;	
};

void SetWorldBeebos(int n)
{
	aiPutHotBug(n);
};

void ObjectDestroy(GeneralObject* n,int mode)
{
	VangerUnit* p;
	int i,k;

	if(n->ID == ID_VANGER){
		if(mode){
			if(NetworkON && !(n->Status & SOBJ_ACTIVE) && ((VangerUnit*)(n))->pNetPlayer) ClearPhantomTarget(((VangerUnit*)(n))->pNetPlayer->client_ID);

			if(n == ActD.Active) ActD.Active = NULL;
			if(n == ActD.pfActive) ActD.pfActive = NULL;
			if(n == ActD.mfActive) ActD.mfActive = NULL;	

			if(actCurrentViewObject == n){
				actCurrentViewObject = (VangerUnit*)(actCurrentViewObject->NextTypeList);
				if(!actCurrentViewObject) actCurrentViewObject = (VangerUnit*)(ActD.Tail);
			};

			if(((VangerUnit*)(n))->aiActionID == AI_ACTION_BRAKE && ActD.Active && !(ActD.Active->Status & SOBJ_DISCONNECT)){
				aciClearTeleportMenu();
				i = TELEPORT_ESCAVE_ID + 1;
				p = (VangerUnit*)(ActD.Tail);
				while(p){
					if(p->uvsPoint->shape == UVS_VANGER_SHAPE::GAMER_SLAVE){
						aciAddTeleportMenuItem(uvsMechosType_to_AciInt(p->uvsPoint->Pmechos->type),i);
						p->VangerCloneID = i;
						i++;
					};
					p = (VangerUnit*)(p->NextTypeList);
				};
				if(CurrentWorld  < MAIN_WORLD_MAX - 1 && (i - TELEPORT_ESCAVE_ID - 1) <= 5) aciAddTeleportMenuItem(-1,TELEPORT_ESCAVE_ID);
			};
		};
		
		if(((VangerUnit*)(n))->VangerChanger){
			if(((VangerUnit*)(n))->VangerChanger->VangerChanger == n){
				((VangerUnit*)(n))->VangerChanger->VangerChanger = NULL;
				((VangerUnit*)(n))->VangerChanger->VangerChangerCount = 0;
				if(NetworkON && (((VangerUnit*)(n))->VangerChanger->Status & SOBJ_ACTIVE)){
					((VangerUnit*)(n))->VangerChanger->NetChanger = 0;
					((VangerUnit*)(n))->VangerChanger->ShellUpdateFlag = 1;
				};
			};
			((VangerUnit*)(n))->VangerChanger = NULL;
			((VangerUnit*)(n))->VangerChangerCount = 0;
		};

		for(k = 0;k < AI_EVENT_MAX;k++){
			for(i = 0;i < aiNumGlobalEvent[0][k];i++){
				if(aiGlobalEventData[0][k][i].Obj == n || aiGlobalEventData[0][k][i].Subj == (VangerUnit*)(n))
					aiGlobalEventData[0][k][i].Refresh = 0;
			};

			for(i = 0;i < aiNumGlobalEvent[1][k];i++){
				if(aiGlobalEventData[1][k][i].Obj == n || aiGlobalEventData[1][k][i].Subj == (VangerUnit*)(n))
					aiGlobalEventData[1][k][i].Refresh = 0;
			};
		};

		p = (VangerUnit*)(ActD.Tail);
		while(p){
			p->VangerDestroyHandler(n);
			p = (VangerUnit*)(p->NextTypeList);
		};		
	}else{
		if(NetworkON){
			if(((StuffObject*)(n))->Owner){
				if(((StuffObject*)(n))->Owner->Status & SOBJ_ACTIVE){
					NETWORK_OUT_STREAM.delete_object(((StuffObject*)(n))->NetDeviceID);
					NETWORK_OUT_STREAM < (uchar)(0);
					NETWORK_OUT_STREAM.end_body();
				};
			}else{
				NETWORK_OUT_STREAM.delete_object(n->NetID);
				NETWORK_OUT_STREAM < (uchar)(0);
				NETWORK_OUT_STREAM.end_body();
			};
		};

		for(k = 0;k < AI_EVENT_MAX;k++){
			for(i = 0;i < aiNumGlobalEvent[0][k];i++){
				if(aiGlobalEventData[0][k][i].Obj == n)
					aiGlobalEventData[0][k][i].Refresh = 0;
			};

			for(i = 0;i < aiNumGlobalEvent[1][k];i++){
				if(aiGlobalEventData[1][k][i].Obj == n)
					aiGlobalEventData[1][k][i].Refresh = 0;
			};
		};

		p = (VangerUnit*)(ActD.Tail);
		while(p){
			p->StuffDestroyHandler(n);
			p = (VangerUnit*)(p->NextTypeList);
		};
	};
};

int FuckOFFinWater(void)
{
	if(ActD.Active && (ActD.Active->dynamic_state & TOUCH_OF_WATER)) return 1;
	return 0;
};

int CHECK_BSIGN_INDEX[NUM_CHECK_BSIGN];
char* CHECK_BSIGN_DATA;/* = "ABCDEFKLMNOPR";*/
extern int uvsPoponkaID;

void CreateBoozeeniadaString(void)
{
	int i,t,i1,i2,s1,s2;
	char ch;

	CHECK_BSIGN_DATA = new char[NUM_CHECK_BSIGN + 1]; memset(CHECK_BSIGN_DATA, '\0', NUM_CHECK_BSIGN + 1);

	strcpy(CHECK_BSIGN_DATA, "ABCDEFKLMNOPR");
	for(i = 0;i < NUM_CHECK_BSIGN;i++)
		CHECK_BSIGN_INDEX[i] = i;

	for(i = 0;i < NUM_CHECK_BSIGN;i++){
		i1 = RND(NUM_CHECK_BSIGN);
		i2 = RND(NUM_CHECK_BSIGN);
		if(i1 != i2){
			t = CHECK_BSIGN_INDEX[i1];
			CHECK_BSIGN_INDEX[i1] = CHECK_BSIGN_INDEX[i2];
			CHECK_BSIGN_INDEX[i2] = t;
			ch = CHECK_BSIGN_DATA[i1];
			CHECK_BSIGN_DATA[i1] = CHECK_BSIGN_DATA[i2];
			CHECK_BSIGN_DATA[i2] = ch;
		};
	};

	i1 = 1;
	for(i = 0;i < NUM_CHECK_BSIGN;i++){
		if(CHECK_BSIGN_DATA[i] == 'A'){
			t = CHECK_BSIGN_INDEX[i];
			CHECK_BSIGN_INDEX[i] = CHECK_BSIGN_INDEX[i1];
			CHECK_BSIGN_INDEX[i1] = t;
			ch = CHECK_BSIGN_DATA[i];
			CHECK_BSIGN_DATA[i] = CHECK_BSIGN_DATA[i1];
			CHECK_BSIGN_DATA[i1] = ch;
			break;
		};
	};

	i1 = 3;
	for(i = 0;i < NUM_CHECK_BSIGN;i++){		
		if(CHECK_BSIGN_DATA[i] == 'E'){
			t = CHECK_BSIGN_INDEX[i];
			CHECK_BSIGN_INDEX[i] = CHECK_BSIGN_INDEX[i1];
			CHECK_BSIGN_INDEX[i1] = t;
			ch = CHECK_BSIGN_DATA[i];
			CHECK_BSIGN_DATA[i] = CHECK_BSIGN_DATA[i1];
			CHECK_BSIGN_DATA[i1] = ch;
			break;
		};
	};

	i1 = 5;
	for(i = 0;i < NUM_CHECK_BSIGN;i++){
		if(CHECK_BSIGN_DATA[i] == 'O'){
			t = CHECK_BSIGN_INDEX[i];
			CHECK_BSIGN_INDEX[i] = CHECK_BSIGN_INDEX[i1];
			CHECK_BSIGN_INDEX[i1] = t;
			ch = CHECK_BSIGN_DATA[i];
			CHECK_BSIGN_DATA[i] = CHECK_BSIGN_DATA[i1];
			CHECK_BSIGN_DATA[i1] = ch;
			break;
		};
	};

	CHECK_BSIGN_DATA[NUM_CONTROL_BSIGN] = '\0';

	for(i = 0;i < 8;i++){
		i1 = RND(2);
		i2 = RND(MAX_TIME_SECRET);
		s1 = RND(2);
		s2 = RND(MAX_TIME_SECRET);
		t = TimeSecretType[i1][i2];
		TimeSecretType[i1][i2] = TimeSecretType[s1][s2];
		TimeSecretType[s1][s2] = t;
	};

	for(i = 0;i < MAX_TIME_SECRET;i++){
		TimeSecretData0[0][i] = 0;
		if(UVS_ITEM_TYPE::POPONKA)
			TimeSecretData1[0][i] = uvsPoponkaID;
		else 
			TimeSecretData1[0][i] = 0;

		TimeSecretData0[1][i] = 0;
		if(UVS_ITEM_TYPE::POPONKA)
			TimeSecretData1[1][i] = uvsPoponkaID;
		else 
			TimeSecretData1[1][i] = 0;
	};
};

char* StringOfBoozeeniada(void)
{
	return CHECK_BSIGN_DATA;
};

extern listElem* DollyTail;

void VangerUnit::CalcAiFactor(VangerUnit* p,int time)
{
	int dx,dy,d1,d2;	
	int i,d,a;
	float dd;
	uvsDolly* dl;	

	if((time >= aiAttackDown  || aiAttackDown < 0) && (time <= aiAttackUp || aiAttackUp < 0))
		aiResolveFactor[AI_FACTOR_ATTACK] = aiFactorAttack;
	else 
		aiResolveFactor[AI_FACTOR_ATTACK] = 0;


	if((time >= aiRunDown || aiRunDown < 0) && (time <= aiRunUp || aiRunUp < 0))
		aiResolveFactor[AI_FACTOR_RUN] = aiFactorRun;
	else 
		aiResolveFactor[AI_FACTOR_RUN] = 0;


	if((time >= aiIgnoreDown || aiIgnoreDown < 0) && (time <= aiIgnoreUp || aiIgnoreUp < 0))
		aiResolveFactor[AI_FACTOR_IGNORE] = aiFactorIgnore;
	else
		aiResolveFactor[AI_FACTOR_IGNORE] = 0;


	if((time >= aiNoWayDown || aiNoWayDown < 0) && (time <= aiNoWayUp || aiNoWayUp < 0))
		aiResolveFactor[AI_FACTOR_NOWAY] = aiFactorNoWay;
	else 
		aiResolveFactor[AI_FACTOR_NOWAY] = 0;
	

#ifdef SAVE_AI_STATUS
	if(p->Status & SOBJ_ACTIVE) SaveAiStaus("InitFactor",this,time);
#endif

	if(aiAlarmTime){
		aiResolveFactor[AI_FACTOR_RUN] += aiAlarmRun;
		aiResolveFactor[AI_FACTOR_ATTACK] += aiAlarmAttack;
	};

#ifdef SAVE_AI_STATUS
	if(p->Status & SOBJ_ACTIVE) SaveAiStaus("AlarmFactor",this,time);
#endif


	if((PointStatus & TRK_BRANCH_MASK) && (p->PointStatus & TRK_BRANCH_MASK) && pBranch == p->pBranch){
		if(MoveDir){
			if(pNextLink < p->pNextLink){
				d1 = 1;
				d2 = 0;
			}else{
				d1 = 0;
				d2 = 1;
			};
		}else{
			if(pNextLink > p->pNextLink){
				d1 = 1;
				d2 = 0;
			}else{
				d1 = 0;
				d2 = 1;
			};
		};
	}else{
		dx = getDistX(R_curr.x,vPoint.x);
		dy = getDistY(R_curr.y,vPoint.y);
		d1 = dx*dx + dy*dy;
		dx = getDistX(p->R_curr.x,vPoint.x);
		dy = getDistY(p->R_curr.y,vPoint.y);
		d2 = dx*dx + dy* dy;
	};

	a = rPI(Angle - p->Angle);
	if(a > PI) a -= 2*PI;

	if(d1 < d2){
		if(a > abs(PI / 2)){
	   		aiResolveFactor[AI_FACTOR_RUN] += aiFrontOverRun;
			aiResolveFactor[AI_FACTOR_ATTACK] += aiFrontOverAttack;
			aiResolveFactor[AI_FACTOR_IGNORE] += aiFrontOverIgnore;
			aiResolveFactor[AI_FACTOR_NOWAY] += aiFrontOverNoWay;
		}else{
	   		aiResolveFactor[AI_FACTOR_RUN] += aiFrontUnderRun;
			aiResolveFactor[AI_FACTOR_ATTACK] += aiFrontUnderAttack;
			aiResolveFactor[AI_FACTOR_IGNORE] += aiFrontUnderIgnore;
			aiResolveFactor[AI_FACTOR_NOWAY] += aiFrontUnderNoWay;
		};
	}else{
		if(a > abs(PI / 2)){
			aiResolveFactor[AI_FACTOR_RUN] += aiBackOverRun;
			aiResolveFactor[AI_FACTOR_ATTACK] += aiBackOverAttack;
			aiResolveFactor[AI_FACTOR_IGNORE] += aiBackOverIgnore;
			aiResolveFactor[AI_FACTOR_NOWAY] += aiBackOverNoWay;
		}else{
			aiResolveFactor[AI_FACTOR_RUN] += aiBackUnderRun;
			aiResolveFactor[AI_FACTOR_ATTACK] += aiBackUnderAttack;
			aiResolveFactor[AI_FACTOR_IGNORE] += aiBackUnderIgnore;
			aiResolveFactor[AI_FACTOR_NOWAY] += aiBackUnderNoWay;
		};
	};

	if(NoWayDirect != -1) aiResolveFactor[AI_FACTOR_NOWAY] = -100.0;

#ifdef SAVE_AI_STATUS
	if(p->Status & SOBJ_ACTIVE) SaveAiStaus("NoWayFactor",this,time);
#endif

	if(LowAmmo){
   		aiResolveFactor[AI_FACTOR_RUN] += aiAmmoRun;
		aiResolveFactor[AI_FACTOR_ATTACK] += aiAmmoAttack;
	};

	if(LowArmor){
   		aiResolveFactor[AI_FACTOR_RUN] += aiArmorRun;
		aiResolveFactor[AI_FACTOR_ATTACK] += aiArmorAttack;
	};	

#ifdef SAVE_AI_STATUS
	if(p->Status & SOBJ_ACTIVE) SaveAiStaus("LowFactor",this,time);
#endif

	if(aiEvent.Num > 0) aiResolveFactor[AI_FACTOR_ATTACK] += aiMultyAttack*(aiEvent.Num - 1);
	if(aiResolveAttack.Num > 0) aiResolveFactor[AI_FACTOR_ATTACK] += aiTargetAttack*(aiResolveAttack.Num - 1);
	if(aiResolveFind.Num > 0) aiResolveFactor[AI_FACTOR_ATTACK] += aiTargetFind*(aiResolveFind.Num - 1);

#ifdef SAVE_AI_STATUS
	if(p->Status & SOBJ_ACTIVE) SaveAiStaus("MultyFactor",this,time);
#endif

	dl = (uvsDolly*)(DollyTail);
	while(dl){
		if(dl->biosNindex == aiModifier && dl->Pworld->gIndex == CurrentWorld){
			dx = getDistX(R_curr.x,dl->pos_x);
			dy = getDistY(R_curr.y,dl->pos_y);
			dd = (float)(dx*dx + dy*dy);
			if(dd < AI_MAX_DOLLY_FACTOR){
				if(uvsPoint->CurrentDolly() == dl ->gIndex) aiResolveFactor[AI_FACTOR_RUN] += dd*aiDollyRun / AI_MAX_DOLLY_FACTOR;
				else aiResolveFactor[AI_FACTOR_ATTACK] += dd*aiDollyAttack / AI_MAX_DOLLY_FACTOR;
			};
		};
		dl = (uvsDolly*)(dl->next);
	};

//	if(aiActionID == AI_ACTION_FARMER && SeedNum >= MaxSeed)
//		aiResolveFactor[AI_FACTOR_ATTACK] = -100;

	if(p->VangerRaceStatus != VANGER_RACE_NONE && (p->Status & SOBJ_ACTIVE)){
		aiResolveFactor[AI_FACTOR_ATTACK] += aiRacerAttack;
		if(CurrentWorld == WORLD_NECROSS && ActD.BigZek && p == ActD.BigZek->Owner)
			time = aiFindLevel;
	};

	if((p->Status & SOBJ_AUTOMAT)){
		if(RND(300) < 250){
			if(p->Visibility == VISIBLE){
				if(aiResolveFactor[AI_FACTOR_ATTACK] > 0) aiResolveFactor[AI_FACTOR_ATTACK] *= .5;
			}else{
				if(aiResolveFactor[AI_FACTOR_RUN] > 0) aiResolveFactor[AI_FACTOR_RUN] *= .5;
				if(aiResolveFactor[AI_FACTOR_ATTACK] > 0) aiResolveFactor[AI_FACTOR_ATTACK] *= .25;
			};
			if(aiResolveFactor[AI_FACTOR_NOWAY] > 0) aiResolveFactor[AI_FACTOR_NOWAY] *= .5;
		};
	}else{
		aiResolveFactor[AI_FACTOR_ATTACK] += aiDominanceAttack*(float)(aiCutDominance) / (float)(100.0);
		aiResolveFactor[AI_FACTOR_RUN] += aiDominanceRun*(float)(aiCutDominance) / (float)(100.0);
	};

#ifdef SAVE_AI_STATUS
	if(p->Status & SOBJ_ACTIVE) SaveAiStaus("DominanceFactor",this,time);
#endif

	if(!(Status & SOBJ_ACTIVE) && (p->Status & SOBJ_ACTIVE)) 
		aiResolveFactor[AI_FACTOR_ATTACK] += aiFuryLevel*(float)(GlobalFuryLevel);

#ifdef SAVE_AI_STATUS
	if(p->Status & SOBJ_ACTIVE) SaveAiStaus("FuryFactor",this,time);
#endif

	dd = aiResolveFactor[0];
	d = 0;
	for(i = 1;i < MAX_AI_FACTOR;i++){
		if(aiResolveFactor[i] > dd){
			d = i;
			dd = aiResolveFactor[i];
		};
	};

	switch(d){
		case AI_FACTOR_ATTACK:
			if(!AddAttackTime){
				if(time >= aiFindLevel)
					AddFindResolve(UNIT_ORDER_VANGER,UnitOrderType(p),round((float)(time)*aiAttackTime) + 10,AI_RESOLVE_STATUS_VAR)->Time = MaxWayCount;
				aiResolveAttack.AddResolve(UNIT_ORDER_VANGER,UnitOrderType(p),round((float)(time)*aiAttackTime) + 10,AI_RESOLVE_STATUS_VAR)->Time = MaxWayCount;
				AddAttackTime = round((float)(time)*aiAttackTime);
			};
			break;
		case AI_FACTOR_RUN:
			if(NoWayEnable == AI_NO_WAY_NONE && !(OtherFlag & MECHOS_CALC_WAY)){
				if(!LowArmor){
					aiResolveFind.ClearResolve();
					if(Status & SOBJ_ACTIVE) GamerOrderInit();
					else MainOrderInit();
				};
				CalcImpulse();
			};
			break;
		case AI_FACTOR_NOWAY:
			if(!(OtherFlag & MECHOS_CALC_WAY))
				NoWayInit();
			break;
	};
};

void aiFactorType::FactorLoad(Parser& in)
{
	in.search_name("Type");
	aiSection = in.get_int();	

	if(aiSection & 1){
		in.search_name("aiAttackUp");
		aiAttackUp = in.get_int();

		in.search_name("aiAttackDown");
		aiAttackDown = in.get_int();

		in.search_name("aiRunUp");
		aiRunUp = in.get_int();

		in.search_name("aiRunDown");
		aiRunDown = in.get_int();

		in.search_name("aiIgnoreUp");
		aiIgnoreUp = in.get_int();

		in.search_name("aiIgnoreDown");
		aiIgnoreDown = in.get_int();

		in.search_name("aiNoWayUp");
		aiNoWayUp = in.get_int();

		in.search_name("aiNoWayDown");
		aiNoWayDown = in.get_int();	


		in.search_name("aiFactorAttack");
		aiFactorAttack = (float)(in.get_double());

		in.search_name("aiFactorRun");
		aiFactorRun = (float)(in.get_double());

		in.search_name("aiFactorIgnore");
		aiFactorIgnore = (float)(in.get_double());

		in.search_name("aiFactorNoWay");
		aiFactorNoWay = (float)(in.get_double());


		in.search_name("aiRacerAttack");
		aiRacerAttack = (float)(in.get_double());


		in.search_name("aiAddAttack");
		aiAddAttack = in.get_int();

		in.search_name("aiAddView");
		aiAddView = in.get_int();

		in.search_name("aiAddFrontCollision");
		aiAddFrontCollision = in.get_int();

		in.search_name("aiAddBackCollision");
		aiAddBackCollision = in.get_int();

		in.search_name("aiAddSideCollision");
		aiAddSideCollision = in.get_int();

		in.search_name("aiAddAlarm");
		aiAddAlarm = in.get_int();



		in.search_name("aiMaxAttack");
		aiMaxAttack = in.get_int();

		in.search_name("aiMaxView");
		aiMaxView = in.get_int();

		in.search_name("aiMaxFrontCollision");
		aiMaxFrontCollision = in.get_int();

		in.search_name("aiMaxBackCollision");
		aiMaxBackCollision = in.get_int();

		in.search_name("aiMaxSideCollision");
		aiMaxSideCollision = in.get_int();		

		in.search_name("aiMaxAlarm");
		aiMaxAlarm = in.get_int();

		

		in.search_name("aiMainLevel");
		aiMainLevel = in.get_int();

		in.search_name("aiDeltaLevel");
		aiDeltaLevel = in.get_int();

		in.search_name("aiFindLevel");
		aiFindLevel = in.get_int();

		in.search_name("aiFuryLevel");
		aiFuryLevel = (float)(in.get_double());
	};

//----------------------------------------------------
	if(aiSection & 2){
		in.search_name("aiAlarmRun");
		aiAlarmRun = (float)(in.get_double());

		in.search_name("aiAlarmAttack");
		aiAlarmAttack = (float)(in.get_double());


		in.search_name("aiAmmoRun");
		aiAmmoRun = (float)(in.get_double());

		in.search_name("aiAmmoAttack");
		aiAmmoAttack = (float)(in.get_double());


		in.search_name("aiArmorRun");
		aiArmorRun = (float)(in.get_double());

		in.search_name("aiArmorAttack");
		aiArmorAttack = (float)(in.get_double());

	};

//------------------------------------------------------

	if(aiSection & 4){
		in.search_name("aiMultyAttack");
		aiMultyAttack = (float)(in.get_double());

		in.search_name("aiTargetAttack");
		aiTargetAttack = (float)(in.get_double());

		in.search_name("aiTargetFind");
		aiTargetFind = (float)(in.get_double());
	};
//------------------------------------------------------

	if(aiSection & 8){
		in.search_name("aiFrontOverAttack");
		aiFrontOverAttack = (float)(in.get_double());

		in.search_name("aiFrontOverRun");
		aiFrontOverRun = (float)(in.get_double());

		in.search_name("aiFrontOverIgnore");
		aiFrontOverIgnore = (float)(in.get_double());

		in.search_name("aiFrontOverNoWay");
		aiFrontOverNoWay = (float)(in.get_double());



		in.search_name("aiBackOverAttack");
		aiBackOverAttack = (float)(in.get_double());

		in.search_name("aiBackOverRun");
		aiBackOverRun = (float)(in.get_double());

		in.search_name("aiBackOverIgnore");
		aiBackOverIgnore = (float)(in.get_double());

		in.search_name("aiBackOverNoWay");
		aiBackOverNoWay = (float)(in.get_double());



		in.search_name("aiFrontUnderAttack");
		aiFrontUnderAttack = (float)(in.get_double());

		in.search_name("aiFrontUnderRun");
		aiFrontUnderRun = (float)(in.get_double());

		in.search_name("aiFrontUnderIgnore");
		aiFrontUnderIgnore = (float)(in.get_double());

		in.search_name("aiFrontUnderNoWay");
		aiFrontUnderNoWay = (float)(in.get_double());

		

		in.search_name("aiBackUnderAttack");
		aiBackUnderAttack = (float)(in.get_double());

		in.search_name("aiBackUnderRun");
		aiBackUnderRun = (float)(in.get_double());

		in.search_name("aiBackUnderIgnore");
		aiBackUnderIgnore = (float)(in.get_double());

		in.search_name("aiBackUnderNoWay");
		aiBackUnderNoWay = (float)(in.get_double());



		in.search_name("aiDollyAttack");
		aiDollyAttack = (float)(in.get_double());

		in.search_name("aiDollyRun");
		aiDollyRun = (float)(in.get_double());



		in.search_name("aiDominanceAttack");
		aiDominanceAttack = (float)(in.get_double());
			
		in.search_name("aiDominanceRun");
		aiDominanceRun = (float)(in.get_double());



 		in.search_name("aiAttackTime");
		aiAttackTime = (float)(in.get_double());
	};
};

void aiFactorType::FactorOpen(aiFactorType* in)
{
	if(in->aiSection & 1){
		aiAttackUp = in->aiAttackUp;
		aiAttackDown = in->aiAttackDown;								                                     
		aiRunUp = in->aiRunUp;
		aiRunDown = in->aiRunDown;
		aiIgnoreUp = in->aiIgnoreUp;
		aiIgnoreDown = in->aiIgnoreDown;
		aiNoWayUp = in->aiNoWayUp;
		aiNoWayDown = in->aiNoWayDown;
													     
		aiFactorAttack = in->aiFactorAttack;
		aiFactorRun = in->aiFactorRun;
		aiFactorIgnore = in->aiFactorIgnore;
		aiFactorNoWay = in->aiFactorNoWay;
													     
		aiAddAttack = in->aiAddAttack;          
		aiAddView = in->aiAddView;
		aiAddFrontCollision = in->aiAddFrontCollision;
		aiAddBackCollision = in->aiAddBackCollision;
		aiAddSideCollision = in->aiAddSideCollision;

		aiMaxAttack = in->aiMaxAttack;
		aiMaxView = in->aiMaxView;
		aiMaxFrontCollision = in->aiMaxFrontCollision;
		aiMaxBackCollision = in->aiMaxBackCollision;
		aiMaxSideCollision = in->aiMaxSideCollision;

		aiAddAlarm = in->aiAddAlarm;
		aiMaxAlarm = in->aiMaxAlarm;

		aiMainLevel = in->aiMainLevel;
		aiDeltaLevel = in->aiDeltaLevel;

		aiRacerAttack = in->aiRacerAttack;
		aiFindLevel = in->aiFindLevel;

		aiFuryLevel = in->aiFuryLevel;
	};

	if(in->aiSection & 2){
		aiAlarmRun = in->aiAlarmRun;
		aiAlarmAttack = in->aiAlarmAttack;

		aiAmmoRun = in->aiAmmoRun;
		aiAmmoAttack = in->aiAmmoAttack;

		aiArmorRun = in->aiArmorRun;
		aiArmorAttack = in->aiArmorAttack;

	};

	if(in->aiSection & 4){
		aiMultyAttack = in->aiMultyAttack;
		aiTargetAttack = in->aiTargetAttack;
		aiTargetFind = in->aiTargetFind;
	};

	if(in->aiSection & 8){
		aiFrontOverAttack = in->aiFrontOverAttack;
		aiFrontOverRun = in->aiFrontOverRun;
		aiFrontOverIgnore = in->aiFrontOverIgnore;
		aiFrontOverNoWay = in->aiFrontOverNoWay;
		aiBackOverAttack = in->aiBackOverAttack;
		aiBackOverRun = in->aiBackOverRun;
		aiBackOverIgnore = in->aiBackOverIgnore;
		aiBackOverNoWay = in->aiBackOverNoWay;
		aiFrontUnderAttack = in->aiFrontUnderAttack;
		aiFrontUnderRun	= in->aiFrontUnderRun;
		aiFrontUnderIgnore = in->aiFrontUnderIgnore;
		aiFrontUnderNoWay = in->aiFrontUnderNoWay;
		aiBackUnderAttack = in->aiBackUnderAttack;
		aiBackUnderRun = in->aiBackUnderRun;
		aiBackUnderIgnore = in->aiBackUnderIgnore;
		aiBackUnderNoWay = in->aiBackUnderNoWay;		

		aiDollyAttack = in->aiDollyAttack;
		aiDollyRun = in->aiDollyRun;

		aiDominanceAttack = in->aiDominanceAttack;			
		aiDominanceRun = in->aiDominanceRun;

		aiAttackTime = in->aiAttackTime;
	};
};

void addInfernalsKill2Protractor(void)
{
	ActD.ThreallDestroyActive = 1;
	aciActivateItemFunction(ACI_PROTRACTOR,ACI_PROTRACTOR_EVENT8);
};

void addSpectorsKill2Messiah(void)
{
	ActD.SpobsDestroyActive = 1;
	aciActivateItemFunction(ACI_MECHANIC_MESSIAH,ACI_MECH_MESSIAH_EVENT6);
};

int getSpobsState(void)
{
	if(ActD.SpobsDestroy == 0) return 1;
	return 0;
};

int getThreallState(void)
{
	 if(ActD.ThreallDestroy == 0) return 1;
	 return 0;
};

int isSpummyDeath(void)
{
	return ActD.SpummyRunner;	
};

void InsectUnit::HideAction(void)
{	
	dynamic_state = WHEELS_TOUCH;
	if(SpeedDir == 0) return;
	else{
		if(SpeedDir > 0){
			CurrSpeed = MaxHideSpeed;
			MoveAngle = rPI(MoveAngle - Angle);
		}else{
			MoveAngle = rPI(MoveAngle - Angle + PI);
			CurrSpeed = -MaxHideSpeed;
		};
	};

	if(MoveAngle > PI) MoveAngle -= 2*PI;

	if(MoveAngle > 0){
		if(MoveAngle > MECHOS_ROT_DELTA) MoveAngle = MECHOS_ROT_DELTA;
	}else{
		if(MoveAngle < -MECHOS_ROT_DELTA) MoveAngle = -MECHOS_ROT_DELTA;
	};

	Angle = rPI(Angle + MoveAngle);
	vDirect = Vector(CurrSpeed,0,0) * DBM(Angle,Z_AXIS);

	R_curr.x += vDirect.x;
	R_curr.y += vDirect.y;
	cycleTor(R_curr.x,R_curr.y);
};

int UnitItemMatrix::GetFullNum(void)
{
	int s,i;
	s = 1;
	if(NumID){
		for(i = 0;i < NumID;i++)
			s += DataID[i]->GetFullNum();
	};
	return s;	
};

void VangerUnit::NetEvent(int type,int id,int creator,int time,int x,int y,int radius_)
{
	switch(type){
		case UPDATE_OBJECT:
		case CREATE_OBJECT:
			Object::NetEvent(type,id,creator,time,x,y,radius_);
			Status &= ~SOBJ_WAIT_CONFIRMATION;
			break;
		case HIDE_OBJECT:
			Status |= SOBJ_WAIT_CONFIRMATION;
			break;
	};
};

void VangerUnit::ShellNetEvent(int type,int id,int creator,int time,int x,int y,int radius_)
{
//	int t;
	unsigned char ch;
	BulletObject* g;
	Vector vCheck;

	switch(type){
		case UPDATE_OBJECT:
		case CREATE_OBJECT:
			NETWORK_IN_STREAM > ch;
			NetFunction = ch;

			NETWORK_IN_STREAM > ch;
			Energy = ((MaxEnergy - 1) / 15) * (int)(ch & 0x0f);
			Armor = ((MaxArmor - 1) / 15) * (int)((ch  >> 4) & 0x0f);

			NETWORK_IN_STREAM > ch;
			if(ch != NetDestroyID && GlobalStationID == ch){
				NetStatisticUpdate(NET_STATISTICS_KILL);
				if(my_server_data.GameType == VAN_WAR){
					aiPutHotBug(aiGetHotBug() + uvsMechosTable[uvsPoint->Pmechos->type]->price);
//					ActD.HotBug += uvsMechosTable[uvsPoint->Pmechos->type]->price;
					my_player_body.beebos = aiGetHotBug();
				};
				send_player_body(my_player_body);
			};
			NetDestroyID = ch;

			NETWORK_IN_STREAM > ch;
			
			if(NetChanger != ch){
				if(!ch){
					VangerChanger = NULL;
					VangerChangerCount = 0;
				}else{
					if(ActD.Active && GlobalStationID == ch){
						VangerChanger = ActD.Active;
						MechosChangerType = ActD.Active->uvsPoint->Pmechos->type;
						VangerChangerCount = 3;
						vChangerPosition = ActD.Active->R_curr;
						VangerChangerAngle = ActD.Active->Angle;
						VangerChangerArmor = ActD.Active->Armor;
						VangerChangerEnergy = ActD.Active->Energy;

						if(!(ActD.Active->VangerChanger)){
							ActD.Active->ShellUpdateFlag = 1;
							ActD.Active->NetChanger = GET_STATION(NetID);
							ActD.Active->VangerChanger = this;
							ActD.Active->MechosChangerType = uvsPoint->Pmechos->type;
							ActD.Active->VangerChangerCount = 3;
							ActD.Active->vChangerPosition = R_curr;
							ActD.Active->VangerChangerAngle = Angle;
							ActD.Active->VangerChangerArmor = Armor;
							ActD.Active->VangerChangerEnergy = Energy;
						};
					};
				};
			};

			NetChanger = ch;
			NETWORK_IN_STREAM > NetRuffaGunTime;
			if(!(Status & SOBJ_WAIT_CONFIRMATION) && Visibility == VISIBLE && GetDistTime(NetGlobalTime,NetRuffaGunTime) < 5*256 && RuffaGunTime > (RUFFA_GUN_WAIT * WeaponWaitTime >> 8)){
				RuffaGunTime = 0;
				g = BulletD.CreateBullet();
				vCheck = Vector(64,0,0)*RotMat;
				g->CreateBullet(Vector(R_curr.x,R_curr.y,R_curr.z),
					Vector(XCYCL(vCheck.x + R_curr.x),YCYCL(vCheck.y + R_curr.y),R_curr.z + vCheck.z),NULL,&GameBulletData[WD_BULLET_RUFFA],this,Speed);
				if(ActD.Active)
					SOUND_MACHOTINE_SHOT(getDistX(ActD.Active->R_curr.x,R_curr.x));
			};
			NETWORK_IN_STREAM > NetProtractorFunctionTime;
			NETWORK_IN_STREAM > NetMessiahFunctionTime;
			NETWORK_IN_STREAM > NetFunction83Time;
			
			if(NetFunction83Time != PrevNetFunction83Time && GetDistTime(NetGlobalTime,NetFunction83Time) < 256*ROTOR_PROCESS_LIFE_TIME / 20){
				CreateParticleRotor(R_curr,83);
				ExternalDraw = 0;
				switch_analysis(1);
			};
			PrevNetFunction83Time = NetFunction83Time;
			break;
		case DELETE_OBJECT:
			Status |= SOBJ_DISCONNECT;
			break;
	};
};

void ActionDispatcher::ShellNetEvent(int type,int id)
{
	VangerUnit* p;

	p = (VangerUnit*)(ActD.Tail);
	while(p){
		if(p->ShellNetID == id){
			if(p->Status & (SOBJ_DISCONNECT | SOBJ_ACTIVE))	NETWORK_IN_STREAM.ignore_event();
			else{
				p->ShellNetEvent(type,id,NETWORK_IN_STREAM.current_creator(),NETWORK_IN_STREAM.current_time(),
					 NETWORK_IN_STREAM.current_x(),NETWORK_IN_STREAM.current_y(),NETWORK_IN_STREAM.current_radius());
			};
			return;
		};
		p = (VangerUnit*)(p->NextTypeList);
	};

	if(!p) NETWORK_IN_STREAM.ignore_event();
};

void ActionDispatcher::NetEvent(int type,int id)
{
	VangerUnit* p;

	p = (VangerUnit*)(GetNetObject(id));
	if(p){
		if(p->Status & (SOBJ_DISCONNECT | SOBJ_ACTIVE)) NETWORK_IN_STREAM.ignore_event();
		else{
			p->NetEvent(type,id,NETWORK_IN_STREAM.current_creator(),NETWORK_IN_STREAM.current_time(),
				 NETWORK_IN_STREAM.current_x(),NETWORK_IN_STREAM.current_y(),NETWORK_IN_STREAM.current_radius());
		};
	}else NETWORK_IN_STREAM.ignore_event();
};

int UnitItemMatrix::CheckSize(int sz,int*& p)
{
	int i;
	int* t;
	if(*p){
		p += FullNum;
		return 0;
	};	
	if(ID == sz){
		t = p;
		if(CheckFree(t))
			return 1;
		else
			return 0;
	};
	p++;
	for(i = 0;i < NumID;i++){		
		if(DataID[i]->CheckSize(sz,p))
			return 1;
	};
	return 0;
};

int UnitItemMatrix::CheckFree(int*& p)
{
	int i;
	if(*p) return 0;
	p++;
	for(i = 0;i < NumID;i++){
		if(!DataID[i]->CheckFree(p))
			return 0;
	};
	return 1;
};

int UnitItemMatrix::GetSize(int sz,int*& p)
{
	int i;
	if(ID == sz && (*p) == (sz + 1)) return 1;
	p++;
	for(i = 0;i < NumID;i++){		
		if(DataID[i]->GetSize(sz,p))
			return 1;
	};
	return 0;
};

int VangerUnit::CheckInMatrix(int sz)
{
	int* p;
	p = ItemMatrixData;
	if(ItemMatrix->CheckSize(sz,p)){
		*p = sz + 1;
		return 1;
	};
	return 0;
};

int VangerUnit::CheckOutMatrix(int sz)
{
	int* p;
	p = ItemMatrixData;
	if(ItemMatrix->GetSize(sz,p)){
		*p = 0;
		return 1;
	};
	return 0;
};

void VangerUnit::DischargeItem(StuffObject* p)
{
	switch(p->ActIntBuffer.type){
		case ACI_MACHOTINE_GUN_LIGHT:
		case ACI_MACHOTINE_GUN_HEAVY:
		case ACI_SPEETLE_SYSTEM_LIGHT:
		case ACI_SPEETLE_SYSTEM_HEAVY:
		case ACI_GHORB_GEAR_LIGHT:
		case ACI_GHORB_GEAR_HEAVY:
			if(NetworkON) p->ActIntBuffer.data1 = RND(p->ActIntBuffer.data1);
			else p->ActIntBuffer.data1 = RND(aiCutLuck) * p->ActIntBuffer.data1 / 100;
			CheckOutDevice(p);
			if(Status & SOBJ_ACTIVE){
				ActD.CheckDevice(p);
				aciRemoveItem(&(p->ActIntBuffer));
			};
			p->DeviceOut(R_curr + Vector(0,0,radius*4));
			break;
		case ACI_COPTE_RIG:
		case ACI_CUTTE_RIG:
		case ACI_CROT_RIG:
			if(NetworkON) p->ActIntBuffer.data1 = RND(p->ActIntBuffer.data1);
			else p->ActIntBuffer.data1 = RND(aiCutLuck) * p->ActIntBuffer.data1 / 100;			
			CheckOutDevice(p);
			if(Status & SOBJ_ACTIVE){
				ActD.CheckDevice(p);
				aciRemoveItem(&(p->ActIntBuffer));
			};
			p->ActIntBuffer.type = uvsSetItemType(p->uvsDeviceType,p->ActIntBuffer.data0,p->ActIntBuffer.data1);
			p->DeviceOut(R_curr + Vector(0,0,radius*4));
			break;
		case ACI_TERMINATOR:
		case ACI_TERMINATOR2:
		case ACI_SPEETLE_SYSTEM_AMMO0:
		case ACI_SPEETLE_SYSTEM_AMMO1:
		case ACI_CRUSTEST_CANNON_AMMO:
			if(p->ActIntBuffer.data1 > 1){
				if(NetworkON) p->ActIntBuffer.data1 = 1 + RND(p->ActIntBuffer.data1- 1);
				else p->ActIntBuffer.data1 = 1 + RND(aiCutLuck) * (p->ActIntBuffer.data1 - 1) / 100;
				CheckOutDevice(p);
				if(Status & SOBJ_ACTIVE){
					ActD.CheckDevice(p);
					aciRemoveItem(&(p->ActIntBuffer));
				};
				p->DeviceOut(R_curr + Vector(0,0,radius*4));
			}else{
				ObjectDestroy(p);
				CheckOutDevice(p);
				if(Status & SOBJ_ACTIVE){
					ActD.CheckDevice(p);
					aciRemoveItem(&(p->ActIntBuffer));
				};
				p->Storage->Deactive(p);
				DelDevice(p);
			};
			break;
		case ACI_SPUMMY:
			if(!(Status & SOBJ_ACTIVE)){			
				CheckOutDevice(p);
				p->DeviceOut(R_curr + Vector(0,0,radius*4));
			};
			break;
		case ACI_RUBOX:
			ObjectDestroy(p);
			CheckOutDevice(p);
			if(Status & SOBJ_ACTIVE){
				ActD.CheckDevice(p);
//				aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
				aciRemoveItem(&(p->ActIntBuffer));
			};
			p->Storage->Deactive(p);
			DelDevice(p);
			break;
		default:
			CheckOutDevice(p);
			if(Status & SOBJ_ACTIVE){
				ActD.CheckDevice(p);
//				aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
				aciRemoveItem(&(p->ActIntBuffer));
			};
			p->DeviceOut(R_curr + Vector(0,0,radius*4));
			break;
	};
};

void VangerUnit::DestroyItem(StuffObject* p)
{
	switch(p->ActIntBuffer.type){
		case ACI_CIRTAINER:
		case ACI_EMPTY_CIRTAINER:
		case ACI_CONLARVER:
		case ACI_EMPTY_CONLARVER:
		case ACI_ELEECH:
		case ACI_KERNOBOO:
		case ACI_PIPETKA:
		case ACI_WEEZYK:
		case ACI_PROTRACTOR:
		case ACI_MECHANIC_MESSIAH:
		case ACI_FUNCTION83:		
		case ACI_BOOT_SECTOR:
		case ACI_PEELOT:
		case ACI_LEEPURINGA:
		case ACI_SANDOLL_PART1:
		case ACI_SANDOLL_PART2:
		case ACI_LAST_MOGGY_PART1:
		case ACI_LAST_MOGGY_PART2:
		case ACI_QUEEN_FROG_PART1:
		case ACI_QUEEN_FROG_PART2:
		case ACI_LAWN_MOWER_PART1:
		case ACI_LAWN_MOWER_PART2:
		case ACI_WORMASTER_PART1:
		case ACI_WORMASTER_PART2:
		case ACI_TABUTASK:
		case ACI_ELEEPOD:
		case ACI_BEEBOORAT:
		case ACI_ZEEX:
		case ACI_PALOCHKA:
		case ACI_PIPKA:
		case ACI_NOBOOL:
		case ACI_BOORAWCHIK:
		case ACI_PEREPONKA:
		case ACI_ZEEFICK:
		case ACI_TABUTASK_FAILED:
		case ACI_TABUTASK_SUCCESSFUL:
			CheckOutDevice(p);
			if(Status & SOBJ_ACTIVE){
				ActD.CheckDevice(p);
//				aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
				aciRemoveItem(&(p->ActIntBuffer));
			};
			p->DeviceOut(R_curr + Vector(0,0,radius*4));
			break;
		case ACI_RUBOX:
			ObjectDestroy(p);
			CheckOutDevice(p);
			if(Status & SOBJ_ACTIVE){
				ActD.CheckDevice(p);
//				aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
				aciRemoveItem(&(p->ActIntBuffer));
			};
			p->Storage->Deactive(p);
			DelDevice(p);
			break;
		case ACI_SPUMMY:
			if(!(Status & SOBJ_ACTIVE)){
				CheckOutDevice(p);
				p->DeviceOut(R_curr + Vector(0,0,radius*4));
			};
			break;
		default:
			if(NetworkON){
				if((int)(RND(ItemD.Total)) < ItemD.Num){
					ObjectDestroy(p);
					CheckOutDevice(p);
					if(Status & SOBJ_ACTIVE){
						ActD.CheckDevice(p);
	//					aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
						aciRemoveItem(&(p->ActIntBuffer));
					};
					p->Storage->Deactive(p);
					DelDevice(p);
				}else DischargeItem(p);
			}else{
				if(RND(aiCutLuck) < 30){
					ObjectDestroy(p);
					CheckOutDevice(p);
					if(Status & SOBJ_ACTIVE){
						ActD.CheckDevice(p);
	//					aciSendEvent2actint(ACI_DROP_ITEM,&(p->ActIntBuffer));
						aciRemoveItem(&(p->ActIntBuffer));
					};
					p->Storage->Deactive(p);
					DelDevice(p);
				}else DischargeItem(p);
			};
			break;
	};
};

void FakeOfMight(void)
{
	if(uvsKronActive && ActD.Active){
		ActD.Active->Armor = ActD.Active->MaxArmor;
		ActD.Active->Energy = ActD.Active->MaxEnergy;
	};
};

void XpeditionOFF(int type)
{
	if(uvsKronActive && ActD.Active){
		ActD.Active->ExternalMode = EXTERNAL_MODE_EARTH_IN;
		aciSendEvent2actint(ACI_LOCK_INTERFACE,NULL);	
		ActD.Active->ExternalTime = ROTOR_PROCESS_LIFE_TIME;
		ActD.Active->CreateParticleRotor(ActD.Active->R_curr,ActD.Active->radius);		
		camera_direct(ActD.Active->R_curr.x,ActD.Active->R_curr.y,1 << 7,0,0,ROTOR_PROCESS_LIFE_TIME + 1);
		PalCD.Set(CPAL_HIDE_PASSAGE,ROTOR_PROCESS_LIFE_TIME);			
		ActD.Active->ExternalLock = 1;
		ActD.Active->ExternalDraw = 0;
		ActD.Active->ExternalObject = NULL;
		ActD.Active->switch_analysis(1);
		StopCDTRACK();

		switch(type){
			case GAME_OVER_INFERNAL_LOCKED:
				ActD.ThreallDestroy = 1;
				ActD.SpobsDestroy = 0;
				break;
			case GAME_OVER_SPOBS_LOCKED:
				ActD.ThreallDestroy = 0;
				ActD.SpobsDestroy = 1;
				break;
			case GAME_OVER_ALL_LOCKED:
				ActD.ThreallDestroy = 1;
				ActD.SpobsDestroy = 1;
				break;
			case GAME_OVER_LUCKY:
				ActD.LuckyFunction = 1;
				ActD.ThreallDestroy = 0;
				ActD.SpobsDestroy = 0;
				break;
		};
	};
};

const int NETWORK_NUM_ESCAVE = 7;
const char* NetworkEscaveName[NETWORK_NUM_ESCAVE] ={"Podish","Incubator","VigBoo","Lampasso","Ogorod","ZeePa","B-Zone"};

void NetworkGetStart(char* name,int& x,int& y)
{
	int i,j,t;
	SensorSortedData = new SensorDataType*[SnsTableSize];
	StaticSort(SnsTableSize,(StaticObject**)SensorObjectData,(StaticObject**)SensorSortedData);

	for(i = 0;i < NETWORK_NUM_ESCAVE;i++){
		if(!strcmp(name,NetworkEscaveName[i])){
			t = -1;
			for(j = ActD.NetEnterOffset + 1;j < SnsTableSize;j++){
				if(SensorObjectData[j]->SensorType == SensorTypeList::PLAYER_BIRTH && SensorObjectData[j]->data5 == i){
					t = j;
					break;
				};
			};

			if(t == -1){
				for(j =0;j < SnsTableSize;j++){
					if(SensorObjectData[j]->SensorType == SensorTypeList::PLAYER_BIRTH && SensorObjectData[j]->data5 == i){
						ActD.NetEnterOffset = j;
						x = SensorObjectData[j]->R_curr.x;
						y = SensorObjectData[j]->R_curr.y;
						return;
					};
				};
			}else{
				ActD.NetEnterOffset = t;
				x = SensorObjectData[t]->R_curr.x;
				y = SensorObjectData[t]->R_curr.y;
				return;
			};
		};
	};
//	ErrH.Abort("Escave Name Not FOUND!");
};

void VangerUnit::OutCarNator(void)
{
	StuffObject* p;
	StuffObject* pp;
	uvsItem* g;			
	
	if(NetworkON){
		FreeList(uvsPoint->Pitem);
		p = VangerChanger->DeviceData;
		while(p){
			if(p->ActIntBuffer.type == ACI_MECHOSCOPE)
				p->ActIntBuffer.data1 = 0;
			
			g = new uvsItem(p->uvsDeviceType);
			g->param1 = p->ActIntBuffer.data0;
			g->param2 = p->ActIntBuffer.data1;
			uvsPoint->addItem(g,1);	
			p = p->NextDeviceList;
		};
	}else{
		FreeList(VangerChanger->uvsPoint->Pitem);
		if(Status & SOBJ_ACTIVE) aciDropMoveItem();
		p = DeviceData;
		while(p){
			pp = p->NextDeviceList;
			if(p->ActIntBuffer.type == ACI_MECHOSCOPE)
				p->ActIntBuffer.data1 = 0;
			ObjectDestroy(p);
			CheckOutDevice(p);
			g = new uvsItem(p->uvsDeviceType);
			if(Status & SOBJ_ACTIVE){
				aciGetItemCoords(&(p->ActIntBuffer),g->pos_x,g->pos_y);
				ActD.CheckDevice(p);
				aciRemoveItem(&(p->ActIntBuffer));
			};
			g->param1 = p->ActIntBuffer.data0;
			g->param2 = p->ActIntBuffer.data1;
			VangerChanger->uvsPoint->addItem(g,1);

			p->Storage->Deactive(p);
			DelDevice(p);
			p = pp;
		};
	};
};

//TODO stalkerg скорее всего тут глупость
void VangerUnit::InCarNator(void)
{
	StuffObject* p;
	listElem* n;

	n = uvsPoint->Pitem;
	while(n){
		addDevice(((uvsItem*)(n))->pos_x,((uvsItem*)(n))->pos_y,0,((uvsItem*)(n))->type,((uvsItem*)(n))->param1,((uvsItem*)(n))->param2,this);
		n = n->next;
	};

	if(Status & SOBJ_ACTIVE){
		p = DeviceData;
		while(p){
			p->ActIntBuffer.slot = -1;
			aciSendEvent2actint(ACI_PUT_ITEM,&(p->ActIntBuffer));
			p = p->NextDeviceList;
		};
	};
	FreeList(uvsPoint->Pitem);
};


//TODO stalkerg: тут явно поломано
void aciPrepareMenus(void);
void VangerUnit::ChangeVangerProcess(void)
{
	uvsMechosType* sc;
	int i;
	StuffObject* p;
	StuffObject* pp;

	if(NetworkON && (Status & SOBJ_ACTIVE)){
		aciDropMoveItem();
		p = DeviceData;
		while(p){
			pp = p->NextDeviceList;
			if(p->ActIntBuffer.type == ACI_MECHOSCOPE)
				p->ActIntBuffer.data1 = 0;

			ObjectDestroy(p);
			CheckOutDevice(p);			
			if(Status & SOBJ_ACTIVE){
				ActD.CheckDevice(p);
				aciRemoveItem(&(p->ActIntBuffer));
			};
			p->Storage->Deactive(p);
			DelDevice(p);
			p = pp;
		};
	};

	for(i = 0;i < ItemMatrix->NumSlot;i++)
		GunSlotData[ItemMatrix->nSlot[i]].CloseSlot();

	aiEvent.Free();
	aiResolveFind.Free();
	aiResolveAttack.Free();

	delete ItemMatrixData;

//!!!!	

	if(!NetworkON){
		uvsPoint->Pmechos->type = MechosChangerType;
		uvsPoint->Pmechos->color = VangerChangerColor;
	}else{
		if(Status & SOBJ_ACTIVE){
			uvsPoint->Pmechos->type = MechosChangerType;
			uvsPoint->Pmechos->color = my_player_body.color;
		}else{
			uvsPoint->Pmechos->type = pNetPlayer->body.CarIndex;
			uvsPoint->Pmechos->color = pNetPlayer->body.color;
		};
	};

	sc = uvsMechosTable[uvsPoint->Pmechos->type];
	uvsMaxSpeed = sc->MaxSpeed;
	MaxArmor = sc->MaxArmor << 16;
	MaxEnergy = sc->MaxEnergy << 16;
	dEnergy = ((MaxEnergy / 10) * sc->DeltaEnergy) / (UnitGlobalTime * 100);
	DropEnergy = MaxEnergy * sc->DropEnergy / 100;
	DelayDrop = sc->DropTime;
	ImpulsePower = (MaxEnergy / 100) * sc->MaxFly;
	OxigenResource = MaxOxigenResource = sc->MaxOxigen;
	PowerFlag = sc->MaxFire;
	aiPowerFlag = VANGER_POWER_NONE;
	MaxPassageCount = sc->MaxTeleport;
	DestroyClass = sc->MaxDamage;

	PassageCount = 0;
	uvsPoint->Pmechos->teleport &= ~0xff;
	uvsPoint->Pmechos->teleport |= PassageCount;

//!!!!!!!!!
	if(!NetworkON || (Status & SOBJ_ACTIVE)){
		Energy = VangerChangerEnergy;
		Armor = VangerChangerArmor;
	};

	switch(uvsPoint->Pmechos->type){
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
			ItemMatrix = &UnitMatrixData[UNIT_SUB_MATRIX_NUM + uvsPoint->Pmechos->type];
			break;
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
			ItemMatrix = &UnitMatrixData[23];
			break;
		case 19://Sandoll
			ItemMatrix = &UnitMatrixData[25];
			break;
		case 20://BTR
			ItemMatrix = &UnitMatrixData[26];
			break;
		case 21://Amphyb
			ItemMatrix = &UnitMatrixData[24];
			break;
		case 22://Helicopt
			ItemMatrix = &UnitMatrixData[27];
			break;
		case 23://Worm
			ItemMatrix = &UnitMatrixData[28];
			break;
	};

	if(Status & SOBJ_ACTIVE){
		aciNewMechos(uvsMechosType_to_AciInt(uvsPoint->Pmechos->type));
		aciSetRedraw();
	};

//!!!!!!!!
	if(!NetworkON || (Status & SOBJ_ACTIVE)){
		R_curr = vChangerPosition;
		Angle = VangerChangerAngle;
	};

	nModel = ModelD.FindModel(uvsMechosTable[uvsPoint->Pmechos->type]->name);
	Object::operator = (ModelD.ActiveModel(nModel));
	if(Status & SOBJ_ACTIVE) set_active(1);
	else set_active(0);

	cycleTor(R_curr.x,R_curr.y);
	set_3D(SET_3D_DIRECT_PLACE,R_curr.x,R_curr.y,R_curr.z,0,-Angle,0);
	ActionUnit::InitEnvironment();
	MovMat = RotMat = DBM(Angle,Z_AXIS);	
	vUp = Vector(ymax_real,0,0)*MovMat;
	vDown = -vUp;
	vDirect =  Vector(0,0,0);

	WayDist = WayDistData;
	CalcWayDist = WayDistData + HideTrack.NumNode;
	memset(WayDistData,0,sizeof(int)*HideTrack.NumNode*2);
	TargetAngle = TargetLen = 0;
	EnvirReaction = 128;
	TargetReaction = TOTAL_REACTION - EnvirReaction;
	MoveDir = 1;
	PointStatus = TRK_IN_BRANCH;
	pNextLink = NULL;
	pBranch = NULL;
	HideTrack.GetFirstPosition(R_curr.x,R_curr.y,pNextLink,pBranch);
	if(pNextLink == pBranch->Link){
		pPrevLink = pNextLink;
		pNextLink++;
	}else pPrevLink = pNextLink - 1;
	if(pBranch->pBeg) pNode = pBranch->pBeg;
	else pNode = pBranch->pEnd;
	FrontDir = -1;
	FrontPoint.PointStatus = TRK_IN_BRANCH;
	FrontPoint.pNextLink = pNextLink;
	FrontPoint.pPrevLink = pPrevLink;
	FrontPoint.pBranch = pBranch;
	FrontPoint.pNode = pNode;
	PrevBranch = pBranch;
	OtherFlag = MECHOS_RECALC_FRONT | MECHOS_TARGET_MOVE;
	CheckWayCount = MaxWayCount = 50;
	CheckPosition = MaxCheckPosition = 15;
	vTarget = Vector(0,0,0);
	for(i =0;i < n_wheels;i++){
		PrevWheelX[i] = 0;
		PrevWheelY[i] = 0;
		PrevWheelFlag[i] = 0;
	};
	NullTime = 0;
	vEnvir = Vector(0,0,0);
	vDirect = Vector(0,0,0);	
	aiMoveMode = AI_MOVE_TRACK;
	aiMoveFunction = AI_MOVE_FUNCTION_WHEEL;
	gRnd = 0xFFFFFFFF;

	ShellUpdateFlag = 1;
	switch(uvsPoint->Pmechos->color){
		case 0:
			set_body_color(COLORS_IDS::BODY_GREEN);
			aiModifier = AI_MODIFIER_ELEEPOD;
			break;
		case 1:
			set_body_color(COLORS_IDS::BODY_RED);
			aiModifier = AI_MODIFIER_BEEBOORAT;
			break;
		case 2:
			set_body_color(COLORS_IDS::BODY_BLUE);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 3:
			set_body_color(COLORS_IDS::BODY_YELLOW);
			aiModifier = AI_MODIFIER_ZEEX;
			break;

		case 4:
			set_body_color(COLORS_IDS::BODY_CRIMSON);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 5:
			set_body_color(COLORS_IDS::BODY_GRAY);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 6:
			set_body_color(COLORS_IDS::ROTTEN_ITEM);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 7:
			set_body_color(COLORS_IDS::MATERIAL_3);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 8:
			set_body_color(COLORS_IDS::MATERIAL_1);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
		case 9:
			set_body_color(COLORS_IDS::MATERIAL_0);
			aiModifier = AI_MODIFIER_ZEEX;
			break;
	};

	DoorFlag = 0;
	ExternalMode = EXTERNAL_MODE_NORMAL;
	ExternalTime = 0;
	ExternalLock = 0;
	ExternalDraw = 1;
	ExternalObject = ExternalLastSensor = ExternalSensor = NULL;
	ExternalTime2 = 0;
	ExternalAngle = 0;
	
	ItemMatrixData = new int[ItemMatrix->FullNum];
	for(i = 0;i < ItemMatrix->FullNum;i++) ItemMatrixData[i] = 0;

	for(i = 0;i < MAX_ACTIVE_SLOT;i++){
		GunSlotData[i].pData = NULL;
		GunSlotData[i].Owner = NULL;
	};

	for(i = 0;i < ItemMatrix->NumSlot;i++)
		GunSlotData[ItemMatrix->nSlot[i]].OpenSlot(ItemMatrix->nSlot[i],this,ItemMatrix->nSlot[i]);

	aiEvent.Init();
	aiResolveFind.Init();
	aiResolveAttack.Init();	

//!!!!!!!!!
	if(NetworkON){
		if(Status & SOBJ_ACTIVE){
			aciPrepareMenus();
			InCarNator();
			my_player_body.CarIndex = uvsPoint->Pmechos->type;
			send_player_body(my_player_body);
		};
	}else{
		if(Status & SOBJ_ACTIVE) aciPrepareMenus();
		InCarNator();
	};

	TouchKeyObject = NULL;
	TouchKeyObjectFlag = 1;

	CoptePoint = NULL;
	SwimPoint = NULL;
	MolePoint = NULL;
	
	if(Status & SOBJ_ACTIVE){
		aciUpdateCurCredits(aiGetHotBug());
//		aciCurCredits = ActD.HotBug;
		aciMaxEnergy = (MaxEnergy >> 16) + 1;
		aciMaxArmor = (MaxArmor	>> 16) + 1;
		aciCurEnergy = Energy >> 16;
		aciCurArmor = Armor >> 16;
		aciCurJumpCount = PassageCount;
		aciMaxJumpCount = MaxPassageCount + 1;
	};

	InitAI();
	RuffaGunTime = 0;
	SensorEnable = 0;
	VangerRaceStatus = VANGER_RACE_NONE;
	PlayerDestroyFlag = 0;	
	NetworkArmor = Armor;
	NetworkEnergy = Energy;
	NetRuffaGunTime = 0;
	PrevImpuseFrame = 0;
};

extern aciPromptData aiMessageBuffer;
void NetCheckRemovePlayer(PlayerData* p)
{
	VangerUnit* v;
	if(p->uvsPoint){
		v = (VangerUnit*)(ActD.Tail);
		while(v){
			if(v->pNetPlayer == p){
				ClearPhantomTarget(p->client_ID);
				v->pNetPlayer = NULL;
			};			
			v = (VangerUnit*)(v->NextTypeList);
		};
	};

	if(uvsKronActive && ActD.Active && p->name){
		RaceTxtBuff.init();		

		switch(my_server_data.GameType){
			case MECHOSOMA:
				if((p->body.MechosomaStat.ItemCount1 + p->body.MechosomaStat.ItemCount2) >= (my_server_data.Mechosoma.ProductQuantity1 + my_server_data.Mechosoma.ProductQuantity2)){
					RaceTxtBuff < p->name;
					if(lang() == RUSSIAN) RaceTxtBuff < rPlayerWinnerSecondMessage;
					else RaceTxtBuff < PlayerWinnerSecondMessage;
					aiMessageBuffer.align_type = 0;
					aiMessageBuffer.TimeBuf[0] = 200;
					aiMessageBuffer.ColBuf[0] = 143;
					aiMessageBuffer.TimeBuf[1] = 200;
					aiMessageBuffer.ColBuf[1] = 143;
					aiMessageBuffer.TimeBuf[2] = 200;
					aiMessageBuffer.ColBuf[2] = 143;
					aiMessageBuffer.NumStr = 3;
					if(lang() == RUSSIAN){
						aiMessageBuffer.add_str(0,(uchar*)(rPlayerWinnerFirstMessage));
						aiMessageBuffer.add_str(2,(uchar*)(rPlayerWinnerOtherMessage));
					}else{
						aiMessageBuffer.add_str(0,(uchar*)(PlayerWinnerFirstMessage));
						aiMessageBuffer.add_str(2,(uchar*)(PlayerWinnerOtherMessage));
					};
					aiMessageBuffer.add_str(1,(uchar*)(RaceTxtBuff.GetBuf()));
					aciSendPrompt(&aiMessageBuffer);
				}else{
					RaceTxtBuff < p->name;
					if(lang() == RUSSIAN) RaceTxtBuff < rPlayerFirstDropedMessage;
					else RaceTxtBuff < PlayerFirstDropedMessage;
					aiMessageBuffer.align_type = 0;
					aiMessageBuffer.TimeBuf[0] = 200;
					aiMessageBuffer.ColBuf[0] = 143;
					aiMessageBuffer.TimeBuf[1] = 200;
					aiMessageBuffer.ColBuf[1] = 143;
					aiMessageBuffer.NumStr = 2;
					aiMessageBuffer.add_str(0,(uchar*)(RaceTxtBuff.GetBuf()));
					if(lang() == RUSSIAN) aiMessageBuffer.add_str(1,(uchar*)(rPlayerSecondDropedMessage));
					else aiMessageBuffer.add_str(1,(uchar*)(PlayerSecondDropedMessage));
					aciSendPrompt(&aiMessageBuffer);
				};
				break;
			case PASSEMBLOSS:
				if(p->body.PassemblossStat.CheckpointLighting >= my_server_data.Passembloss.CheckpointsNumber){
					RaceTxtBuff < p->name;
					if(lang() == RUSSIAN) RaceTxtBuff < rPlayerWinnerSecondMessage;
					else RaceTxtBuff < PlayerWinnerSecondMessage;
					aiMessageBuffer.align_type = 0;
					aiMessageBuffer.TimeBuf[0] = 200;
					aiMessageBuffer.ColBuf[0] = 143;
					aiMessageBuffer.TimeBuf[1] = 200;
					aiMessageBuffer.ColBuf[1] = 143;
					aiMessageBuffer.TimeBuf[2] = 200;
					aiMessageBuffer.ColBuf[2] = 143;
					aiMessageBuffer.NumStr = 3;					
					if(lang() == RUSSIAN){
						aiMessageBuffer.add_str(0,(uchar*)(rPlayerWinnerFirstMessage));
						aiMessageBuffer.add_str(2,(uchar*)(rPlayerWinnerOtherMessage));
					}else{
						aiMessageBuffer.add_str(0,(uchar*)(PlayerWinnerFirstMessage));
						aiMessageBuffer.add_str(2,(uchar*)(PlayerWinnerOtherMessage));
					};					
					aiMessageBuffer.add_str(1,(uchar*)(RaceTxtBuff.GetBuf()));					
					aciSendPrompt(&aiMessageBuffer);
				}else{
					RaceTxtBuff < p->name;
					if(lang() == RUSSIAN) RaceTxtBuff < rPlayerFirstDropedMessage;
					else RaceTxtBuff < PlayerFirstDropedMessage;
					aiMessageBuffer.align_type = 0;
					aiMessageBuffer.TimeBuf[0] = 200;
					aiMessageBuffer.ColBuf[0] = 143;
					aiMessageBuffer.TimeBuf[1] = 200;
					aiMessageBuffer.ColBuf[1] = 143;
					aiMessageBuffer.NumStr = 2;
					aiMessageBuffer.add_str(0,(uchar*)(RaceTxtBuff.GetBuf()));
					if(lang() == RUSSIAN) aiMessageBuffer.add_str(1,(uchar*)(rPlayerSecondDropedMessage));
					else aiMessageBuffer.add_str(1,(uchar*)(PlayerSecondDropedMessage));
					aciSendPrompt(&aiMessageBuffer);
				};
				break;
			case VAN_WAR:
				RaceTxtBuff < p->name;
				if(lang() == RUSSIAN) RaceTxtBuff < rPlayerFirstDropedMessage;
				else RaceTxtBuff < PlayerFirstDropedMessage;
				aiMessageBuffer.align_type = 0;
				aiMessageBuffer.TimeBuf[0] = 200;
				aiMessageBuffer.ColBuf[0] = 143;
				aiMessageBuffer.TimeBuf[1] = 200;
				aiMessageBuffer.ColBuf[1] = 143;
				aiMessageBuffer.NumStr = 2;
				aiMessageBuffer.add_str(0,(uchar*)(RaceTxtBuff.GetBuf()));
				if(lang() == RUSSIAN) aiMessageBuffer.add_str(1,(uchar*)(rPlayerSecondDropedMessage));
				else aiMessageBuffer.add_str(1,(uchar*)(PlayerSecondDropedMessage));
				aciSendPrompt(&aiMessageBuffer);
				break;
		};
	};
};

void VangerUnit::ClearItemList(void)
{
	StuffObject* p;
	StuffObject* pp;	

	p = DeviceData;

	while(p){
		pp = p->NextDeviceList;
		p->Storage->Deactive(p);
		DelDevice(p);
		p = pp;
	};
};


#ifdef SAVE_AI_STATUS
void SaveAiStaus(char* p,VangerUnit* v,int time)
{
	if(v->Visibility == VISIBLE){
		AiStatusLog < "\nFrame " <= frame < ",Armor " <= (v->Armor >> 16);
		AiStatusLog < "\n:" <= (unsigned int)(v);
		AiStatusLog < "\n" < p < "\n";
		AiStatusLog < "\nAttack " <=  v->aiResolveFactor[AI_FACTOR_ATTACK];
		AiStatusLog < "\nRun " <=  v->aiResolveFactor[AI_FACTOR_RUN];
		AiStatusLog < "\nNoWay " <=  v->aiResolveFactor[AI_FACTOR_NOWAY];
		AiStatusLog < "\nIgnore " <=  v->aiResolveFactor[AI_FACTOR_IGNORE];
		AiStatusLog < "\nTime " <= time;
		AiStatusLog < "\n";
	};
};
#endif

void GetNetworkGameTime(int& day,int& hour,int& min,int& sec)
{
	int t;
	t = age_of_current_game();
	sec = t  %  60;
	min = (t / 60) % 60;
	hour = (t / (60*60)) % 12;
	day = (t / (60*60*12)) % 30;
};
