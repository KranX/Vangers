#include "../global.h"

#include "../3d/3d_math.h"
#include "../3d/3dgraph.h"
#include "../3d/3dobject.h"
#include "../3d/parser.h"

#include "../common.h"
#include "../sqexp.h"
#include "../backg.h"

#include "../actint/item_api.h"
#include "uvsapi.h"
#include "../network.h"

#include "../terra/vmap.h"
#include "../terra/world.h"
#include "../terra/render.h"

#include "../particle/particle.h"
#include "../particle/partmap.h"
#include "../particle/df.h"
#include "../particle/light.h"

#include "../uvs/univang.h"

#include "../dast/poly3d.h"

#include "track.h"
#include "hobj.h"
#include "moveland.h"
#include "items.h"
#include "sensor.h"
#include "effect.h"
#include "mechos.h"

#include "../sound/hsound.h"

extern iGameMap* curGMap;
int SPViewX,SPViewY,SPScaleMap,SPTorXSize,SPTorYSize;

extern int AdvancedView;
extern int RAM16;

unsigned effectRNDVAL = 8383;
inline unsigned effectRND(unsigned m)
{
	effectRNDVAL ^= effectRNDVAL >> 3;
	effectRNDVAL ^= effectRNDVAL << 28;
	effectRNDVAL &= 0x7FFFFFFF;

	if(!m) return 0;

	return effectRNDVAL%m;
}

void MakeColorTable(int fc,int lc,uchar* d,uchar* pal)
{
	int ind;
	int mr,nr;
	int i,j,r;
	Vector vColor;
	for(i = 0;i < 256;i++){
		vColor = Vector(pal[i * 3],pal[i * 3 + 1],pal[i * 3 + 2]);
		ind  = vColor.vabs();

		ind += 20;

		vColor = Vector(pal[fc * 3],pal[fc * 3 + 1],pal[fc * 3 + 2]);
		mr = abs(ind - vColor.vabs());
		nr = fc;
		for(j = fc + 1;j < lc;j++){
			vColor = Vector(pal[j * 3],pal[j * 3 + 1],pal[j * 3 + 2]);
			r = abs(ind - vColor.vabs());
			if(r < mr){
				mr = r;
				nr = j;
			};
		};
		d[i] = nr;
	};
};

void ExplosionObject::Init(StorageType* s)
{
	BaseObject::Init(s);
	ID = ID_EXPLOSION;
	Scale = 1 << 15;
	dScale = 0;
	Owner = NULL;
};

void ExplosionObject::Quant(void)
{
	GetVisible();
	Scale += dScale;

	if(Scale > dScale) radius = (FirstRadius * Scale) >> 15;
	if(Owner){
		if(Owner->Status & SOBJ_DISCONNECT) Owner = NULL;
		else{
			R_curr = Owner->R_curr;
			R_curr.z += EXPLOSION_OFFSET;
		};
	};

	if(Visibility != VISIBLE){
		if(MainMapProcess.process((char*)XGR_GetVideoLine(0),0,0,1,0,1, 255, 1, 1))  Status |= SOBJ_DISCONNECT;
	}else MapLevel = 1;
};

void ExplosionObject::InitProcess(char _type)
{
	MainMapProcess.init(_type);
	FirstRadius = (MainMapProcess.Xmax > MainMapProcess.Ymax) ? MainMapProcess.Xmax : MainMapProcess.Ymax;
};

void ExplosionObject::Free(void)
{
	MainMapProcess.finit();
};

void ExplosionObject::DrawQuant(void)
{
	int tx,ty,s;
	if(AdvancedView) s = G2LF(R_curr,tx,ty);
	else s = G2LS(R_curr,tx,ty);
	if(MainMapProcess.process((char*)XGR_GetVideoLine(0),tx,ty,Scale*s >> 8,0,0,R_curr.z,R_curr.x & clip_mask_x, R_curr.y & clip_mask_y)) Status |= SOBJ_DISCONNECT;

//	if(MainMapProcess.process((char*)(VS(_video)->_video),tx,ty,Scale*curGMap -> xsize / TurnSecX,0,0)) Status |= SOBJ_DISCONNECT;
};

uchar GetGlobalAlt(int x,int y)
{
	uchar* p = vMap->lineT[y];
	uchar* t;
	if(p){
		p += x;
		t = p + H_SIZE;
		if((*t) & DOUBLE_LEVEL){
			if(x & 1) return *p;
			else return *(p + 1);
		}else return *p;
	};
	return 0;
};

void ExplosionObject::CreateExplosion(const Vector& v,BaseObject* _owner,int _scale,int _dscale)
{
	Owner = _owner;
	if(Owner) R_curr = Owner->R_curr;
	else R_curr = v;
	R_curr.z += EXPLOSION_OFFSET;
	Scale = _scale;
	dScale = _dscale;
	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
	GetVisible();
	MapLevel = 1;
	MainMapProcess.activate();
	radius = (FirstRadius * Scale) >> 15;
	MapD.CreateLight(R_curr.x,R_curr.y,R_curr.z,radius,32,LIGHT_TYPE::STATIONARY);
};

void EffectDispatcher::Init(Parser& in)
{
	int i,j,k;
	UnitList::Init(in);

	Total = 0;

	MaxUnit = new int[MAX_EFFECT_TYPE];
	UnitStorage = new StorageType[MAX_EFFECT_TYPE];

	in.search_name("NumEffect");

	if(RAM16){
		for(i = 0;i < MAX_EFFECT_TYPE;i++){
			MaxUnit[i] = in.get_int();
			MaxUnit[i] /= 2;
			if(MaxUnit[i] == 0) MaxUnit[i] = 1;
			Total += MaxUnit[i];
		};
	}else{
		for(i = 0;i < MAX_EFFECT_TYPE;i++){
			MaxUnit[i] = in.get_int();
			Total += MaxUnit[i];
		};
	};

	UnitData = new GeneralObject*[Total];
	k = 0;

	for(i = 0;i < MAX_EFFECT_TYPE;i++){
		UnitStorage[i].Init(MaxUnit[i]);
		for(j = 0;j < MaxUnit[i];j++){
			switch(i){
				case EFF_EXPLOSION01:
				case EFF_EXPLOSION02:
				case EFF_EXPLOSION03:
				case EFF_EXPLOSION04:
				case EFF_EXPLOSION05:
					UnitData[k] = new ExplosionObject;
					((ExplosionObject*)(UnitData[k]))->InitProcess(i - EFF_EXPLOSION01);
					break;
				case EFF_DEFORM01:
					UnitData[k] = new DeformObject;
					break;
				case EFF_PARTICLE01:
					UnitData[k] = new ParticleObject;
					((ParticleObject*)(UnitData[k]))->InitParicle(PARTICLE_SIZE01);
					break;
				case EFF_PARTICLE02:
					UnitData[k] = new TargetParticleObject;
					((TargetParticleObject*)(UnitData[k]))->InitParicle(PARTICLE_SIZE02);
					break;
				case EFF_PARTICLE03:
					UnitData[k] = new WaterParticleObject;
					((WaterParticleObject*)(UnitData[k]))->InitParicle(PARTICLE_SIZE03);
					break;
				case EFF_FIREBALL:
					UnitData[k] = new FireBallObject;
					break;
				case EFF_PARTICLE04:
					UnitData[k] = new WaterParticleObject;
					((WaterParticleObject*)(UnitData[k]))->InitParicle(PARTICLE_SIZE04);
					break;
				case EFF_PARTICLE05:
					UnitData[k] = new ParticleObject;
					((ParticleObject*)(UnitData[k]))->InitParicle(PARTICLE_SIZE05);
					break;
				case EFF_PARTICLE06:
					UnitData[k] = new ParticleObject;
					((ParticleObject*)(UnitData[k]))->InitParicle(PARTICLE_SIZE06);
					break;
				case EFF_PARTICLE_GENERATOR:
					UnitData[k] = new ParticleGenerator;
					break;
				case EFF_PARTICLE07:
					UnitData[k] = new ParticleObject;
					((ParticleObject*)(UnitData[k]))->InitParicle(PARTICLE_SIZE07);
			};
			UnitData[k]->Init(&UnitStorage[i]);
			UnitStorage[i].Deactive(UnitData[k]);
			k++;
		};
	};

/*	XStream fff;
	fff.open("pal.pal",XS_OUT);
	char dd;
	for(i = 0;i < 256;i++){
		dd = 63*i / 256;
		fff < dd < dd < dd;
	};
	fff.close();*/

//	FireBallProcess aaa;
//	aaa.Save("e:\\aaa\\aaa\\FireBall.lst");

//	FireBallProcess bbb;
//	bbb.Save("c:\\kron\\bml\\FireBall.lst");

	in.search_name("NumFireBallProcessType");
	NumFireBallProcessType = in.get_int();
	FireBallData = new FireBallProcess[NumFireBallProcessType];
//	char* nnn;

	for(i = 0;i < NumFireBallProcessType;i++){
		in.search_name("NameFireBallProcess");
		FireBallData[i].Load(in.get_name());

/*		nnn = in.get_name();
		FireBallData[i].Load(nnn);
		FireBallData[i].Convert(nnn);*/
	};

	in.search_name("NumDeformProcessType");
	NumDeformProcessType = in.get_int();
	DeformData = new WaveProcess[NumDeformProcessType];
	for(i = 0;i < NumDeformProcessType;i++){
		in.search_name("NameDeformProcess");
		DeformData[i].Init(in.get_name());
	};

	in.search_name("NumParticleProcessType");
	NumParticleInitType = in.get_int();
	ParticleInitData = new ParticleInitDataType[NumParticleInitType];

	for(i = 0;i < NumParticleInitType;i++){
		in.search_name("LifeTime");
		ParticleInitData[i].LifeTime = in.get_int();
		in.search_name("Velocity");
		ParticleInitData[i].Velocity = in.get_int() << in.get_int();
		in.search_name("FirstRadius");
		ParticleInitData[i].FirstRadius = in.get_int();
		in.search_name("EndRadius");
		ParticleInitData[i].EndRadius = in.get_int();
		in.search_name("FirstColor");
		ParticleInitData[i].FirstColor = in.get_int();
		in.search_name("EndColor");
		ParticleInitData[i].EndColor = in.get_int();
		in.search_name("FirstAlpha");
		ParticleInitData[i].FirstAlpha = in.get_int();
		in.search_name("StepAlpha");
		ParticleInitData[i].StepAlpha = in.get_int();
	};
};

void EffectDispatcher::Free(void)
{
	int i;

	delete[] ParticleInitData;

	for(i = 0;i < NumDeformProcessType;i++) DeformData[i].Free();
	delete[] DeformData;

	for(i = 0;i < NumFireBallProcessType;i++) FireBallData[i].Free();
	delete[] FireBallData;

	for(i = 0;i < Total;i++){
		UnitData[i]->Free();
		delete UnitData[i];
	};

	for(i = 0;i < MAX_EFFECT_TYPE;i++) UnitStorage[i].Free();

	delete[] UnitData;
	delete[] MaxUnit;
	delete[] UnitStorage;
};

void EffectDispatcher::CalcWave(void)
{
	int i;
	for(i = 0;i < NumDeformProcessType;i++) DeformData[i].Make();
};

void EffectDispatcher::Open(Parser& in)
{
	int i;
	UnitList::Open(in);
	for(i = 0;i < Total;i++) UnitData[i]->Open();
};

void EffectDispatcher::Close(void)
{
	int i;

	BaseObject* p;
	p = (BaseObject*)Tail;

	while(p){
		DeleteEffect(p);
		p = (BaseObject*)(p->NextTypeList);
	};

	for(i = 0;i < MAX_EFFECT_TYPE;i++) UnitStorage[i].Check();
	for(i = 0;i < Total;i++) UnitData[i]->Close();
};

void EffectDispatcher::DeleteEffect(BaseObject* p)
{
	GameD.DisconnectBaseList(p);
	DisconnectTypeList(p);
	p->Storage->Deactive(p);
};

void EffectDispatcher::CreateExplosion(const Vector& v,unsigned char _type,BaseObject* _owner,int _scale,int _dscale)
{
	ExplosionObject* p;
	p = (ExplosionObject*)(UnitStorage[_type].Active());
	if(p){
		p->CreateExplosion(v,_owner,_scale,_dscale);
		ConnectTypeList(p);
		GameD.ConnectBaseList(p);
	};	
};

void EffectDispatcher::CreateFireBall(const Vector& v,unsigned char _type,BaseObject* _owner,int _scale,int _dscale)
{
	FireBallObject* p;

//	if(_type < 0 || _type >=NumParticleInitType) ErrH.Abort("Error Number of  Deform System");

	p = (FireBallObject*)(UnitStorage[EFF_FIREBALL].Active());
	if(p){
		p->CreateFireBall(v,_owner,_scale,_dscale,_type);
		ConnectTypeList(p);
		GameD.ConnectBaseList(p);
	};
};

void EffectDispatcher::CreateParticle(char _init,const Vector& v1,const Vector& v2,unsigned char _type)
{
	ParticleObject* p;
//	if(_init < 0 || _init >=NumParticleInitType) ErrH.Abort("Error Number of Particle System");

	p = (ParticleObject*)(UnitStorage[_type].Active());
	if(p){
//		p->CreateParticle(ParticleInitData[_init].LifeTime,ParticleInitData[_init].Velocity,ParticleInitData[_init].FirstRadius,ParticleInitData[_init].EndRadius,ParticleInitData[_init].FirstColor,ParticleInitData[_init].EndColor,v1,v2);
		p->CreateParticle(ParticleInitData + _init,v1,v2);
		ConnectTypeList(p);
		GameD.ConnectBaseList(p);
	};
};

void EffectDispatcher::CreateParticleTime(unsigned char _init,const Vector& v1,const Vector& v2, unsigned char _type,int _LifeTime)
{
	ParticleObject* p;
//	if(_init < 0 || _init >=NumParticleInitType) ErrH.Abort("Error Number of Particle System");

	p = (ParticleObject*)(UnitStorage[_type].Active());
	if(p){
		p->CreateParticle(_LifeTime,ParticleInitData[_init].Velocity,ParticleInitData[_init].FirstRadius,ParticleInitData[_init].EndRadius,ParticleInitData[_init].FirstColor,ParticleInitData[_init].EndColor,v1,v2);
		ConnectTypeList(p);
		GameD.ConnectBaseList(p);
	};
};


void EffectDispatcher::CreateParticle(char _init,const Vector& v,unsigned char _type)
{
	ParticleObject* p;
//	if(_init < 0 || _init >=NumParticleInitType) ErrH.Abort("Error Number of Particle System");

	p = (ParticleObject*)(UnitStorage[_type].Active());
	if(p){
		p->CreateParticle(ParticleInitData + _init,v);
		ConnectTypeList(p);
		GameD.ConnectBaseList(p);
	};
};

void EffectDispatcher::CreateDirectParticle(char _init,const Vector& v,unsigned char _type)
{
	ParticleObject* p;
//	if(_init < 0 || _init >=NumParticleInitType) ErrH.Abort("Error Number of Particle System");

	p = (ParticleObject*)(UnitStorage[_type].Active());
	if(p){
		p->CreateDirectParticle(ParticleInitData + _init,v);
		ConnectTypeList(p);
		GameD.ConnectBaseList(p);
	};
};

void EffectDispatcher::CreateParticle(char _init,const Vector& v1,const Vector& v2,const DBM& m,unsigned char _type)
{
	ParticleObject* p;
//	if(_init < 0 || _init >=NumParticleInitType) ErrH.Abort("Error Number of Particle System");
	p = (ParticleObject*)(UnitStorage[_type].Active());
	if(p){
		p->CreateParticle(ParticleInitData + _init,v1,v2,m);
		ConnectTypeList(p);
		GameD.ConnectBaseList(p);
	};
};

void EffectDispatcher::CreateRingOfLord(int type,const Vector v1,int rad,int ltime,int fcol,int lcol,int vel)
{
	ParticleObject* p;
	p = (ParticleObject*)(UnitStorage[type].Active());
	if(p){
		p->CreateRingOfLord(v1,rad,ltime,fcol,lcol,vel);
		ConnectTypeList(p);
		GameD.ConnectBaseList(p);
	};
};


void EffectDispatcher::Quant(void)
{
	BaseObject* p;

/*	SPViewX = ViewX << 15;
	SPViewY = ViewY << 15;
	SPScaleMap = ScaleMap << 7;
	SPTorXSize = TOR_XSIZE << 15;*/

	SPViewX = ViewX << 8;
	SPViewY = ViewY << 8;
	SPScaleMap = ScaleMap;
	SPTorXSize = TOR_XSIZE << 8;
	SPTorYSize = TOR_YSIZE << 8;

	p = (BaseObject*)Tail;
	while(p){
		p->Quant();
		p = (BaseObject*)(p->NextTypeList);
	};

	p = (BaseObject*)Tail;
	while(p){
		if(p->Status & SOBJ_DISCONNECT) DeleteEffect(p);
		p = (BaseObject*)(p->NextTypeList);
	};
};

void EffectDispatcher::CreateDeform(const Vector& v,char _fl, unsigned char _type)
{
	DeformObject* p;

//	if(_type < 0 || _type >=NumParticleInitType)
//		ErrH.Abort("Error Number of  Deform System");

	p = (DeformObject*)(UnitStorage[EFF_DEFORM01].Active());
	if(p){
		p->CreateDeform(v,_fl,DeformData + _type);
		ConnectTypeList(p);
		GameD.ConnectBaseList(p);
	};
};

void DeformObject::CreateDeform(const Vector& v,char _fl,WaveProcess* p)
{
	R_curr = v;
//	cycleTor(R_curr.x,R_curr.y);
	R_curr.x = XCYCL(R_curr.x);
	R_curr.y = YCYCL(R_curr.y);
	Status = 0;
	GetVisible();
	Offset = 0;
	wProcess = p;
	MapLevel = 1;
	FullFlag = _fl;
	radius = (wProcess->sx >  wProcess->sy) ? wProcess->sx : wProcess->sy;
	GetVisible();
};

void DeformObject::Quant(void)
{
	GetVisible();	
	if(wProcess->CheckOffset(Offset) || Visibility == UNVISIBLE) Status |= SOBJ_DISCONNECT;
};

void DeformObject::DrawQuant(void)
{
	int tx,ty;
	G2L(R_curr,tx,ty);
	wProcess->Deform(tx,ty,Offset,FullFlag);
};

void DeformObject::Init(StorageType* s)
{
	BaseObject::Init(s);
	ID = ID_DEFORM;
};

void ParticleObject::InitParicle(int num)
{
	NumParticle = num;
	Data = new SimpleParticleType[NumParticle];
};

void ParticleObject::Free(void)
{
	delete[] Data;
};

void ParticleObject::Init(StorageType* s)
{
	BaseObject::Init(s);
	ID = ID_EXPLOSION;
};

void ParticleObject::Quant(void)
{
	if(DepthShow){
		int xt = getDistX(R_curr.x,ViewX);
		int yy = getDistY(R_curr.y,ViewY);
		int zz = round(A_g2s.a[6]*xt + A_g2s.a[7]*yy) - R_curr.z + ViewZ;
		int xx = round(A_g2s.a[0]*xt + A_g2s.a[1]*yy);
		yy = round(A_g2s.a[3]*xt + A_g2s.a[4]*yy);
		if(zz < -radius || abs(xx) > zz*TurnSideX/ViewZ + radius*2 ||  abs(yy) > zz*TurnSideY/ViewZ + radius*2) 
			Visibility = UNVISIBLE;
		else 
			Visibility = VISIBLE;
		}
	else{
		if(abs(getDistX(R_curr.x,ViewX)) - (radius << 1) < TurnSideX && abs(getDistY(R_curr.y,ViewY)) -  (radius << 1) < TurnSideY) 
			Visibility = VISIBLE;
		else 
			Visibility = UNVISIBLE;
		}
	if(Time++ >= LifeTime) Status |= SOBJ_DISCONNECT;
};

void SimpleParticleType::Quant(void)
{
	vR += vD;
	vR.x &= PTrack_mask_x;
	vR.y &= PTrack_mask_y;
	Color += dColor;
};

const int PARTICLE_MAX_DELTA = 15 << 8;

void SimpleParticleType::QuantRingOfLord(Vector v,int s,int c)
{
	int tx,ty,px,py,d;

	tx = v.x - vR.x;
	ty = v.y - vR.y;

	int SPX_100 = SPTorXSize - (500<<8);
	int SPY_100 = SPTorYSize - (500<<8);

	if(tx > (SPX_100))
		tx -= SPTorXSize;
	else if((tx) < (-SPX_100)) 
		tx += SPTorXSize;		

	if(ty > (SPY_100))
		ty -= SPTorYSize;
	else if((ty) < (-SPY_100)) 
		ty += SPTorYSize;

	px = ty*c;
	py = -tx*c;

	tx += px;
	ty += py;

	d = abs(tx) + abs(ty);
	if(d){
		vD.x = tx * s / d;
		vD.y = ty * s / d;
	};
	
	vR += vD;
	vR.z = v.z;
	
	vR.x &= PTrack_mask_x;
//	vR.y &= PTrack_mask_y;
	Color += dColor;
};


void SimpleParticleType::QuantP(Vector _c, Vector _n, int s,int c)
{
	int tx,ty,d;

	vD = Vector(0,0,0);

	tx = -(vR.x - _c.x);
	ty = -(vR.y - _c.y);

	int SPX_100 = SPTorXSize - (300<<8);
	int SPY_100 = SPTorYSize - (300<<8);

	if(tx > (SPX_100))
		tx -= SPTorXSize;
	else if((tx) < (-SPX_100)) 
		tx += SPTorXSize;

	if(ty > (SPY_100) )
		ty -= SPTorYSize;
	else if((ty) < (-SPY_100)) 
		ty += SPTorYSize;

	/*px = ty*c;
	py = -tx*c;

	if (RND(3)){
		tx += px;
		ty += py;
	} else {
		tx = px - tx;
		ty = py - ty;
	} */

//	vD.x >>= 1;
//	vD.y >>= 1;

	d = abs(tx) + abs(ty);
	if(d > 100 && !RND(4)){
		vD.x = tx * s / d;
		vD.y = ty * s / d;
	};
	
	vD += _n;
	vD.x += ((3 - RND(7))<<8);
	vD.y += ((3 - RND(7))<<8);
	/*d = abs(tx) + abs(ty);
	if(d){
		vD.x += tx * s / d;
		vD.y += ty * s / d;
		vD.z += (_n.z - _c.z) * s / d;
	};*/
	
	vR += vD;
	vR.z = _c.z;
	
	vR.x &= PTrack_mask_x;
	vR.y &= PTrack_mask_y;
};

void SimpleParticleType::QuantT(int x,int y,int s)
{
	Vector vTrack,vTrackP;
	int d;
	vTrack.x = x - vR.x;
	vTrack.y = y - vR.y;
	int SPX_100 = SPTorXSize - (100<<8);
	int SPY_100 = SPTorYSize - (100<<8);

	if(vTrack.x > (SPX_100) )
		vTrack.x -= SPTorXSize;
	else if((vTrack.x) < (-SPX_100)) 
		vTrack.x += SPTorXSize;		

	if(vTrack.y > (SPY_100) )
		vTrack.y -= SPTorYSize;
	else if((vTrack.y) < (-SPY_100)) 
		vTrack.y += SPTorYSize;		

	vTrackP.x = vTrack.y;
	vTrackP.y = -vTrack.x;

	vTrack.x += vTrackP.x;
	vTrack.y += vTrackP.y;

	d = (abs(vTrack.x) + abs(vTrack.y))>>2;
//	d <<= WATER_PARTICLE_DIVISION;
	if(d){
		vD.x = vTrack.x * s / d;
		vD.y = vTrack.y * s / d;
	};

//	d = abs(vD.x) + abs(vD.y);
	vD.x -= vD.x >> 4;
	vD.y -= vD.y >> 4;

	vR += vD;
//	vR += vTrack;
	vR.x &= PTrack_mask_x;
	vR.y &= PTrack_mask_y;
	Color += dColor;
};

void ParticleObject::DrawQuant(void)
{
	int i;
	SimpleParticleType* p;
	Vector vPos;
	int tx,ty;
	int phi,dphi;

	if(Mode){
		if(Time < LifeTime){
			dphi = (Time*PI << 8) / (2*NumParticle * LifeTime);
			phi = 0;
		}else{
			phi = PI / 2;
			dphi = 0;
		};

		if(AdvancedView){
			for(i = 0,p = Data;i < NumParticle;i++,p++){				
				p->QuantRingOfLord(Vector(R_curr.x << 8,R_curr.y << 8,R_curr.z << 8),abs(25 * SI[rPI(phi >> 8)] >> 8),32);
				vPos = p->vR;
				vPos >>= 8;
//				if(GetAltLevel(vPos)){
					G2LQ(vPos,tx,ty);
	//				G2L(vPos.x,vPos.y,tx,ty);
					if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
//				};
				phi += dphi;
			};
		}else{
			if(CurrentWorld < MAIN_WORLD_MAX - 1){			
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					p->QuantRingOfLord(Vector(R_curr.x << 8,R_curr.y << 8,R_curr.z << 8),abs(25 * SI[rPI(phi >> 8)] >> 8),32);
					vPos = p->vR;
					vPos >>= 8;
	//				if(GetAltLevel(vPos)){
		//				tx = round(SPGetDistX(p->vR.x,SPViewX)*ScaleMapInvFlt) + ScreenCX;
		//				ty = round((p->vR.y - SPViewY) * ScaleMapInvFlt) + ScreenCY;

						tx = ((int)round(SPGetDistX(p->vR.x,SPViewX) * ScaleMapInvFlt) >> 8) + ScreenCX;
						ty = ((int)round((p->vR.y - SPViewY) * ScaleMapInvFlt) >> 8)+ ScreenCY;

						if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
	//				};
					phi +=dphi;
				};
			}else{
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					p->QuantRingOfLord(Vector(R_curr.x << 8,R_curr.y << 8,R_curr.z << 8),abs(25 * SI[rPI(phi >> 8)] >> 8),32);
					vPos = p->vR;
					vPos >>= 8;
	//				if(GetAltLevel(vPos)){
		//				tx = round(SPGetDistX(p->vR.x,SPViewX)*ScaleMapInvFlt) + ScreenCX;
		//				ty = round((p->vR.y - SPViewY) * ScaleMapInvFlt) + ScreenCY;

						tx = ((int)round(SPGetDistX(p->vR.x,SPViewX) * ScaleMapInvFlt) >> 8) + ScreenCX;
						ty = ((int)round(SPGetDistY(p->vR.y,SPViewY) * ScaleMapInvFlt) >> 8) + ScreenCY;

						if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
	//				};
					phi +=dphi;
				};
			};
		};
	}else{
		if(AdvancedView){
			for(i = 0,p = Data;i < NumParticle;i++,p++){
				p->Quant();
				vPos = p->vR;
				vPos >>= 8;
				if(GetAltLevel(vPos)){
					G2LQ(vPos,tx,ty);
	//				G2L(vPos.x,vPos.y,tx,ty);
					if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
				};
			};
		}else{
			if(CurrentWorld < MAIN_WORLD_MAX - 1){
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					p->Quant();
					vPos = p->vR;
					vPos >>= 8;
					if(GetAltLevel(vPos)){
		//				tx = round(SPGetDistX(p->vR.x,SPViewX)*ScaleMapInvFlt) + ScreenCX;
		//				ty = round((p->vR.y - SPViewY) * ScaleMapInvFlt) + ScreenCY;

						tx = ((int)round(SPGetDistX(p->vR.x,SPViewX) * ScaleMapInvFlt) >> 8) + ScreenCX;
						ty = ((int)round((p->vR.y - SPViewY) * ScaleMapInvFlt) >> 8)+ ScreenCY;

						if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
					};
				};
			}else{
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					p->Quant();
					vPos = p->vR;
					vPos >>= 8;
					if(GetAltLevel(vPos)){
		//				tx = round(SPGetDistX(p->vR.x,SPViewX)*ScaleMapInvFlt) + ScreenCX;
		//				ty = round((p->vR.y - SPViewY) * ScaleMapInvFlt) + ScreenCY;

						tx = ((int)round(SPGetDistX(p->vR.x,SPViewX) * ScaleMapInvFlt) >> 8) + ScreenCX;
						ty = ((int)round(SPGetDistY(p->vR.y,SPViewY) * ScaleMapInvFlt) >> 8) + ScreenCY;

						if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
					};
				};
			};
		};
	};
};

int GetAltLevel(Vector v)
{
	uchar* p = vMap->lineT[v.y];
	if(p){
		p += v.x;
		if( (*(p+1)) < v.z && (*p) < v.z ) return 1;
	};
	return 0;
};

/*
char GetAltLevel(int x,int y,int z)
{
	uchar* p = vMap->lineT[y];
	uchar* t;
	if(p){
		p += x;
		t = p + H_SIZE;
		if((*t) & DOUBLE_LEVEL){
			if(x & 1){
				if((*p) < z) return 1;
			}else{
				if(*(p + 1) < z) return 1;
			};
		}else if((*p) < z) return 1;
	};
	return 0;
};*/

void ParticleObject::CreateParticle(ParticleInitDataType* n,const Vector& v1,const Vector& v2)
{
	int i;
	int Radius,SignRadius,DeltaRadius,SignVelocity,FirstColor,DeltaColor,Velocity;
	Vector vDelta,vPos;
	SimpleParticleType* p;

	Mode = 0;

	Time = 0;

	LifeTime = n->LifeTime;

	FirstColor = n->FirstColor << 8;
	DeltaColor = ((n->EndColor << 8) - FirstColor) / LifeTime;

	Radius = n->FirstRadius << 8;
	DeltaRadius = ((n->EndRadius << 8) - Radius) / NumParticle;

	Velocity = n->Velocity;
	SignVelocity = Velocity << 1;

	vDelta = Vector(getDistX(v2.x,v1.x),getDistY(v2.y,v1.y),(v2.z - v1.z));

	R_curr.x = vDelta.x >> 1;
	R_curr.y = vDelta.y >> 1;
	R_curr.z = vDelta.z >> 1;

	radius = (R_curr.x > R_curr.y) ? R_curr.x : R_curr.y;
//	if(radius > 64) ErrH.Abort("Bad Particle Radius");

	R_curr += v1;
	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
//	GetVisible();
	MapLevel = 1;

	vDelta <<= 8;
	vDelta /= NumParticle;
	vPos = Vector(v1.x << 8,v1.y << 8,v1.z << 8);

	for(i = 0,p = Data;i < NumParticle;i++,p++){
		SignRadius = Radius << 1;
		p->Color = FirstColor;
		p->dColor = DeltaColor;
		p->vD = Vector(Velocity - effectRND(SignVelocity),Velocity - effectRND(SignVelocity),Velocity - effectRND(SignVelocity));
		p->vR = vPos + Vector(Radius - effectRND(SignRadius),Radius - effectRND(SignRadius),Radius - effectRND(SignRadius));
		vPos += vDelta;
		Radius += DeltaRadius;
	};
};

void ParticleObject::CreateParticle(ParticleInitDataType* n,const Vector& v1,const Vector& v2,const DBM& m)
{
	int i;
	int Radius,SignRadius,DeltaRadius,FirstColor,DeltaColor;
	Vector vDelta,vPos,vMax;
	SimpleParticleType* p;
	int Alpha,StepAlpha;

	Time = 0;
	Mode = 0;

	LifeTime = n->LifeTime;

	FirstColor = n->FirstColor << 8;
	DeltaColor = ((n->EndColor << 8) - FirstColor) / LifeTime;

	Radius = n->FirstRadius << 8;
	DeltaRadius = ((n->EndRadius << 8) - Radius) / NumParticle;

	Alpha = n->FirstAlpha;
	StepAlpha = n->StepAlpha;

	vDelta = Vector(getDistX(v2.x,v1.x),getDistY(v2.y,v1.y),(v2.z - v1.z));

	R_curr.x = vDelta.x >> 1;
	R_curr.y = vDelta.y >> 1;
	R_curr.z = vDelta.z >> 1;

	radius = (R_curr.x > R_curr.y) ? R_curr.x : R_curr.y;
//	if(radius > 64) ErrH.Abort("Bad Particle Radius");

	R_curr += v1;
	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
//	GetVisible();
	MapLevel = 1;

	vDelta <<= 8;
	vDelta /= NumParticle;
	vPos = Vector(v1.x << 8,v1.y << 8,v1.z << 8);

	vMax = Vector(n->Velocity,0,0)*m;

	for(i = 0,p = Data;i < NumParticle;i++,p++){
		SignRadius = Radius << 1;
		p->Color = FirstColor;
		p->dColor = DeltaColor;
		p->vD = vMax*abs(COS(Alpha)) >> 8;
		p->vR = vPos + Vector(Radius - effectRND(SignRadius),Radius - effectRND(SignRadius),Radius - effectRND(SignRadius));
		vPos += vDelta;
		Radius += DeltaRadius;
		Alpha += StepAlpha;
	};
};

void ParticleObject::CreateParticle(int _LifeTime,int _Velocity,int _FirstRadius,int _EndRadius,int _FirstColor,int _EndColor,const Vector& v1,const Vector& v2)
{
	int i;
	int Radius,SignRadius,DeltaRadius,SignVelocity,FirstColor,DeltaColor;
	Vector vDelta,vPos;
	SimpleParticleType* p;

	Time = 0;
	Mode = 0;

	LifeTime = _LifeTime;

	FirstColor = _FirstColor << 8;
	DeltaColor = ((_EndColor << 8) - FirstColor) / LifeTime;

	Radius = _FirstRadius << 8;
	DeltaRadius = ((_EndRadius << 8) - Radius) / NumParticle;

	SignVelocity = _Velocity << 1;

	vDelta = Vector(getDistX(v2.x,v1.x),getDistY(v2.y,v1.y),(v2.z - v1.z));

	R_curr.x = vDelta.x >> 1;
	R_curr.y = vDelta.y >> 1;
	R_curr.z = vDelta.z >> 1;

	radius = (R_curr.x > R_curr.y) ? R_curr.x : R_curr.y;
//	if(radius > 64) ErrH.Abort("Bad Particle Radius");

	R_curr += v1;
	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
//	GetVisible();
	MapLevel = 1;

	vDelta <<= 8;
	vDelta /= NumParticle;
	vPos = Vector(v1.x << 8,v1.y << 8,v1.z << 8);

	for(i = 0,p = Data;i < NumParticle;i++,p++){
		SignRadius = Radius << 1;
		p->Color = FirstColor;
		p->dColor = DeltaColor;
		p->vD = Vector(_Velocity - effectRND(SignVelocity),_Velocity - effectRND(SignVelocity),_Velocity - effectRND(SignVelocity));
		p->vR = vPos + Vector(Radius - effectRND(SignRadius),Radius - effectRND(SignRadius),Radius - effectRND(SignRadius));
		vPos += vDelta;
		Radius += DeltaRadius;
	};
};

void ParticleObject::CreateParticle(int _LifeTime,int _Velocity,int _FirstRadius,int _EndRadius,int _FirstColor,int _EndColor,int _StepAlpha,const Vector& v1,const Vector& v2,const DBM& m)
{
	int i;
	int Radius,SignRadius,DeltaRadius,FirstColor,DeltaColor;
	Vector vDelta,vPos,vMax;
	SimpleParticleType* p;
	int Alpha = 0;

	Time = 0;
	Mode = 0;

	LifeTime = _LifeTime;

	FirstColor = _FirstColor << 8;
	DeltaColor = ((_EndColor << 8) - FirstColor) / LifeTime;

	Radius = _FirstRadius << 8;
	DeltaRadius = ((_EndRadius << 8) - Radius) / NumParticle;

	vDelta = Vector(getDistX(v2.x,v1.x),getDistY(v2.y,v1.y),(v2.z - v1.z));

	R_curr.x = vDelta.x >> 1;
	R_curr.y = vDelta.y >> 1;
	R_curr.z = vDelta.z >> 1;

	radius = (R_curr.x > R_curr.y) ? R_curr.x : R_curr.y;
//	if(radius > 64) ErrH.Abort("Bad Particle Radius");

	R_curr += v1;
	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
//	GetVisible();
	MapLevel = 1;

	vDelta <<= 8;
	vDelta /= NumParticle;
	vPos = Vector(v1.x << 8,v1.y << 8,v1.z << 8);

	vMax = Vector(_Velocity,0,0)*m;

	for(i = 0,p = Data;i < NumParticle;i++,p++){
		SignRadius = Radius << 1;
		p->Color = FirstColor;
		p->dColor = DeltaColor;
		p->vD = vMax*fabs(Cos(Alpha));
		p->vR = vPos + Vector(Radius - effectRND(SignRadius),Radius - effectRND(SignRadius),Radius - effectRND(SignRadius));
		vPos += vDelta;
		Radius += DeltaRadius;
		Alpha += _StepAlpha;
	};
};

void ParticleObject::CreateParticle(ParticleInitDataType* n,const Vector& v)
{
	int i;
	int SignVelocity,FirstColor,DeltaColor,Velocity;
	Vector vDelta,vPos;
	SimpleParticleType* p;
	int Alpha,StepAlpha;

	Time = 0;
	LifeTime = n->LifeTime;

	Mode = 0;

	FirstColor = n->FirstColor << 8;
	DeltaColor = ((n->EndColor << 8) - FirstColor) / LifeTime;

	radius = n->FirstRadius;
	R_curr = v;
	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
	MapLevel = 1;

	Velocity = n->Velocity;
	SignVelocity = Velocity << 1;

	vPos = Vector(v.x << 8,v.y << 8,v.z << 8);	

	StepAlpha = rPI(2*PI / NumParticle);
	Alpha = RND(PI);

	for(i = 0,p = Data;i < NumParticle;i++,p++){
		p->Color = FirstColor;
		p->dColor = DeltaColor;
		p->vD = Vector(Velocity - RND(SignVelocity),Velocity - RND(SignVelocity),Velocity - RND(SignVelocity));
		p->vR = vPos + Vector(radius*COS(Alpha) >> 8,radius*SIN(Alpha) >> 8,0);
		Alpha += StepAlpha;
	};
};

void ParticleObject::CreateRingOfLord(const Vector v1,int rad,int ltime,int fcol,int lcol,int vel)
{
	int i;
	int FirstColor,DeltaColor;
	Vector vDelta,vPos;
	SimpleParticleType* p;

	Mode = 1;

	Time = 0;
	LifeTime = ltime;	

	FirstColor = fcol << 8;
	DeltaColor = ((lcol << 8) - FirstColor) / LifeTime;

	radius = rad;
	R_curr = v1;
	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
	MapLevel = 1;

	Phase = 0;
	dPhase = 2*PI / LifeTime;

	vPos.x = R_curr.x << 8;
	vPos.y = R_curr.y << 8;
	vPos.z = R_curr.z << 8;

	for(i = 0,p = Data;i < NumParticle;i++,p++){
		p->Color = FirstColor;
		p->dColor = DeltaColor;
		p->vD = Vector(0,0,0);
		p->vR = vPos + Vector(vel,0,0);
	};
};

void ParticleObject::CreateDirectParticle(ParticleInitDataType* n,const Vector& v)
{
	int i;
	int FirstColor,DeltaColor,Velocity;
	DBV vDelta;
	Vector vPos;
	SimpleParticleType* p;
	int Alpha,StepAlpha;

	Mode = 0;

	Time = 0;
	LifeTime = n->LifeTime;

	FirstColor = n->FirstColor << 8;
	DeltaColor = ((n->EndColor << 8) - FirstColor) / LifeTime;

	radius = n->FirstRadius;
	R_curr = v;
	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
	MapLevel = 1;

	Velocity = n->Velocity;

	vPos = Vector(v.x << 8,v.y << 8,v.z << 8);

	StepAlpha = rPI(2*PI / NumParticle);
	Alpha = RND(PI);
	double radius_inv = 1/(double)radius;
	for(i = 0,p = Data;i < NumParticle;i++,p++){
		p->Color = FirstColor;
		p->dColor = DeltaColor;
		vDelta = DBV(radius*Cos(Alpha),radius*Sin(Alpha),0);
		p->vD = vDelta*(RND(Velocity)*radius_inv);
//		p->vD = Vector(Velocity - RND(SignVelocity),Velocity - RND(SignVelocity),Velocity - RND(SignVelocity));
		p->vR = vPos + Vector(vDelta*256.);
		Alpha += StepAlpha;
	};
};

void TargetParticleObject::InitParicle(int num)
{
	CurrParticle = 0;
	NumParticle = num;
	Data = new TargetParticleType[num];
	FadeTime = FadeNum = 0;
};

void TargetParticleObject::Free(void)
{
	delete[] Data;
};

void TargetParticleObject::Init(StorageType* s)
{
	BaseObject::Init(s);
	ID = ID_EXPLOSION;
};

const int TARGET_PARTICLE_FADE_TIME = 30;

void TargetParticleObject::Quant(void)
{
	if(DepthShow){
		int xt = getDistX(R_curr.x,ViewX);
		int yy = getDistY(R_curr.y,ViewY);
		int zz = round(A_g2s.a[6]*xt + A_g2s.a[7]*yy) - R_curr.z + ViewZ;
		int xx = round(A_g2s.a[0]*xt + A_g2s.a[1]*yy);
		yy = round(A_g2s.a[3]*xt + A_g2s.a[4]*yy);
		if(zz < -radius || abs(xx) > zz*TurnSideX/ViewZ + radius*2 ||  abs(yy) > zz*TurnSideY/ViewZ + radius*2) Visibility = UNVISIBLE;
		else Visibility = VISIBLE;
	}else{
		if(abs(getDistX(R_curr.x,ViewX)) - (radius << 1) < TurnSideX && abs(getDistY(R_curr.y,ViewY)) -  (radius << 1) < TurnSideY) Visibility = VISIBLE;
		else Visibility = UNVISIBLE;
	};
	if(TargetType){
		if(Time == 0){
			FadeTime = LifeTime - TARGET_PARTICLE_FADE_TIME;
			FadeNum = CurrParticle / TARGET_PARTICLE_FADE_TIME;
		}else{
			if(Time >= FadeTime) 
				CurrParticle -= FadeNum;
		};
	};
	if(++Time > LifeTime) 	Status |= SOBJ_DISCONNECT;	
};


void TargetParticleObject::DrawQuant(void)
{
	int i;
	TargetParticleType* p;

	if(TargetType){
		if(AdvancedView) for(i = 0,p = Data;i < CurrParticle;i++,p++) p->aQuant();
		else for(i = 0,p = Data;i < CurrParticle;i++,p++) p->sQuant();
	}else{
		if(AdvancedView) for(i = 0,p = Data;i < CurrParticle;i++,p++) p->aQuant2();
		else for(i = 0,p = Data;i < CurrParticle;i++,p++) p->sQuant2();
	};
};

/*
void TargetParticleObject::DrawQuant(void)
{
	int i;
	TargetParticleType* p;
	Vector vPos;
	int tx,ty;

	if(TargetType){
		if(AdvancedView){
			for(i = 0,p = Data;i < CurrParticle;i++,p++){
				p->Quant();
				vPos = p->vR;
				vPos >>= 8;
				if(GetAltLevel(vPos.x,vPos.y,vPos.z)){
					G2LQ(vPos.x,vPos.y,vPos.z,tx,ty);
	//				G2L(vPos.x,vPos.y,tx,ty);
					if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color);
				};
			};
		}else{
			for(i = 0,p = Data;i < CurrParticle;i++,p++){
				p->Quant();
				vPos = p->vR;
				vPos >>= 8;
				if(GetAltLevel(vPos.x,vPos.y,vPos.z)){
					tx = SPGetDistX(p->vR.x,SPViewX) * ScaleMapInvFlt + ScreenCX;
					ty = (p->vR.y - SPViewY) * ScaleMapInvFlt + ScreenCY;
					if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color);
				};
			};
		};
	}else{
		if(AdvancedView){
			for(i = 0,p = Data;i < CurrParticle;i++,p++){
				p->Quant2();
				vPos = p->vR;
				vPos >>= 8;
				if(GetAltLevel(vPos.x,vPos.y,vPos.z)){
					G2LQ(vPos.x,vPos.y,vPos.z,tx,ty);
	//				G2L(vPos.x,vPos.y,tx,ty);
					if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color);
				};
			};
		}else{
			for(i = 0,p = Data;i < CurrParticle;i++,p++){
				p->Quant2();
				vPos = p->vR;
				vPos >>= 8;
				if(GetAltLevel(vPos.x,vPos.y,vPos.z)){
					tx = SPGetDistX(p->vR.x,SPViewX) * ScaleMapInvFlt + ScreenCX;
					ty = (p->vR.y - SPViewY) * ScaleMapInvFlt + ScreenCY;
					if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color);
				};
			};
		};
	};
};
*/

const uchar TARGET_PARTICLE_NORMAL_SHIFT = 3;

void TargetParticleType::aQuant(void)
{
	int tx,ty,d;
	tx = SPGetDistX(vT.x,vR.x);
	ty = vT.y - vR.y;
	if(tx || ty){
//		d = (int)(sqrt(tx*(double)tx + ty*(double)ty));
		if(tx > 0){
			if(ty > 0) d = tx + ty;
			else d = tx - ty;
		}else{
			if(ty > 0) d = ty - tx;
			else d = -tx - ty;
		};

		tx = tx * s / d;
		ty = ty * s / d;

//		vD.x += tx*s / d;
//		vD.y += ty*s / d;
		vD.x += tx + (ty >> TARGET_PARTICLE_NORMAL_SHIFT);
		vD.y += ty - (tx >> TARGET_PARTICLE_NORMAL_SHIFT);

		if(pDist < d){
			vD.x -= vD.x >> 4;
			vD.y -= vD.y >> 4;
		};

		vR.x += vD.x;
		vR.y += vD.y;
		vR.z += vD.z;

		vR.x &= PTrack_mask_x;
//		vR.y &= PTrack_mask_y;
		pDist = d;

		G2LQ(Vector(vR.x >> 8,vR.y >> 8,vR.z),tx,ty);
		if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,Color);
	};
};


void TargetParticleType::sQuant(void)
{
	int tx,ty,d;
	tx = SPGetDistX(vT.x,vR.x);
	ty = vT.y - vR.y;
	if(tx || ty){
//		d = (int)(sqrt(tx*(double)tx + ty*(double)ty));
		if(tx > 0){
			if(ty > 0) d = tx + ty;
			else d = tx - ty;
		}else{
			if(ty > 0) d = ty - tx;
			else d = -tx - ty;
		};

		tx = tx * s / d;
		ty = ty * s / d;

//		vD.x += tx*s / d;
//		vD.y += ty*s / d;
		vD.x += tx + (ty >> TARGET_PARTICLE_NORMAL_SHIFT);
		vD.y += ty - (tx >> TARGET_PARTICLE_NORMAL_SHIFT);

		if(pDist < d){
			vD.x -= vD.x >> 4;
			vD.y -= vD.y >> 4;
		};

		vR.x += vD.x;
		vR.y += vD.y;
		vR.z += vD.z;

		vR.x &= PTrack_mask_x;
//		vR.y &= PTrack_mask_y;
		pDist = d;

//		tx = round(SPGetDistX(vR.x,SPViewX) * ScaleMapInvFlt) + ScreenCX;
//		ty = round((vR.y - SPViewY) * ScaleMapInvFlt) + ScreenCY;

		tx = ((int)round(SPGetDistX(vR.x,SPViewX) * ScaleMapInvFlt) >> 8) + ScreenCX;
		ty = ((int)round((vR.y - SPViewY) * ScaleMapInvFlt) >> 8)+ ScreenCY;

		if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,Color);
	};
};

void TargetParticleObject::CreateParticle(const Vector& _vTarget,int _LifeTime,char type)
{
	Time = 0;
	CurrParticle = 0;
	LifeTime = _LifeTime;
	R_curr = _vTarget;
	cycleTor(R_curr.x,R_curr.y);
	vTarget.x = R_curr.x << 8;
	vTarget.y = R_curr.y << 8;
	vTarget.z = R_curr.z << 8;
	radius = 0;
	Status = 0;
	MapLevel = 1;
	TargetType = type;
	FadeTime = FadeNum = 0;
};

void TargetParticleObject::AddVertex(const Vector& _vR,int _Color,int _Speed1,int _Speed2)
{
	TargetParticleType* p;
	Vector vCheck;
	int d;

	if(CurrParticle < NumParticle){
		vCheck = Vector(abs(getDistX(_vR.x,R_curr.x)),abs(getDistY(_vR.y,R_curr.y)),0);
		if(vCheck.x > vCheck.y){
			if(vCheck.x > radius) radius = vCheck.x;
		}else{
			if(vCheck.y > radius) radius = vCheck.y;
		};

		p = &Data[CurrParticle];

		vCheck = _vR;
		cycleTor(vCheck.x,vCheck.y);
		p->type = PARTICLE_DIRECT;
		p->Color = _Color;
		p->vR.x = vCheck.x << 8;
		p->vR.y = vCheck.y << 8;
		p->vR.z = vCheck.z << 8;

		vCheck.x = getDistX(R_curr.x,vCheck.x);
		vCheck.y = getDistY(R_curr.y,vCheck.y);
		vCheck.z = R_curr.z - vCheck.z;

		d = vCheck.vabs();
		if(d){
			vCheck.x = _Speed1 * vCheck.x / d;
			vCheck.y = _Speed1 * vCheck.y / d;
			p->vD.x = vCheck.x + vCheck.y;
			p->vD.y = vCheck.y - vCheck.x;
		}else p->vD = Vector(0,0,0);

		p->vD.z = (vCheck.z << 8) / LifeTime;

		p->s = _Speed2;
		p->vT = vTarget;
		p->pDist = d;
		CurrParticle++;
	};
};


void TargetParticleType::aQuant2(void)
{
	int tx,ty;

	tx = SPGetDistX(vT.x,vR.x);
	ty = vT.y - vR.y;

	vR.x += tx / pDist;
	vR.y += ty / pDist;
	vR.z += s;

	vR.x &= PTrack_mask_x;
//	vR.y &= PTrack_mask_y;
	pDist--;

	if (!pDist && type){
		pDist = LifeTime;

		vT = vD;
	}

	G2LQ(Vector(vR.x >> 8,vR.y >> 8,vR.z),tx,ty);
	if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,Color);
};


void TargetParticleType::sQuant2(void)
{
	int tx,ty;

	tx = SPGetDistX(vT.x,vR.x);
	ty = vT.y - vR.y;

	vR.x += tx / pDist;
	vR.y += ty / pDist;
	vR.z += s;

	vR.x &= PTrack_mask_x;
//	vR.y &= PTrack_mask_y;
	pDist--;

	if (!pDist && type){
		pDist = LifeTime;

		vT = vD;
	}

//	tx = round(SPGetDistX(vR.x,SPViewX)*ScaleMapInvFlt) + ScreenCX;
//	ty = round((vR.y - SPViewY)*ScaleMapInvFlt) + ScreenCY;

	tx = ((int)round(SPGetDistX(vR.x,SPViewX) * ScaleMapInvFlt) >> 8) + ScreenCX;
	ty = ((int)round((vR.y - SPViewY) * ScaleMapInvFlt) >> 8)+ ScreenCY;

	if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,Color);
};     

void TargetParticleObject::AddVertex2(const Vector& _vR,const Vector& _vT, int _Color, int _type)
{
	TargetParticleType* p;
	Vector vCheck;
	if(CurrParticle < NumParticle){
		vCheck = Vector(abs(getDistX(_vR.x,R_curr.x)),abs(getDistY(_vR.y,R_curr.y)),0);
		if(vCheck.x > vCheck.y){
			if(vCheck.x > radius) radius = vCheck.x;
		}else{
			if(vCheck.y > radius) radius = vCheck.y;
		};

		p = &Data[CurrParticle];

		vCheck = _vR;
		cycleTor(vCheck.x,vCheck.y);

		p->Color = _Color;
		p->type = _type;
		p->LifeTime = LifeTime;

		if (_type){
			p->vT.x = vCheck.x << 8;
			p->vT.y = vCheck.y << 8;
			p->vT.z = vCheck.z << 8;
		} else {
			p->vR.x = vCheck.x << 8;
			p->vR.y = vCheck.y << 8;
			p->vR.z = vCheck.z << 8;

			p->vD.x = vCheck.x << 8;
			p->vD.y = vCheck.y << 8;
			p->vD.z = vCheck.z << 8;
		}

		vCheck = _vT;
		cycleTor(vCheck.x,vCheck.y);

		if ( _type){
			p->vR.x = vCheck.x << 8;
			p->vR.y = vCheck.y << 8;
			p->vR.z = vCheck.z << 8;

			p->vD.x = vCheck.x << 8;
			p->vD.y = vCheck.y << 8;
			p->vD.z = vCheck.z << 8;
		} else {
			p->vT.x = vCheck.x << 8;
			p->vT.y = vCheck.y << 8;
			p->vT.z = vCheck.z << 8;
		}
		p->pDist = LifeTime;
		if ( _type ) {
			p->pDist >>= 1;
			p->LifeTime >>= 1;
		}
		p->s =((_vR.z  - _vT.z) << 8) / p->pDist;

		CurrParticle++;
	};
};

void WaterParticleObject::InitParicle(int num)
{
	NumParticle = num;
	Data = new SimpleParticleType[NumParticle];
};

void WaterParticleObject::Free(void)
{
	delete[] Data;
};

void WaterParticleObject::Init(StorageType* s)
{
	BaseObject::Init(s);
	ID = ID_EXPLOSION;
};

void WaterParticleObject::Quant(void)
{
	if(DepthShow){
		int xt = getDistX(R_curr.x,ViewX);
		int yy = getDistY(R_curr.y,ViewY);
		int zz = round(A_g2s.a[6]*xt + A_g2s.a[7]*yy) - R_curr.z + ViewZ;
		int xx = round(A_g2s.a[0]*xt + A_g2s.a[1]*yy);
		yy = round(A_g2s.a[3]*xt + A_g2s.a[4]*yy);
		if(zz < -radius || abs(xx) > zz*TurnSideX/ViewZ + radius*2 ||  abs(yy) > zz*TurnSideY/ViewZ + radius*2){
			Status |= SOBJ_DISCONNECT;
			Visibility = UNVISIBLE;
		}else Visibility = VISIBLE;
	}else{
		if(abs(getDistX(R_curr.x,ViewX)) - (radius << 1) < TurnSideX && abs(getDistY(R_curr.y,ViewY)) -  (radius << 1) < TurnSideY) Visibility = VISIBLE;
		else{
			Status |= SOBJ_DISCONNECT;
			Visibility = UNVISIBLE;
		};
	};
//	if(z >= (1 << TOUCH_SHIFT)) VsFlag = oUNVISIBLE;
	if(Time > LifeTime) Status |= SOBJ_DISCONNECT;
	Time++;
};

void WaterParticleObject::DrawQuant(void)
{
	int i;
	SimpleParticleType* p;
	Vector vPos;
	int tx,ty;
	
	if(TargetType){
		if(Time == SetLifeTime){
			if(AdvancedView){
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					p->QuantT(vCenter.x,vCenter.y,Velocity);
					p->dColor = DeltaColor;
					vPos = p->vR;
					vPos >>= 8;
					if(WaterAltLevel(vPos)){
						G2LQ(vPos,tx,ty);
						if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
					};
				};
			}else{
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					p->QuantT(vCenter.x,vCenter.y,Velocity);
					p->dColor = DeltaColor;
					vPos = p->vR;
					vPos >>= 8;
					if(WaterAltLevel(vPos)){
						tx = ((int)round(SPGetDistX(p->vR.x,SPViewX) * ScaleMapInvFlt) >> 8) + ScreenCX;
						ty = ((int)round((p->vR.y - SPViewY) * ScaleMapInvFlt) >> 8)+ ScreenCY;
						if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
					};
				};
			};
		}else{
			if(AdvancedView){
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					p->QuantT(vCenter.x,vCenter.y,Velocity);
					vPos = p->vR;
					vPos >>= 8;
					if(GetAltLevel(vPos)){
						G2LQ(vPos,tx,ty);
						if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
					};
				};
			}else{
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					p->QuantT(vCenter.x,vCenter.y,Velocity);
					vPos = p->vR;
					vPos >>= 8;
					if(GetAltLevel(vPos)){
						tx = ((int)round(SPGetDistX(p->vR.x,SPViewX) * ScaleMapInvFlt) >> 8) + ScreenCX;
						ty = ((int)round((p->vR.y - SPViewY) * ScaleMapInvFlt) >> 8)+ ScreenCY;

						if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
					};
				};
			};
		};
	}else{
		if(Time == SetLifeTime){
			if(AdvancedView){
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					p->Quant();
					p->dColor = DeltaColor;
					vPos = p->vR;
					vPos >>= 8;
					if(WaterAltLevel(vPos)){
						G2LQ(vPos,tx,ty);
						if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
					};
				};
			}else{
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					p->Quant();
					p->dColor = DeltaColor;
					vPos = p->vR;
					vPos >>= 8;
					if(WaterAltLevel(vPos)){
						tx = ((int)round(SPGetDistX(p->vR.x,SPViewX) * ScaleMapInvFlt) >> 8) + ScreenCX;
						ty = ((int)round((p->vR.y - SPViewY) * ScaleMapInvFlt) >> 8)+ ScreenCY;

						if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
					};
				};
			};
		}else{
			if(AdvancedView){
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					p->Quant();
					vPos = p->vR;
					vPos >>= 8;
					if(GetAltLevel(vPos)){
						G2LQ(vPos,tx,ty);
						if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
					};
				};
			}else{
				for(i = 0,p = Data;i < NumParticle;i++,p++){
					p->Quant();
					vPos = p->vR;
					vPos >>= 8;
					if(GetAltLevel(vPos)){
						tx = ((int)round(SPGetDistX(p->vR.x,SPViewX) * ScaleMapInvFlt) >> 8) + ScreenCX;
						ty = ((int)round((p->vR.y - SPViewY) * ScaleMapInvFlt) >> 8)+ ScreenCY;

						if(tx > UcutLeft && tx < UcutRight && ty > VcutUp && ty < VcutDown) XGR_SetPixelFast(tx,ty,p->Color >> 8);
					};
				};
			};
		};
	};
};

void WaterParticleObject::CreateParticle(int _LifeTime,int _SetLifeTime,int _Velocity,int _Radius,int _FirstColor,int _SetColor,int _EndColor,const Vector& v,int _TargetType)
{
	int i;
	int Radius/*,SignRadius*/;
	int SignVelocity,FirstColor,DeltaColor1,SetColor;
	SimpleParticleType* p;

	Time = 0;

	LifeTime = _LifeTime;
	SetLifeTime = _SetLifeTime;

	FirstColor = _FirstColor << 8;
	SetColor = _SetColor << 8;
	DeltaColor1 = (SetColor  - FirstColor) / SetLifeTime ;
	DeltaColor = ((_EndColor << 8) - SetColor) / (LifeTime - SetLifeTime);

//	Radius = _Radius << 8;
//	SignRadius = Radius << 1;

	Velocity = _Velocity;
	TargetType = _TargetType;

	if(TargetType) _Velocity = 0;
	SignVelocity = _Velocity << 1;

	R_curr = v;
	radius = _Radius;
	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
	MapLevel = 1;

	vCenter.x = R_curr.x << 8;
	vCenter.y = R_curr.y << 8;
	vCenter.z = R_curr.z << 8;	

/*	for(i = 0,p = Data;i < NumParticle;i++,p++){
		p->Color = FirstColor;
		p->dColor = DeltaColor1;
		p->vD = Vector(_Velocity - effectRND(SignVelocity),_Velocity - effectRND(SignVelocity),_Velocity - effectRND(SignVelocity));
		p->vR = vPos + Vector(Radius - effectRND(SignRadius),Radius - effectRND(SignRadius),Radius - effectRND(SignRadius));
	};*/

	int phi;
	int radius8 = radius << 8;
	for(i = 0,p = Data;i < NumParticle;i++,p++){
		p->Color = FirstColor;
		p->dColor = DeltaColor1;
		//phi = rPI((RND(16)*PI)>>3);
		phi = rPI(RND(2*PI));
		Radius = RND(radius8);
		//Radius = radius8 - RND(10);
		p->vR = vCenter + Vector(round(Cos(phi)*Radius),round(Sin(phi)*Radius),0);
		p->vD = Vector(_Velocity - effectRND(SignVelocity),_Velocity - effectRND(SignVelocity),_Velocity - effectRND(SignVelocity));
	};
};

int WaterAltLevel(Vector v)
{
	uchar* p = vMap->lineT[v.y];
	uchar* t;
	if(p){
		p += v.x;
		t = p + H_SIZE;
		if((*t) & DOUBLE_LEVEL){
			if(v.x & 1){
				return (GET_TERRAIN_TYPE(*t) == WATER_TERRAIN);
			}else{
				return (GET_TERRAIN_TYPE(*(t + 1)) == WATER_TERRAIN);
			};
		}else return (GET_TERRAIN_TYPE(*t) == WATER_TERRAIN);
	};
	return 0;
};

void FireBallObject::Init(StorageType* s)
{
	BaseObject::Init(s);
	ID = ID_EXPLOSION;
	Scale = 1 << 15;
	dScale = 0;
	Owner = NULL;
};

void FireBallObject::Quant(void)
{
	GetVisible();
	Scale += dScale;
	radius = (FirstRadius * Scale) >> 8;
	if(Owner){
		if(Owner->Status & SOBJ_DISCONNECT) Owner = NULL;
		else{
			R_curr = Owner->R_curr;
			R_curr.z += EXPLOSION_OFFSET;
		};
	};
	if(Visibility != VISIBLE){
		if(FBP->CheckOut(frame)) Status |= SOBJ_DISCONNECT;
	}else MapLevel = 1;
};

void FireBallObject::DrawQuant(void)
{
	int tx,ty,s;
	if(AdvancedView) s = G2LF(R_curr,tx,ty);
	else s = G2LS(R_curr,tx,ty);
	s = s * Scale;
//	FBP->Show(tx,ty,R_curr.z,s << 7,frame);
	FBP->Show(tx,ty,R_curr.z,s,frame);
	if(FBP->CheckOut(frame)) Status |= SOBJ_DISCONNECT;
};

void FireBallObject::CreateFireBall(const Vector& v,BaseObject* _owner,int _scale,int _dscale,char _type)
{
	Owner = _owner;
	if(Owner) R_curr = Owner->R_curr;
	else R_curr = v;
	R_curr.z += EXPLOSION_OFFSET;
	Scale = _scale;
	dScale = _dscale;
	cycleTor(R_curr.x,R_curr.y);
	Status = 0;
	GetVisible();
	MapLevel = 1;
	FBP = &EffD.FireBallData[_type];
	frame = 0;
	FirstRadius = FBP->x_size;
	radius = (FirstRadius * Scale) >> 8;
	if(FBP->cTable == 0)
		MapD.CreateLight(R_curr.x,R_curr.y,R_curr.z,radius,32,LIGHT_TYPE::STATIONARY);
};


void ParticleGenerator::CreateGenerator(Vector vC,Vector vT,Vector vD,int mode)
{
	switch(mode){
		case PG_STYLE_INCAR:
			R_curr = vC;
			cycleTor(R_curr.x,R_curr.y);
			ParticleStorage = EFF_PARTICLE07;
			ParticleType = 7;
			vTarget = vT;
			vDelta = vD;
			Speed = 15;
			MoveMode = PG_MODE_MOVE_TARGET;
			TargetMode = PG_RADIUS;
			Time = 300;
			FlyRadius = 100;
			radius = FlyRadius;
			Precision = 5;
			GetVisible();
			break;
		case PG_STYLE_ENTER:
			R_curr = vC;
			cycleTor(R_curr.x,R_curr.y);
			ParticleStorage = EFF_PARTICLE07;
			ParticleType = 8;
			vTarget = vT;
			vDelta = vD;
			Speed = 8;
			MoveMode = PG_MODE_TRUE_MASS;
			TargetMode = PG_RADIUS;
			Time = 40;
			FlyRadius = RND(50);
			radius = FlyRadius;
			Precision = 5;
			GetVisible();
			break;
		case PG_STYLE_REINCAR:
			R_curr = vC;
			cycleTor(R_curr.x,R_curr.y);
			ParticleStorage = EFF_PARTICLE07;
			ParticleType = 7;
			vTarget = vT;
			vDelta = vD;
			Speed = 5;
			MoveMode = PG_MODE_TRUE_MASS;
			TargetMode = PG_TARGET_RANDOM | PG_TARGET_RIGHT | PG_TARGET;
			Time = CHANGE_VANGER_TIME;
			FlyRadius = 100;
			radius = FlyRadius;
			Precision = 2;
			GetVisible();
			break;
	};
	R_prev = R_curr;
	Status = 0;
};

void ParticleGenerator::Init(StorageType* s)
{
	BaseObject::Init(s);
	ID = ID_EXPLOSION;
};

void ParticleGenerator::Quant(void)
{
	uchar alt;
	Vector vT;
	int d;

	vT = Vector(0,0,0);

	if(TargetMode & PG_RADIUS){
		vT = Vector(getDistX(vTarget.x,R_curr.x),getDistY(vTarget.y,R_curr.y),0);
		d =  vT.vabs();
		vT *= d - FlyRadius;
		vT /= d;
		d = vT.y;
		vT.y +=  vT.x;
		vT.x -= d;
	};

	if(TargetMode & PG_TARGET)
		vT += Vector(getDistX(vTarget.x,R_curr.x),getDistY(vTarget.y,R_curr.y),0);

	if((TargetMode & PG_TARGET_RANDOM) && RND(100) < 50)
		vT += Vector(BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2),BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2),BMAX_TARGET_VECTOR - RND(BMAX_TARGET_VECTOR2));

	if(TargetMode & PG_TARGET_RANDOM)
		vT += Vector(vDelta.y,-vDelta.x,0);
	if(TargetMode & PG_TARGET_RIGHT)
		vT += Vector(-vDelta.y,vDelta.x,0);

	if(TargetMode & PG_WALL){
		vT += Vector(getDistX(vTarget.x,R_curr.x),getDistY(vTarget.y,R_curr.y),vTarget.z - R_curr.z);
		if(vT.abs2() == radius*radius) Speed = 0;
	};

	d = vT.vabs();
	if(d){
		vDelta += vT * Precision / d;
		d = vDelta.vabs();
		if(!(MoveMode & PG_MODE_TRUE_MASS) || d > Speed) vDelta = vDelta * Speed / d;
	};

	R_prev = R_curr;
	R_curr += vDelta;
	cycleTor(R_curr.x,R_curr.y);	

	Time--;
	GetAlt(R_curr,alt);
	R_curr.z = alt + radius;
	if(Time <= 0) Status |= SOBJ_DISCONNECT;
};

void ParticleGenerator::DrawQuant(void)
{
	EffD.CreateParticle(ParticleType,R_prev,R_curr,ParticleStorage);
};

void EffectDispatcher::CreateParticleGenerator(Vector vC,Vector vT,Vector vD,int mode)
{
	ParticleGenerator* pg;
	pg = (ParticleGenerator*)(UnitStorage[EFF_PARTICLE_GENERATOR].Active());
	if(pg){		
		pg->CreateGenerator(vC,vT,vD,mode);
		ConnectTypeList(pg);
		GameD.ConnectBaseList(pg);
	};
};
