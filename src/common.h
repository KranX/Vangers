#ifndef __COMMON_H__
#define __COMMON_H__


#define MIN(a,b)	(((a) < (b))?(a):(b))
#define MAX(a,b)	(((a) > (b))?(a):(b))

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

const uint MAP_POWER_X = 11;
const uint map_size_x = 1 << MAP_POWER_X;
const uint clip_mask_x = map_size_x - 1;
const uint TOR_POWER = 1;
const uint TOR_XSIZE = TOR_POWER*map_size_x;

extern uint MAP_POWER_Y;
extern uint map_size_y;
extern uint clip_mask_y;
extern uint iclip_mask_y;
extern uint clip_mask;
extern uint TOR_YSIZE;

#define XCYCL(x)	((x) & clip_mask_x)
#define YCYCL(y)	((y) & clip_mask_y)
#define iXCYCL(x)	((x) & clip_mask_x)
#define iYCYCL(y)	((y) & iclip_mask_y)

#ifdef _SURMAP_
const int MAX_RADIUS = 175;
#else
const int MAX_RADIUS = 48;
#endif

const int UNIT = 0x10000;	//32768;
const int SHIFT = 16;

const int PIx2 = 1 << 12;
const int PI = PIx2/2;
const int locPI = PIx2/2;

#define rPI(a)	(((a)) & (PIx2 - 1))

extern int* SI;
extern int* CO;

inline int COS(int x) { return CO[rPI(x)]; }
inline int SIN(int x) { return SI[rPI(x)]; }

#undef random
#define random(num) (int)RND(num)

#define RNDSIGN  ((!RND(2)) ? -1 : 1)

extern const char* mapFName;

extern int ColorShow;
extern unsigned char* palbuf,*palbufA,*palbufC;

extern unsigned RNDVAL;
inline unsigned RND(unsigned m)
{
	RNDVAL ^= RNDVAL >> 3;
	RNDVAL ^= RNDVAL << 28;
	RNDVAL &= 0x7FFFFFFF;

	if(!m) return 0;
	//std::cout<<"m:"<<m<<" r:"<<RNDVAL%m<<std::endl;
	return RNDVAL%m;
}

//extern XBuffer buffer_out;
//#define RND(m)  (buffer_out < __FILE__ < "\t" <= __LINE__ < "\t" <= RNDVAL < "\n",_RND(m))

/*extern unsigned realRNDVAL;
inline unsigned realRND(unsigned m)
{
	realRNDVAL ^= realRNDVAL >> 3;
	realRNDVAL ^= realRNDVAL << 28;
	realRNDVAL &= 0x7FFFFFFF;

	if(!m) return 0;
	std::cout<<"m:"<<m<<" r:"<<realRNDVAL%m<<std::endl;
	return realRNDVAL%m;
}*/
inline unsigned realRND(unsigned m) {
	if (!m) {
		return 0;
	}
	return rand()%m;
}


inline void cycleTor(int& x,int& y)
{
	x &= TOR_XSIZE - 1;
	y &= TOR_YSIZE - 1;
}

#define PCSIDE	   64
#define PC_MAX	   2*PCSIDE + 1

#define mSQRT(a,b)	sqrt((a)*(double)(a) + (b)*(double)(b))
#define mATAN2(a,b)	rPI((int)(atan2((double)-b,(double)a)*PIx2/(2.*_PI)))

extern void* _video;

#define EXTERNAL_USE

char* GetTargetName(const char* name);

#define UP_LAYER	0
#define DOWN_LAYER	1

#define COL1		255
#define COL2		252
#define COL3		250
#define COL4		249
#define COL5		251

#ifdef _ROAD_
#define CLOCK()		(SDL_GetTicks()*18/1000)

const int SQ_SYSCOLOR = 256 - 8;
const int SYSCOLOR = 159;

struct sqFont {
	short num;
	unsigned char first,last;
	short sx,sy;
	void** data;

	void init(void* d);
	void draw(int x,int y,unsigned char* s,int fore = SQ_SYSCOLOR,int back = SQ_SYSCOLOR + 5);
	void draw(int x,int y,char* s,int fore = SYSCOLOR,int back = -1){ draw(x,y,(unsigned char*)s,fore,back); }
	void drawtext(int x,int y,char* s,int fore = SQ_SYSCOLOR,int back = SQ_SYSCOLOR + 5);
	void drawchar(int x,int y,int ch,int fore = SQ_SYSCOLOR,int back = SQ_SYSCOLOR + 5);
	};
#endif
#ifdef _DEBUG
extern int __GlobalSize__;
extern  XStream fmemory;
#define _MEM_STATISTIC_(a) fmemory < a <= __GlobalSize__ < "\tclock\t" <= CLOCK() < "\n";
#else
#define _MEM_STATISTIC_(a) ;
#endif

#endif
