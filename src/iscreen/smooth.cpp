
#include "../global.h"

#include "hfont.h"
#include "iscreen.h"
#include "i_mem.h"

const int 	MAX_SMOOTH_INDEX	= 20;
const int 	START_VAL		= 255;
const int 	END_VAL 		= 0;

const int 	VAL_STEP		= (START_VAL - END_VAL)/MAX_SMOOTH_INDEX;

void xy_smooth_quant(int sx,int sy,unsigned char* buf,unsigned char* buf1,int delta);
void s_xy_smooth_quant(int sx,int sy,unsigned char* buf,unsigned char* buf1,int delta,int start_col);
void smooth_shape_quant(int sx,int sy,unsigned char* buf,int src,int dest,int cnd);

void smooth_buf(int sx,int sy,unsigned char* buf,int steps)
{
	int sz = sx * sy;
	unsigned char* p;
	unsigned heap_offs = iHeapOffs;

	_iALLOC_A_(unsigned char,p,sx * sy);

	xy_smooth_quant(sx,sy,buf,p,steps);
	memcpy(buf,p,sz);

	iHeapOffs = heap_offs;
}

#define S_SMOOTH_DELTA	10

void s_smooth_buf(int sx,int sy,unsigned char* buf,int steps,int start_col)
{
	int i,col1,col2,cnd_col;

	col1 = 0;
	col2 = iS_STR_COL - S_SMOOTH_DELTA;
	cnd_col = iS_STR_COL;

	smooth_shape_quant(sx,sy,buf,col1,col2,cnd_col);

	for(i = 0; i < steps; i ++){
		cnd_col = col2;
		col2 -= S_SMOOTH_DELTA;
		if(col2 < start_col) col2 = 0;
		smooth_shape_quant(sx,sy,buf,col1,col2,cnd_col);
	}
/*
	int sz = sx * sy;
	unsigned char* p;
	unsigned heap_offs = iHeapOffs;

	_iALLOC_A_(unsigned char,p,sx * sy);

	s_xy_smooth_quant(sx,sy,buf,p,steps,start_col);
	memcpy(buf,p,sz);

	iHeapOffs = heap_offs;
*/
}

void smooth_shape(int sx,int sy,unsigned char* buf)
{
	int i;
	int sz = sx * sy;
	for(i = 0; i < sz; i ++){
		if(buf[i] > 20)
			buf[i] = 1;
		else
			buf[i] = 0;
	}
	smooth_shape_quant(sx,sy,buf,1,3,0);
	smooth_shape_quant(sx,sy,buf,1,4,3);
	smooth_shape_quant(sx,sy,buf,1,5,4);
	smooth_shape_quant(sx,sy,buf,1,6,5);
	smooth_shape_quant(sx,sy,buf,1,7,6);
	smooth_shape_quant(sx,sy,buf,1,8,7);
/*
	XStream fh("shp.bmp",XS_OUT);
	fh < (short)sx < (short)sy;
	fh.write(buf,sz);
	fh.close();
*/
}

void xy_smooth_quant(int sx,int sy,unsigned char* buf,unsigned char* buf1,int delta)
{
	int i,j,d,index = 0,offset = 0,sum = 0,sz,offs,doffs,size,ex,ey;

	sz = 1 << delta;
	size = sz >> 1;
	doffs = size * sx;
	ex = sx - size;
	ey = sy - size;

	for(i = 0; i < sy; i ++){
		sum = 0;
		for(d = 0; d < size; d ++)
			sum += buf[index + d] - HFONT_NULL_LEVEL;

		offset = index;
		if(size){
			for(j = 0; j < size; j ++){
				sum += buf[offset + size] - HFONT_NULL_LEVEL;
				buf1[offset] = (sum >> delta) + HFONT_NULL_LEVEL;
				offset ++;
			}
			for(j = size; j < ex; j ++){
				sum += buf[offset + size] - HFONT_NULL_LEVEL;
				sum -= buf[offset - size] - HFONT_NULL_LEVEL;
				buf1[offset] = (sum >> delta) + HFONT_NULL_LEVEL;
				offset ++;
			}
			for(j = ex; j < sx; j ++){
				sum -= buf[offset - size] - HFONT_NULL_LEVEL;
				buf1[offset] = (sum >> delta) + HFONT_NULL_LEVEL;
				offset ++;
			}
		}
		else {
			for(j = 0; j < sx; j ++){
				offset = index + j;
				buf1[offset] = buf[offset];
			}
		}
		index += sx;
	}

	index = 0;
	memcpy(buf,buf1,sx * sy);
	for(j = 0; j < sx; j ++){
		sum = offs = 0;
		for(d = 0; d < size; d ++){
			sum += buf[index + offs] - HFONT_NULL_LEVEL;
			offs += sx;
		}

		offs = 0;
		if(size){
			for(i = 0; i < size; i ++){
				offset = index + offs;
				sum += buf[offset + doffs] - HFONT_NULL_LEVEL;
				buf1[offset] = (sum >> delta) + HFONT_NULL_LEVEL;
				offs += sx;
			}
			for(i = size; i < ey; i ++){
				offset = index + offs;
				sum += buf[offset + doffs] - HFONT_NULL_LEVEL;
				sum -= buf[offset - doffs] - HFONT_NULL_LEVEL;
				buf1[offset] = (sum >> delta) + HFONT_NULL_LEVEL;
				offs += sx;
			}
			for(i = ey; i < sy; i ++){
				offset = index + offs;
				sum -= buf[offset - doffs] - HFONT_NULL_LEVEL;
				buf1[offset] = (sum >> delta) + HFONT_NULL_LEVEL;
				offs += sx;
			}
		}
		else {
			for(i = 0; i < sy; i ++){
				offset = index + offs;
				buf1[offset] = buf[offset];
				offs += sx;
			}
		}
		index ++;
	}
}

void s_xy_smooth_quant(int sx,int sy,unsigned char* buf,unsigned char* buf1,int delta,int start_col)
{
	int i,j,d,index = 0,offset = 0,sum = 0,sz,offs,doffs,size,ex,ey,t = 0;

	sz = 1 << delta;
	size = sz >> 1;
	doffs = size * sx;
	ex = sx - size;
	ey = sy - size;

	for(i = 0; i < sy; i ++){
		sum = 0;
		for(d = 0; d < size; d ++){
			t = buf[index + d] - start_col;
			if(t < 0) t = 0;
			sum += t;
		}

		offset = index;
		if(size){
			for(j = 0; j < size; j ++){
				t = buf[offset + size] - start_col;
				if(t < 0) t = 0;
				sum += t;

				buf1[offset] = (sum >> delta) + start_col;
				if(buf1[offset] == start_col) buf1[offset] = 0;
				offset ++;
			}
			for(j = size; j < ex; j ++){
				t = buf[offset + size] - start_col;
				if(t < 0) t = 0;
				sum += t;

				t = buf[offset - size] - start_col;
				if(t < 0) t = 0;
				sum -= t;

				buf1[offset] = (sum >> delta) + start_col;
				if(buf1[offset] == start_col) buf1[offset] = 0;
				offset ++;
			}
			for(j = ex; j < sx; j ++){
				t = buf[offset - size] - start_col;
				if(t < 0) t = 0;
				sum -= t;

				buf1[offset] = (sum >> delta) + start_col;
				if(buf1[offset] == start_col) buf1[offset] = 0;
				offset ++;
			}
		}
		else {
			for(j = 0; j < sx; j ++){
				offset = index + j;
				buf1[offset] = buf[offset];
			}
		}
		index += sx;
	}

	index = 0;
	memcpy(buf,buf1,sx * sy);
	for(j = 0; j < sx; j ++){
		sum = offs = 0;
		for(d = 0; d < size; d ++){
			t = buf[index + offs] - start_col;
			if(t < 0) t = 0;
			sum += t;
			offs += sx;
		}

		offs = 0;
		if(size){
			for(i = 0; i < size; i ++){
				offset = index + offs;
				t = buf[offset + doffs] - start_col;
				if(t < 0) t = 0;

				sum += t;
				buf1[offset] = (sum >> delta) + start_col;
				if(buf1[offset] == start_col) buf1[offset] = 0;
				offs += sx;
			}
			for(i = size; i < ey; i ++){
				offset = index + offs;
				t = buf[offset + doffs] - start_col;
				if(t < 0) t = 0;
				sum += t;

				t = buf[offset - doffs] - start_col;
				if(t < 0) t = 0;
				sum -= t;

				buf1[offset] = (sum >> delta) + start_col;
				if(buf1[offset] == start_col) buf1[offset] = 0;
				offs += sx;
			}
			for(i = ey; i < sy; i ++){
				offset = index + offs;

				t = buf[offset - doffs] - start_col;
				if(t < 0) t = 0;
				sum -= t;

				buf1[offset] = (sum >> delta) + start_col;
				if(buf1[offset] == start_col) buf1[offset] = 0;
				offs += sx;
			}
		}
		else {
			for(i = 0; i < sy; i ++){
				offset = index + offs;
				buf1[offset] = buf[offset];
				offs += sx;
			}
		}
		index ++;
	}
}

void smooth_shape_quant(int sx,int sy,unsigned char* buf,int src,int dest,int cnd)
{
	int i,j,index = 0,offset = sx * (sy - 1);

	for(i = 1; i < (sx - 1); i ++){
		if(buf[i] == src && buf[i + sx] == cnd){
			buf[i] = dest;
		}
		if(buf[i + offset] == src && buf[i + offset - sx] == cnd){
			buf[i + offset] = dest;
		}
	}

	index = offset = 0;
	for(j = 1; j < (sy - 1); j ++){
		offset += sx;
		for(i = 1; i < (sx - 1); i ++){
			index = offset + i;
			if(buf[index] == src){
				if(buf[index + sx] == cnd || buf[index - sx] == cnd)
					buf[index] = dest;
			}
		}
	}

	index = 0;
	offset = sx - 1;
	for(i = 0; i < (sy - 1); i ++){
		if(buf[index] == src && buf[index + 1] == cnd){
			buf[index] = dest;
		}
		if(buf[index + offset] == src && buf[index + offset - 1] == cnd){
			buf[index + offset] = dest;
		}
		index += sx;
	}
	offset = sx;
	for(i = 1; i < (sx - 1); i ++){
		for(j = 1; j < (sy - 1); j ++){
			index = offset + i;
			if(buf[index] == src){
				if(buf[index + 1] == cnd || buf[index - 1] == cnd)
					buf[index] = dest;
			}
			offset += sx;
		}
		offset = sx;
	}
}

