#include "../global.h"

#include "../3d/3d_math.h"

#include "../sqexp.h"
#include "../backg.h"
#include "../3d/3dgraph.h"

#include "../terra/vmap.h"

#include "particle.h"
#include "partmap.h"

extern uchar* FireColorTable;
extern unsigned char* draw_vbuf;
extern unsigned char** draw_lt;
extern int draw_k_xscr_x;
extern int draw_k_xscr_y;
unsigned char z_level;

#ifdef EXTERNAL_USE
	extern int UcutLeft;
	extern int UcutRight;
	extern int VcutUp;
	extern int VcutDown;
#else
	#define UcutLeft 200*0
	#define UcutRight (XGR_MAXX - 200*0)
	#define VcutUp 150*0
	#define VcutDown (XGR_MAXY - 150*0)
#endif

void transparency_line_non_test_f(int len,unsigned char* dbuf,int bx,int bKx);
void color_line_non_test_f(int len,unsigned char* dbuf,int bx,int bKx);
void transparency_line_f(int len,unsigned char* dbuf,int bx,int bKx,int fx,int fy);
void color_line_f(int len,unsigned char* dbuf,int bx,int bKx,int fx,int fy);

#define ParticlePaletteTable FirePaletteTable
void putBitmap2scrLine(int x,int xr_,int bx,int by,int bKx,int bKy,unsigned char* data,int FIRE_COLOR_FIRST,unsigned char* ParticlePaletteTable,unsigned char*& bGr,int MAX_SPRITE_POWER);

void bitmap2screenNoTurn(char* vb,unsigned char* data,int Xcenter,int Ycenter,int XsizeB,int YsizeB,int ScaleXsize);

/*void bitmap2screen(char* vb,unsigned char* data,int Xcenter,int Ycenter,int XsizeB,int YsizeB,int ScaleXsize,int turn)
{
#ifndef BMP2SCR_TURN_ENABLE
	bitmap2screenNoTurn(vb,data,Xcenter,Ycenter,XsizeB,YsizeB,ScaleXsize);
	return;
#else
	int MAX_SPRITE_POWER = 0;
	switch(XsizeB){
		case 1 << 0:
			MAX_SPRITE_POWER = 0;
			break;
		case 1 << 1:
			MAX_SPRITE_POWER = 1;
			break;
		case 1 << 2:
			MAX_SPRITE_POWER = 2;
			break;
		case 1 << 3:
			MAX_SPRITE_POWER = 3;
			break;
		case 1 << 4:
			MAX_SPRITE_POWER = 4;
			break;
		case 1 << 5:
			MAX_SPRITE_POWER = 5;
			break;
		case 1 << 6:
			MAX_SPRITE_POWER = 6;
			break;
		case 1 << 7:
			MAX_SPRITE_POWER = 7;
			break;
		case 1 << 8:
			MAX_SPRITE_POWER = 8;
			break;
		case 1 << 9:
			MAX_SPRITE_POWER = 9;
			break;
		case 1 << 10:
			MAX_SPRITE_POWER = 10;
			break;
		default:
			return;
		}
	int XsizeS = ScaleXsize;
	int YsizeS = ScaleXsize*YsizeB/XsizeB;
	int sinTurn,cosTurn;
	int bKx,bKy;
	int K0bx = 0,K0by = 0;
	int K0bx1 = 0,K0bx2 = 0;
	int K0by1 = 0,K0by2 = 0;
	int xl,xr;
	int bx,by;
	int X,Y;
	int x,y;
	int l,r,lx,rx;
	turn &= (PIx2 - 1);
#ifdef EXTERNAL_USE
	sinTurn = SI[turn] >> 1;
	cosTurn = CO[turn] >> 1;
#else
	sinTurn = SI[turn];
	cosTurn = CO[turn];
#endif
	int Xs2 = XsizeS/2;
	int Ys2 = YsizeS/2;
	X = (Xcenter << 15) + cosTurn*(-Xs2) - sinTurn*(-Ys2);
	Y = (Ycenter << 15) + sinTurn*(-Xs2) + cosTurn*(-Ys2);
	int Xx = cosTurn*XsizeS;		 //	  X	
	int Xy = sinTurn*XsizeS;		 //  0 3	 
	int Yx = -sinTurn*YsizeS;		 //	Y	 
	int Yy = cosTurn*YsizeS;		 //	 1 2	 

	int Xy_ = Xy >> 15;
	int Yy_ = Yy >> 15;
	bKx = cosTurn*XsizeB/ScaleXsize;
	bKy = -sinTurn*XsizeB/ScaleXsize;
	int Kl = 0,Kr = 0;
	int Kl1 = 0,Kr1 = 0;
	int Klr2 = 0;
	int Kl3 = 0,Kr3 = 0;
	int x2 = 0,x3 = 0;
	int bx1 = 0,bx2 = 0,bx3 = 0;
	int by1 = 0,by2 = 0,by3 = 0;
	int yt1,yt2,yt3;
	int v1st = 0;
	int l_longer = 0;
	int Xsize15 = XsizeB << 15;
	int Ysize15 = YsizeB << 15;
	int Xsize151 = (XsizeB) << 15;
	int Ysize151 = (YsizeB) << 15;
	if(Xy > 0){
		if(Yy > 0){
			v1st = 0;
			if(Yy_) Kl1 = Yx/Yy_;
			if(Xy_) Kr1 = Xx/Xy_;
			lx = X + Yx;
			rx = X + Xx;
			l = Y + Yy >> 15;
			r = Y + Xy >> 15;
			bx1 = 0;
			by1 = 0;
			K0bx1 = 0;
			if(Yy_) K0by1 = Ysize15/Yy_;
			if(Xy_) K0bx2 = Xsize15/Xy_;
			K0by2 = 0;
			if(Xy > Yy){
				x2 = lx;
				x3 = rx;
				yt1 = l;
				yt2 = r;
				Klr2 = Kr1;
				bx2 = 0;
				by2 = Ysize151;
				bx3 = Xsize151;
				by3 = 0;
				}
			else{
				l_longer = 1;
				x2 = rx;
				x3 = lx;
				yt1 = r;
				yt2 = l;
				Klr2 = Kl1;
				bx2 = Xsize151;
				by2 = 0;
				bx3 = 0;
				by3 = Ysize151;
				}
			yt3 = Y + Xy + Yy >> 15;
			}
		else{
			v1st = 1;
			X += Yx;
			Y += Yy;
			if(Xy_) Kl1 = Xx/Xy_;
			if(Yy_) Kr1 = Yx/Yy_;
			lx = X + Xx;
			rx = X - Yx;
			l = Y + Xy >> 15;
			r = Y - Yy >> 15;
			bx1 = 0;
			by1 = Ysize151;
			if(Xy_) K0bx1 = Xsize15/Xy_;
			K0by1 = 0;
			K0bx2 = 0;
			if(Yy_) K0by2 = Ysize15/Yy_;
			if(Xy > -Yy){
				l_longer = 1;
				x2 = rx;
				x3 = lx;
				yt1 = r;
				yt2 = l;
				Klr2 = Kl1;
				bx2 = 0;
				by2 = 0;
				bx3 = Xsize151;
				by3 = Ysize151;
				}
			else{
				x2 = lx;
				x3 = rx;
				yt1 = l;
				yt2 = r;
				Klr2 = Kr1;
				bx2 = Xsize151;
				by2 = Ysize151;
				bx3 = 0;
				by3 = 0;
				}
			yt3 = Y + Xy - Yy >> 15;
			}
		}
	else{
		if(Yy > 0){
			v1st = 3;
			X += Xx;
			Y += Xy;
			if(Xy_) Kl1 = Xx/Xy_;
			if(Yy_) Kr1 = Yx/Yy_;
			lx = X - Xx;
			rx = X + Yx;
			l = Y - Xy >> 15;
			r = Y + Yy >> 15;
			bx1 = Xsize151;
			by1 = 0;
			if(Xy_) K0bx1 = Xsize15/Xy_;
			K0by1 = 0;
			K0bx2 = 0;
			if(Yy_) K0by2 = Ysize15/Yy_;
			if(-Xy > Yy){
				l_longer = 1;
				x2 = rx;
				x3 = lx;
				yt1 = r;
				yt2 = l;
				Klr2 = Kl1;
				bx2 = Xsize151;
				by2 = Ysize151;
				bx3 = 0;
				by3 = 0;
				}
			else{
				x2 = lx;
				x3 = rx;
				yt1 = l;
				yt2 = r;
				Klr2 = Kr1;
				bx2 = 0;
				by2 = 0;
				bx3 = Xsize151;
				by3 = Ysize151;
				}
			yt3 = Y - Xy + Yy >> 15;
			}
		else{
			v1st = 2;
			X += Yx + Xx;
			Y += Yy + Xy;
			if(Yy_) Kl1 = Yx/Yy_;
			if(Xy_) Kr1 = Xx/Xy_;
			lx = X - Yx;
			rx = X - Xx;
			l = Y - Yy >> 15;
			r = Y - Xy >> 15;
			bx1 = Xsize151;
			by1 = Ysize151;
			K0bx1 = 0;
			if(Yy_) K0by1 = Ysize15/Yy_;
			if(Xy_) K0bx2 = Xsize15/Xy_;
			K0by2 = 0;
			if(-Xy > -Yy){
				x2 = lx;
				x3 = rx;
				yt1 = l;
				yt2 = r;
				Klr2 = Kr1;
				bx2 = Xsize151;
				by2 = 0;
				bx3 = 0;
				by3 = Ysize151;
				}
			else{
				l_longer = 1;
				x2 = rx;
				x3 = lx;
				yt1 = r;
				yt2 = l;
				Klr2 = Kl1;
				bx2 = 0;
				by2 = Ysize151;
				bx3 = Xsize151;
				by3 = 0;
				}
			yt3 = Y - Xy - Yy >> 15;
			}
		}
	int xl_,xr_,yt = 0;
	int Lbx = 0,Lby = 0;
	y = Y >> 15;
	xl = xr = X + (1 << 14);
	for(int pass = 0;pass < 3;pass++){
		switch(pass){
			case 0:
				yt = yt1;
				Kl = Kl1;
				Kr = Kr1;
				Lbx = bx1;
				Lby = by1;
				K0bx = K0bx1;
				K0by = K0by1;
				break;
			case 1:
				if(l_longer)
					xr = x2 + (1 << 14);
				else{
					xl = x2 + (1 << 14);
					K0bx = K0bx2;
					K0by = K0by2;
					Lbx = bx2;
					Lby = by2;
					}
				yt = yt2;
				Kl = Kr = Klr2;
				break;
			case 2:
				if(l_longer){
					K0bx = K0bx2;
					K0by = K0by2;
					Lbx = bx3;
					Lby = by3;
					xl = x3 + (1 << 14);
					}
				else
					xr = x3 + (1 << 14);
				yt = yt3;
				Kl = Kr1;
				Kr = Kl1;
				break;
			}
		if(yt > VcutDown)
			yt = VcutDown;
		int y_min = VcutUp < yt ? VcutUp : yt;
		for(;y < y_min;y++){
			xl += Kl;
			xr += Kr;
			Lbx += K0bx;
			Lby += K0by;
			}
		for(;y < yt;y++){
			xl_ = xl >> 15;
			xr_ = xr >> 15;
			xl += Kl;
			xr += Kr;
			bx = Lbx;
			by = Lby;
			Lbx += K0bx;
			Lby += K0by;
			if (bx < 0) bx = 0;
			if (bx >= Xsize15)
				bx = Xsize15 - 1;
			if (by < 0) by = 0;
			if (by >= Ysize15)
				by = Ysize15 - 1;

#ifdef OVERLAP_CHECK
			if(AutoTurn){
#endif
			int xr_xl_ = xr_ - xl_;
			if(xr_xl_){
				int tmp = bx + bKx*(xr_xl_ - 1);
				if(tmp >= Xsize15)
					bKx = ((Xsize15 - 1) - bx)/(xr_xl_);
				else if(tmp < 0)
					bKx = (-bx)/(xr_xl_);
				tmp = by + bKy*(xr_xl_ - 1);
				if(tmp >= Ysize15)
					bKy = ((Ysize15 - 1) - by)/(xr_xl_);
				else if(tmp < 0)
					bKy = (-by)/(xr_xl_);
				}
#ifdef OVERLAP_CHECK
				}
#endif
			if(xr_ > UcutRight) xr_ = UcutRight;
			for(x = xl_;x < UcutLeft;x++){
				bx += bKx;
				by += bKy;
				}
			unsigned char* bGr = (unsigned char*)vb + y*XGR_MAXX + (x);
#define ASM_OPTIMIZE
#ifdef ASM_OPTIMIZE
			putBitmap2scrLine(x,xr_,bx,by,bKx,bKy,data,FIRE_COLOR_FIRST,ParticlePaletteTable,bGr,MAX_SPRITE_POWER);
#else
			for(;x < xr_;x++){
				int bx_ = bx >> 15;
				int by_ = by >> 15;
				bx += bKx;
				by += bKy;
#ifdef OVERLAP_CHECK
				if(bx_ >= 0 && bx_ < XsizeB && by_ >= 0 && by_ < YsizeB){
#endif
					unsigned char c = *(data + (by_ << MAX_SPRITE_POWER) + bx_);
					if(c || AutoTurn){
						c = FIRE_COLOR_FIRST + ParticlePaletteTable[*bGr + (c << 8)];
						*bGr = c;
//						  XGR_SetPixel(x,y,c);
						}
#ifdef OVERLAP_CHECK
					}
				else{
					*bGr = 222;
					}
#endif
				bGr++;
				}
#endif
			}

		}
#endif
}

void putBitmap2scrLine(int x,int xr_,int bx,int by,int bKx,int bKy,unsigned char* data,int FIRE_COLOR_FIRST,unsigned char* ParticlePaletteTable,unsigned char*& bGr,int MAX_SPRITE_POWER)
{
	for(;x < xr_;x++){
		int bx_ = bx >> 15;
		int by_ = by >> 15;
		bx += bKx;
		by += bKy;
		unsigned char c = *(data + (by_ << MAX_SPRITE_POWER) + bx_);
		if(c){
//			c = FIRE_COLOR_FIRST + ParticlePaletteTable[*bGr + (c << 8)];
			c = FireColorTable[*bGr + (c << 11)];
			*bGr = c;
			}
		bGr++;
		}
}

void bitmap2screenNoTurn(char* vb,unsigned char* data,int Xcenter,int Ycenter,int XsizeB,int YsizeB,int ScaleXsize)
{
	int XsizeS = ScaleXsize;
	int YsizeS = ScaleXsize*YsizeB/XsizeB;
	int bKx,bKy;
	int X,Y;
	int x,y;
	X = Xcenter - XsizeS/2;
	Y = Ycenter - YsizeS/2;
	bKx = bKy = (XsizeB << 15)/ScaleXsize;
	int bx_ = 0;
	int by_ = 0;
	int bbx = 0;
	int bx = 0;
	int by = 0;
	int xt = X + XsizeS;
	int yt = Y + YsizeS;
	if(X < UcutLeft){
		bbx = (UcutLeft - X << 15)*XsizeB/ScaleXsize;
		X = UcutLeft;
		}
	if(Y < VcutUp){
		by = (VcutUp - Y << 15)*XsizeB/ScaleXsize;
		Y = VcutUp;
		}
	if(xt > UcutRight)
		xt = UcutRight;
	if(yt > VcutDown)
		yt = VcutDown;
	for(y = Y;y < yt;y++){
		int by_ = by >> 15;
		by += bKy;
		unsigned char* bGr = (unsigned char*)vb + y*XGR_MAXX + (X);
		unsigned char* cy = data + by_ * XsizeB;
		bx = bbx;
		for(x = X;x < xt;x++){
			int bx_ = bx >> 15;
			bx += bKx;
			unsigned char c = cy[bx_];
			if(c){
				//c = FIRE_COLOR_FIRST + ParticlePaletteTable[*bGr + (c << 8)];
				c = FireColorTable[*bGr + (c << 11)];
				*bGr = c;
				}
			bGr++;
			}
		}
}

void bitmap2screenFire(char* vb,unsigned char* data,int Xcenter,int Ycenter,int XsizeB,int YsizeB,int ScaleXsize)
{
	int XsizeS = ScaleXsize;
	int YsizeS = ScaleXsize*YsizeB/XsizeB;
	int bKx,bKy;
	int X,Y;
	int x,y;
	X = Xcenter - XsizeS/2;
	Y = Ycenter - YsizeS/2;
	bKx = bKy = (XsizeB << 15)/ScaleXsize;
	int bx_ = 0;
	int by_ = 0;
	int bbx = 0;
	int bx = 0;
	int by = 0;
	int xt = X + XsizeS;
	int yt = Y + YsizeS;
	if(X < UcutLeft){
		bbx = (UcutLeft - X << 15)*XsizeB/ScaleXsize;
		X = UcutLeft;
		}
	if(Y < VcutUp){
		by = (VcutUp - Y << 15)*XsizeB/ScaleXsize;
		Y = VcutUp;
		}
	if(xt > UcutRight)
		xt = UcutRight;
	if(yt > VcutDown)
		yt = VcutDown;
	for(y = Y;y < yt;y++){
		int by_ = by >> 15;
		by += bKy;
		unsigned char* bGr = (unsigned char*)vb + y*XGR_MAXX + (X);
		unsigned char* cy = data + by_ * XsizeB;
		bx = bbx;
		for(x = X;x < xt;x++){
			int bx_ = bx >> 15;
			bx += bKx;
			unsigned char c = cy[bx_];
			if(c){
				c = FireColorTable[*bGr + (c << 8)];
				*bGr = c;
				}
			bGr++;
			}
		}
}*/

void smart_putspr_f(unsigned char* data,int Xcenter,int Ycenter,int XsizeB,int YsizeB,int ScaleXsize,int height)
{
	if(!XsizeB || !ScaleXsize)
		return;
	int XsizeS = ScaleXsize;
	int YsizeS = ScaleXsize*YsizeB/XsizeB;
	if(DepthShow)
		YsizeS = round(YsizeS*Cos(SlopeAngle));
	int bKx,bKy;
	int X = Xcenter - XsizeS/2;
	int Y = Ycenter - YsizeS/2;
	bKx = bKy = (XsizeB << 15)/ScaleXsize;
	int bx = 0;
	int by = 0;
	int xt = X + XsizeS;
	int yt = Y + YsizeS;
	if(X < UcutLeft){
		bx = ((UcutLeft - X) << 15)*XsizeB/ScaleXsize;
		XsizeS -= UcutLeft - X;
		X = UcutLeft;
		}
	if(Y < VcutUp){
		by = ((VcutUp - Y) << 15)*XsizeB/ScaleXsize;
		YsizeS -= VcutUp - Y;
		Y = VcutUp;
		}
	if(xt > UcutRight){
		XsizeS -= xt - UcutRight;
		xt = UcutRight;
		}
	if(yt > VcutDown){
		YsizeS -= yt - VcutDown;
		yt = VcutDown;
		}
	if(XsizeS <= 0)
		return;

	int vadd = XGR_MAXX - XsizeS;
	draw_vbuf = XGR_VIDEOBUF + Y*XGR_MAXX + X;
	draw_lt = vMap -> lineT;

	z_level = height;

	if(height >= 256){
		while(YsizeS-- > 0){
			transparency_line_non_test_f(XsizeS,data + (by >> 15)*XsizeB,bx,bKx);
			by += bKy;
			draw_vbuf += vadd;
			}
		return;
		}


	int x = X - ScreenCX;
	int y = Y - ScreenCY;

	if(!DepthShow){
		draw_k_xscr_x = cosTurnInv;
		draw_k_xscr_y = -sinTurnInv;
		int k_yscr_x = sinTurnInv;
		int k_yscr_y = cosTurnInv;
		int tfx = (ViewX << 16) + (x*cosTurnInv + y*sinTurnInv) + (1 << 15);
		int tfy = (ViewY << 16) + (-x*sinTurnInv + y*cosTurnInv) + (1 << 15);
		while(YsizeS-- > 0){
			transparency_line_f(XsizeS,data + (by >> 15)*XsizeB,bx,bKx,tfx,tfy);
			by += bKy;
			draw_vbuf += vadd;
			tfx += k_yscr_x;
			tfy += k_yscr_y;
			}
		}
	else{
		int fx,fy;
		int cx = ViewX << 16;
		int cy = ViewY << 16;

		double al = Ha*x + Va*y;
		double bl = Hb*x + Vb*y;
		double cl = Oc + Hc*x + Vc*y;

		double ar = Ha*(x + XsizeS) + Va*y;
		double br = Hb*(x + XsizeS) + Vb*y;

		double nxs_inv = 1/(double)XsizeS;
		double cl_inv;

		while(YsizeS-- > 0){
			if(fabs(cl) < DBL_EPS) 
				return;
			cl_inv = 1/cl;
			fx = round(al*cl_inv);
			fy = round(bl*cl_inv);
			cl_inv *= nxs_inv;
			draw_k_xscr_x = round((ar - al)*cl_inv);
			draw_k_xscr_y = round((br - bl)*cl_inv);
			transparency_line_f(XsizeS,data + (by >> 15)*XsizeB,bx,bKx,fx + cx,fy + cy);

			by += bKy;
			draw_vbuf += vadd;

			al += Va;
			bl += Vb;
			cl += Vc;
			ar += Va;
			br += Vb;
			}
		}
}

void transparency_line_f(int len,unsigned char* dbuf,int bx,int bKx,int fx,int fy)
{
	unsigned char c;
	unsigned char* mbuf;
	while(len-- > 0){
		if((c = dbuf[bx >> 15]) != 0){
			mbuf = draw_lt[(fy >> 16) & clip_mask_y] + (((fx >> 16) | 1) & clip_mask_x);

			if ( c > 31 ) c = 31;

			if(z_level > *mbuf)
				*draw_vbuf = FireColorTable[*draw_vbuf + (c << 11)];
			}
		draw_vbuf++;
		bx += bKx;
		fx += draw_k_xscr_x;
		fy += draw_k_xscr_y;
		}
}

void transparency_line_non_test_f(int len,unsigned char* dbuf,int bx,int bKx)
{
	unsigned char c;
	while(len-- > 0){
		if((c = dbuf[bx >> 15]) != 0){

			if ( c > 31 ) c = 31;

			*draw_vbuf = FireColorTable[*draw_vbuf + (c << 11)];
		}
		draw_vbuf++;
		bx += bKx;
		}
}
