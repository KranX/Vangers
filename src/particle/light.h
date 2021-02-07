#ifndef __PARTICLE__LIGHT_H__
#define __PARTICLE__LIGHT_H__

#include "../3d/3dobject.h"
#include "../backg.h"

#define LIGHT_HEIGHT 100
#define LIGHT_LIFE_TIME 16
#define LIGHT_FIRST_TIME 2
#define LIGHT_MAX_RADIUSE 256
#define LIGHT_MAX_BRIGHTNESS 32

#define CHAR_SPEED 2

struct LIGHT_TYPE
{
	enum{
		STATIONARY = 1,
		DYNAMIC = 2,
		STATIC = 3,
		TOR = 4
	};
};

struct LandQuake{
	int x, y;
	int Rmin, Rmax, Rcur;
	int dx;

	LandQuake(void){Rmax = 0; Rcur = 1;};
	~LandQuake(void){};

	void set(int _x, int _y, int _Rmin, int _Rmax, int _dx = 0);
	int quant(int);
};

struct LightPoint : GeneralObject, BackgroundElement
{
	int type;
	int x, y;
	int xn, yn;
	unsigned int x_size, y_size;
	int c_time;
	int energy;
	int d_energy;
	int maxR;
	int last_z;
	uchar *buf;
	uchar *bool_buf;

	void load_color_line( void );
	void restore_color_line( void );
	void set_position(int _x,int _y,int _z);
	int quant(void);
	void BackRestore(void);

	void FourLightLine( int, int, int );
	void CreateLight(int _r,int _e,int _type);
	void Quant(void);

	void Free(void);
	void Init(StorageType* s);
	void MemInit(int R = 64);
	void Destroy(void);
};

void PrepareLight( void );
void aWriteHelpString(int count, int* x, int* y, int font, unsigned char **pstr, int timer, int color, int space, unsigned int *pcolor);

extern int *light_table;
extern uchar *invisible_table;
extern LightPoint LPoint;

#endif