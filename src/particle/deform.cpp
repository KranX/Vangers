#include "../global.h"

#include "../3d/3d_math.h"

#include "../common.h"
#include "../backg.h"
#include "../sqexp.h"

#include "../terra/vmap.h"

#include "particle.h"
#include "partmap.h"

//int* X_MAXX_TABLE = 0;
//const X_MAXX_AMPL = 20;

// For set dirty
extern iGameMap* curGMap;


void Mask::finit( void ){
	if( offset ){
		delete[] offset;
		offset = NULL;
	};
	if( buffer ){
		delete buffer;
		buffer = NULL;
	};
}

Mask::Mask(void)
{
	offset = NULL;
	buffer = NULL;
};

void Mask::load(const char* n){
	strcpy(MSSfile,n);
	XStream f(n,XS_IN);
	f > n_frame > mx > my > bx > by;
	offset = new TABLE_TYPE[mx*my*n_frame];
	f.read(offset,sizeof(TABLE_TYPE)*mx*my*n_frame);
	buffer = new unsigned char [mx*my];
/*
	if(!X_MAXX_TABLE){
		X_MAXX_TABLE = new int[X_MAXX_AMPL + 1];
		for(int i = -X_MAXX_AMPL;i <= X_MAXX_AMPL;i++){
			X_MAXX_TABLE[i + X_MAXX_AMPL] = 640*i;
			}
		X_MAXX_TABLE += X_MAXX_AMPL;
		}
*/
}

void Mask::load_to_flame(char* n){
	strcpy(MSSfile,n);
	XStream f(n,XS_IN);
	f > n_frame > mx > my > bx > by;
	offset = new TABLE_TYPE[mx*my*n_frame];
	f.read(offset,sizeof(TABLE_TYPE)*mx*my*n_frame);
	buffer = 0;
}

/*
void Mask::deform(int fr,unsigned char* from,unsigned char* to)
{
	fr = fr % (2*n_frame);
	if(fr >= n_frame)
		return;
	unsigned char* boff = from;
	TABLE_TYPE* moff = offset + fr*mx*my;
	unsigned char* buffer_ptr = to;
	int i_max = mx*my;
	for(int i = 0;i < i_max;i++){
		boff += *moff++;
//		  *buffer_ptr++ = *boff;
		*buffer_ptr++ = 0;
		}
}
*/
void Mask::deformBuff(int fr,int c_x,int c_y,unsigned char* buff){
	char* vb = (char*)XGR_VIDEOBUF;
	fr = fr % (2*n_frame);
	if(fr >= n_frame)
		fr = 2*n_frame - 1 - fr;
	unsigned char* boff = (unsigned char*)vb + c_y*XGR_MAXX + (c_x);
	TABLE_TYPE* moff = offset + fr*mx*my;
	unsigned char* buffer_ptr = buff + mx;
	short cell;
	char dx,dy;
	int x;
	int y;
	int x1 = c_x + mx;
	int y1 = c_y + my;
	uchar** ltc = vMap -> lineTcolor;
	for(y = c_y;y < y1;y++){
		curGMap->set_durty(y);
		for(x = c_x;x < x1;x++){
			cell = *moff++;
			dx = (char)cell;
			dy = cell >> 8;
#ifndef EXTERNAL_USE
			*buffer_ptr++ = *(vb + (x + dx) + (y + dy)*C_MAXX);
#else
//			  *buffer_ptr++ = *(ltc[y + dy] + x + dx) & 31;
			*buffer_ptr++ = *(ltc[y + dy] + x + dx);
#endif
			}
		}
	buffer_ptr = buff + mx;

	for(y = c_y;y < y1;y++){
#ifndef EXTERNAL_USE
		boff = (unsigned char*)vb + y*C_MAXX + c_x;
#else
		boff = ltc[y] + c_x;
#endif
		memcpy(buffer_ptr - mx,boff,mx);
		memcpy(boff,buffer_ptr,mx);
		buffer_ptr += mx;
		}
}
//void Mask::refresh(int x0,int y0)
//{
//	  unsigned char* boff = data + x0 + sx*y0;
//	  for(int y = y0;y < y0 + my;y++){
//		  for(int x = x0;x < x0 + mx;x++){
//			  XGR_SetPixel(x,y,*boff++);
//			  }
//		  boff += sx - mx;
//		  }
//}
void DeformProcess::init(){
	if(!DeformMask){
		DeformMask = new Mask;
		DeformMask -> load("wave.mss");
		}
	buffer = new unsigned char [DeformMask -> mx*(DeformMask -> my + 1)];
	phase = 0;
}

void DeformProcess::activate(){
	phase = 0;
}

int DeformProcess::deform(int d_phase,int x,int y){
	OrX = x - (DeformMask -> mx >> 1);
	OrY = y - (DeformMask -> my >> 1);

	DeformMask -> deformBuff(phase,OrX,OrY,buffer);
	phase += d_phase;
#ifdef EXTERNAL_USE
	BackD.put(this);
#endif
	return phase >= DeformMask -> n_frame;
}

void DeformProcess::BackRestore(void){
	DeformMask -> BackRestore(OrX,OrY,buffer);
}


void Mask::BackRestore(int c_x,int c_y,unsigned char* buff){
	unsigned char* buffer_ptr = buff;
	int y1 = c_y + my;
	unsigned char* boff = 0;
	uchar** ltc = vMap -> lineTcolor;
	for(int y = c_y;y < y1;y++){
#ifdef EXTERNAL_USE
		boff = ltc[y] + c_x;
#endif
		memcpy(boff,buffer_ptr,mx);
		buffer_ptr += mx;
		curGMap->set_durty(y);
		}
}

Mask* DeformProcess::DeformMask = 0;
