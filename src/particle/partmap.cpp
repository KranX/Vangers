#include "../global.h"
#include "../runtime.h"
#include "../3d/3d_math.h"

#include "../common.h"

#ifdef _DEBUG
#include "../win32f.h"
#endif

#include "../sqexp.h"
#include "../backg.h"

#include "../terra/vmap.h"
#include "../terra/world.h"
#include "../terra/render.h"
#include "../dast/poly3d.h"

#include "particle.h"
#include "partmap.h"
#include <string.h>

#undef random
#define random(a) BogusRND(a)

#ifdef __HIGHC__
#ifndef EXTERNAL_USE
#ifndef random
	#define random(a) ((rand() >> 16)*(a) >> 15)
#endif
#endif
#endif


Mask* ParticleMapProcessMaskArray = 0;
int ParticleMapProcessMaskTotal = 0;

unsigned char *FirePaletteTable;
#define random1(a) (rand() & 1 ? random(a) : -random(a))

void color_line_f(int len,unsigned char* dbuf,int bx,int bKx,int fx,int fy);
void transparency_line_f(int len,unsigned char* dbuf,int bx,int bKx,int fx,int fy);
void smart_putspr_f(unsigned char* data,int Xcenter,int Ycenter,int XsizeB,int YsizeB,int ScaleXsize,int height = 255);

void processParticleMapLineWithNoMask(unsigned char*& f1p,unsigned char*& f2p,int Xmax);
void processParticleMapLineWithMask(unsigned char*& f1p,unsigned char*& f2p,int Xmax,TABLE_TYPE*& moff);
void bitmap2screen(char* vb,unsigned char* data,int Xcenter,int Ycenter,int XsizeB,int YsizeB,int ScaleXsize,int turn);

//#define SAVE_EXPLOSION

#ifdef SAVE_EXPLOSION
int ExplosionSaveNumber = 0;
void SaveExplosion(int xs,int ys,uchar* b);
#endif

void ParticleMapProcess::init_flame(unsigned char* f,int fade)
{
	memset(f,0,Xmax);
	int i,r,a,x,y,tmp;

	if(fade){
		int tmp = Xmax*Ymax;
		unsigned char* ff = f;
		for(i = 0;i < tmp;){
			*ff = 0;
			*(ff + 1) = 0;
			*(ff + Xmax) = 0;
			*(ff + Xmax + 1) = 0;
			int inc = 1 + random(FadePower);
			ff += inc;
			i += inc;
		}//  END FOR I
		return;
	}//  end if

	int i_hc = 0;
	int curr_i_term = 0;
	int ccx = 0;
	int ccy = 0;
	for(i = 0;i < CurrNhotSpots;i++){
			if(i == curr_i_term){
				ccx =  HotCentersX[i_hc];
				ccy =  HotCentersY[i_hc];
				i_hc++;
				curr_i_term = i_hc*CurrNhotSpots/NhotCenters;
				}//  end if

//			  if(i == cns1){
//				  ccx = 0;
//				  ccy = Xmax/8;
//				  }
//			  if(i == cns2){
//				  ccx = -Xmax/8;
//				  ccy = -Xmax/16;
//				  }
			r = random(HotSpotRadius);
			a = random(360);
			x = (int)(ccx + r*cos(a*2*3.1415926/360));
			y = (int)(ccy + r*sin(a*2*3.1415926/360));
			switch(ProcedureType){
				case 1:
					tmp = random(2*(r)*WHITE_POINT/HotSpotRadius);
					break;
				case 2:
					tmp = random(4*(HotSpotRadius/2 - abs(r - HotSpotRadius/2))*WHITE_POINT/HotSpotRadius);
					break;
				case 3:
					tmp = random(WHITE_POINT);
					break;
				default:
					tmp = random(2*(HotSpotRadius - r)*WHITE_POINT/HotSpotRadius);
				}//  end case
			f[x + (Xmax>>1) + Xmax*(y + (Ymax>>1))] = tmp;
			}//  end for i
}

#define P(a,b) int((f1p)[(a) + (b)])

int CurrDev = 0;

#ifdef EXTERNAL_USE
extern int frame;
#endif

int ParticleMapProcess::process(char* vb,int Xc,int Yc,int XcS,int turn,int noout, int height, int Xreal, int Yreal){
	XcS = XcS*Xmax >> 15;
	if(phase){
		f1 = field1;
		f2 = field2;
		}
	else{
		f1 = field2;
		f2 = field1;
		}
	phase = !phase;

	 uchar l1;
	 uchar l2;

	if ( vMap -> lineT[Yreal] ){
		l1 = *(get_real_ground(0, Xreal, Yreal));
		l2 = *(get_real_ground(1, Xreal, Yreal));
	} else {
		l1 = l2 = 255;
	}

	height += ProcessTime<<3;
	if ( abs( l1 - height) > abs(l2 - height)){
		if ( height >= l1) height = l1;
	}
	int x,y;
	unsigned char* f1p,*f2p;
	f1p = f1;
//	unsigned char* bp = ColorBuf;
//	  double ph = ProcessTime*2*_PI/DeviationPeriod;
	double ph;
	if(DeviationPeriod)
		ph = 2*M_PI*(frame%DeviationPeriod)/DeviationPeriod;
	else
		ph = (2*M_PI*RND(1025))/1024.0;
	CurrDev = int(DeviationRadius*cos(ph)) + int(DeviationRadius*sin(ph))*Xmax;

	if(PrevCurrDev > CurrDev){
		}

	PrevCurrDev = CurrDev;

#ifndef EXTERNAL_USE
	if(!NoFlush)
#endif
	if(!noout)
		//bitmap2screen(vb,f1,Xc,Yc,Xmax,Ymax,XcS,turn);
#ifdef SAVE_EXPLOSION
		SaveExplosion(Xmax,Ymax,f1);
#else
		smart_putspr_f(f1,Xc,Yc,Xmax,Ymax,XcS, height);
#endif
	int i = 0;
	int fade = 1;
	ProcessTime++;
	if(ProcessTime < AttackTime){
		HotSpotRadius = (HotSpotArea*(ProcessTime)/AttackTime)/2 + 1;
		CurrNhotSpots = NhotSpots*ProcessTime/AttackTime*ProcessTime/AttackTime;
		init_flame(f1);
		fade = 0;
		}
	else if(ProcessTime < AttackTime + LifeTime){
		HotSpotRadius = HotSpotArea/2 + 1;
		init_flame(f1);
		fade = 0;
		}
	else if(ProcessTime < AttackTime + LifeTime + FadeTime){
		fade = 1;
		init_flame(f1,1);
		}
	else if(ProcessTime >= AttackTime + LifeTime + FadeTime){
		ProcessTime = 0;
		}

	int sh = 3*Xmax;
//	  memcpy(f1 - 3*Xmax,f1,Xmax*3);

	f1p = f1 + CurrDev;
//	  f2p = f2 + random(Xmax/10) + random(Xmax/10)*Xmax;
	f2p = f2 + CurrDev;
	TABLE_TYPE* moff = Motion -> offset;
	if(fade){
		f1p += Xmax;
		f2p += Xmax;
		/*for(x = 0;x < Xmax;x++){
			f1p++;
			f2p++;
			}*/
		for(y = 1;y < Ymax - 1;y++){
//#define ASM_OPTIMIZE
#ifdef ASM_OPTIMIZE
			processParticleMapLineWithNoMask(f1p,f2p,Xmax);
#else
			for(x = 0; x < Xmax; x++){
				unsigned char* ff1 = f1p - Xmax - 1;
				int tmp = *ff1;
				ff1 ++;				tmp += *ff1;
				ff1 ++;				tmp += *ff1;
				ff1 += Xmax;  tmp += *ff1;
				ff1 --;				tmp += *ff1;
				ff1 --;				tmp += *ff1;
				ff1 += Xmax;  tmp += *ff1;
				ff1 ++;				tmp += *ff1;
				tmp >>= 3;
				*f2p = tmp;
				f2p++;
				f1p++;
				}
#endif
			}
	}else{
		f2p += Xmax;

		for(x = 0;x < Xmax;x++){
			f1p += *moff++;
	//		f2p++;
			}
		for(y = 1;y < Ymax - 1;y++){
#ifdef ASM_OPTIMIZE
			processParticleMapLineWithMask(f1p,f2p,Xmax,moff);
#else
			for(x = 0;x < Xmax;x++){
				unsigned char* ff1 = f1p - Xmax - 1;
				int tmp = *ff1;
				ff1 ++;				tmp += *ff1;
				ff1 ++;				tmp += *ff1;
				ff1 += Xmax;	tmp += *ff1;
				ff1 --;					tmp += *ff1;
				ff1 --;					tmp += *ff1;
				ff1 += Xmax;	tmp += *ff1;
				ff1 ++;				tmp += *ff1;
//				  tmp =  P(-Xmax,-1) + P(-Xmax,0) + P(-Xmax,1) +
//					  P(	0,-1) +P(0,0) + P(    0,1) +
//					  P( Xmax,-1) + P( Xmax,0);
				tmp >>= 3;
				*f2p = tmp;
				f2p++;
				f1p += *moff++;
				}
#endif
			}
		}
	
	return !ProcessTime;
}

void ParticleMapProcess::activate()
{
	activate(Type - ParticleMapProcessTypeArray);
}
void ParticleMapProcess::activate(int type)
{
	ParticleMapProcessType* t = ParticleMapProcessTypeArray + type;
	if(t -> Xmax != Xmax)
		ErrH.Abort("Initialisation and activation type X sizes mismatch.");
	if(t -> Ymax != Ymax)
		ErrH.Abort("Initialisation and activation type Y sizes mismatch.");
	Type = t;
	activate(Type -> NhotSpots,Type -> HotSpotArea,Type -> AttackTime,Type -> LifeTime,Type -> FadeTime,Type -> FadePower,Type -> DeviationRadius,Type -> DeviationPeriod,Type -> ProcedureType,Type -> HotCentersRadius,type);
}
void ParticleMapProcess::activate(int n_hot_spots,int hot_spot_area,int attackT,int lifeT,int fadeT,int fadeP,int dR,int dP,int pType,int HCR,int type)
{
	NhotSpots = n_hot_spots;
	HotSpotArea = hot_spot_area;
	AttackTime = attackT; // not added coefficient due buggy flash on explode start animation, possibly could be fixed in init_flame() function
	LifeTime = (int)round(lifeT * GAME_TIME_COEFF);
	FadeTime = (int)round(fadeT * GAME_TIME_COEFF);
	FadePower = fadeP;
	DeviationRadius = dR;
	DeviationPeriod = dP;
	ProcedureType = pType;
	HotCentersRadius = HCR;

	Type = ParticleMapProcessTypeArray + type;
	ProcessTime = 0;

	//memset(WorkArea,0,2*Xmax*(Ymax + 2*PARTICLE_MAP_GAP));
	memset(WorkArea,0,2*Xmax*(Ymax + PARTICLE_MAP_GAP));
	PrevCurrDev = 0;
	int a,b;
	for(int i = 0;i < GLOBAL_N_HOT_CENTERS;i++){
		a = random(360);
		b = random(HotCentersRadius);
		HotCentersX[i] = (int)(HotCentersRadius*cos(a*2*3.1415926/360));
		HotCentersY[i] = (int)(HotCentersRadius*sin(a*2*3.1415926/360));
		}
}
void ParticleMapProcess::init(int type)
{
	Type = ParticleMapProcessTypeArray + type;
	init(Type -> Xmax,Type -> Ymax,type);
}

void ParticleMapProcess::PickUpMask()
{
	int i;
	if(ParticleMapProcessMaskArray == 0){
		ParticleMapProcessMaskArray = new Mask[MAX_MASK_TOTAL];
		ParticleMapProcessMaskTotal = 0;
		}
	for(i = 0;i < ParticleMapProcessMaskTotal;i++){
//		if(!strcmp(ParticleMapProcessMaskArray[i].MSSfile,Type -> MSSfile,32)){
		if(!strcmp(ParticleMapProcessMaskArray[i].MSSfile,Type -> MSSfile)){
			Motion = ParticleMapProcessMaskArray + i;
			return;
			}
		}
	if(i >= MAX_MASK_TOTAL)
		ErrH.Abort("ParticleMapProcessMaskArray exhausted.");
	ParticleMapProcessMaskTotal++;
	Motion = ParticleMapProcessMaskArray + i;
//	  char* n = "0000000.mss";
//	  sprintf(n,"%03ix%03i.mss",Xmax,Xmax);
//	  Motion -> load(n);
	Motion -> load_to_flame(Type -> MSSfile);
}


void ParticleMapProcess::init(int x_max,int y_max,int type)
{
	Xmax = x_max;
	Ymax = y_max;

	phase = 0;
	Type = ParticleMapProcessTypeArray + type;
	PickUpMask();
	WorkArea = new unsigned char[2*Xmax*(Ymax + PARTICLE_MAP_GAP)];

	field1 = WorkArea + PARTICLE_MAP_GAP*Xmax;
	field2 = WorkArea + Xmax*Ymax + PARTICLE_MAP_GAP*Xmax;

	memset(field1,0,Xmax*Ymax);
	memset(field2,0,Xmax*Ymax);

//	  Motion -> load("partmap.mss");
	if(Motion -> mx != Xmax)
		ErrH.Abort("Mask X size != field X size (%i != %i).");
	if(Motion -> mx != Motion -> bx)
		ErrH.Abort("Mask X size != mask base X size (%i != %i).");
	PrevCurrDev = 0;

	NhotCenters = GLOBAL_N_HOT_CENTERS;
}


void ParticleMapProcess::finit()
{
	if(WorkArea) {
		delete[] WorkArea;
	}
	WorkArea = NULL;
	finitMask();
}

void ParticleMapProcess::finitMask( void ){
	int i;

	if( !ParticleMapProcessMaskArray ) return;

	for ( i = 0; i < MAX_MASK_TOTAL; i++)
		ParticleMapProcessMaskArray[i].finit();

	delete[] ParticleMapProcessMaskArray;
	ParticleMapProcessMaskArray = NULL;
}

void FirePaletteInit(unsigned char* pal)
{
	int first_col = FIRE_COLOR_FIRST;
	int i;
	const int NULL_INTENS = 20;
	for(i = 0;i < RED_POINT;i++){
		pal[3*(i + first_col)] = NULL_INTENS + (64 - NULL_INTENS)*i/RED_POINT;
		pal[3*(i + first_col) + 1] = 0;
		pal[3*(i + first_col) + 2] = 0;
		}
	for(;i < YELLOW_POINT;i++){
		pal[3*(i + first_col)] = 63;
		pal[3*(i + first_col) + 1] = (64*(i - YELLOW_POINT)/(YELLOW_POINT - RED_POINT)) & 63;
//		pal[3*(i + first_col) + 1] = (64*(i - YELLOW_POINT)/(YELLOW_POINT - RED_POINT));
		pal[3*(i + first_col) + 2] = 0;
		}
	for(;i < WHITE_POINT;i++){
		pal[3*(i + first_col)] = 63;
		pal[3*(i + first_col) + 1] = 63;
		pal[3*(i + first_col) + 2] = (64*(i - WHITE_POINT)/(WHITE_POINT - YELLOW_POINT)) & 63;
//		pal[3*(i + first_col) + 2] = (64*(i - WHITE_POINT)/(WHITE_POINT - YELLOW_POINT));
		}
	i = WHITE_POINT - 1;
	pal[3*(i + first_col)] = pal[3*(i + first_col) + 1] = pal[3*(i + first_col) + 2] = 63;
/*
	int middle = first_col + WHITE_POINT/2;
	const BLACK_RAD = 2;
	for(int j = 0;j <= BLACK_RAD;j++){
		pal[3*(middle + j)] =	  pal[3*(middle + BLACK_RAD)]*j/BLACK_RAD;
		pal[3*(middle + j) + 1] = pal[3*(middle + BLACK_RAD) + 1]*j/BLACK_RAD;
		pal[3*(middle + j) + 2] = pal[3*(middle + BLACK_RAD) + 2]*j/BLACK_RAD;

		pal[3*(middle - j)] =	  pal[3*(middle - BLACK_RAD)]*j/BLACK_RAD;
		pal[3*(middle - j) + 1] = pal[3*(middle - BLACK_RAD) + 1]*j/BLACK_RAD;
		pal[3*(middle - j) + 2] = pal[3*(middle - BLACK_RAD) + 2]*j/BLACK_RAD;
		}
*/
}
void FirePaletteTableInit(unsigned char *pal)
{
	/*for(int i = 0;i < 256;i++){

		int Y = ((19595*(int)pal[3*i] + 38470*(int)pal[3*i + 1] + 7471*(int)pal[3*i + 2])>>17);

		for(int j = 0; j < 256; j++){
			int A = ((19595*(int)pal[3*j] + 38470*(int)pal[3*j + 1] + 7471*(int)pal[3*j + 2])>>16);

			if( A >= 32 ) A = 31;
			FirePaletteTable[i + (j << 8)] = A;

			//	if ( ((Y + A ) >> 1) >= FIRE_PROCESS_COLOR_MAX )
			//		FirePaletteTable[i + (j << 8)] = FIRE_PROCESS_COLOR_MAX-1;
			//	else
			//		FirePaletteTable[i + (j << 8)] = (Y + A)>>1;
		}  //  end for j
	}//  end for i*/
}

/*void processParticleMapLineWithNoMask(unsigned char*& f1p,unsigned char*& f2p,int Xmax)
{
	int x;
	for(x = 0;x < Xmax;x++){
		unsigned char* ff1 = f1p - Xmax - 1;
		int tmp = *ff1;
		ff1 ++;tmp += *ff1;
		ff1 ++;tmp += *ff1;
		ff1 += Xmax;tmp += *ff1;
		ff1 --;tmp += *ff1;
		ff1 --;tmp += *ff1;
		ff1 += Xmax;tmp += *ff1;
		ff1 ++;tmp += *ff1;
		tmp >>= 3;
		*f2p = tmp;
		f2p++;
		f1p++;
		}
}


void processParticleMapLineWithMask(unsigned char*& f1p,unsigned char*& f2p,int Xmax,TABLE_TYPE*& moff)
{
	int x;
	for(x = 0;x < Xmax;x++){
		unsigned char* ff1 = f1p - Xmax - 1;
		int tmp = *ff1;
		ff1 ++;tmp += *ff1;
		ff1 ++;tmp += *ff1;
		ff1 += Xmax;tmp += *ff1;
		ff1 --;tmp += *ff1;
		ff1 --;tmp += *ff1;
		ff1 += Xmax;tmp += *ff1;
		ff1 ++;tmp += *ff1;
		tmp >>= 3;
		*f2p = tmp;
		f2p++;
		f1p += *moff++;
		}
}*/
/*
void FirePaletteTableInit(unsigned char *pal)
{
//	FirePaletteTable = new unsigned char[256 << 8];

	for(int i = 0;i < 256;i++){

		//int I = ((int)pal[3*i] + (int)pal[3*i + 1] + (int)pal[3*i + 2])/3;

		int I = ((19595*(int)pal[3*i] + 38470*(int)pal[3*i + 1] + 7471*(int)pal[3*i + 2])>>16)/3;
		//I= I >= FIRE_PROCESS_COLOR_MAX ? FIRE_PROCESS_COLOR_MAX - 1 : I;
		//I >>= 1;

		for(int j = 0;j < 256;j++){
			int A = j >= FIRE_PROCESS_COLOR_MAX ? FIRE_PROCESS_COLOR_MAX - 1 : j;

//			  int P = (I* + A) >> 1;
			int P ;//= (I*(FIRE_PROCESS_COLOR_MAX - A) + A*A)/FIRE_PROCESS_COLOR_MAX;
			P = I + A >> 1;
			if(P >= FIRE_PROCESS_COLOR_MAX)
				P = FIRE_PROCESS_COLOR_MAX - 1;
//			  P = A;
			if(!P) P = i;
			FirePaletteTable[i + (j << 8)] = P;
			I++;
		}  //  end for j
	}//  end for i
}*/


#ifdef SAVE_EXPLOSION
void SaveExplosion(int xs,int ys,uchar* b)
{
	XBuffer fn;
	XStream fout;
	int i;
	fn < "Expl" <= ExplosionSaveNumber < ".bmp";
	fout.open((char*)(fn),XS_OUT);

	fout < (short)(xs);
	fout < (short)(ys);
	for(i = 0;i < xs*ys;i++)
		fout < b[i];
	fout.close();

	ExplosionSaveNumber++;
};
#endif
