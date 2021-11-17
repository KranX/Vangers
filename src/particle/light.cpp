#include "../global.h"

#include "../3d/3d_math.h"
#include "../3d/3dgraph.h"
#include "../3d/3dobject.h"
#include "../3d/parser.h"

#include "../sqexp.h"
#include "../backg.h"

#include "../terra/vmap.h"
#include "../terra/world.h"
#include "../terra/render.h"
#include "../dast/poly3d.h"

#include "../particle/particle.h"
#include "../particle/partmap.h"
#include "../particle/df.h"
#include "../particle/light.h"

#include "../units/hobj.h"
#include "../units/effect.h"

uchar *light_table_pal = NULL;
uchar *invisible_table;

int FrameCount = 0;

void LineQuake( int x1, int y1, int x0, int y0, int dx, int shiftparam );

int aStrLen(unsigned char* str,int font,int space );
void aOutStr(int x,int y,int font,int color,unsigned char* str,int space );

uchar BRIGHTNESS[TERRAIN_MAX] = { 255, 255, 255, 255, 255, 255, 255, 255};

int light_table_pal_size = 256*LIGHT_MAX_BRIGHTNESS;

void LightPoint::load_color_line( void ){
	unsigned int i;
	uchar **ltc = vMap -> lineTcolor;
	uchar **lt = vMap -> lineT;
	uchar *llt;
	uchar *llct;
	uchar *dbuf = buf;
	//int offset = LIGHT_MAX_RADIUSE<<1;
	int offset = x_size;

	memset( bool_buf, 0, y_size );

	for( i = 0; i < y_size; i++, dbuf += offset){
		llt = lt[(y + i) & clip_mask_y];
		llct = ltc[(y + i) & clip_mask_y];
		int _x = (x & clip_mask_x);

		if ( llt  ){
			if ( _x + x_size >=  map_size_x){
				int d =  map_size_x - _x;

				memcpy( dbuf, llct + _x, d);
				memcpy( dbuf + d, llct,  x_size - d);
			} else {
				memcpy( dbuf, llct + _x, x_size);
			}
			bool_buf[i] = 1;
		}//  end if
	}//  end for i
}

void LightPoint::restore_color_line( void ){
	unsigned int i;
	uchar **ltc = vMap -> lineTcolor;
	uchar **lt = vMap -> lineT;
	uchar *llt;
	uchar *llct;
	uchar *dbuf = buf;
	//int offset = LIGHT_MAX_RADIUSE<<1;
	int offset = x_size;

	for( i = 0; i < y_size; i++, dbuf += offset){
		llt = lt[(y + i) & clip_mask_y];
		llct = ltc[(y + i) & clip_mask_y];

		if ( llt && bool_buf[i] ){
			if ( (x & clip_mask_x) + x_size >=  map_size_x){
				int d =  map_size_x - (x & clip_mask_x);

				memcpy( llct + (x & clip_mask_x ), dbuf,  d);
				memcpy( llct, dbuf + d,  x_size - d);
			} else
				memcpy(  llct + (x & clip_mask_x ), dbuf, x_size);
		} else {
		}//  end if
	}//  end for i
}

void LightPoint::set_position(int _x,int _y,int _z){
	last_z = _z;
	xn = _x - (x_size>>1);
	yn = _y - (y_size>>1);
}

int LightPoint::quant(void) {
	int i, j;
	int R = y_size >> 1;
	uchar **ltc = vMap -> lineTcolor;
	uchar **lt = vMap -> lineT;
	uchar *llt;
	uchar *llct;
	uchar *llt1;
	uchar *llt2;
	uchar *llt3;
	uchar *llct1;
	int E = 32;
	int ee;
	int _R;
	unsigned int _z = (unsigned int)last_z;

	x = xn;
	y = yn;

	if (type & LIGHT_TYPE::STATIONARY) {
		if ( c_time < LIGHT_FIRST_TIME) {
			c_time++;
			E = ((c_time*energy)/LIGHT_FIRST_TIME)>>15;
		} else {
			E = (energy -= (3<<15))>>15;
			c_time += 3;
		}

		if( energy <= (5<<15) ) return 0;
	} else if (type & LIGHT_TYPE::DYNAMIC) {
		c_time++;
		if (c_time <= 5 ){
			energy -= d_energy;
		} else {
			if ( c_time == 10 ) c_time = 0;
			energy += d_energy;
		};
		E = energy>>15;
	} else if (type & LIGHT_TYPE::STATIC) {
		E = energy>>15;
	}
	
	//E=32;
	_R = E*(1<<16)/(R*R);

	//std::cout<<"LightPoint::quant type:"<<type<<" R:"<<R<<" _R:"<<_R<<" E:"<<E<<" energy:"<<energy<<std::endl;
	BackD.put(this);
	load_color_line();

	int rr;
	
	if (type & LIGHT_TYPE::TOR) {
		int dd, da, yy, xx, delta;
		R--;

		xx = 0;
		yy = R;
		dd = 2*( 1 - R);
		delta = 0;

		while( yy >= delta){
			FourLightLine( xx, yy, E);

			if (dd < 0){
				da = 2*(dd + yy) - 1;
				if (da <= 0){
					xx++;
					dd += 2*xx + 1;
				} else if ( da > 0){
					xx++;
					yy--;
					dd += 2*(xx - yy + 1);
				} 
			} else if (dd > 0){
				da = 2*(dd + xx) - 1;
				if (da <= 0){
					xx++;
					yy--;
					dd += 2*(xx - yy + 1);
				} else if ( da > 0){
					yy--;
					dd += 1 - 2*yy;
				} 
			} else {
				xx++;
				yy--;
				dd += 2*(xx - yy + 1);
			}
		}//  while
		return 1;
	} else {
		int dimR[256];
		for(i = 0; i <= R+2; i++) {
			dimR[i]=(int)sqrt((double)R*R-i*i);
		}
		for(i = 1; i <= R; i++){//y_size
			if ( bool_buf[i]&& bool_buf[2*R-i] ) {
				int _y = (y+i) & clip_mask_y;
				int _y1 = (y+2*(R)-i-1) & clip_mask_y;//R-1

				llct = ltc[ _y ];
				llct1= ltc[ _y1 ];
	//			llct[x&clip_mask_x]=255;llct[(x+x_size-1)&clip_mask_x]=255;
	//			llct1[x&clip_mask_x]=255;llct1[(x+x_size-1)&clip_mask_x]=255;
				j=0;
				while(dimR[j]-(R-i) >0){
					int _x = ((x+R+j) & clip_mask_x)|1; 
					int _x2 = ((x+R-j-2) & clip_mask_x)|1; 
					j += 2;
					//Sectors light around point (light under pulse)
					llt  =  lt[YCYCL(_y)] + XCYCL(_x);
					llt1 =  lt[YCYCL(_y1)] + XCYCL(_x);
					llt2 = lt[YCYCL(_y)]+XCYCL(_x2);
					llt3 = lt[YCYCL(_y1)]+XCYCL(_x2);
					if (!lt[YCYCL(_y)] || !lt[YCYCL(_y1)]) {
						continue;
					}
					int r4=(R*2-dimR[j]-dimR[R-i]);
					int r, first_shift;
					if ( (*llt) < _z ){
						int _dz = _z - (*llt);
						if  (_dz < 128) {
							//r = (r<<8)/(_dz + 128 );
							r = r4+(2*r4>>((_dz&0x60)>>5));
						} else {
							r = r4;
						}
						rr = (R-r);
						if ( rr >= 0 ){
							ee=rr*rr;
							ee = (ee*_R)>>16;
							first_shift = (ee << 8) + llct[_x];
							if (light_table_pal_size <= first_shift || first_shift < 0) {
								std::cout<<"LightPoint::quant wrong light_table_pal position"<<std::endl;
								break;
							}
							//std::cout<<"rr*rr:"<<rr*rr<<"_R:"<<_R<<" ee:"<<ee<<"A:"<<(ee << 8) + llct[_x]<<" llct[_x]:"<<(long)llct[_x]<<" _x:"<<_x<<" pal:"<<&light_table_pal<<std::endl;
							llct[_x] = light_table_pal[ first_shift ];
							llct[_x-1] = light_table_pal[ (ee << 8) + llct[_x-1] ];
						}
					}
	//2tchk
					if ( (*llt1) < _z ){
						int _dz = _z - (*llt1);
						if  (_dz < 128)
							//r = (r<<8)/(_dz + 128 );
							r = r4+(2*r4>>((_dz&0x60)>>5));
						else r=r4;	
						rr = (R-r);
						if ( rr >= 0 ){ 
							ee=rr*rr;
							ee = (ee*_R)>>16;
							//std::cout<<"B:"<<(ee << 8) + llct[_x]<<" llct[_x]:"<<(long)llct[_x]<<" _x:"<<_x<<" pal:"<<&light_table_pal<<std::endl;
							llct1[_x] = light_table_pal[ (ee << 8) + llct1[_x] ];
							llct1[_x-1] = light_table_pal[ (ee << 8) + llct1[_x-1] ];
						}
					}
	//3
					if ( (*llt2) < _z ){
						int _dz = _z - (*llt2);
						if  (_dz < 128)
							//r = (r<<8)/(_dz + 128 );
							r = r4+(2*r4>>((_dz&0x60)>>5));
						else r=r4;	
						rr = (R-r);
						if ( rr >= 0 ){
							ee = rr*rr;
							ee = (ee*_R)>>16;
							llct[_x2] = light_table_pal[ (ee << 8) + llct[_x2] ]; //ERROR: #895
							llct[_x2-1] = light_table_pal[ (ee << 8) + llct[_x2-1] ];
						}
					}
	//4
					if ( (*llt3) < _z ){
						int _dz = _z - (*llt3);
						if  (_dz < 128)
							//r = (r<<8)/(_dz + 128 );
							r = r4+(2*r4>>((_dz&0x60)>>5));
						else r=r4;	
						rr = (R-r);
						if ( rr >= 0 ){
							ee=rr*rr;
							ee = (ee*_R)>>16;
							llct1[_x2] = light_table_pal[ (ee << 8) + llct1[_x2] ];
							llct1[_x2-1] = light_table_pal[ (ee << 8) + llct1[_x2-1] ];
						}
					}


				}//  end for j
			}//  end if
		}
	//*/
	/*	for( i = 0; i < y_size; i++){
			if ( bool_buf[i] ) {
				int _y = (y+i) & clip_mask_y;
				llct = ltc[ _y ];

				int yr = R - i;
				yr = yr*yr;

				for( j = 0; j < x_size; j++){
					int _x = (x+j) & clip_mask_x; 
					llt =  lt[_y] + _x;
					if(*(llt + H_SIZE) & DOUBLE_LEVEL)
						if(!(_x & 1)) (++llt);

					if ( (*llt) < _z ){
						int _dz = _z - (*llt);
						int xr = R - j;
						xr = xr*xr;
						int r = Sqrt( xr + yr );
						if  (_dz < 127)
							r = (r<<8)/(_dz + 128 );

						uchar cl = llct[_x]; 
						rr = (R-r);

						if ( rr < 0 ) continue;
						
						ee = _power_table[rr];
						ee = (ee*_R)>>16;

						llct[_x] =light_table_pal[ (ee << 8) + cl ];
					}
				}//  end for j
			}//  end if
		}//  end for i
	*/	
		//return c_time;
	}
	return 1;
}

void PrepareLight( void ) {
	std::cout<<"PrepareLight"<<std::endl;
	int i, j, k;
	//int h2 = LIGHT_HEIGHT*LIGHT_HEIGHT;

	light_table_pal = new uchar[ light_table_pal_size ];
	invisible_table = new uchar[ 256*16 ];
	memset(light_table_pal, 0, light_table_pal_size);


	for( i = 0; i < TERRAIN_MAX; i++){
		for( j = 0; j < LIGHT_MAX_BRIGHTNESS; j++){
			for( k = BEGCOLOR[i]; k <= ENDCOLOR[i]; k++){
				light_table_pal[k + (j << 8)] = k + round( float(j*BRIGHTNESS[i])/(255.0f));
				if( light_table_pal[k + (j << 8)] > ENDCOLOR[i] ) light_table_pal[k + (j << 8)] = ENDCOLOR[i];
			}//  end for k
		}//  end for j
	}//  end fro i

	for( i = 0; i < TERRAIN_MAX; i++){
		for( j = 0; j < 16; j++){
			for( k = BEGCOLOR[i]; k <= ENDCOLOR[i]; k++){
				invisible_table[k + (j << 8)] = k - j/2;
				if( invisible_table[k + (j << 8)] > ENDCOLOR[i] ) invisible_table[k + (j << 8)] = ENDCOLOR[i];
				if( invisible_table[k + (j << 8)] < BEGCOLOR[i] ) invisible_table[k + (j << 8)] = BEGCOLOR[i];
			}//  end for k
		}//  end for j
	}//  end fro i
}

void LightPoint::BackRestore(void)
{
	restore_color_line();
};

void LightPoint::CreateLight( int _r,int _e, int _t)
{
	Status = 0;
	//std::cout<<"LightPoint::CreateLight _r:"<<_r<<" _e:"<<_e<<" _t:"<<_t<<" light_table_pal:"<<(long long)light_table_pal<<std::endl;
	//Activate Function	

	if ( _r > maxR ) _r = maxR;

	type = _t;
	x_size = _r<<1;
	y_size = _r<<1;

	if ( _e >= LIGHT_MAX_BRIGHTNESS )
		energy = (LIGHT_MAX_BRIGHTNESS-1)<<15;
	else
		energy = (_e)<<15;
	d_energy = energy/5;

	c_time  = 0;	
};

void LightPoint::Quant(void)
{
	if(Status & SOBJ_LINK) Status = SOBJ_DISCONNECT;
	else{
		if(!quant()) Status = SOBJ_LINK;
	};
};


void LightPoint::Init(StorageType* s )
{
// 	std::cout<<"LightPoint::Init"<<std::endl;
	GeneralObject::Init(s);
};

void LightPoint::MemInit( int R )
{
	
// Allocate memory
	maxR = R;
	buf = new uchar[R*R*4];
	bool_buf = new uchar[R*2];
	memset(bool_buf, 0, R*2);
};

void LightPoint::Free(void)
{
	// Free memory
	delete[] buf;
	delete[] bool_buf;
};

void LightPoint::Destroy(void)
{
//	Status |= SOBJ_DISCONNECT;
	Status |= SOBJ_LINK;
};

void  LandQuake::set(int _x, int _y, int _Rmin, int _Rmax, int _dx){
	x = _x;
	y = _y;
	Rcur = Rmin = _Rmin;
	Rmax = _Rmax;
	dx = _dx;
};

int  LandQuake::quant(int shiftparam){
	int dd, da, yy, xx, delta;

	xx = 0;
	yy = Rcur;
	dd = 2*( 1 - Rcur);
	delta = 0;

	if (Rcur > Rmax) return 0;

	while( yy > delta){

		LineQuake( x + xx, yy + y, x, y, dx, shiftparam );
		LineQuake( x - xx, yy + y, x, y, dx, shiftparam );
		LineQuake( x + xx, y - yy , x, y, dx, shiftparam );
		LineQuake( x - xx, y - yy , x, y, dx, shiftparam );

		if (dd < 0){
			da = 2*(dd + yy) - 1;
			if (da <= 0){
				xx++;
				dd += 2*xx + 1;
			} else if ( da > 0){
				xx++;
				yy--;
				dd += 2*(xx - yy + 1);
			} 
		} else if (dd > 0){
			da = 2*(dd + xx) - 1;
			if (da <= 0){
				xx++;
				yy--;
				dd += 2*(xx - yy + 1);
			} else if ( da > 0){
				yy--;
				dd += 1 - 2*yy;
			} 
		} else {
			xx++;
			yy--;
			dd += 2*(xx - yy + 1);
		}
	}//  while

	Rcur += dx;
	return 1;
}

void LineQuake( int x1, int y1, int x0, int y0, int dx, int shiftparam ){
	if(y1 > VcutDown) return;
	if(x1 > UcutRight) return;
	if(y1 < VcutUp) return;
	if(x1 < UcutLeft) return;

	int coef = (1<<shiftparam)-1;

	int color = XGR_GetPixel(x1, y1);
	XGR_Line(x1,y1,(coef*x1 + x0)>>shiftparam,(coef*y1 + y0)>>shiftparam,color);
}

void LightPoint::FourLightLine(int _dx, int _dy, int _E){
	uchar **ltc = vMap -> lineTcolor;
	uchar **lt = vMap -> lineT;
	unsigned int _z = (unsigned int)last_z;
	int dx, dy, de, ee, _e;
	int _x, _y;
	int n, color;
	int R = y_size>>1;

	
	if (_dx > _dy) 
		n = _dx>>2;
	else
		n = _dy>>2;

	dx = -(_dx<<12)/n;
	dy = -(_dy<<12)/n;
	de = (_E<<15)/n;
	ee = 0 + (1<<14);

	_x = (_dx<<15) + (1<<14);
	_y = (_dy<<15) + (1<<14);

	for( int i = 0; i < 2*n; i++){
		int _x_ = _x >> 15;
		int _y_ = _y >> 15;
		_e = ee>>15;

		int mx = (x + _x_  + R)&clip_mask_x;
		int my = (y + _y_  + R)&clip_mask_y;
		
		if ( lt[my] ){
			if ( lt[my][ mx|1] <= _z){
				color = buf[ (_y_ + R)*x_size +  _x_ + R];
				ltc[ my ][ mx ] = light_table_pal[ ((_e) << 8) + color ];
			}
			mx = (x - _x_ + R)&clip_mask_x;
			if ( lt[my][mx|1] <= _z){
				color = buf[ (_y_ + R)*x_size + R -  _x_];
				ltc[ my ][ mx ] = light_table_pal[ ((_e) << 8) + color ];
			}
		}
		my = (y - _y_ + R)&clip_mask_y;
		mx = (x + _x_ + R)&clip_mask_x;

		if (lt[my] ){
			if ( lt[my][ mx|1] <= _z){
				color = buf[ (R - _y_)*x_size +  _x_ + R];
				ltc[ my ][ mx ] = light_table_pal[ ((_e) << 8) + color ];
			}
			mx = (x - _x_ + R)&clip_mask_x;
			if ( lt[my][mx|1] <= _z){
				color = buf[ (R- _y_)*x_size + (R -  _x_)];
				ltc[ my ][ mx ] = light_table_pal[ ((_e) << 8) + color ];
			}
		}

		_x += dx;
		_y += dy;
		if ( i < n)
			ee += de;
		else 
			ee -= de;	
	}

}

void aWriteHelpString(int count, int* x, int* y, int font, unsigned char **pstr, int timer, int color, int space, unsigned int *pcolor){
	int i, total_len = 0;
	int draw_len = CHAR_SPEED*timer;
	unsigned char tmp[256];
	int _x;
	int _color = color;

	for( i = 0; i < count; i++){
		if (pstr[i]){
			if (pcolor[i]) _color = pcolor[i];

			total_len += strlen((char*)pstr[i]);	
			if (total_len <= draw_len){
				aOutStr(x[i], y[i], font, _color, pstr[i], space );
			} else {
				int d = total_len - draw_len;
				memcpy( tmp, pstr[i],strlen((char*)pstr[i]) - d);
				tmp[strlen((char*)pstr[i]) - d] = 0;

				aOutStr(x[i], y[i], font, _color, tmp, space );
				_x = x[i] + aStrLen(tmp, font, space );

				EffD.FireBallData[2].Show2(_x, y[i], FrameCount);
				EffD.FireBallData[2].CheckOut(FrameCount);

				return;
			}//  end if
		}//  end if pstr[i]
	}//  end for
}
