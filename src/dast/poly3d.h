#ifndef __DAST__POLY3D_H__
#define __DAST__POLY3D_H__

#include "../3d/3d_math.h"
#include "../terra/vmap.h"
#include "../terra/world.h"
#include "../terra/render.h"
  
struct dastResourcePoly3D;
struct dastResourceSign;
extern dastResourcePoly3D *dastResource;
extern dastResourceSign *dastResSign;

struct DAST_SIGN_TYPE{
	enum{
		MOVE = 0,
		DOWN,
		UP
	};
};

struct dastPoly3D{
	Vector *p, *p_array;
	int n;
	int count;

	dastPoly3D( Vector lp, Vector rp, Vector dv );
	dastPoly3D(  Vector, int );
	dastPoly3D(  Vector, int, int );
	~dastPoly3D( void );

	void set( Vector lp, Vector rp, Vector dv );
	void set( Vector, int );
	void set(  Vector _p, int _n, int _count );

	void make_dast( void );
	int quant_make_sign(void);
	void make_mole(dastResourcePoly3D* res = dastResource );
	int make_first_mole(dastResourcePoly3D* res = dastResource );
	int make_last_mole(dastResourcePoly3D* res = dastResource );
	int make_catch_dolly( dastResourcePoly3D* res = dastResource  );
};

struct dastResourcePoly3D{
	short x_size, y_size;
	short n;
	uchar **data;

	dastResourcePoly3D(const char*);
	~dastResourcePoly3D(void);
};

struct dastResourceSign{
	uchar n;
	uchar *poly;
	uchar *once;
	char **x;
	char **y;
	char **type;
	uchar **wide;

	dastResourceSign(const char*);
	~dastResourceSign(void);
};


extern uchar *sqr3_matr;

void precalc_sqr3_matr(void);
uchar sqr3( int x );

extern uchar *sqr3_matr;
extern int MOVER_ON;

uchar* get_down_ground( int x, int y );
uchar* get_up_ground( int x, int y );

inline uchar* get_real_ground( int where, int x, int y )
{
	uchar * lt = vMap -> lineT[y & clip_mask_y] + x;

	//if ( !lt ) ErrH.Abort( "DAST : not it line" );

	if(*(lt + H_SIZE) & DOUBLE_LEVEL){ 
		if ( where ){
			if((x & 1)) return (--lt);
		} else {
			if(!(x & 1)) return (++lt);
		}
	} 
	return (lt) ;
}

void dastCreateResource(const char* FileName);
void dastCreateResourceSign(const char* FileName);

void dastDeleteResource(void);
void dastDeleteResourceSign(void);

void dastPutSpriteOnMapAlt(int x, int y, uchar *data, int x_size, int y_size, unsigned int scale);
void dastPutSandOnMapAlt(int x, int y, uchar _z, uchar, uchar);

#endif