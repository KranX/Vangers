#include "../src/global.h"

#include "3d_shape.h"

#include "../src/common.h"

#include "../src/terra/vmap.h"
#include "../src/terra/world.h"
#include "../src/terra/render.h"

#include "../src/3d/3d_math.h"
#include "../src/3d/3dgraph.h"

#include "missed.h"

//#define _SHAPE_

#ifdef _SURMAP_
extern int DepthShow;
#else
int DepthShow;
#endif

void S3Danalyze(void);

#define COLOR_OFFSET	128
#define COLOR_SHIFT	3

int shape_control(int code);
void load_shape(char* name,int x,int y);
void view_shape();

int shape_size;
int shape_shift;
unsigned char* upper_buffer;
unsigned char* lower_buffer;
int shape_x,shape_y;
int shape_x_avr;
int shape_y_avr;
int shape_z_avr;


int shape_offset;
double shape_scale_x = 1;
double shape_scale_y = 1;
double shape_scale_z = 1;

int catch_log;
DBM A_shape(1,-1,-1,DIAGONAL);
DBM A_shape_inv(1,-1,1,DIAGONAL);
Model shape;

int shape_control(int code)
{
	if(!catch_log)
		return 0;

	switch(code){
		case SDLK_ESCAPE:
			catch_log = 0;
			break;

		case SDLK_SPACE:
			A_shape = DBM(1,-1,-1,DIAGONAL);
			A_shape_inv = DBM(1,-1,1,DIAGONAL);
			if(XKey.Pressed(SDLK_LCTRL))
				shape_scale_x = shape_scale_y = shape_scale_z = 1;
			break;

		case SDLK_RETURN:
			shape.height_project();
			catch_log = 0;
#ifdef _SURMAP_
			S3Danalyze();
#endif
			break;
		}

	int rotate_step;
	int move_step;
	int alt = XKey.Pressed(SDLK_LCTRL);
	double scale_step;
	if(XKey.Pressed(SDLK_LSHIFT)){
		move_step = 20;
		rotate_step = Pi/8;
		scale_step = 1.1;
		}
	else{
		move_step = 1;
		rotate_step = Pi/256;
		scale_step = 1.001;
		}

	if(XKey.Pressed(SDLK_DELETE)){
		A_shape = DBM(-rotate_step,Z_AXIS)*A_shape;
		A_shape_inv *= DBM(rotate_step,Z_AXIS);
		}
	//@caiiiycuk: was VK_NEXT
	if(XKey.Pressed(SDLK_RETURN2)){
		A_shape = DBM(rotate_step,Z_AXIS)*A_shape;
		A_shape_inv *= DBM(-rotate_step,Z_AXIS);
		}

	if(XKey.Pressed(SDLK_HOME)){
		A_shape = DBM(-rotate_step,X_AXIS)*A_shape;
		A_shape_inv *= DBM(rotate_step,X_AXIS);
		}
	if(XKey.Pressed(SDLK_END)){
		A_shape = DBM(rotate_step,X_AXIS)*A_shape;
		A_shape_inv *= DBM(-rotate_step,X_AXIS);
		}

	if(XKey.Pressed(SDLK_INSERT)){
		A_shape = DBM(rotate_step,Y_AXIS)*A_shape;
		A_shape_inv *= DBM(-rotate_step,Y_AXIS);
		}
	if(XKey.Pressed(SDLK_PRIOR)){
		A_shape = DBM(-rotate_step,Y_AXIS)*A_shape;
		A_shape_inv *= DBM(rotate_step,Y_AXIS);
		}

	if(XKey.Pressed(SDLK_EQUALS) || XKey.Pressed(SDLK_KP_PLUS)){
		if(!alt){
			shape_scale_x *= scale_step;
			shape_scale_y *= scale_step;
			}
		shape_scale_z *= scale_step;
		}
	if(XKey.Pressed(SDLK_MINUS) || XKey.Pressed(SDLK_KP_MINUS)){
		if(!alt){
			shape_scale_x /= scale_step;
			shape_scale_y /= scale_step;
			}
		shape_scale_z /= scale_step;
		}

	if(XKey.Pressed(SDLK_UP))
		if(alt)
			shape_scale_y *= scale_step;
		else
			shape.y_off -= move_step;
	if(XKey.Pressed(SDLK_DOWN))
		if(alt)
			shape_scale_y /= scale_step;
		else
			shape.y_off += move_step;
	if(XKey.Pressed(SDLK_LEFT))
		if(alt)
			shape_scale_x /= scale_step;
		else
			shape.x_off -= move_step;
	if(XKey.Pressed(SDLK_RIGHT))
		if(alt)
			shape_scale_x *= scale_step;
		else
			shape.x_off += move_step;

#ifdef _SURMAP_
	shape.x_off &= clip_mask_x;
	shape.y_off &= clip_mask_y;
#endif

	return 1;
}

void load_shape(char* name,int x,int y)
{
	XStream c3d(name,XS_IN);
	XBuffer buf(c3d.size());
	c3d.read(buf.address(),c3d.size());
	c3d.close();
	shape.loadC3D(buf);

	shape.x_off = x;
	shape.y_off = y;
	shape.z_off = 128;

	catch_log = 1;

#ifdef _SHAPE_
	upper_buffer = new unsigned char[1024*1024];
	lower_buffer = new unsigned char[1024*1024];
#endif
}
#ifndef _SURMAP_
void prepare_shape(int mode,char* name,int new_size)
{
	char* c3d_name = ALLOC(strlen(name) + 1,char);
	strlwr(name);
	strcpy(c3d_name,name);
	strcpy(strstr(c3d_name,"asc"),"c3d");

	if(mode & SHAPE_QUIT_IF_EXIST){
		XStream c3d(0);
		if(c3d.open(c3d_name,XS_IN)){
			int version;
			c3d > version;
			if(version == C3D_VERSION_3){
				XCon < "\nFile \"" < name < "\" has already been converted.\n";
				return;
				}
			}
		}
		
	shape.loadASC(name);

	double scale_mul = 1;
	if(mode & SHAPE_SCALE)
		scale_mul = new_size/(double)(mode & SHAPE_RELATIVE ? 100 : shape.rmax);

	Vector offset = Vector(shape.x_off,shape.y_off,shape.z_off);
	if(mode & SHAPE_LOAD_PARAMS){
		XStream f(0);
		if(f.open("shape.prm",XS_IN)){
			Parser prm_in("shape.prm");
			prm_in.search_name("scale_mul:");
			scale_mul = prm_in.get_int();
			
			prm_in.search_name("x_offset:");
			offset.x = prm_in.get_int();
			prm_in.search_name("y_offset:");
			offset.y = prm_in.get_int();
			offset.z = 0;
			}
		}

	XStream prm_out("shape.prm",XS_OUT);
	prm_out.SetDigits(8);

	XCon < "\nConverting ASC to C3D: " < name < "\n";
	prm_out < "\nConverting ASC to C3D: " < name < "\n";
	
	XCon < "Faces: " <= shape.num_poly < "\n";
	prm_out < "Faces: " <= shape.num_poly < "\n";
	XCon < "Vertices: " <= shape.num_vert < "\n";
	prm_out < "Vertices: " <= shape.num_vert < "\n";

	XCon < "Original bound radius: " <= shape.rmax < "\n";
	prm_out < "Original bound radius: " <= shape.rmax < "\n";


	shape.set_colors(COLORS_IDS::BODY,0,0);

	if(mode & SHAPE_MOVE)
		shape.move(-offset);
	shape.make_normals();
	shape.pre_sorting();
	shape.scale(scale_mul,1);

	XCon < "Obtained bound radius: " <= shape.rmax < "\n";
	prm_out < "Obtained bound radius: " <= shape.rmax < "\n";

	prm_out < "scale_mul: " <= scale_mul < "\n";
	prm_out < "x_offset: " <= offset.x < "\n";
	prm_out < "y_offset: " <= offset.y < "\n";

	XStream fout(c3d_name,XS_OUT);
	shape.saveC3D(fout);

	delete c3d_name;
}
#endif
void release_shape()
{
	shape.free();
	if(upper_buffer){
		delete upper_buffer;
		delete lower_buffer;
		}
}
void view_shape()
{
	if(!catch_log)
		return;
#ifdef _SHAPE_
	shape.height_project();
	shape.draw_big();
#else
	shape.draw_big();
#endif
}
void Model::draw_big()
{
	int x = getDistX(x_off,ViewX);
	int y = getDistY(y_off,ViewY);
	if(DepthShow){
		DBV R(x,y,0);
		R *= A_g2s;
		CentrX = round(R.x);
		CentrY = round(R.y);
		CentrZ = ViewZ + round(R.z);
		}
	else{
		float co = Cos(TurnAngle);
		float si = Sin(TurnAngle);
		CentrX = round(x*co - y*si);
		CentrY = round(x*si + y*co);
		CentrZ = ViewZ;
		}

	A_convert = A_g2s*A_shape*DBM(shape_scale_x,0,0,
					 0,shape_scale_y,0,
					 0,0,shape_scale_z);
	Refl = A_shape_inv*Light;

	int direct;
	Polygon** sort_poly;
	double vx = -A_convert.a[6];
	double vy = -A_convert.a[7];
	double vz = -A_convert.a[8];
	if(fabs(vz) >= fabs(vy))
		if(fabs(vz) >= fabs(vx)){
			sort_poly = sorted_polygons[Z_AXIS];
			direct = SIGN(vz);
			}
		else{
			sort_poly = sorted_polygons[X_AXIS];
			direct = -SIGN(vx);
			}
	else
		if(fabs(vx) > fabs(vy)){
			sort_poly = sorted_polygons[X_AXIS];
			direct = -SIGN(vx);
			}
		else{
			sort_poly = sorted_polygons[Y_AXIS];
			direct = -SIGN(vy);
			}

	for(register int i = 0;i < num_vert;i++)
		vertices[i].convert_big();

	int i;
	for(i = 0;i < num_norm;i++)
		normals[i].fast_calc_intensity();

	if(direct > 0){
		for(i = 0;i < num_poly;i++)
			sort_poly[i] -> draw_gouraud_big();
		}
	else{
		for(i = num_poly - 1;i >= 0;i--)
			sort_poly[i] -> draw_gouraud_big();
		}
}
void Vertex::convert_big()
{
	turned = A_convert*DBV(x,y,z);
//	  int zz = CentrZ + turned.z;
//	  if(zz <= 0)
//		  zz = 1;
//	  xscr = (turned.x + CentrX)*focus/(zz) + ScreenCX;
//	  yscr = (turned.y + CentrY)*focus/(zz) + ScreenCY;
	xscr = turned.x + CentrX + ScreenCX;
	yscr = turned.y + CentrY + ScreenCY;
}
void Polygon::draw_gouraud_big()
{
#ifdef COMPACT_3D
	const num_vert = 3;
#endif	
	#define NEXT_LEFT(p)	((p) == beg ? end : (p) - 1)
	#define NEXT_RIGHT(p)	((p) == end ? beg : (p) + 1)

	int xl,xr,al,ar,bl,br,d,where,Y;
	int cl,cr,ckl,ckr;
	int cf,cfk;
	int x1,x2,len;
	Vertex *lfv,*rfv,*ltv,*rtv;
	Vertex **lto,**rto;
	Vertex **beg,**curr,**up,**end;
	Normal *lfn,*rfn,*ltn,*rtn;
	unsigned char* vp;

	beg = up = curr = vertices;
	end = vertices + num_vert - 1;
	curr++;
	int t;
	int x_left = (*beg) -> xscr;
	int x_right = x_left;
	int y_up = (*beg) -> yscr;
	int y_down = y_up;
	for(;curr <= end;curr++){
		if(y_up > (t = (*curr) -> yscr)){
			up = curr;
			y_up = t;
			}
		if(y_down < t)
			y_down = t;
		if(x_left > (t = (*curr) -> xscr))
			x_left = t;
		if(x_right < t)
			x_right = t;
		}

	if(y_down < VcutUp || y_up > VcutDown ||
	   x_right < UcutLeft || x_left > UcutRight)
		return;

	rfv = lfv = *up;
	rfn = lfn = normals[up - beg];

	lto = NEXT_LEFT(up);
	ltv = *lto;
	ltn = normals[lto - beg];

	rto = NEXT_RIGHT(up);
	rtv = *rto;
	rtn = normals[rto - beg];

	Y = lfv -> yscr; xl = lfv -> xscr;
	al = ltv -> xscr - xl; bl = ltv -> yscr - Y;
	ar = rtv -> xscr - xl; br = rtv -> yscr - Y;
	xr = xl = (xl << 16) + (1 << 15);

	cl = lfn -> I_8;
	ckl = ltn -> I_8 - cl;
	ckr = rtn -> I_8 - cl;
	cr = cl <<= 7;

	if(bl){
		DIV16(al,bl);
		ckl = (ckl << 7)/bl;
		}
	else
		al <<= 16;

	if(br){
		DIV16(ar,br);
		ckr = (ckr << 7)/br;
		}
	else
		ar <<= 16;

	vp = XGR_VIDEOBUF + Y*XGR_MAXX;
	while(1){
		if(bl > br){
			d = br;
			where = 0;
			}
		else{
			d = bl;
			where = 1;
			}
		while(d-- > 0){
			x1 = xl >> 16;
			x2 = xr >> 16;
			if(x1 > x2)
				return;
			if(x1 < x2){
				if(Y >= VcutUp && Y < VcutDown &&
				   x2 >= UcutLeft && x1 < UcutRight){
					len = x2 - x1;
					cf = cl;
					cfk = (cr - cl)/len;
					if(x1 < UcutLeft){
						cf += cfk*(UcutLeft - x1);
						x1 = UcutLeft;
						len = x2 - x1;
						}
					if(x2 >= UcutRight){
						x2 = UcutRight;
						len = x2 - x1;
						}
					vp += x1;
					while(len--){
						*(vp++) = COLOR_OFFSET + (cf >> COLOR_SHIFT + 7);
						cf += cfk;
						}
					vp -= x2;
					}
				}
			Y++;
			vp += XGR_MAXX;

			xl += al;
			xr += ar;

			cl += ckl;
			cr += ckr;
			}
		if(where){
			if(ltv == rtv) return;
			lfv = ltv;
			lfn = ltn;
			lto = NEXT_LEFT(lto);
			ltv = *lto;
			ltn = normals[lto - beg];

			br -= bl;

			xl = lfv -> xscr;
			al = ltv -> xscr - xl;
			bl = ltv -> yscr - Y;
			xl = (xl << 16) + (1 << 15);

			cl = lfn -> I_8;
			ckl = ltn -> I_8 - cl;
			cl <<= 7;

			if(bl){
				DIV16(al,bl);
				ckl = (ckl << 7)/bl;
				}
			else{
				al <<= 16;
				}
			}
		else {
			if(rtv == ltv) return;
			rfv = rtv;
			rfn = rtn;
			rto = NEXT_RIGHT(rto);
			rtv = *rto;
			rtn = normals[rto - beg];

			bl -= br;

			xr = rfv -> xscr;
			ar = rtv -> xscr - xr;
			br = rtv -> yscr - Y;
			xr = (xr << 16) + (1 << 15);

			cr = rfn -> I_8;
			ckr = rtn -> I_8 - cr;
			cr <<= 7;

			if(br){
				DIV16(ar,br);
				ckr = (ckr << 7)/br;
				}
			else
				ar <<= 16;
			}
		}
}
/*******************************************************************************
			Height projection
*******************************************************************************/
void Model::height_project()
{
	int x,x_min = INT_INF,x_max = -INT_INF;
	int y,y_min = INT_INF,y_max = -INT_INF;
	int z,z_min = INT_INF,z_max = -INT_INF;
	A_convert = A_shape*DBM(shape_scale_x,0,0,
				   0,shape_scale_y,0,
				   0,0,shape_scale_z);

	for(int i = 0;i < num_vert;i++){
		vertices[i].turned = A_convert*DBV(vertices[i].x,vertices[i].y,vertices[i].z);
		if((x = vertices[i].turned.x) < x_min)
			x_min = x;
		if((x = vertices[i].turned.x) > x_max)
			x_max = x;
		if((y = vertices[i].turned.y) < y_min)
			y_min = y;
		if((y = vertices[i].turned.y) > y_max)
			y_max = y;
		if((z = -vertices[i].turned.z) < z_min)
			z_min = z;
		if((z = vertices[i].turned.z) > z_max)
			z_max = z;
		}

	shape_offset = max(x_max - x_min,y_max - y_min)/2 + 2;
	shape_size = max(x_max - x_min,y_max - y_min) + 2;
	shape_shift = BitSR(shape_size) + 1;

	shape_x = x_off + x_min - 1;
	shape_y = y_off + y_min;

	shape_x_avr = (x_max - x_min)/2;
	shape_y_avr = (y_max - y_min)/2;
	shape_z_avr = (z_max - z_min)/2 + 1;

#ifndef _SHAPE_
	upper_buffer = new unsigned char[shape_size << shape_shift];
	lower_buffer = new unsigned char[shape_size << shape_shift];
#endif
	memset(upper_buffer,0,shape_size << shape_shift);
	memset(lower_buffer,255,shape_size << shape_shift);

	int i;
	for(i = 0;i < num_vert;i++){
		vertices[i].xscr = vertices[i].turned.x - x_min;
		vertices[i].yscr = vertices[i].turned.y - y_min;
		vertices[i].turned.z = -vertices[i].turned.z - z_min + 1;
		}

	for(i = 0;i < num_poly;i++)
		polygons[i].height_project();

#ifdef _SHAPE_
	c_putspr(shape_x,shape_y,1 << shape_shift,shape_size,upper_buffer,BLACK_FON);
//	  c_putspr(shape_x,shape_y - shape_size/2 - 2,1 << shape_shift,shape_size,upper_buffer,BLACK_FON);
//	  c_putspr(shape_x,shape_y + shape_size/2 + 2,1 << shape_shift,shape_size,lower_buffer,BLACK_FON);
#endif
}
void Polygon::height_project()
{
#ifdef COMPACT_3D
	const num_vert = 3;
#endif	
#define NEXT_LEFT(p)	((p) == beg ? end : (p) - 1)
#define NEXT_RIGHT(p)	((p) == end ? beg : (p) + 1)

	int xl,xr,al,ar,bl,br,d,where,Y;
	int cl,cr,ckl,ckr;
	int cf,cfk,z;
	int x1,x2,len,swap_log;
	Vertex *lfv,*rfv,*ltv,*rtv;
	Vertex **lto,**rto;
	Vertex **beg,**curr,**up,**end;
	unsigned char *up_vp;
	unsigned char *lo_vp;

	beg = up = curr = vertices;
	end = vertices + num_vert - 1;
	curr++;
	for(;curr <= end;curr++)
		if((*up) -> yscr > (*curr) -> yscr)
			up = curr;

	rfv = lfv = *up;

	lto = NEXT_LEFT(up);
	ltv = *lto;

	rto = NEXT_RIGHT(up);
	rtv = *rto;

	Y = lfv -> yscr; xl = lfv -> xscr;
	al = ltv -> xscr - xl; bl = ltv -> yscr - Y;
	ar = rtv -> xscr - xl; br = rtv -> yscr - Y;
	xr = xl = (xl << 16) + (1 << 15);

	cl = lfv -> turned.z;
	ckl = ltv -> turned.z - cl;
	ckr = rtv -> turned.z - cl;
	cr = cl <<= 16;

	if(bl){
		DIV16(al,bl);
		DIV16(ckl,bl);
		}

	if(br){
		DIV16(ar,br);
		DIV16(ckr,br);
		}

	up_vp = upper_buffer + (Y << shape_shift);
	lo_vp = lower_buffer + (Y << shape_shift);

	while(1){
		if(bl > br){
			d = br;
			where = 0;
			}
		else{
			d = bl;
			where = 1;
			}
		while(d-- > 0){
			x1 = xl >> 16;
			x2 = xr >> 16;
			if(x1 != x2){
				if(x1 > x2){
					SWAP(x1,x2);
					SWAP(cl,cr);
					swap_log = 1;
					}
				else
					swap_log = 0;
				len = x2 - x1;
				cf = cl;
				cfk = (cr - cl)/len;
				up_vp += x1;
				lo_vp += x1;
				while(len--){
					z = cf >> 16;
					if(z > 0 && z < 256){
						if(*up_vp < z)
							*up_vp = z;
						if(*lo_vp > z)
							*lo_vp = z;
						}
#ifdef _FOUT_
					else
						fout < "High project: Over/under-flow: " <= z < "\n";
#endif
					cf += cfk;
					up_vp++;
					lo_vp++;
					}
				up_vp -= x2;
				lo_vp -= x2;
				if(swap_log)
					SWAP(cl,cr);
				}
			Y++;
			up_vp += 1 << shape_shift;
			lo_vp += 1 << shape_shift;

			xl += al;
			xr += ar;

			cl += ckl;
			cr += ckr;
			}
		if(where){
			if(ltv == rtv) return;
			lfv = ltv;
			lto = NEXT_LEFT(lto);
			ltv = *lto;

			br -= bl;

			xl = lfv -> xscr;
			al = ltv -> xscr - xl;
			bl = ltv -> yscr - Y;
			xl = (xl << 16) + (1 << 15);

			cl = lfv -> turned.z;
			ckl = ltv -> turned.z - cl;
			cl <<= 16;

			if(bl){
				DIV16(al,bl);
				DIV16(ckl,bl);
				}
			}
		else {
			if(rtv == ltv) return;
			rfv = rtv;
			rto = NEXT_RIGHT(rto);
			rtv = *rto;

			bl -= br;

			xr = rfv -> xscr;
			ar = rtv -> xscr - xr;
			br = rtv -> yscr - Y;
			xr = (xr << 16) + (1 << 15);

			cr = rfv -> turned.z;
			ckr = rtv -> turned.z - cr;
			cr <<= 16;

			if(br){
				DIV16(ar,br);
				DIV16(ckr,br);
				}
			}
		}
}

