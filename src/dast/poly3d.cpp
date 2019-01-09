
#include "../global.h"


#define _DAST_
//#define UPMECH 24

#include "../common.h"
//#include "..\terra\world.h"

#include "../sqexp.h"
#include "poly3d.h"

extern uchar DestroyMoleTable[TERRAIN_MAX];
#define BREAKABLE_TERRAIN(prop)		(DestroyMoleTable[GET_TERRAIN((prop))] > 127)

//-----------------------------------------------------------------
extern iGameMap* curGMap;

//-----------------------------------------------------------------

//----------------------------------------------------------------
//extern vrtMap* vMap;
dastResourcePoly3D *dastResource;
dastResourceSign *dastResSign;


//uchar *sqr3_matr;
//int max_len_mech = 64;
int MOVER_ON = 1;

uchar*  (*pf)( int x, int y );

inline uchar* get_down_ground( int x, int y )
{
	uchar * lt = vMap -> lineT[y & clip_mask_y] + x;

//	if ( !lt ) ErrH.Abort( "DAST : not it line" );

	if(*(lt + H_SIZE) & DOUBLE_LEVEL){ 
			if((x & 1)) return (--lt);
	} 
	return (lt) ;
}

inline uchar* get_up_ground( int x, int y )
{
	uchar * lt = vMap -> lineT[y & clip_mask_y] + x;

//	if ( !lt ) ErrH.Abort( "DAST : not it line" );

	if(*(lt + H_SIZE) & DOUBLE_LEVEL){ 
			if(!(x & 1)) return (++lt);
	} 
	return (lt) ;
}


/*uchar sqr3( int x ){
	uchar i;

	//if ( x >= 512 ) ErrH.Abort( "DAST : greater when table sqrt3" );
	if ( x >= 512 ) return 8;
	
	//if ( !x ) return 0;

	while ( x >= 512 ){
		i  <<= 3;
		x  >>= 9;
	}
	i = sqr3_matr[x];

	return i;
}

void precalc_sqr3_matr(void){
	int i, j, j3;

	sqr3_matr = new uchar[512];

	j = 2;
	j3 =8;
	for( i = 0; i < 512; i++){
		if ( i <= j3 ){
			sqr3_matr[i] = j-1;
		} else {
			++j;
			j3 = j*j*j;
			sqr3_matr[i] = j-1;
		}
	}//  end for i
	*sqr3_matr = 0;
}*/

dastPoly3D::dastPoly3D(  Vector _p, int _n ){
	p = NULL;
	p_array = NULL;
	
	n = _n;
	count = dastResSign->poly[n];

	p = new Vector;
	*p = _p;

/*	 uchar *l1 = get_real_ground(0, p[0].x & clip_mask_x, p[0].y);
	 uchar *l2 = get_real_ground(1, p[0].x & clip_mask_x, p[0].y);

	if ( abs( *l1 - p[0].z) < abs(*l2 - p[0].z)){
		pf = &get_up_ground;
	} else {
		pf = &get_down_ground;
	}*/
}

void dastPoly3D::set(  Vector _p, int _n ){
	*p = _p;
	n = _n;
	count = 0;
}

void dastPoly3D::set(  Vector _p, int _n_, int _count_ ){
	*p = _p;
	n = _n_;
	count = _count_;
}

int dastPoly3D::quant_make_sign(void){
	Vector lp;
	int xl, xr, yl, yr;
	int dx, dy;
	int _z = p->z;

	for( int s = 0; s < dastResSign->once[n]; s++){

		if (count == dastResSign->poly[n]) return 0;

		lp.x = p->x + dastResSign -> x[n][count];
		lp.y = p->y + dastResSign -> y[n][count];


		if (p->x < lp.x ) {
			xl = p->x; 
			xr = lp.x; 
		}else {
			xl = lp.x;
			xr = p->x;
		}

		if (p->y < lp.y ) {
			yl = p->y; 
			yr = lp.y; 
		}else {
			yl = lp.y;
			yr = p->y;
		}

		switch (dastResSign -> type[n][count]){
		case DAST_SIGN_TYPE::UP:
		case DAST_SIGN_TYPE::DOWN:
		{
			dx = (lp.x-p->x);
			dy = (lp.y-p->y);

			int max;
			p->x <<= 16;
			p->x += 1<<15;

			p->y <<= 16;
			p->y += 1<<15;

			if ( abs(dx ) > abs(dy) ) max = abs(dx); else max = abs(dy);
			int tt = (1<<16)/max;

			dx *= tt;
			dy *= tt;

			for( int i = 0; i < max; i++){
				int x = (p->x)>>16;
				int y = (p->y)>>16;
				//dastPutSandOnMapAlt( x ,y, uchar(p->z), uchar(5), dastResSign -> wide[n][count] );
				dastPutSandOnMapAlt( x ,y, uchar(p->z), uchar(6), dastResSign -> wide[n][count] );
				//regRender(x - 7, y - 7, x + 7, y + 7, 0);
				p->x += dx;
				p->y += dy;
			}
			break;
		}
		case DAST_SIGN_TYPE::MOVE:	
			break;
		}
		*p = lp;
		p->z = _z;
		count++;
	}
	return (count != dastResSign->poly[n]);
}

dastPoly3D::dastPoly3D(  Vector _p, int x, int y ){
	p = NULL;
	p_array = NULL;
	
	n = x;
	count = y;

	p = new Vector;
	*p = _p;
}

dastPoly3D::dastPoly3D(Vector lp, Vector rp, Vector dv){
	p = NULL;
	p_array = NULL;
	
	n = 4;
	count = 0;
	p_array = new Vector[4];

	p_array[0] = rp;
	p_array[1] = lp;

	p_array[2] = lp + dv;
	p_array[3] = rp + dv;
}

void dastPoly3D::set( Vector lp, Vector rp, Vector dv ){
	p_array[0] = rp;
	p_array[1] = lp;

	p_array[2] = lp + dv;
	p_array[3] = rp + dv;
}

dastPoly3D::~dastPoly3D(void){
	if (p) {
		delete p;
	}
	if (p_array) {
		delete[] p_array;
	}
}

//#define _TEST_

#ifdef _TEST_
/*
void dastPoly3D::make_dast( void ){
	float dxv, dyv, dzv;
	int mv;
	float dxh, dyh, dzh;
	int mh;
	float x, y, z;
	int i, j, k;
	int *h;
	int *hh;
	int down;
	uchar *p0;

	int xr, xl,yr, yl;
	int wide = 10;

	if ( !MOVER_ON ) return;

//	if ( !vMap -> lineT[yl-10] || !vMap -> lineT[yr+10] ) return;

//  Нахождение уровня машинки

	 uchar *l1 = get_real_ground(0, p[0].x & clip_mask_x, p[0].y);
	 uchar *l2 = get_real_ground(1, p[0].x & clip_mask_x, p[0].y);

	if ( abs( *l1 - p[0].z) < abs(*l2 - p[0].z)){
		down = 0;
		pf = &get_up_ground;
	} else {
		down = 1;
		pf = &get_down_ground;
	}

//   Построение минимальных сдвигов.

	dxv = p[1].x - p[0].x;
	dyv = p[1].y - p[0].y;

	if ( dxv < (max_len_mech - (int)(1<<MAP_POWER_X)) ){
		dxv += (int)(1<<MAP_POWER_X);
	} else if ( dxv > ((int)(1<<MAP_POWER_X) - max_len_mech)){
		dxv -= (int)(1<<MAP_POWER_X);
	}

	int d_wide = 4;

	if ( abs( dxv ) < abs( dyv ) ) {
		mv = round(double(abs(dyv)))*2;
	} else {
		mv = round(double(abs(dxv)))*2;
	}

	float iii = 1.0/mv;

	dyv = dyv*iii;
	dxv = dxv*iii;
	dzv = (p[1].z - p[0].z)*iii;

	dxh = p[3].x - p[0].x;
	dyh = p[3].y - p[0].y;

	if ( dxh == 0.0 && dyh == 0.0 ) return;

	if ( dxh < (max_len_mech - (int)(1<<MAP_POWER_X)) ){
		dxh += (int)(1<<MAP_POWER_X);
	} else if ( dxh > ((int)(1<<MAP_POWER_X) - max_len_mech)){
		dxh -= (int)(1<<MAP_POWER_X);
	}

	if ( abs( dxh ) < abs( dyh ) ) {
		mh = round(double(abs(dyh)))*2;
	} else {
		mh = round(double(abs(dxh)))*2;
	}

	iii = 1.0/mh;
	dyh = dyh*iii;
	dxh = dxh*iii;
	dzh = (p[3].z - p[0].z)*iii;

//  Определение поверхности для пересчета цвета

	if ( dyh < 0 ){
		if ( dxh > 0){
			yr = p[2].y;
			yl = p[0].y;

			xr = p[1].x;
			xl = p[3].x;
		} else {
			yr = p[3].y;
			yl = p[1].y;

			xr = p[2].x;
			xl = p[0].x;
		}
	} else {
		if ( dxh > 0){
			yr = p[1].y;
			yl = p[3].y;

			xr = p[0].x;
			xl = p[2].x;
		} else {
			yr = p[0].y;
			yl = p[2].y;

			xr = p[3].x;
			xl = p[1].x;
		}
	}

	if ( xr > xl ) {
		int tmp = xr;
		xr = xl;
		xl = tmp;
	}

	if ( yr > yl ) {
		int tmp = yr;
		yr = yl;
		yl = tmp;
	}

	//  приготовление к основному циклу.

	h = new int[mv];
	hh = new int[mv+10];

	hh += 5;

	memset( h, 0, mv*sizeof( int ));

	x = p[0].x;
	y = p[0].y;
	z = p[0].z;

	int  dxv5 = dxv*5.0;
	int  dyv5 = dyv*5.0;

//  Цикл по шагам сдвига ковша.

	for( i = 0; i < mh + d_wide; i++){
		float xx = x, yy = y, zz = z;

		
//     Цикл по точкам ковша - сдвигание поверхности.

		for( j = 0; j < mv; j++){
			int zzz = round(double(zz));
			
			p0 = pf( round(double(xx)) & clip_mask_x, round(double(yy)) & clip_mask_y );

			if ( *p0 > zzz ) {
				*p0 = zzz;
			} 

			xx += dxv;
			yy += dyv;
			zz += dzv;
		}//  end for j

		x += dxh;
		y += dyh;
		z += dzh;
	}//  end for i

	z = round(double(dxv*10));
	y = round(double(dxh*10));

	if ( xr + z < xr + y ) x = xr + z; else x = xr + y;
	if ( xr > x ) xr = x - 6;
	else {
		if ( xl + z < xl + y ) x = xl + y; else x = xl + z;
		if ( xl < x ) xl = x + 6;
	}

	z = round(double(dyv*10));
	y = round(double(dyh*10));

	if ( yr + z < yr + y ) x = yr + z; else x = yr + y;
	if ( yr > x ) yr = x - 6;
	else {
		if ( yl + z < yl + y ) x = yl + y; else x = yl + z;
		if ( yl < x ) yl = x + 6;
	}
	regSet(xr, yr, xl, yl, 0);
	regRender(xr, yr, xl, yl, 0);

	delete h;
	hh -= 5;
	delete hh; 
}

#else

const int _boder_ = 8;

void dastPoly3D::make_dast( void ){
	int dxv, dyv, dzv, mv;
	int dxh, dyh, dzh, mh;
	int x, y, z;
	int i, j, k;
	int *h;
	int *hh;
	int down;
//	unsigned char **lt = vMap -> lineT;
	uchar *p0;

	int xr, xl,yr, yl;
	
	if ( !MOVER_ON ) return;

//	if ( !vMap -> lineT[yl-10] || !vMap -> lineT[yr+10] ) return;

//  Нахождение уровня машинки

	 uchar *l1 = get_real_ground(0, p[0].x & clip_mask_x, p[0].y);
	 uchar *l2 = get_real_ground(1, p[0].x & clip_mask_x, p[0].y);

	if ( abs( *l1 - p[0].z) < abs(*l2 - p[0].z)){
		down = 0;
		pf = &get_up_ground;
	} else {
		down = 1;
		pf = &get_down_ground;
	}

//   Построение минимальных сдвигов.

	dxv = p[1].x - p[0].x;
	dyv = p[1].y - p[0].y;

	if ( dxv < (max_len_mech - (int)(1<<MAP_POWER_X)) ){
		dxv += (int)(1<<MAP_POWER_X);
	} else if ( dxv > ((int)(1<<MAP_POWER_X) - max_len_mech)){
		dxv -= (int)(1<<MAP_POWER_X);
	}

	if ( abs( dxv ) < abs( dyv ) ) {
		mv = abs(dyv)*2;
	} else {
		mv = abs(dxv)*2;
	}

	int iii = (1<<16)/mv;

	dyv = dyv*iii;
	dxv = dxv*iii;
	dzv = (p[1].z - p[0].z)*iii;

	dxh = p[3].x - p[0].x;
	dyh = p[3].y - p[0].y;

	if ( !dxh && !dyh ) return;

	if ( dxh < (max_len_mech - (int)(1<<MAP_POWER_X)) ){
		dxh += (int)(1<<MAP_POWER_X);
	} else if ( dxh > ((int)(1<<MAP_POWER_X) - max_len_mech)){
		dxh -= (int)(1<<MAP_POWER_X);
	}

	if ( abs( dxh ) < abs( dyh ) ) {
		mh = abs(dyh)*2;
	} else {
		mh = abs(dxh)*2;
	}

	iii = (1<<16)/mh;
	dyh = dyh*iii;
	dxh = dxh*iii;
	dzh = (p[3].z - p[0].z)*iii;

//  Определение поверхности для пересчета цвета

	if ( dyh < 0 ){
		if ( dxh > 0){
			yr = p[2].y;
			yl = p[0].y;

			xr = p[1].x;
			xl = p[3].x;
		} else {
			yr = p[3].y;
			yl = p[1].y;

			xr = p[2].x;
			xl = p[0].x;
		}
	} else {
		if ( dxh > 0){
			yr = p[1].y;
			yl = p[3].y;

			xr = p[0].x;
			xl = p[2].x;
		} else {
			yr = p[0].y;
			yl = p[2].y;

			xr = p[3].x;
			xl = p[1].x;
		}
	}

	if ( xr > xl ) {
		int tmp = xr;
		xr = xl;
		xl = tmp;
	}

	if ( yr > yl ) {
		int tmp = yr;
		yr = yl;
		yl = tmp;
	}

	for ( i = yr-20; i <= yl+20; i++)
		if ( !lt[i & clip_mask_y] ) return;	

	//  приготовление к основному циклу.

	h = new int[mv];
	hh = new int[mv+_boder_*4];

	hh += _boder_*2;

	memset( h, 0, mv*sizeof( int ));

	x = (p[0].x << 16) + (1<<15);
	y = (p[0].y << 16) + (1<<15);
	z = (p[0].z  << 16) + (1<<15);

	int  dxv5 = dxv*_boder_*2;
	int  dyv5 = dyv*_boder_*2;

//  Цикл по шагам сдвига ковша.

	for( i = 0; i < mh + 4; i++){
		int xx = x, yy = y, zz = z;

//     Цикл по точкам ковша - пересыпание сдвинутой поверхности.

		memset( hh - _boder_*2, 0, (mv+_boder_*4)*sizeof( int ));

		for( j = 0; j < mv; j++){
			int ha = h[j];

			hh[j] += ha>>2;


			for( k = 1; k <= _boder_*2; k++){
				hh[j+k] += (3*ha)>>7;
				hh[j-k] += (3*ha)>>7;
			}

		}
//		hh[-_boder_*2] = 100<<10;

		xx = x - dxv5;
		yy = y - dyv5;

		for( j = -_boder_*2; j < mv+_boder_*2; j++){
			int _h_;
			xxx  = (xx >> 16) & clip_mask_x, yyy = yy >> 16;

			p0 = pf(  xxx, yyy );
			_h_ = hh[j] >> 10;
			hh[j] = 0;

			if (_h_ > 20 ) {
				hh[j] += _h_ - 20;
				_h_ = 20;
			}

			if ( (_h_ + *p0) > 255 ) {
					hh[j] += (int(*p0) + _h_ - 255)<<10;
					*p0 = 254;
			}else{
					*p0 += _h_;
					hh[j] = 0;
			}
			xx += dxv;
			yy += dyv;
		}

		xx = x;
		yy = y;

		memcpy( h, hh, mv*sizeof(int));

//     Цикл по точкам ковша - сдвигание поверхности.

		for( j = 0; j < mv; j++){
			int zzz = zz >> 16;
			//uchar *pa = lt[yyy]+(xxx & clip_mask_x);

			p0 = pf( (xx >> 16) & clip_mask_x, (yy >> 16) & clip_mask_y );

			if ( *p0 > zzz ) {
				h[j] += ((int)(*p0) - zzz)<<10;

				//*pa = zzz;
				if (zzz > 0 ) 
					*p0 = zzz;
				else
					*p0 = 0;
			} 

			xx += dxv;
			yy += dyv;
			zz += dzv;
		}//  end for j

		x += dxh;
		y += dyh;
		z += dzh;
	}//  end for i

//  Пересыпание после сдвига	

	memset( hh - _boder_*2, 0, (mv+_boder_*4)*sizeof( int ));

	for( j = 0; j < mv; j++){
		int ha = h[j];

		hh[j] += ha>>2;

		for( k = 1; k <= _boder_*2; k++){
			hh[j+k] += (3*ha)>>7;
			hh[j-k] += (3*ha)>>7;
		}
	}

//	memcpy( hh, h, mv*sizeof(int));
	x -= dxv5;
	y -= dyv5;
	int xx = x, yy = y;

	xx += dxh*2;
	yy += dyh*2;

	for( i = -(_boder_*2); i < (mv + _boder_*2); i++){
		uchar hhh = sqr3(hh[i]>>10);
//		uchar hhh = (sqrt(double(hh[i]>>10)) + sqr3(hh[i]>>10))/2;
//		uchar hhh = sqrt(double(hh[i]>>10));
//		uchar hhh = hh[i]>>10;

		if ( !hhh ) continue;

		p0 = pf( (( x >> 16) & clip_mask_x), y>>16 );
		*p0 += hhh;

		int _xx = xx, _yy = yy;

		for( k = 0; k < 8*hhh; k++){//6
			int xxx = _xx , yyy = _yy;

			int _xxx = (xxx >> 16) & clip_mask_x, _yyy = yyy >> 16;
			uchar tt = 0;
			 tt = hhh - (k>>3);

			p0 = pf( _xxx, _yyy );		
			*p0 += tt;

			_xx += dxh;
			_yy += dyh;
		}//  end for k

		xx += dxv;
		yy += dyv;

		x += dxv;
		y += dyv;
	}//  end for i

	z = dxv>>12;
	y = dxh>>12;

	if ( xr + z < xr + y ) x = xr + z; else x = xr + y;
	if ( xr > x ) xr = x - 10;
	else {
		if ( xl + z < xl + y ) x = xl + y; else x = xl + z;
		if ( xl < x ) xl = x + 10;
	}

	z = dyv>>12;
	y = dyh>>12;

	if ( yr + z < yr + y ) x = yr + z; else x = yr + y;
	if ( yr > x ) yr = x - 10;
	else {
		if ( yl + z < yl + y ) x = yl + y; else x = yl + z;
		if ( yl < x ) yl = x + 10;
	}
	int dob = 20;
	regSet(xr - dob, yr - dob, xl + dob, yl + dob, 0);
	regRender(xr - dob, yr - dob, xl + dob, yl + dob, 0);

	delete h;
	hh -= _boder_*2;
	delete hh; 
}*/
#endif

void dastPoly3D::make_mole( dastResourcePoly3D* res ){
	int xl, xr, yl, yr;

//--------------------------------------
	if (p_array[0].x < p_array[1].x ) {
		xl = p_array[0].x; 
		xr = p_array[1].x; 
	}else {
		xl = p_array[1].x;
		xr = p_array[0].x;
	}
	if (xl > p_array[2].x ) xl = p_array[2].x;
	if (xl > p_array[3].x ) xl = p_array[3].x;

	if (xr < p_array[2].x ) xr = p_array[2].x;
	if (xr < p_array[3].x ) xr = p_array[3].x;

	if (p_array[0].y < p_array[1].y ) {
		yl = p_array[0].y; 
		yr = p_array[1].y; 
	}else {
		yl = p_array[1].y;
		yr = p_array[0].y;
	}
	if (yl > p_array[2].y ) yl = p_array[2].y;
	if (yl > p_array[3].y ) yl = p_array[3].y;

	if (yr < p_array[2].y ) yr = p_array[2].y;
	if (yr < p_array[3].y ) yr = p_array[3].y;
//--------------------------------------

	if (!vMap -> lineT[p_array[0].y & clip_mask_y])
		return;

	unsigned int scale;
	int dz = *get_down_ground( (p_array[0].x) & clip_mask_x, p_array[0].y );

	if ( dz < 50 ) scale = 1 << 15;
	else if ( dz < 120 ) scale = 1 << 14;
	else if ( dz < 200 ) scale = 1 << 13;
	else return ;

	for( int i = 1; i < 8; i++)
		if(!RND(5)) {
			dastPutSpriteOnMapAlt( (i*p_array[1].x + (8-i)*p_array[0].x)>>3, (i*p_array[1].y +  (8-i)*p_array[0].y)>>3, res->data[RND(res->n)], res->x_size, res->y_size, scale);
			++i;
		}

	regRender(xl - (res->x_size>>1), yl - (res->y_size>>1), xr + (res->x_size>>1), yr + (res->y_size>>1), 0);
}

int dastPoly3D::make_first_mole( dastResourcePoly3D* res ){
	int dA = PIx2>>5;
	int A = 0;
	unsigned int scale;
	if (count < 3 || n < 3) return 0;

	if (!vMap -> lineT[p->y & clip_mask_y])
		return 0;

	int dz = *get_down_ground( (p->x) & clip_mask_x, p->y );

	n -= 3;
	count -= 3;

	if ( dz < 50 ) scale = 1 << 15;
	else if ( dz < 160 ) scale = 1 << 14;
	else if ( dz < 220 ) scale = 1 << 13;
	else return 0;

	for( int i = 1; i < 32; i++, A += dA)
//		if ( !RND(16 - (count/5)) ){
		if ( RND(count) && RND(3)){
			dastPutSpriteOnMapAlt( p->x + ((SI[A]*count)>>16), p->y + ((CO[A]*count)>>16), res->data[RND(res->n)], res->x_size, res->y_size, scale);
			//regRender( p->x - n - 8, p->y - count - 8, p->x + n + 8, p->y + count + 8, 0);
		}

	regRender( p->x - n - 8, p->y - count - 8, p->x + n + 8, p->y + count + 8, 0);
	
	return ((n > 0)&&(count > 0));
}

int dastPoly3D::make_last_mole( dastResourcePoly3D* res ){
	int dA = PIx2>>5;
	int A = 0;
	unsigned int scale;
	if (count >= n ) return 0;

	if (!vMap -> lineT[p->y & clip_mask_y])
		return 0;

	int dz = *get_down_ground( (p->x) & clip_mask_x, p->y );

	count += 3;

	if ( dz < 50 ) scale = 1 << 15;
	else if ( dz < 200 ) scale = 1 << 14;
	else scale = 1 << 13;
	
	for( int i = 1; i < 32; i++, A += dA)
		if ( RND(count) ){
//		if ( !RND(16 - (count/5)) ){
			dastPutSpriteOnMapAlt( p->x + ((SI[A]*count)>>16), p->y + ((CO[A]*count)>>16), res->data[RND(res->n)], res->x_size, res->y_size, scale);
			//regRender( p->x - n - 8, p->y - count - 8, p->x + n + 8, p->y + count + 8, 0);
		}

	regRender( p->x - count - 8, p->y - count - 8, p->x + count + 8, p->y + count + 8, 0);
	
	return (count < n);
}

int dastPoly3D::make_catch_dolly( dastResourcePoly3D* res ){
	int dA = PIx2>>5;
	int A = 0;
	unsigned int scale;
	int dx, dy;
	if (count <= 0 ) return 0;

	if (!vMap -> lineT[p->y & clip_mask_y])
		return 0;

	int dz = *get_down_ground( (p->x) & clip_mask_x, p->y );

	if ( dz < 50 ) scale = 1 << 15;
	else if ( dz < 160 ) scale = 1 << 14;
	else if ( dz < 220 ) scale = 1 << 13;
	else return 0;


	for( int i = 1; i < 32; i++, A += dA)
		if ( !RND(16) ){
			dx = ((SI[A]*(n + RND(10)))>>16);
			dy = ((CO[A]*(n + RND(10)))>>16);
			dastPutSpriteOnMapAlt( p->x + dx, p->y + dy, res->data[RND(res->n)], res->x_size, res->y_size, scale);
			regRender( p->x + dx  - 8, p->y + dy - 8, p->x + dx + 8, p->y + dy + 8, 0);
		}

	//regRender( p->x - n - 18, p->y - n -18, p->x + n + 18, p->y + n + 18, 0);
	
	count--;
	return (count > 0);
}

dastResourcePoly3D::dastResourcePoly3D(const char* FileName){
	int i;
	XStream fin(FileName, XS_IN);

	fin > x_size > y_size > n;

	data = new uchar*[n];

	for( i = 0; i < n; i++){
		data[i] = new uchar[x_size*y_size];
		fin.read(data[i], x_size*y_size);
	}

	fin.close();
}

dastResourceSign::dastResourceSign(const char* FileName){
	int i;
	int tmp;
	XStream fin(FileName, XS_IN);

	fin >=  n;
	//std::cout<<"dastResourceSig n:"<<(int)n<<std::endl;
	poly = new uchar[n];
	x = new char*[n];
	y = new char*[n];
	type = new char*[n];
	once = new uchar[n];
	wide = new uchar*[n];
	int coef;

	for( i = 0; i < n; i++){
		if (i == 7) coef = 2;
		else coef = 1;

		fin >= poly[i];
		//std::cout<<"poly:"<<(int)poly[i]<<std::endl;
		fin >= once[i];
		x[i] = new char[poly[i]];
		y[i] = new char[poly[i]];
		type[i] = new char[poly[i]];
		wide[i] = new uchar[poly[i]];

		for( int j = 0; j < poly[i]; j++){
			fin >= tmp;		x[i][j] = tmp*coef;
			//std::cout<<"x[i][j]:"<<(int)x[i][j]<<" "<<j<<std::endl;
			fin >= tmp;		y[i][j] = tmp*coef;
			//std::cout<<"y[i][j]:"<<(int)y[i][j]<<std::endl;
			fin >= tmp;		type[i][j] = tmp;
			//std::cout<<"type[i][j]:"<<(int)type[i][j]<<std::endl;
			fin >= tmp;		wide[i][j] = tmp;
			//std::cout<<"wide[i][j]:"<<(int)wide[i][j]<<std::endl;
		}
	}

	fin.close();
}

dastResourcePoly3D::~dastResourcePoly3D(void){
	int i;

	if ( n ){
		for( i = 0; i < n; i++)
			delete data[i];

		delete[] data;
	}
}

dastResourceSign::~dastResourceSign(void){
	int i;

	if ( n ){
		delete[] poly;
		for( i = 0; i < n; i++){
			delete[] x[i];
			delete[] y[i];
			delete[] type[i];
			delete[] wide[i];
		}
		delete[] once;
		delete[] x;
		delete[] y;
		delete[] type;
		delete[] wide;
	}
}

void dastCreateResource(const char* FileName){
	dastResource = new dastResourcePoly3D(FileName);
}

void dastCreateResource(void){
	delete dastResource;
}

void dastCreateResourceSign(const char* FileName){
	dastResSign = new dastResourceSign(FileName);
}

void dastDeleteResourceSign(void){
	delete dastResSign;
}

void dastPutSpriteOnMapAlt(int x, int y, uchar *data, int x_size, int y_size, unsigned int scale){
		uchar **lt = vMap -> lineT;
		int i, j;
		uchar *llt;

		for( i = y - (y_size>>1); i < y + (y_size>>1); i++)
			if (!lt[(i) & clip_mask_y]) return;

		for( i = y - (y_size>>1); i < y + (y_size>>1); i++)
			for( j = x - (x_size>>1); j < x + (x_size>>1); j++, data++){
				llt = get_down_ground( (j) & clip_mask_x, i );
				if (BREAKABLE_TERRAIN(*(llt + H_SIZE))){
					int dz = (*data)*scale>>15;
					if ( *llt + dz < 256 ) *llt += dz;
					else *llt = 255;
				}
			}
}

void dastPutSandOnMapAlt(int x, int y, uchar _z,  uchar newter, uchar wide){
		uchar **lt = vMap -> lineT;
		int i, j;
		uchar _d = 82;
		newter <<= TERRAIN_OFFSET;

		int x_size = wide;
		int y_size = wide;

		for( i = y - (y_size>>1); i < y + (y_size>>1); i++)
			if (!lt[(i) & clip_mask_y]) return;

		for( i = y - (y_size>>1); i <= y + (y_size>>1); i++)
			for( j = x - (x_size>>1); j <= x + (x_size>>1); j++){
				//uchar *lc = get_down_ground( (j) & clip_mask_x, i );
			    //uchar *lc = (*pf)( (j) & clip_mask_x, i );
				uchar *lc = get_up_ground( (j) & clip_mask_x, i );
				if ( *lc > _z ) lc = get_down_ground( (j) & clip_mask_x, i );
				if (*lc + _d < _z) continue;

				uchar hh = RND(4);
				//if ( GET_TERRAIN(*(lc + H_SIZE)) ){
					*lc += hh + RND(2);
					//*lc += RND(6);
				if ( GET_TERRAIN(*(lc + H_SIZE)) ){
					//if ( hh || !RND(4) ){
					if ( !RND(4) ){
						*(lc + H_SIZE) &= ~TERRAIN_MASK;
 						*(lc + H_SIZE) += newter; 
					} 
				}
		}
		regRender(x - (x_size>>1), y - (y_size>>1), x + (x_size>>1), y + (y_size>>1), 0);
}
