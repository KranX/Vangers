/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include "../global.h"

#include "ivmap.h"
#include "iworld.h"

#include "hfont.h"
#include "iscreen.h"
#include "i_mem.h"
#include "ikeys.h"

/* ----------------------------- EXTERN SECTION ----------------------------- */

extern iScreenDispatcher* iScrDisp;
extern char* iMouseBMP;

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void scale_buf(int sx,int sy,int sc,unsigned char* p,unsigned char* p1);
void dscale_buf(int sx,int sy,int sc,unsigned char* p,unsigned char* p1,int dx,int dy,int lev);
void put_buf(int x,int y,int sx,int sy,unsigned char* buf,unsigned char* mask,int lev,int hide_mode);
void put_tbuf(int x,int y,int sx,int sy,unsigned char* buf,unsigned char* mask,int lev,int hide_mode,int terr);
void add_buf(int x,int y,int sx,int sy,unsigned char* buf,int lev);

void put_fon(int x,int y,int sx,int sy,unsigned char* buf);
void put_attr_fon(int x,int y,int sx,int sy,unsigned char* buf);
void put_terr_fon(int x,int y,int sx,int sy,unsigned char* buf);
void get_fon(int x,int y,int sx,int sy,unsigned char* buf);
void get_terr_fon(int x,int y,int sx,int sy,unsigned char* buf);

void put_level(int x,int y,int sx,int sy,int lev);
void get_buf(int x,int y,int sx,int sy,unsigned char* buf);
void get_col_buf(int x,int y,int sx,int sy,unsigned char* buf);
void iPutStr(int x,int y,int fnt,unsigned char* str,int mode,int scale,int space,int level,int hide_mode);
int iStrLen(unsigned char* p,int f,int space);

void put_map(int x,int y,int sx,int sy);
void change_terrain(int x,int y,int sx,int sy,int terr1,int terr2);

void map_init(void);

#ifdef _DEBUG
void map_rectangle(int x,int y,int sx,int sy,int col);
#endif

/* --------------------------- DEFINITION SECTION --------------------------- */

void map_init(void)
{
	ivMap -> accept(0,I_RES_Y - 1);
//	ivMap -> render(0,I_RES_Y - 1);
}

void scale_buf(int sx,int sy,int sc,unsigned char* p,unsigned char* p1)
{
	int i,j,index = 0,t,t1;
	if(sc == 256){
		for(i = 0; i < sy; i ++){
			for(j = 0; j < sx; j ++){
				p1[index] = p[index];
				index ++;
			}
		}
	}
	else {
		for(i = 0; i < sy; i ++){
			for(j = 0; j < sx; j ++){
				t = p[index] - HFONT_NULL_LEVEL;
				t1 = (t * sc) >> 8;
				p1[index] = t1 + HFONT_NULL_LEVEL;
				index ++;
			}
		}
	}
}

void dscale_buf(int sx,int sy,int sc,unsigned char* p,unsigned char* p1,int dx,int dy,int lev)
{
	int i,j,index = 0,t,t1,ey = sy - dy;
	if(sc == 256){
		for(i = 0; i < ey; i ++){
			for(j = 0; j < sx - 1; j ++){
				p1[index + j] = p[index + j];
			}
			p1[index + sx - 1] = p[index + sx - 1];
			index += sx;
		}
	}
	else {
		for(i = 0; i < ey; i ++){
			for(j = 0; j < dx; j ++){
				p1[index + j] = p[index + j];
			}
			for(j = dx; j < sx - 1; j ++){
				t = p[index + j] - HFONT_NULL_LEVEL;
				t1 = (t * sc) >> 8;
				p1[index + j] = t1 + HFONT_NULL_LEVEL + lev;
			}
			p1[index + sx - 1] = p[index + sx - 1];
			index += sx;
		}
	}
}

void put_buf(int x,int y,int sx,int sy,unsigned char* buf,unsigned char* mask,int lev,int hide_mode)
{
//std::cout<<"put_buf "<<hide_mode<<std::endl;
	int i,j,index = 0;
	int t,m;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	switch(hide_mode){
		case 0:
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					t = buf[index] - HFONT_NULL_LEVEL;
					m = mask[index] - HFONT_NULL_LEVEL;
					if(t || m){
						t += lev;
						if(t < 0) t = 0;
						else if(t > 255) t = 255;
						lta[yy][xx] = t;
					}
					index ++;
				}
			}
			break;
		case 1:
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					t = buf[index] - HFONT_NULL_LEVEL;
					if(t){
						t += lev;
						if(t < 0) t = 0;
						else if(t > 255) t = 255;
						lta[yy][xx] = t;
					}
					index ++;
				}
			}
			break;
		case 2:
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					t = buf[index] - HFONT_NULL_LEVEL;
					t += lev;
					if(t < 0) t = 0;
					else if(t > 255) t = 255;
					lta[yy][xx] = t;
					index ++;
				}
			}
			break;
	}
}

void change_terrain(int x,int y,int sx,int sy,int terr1,int terr2)
{
	int i,j,t,yy,xx;
	unsigned char** lta = ivMap -> lineT;

	for(j = 0; j < sy; j ++){
		yy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			t = GET_TERRAIN(lta[yy][xx + H_SIZE]);
			if(t == terr1){
				SET_TERRAIN(lta[yy][xx + H_SIZE],terr2 << TERRAIN_OFFSET);
			}
		}
	}
}

void put_tbuf(int x,int y,int sx,int sy,unsigned char* buf,unsigned char* mask,int lev,int hide_mode,int terr)
{
	int i,j,index = 0;
	int t,m;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	switch(hide_mode){
		case 0:
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					t = buf[index] - HFONT_NULL_LEVEL;
					m = mask[index] - HFONT_NULL_LEVEL;
					if(t || m){
						t += lev;
						lta[yy][xx] = (t < 0) ? 0 : t;
						SET_TERRAIN(lta[yy][xx + H_SIZE],terr << TERRAIN_OFFSET);
					}
					index ++;
				}
			}
			break;
		case 1:
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					t = buf[index] - HFONT_NULL_LEVEL;
					if(t){
						t += lev;
						lta[yy][xx] = (t < 0) ? 0 : t;
						SET_TERRAIN(lta[yy][xx + H_SIZE],terr << TERRAIN_OFFSET);
					}
					index ++;
				}
			}
			break;
		case 2:
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					t = buf[index] - HFONT_NULL_LEVEL;
					t += lev;
					lta[yy][xx] = (t < 0) ? 0 : t;
					index ++;
				}
			}
			break;
	}
}

void add_buf(int x,int y,int sx,int sy,unsigned char* buf,int lev)
{
	int i,j,index = 0;
	int t,t1;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	for(j = 0; j < sy; j ++){
		yy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			t = buf[index];
			if(t){
				t1 = lta[yy][xx] + t - lev;
				if(t1 < 0) t1 = 0;
				if(t1 > 255) t1 = 255;
				lta[yy][xx] = t1;
			}
			index ++;
		}
	}
}

void put_buf2col(int x,int y,int sx,int sy,unsigned char* buf,int null_lev,int n_mode)
{
	int i,j,index = 0;
	unsigned t,l,n = null_lev;
	unsigned char** lta = ivMap -> lineT;
	unsigned char** ltc = ivMap -> lineTcolor;
	int yy,xx;

	if(!n_mode){
		if(n > 255){
			for(j = 0; j < sy; j ++){
				yy = y + j;
				for(i = 0; i < sx; i ++){
					t = buf[index];
					if(t) ltc[yy][x + i] = t;
					index ++;
				}
			}
		}
		else {
			for(j = 0; j < sy; j ++){
				yy = y + j;
				for(i = 0; i < sx; i ++){
					xx = x + i;
					t = buf[index];
					l = lta[yy][xx];
					if(t && n > l)
						ltc[yy][xx] = t;

					index ++;
				}
			}
		}
	}
	else {
		if(n_mode == 1){
			for(j = 0; j < sy; j ++){
				yy = y + j;
				for(i = 0; i < sx; i ++){
					xx = x + i;
					l = lta[yy][xx];
					if(n > l)
						ltc[yy][xx] = buf[index];

					index ++;
				}
			}
		}
		else {
			for(j = 0; j < sy; j ++){
				yy = y + j;
				for(i = 0; i < sx; i ++){
					xx = x + i;
					ltc[yy][xx] = buf[index];

					index ++;
				}
			}
		}
	}
}

void put_frame(int x,int y,int sx,int sy,unsigned char* buf,int null_lev,int n_mode)
{
	int i,j,index = 0,scr_offs = iScreenOffs;
	unsigned t,l,n = null_lev;
	unsigned char** lta = ivMap -> lineT;
	unsigned char** ltc = ivMap -> lineTcolor;
	int yy,xx;

	if(!n_mode){
		for(j = 0; j < sy; j ++){
			yy = iYCYCL(y + j);
			for(i = 0; i < sx; i ++){
				xx = iXCYCL(x + i);
				if(xx >= scr_offs && xx < scr_offs + I_RES_X){
					t = buf[index];
					l = lta[yy][xx];
					if(t && n > l)
						XGR_SetPixel(xx - scr_offs,yy,t);
					else
						XGR_SetPixel(xx - scr_offs,yy,ltc[yy][xx]);
				}
				index ++;
			}
		}
	}
	else {
		for(j = 0; j < sy; j ++){
			yy = iYCYCL(y + j);
			for(i = 0; i < sx; i ++){
				xx = iXCYCL(x + i);
				if(xx >= scr_offs && xx < scr_offs + I_RES_X){
					t = buf[index];
					l = lta[yy][xx];
					if(n > l)
						XGR_SetPixel(xx - scr_offs,yy,t);
					else
						XGR_SetPixel(xx - scr_offs,yy,ltc[yy][xx]);
				}
				index ++;
			}
		}
	}
}

void put_sframe(int x,int y,int sx,int sy,unsigned char* buf,int null_lev,char* shape,int n_mode)
{
	int i,j,s = 0,index = 0;
	int t = 0,l;
	unsigned char** lta = ivMap -> lineT;
	unsigned char** ltc = ivMap -> lineTcolor;
	int yy,xx;

	int scr_offs = iScreenOffs;

	if(shape){
		if(!n_mode){
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					if(xx >= scr_offs && xx < scr_offs + I_RES_X){
						s = shape[index];
						if(s){
							if(s != 1){
								t = buf[index];
								l = ltc[yy][xx] - s;
								if(l < 0) l = 0;
								if(t && null_lev > l)
									XGR_SetPixel(xx,yy,l);
								else
									XGR_SetPixel(xx,yy,ltc[yy][xx]);
							}
							else {
								t = buf[index];
								l = lta[yy][xx];
								if(t && null_lev > l)
									XGR_SetPixel(xx,yy,t);
								else
									XGR_SetPixel(xx,yy,ltc[yy][xx]);
							}
						}
						else
							XGR_SetPixel(xx,yy,ltc[yy][xx]);
					}

					index ++;
				}
			}
		}
		else {
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					if(xx >= scr_offs && xx < scr_offs + I_RES_X){
						s = shape[index];
						if(s){
							if(s != 1){
								l = ltc[yy][xx] - s;
								if(l < 0) l = 0;
								if(null_lev > l)
									XGR_SetPixel(xx,yy,l);
								else
									XGR_SetPixel(xx,yy,ltc[yy][xx]);
							}
							else {
								t = buf[index];
								l = lta[yy][xx];
								if(null_lev > l)
									XGR_SetPixel(xx,yy,t);
								else
									XGR_SetPixel(xx,yy,ltc[yy][xx]);
							}
						}
						else
							XGR_SetPixel(xx,yy,ltc[yy][xx]);
					}
					index ++;
				}
			}
		}
	}
	else {
		if(!n_mode){
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					if(xx >= scr_offs && xx < scr_offs + I_RES_X){
						t = buf[index];
						l = lta[yy][xx];
						if(t && null_lev > l)
							XGR_SetPixel(xx,yy,t);
						else
							XGR_SetPixel(xx,yy,ltc[yy][xx]);
					}

					index ++;
				}
			}
		}
		else {
			for(j = 0; j < sy; j ++){
				yy = iYCYCL(y + j);
				for(i = 0; i < sx; i ++){
					xx = iXCYCL(x + i);
					if(xx >= scr_offs && xx < scr_offs + I_RES_X){
						t = buf[index];
						l = lta[yy][xx];
						if(null_lev > l)
							XGR_SetPixel(xx,yy,t);
						else
							XGR_SetPixel(xx,yy,ltc[yy][xx]);
					}
					index ++;
				}
			}
		}
	}
}

void put_frame2scr(int x,int y,int sx,int sy,unsigned char* buf)
{
	x -= iScreenOffs;
	XGR_PutSpr(x,y,sx,sy,buf,XGR_BLACK_FON);
}

void put_buf2buf(int x,int y,int sx,int sy,int bsx,int bsy,unsigned char* buf,unsigned char* buf_to)
{
	int i,scr_index,index,start_x,start_y,end_x,end_y,size_x;

	start_x = (x < 0) ? 0 : x;
	start_y = (y < 0) ? 0 : y;
	end_x = ((x + sx) > bsx) ? bsx : (x + sx);
	end_y = ((y + sy) > bsy) ? bsy : (y + sy);

	size_x = end_x - start_x;
	index = start_x - x + (start_y - y) * sx;
	scr_index = start_x + start_y * bsx;

	if(size_x <= 0)
		return;

	for(i = start_y; i < end_y; i ++){
		memcpy(buf_to + scr_index,buf + index,size_x);
		scr_index += bsx;
		index += sx;
	}
}

void h_put_buf2buf(int x,int y,int sx,int sy,int bsx,int bsy,unsigned char* buf,unsigned char* buf_to)
{
	int i,j,scr_index,index,start_x,start_y,end_x,end_y,size_x;
	int c = 0;

	start_x = (x < 0) ? 0 : x;
	start_y = (y < 0) ? 0 : y;
	end_x = ((x + sx) > bsx) ? bsx : (x + sx);
	end_y = ((y + sy) > bsy) ? bsy : (y + sy);

	size_x = end_x - start_x;
	index = -x + (start_y - y) * sx;
	scr_index = start_y * bsx;

	if(size_x <= 0)
		return;

	for(i = start_y; i < end_y; i ++){
		for(j = start_x; j < end_x; j ++){
			c = buf[index + j];
			if(c != HFONT_NULL_LEVEL)
				buf_to[scr_index + j] = c;
		}
		scr_index += bsx;
		index += sx;
	}
}

void get_fon(int x,int y,int sx,int sy,unsigned char* buf)
{
	int i,j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	for(j = 0; j < sy; j ++){
		yy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			buf[index + i] = lta[yy][xx];
		}
		index += sx;
	}
}

void get_terr_fon(int x,int y,int sx,int sy,unsigned char* buf)
{
	int i,j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	for(j = 0; j < sy; j ++){
		yy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			buf[index + i] = lta[yy][xx + H_SIZE];
		}
		index += sx;
	}
}

void put_fon(int x,int y,int sx,int sy,unsigned char* buf)
{
	int j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	for(j = 0; j < sy; j ++){
		memcpy(lta[y + j] + x,buf + index,sx);
		index += sx;
	}
}

void put_attr_fon(int x,int y,int sx,int sy,unsigned char* buf)
{
	int j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	for(j = 0; j < sy; j ++){
		memcpy(lta[y + j] + x + H_SIZE,buf + index,sx);
		index += sx;
	}
}

void put_terr_fon(int x,int y,int sx,int sy,unsigned char* buf)
{
	int i,j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	for(j = 0; j < sy; j ++){
		yy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			lta[yy][xx + H_SIZE] = buf[index + i];
		}
		index += sx;
	}
}

/*
void put2fon(int x,int y,int sx,int sy,unsigned char* buf,int lev)
{
	int _x,_y,tmp,index,buf_index;

	index = x + y * I_MAP_RES_X;
	buf_index = 0;

	for(_y = 0; _y < sy; _y ++){
		for(_x = 0; _x < sx; _x ++){
			tmp = buf[buf_index + _x] - HFONT_NULL_LEVEL + lev;
			if(tmp < 0) tmp = 0;
			iScrDisp -> FonData[index + _x] = tmp;
		}
		index += I_MAP_RES_X;
		buf_index += sx;
	}
}
*/

void put_map(int x,int y,int sx,int sy)
{
	int j;
	unsigned char** ltc = ivMap -> lineTcolor;
	int yy,x0,x1,bsx,index;

	int scr_offs = iScreenOffs;

	x0 = x;
	x1 = x + sx;

	if(x0 < scr_offs) x0 = scr_offs;
	if(x1 >= scr_offs + I_RES_X) x1 = scr_offs + I_RES_X;
	bsx = x1 - x0;

	if(bsx > 0){
		index = (x0 - scr_offs) + y * XGR_MAXX;

		for(j = 0; j < sy; j ++){
			yy = iYCYCL(y + j);
			unsigned char* dest = XGR_VIDEOBUF + index;
			if(ltc[yy] == NULL){
				memset(dest, 0, bsx);
			}else{
				auto* source = ltc[yy] + x0;
				memcpy(dest, source, bsx);
			}
			index += XGR_MAXX;
		}
	}
}

void put_level(int x,int y,int sx,int sy,int lev)
{
	int i,j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	int yy,xx;

	for(j = 0; j < sy; j ++){
		yy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			lta[yy][xx] = lev;
			index ++;
		}
	}
}

void get_buf(int x,int y,int sx,int sy,unsigned char* buf)
{
	int i,j,index = 0;
	unsigned char** lta = ivMap -> lineT;
	int vy,xx;

	for(j = 0; j < sy; j ++){
		vy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			buf[index] = *(lta[vy] + xx);
			index ++;
		}
	}
}

void get_col_buf(int x,int y,int sx,int sy,unsigned char* buf)
{
	int i,j,index = 0;
	unsigned char** ltc = ivMap -> lineTcolor;
	int vy,xx;

	for(j = 0; j < sy; j ++){
		vy = iYCYCL(y + j);
		for(i = 0; i < sx; i ++){
			xx = iXCYCL(x + i);
			buf[index] = *(ltc[vy] + xx);
			index ++;
		}
	}
}

void iPutStr(int x,int y,int fnt,unsigned char* str,int mode,int scale,int space,int level,int hide_mode)
{
	int _x,_y,i,sx,sy,ss,sz = strlen((char*)str);
	HFont* p = HFntTable[fnt];
	HChar* ch;
	unsigned char* ptr,*ptr1;

	_x = x;
	_y = y;
	for(i = 0; i < sz; i ++){
		ch = p -> data[str[i]];
		ptr = ch -> HeightMap;

		sx = ch -> SizeX;
		sy = ch -> SizeY;
		ss = sx * sy;

		if(str[i] == '\n'){
			_x = x;
			_y += sy + space;
		}
		else {
			_iALLOC_A_(unsigned char,ptr1,ss);
			scale_buf(sx,sy,scale,ptr,ptr1);
			put_buf(_x - ch -> LeftOffs,_y,sx,sy,ptr1,ptr,level,hide_mode);
			_iFREE_A_(unsigned char,ptr1,ss);
			_x += sx - ch -> RightOffs - ch -> LeftOffs + space;
		}
	}
}

void i_terrPutStr(int x,int y,int fnt,unsigned char* str,int mode,int scale,int space,int level,int hide_mode,int terr)
{
	int _x,_y,i,sx,sy,ss,sz = strlen((char*)str);
	HFont* p = HFntTable[fnt];
	HChar* ch;
	unsigned char* ptr,*ptr1;

	_x = x;
	_y = y;
	for(i = 0; i < sz; i ++){
		ch = p -> data[str[i]];
		ptr = ch -> HeightMap;

		sx = ch -> SizeX;
		sy = ch -> SizeY;
		ss = sx * sy;

		if(str[i] == '\n'){
			_x = x;
			_y += sy + space;
		}
		else {
			_iALLOC_A_(unsigned char,ptr1,ss);
			scale_buf(sx,sy,scale,ptr,ptr1);
			put_tbuf(_x - ch -> LeftOffs,_y,sx,sy,ptr1,ptr,level,hide_mode,terr);
			_iFREE_A_(unsigned char,ptr1,ss);
			_x += sx - ch -> RightOffs - ch -> LeftOffs + space;
		}
	}
}

void iPutStr2buf(int x,int y,int fnt,int bsx,int bsy,unsigned char* str,unsigned char* buf_to,int mode,int scale,int space)
{
	int _x,_y,i,sx,sy,ss,sz = strlen((char*)str);
	HFont* p = HFntTable[fnt];
	HChar* ch;
	unsigned char* ptr;

	_x = x;
	_y = y;
	for(i = 0; i < sz; i ++){
		ch = p -> data[str[i]];
		ptr = ch -> HeightMap;

		sx = ch -> SizeX;
		sy = ch -> SizeY;
		ss = sx * sy;

		if(str[i] == '\n'){
			_x = x;
			_y += sy + space;
		}
		else {
			h_put_buf2buf(_x - ch -> LeftOffs,_y,sx,sy,bsx,bsy,ptr,buf_to);
			_x += sx - ch -> RightOffs - ch -> LeftOffs + space;
		}
	}
}

int iStrLen(unsigned char* p,int f,int space)
{
	int i,len = 0,sz = strlen((char*)p);
	unsigned char s;

	HFont* fnt = HFntTable[f];

	for(i = 0; i < sz; i ++){
		s = p[i];
		len += space + fnt -> SizeX - (fnt -> data[s] -> LeftOffs + fnt -> data[s] -> RightOffs);
	}
	return len;
}

int iS_StrLen(unsigned char* str,int font,int space)
{
	int s,sz = strlen((char*)str),len = 0;
	for(s = 0; s < sz; s ++)
		len += iScrFontTable[font] -> SizeX - (iScrFontTable[font] -> LeftOffs[str[s]] + iScrFontTable[font] -> RightOffs[str[s]]) + space;
	return len;
}

void iPutS_Str(int x,int y,int font,int color,unsigned char* str,int bsx,int bsy,unsigned char* buf,int space)
{
	int i,j,s,xs,ys,offs = 0,sz = strlen((char*)str),X = 0,col = color;
	int buf_index = 0,buf_offs = 0;

	if(iScrFontTable[font] == NULL)
		ErrH.Abort("MISSING FONT...");

	char* p = iScrFontTable[font] -> data;
	xs = iScrFontTable[font] -> SizeX;
	ys = iScrFontTable[font] -> SizeY;

	buf_index = x + y * bsx;

	for(s = 0; s < sz; s ++){
		buf_offs = 0;
		offs = ys * str[s];

		col = color;

		X -= iScrFontTable[font] -> LeftOffs[str[s]];

		offs = ys * str[s];
		for(i = 0; i < ys; i ++){
			for(j = 0; j < xs; j ++){
				if(p[offs] & (1 << (7 - j)))
					buf[X + j + buf_index + buf_offs] = col;
			}
			offs ++;
			buf_offs += bsx;
		}
		X += xs + space - iScrFontTable[font] -> RightOffs[str[s]];
	}
/*
	XStream fh("s_str.bmp",XS_OUT);
	fh < (short)bsx < (short)bsy;
	fh.write(buf,bsx * bsy);
	fh.close();
*/
}

#ifdef _DEBUG
void map_rectangle(int x,int y,int sx,int sy,int col)
{
	int i,scr_offs = iScreenOffs;
	unsigned char** ltc = ivMap -> lineTcolor;
	int yy,xx;

	yy = iYCYCL(y);
	for(i = 0; i < sx; i ++){
		xx = iXCYCL(x + i);
		if(xx >= scr_offs && xx < scr_offs + I_RES_X){
			ltc[yy][xx] = col;
		}
	}
	yy = iYCYCL(y + sy);
	for(i = 0; i < sx; i ++){
		xx = iXCYCL(x + i);
		if(xx >= scr_offs && xx < scr_offs + I_RES_X){
			ltc[yy][xx] = col;
		}
	}

	xx = iXCYCL(x);
	if(xx >= scr_offs && xx < scr_offs + I_RES_X){
		for(i = 0; i < sy; i ++){
			yy = iYCYCL(y + i);
			ltc[yy][xx] = col;
		}
	}
	xx = iXCYCL(x + sx);
	if(xx >= scr_offs && xx < scr_offs + I_RES_X){
		for(i = 0; i < sy; i ++){
			yy = iYCYCL(y + i);
			ltc[yy][xx] = col;
		}
	}
}
#endif
