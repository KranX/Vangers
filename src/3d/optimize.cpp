#include "../global.h"

#include "general.h"

#include "../common.h"
#include "../sound/hsound.h"

#include "../terra/world.h"
#include "../terra/vmap.h"
#include "../terra/render.h"

#define MIN_SHADOW_dZ	4
#define OBJSHADOW_H	50
#define GET_MIDDLE_HIGHT(lh,uh)		(uh - lh > 130 ? lh + 110 : lh + 48)
#define BREAKABLE_TERRAIN(prop)		(GET_DESTROY_TERRAIN(GET_TERRAIN(prop)) > 10)

extern unsigned char* invisible_table;
#define TRANSPARENCY(color,intensity) (invisible_table[color + ((intensity) << 8)])
				         
#undef SWAP
#define SWAP(a,b)	{ a ^= b; b ^= a; a ^= b; }
// returns (x*y >> 7), where x - [-127,127], y - [-127,127]
#define MUL_CHAR(x,y)	     (mul_table[((x) & 0xFF) | (((y) << 8) & 0xFF00)])
//#define LOW_LEVEL
//#define LOW_LEVEL_LINE

extern "C"{
	int low_cycle_z(int d);
	int low_cycle_z_test(int d);
}

void (*low_shadow_line)(int,int,int);
void (*low_image_line)(int,int,int);
           
void putspr(int x,int y,int size,int shift,unsigned char* buffer,int image);
void DrawLinear(int x,int y,int size,int shift,unsigned char* buffer,int image);
void Draw3DPlane(int x,int y,int size,int shift,unsigned char* buffer,int image,int y_offset);
void CastShadow(int x,int y,int zg,int size,int shift,unsigned char* buffer);
void CastArbitraryShadow(int x,int y,int zg,int size,int shift,unsigned char* buffer);
int trace_shadow(int x,int y,int z);

extern unsigned char* ShadowColorTable;
extern int dz_between_water_and_mask_levels;
extern int dz_shadow;
extern int SHADOWDEEP_3D;

DBM A_convert;
char convert_offset;
char z_global_offset;
Matrix A_convert_8;

int draw_offset;
int draw_size;
int draw_x_size;
unsigned char draw_shift;
int z_low_level;
int z_low_level_water;
int ground_pressing_z_offset;
unsigned char* draw_color_table;
unsigned char* draw_buffer;
unsigned char draw_state;

int ScreenSHADOWDEEP;
int draw_x_bmp_corner_16;
int draw_y_bmp_corner_16;
int draw_dx;
int draw_dy;

unsigned char COLORS_VALUE_TABLE[2*COLORS_IDS::MAX_COLORS_IDS] =  
{ 
	0,	0,				// 0 - ZERO_RESERVED,       				
			   			                                        
	128, 	3,			      //1 - BODY,  changeable
			   			                                        
	176, 	4,			      // 2 - WINDOW,                     
	224, 	7,			      // 3 - WHEEL,                         
	184, 	4,			      // 4 - DEFENSE,                     
	224, 	3,			      // 5 - WEAPON,                     
	224,	7,			      // 6 - TUBE,                            
			   			                                        
	128, 	3,			      // 7 - BODY_RED,                  
	144, 	3,			      // 8 - BODY_BLUE,               
	160, 	3,			      // 9 - BODY_YELLOW,         
	228, 	4,			      // 10 - BODY_GRAY,              
	112, 	4,			      // 11 - YELLOW_CHARGED, 
	
	0,	2,				// 12 - MATERIAL_0,               
	32,	2,			       // 13 - MATERIAL_1,               
	64,	4,			       // 14 - MATERIAL_2,               
	72,	3,			       // 15 - MATERIAL_3,               
	88,	3,			       // 16 - MATERIAL_4,               
	104,	4,			      // 17 - MATERIAL_5;               
	112,	4,			      // 18 - MATERIAL_6,               
	120,	4,			      // 19 - MATERIAL_7,               
	184,	4,        		      // 20 - BLACK
	240,	3,		              // 21 - BODY_GREEN

	136,	4,				// 22 - SKYFARMER_KERNOBOO
	128,    4,				// 23 - SKYFARMER_PIPETKA
	224, 	4,			        // 24 - ROTTEN_ITEM

	192, 	3			        // 25 - BODY_CRIMSON
};

// Visualization adjust params loaded in dynamics.cpp
int dz_between_water_and_mask_levels;
int light_modulation = 256;
int dz_shadow;
int SHADOWDEEP_3D;


/*******************************************************************************
	 	COLORS SUPPORT FUNCTION		
*******************************************************************************/
void register_color(unsigned int id,int first,int last)
{
	if(id > COLORS_IDS::MAX_COLORS_IDS)
		ErrH.Abort("Bad color id",XERR_USER,id);
	COLORS_VALUE_TABLE[id*2] = first;
	COLORS_VALUE_TABLE[id*2 + 1] = 7 - BitSR(last - first);
}	
void Object::set_body_color(unsigned int color_id)
{
	if(color_id > COLORS_IDS::MAX_COLORS_IDS)
		ErrH.Abort("Bad color id",XERR_USER,color_id);
	body_color_offset = COLORS_VALUE_TABLE[2*color_id];
	body_color_shift = COLORS_VALUE_TABLE[2*color_id + 1];
}	
/*******************************************************************************
		SORTED DRAW FUNCTION
*******************************************************************************/
void Object::draw()
{
	int i;
	if(n_models > 1)
		model = &models[((i_model += traction) >> 8) % n_models];
	COLORS_VALUE_TABLE[2*COLORS_IDS::BODY] = body_color_offset;
	COLORS_VALUE_TABLE[2*COLORS_IDS::BODY + 1] = body_color_shift;

	draw_offset = rmax_screen + 2;
	convert_offset = draw_offset;
	draw_size = draw_offset*2;
	draw_shift = BitSR(draw_size) + 1;
	draw_x_size = 1 << (draw_shift + 1);
	if(draw_shift > 8)
		ErrH.Abort("Incorrect draw shift");

	draw_state = 0;
	memset(draw_buffer,0,2*draw_size << draw_shift);

	z_global_offset = radius + 2;
	if(z_global_offset < 30)
		z_global_offset = 30;
	z_low_level_water = R_curr.z - z_global_offset;

	z_low_level = z_low_level_water + dz_between_water_and_mask_levels;
	z_low_level_water -= FloodLEVEL;

	CentrX = 0;
	CentrY = 0;
	Refl = A_g2l*Light;
	View = Vector(round(A_g2l[2]*ModulatedNORMAL),round(A_g2l[5]*ModulatedNORMAL),round(A_g2l[8]*ModulatedNORMAL));

	A_convert[0] = A_scl[0]; A_convert[1] = A_scl[1]; A_convert[2] = A_scl[2];
	A_convert[3] = A_scl[3]; A_convert[4] = A_scl[4]; A_convert[5] = A_scl[5];
	A_convert[6] = A_l2g[6]*scale_real; A_convert[7] = A_l2g[7]*scale_real; A_convert[8] = A_l2g[8]*scale_real;
	A_convert_8 = Matrix(A_convert*127);

	model -> z_buffering_draw();

	if(ID & ID_VANGER){
		for(i = 0;i < MAX_SLOTS;i++)
			if(data_in_slots[i]){
				double scl = data_in_slots[i] -> scale_size/original_scale_size;
				if(location_angle_of_slots[i]){
					DBM A_c2p = DBM(location_angle_of_slots[i],Y_AXIS);
					DBM A_p2c = transpose(A_c2p);
					A_c2p *= scl;
					Vector off = A_c2p*Vector(data_in_slots[i] -> model -> x_off,data_in_slots[i] -> model -> y_off,data_in_slots[i] -> model -> z_off);
					data_in_slots[i] -> model -> draw_child(R_slots[i] - off,A_c2p,A_p2c);
					}
				else{
					Vector off = Vector(data_in_slots[i] -> model -> x_off,data_in_slots[i] -> model -> y_off,data_in_slots[i] -> model -> z_off)*scl;
					data_in_slots[i] -> model -> draw_child(R_slots[i] - off,DBM()*scl,DBM());
					}
				}
		 
		DBM A_c2p = DBM(rudder,Z_AXIS);
		#ifdef _MT_
		if(model == models)
		#endif
		for(i = 0;i < n_wheels;i++)
			if(wheels[i].steer)
				wheels[i].model.draw_child(Vector(wheels[i].model.x_off,wheels[i].model.y_off,wheels[i].model.z_off),A_c2p,transpose(A_c2p));
		}

#ifdef _ROAD_
	// Put Shadow 
	if(draw_mode == NORMAL_DRAW_MODE){
		if(ID != ID_INSECT)
			if(TurnAngle)
				CastArbitraryShadow(R_scr.x - draw_offset,R_scr.y - draw_offset,R_curr.z - z_global_offset,draw_size,draw_shift,draw_buffer);
			else
				CastShadow(R_scr.x - draw_offset,R_scr.y - draw_offset,R_curr.z - z_global_offset,draw_size,draw_shift,draw_buffer);
		else{
			int dz = R_curr.z - z_below - (zmax_real >> 2);
			if(dz < MIN_SHADOW_dZ)
				dz = MIN_SHADOW_dZ;
			if(dz > zmax)
				dz = trace_shadow(R_curr.x,R_curr.y,R_curr.z);
			double dzd = (double)dz*ScaleMapInvFlt;
			int vx = -round(A_g2s[0]*dzd);
			int vy = round(A_g2s[1]*dzd);
			if(!DepthShow)
				DrawLinear(R_scr.x - draw_offset + vx,R_scr.y - draw_offset + vy,draw_size,draw_shift,draw_buffer,0);
			else
         			Draw3DPlane(R_scr.x - draw_offset + vx,R_scr.y - draw_offset + vy,draw_size,draw_shift,draw_buffer,0,0);
			}
		}

	// Put Image
	if(!DepthShow)
		DrawLinear(R_scr.x - draw_offset,R_scr.y - draw_offset,draw_size,draw_shift,draw_buffer,draw_mode);
	else{
		int y_offset =0;// (2*zmax*scale >> 9)*Sin(-SlopeAngle)/Cos(SlopeAngle);
		Draw3DPlane(R_scr.x - draw_offset,R_scr.y - draw_offset,draw_size,draw_shift,draw_buffer,draw_mode,y_offset);
		}
	
	dynamic_state &= ~(TOUCH_OF_WATER | TOUCH_OF_AIR);
	dynamic_state |= draw_state;
//	if(mole_on)
//		dynamic_state |= TOUCH_OF_AIR;
#endif
#ifdef _MT_
	putspr(R_scr.x - draw_offset,R_scr.y - draw_offset,draw_size,draw_shift,draw_buffer,1);
	//static char slot_color[3] = { 111,63,255};
	//for(i = 0;i < MAX_SLOTS;i++)
	//	if(slots_existence & (1 << i))
	//		XGR_SetPixel(R_scr.x + (A_scl*R_slots[i]).x, R_scr.y + (A_scl*R_slots[i]).y,slot_color[i]);
#endif
}
void Model::draw_child(const Vector& R,const DBM& A_c2p,const DBM& A_p2c)
{
	int CentrX_old = CentrX;
	int CentrY_old = CentrY;
	int z_global_offset_old = z_global_offset;
	Matrix A_convert_8_old = A_convert_8;
	Vector Refl_old = Refl;
	Vector View_old = View;

	Vector R_convert = A_convert*R;

	CentrX += R_convert.x;
	CentrY += R_convert.y;
	z_global_offset += R_convert.z;

	A_convert_8 = Matrix(A_convert*A_c2p*127);

	Refl *= A_p2c;
	View *= A_p2c;

	z_buffering_draw();

	CentrX = CentrX_old;
	CentrY = CentrY_old;
	z_global_offset = z_global_offset_old;
	A_convert_8 = A_convert_8_old;
	Refl = Refl_old;
	View = View_old;
}
void Model::z_buffering_draw()
{
	int i;
	for(i = 0;i < num_vert;i++)
		vertices[i].fast_convert_z();

	for(i = 0;i < num_norm;i++)
		normals[i].fast_calc_intensity();

	for(i = 0;i < num_poly;i++)
		polygons[i].draw_gouraud_decomposed_true_z();
}

/*******************************************************************************
		Pixel-visualisation
*******************************************************************************/
void Object::pixel_draw()
{
	COLORS_VALUE_TABLE[2*COLORS_IDS::BODY] = body_color_offset;
	COLORS_VALUE_TABLE[2*COLORS_IDS::BODY + 1] = body_color_shift;

	draw_offset = rmax_screen + 2;
	convert_offset = draw_offset;
	draw_size = draw_offset*2;
	draw_shift = BitSR(draw_size) + 1;
	draw_x_size = 1 << (draw_shift + 1);
	if(draw_shift > 8)
		ErrH.Abort("Incorrect draw shift");

	draw_state = 0;
	memset(draw_buffer,0,2*draw_size << draw_shift);

	z_global_offset = radius + 2;
	if(z_global_offset < 30)
		z_global_offset = 30;
	z_low_level_water = R_curr.z - z_global_offset;

	z_low_level = z_low_level_water + dz_between_water_and_mask_levels;
	z_low_level_water -= FloodLEVEL;

	CentrX = 0;
	CentrY = 0;
	Refl = A_g2l*Light;
	View = Vector(round(A_g2l[2]*ModulatedNORMAL),round(A_g2l[5]*ModulatedNORMAL),round(A_g2l[8]*ModulatedNORMAL));
	A_convert[0] = A_scl[0]; A_convert[1] = A_scl[1]; A_convert[2] = A_scl[2];
	A_convert[3] = A_scl[3]; A_convert[4] = A_scl[4]; A_convert[5] = A_scl[5];
	A_convert[6] = A_l2g[6]*scale_real; A_convert[7] = A_l2g[7]*scale_real; A_convert[8] = A_l2g[8]*scale_real;
	A_convert_8 = Matrix(A_convert*127);
	model -> pixel_draw();

#ifdef _ROAD_
	if(DepthShow){
		double dz = (double)trace_shadow(R_curr.x,R_curr.y,R_curr.z)*ScaleMapInvFlt;
		int vx = -round(A_g2s[0]*dz);
		int vy = round(A_g2s[1]*dz);
		Draw3DPlane(R_scr.x - draw_offset + vx,R_scr.y - draw_offset + vy,draw_size,draw_shift,draw_buffer,0,0);
		Draw3DPlane(R_scr.x - draw_offset,R_scr.y - draw_offset,draw_size,draw_shift,draw_buffer,1,0);
		}
	else{
		double dz = (double)trace_shadow(R_curr.x,R_curr.y,R_curr.z)*ScaleMapInvFlt;
		int vx = -round(A_g2s[0]*dz);
		int vy = round(A_g2s[1]*dz);
		DrawLinear(R_scr.x - draw_offset + vx,R_scr.y - draw_offset + vy,draw_size,draw_shift,draw_buffer,0);
		DrawLinear(R_scr.x - draw_offset,R_scr.y - draw_offset,draw_size,draw_shift,draw_buffer,1);
		}
	
	dynamic_state &= ~(TOUCH_OF_WATER | TOUCH_OF_AIR);
	dynamic_state |= draw_state;
#endif
}

void Model::pixel_draw()
{
	int i;
	for(i = 0;i < num_vert;i++)
		vertices[i].fast_convert_z();

	for(i = 0;i < num_norm;i++)
		normals[i].fast_calc_intensity();

	for(i = 0;i < num_poly;i++)
		polygons[i].draw_pixel();
}

/*******************************************************************************
		Vertex & Normal functions
*******************************************************************************/
//#ifndef LOW_LEVEL

//#endif
void Polygon::draw_pixel()
{
	unsigned char color_offset = COLORS_VALUE_TABLE[color_id*2];
	unsigned char color_shift = COLORS_VALUE_TABLE[color_id*2 + 1];

	unsigned x = ((unsigned)vertices[0] -> xscr_8 + vertices[1] -> xscr_8 + vertices[2] -> xscr_8)/3;
	unsigned y = ((unsigned)vertices[0] -> yscr_8 + vertices[1] -> yscr_8 + vertices[2] -> yscr_8)/3;
	unsigned z = ((unsigned)vertices[0] -> zscr_8 + vertices[1] -> zscr_8 + vertices[2] -> zscr_8)/3;
	unsigned c = color_offset + (((unsigned)normals[0] -> I_8 + normals[1] -> I_8 + normals[2] -> I_8) >> color_shift)/3;
	unsigned char* draw_vp = draw_buffer + (((((y + CentrY) << draw_shift) + x + CentrX) << 1) & (2*DRAW_SX*DRAW_SY - 1));
	if(z > *draw_vp){				
		*draw_vp = z;
		*(draw_vp  + 1) = c;
		}
}
/*******************************************************************************
			Draw Polygon with true z-buffering
*******************************************************************************/

#define X_PREC		8
#define COLOR(I)	(((unsigned int)(color_offset + I)) << X_PREC)
#define DIV_I(x,y)	((int)div_table_short[((x) & 0xFF) | ((y << 8) & 0xFF00)])
#define DIV_X(x,y)	((int)div_table_short[((x) & 0xFF) | (((y) << 8) & 0xFF00)])
#define SHL_8(x)	(((unsigned int)(x) << 8) + (1 << 7))

#define NO_SHIFT_Z() {				\
	I0 = normals[0] -> I_8 >> color_shift;	\
	I1 = normals[1] -> I_8 >> color_shift;	\
	I2 = normals[2] -> I_8 >> color_shift;	\
	}


#define SHIFT_COUNTERCLOCKWISE_Z() {		\
	SWAP(x0,x2);				\
	SWAP(x1,x2);				\
							\
	SWAP(y0,y2);				\
	SWAP(y1,y2);				\
							\
	SWAP(z0,z2);				\
	SWAP(z1,z2);				\
						\
	I0 = normals[2] -> I_8 >> color_shift;	\
	I1 = normals[0] -> I_8 >> color_shift;	\
	I2 = normals[1] -> I_8 >> color_shift;	\
	}


#define SHIFT_CLOCKWISE_Z() {			\
	SWAP(x0,x1);				\
	SWAP(x1,x2);				\
						\
	SWAP(y0,y1);				\
	SWAP(y1,y2);				\
						\
	SWAP(z0,z1);				\
	SWAP(z1,z2);				\
						\
	I0 = normals[1] -> I_8 >> color_shift;	\
	I1 = normals[2] -> I_8 >> color_shift;	\
	I2 = normals[0] -> I_8 >> color_shift;	\
	}

#define cycle_true_z(d)		{											\
	unsigned char xl_,xr_,len,z;												\
	int cf,cfk;															\
	int zf,zfk;															\
	while(d-- > 0){														\
		if(draw_xl > draw_xr)												\
			return;														\
																	\
		xl_ = draw_xl >> X_PREC;										\
		xr_ = draw_xr >> X_PREC;										\
																		\
		len = xr_ - xl_;													\
		if(len){															\
			cf = draw_cl;													\
			cfk = DIV_X((((draw_cr - draw_cl) >> X_PREC) + 1),len);				\
			zf = draw_zl;													\
			zfk = DIV_X((((draw_zr - draw_zl) >> X_PREC) + 1),len);				\
			draw_vp_offset += 2*xl_;									\
			do {												\
				draw_vp = draw_buffer + (draw_vp_offset & (2*DRAW_SX*DRAW_SY - 1)); \
				if((z = zf >> X_PREC) > *draw_vp){				\
					*draw_vp = z;						\
					*(draw_vp + 1) = cf >> X_PREC;				\
					draw_vp_offset += 2;				\
					}						\
				else							\
					draw_vp_offset += 2;				\
				cf += cfk;				\
				zf += zfk;				\
				} while(--len);				\
			draw_vp_offset -= 2*xr_;				\
		}								\
										\
		draw_Y++;						\
		draw_vp_offset += draw_x_size;				\
										\
		draw_xl += draw_al;				\
		draw_xr += draw_ar;				\
										\
		draw_cl += draw_ckl;				\
		draw_cr += draw_ckr;				\
										\
		draw_zl += draw_zkl;				\
		draw_zr += draw_zkr;				\
		}								\
	}				
inline void Polygon::draw_gouraud_decomposed_true_z()
{
	int draw_xl,draw_xr;
	int draw_al,draw_ar;
	int draw_cl,draw_cr,draw_ckl,draw_ckr;
	int draw_zl,draw_zr,draw_zkl,draw_zkr;
	unsigned char draw_Y;
	unsigned char* draw_vp;
	int draw_vp_offset;

	unsigned char bl,br;
	unsigned char I0,I1,I2;

	unsigned char color_offset = COLORS_VALUE_TABLE[color_id*2];
	unsigned char color_shift = COLORS_VALUE_TABLE[color_id*2 + 1];

	unsigned char x0 = vertices[0] -> xscr_8;
	unsigned char y0 = vertices[0] -> yscr_8;
	unsigned char z0 = vertices[0] -> zscr_8;

	unsigned char x1 = vertices[1] -> xscr_8;
	unsigned char y1 = vertices[1] -> yscr_8;
	unsigned char z1 = vertices[1] -> zscr_8;

	unsigned char x2 = vertices[2] -> xscr_8;
	unsigned char y2 = vertices[2] -> yscr_8;
	unsigned char z2 = vertices[2] -> zscr_8;


	if(x0 == x1 && x1 == x2)
		return;

	if(y0 == y1){
		if(y1 == y2 || x0 == x1)
			return;
		if(y0 < y2){
			NO_SHIFT_Z();
			goto case_3;
			}
		else{
			SHIFT_COUNTERCLOCKWISE_Z();
			goto case_4;
			}
		}
	if(y1 == y2){
		if(x1 == x2)
			return;
		if(y1 < y0){
			SHIFT_CLOCKWISE_Z();
			goto case_3;
			}
		else{
			NO_SHIFT_Z();
			goto case_4;
			}
		}
	if(y0 == y2){
		if(x0 == x2)
			return;
		if(y0 < y1){
			SHIFT_COUNTERCLOCKWISE_Z();
			goto case_3;
			}
		else{
			SHIFT_CLOCKWISE_Z();
			goto case_4;
			}
		}


	if(y0 < y1){
		if(y1 < y2){
			NO_SHIFT_Z();
			goto case_1;
			}
		else
			if(y0 < y2){
				NO_SHIFT_Z();
				goto case_2;
				}
			else{
				SHIFT_COUNTERCLOCKWISE_Z();
				goto case_1;
				}
		}
	else{
		if(y1 > y2){
			SHIFT_COUNTERCLOCKWISE_Z();
			goto case_2;
			}
		else
			if(y0 < y2){
				SHIFT_CLOCKWISE_Z();
				goto case_2;
				}
			else{
				SHIFT_CLOCKWISE_Z();
				goto case_1;
				}
		}


case_1:

	draw_Y = y0;
	draw_xr = draw_xl = SHL_8(x0);
	bl = y2 - draw_Y;
	br = y1 - draw_Y;

	draw_cr = draw_cl = COLOR(I0);
	draw_zr = draw_zl = SHL_8(z0);

	draw_al = DIV_X(x2 - x0,bl);
	draw_ckl = DIV_I(I2 - I0,bl);
	draw_zkl = DIV_I(z2 - z0,bl);
	draw_ar = DIV_X(x1 - x0,br);
	draw_ckr = DIV_I(I1 - I0,br);
	draw_zkr = DIV_I(z1 - z0,br);

	if(draw_al == draw_ar)
		return;

	draw_vp_offset = ((((draw_Y + CentrY) << draw_shift) + CentrX) << 1);

	cycle_true_z(br);

	draw_xr = SHL_8(x1);
	br = y2 - draw_Y;

	draw_cr = COLOR(I1);
	draw_zr = SHL_8(z1);

	draw_ar = DIV_X(x2 - x1,br);
	draw_ckr = DIV_I(I2 - I1,br);
	draw_zkr = DIV_I(z2 - z1,br);

	cycle_true_z(br);
	return;

case_2:

	draw_Y = y0;
	draw_xr = draw_xl = SHL_8(x0);
	bl = y2 - draw_Y;
	br = y1 - draw_Y;

	draw_cr = draw_cl = COLOR(I0);
	draw_zr = draw_zl = SHL_8(z0);

	draw_al = DIV_X(x2 - x0,bl);
	draw_ckl = DIV_I(I2 - I0,bl);
	draw_zkl = DIV_I(z2 - z0,bl);
	draw_ar = DIV_X(x1 - x0,br);
	draw_ckr = DIV_I(I1 - I0,br);
	draw_zkr = DIV_I(z1 - z0,br);

	if(draw_al == draw_ar)
		return;

	draw_vp_offset = ((((draw_Y + CentrY) << draw_shift) + CentrX) << 1);

	cycle_true_z(bl);

	draw_xl = SHL_8(x2);
	bl = y1 - draw_Y;

	draw_cl = COLOR(I2);
	draw_zl = SHL_8(z2);

	draw_al = DIV_X(x1 - x2,bl);
	draw_ckl = DIV_I(I1 - I2,bl);
	draw_zkl = DIV_I(z1 - z2,bl);

	cycle_true_z(bl);
	return;

case_3:

	draw_Y = y0;
	draw_xl = SHL_8(x0);
	draw_xr = SHL_8(x1);
	bl = y2 - draw_Y;

	draw_cl = COLOR(I0);
	draw_cr = COLOR(I1);
	draw_zl = SHL_8(z0);
	draw_zr = SHL_8(z1);
	
	draw_al = DIV_X(x2 - x0,bl);
	draw_ar = DIV_X(x2 - x1,bl);
	draw_ckl = DIV_I(I2 - I0,bl);
	draw_ckr = DIV_I(I2 - I1,bl);
	draw_zkl = DIV_I(z2 - z0,bl);
	draw_zkr = DIV_I(z2 - z1,bl);

	draw_vp_offset = ((((draw_Y + CentrY) << draw_shift) + CentrX) << 1);

	cycle_true_z(bl);
	return;

case_4:

	draw_Y = y0;
	draw_xr = draw_xl = SHL_8(x0);
	bl = y2 - draw_Y;

	draw_cr = draw_cl = COLOR(I0);
	draw_zr = draw_zl = SHL_8(z0);

	draw_al = DIV_X(x2 - x0,bl);
	draw_ar = DIV_X(x1 - x0,bl);
	draw_ckl = DIV_I(I2 - I0,bl);
	draw_ckr = DIV_I(I1 - I0,bl);
	draw_zkl = DIV_I(z2 - z0,bl);
	draw_zkr = DIV_I(z1 - z0,bl);

	draw_vp_offset  = ((((draw_Y + CentrY) << draw_shift) + CentrX) << 1);

	cycle_true_z(bl);
	return;
}

/*******************************************************************************
			Non-Masked Draw Shadow & Image Functions
*******************************************************************************/
#ifndef _SURMAP_
void putspr(int x,int y,int size,int shift,unsigned char* buffer,int image)
{
	int dx,dy;
	int nx,ny;
	int nxs,nys;
	int i;
	int vadd,dadd;
	unsigned char* vbuf;
	unsigned char* dbuf;

	if(y > VcutDown || x > UcutRight)
		return;

	dx = x + size;
	dy = y + size;
	nxs = size;
	nys = size;
	nx = x;
	ny = y;

	if(ny < VcutUp){
		if(dy <= VcutUp)
			return;
		else{
			ny = VcutUp;
			nys = dy - VcutUp;
			}
		}
	if(nx < UcutLeft){
		if(dx <= UcutLeft)
			return;
		else{
			nx = UcutLeft;
			nxs = (dx - UcutLeft) & ~1;
			}
		}

	if(dy >= VcutDown)
		nys = VcutDown - ny;
	if(dx >= UcutRight){
		nxs = UcutRight - nx;
		nx += nxs & 1;
		nxs &= ~1;
		}

	vadd = XGR_MAXX - nxs;
	dadd = ((1 << shift) - nxs) << 1;

	vbuf = XGR_VIDEOBUF + ny*XGR_MAXX + nx;
	dbuf = buffer + ((((ny - y) << shift) + (nx - x)) << 1);

	nx -= ScreenCX;
	ny -= ScreenCY;

	int delta,len;
	int* ibuf;

	if(image)
		for(i = 0;i < nys;i++){
			len = nxs;

			// Skip of left zeros
			ibuf = (int*)dbuf;
			delta = len;
			while(len > 0 && !*ibuf){
				len -= 2;
				ibuf++;
				}
			delta -= len;
			vbuf += delta;
			dbuf += delta << 1;

			// Skip of right zeros
			ibuf += (len >> 1) - 1;
			delta = len;
			while(len > 0 && !*ibuf){
				len -= 2;
				ibuf--;
				}
			delta -= len;

			while(len-- > 0){
				if(*dbuf)
					*vbuf = *(dbuf + 1);
				vbuf++;
				dbuf += 2;
				}
			vbuf += vadd + delta;
			dbuf += dadd + (delta << 1);
			}
#ifdef _ROAD_
	else
		for(i = 0;i < nys;i++){
			len = nxs;

			// Skip of left zeros
			ibuf = (int*)dbuf;
			delta = len;
			while(len > 0 && !*ibuf){
				len -= 2;
				ibuf++;
				}
			delta -= len;
			vbuf += delta;
			dbuf += delta << 1;

			// Skip of right zeros
			ibuf += (len >> 1) - 1;
			delta = len;
			while(len > 0 && !*ibuf){
				len -= 2;
				ibuf--;
				}
			delta -= len;
					                                      
			while(len-- > 0){
				if(*dbuf)
					*vbuf = ShadowColorTable[*vbuf];
				vbuf++;
				dbuf += 2;
				}
			vbuf += vadd + delta;
			dbuf += dadd + (delta << 1);
			}
#endif
}
#endif
/*******************************************************************************
			Masked Draw Shadow & Image Functions
*******************************************************************************/
#ifdef _ROAD_
unsigned char* draw_vbuf;
unsigned char* draw_dbuf;
unsigned char* draw_mbuf;

int draw_k_xscr_x;
int draw_k_xscr_y;
int draw_k_xscr_x_2;
int draw_k_xscr_y_2;

unsigned char** draw_lt;

inline void shadow_line(int len,int fx,int fy)
{
	unsigned char dz;
	unsigned char* mbuf;

	// Skip of left zeros
	int* ibuf = (int*)draw_dbuf;
	int delta = len;
	while(len > 0 && !*ibuf){
		len -= 2;
		ibuf++;
		fx += draw_k_xscr_x_2;
		fy += draw_k_xscr_y_2;
		}
	delta -= len;
	draw_vbuf += delta;
	draw_dbuf += delta << 1;

	// Skip of right zeros
	ibuf += (len >> 1) - 1;
	delta = len;
	while(len > 0 && !*ibuf){
		len -= 2;
		ibuf--;
		}
	delta -= len;

	while(len-- > 0){
		uchar* linePtr = draw_lt[(fy >> 16) & clip_mask_y];
		if(linePtr != nullptr && (dz = *draw_dbuf) != 0){
			mbuf = linePtr + (((fx >> 16) | 1) & clip_mask_x);
			if ((unsigned int)z_low_level + (unsigned int)dz > (unsigned int)*mbuf)
				*draw_vbuf = ShadowColorTable[*draw_vbuf];
		}
		draw_vbuf++;
		draw_dbuf += 2;
		fx += draw_k_xscr_x;
		fy += draw_k_xscr_y;
	}
	draw_vbuf += delta;
	draw_dbuf += delta << 1;
}
inline void image_line(int len,int fx,int fy)
{
	int z,dz;
	unsigned char type, *mbuf;

	// Skip of left zeros
	int* ibuf = (int*)draw_dbuf;
	int delta = len;
	while(len > 0 && !*ibuf){
		len -= 2;
		ibuf++;
		fx += draw_k_xscr_x_2;
		fy += draw_k_xscr_y_2;
		}
	delta -= len;
	draw_vbuf += delta;
	draw_dbuf += delta << 1;

	// Skip of right zeros
	ibuf += (len >> 1) - 1;
	delta = len;
	while(len > 0 && !*ibuf){
		len -= 2;
		ibuf--;
		}
	delta -= len;

	while(len-- > 0) {
		auto linePtr = draw_lt[(fy >> 16) & clip_mask_y];
		if(linePtr != nullptr && (dz = *draw_dbuf) != 0) {
			mbuf = linePtr + (((fx >> 16) | 1) & clip_mask_x);
			z = z_low_level + dz - (int)*mbuf;
			if(z > 0){
				if(((type = *(mbuf + H_SIZE)) & (7 << 3)) != 0){
					if(!(type & OBJSHADOW) || z > OBJSHADOW_H)
						*draw_vbuf = *(draw_dbuf + 1);
					else
						*draw_vbuf = ShadowColorTable[*(draw_dbuf + 1)];
					draw_state |= TOUCH_OF_AIR;
					}
				else{
					z = z_low_level_water + dz;
					if(z > 0){
						if(!(type & OBJSHADOW) || z > OBJSHADOW_H)
							*draw_vbuf = *(draw_dbuf + 1);
						else
							*draw_vbuf = ShadowColorTable[*(draw_dbuf + 1)];
						draw_state |= TOUCH_OF_AIR;
						}
					else{
						*draw_vbuf = (*(draw_dbuf + 1) & 15);
						draw_state |= TOUCH_OF_WATER;
						}
					}
				}
			else
				if((type = *(mbuf + H_SIZE - 1)) & DOUBLE_LEVEL)
					draw_state |= (type & (7 << 3)) != 0 || z_low_level_water + dz > 0 ? TOUCH_OF_AIR : TOUCH_OF_WATER;
		}
		draw_vbuf++;
		draw_dbuf += 2;
		fx += draw_k_xscr_x;
		fy += draw_k_xscr_y;
	}
	draw_vbuf += delta;
	draw_dbuf += delta << 1;
}
inline void transparency_line(int len,int fx,int fy)
{
	int z,dz;
	unsigned char type, *mbuf;

	// Skip of left zeros
	int* ibuf = (int*)draw_dbuf;
	int delta = len;
	while(len > 0 && !*ibuf){
		len -= 2;
		ibuf++;
		fx += draw_k_xscr_x_2;
		fy += draw_k_xscr_y_2;
		}
	delta -= len;
	draw_vbuf += delta;
	draw_dbuf += delta << 1;

	// Skip of right zeros
	ibuf += (len >> 1) - 1;
	delta = len;
	while(len > 0 && !*ibuf){
		len -= 2;
		ibuf--;
		}
	delta -= len;

	while(len-- > 0){
		if((dz = *draw_dbuf) != 0){
			mbuf = draw_lt[(fy >> 16) & clip_mask_y] + (((fx >> 16) | 1) & clip_mask_x);
			z = z_low_level + dz - (int)*mbuf;
			if(z > 0){
				if(((type = *(mbuf + H_SIZE)) & (7 << 3)) != 0){
					if(!(type & OBJSHADOW) || z > OBJSHADOW_H)
						*draw_vbuf = TRANSPARENCY(*draw_vbuf,*(draw_dbuf + 1) & 15);
					else
						*draw_vbuf = TRANSPARENCY(*draw_vbuf,(*(draw_dbuf + 1) & 15) >> 1);
					draw_state |= TOUCH_OF_AIR;
					}
				else{
					z = z_low_level_water + dz;
					if(z > 0){
						if(!(type & OBJSHADOW) || z > OBJSHADOW_H)
							*draw_vbuf = TRANSPARENCY(*draw_vbuf,*(draw_dbuf + 1) & 15);
						else
							*draw_vbuf = TRANSPARENCY(*draw_vbuf,(*(draw_dbuf + 1) & 15) >> 1);
						draw_state |= TOUCH_OF_AIR;
						}
					else{
						*draw_vbuf = TRANSPARENCY(*draw_vbuf,(*(draw_dbuf + 1) & 15) >> 2);
						draw_state |= TOUCH_OF_WATER;
						}
					}
				}
			else
				if((type = *(mbuf + H_SIZE - 1)) & DOUBLE_LEVEL)
					draw_state |= (type & (7 << 3)) != 0 || z_low_level_water + dz > 0 ? TOUCH_OF_AIR : TOUCH_OF_WATER;
			}
		draw_vbuf++;
		draw_dbuf += 2;
		fx += draw_k_xscr_x;
		fy += draw_k_xscr_y;
		}
	draw_vbuf += delta;
	draw_dbuf += delta << 1;
}
void DrawLinear(int x,int y,int size,int shift,unsigned char* buffer,int image)
{
	int dx,dy;
	int nx,ny;
	int nxs,nys;
	int i;
	int vadd,dadd;

	if(y > VcutDown || x > UcutRight)
		return;

	dx = x + size;
	dy = y + size;
	nxs = size;
	nys = size;
	nx = x;
	ny = y;

	if(ny < VcutUp){
		if(dy <= VcutUp)
			return;
		else{
			ny = VcutUp;
			nys = dy - VcutUp;
			}
		}
	if(nx < UcutLeft){
		if(dx <= UcutLeft)
			return;
		else{
			nx = UcutLeft;
			nxs = (dx - UcutLeft) & ~1;
			}
		}

	if(dy >= VcutDown)
		nys = VcutDown - ny;
	if(dx >= UcutRight){
		nxs = UcutRight - nx;
		nx += nxs & 1;
		nxs &= ~1;
		}

	vadd = XGR_MAXX - nxs;
	dadd = ((1 << shift) - nxs) << 1;

	draw_vbuf = XGR_VIDEOBUF + ny*XGR_MAXX + nx;
	draw_dbuf = buffer + ((((ny - y) << shift) + (nx - x)) << 1);

	draw_k_xscr_x = cosTurnInv;
	draw_k_xscr_y = -sinTurnInv;
	draw_k_xscr_x_2 = draw_k_xscr_x << 1;
	draw_k_xscr_y_2 = draw_k_xscr_y << 1;

	int k_yscr_x = sinTurnInv;
	int k_yscr_y = cosTurnInv;

	draw_lt = vMap -> lineT;

	nx -= ScreenCX;
	ny -= ScreenCY;

	int tfx = (ViewX << 16) + (nx*cosTurnInv + ny*sinTurnInv) + (1 << 15);
	int tfy = (ViewY << 16) + (-nx*sinTurnInv + ny*cosTurnInv) + (1 << 15);

	switch(image){
		case NORMAL_DRAW_MODE:
			for(i = 0;i < nys;i++){
				#ifndef LOW_LEVEL_LINE
					image_line(nxs,tfx,tfy);
				#else
					low_image_line(nxs,tfx,tfy);
				#endif
				draw_vbuf += vadd;
				draw_dbuf += dadd;
				tfx += k_yscr_x;
				tfy += k_yscr_y;
				}
			break;
		case 0:
			//std::cout<<"DrawLinear__shadow_line"<<std::endl;
			for(i = 0;i < nys;i++){
				#ifndef LOW_LEVEL_LINE
					shadow_line(nxs,tfx,tfy);
				#else
					low_shadow_line(nxs,tfx,tfy);
				#endif
				draw_vbuf += vadd;
				draw_dbuf += dadd;
				tfx += k_yscr_x;
				tfy += k_yscr_y;
				}
			break;
		case TRANSPARENCY_DRAW_MODE:
			//std::cout<<"DrawLinear__transparency_line"<<std::endl;
			for(i = 0;i < nys;i++){
				transparency_line(nxs,tfx,tfy);
				draw_vbuf += vadd;
				draw_dbuf += dadd;
				tfx += k_yscr_x;
				tfy += k_yscr_y;
				}
			break;
		}
}
void Draw3DPlane(int x,int y,int size,int shift,unsigned char* buffer,int image,int y_offset)
{
	int dx,dy;
	int nx,ny;
	int nxs,nys;
	int i;
	int vadd,dadd;

	if(y > VcutDown || x > UcutRight)
		return;

	dx = x + size;
	dy = y + size;
	nxs = size;
	nys = size;
	nx = x;
	ny = y;

	if(ny < VcutUp){
		if(dy <= VcutUp)
			return;
		else{
			ny = VcutUp;
			nys = dy - VcutUp;
			}
		}
	if(nx < UcutLeft){
		if(dx <= UcutLeft)
			return;
		else{
			nx = UcutLeft;
			nxs = (dx - UcutLeft) & ~1;
			}
		}

	if(dy >= VcutDown)
		nys = VcutDown - ny;
	if(dx >= UcutRight){
		nxs = UcutRight - nx;
		nx += nxs & 1;
		nxs &= ~1;
		}
	if(!nxs)
		return;

	vadd = XGR_MAXX - nxs;
	dadd = ((1 << shift) - nxs) << 1;

	draw_vbuf = XGR_VIDEOBUF + ny*XGR_MAXX + nx;
	draw_dbuf = buffer + ((((ny - y) << shift) + (nx - x)) << 1);

	int fx,fy;

	draw_lt = vMap -> lineT;

	nx -= ScreenCX;
	ny -= ScreenCY;
	ny += y_offset;

	int cx = ViewX << 16;
	int cy = ViewY << 16;

	double al = Ha*nx + Va*ny;
	double bl = Hb*nx + Vb*ny;
	double cl = Oc + Hc*nx + Vc*ny;

	double ar = Ha*(nx + nxs) + Va*ny;
	double br = Hb*(nx + nxs) + Vb*ny;

	double cl_inv;
	double nxs_inv = 1/(double)nxs;

	switch(image){
		case NORMAL_DRAW_MODE:
			for(i = 0;i < nys;i++){
				if(fabs(cl) < DBL_EPS) 
					return;
				cl_inv = 1/cl;
				fx = round(al*cl_inv);
				fy = round(bl*cl_inv);
				cl_inv *= nxs_inv;
				draw_k_xscr_x = round((ar - al)*cl_inv);
				draw_k_xscr_y = round((br - bl)*cl_inv);
				draw_k_xscr_x_2 = draw_k_xscr_x << 1;
				draw_k_xscr_y_2 = draw_k_xscr_y << 1;

				#ifndef LOW_LEVEL_LINE
					image_line(nxs,fx + cx,fy + cy);
				#else
					low_image_line(nxs,fx + cx,fy + cy);
				#endif

				draw_vbuf += vadd;
				draw_dbuf += dadd;

				al += Va;
				bl += Vb;
				cl += Vc;
				ar += Va;
				br += Vb;
				}
			break;
		case 0:
			for(i = 0;i < nys;i++){
				if(fabs(cl) < DBL_EPS) 
					return;
				cl_inv = 1/cl;
				fx = round(al*cl_inv);
				fy = round(bl*cl_inv);
				cl_inv *= nxs_inv;
				draw_k_xscr_x = round((ar - al)*cl_inv);
				draw_k_xscr_y = round((br - bl)*cl_inv);
				draw_k_xscr_x_2 = draw_k_xscr_x << 1;
				draw_k_xscr_y_2 = draw_k_xscr_y << 1;

				#ifndef LOW_LEVEL_LINE
					shadow_line(nxs,fx + cx,fy + cy);
				#else
					low_shadow_line(nxs,fx + cx,fy + cy);
				#endif

				draw_vbuf += vadd;
				draw_dbuf += dadd;

				al += Va;
				bl += Vb;
				cl += Vc;
				ar += Va;
				br += Vb;
				}
			break;
		case TRANSPARENCY_DRAW_MODE:
			for(i = 0;i < nys;i++){
				if(fabs(cl) < DBL_EPS) 
					return;
				cl_inv = 1/cl;
				fx = round(al*cl_inv);
				fy = round(bl*cl_inv);
				cl_inv *= nxs_inv;
				draw_k_xscr_x = round((ar - al)*cl_inv);
				draw_k_xscr_y = round((br - bl)*cl_inv);
				draw_k_xscr_x_2 = draw_k_xscr_x << 1;
				draw_k_xscr_y_2 = draw_k_xscr_y << 1;

				transparency_line(nxs,fx + cx,fy + cy);

				draw_vbuf += vadd;
				draw_dbuf += dadd;

				al += Va;
				bl += Vb;
				cl += Vc;
				ar += Va;
				br += Vb;
				}
			break;
		}
}
/*******************************************************************************
		    Masked Put Bitmap Function
*******************************************************************************/
void color_line(int len,unsigned char* dbuf,int bx,int bKx,int fx,int fy);
void transparency_line(int len,unsigned char* dbuf,int bx,int bKx,int fx,int fy);
void color_line_non_test(int len,unsigned char* dbuf,int bx,int bKx);
void transparency_line_non_test(int len,unsigned char* dbuf,int bx,int bKx);

void smart_putspr(unsigned char* data,int Xcenter,int Ycenter,int XsizeB,int YsizeB,int ScaleXsize,int height,unsigned char* color_table)
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
	draw_color_table = color_table;
	draw_vbuf = XGR_VIDEOBUF + Y*XGR_MAXX + X;
	draw_lt = vMap -> lineT;
	z_low_level = height;

	if(height >= 256){
		while(YsizeS-- > 0){
			if(color_table)
				transparency_line_non_test(XsizeS,data + (by >> 15)*XsizeB,bx,bKx);
			else
				color_line_non_test(XsizeS,data + (by >> 15)*XsizeB,bx,bKx);
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
			if(color_table)
				transparency_line(XsizeS,data + (by >> 15)*XsizeB,bx,bKx,tfx,tfy);
			else
				color_line(XsizeS,data + (by >> 15)*XsizeB,bx,bKx,tfx,tfy);
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

		double cl_inv;
		double nxs_inv = 1/(double)XsizeS;

		while(YsizeS-- > 0){
			if(fabs(cl) < DBL_EPS) 
				return;
			cl_inv = 1/cl;
			fx = round(al*cl_inv);
			fy = round(bl*cl_inv);
			cl_inv *= nxs_inv;
			draw_k_xscr_x = round((ar - al)*cl_inv);
			draw_k_xscr_y = round((br - bl)*cl_inv);
			if(color_table)
				transparency_line(XsizeS,data + (by >> 15)*XsizeB,bx,bKx,fx + cx,fy + cy);
			else
				color_line(XsizeS,data + (by >> 15)*XsizeB,bx,bKx,fx + cx,fy + cy);

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
void color_line(int len,unsigned char* dbuf,int bx,int bKx,int fx,int fy)
{
	unsigned char c;
	unsigned char* mbuf;
	while(len-- > 0){
		if((c = dbuf[bx >> 15]) != 0){
			mbuf = draw_lt[(fy >> 16) & clip_mask_y] + (((fx >> 16) | 1) & clip_mask_x);
			if(z_low_level > *mbuf)
				*draw_vbuf = c;
			}
		draw_vbuf++;
		bx += bKx;
		fx += draw_k_xscr_x;
		fy += draw_k_xscr_y;
		}
}
void transparency_line(int len,unsigned char* dbuf,int bx,int bKx,int fx,int fy)
{
	unsigned char c;
	unsigned char* mbuf;
	while(len-- > 0){
		if((c = dbuf[bx >> 15]) != 0){
			mbuf = draw_lt[(fy >> 16) & clip_mask_y] + (((fx >> 16) | 1) & clip_mask_x);
			if(z_low_level > *mbuf)
				*draw_vbuf = draw_color_table[*draw_vbuf + (c << 8)];
			}
		draw_vbuf++;
		bx += bKx;
		fx += draw_k_xscr_x;
		fy += draw_k_xscr_y;
		}
}
void color_line_non_test(int len,unsigned char* dbuf,int bx,int bKx)
{
	unsigned char c;
	while(len-- > 0){
		if((c = dbuf[bx >> 15]) != 0)
			*draw_vbuf = c;
		draw_vbuf++;
		bx += bKx;
		}
}
void transparency_line_non_test(int len,unsigned char* dbuf,int bx,int bKx)
{
	unsigned char c;
	while(len-- > 0){
		if((c = dbuf[bx >> 15]) != 0)
			*draw_vbuf = draw_color_table[*draw_vbuf + (c << 8)];
		draw_vbuf++;
		bx += bKx;
		}
}
/*******************************************************************************
		    Double Level Destroying
*******************************************************************************/
void Object::destroy_double_level()
{							  
	draw_offset = radius + 2;
	convert_offset = draw_offset;
	draw_size = draw_offset*2;
	draw_shift = BitSR(draw_size) + 1;
	draw_x_size = 1 << (draw_shift + 1);
	if(draw_shift > 8)
		ErrH.Abort("Incorrect draw shift");

	memset(draw_buffer,0,2*draw_size << draw_shift);

	z_global_offset = radius + 2;
	if(z_global_offset < 30)
		z_global_offset = 30;
	z_low_level_water = R_curr.z - z_global_offset;
#ifdef _ROAD_
	z_low_level = z_low_level_water + dz_between_water_and_mask_levels;
#endif
	z_low_level_water -= FloodLEVEL;

	CentrX = 0;
	CentrY = 0;
	A_convert = DBM(A_l2g[0],A_l2g[1],A_l2g[2],
					A_l2g[3],A_l2g[4],A_l2g[5],
					-A_l2g[6],-A_l2g[7],-A_l2g[8])*scale_real;
	A_convert_8 = Matrix(A_convert*127);
	model -> z_buffering_draw();

	int x,y,log;
	int x0 = R_curr.x - draw_offset;
	int x0_1 = x0 & 1;
	unsigned char* dbuf = draw_buffer + (x0_1 << 1);
	int dadd = ((1 << draw_shift) - draw_size + (x0_1 << 1)) << 1;
	int x1 = x0 + draw_size - x0_1;
	x0 += x0_1;
	unsigned char *p0, *p;
	unsigned char** lineT = vMap -> lineT;
	int change_log = 0;
	for(y = R_curr.y - draw_offset; y < R_curr.y + draw_offset;y++){
		p0 = lineT[y & clip_mask_y];
		log = 0;
		if(p0)
			for(x = x0;x < x1;x += 2,dbuf += 4){
				if(*dbuf || *(dbuf + 2))
					log = 2;
				if(log-- > 0){
					p = p0 + (x & clip_mask_x);
					if(p[H_SIZE] & DOUBLE_LEVEL){
						p[1 + H_SIZE] = p[H_SIZE] &= ~DOUBLE_LEVEL;
						p[1] = (p0[(x + 2) & clip_mask_x] + *p) >> 1;
						change_log = 1;
						}
					}
				}
		dbuf += dadd;
		}
	if(change_log){
		SOUND_DESTROY_TERRAIN();
		regRender(R_curr.x - draw_offset,R_curr.y - draw_offset,R_curr.x + draw_offset,R_curr.y + draw_offset);
		}
}

/*******************************************************************************
		    Ground Pressing
*******************************************************************************/
void Object::ground_pressing() {
	int i;
	if(analysis_off || !(dynamic_state & (GROUND_COLLISION | WHEELS_TOUCH)) || mole_on)
		return;

	draw_offset = radius + 2;
	convert_offset = draw_offset;
	draw_size = draw_offset*2;
	draw_shift = BitSR(draw_size) + 1;
	draw_x_size = 1 << (draw_shift + 1);
	if(draw_shift > 8)
		ErrH.Abort("Incorrect draw shift");

	memset(draw_buffer,0,2*draw_size << draw_shift);

	z_global_offset = radius + 2;
	if(z_global_offset < 30)
		z_global_offset = 30;
	int z_low_level_pressing = R_curr.z + z_global_offset + ground_pressing_z_offset;

	CentrX = 0;
	CentrY = 0;
	A_convert = DBM(A_l2g[0],A_l2g[1],A_l2g[2],
								  A_l2g[3],A_l2g[4],A_l2g[5],
								  -A_l2g[6],-A_l2g[7],-A_l2g[8])*scale_real;
	A_convert_8 = Matrix(A_convert*127);
	model -> z_buffering_draw();
	DBM A_c2p = DBM(rudder,Z_AXIS);
	
	for(i = 0; i < n_wheels; i++) {
		if(wheels[i].steer) {
			wheels[i].model.draw_child(Vector(wheels[i].model.x_off,wheels[i].model.y_off,wheels[i].model.z_off),A_c2p,transpose(A_c2p));
		}
	}

	int x,y,z,log,lh,mh,uh;
	int x0 = R_curr.x - draw_offset;
	int x0_1 = x0 & 1;
	unsigned char* dbuf = draw_buffer + (x0_1 << 1);
	int dadd = ((1 << draw_shift) - draw_size + (x0_1 << 1)) << 1;
	int x1 = x0 + draw_size - x0_1;
	x0 += x0_1;
	unsigned char *p0, *p;
	unsigned char** lineT = vMap -> lineT;
	unsigned char type;
	int regrender_log = 0;
	for(y = R_curr.y - draw_offset; y < R_curr.y + draw_offset;y++){
		p0 = lineT[y & clip_mask_y];
		log = 0;
		if(p0)
			for(x = x0;x < x1;x += 2,dbuf += 4){
				if((z = *dbuf) != 0){
					p = p0 + (x & clip_mask_x);
					type = *(p + H_SIZE);
					if(!BREAKABLE_TERRAIN(type))
						continue;
					z = z_low_level_pressing - z;
					if(!(type & DOUBLE_LEVEL) ){ // type == (1<<3) == 8 это слой с тенью
						if(z < *p){
							//std::cout<<"ground_pressing p1 z:"<<z<<" p:"<<(int)*p<<" type:"<<(int)type<<std::endl;
							p[1] = *p = z > 0 ? z : 0;
							regrender_log = 1;
							}
						}
					else{
						if(!BREAKABLE_TERRAIN(p[1 + H_SIZE]))
							continue;
						if(z < (uh = p[1])){
							lh = *p;
							mh = GET_MIDDLE_HIGHT(lh,uh);
							if(z > mh){
								p[1] = z;
								regrender_log = 1;
								}
							else{
								if(z + 16 > mh){
									p[1 + H_SIZE] = p[H_SIZE] &= ~DOUBLE_LEVEL;
									p[1] = (p0[(x + 2) & clip_mask_x] + p[0]) >> 1;
									regrender_log = 1;
									}
								else{
									if(z < lh){
										p[1] = *p = z > 0 ? z : 0;
										regrender_log = 1;
										}
									}
								}
							}
						}
					}
				}
		dbuf += dadd;
		}
	
	if(regrender_log)
		regRender(R_curr.x - draw_offset,R_curr.y - draw_offset,R_curr.x + draw_offset,R_curr.y + draw_offset);
}
/*******************************************************************************
		    Cast Shadow Function
*******************************************************************************/
#define SHR8(a) (a  >> 8)
inline void shadowing_line(int x1_scr,int x2_scr) 
{													
	int x1 = x1_scr >> 8;								
	int x2 = x2_scr >> 8;							
	while(x1 > x2){										
		draw_vbuf[x1] = ShadowColorTable[draw_vbuf[x1]];    	
		//draw_vbuf[x1] = draw_vbuf[x1] > 0 && draw_vbuf[x1] < 255 ? 0 : 255; 
		x1--;											
		} 
}
void cast_shadow_line(int x_scr0,int xg0,int zg0,int len)
{
	int z,dz,zA,zB;
	unsigned char *p,*buf;
	int xB,xg,xgt;
	int x1_scr,x2_scr;
	int left_border_8 = UcutLeft << 8;
	int right_border = UcutRight;

	zg0 = (zg0 << 8) + (1 << 7);
	xB = len - 2;
	for(;;){
		buf = draw_dbuf + (xB << 1);
		// Skip of right zeros, searching point A
		while(xB > 0 && !*buf){
			xB--;
			buf -= 2;
			}
		if(xB <= 0)
			return;
		// Skip of non-zeros, searching point B
		zB = (int)*buf << 8;
		zA = zB >> 1;
		do {
			xB--;
			buf -= 2;
			zA -= ScreenSHADOWDEEP;
			zB -= ScreenSHADOWDEEP;
			if((z = *buf) != 0) {
				if((z <<= 8) > zB)
					zB = z;
            } else {
				// Skip small area of zeros
				if(xB < 2) {
					goto end_of_scan_section;
                }
				if(!(*(buf -= 2) | *(buf -= 2))){
					buf += 4;
					goto end_of_scan_section;
                }
				zA -= ScreenSHADOWDEEP << 1;
				zB -= ScreenSHADOWDEEP << 1;
				xB -= 2;
            }
        } while(xB > 0);
		
end_of_scan_section:
		x2_scr = x_scr0 + xB;
		// Right Border skip
		while(x2_scr >= right_border){
			x2_scr--;
			zA -= ScreenSHADOWDEEP;
			zB -= ScreenSHADOWDEEP;
			}
		if(x2_scr < UcutLeft)
			return;
			
		xg  = xg0 + ((x2_scr - x_scr0)*ScaleMap >> 8);
		zA += zg0;
		zB += zg0;
		
		// Scan in Map Space
		x2_scr = (x2_scr << 8) + (1 << 7);
		while(1){
			// Scan of lower ray 
	scan_lower_ray:
			while(1){
				if(x2_scr <= left_border_8)
					return;
				z = *(p = draw_mbuf + ((xg | 1) & clip_mask_x));
				if(z > SHR8(zA))
					break;
				xg--;
				x2_scr -= ScaleMapInv;
				zA -= SHADOWDEEP_3D;
				zB -= SHADOWDEEP_3D;
				}

			// Scan of interval between lower and upper rays
			x1_scr = x2_scr;
			while(1){
				if(z > SHR8(zB)){
					shadowing_line(x1_scr,x2_scr);
					if(!(*(p + H_SIZE) & DOUBLE_LEVEL))
						goto end_scan_of_part;
					else{
						xgt = xg;
						dz = zB - ((z - 16) << 8);
						if(dz > 0)
							zB -= dz;
						while(1){
							xgt++;
							zB -= SHADOWDEEP_3D;
							p = draw_mbuf + (xgt & clip_mask_x);
							if(!(*(p + H_SIZE) & DOUBLE_LEVEL)) {
								if(*p > SHR8(zB)) {
									goto end_scan_of_part;
                                } else {
									break;
                                }
                            }
							if(zB < zA)
								goto end_scan_of_part;
							}	
						// Scan of upper ray 
						while(1){
							p = draw_mbuf + ((xg & ~1) & clip_mask_x);
							if(*p > SHR8(zB))
								goto end_scan_of_part;
							if(!(*(p + H_SIZE) & DOUBLE_LEVEL))
								goto scan_lower_ray;
							xg--;
							x2_scr -= ScaleMapInv;
							zA -= SHADOWDEEP_3D;
							zB -= SHADOWDEEP_3D;
							}	
						}
					}
				x2_scr -= ScaleMapInv;
				if(x2_scr <= left_border_8){
					shadowing_line(x1_scr,left_border_8);
					return;
					}
				xg--;
				zB -= SHADOWDEEP_3D;
				zA -= SHADOWDEEP_3D;
				z = *(p = draw_mbuf + ((xg | 1) & clip_mask_x));
				if(z < SHR8(zA)){
					shadowing_line(x1_scr,x2_scr);
					goto scan_lower_ray;
					}
				}
			}

end_scan_of_part: ;
		right_border = x2_scr >> 8;
		}
}	
void CastShadow(int x,int y,int zg,int size,int shift,unsigned char* buffer)
{
	int dx,dy;
	int nx,ny;
	int nxs,nys;
	int i;
	int vadd,dadd;

	if(y > VcutDown)
		return;

	dx = x + size;
	dy = y + size;
	nxs = size;
	nys = size;
	nx = x;
	ny = y;

	if(ny < VcutUp){
		if(dy <= VcutUp)
			return;
		else{
			ny = VcutUp;
			nys = dy - VcutUp;
			}
		}

	if(dy >= VcutDown)
		nys = VcutDown - ny;

	vadd = XGR_MAXX;
	dadd = (1 << shift) << 1;

	ScreenSHADOWDEEP = round(SHADOWDEEP_3D*ScaleMapFlt);
	draw_vbuf = XGR_VIDEOBUF + ny*XGR_MAXX;
	draw_dbuf = buffer + (((ny - y) << shift) << 1);

	int k_yscr_x = sinTurnInv;
	int k_yscr_y = cosTurnInv;

	draw_lt = vMap -> lineT;

	nx -= ScreenCX;
	ny -= ScreenCY;

	int tfx = (ViewX << 16) + (nx*cosTurnInv + ny*sinTurnInv) + (1 << 15);
	int tfy = (ViewY << 16) + (-nx*sinTurnInv + ny*cosTurnInv) + (1 << 15);

	for(i = 0;i < nys;i++){
		draw_mbuf = draw_lt[(tfy >> 16) & clip_mask_y];
		if(draw_mbuf)
			cast_shadow_line(x,tfx >> 16,zg,nxs);
		draw_vbuf += vadd;
		draw_dbuf += dadd;
		tfx += k_yscr_x;
		tfy += k_yscr_y;
		}
}

/*******************************************************************************
		    Cast Shadow Function from Arbitrary Located Source
*******************************************************************************/
#define SHR16(a) ((a + (1 << 15)) >> 16)
#define SHL16(a) ((a << 16) + (1 << 15))
#define ZB(x,y)		(draw_buffer[( ( (x >> 16) + ((y >> 16) << draw_shift) ) << 1) & 0x1ffff])
int ScaleMapTurned;
int ScaleMapInvTurned;
int sLast;
int draw_x_scr;
int draw_y_scr;
inline void arbitrary_shadowing_line(int s1,int s2) 
{													
	unsigned char* p;
	int len = (s1 >> 8) - sLast;
	while(len > 0){										
		draw_x_scr += draw_dx;
		draw_y_scr += draw_dy;	  
		sLast++;
		len--;				
		} 

	len = (s2 >> 8) - (s1 >> 8);								
	sLast += len;
	while(len > 0){										
		p = XGR_GetVideoLine(draw_y_scr >> 16) + (draw_x_scr >> 16);
		*p = ShadowColorTable[*p];    	
		//*p = *p > 0 && *p < 255 ? 0 : 255;
		draw_x_scr += draw_dx;
		draw_y_scr += draw_dy;	  
		len--;				
		}
}
void cast_arbitrary_shadow_line(int x_bmp_16,int y_bmp_16,int xg0,int zg0,int bmp_len,int screen_len,int screen_skip)
{
//	clip_pixel(draw_x_bmp_corner_16 + x_bmp_16 >> 16,
//			draw_y_bmp_corner_16 + y_bmp_16 >> 16,111);
//	clip_pixel(draw_x_bmp_corner_16 + x_bmp_16 + draw_dx*screen_len >> 16,
//			draw_y_bmp_corner_16 + y_bmp_16 + draw_dy*screen_len >> 16,143);
//	clip_pixel(draw_x_bmp_corner_16 +x_bmp_16 + draw_dx*bmp_len >> 16,
//			draw_y_bmp_corner_16 + y_bmp_16 + draw_dy*bmp_len >> 16,152);
//	clip_pixel(draw_x_bmp_corner_16 +x_bmp_16 + draw_dx*screen_skip >> 16,
//			draw_y_bmp_corner_16 + y_bmp_16 + draw_dy*screen_skip >> 16,174);

	int xg,xgt;
	int z,dz,zA,zB;
	unsigned char *p,zc;
	int xB,xC;
	int yB,yC;
	int sA,sB,sC;
	int screen_len_8 = screen_len << 8;

	zg0 = (1 << 7) + (zg0 << 8);
	xB = x_bmp_16;
	yB = y_bmp_16;
	sB = 0;
	for(;;){
		// Skip of right zeros, searching point A
		while(sB < bmp_len && !ZB(xB,yB)){
			sB++;
			xB += draw_dx;
			yB += draw_dy;
			}
		if(sB >= bmp_len)
			return;
		// Skip of non-zeros, searching point B
		zB = (int)ZB(xB,yB) << 8;
		zA = zB >> 1;
		do {
			sB++;
			zA -= ScreenSHADOWDEEP;
			zB -= ScreenSHADOWDEEP;
			xB += draw_dx;
			yB += draw_dy;
			if((z = ZB(xB,yB)) != 0){
				if((z <<= 8) > zB)
					zB = z;
				}
			else{
				// Skip small area of zeros
				if(bmp_len - sB < 2)
					goto end_of_scan_section;
				zc = 0;
				xC = xB + draw_dx;
				yC = yB + draw_dy;
				zc |= ZB(xC,yC);
				xC += draw_dx;
				yC += draw_dy;
				zc |= ZB(xC,yC);
				if(!zc)
					goto end_of_scan_section;
				xB = xC;
				yB = yC;
				zA -= ScreenSHADOWDEEP << 1;
				zB -= ScreenSHADOWDEEP << 1;
				sB += 2;
				}
			} while(sB < bmp_len);

end_of_scan_section:
		sC = sB;
		xC = xB;
		yC = yB;
		// Right Border skip
		while(sC < screen_skip){
			sC++;			    
			xC += draw_dx;
			yC += draw_dy;
			zA -= ScreenSHADOWDEEP;
			zB -= ScreenSHADOWDEEP;
			}

		if(sC >= screen_len)
			return;
		
		xg  = xg0 - (sC*ScaleMapTurned >> 8);
		zA += zg0;
		zB += zg0;
		
		// Scan in Map Space
		sLast = sC;
		sC = (sC << 8) + (1 << 7);
		draw_x_scr = draw_x_bmp_corner_16 + xC;
		draw_y_scr = draw_y_bmp_corner_16 + yC;
		while(1){
			// Scan of lower ray 
	scan_lower_ray:
			while(1){
				if(sC >= screen_len_8)
					return;
				z = *(p = draw_mbuf + ((xg | 1) & clip_mask_x));
				if(z > SHR8(zA))
					break;
				xg--;
				sC += ScaleMapInvTurned;
				zA -= SHADOWDEEP_3D;
				zB -= SHADOWDEEP_3D;
				}

			// Scan of interval between lower and upper rays
			sA = sC;
			while(1){
				if(z > SHR8(zB)){
					arbitrary_shadowing_line(sA,sC);
					if(!(*(p + H_SIZE) & DOUBLE_LEVEL))
						goto end_scan_of_part;
					else{
						xgt = xg;
						dz = zB - ((z - 16) << 8);
						if(dz > 0)
							zB -= dz;
						while(1){
							xgt++;
							zB -= SHADOWDEEP_3D;
							p = draw_mbuf + (xgt & clip_mask_x);
							if(!(*(p + H_SIZE) & DOUBLE_LEVEL)) {
								if(*p > SHR8(zB)) {
									goto end_scan_of_part;
								} else {
									break;
                                }
                            }
							if(zB < zA)
								goto end_scan_of_part;
							}	
						// Scan of upper ray 
						while(1){
							p = draw_mbuf + ((xg & ~1) & clip_mask_x);
							if(*p > SHR8(zB))
								goto end_scan_of_part;
							if(!(*(p + H_SIZE) & DOUBLE_LEVEL))
								goto scan_lower_ray;
							xg--;
							sC += ScaleMapInvTurned;
							zA -= SHADOWDEEP_3D;
							zB -= SHADOWDEEP_3D;
							}	
						}
					}
				
				sC += ScaleMapInvTurned;
				if(sC >= screen_len_8){
					arbitrary_shadowing_line(sA,screen_len_8);
					return;
					}

				xg--;
				zA -= SHADOWDEEP_3D;
				zB -= SHADOWDEEP_3D;
				z = *(p = draw_mbuf + ((xg | 1) & clip_mask_x));
				if(z < SHR8(zA)){
					arbitrary_shadowing_line(sA,sC);
					goto scan_lower_ray;
					}
				}
			}

end_scan_of_part: 
		screen_skip = sLast;
		}
}	


void CastArbitraryShadow(int x,int y,int zg,int size,int shift,unsigned char* buffer)
{
	int tA,tB,tC,tD,tE,tF;
	int screen_len,d_screen_len;
	int screen_skip,d_screen_skip;
	int bmp_len,d_bmp_len;
	int x0,y0,dx0,dy0;
	int xg,yg,dxg,dyg,xx,yy,dCxg,dCyg;
	int s,t,tmax,tX;
	int dx,dy,dxa,dya,d_1_y,d_1_y_s,d_1_x_s,tg_alpha;
	double CosAbs;
	int x_direction,y_direction,xy_swap;
	int x_bmp_corner_16,y_bmp_corner_16;

	int X0 = x;
	int Y0 = y;
	int X1 = x + size - 1;
	int Y1 = y + size - 1;
	int y_size = size;

	int dborder = round(ScaleMapInvFlt);
	if(dborder < 3)
		dborder = 3;
	int XS0 = UcutLeft  + dborder;
	int XS1 = UcutRight - dborder;
	int YS0 = VcutUp  + dborder;
	int YS1 = VcutDown  - dborder;

	int k_xscr_x = cosTurnInv;
	int k_xscr_y = -sinTurnInv;
	int k_yscr_x = sinTurnInv;
	int k_yscr_y = cosTurnInv;

	draw_dx = -COS(TurnAngle);
	draw_dy = -SIN(TurnAngle);
	dxa = abs(draw_dx);
	dya = abs(draw_dy);
	if(dxa > dya){
		draw_dy = round((double)draw_dy*65536./(double)dxa);
		draw_dx = SIGN(draw_dx) << 16;
		xy_swap = 0;
		CosAbs = fabs(Cos(TurnAngle));
		x_bmp_corner_16 = x << 16;
		y_bmp_corner_16 = y << 16;
		dx = draw_dx;
		dy = draw_dy;
		}
	else{
		draw_dx = round((double)draw_dx*65536./(double)dya);
		draw_dy = SIGN(draw_dy) << 16;
		xy_swap = 1;
		SWAP(X0,Y0);
		SWAP(X1,Y1);
		SWAP(XS0,YS0);
		SWAP(XS1,YS1);
		SWAP(k_xscr_x,k_yscr_x);
		SWAP(k_xscr_y,k_yscr_y);
		CosAbs = fabs(Sin(TurnAngle));
		x_bmp_corner_16 = y << 16;
		y_bmp_corner_16 = x << 16;
		dx = draw_dy;
		dy = draw_dx;
		}
	
	dya = abs(dy);
	if(dya < 256)
		dy = dya = 0;

	if(dx <= 0){
		x_direction = 1;
		}
	else{
		x_direction = -1;
		SWAP(X0,X1);
		SWAP(XS0,XS1);
		k_xscr_x = -k_xscr_x;
		k_xscr_y = -k_xscr_y;
		}
	
	if(dy <= 0){
		y_direction = 1;
		}
	else{
		y_direction = -1;
		SWAP(Y0,Y1);
		SWAP(YS0,YS1);
		k_yscr_x = -k_yscr_x;
		k_yscr_y = -k_yscr_y;
		}

	if(!dy){
		if(y_direction > 0){
			if(Y1 <= YS0 || Y0 >= YS1)
				return;
			if(Y0 < YS0){
				y_size = Y1 - YS0;
				Y0 = YS0;
				}
			if(Y1 > YS1){
				y_size = YS1 - Y0;
				Y1 = YS1;
				}
			}
		else{
			if(Y1 >= YS0 || Y0 <= YS1)
				return;
			if(Y0 > YS0){
				y_size = -(Y1 - YS0);
				Y0 = YS0;
				}
			if(Y1 < YS1){
				y_size = -(YS1 - Y0);
				Y1 = YS1;
				}
			}
		}
	d_1_x_s = dx ? round((65536.*65536.)/(double)(-dx)) : 0;
	d_1_y_s = dy ? round((65536.*65536.)/(double)(-dy)) : 0;
	d_1_y = abs(d_1_y_s);
	tg_alpha = round((double)dya * fabs((float)d_1_x_s)/65536.);


	ScaleMapTurned = round(ScaleMap/CosAbs);
	ScaleMapInvTurned = round(ScaleMapInv*CosAbs);
	ScreenSHADOWDEEP = SHADOWDEEP_3D*ScaleMapTurned >> 8;
	draw_x_bmp_corner_16 = x << 16;
	draw_y_bmp_corner_16 = y << 16;

	draw_lt = vMap -> lineT;
	
	tA = (Y0 - YS0)*y_direction;
	tB = SHR16(y_size*tg_alpha) + tA;
	tC = y_size + tA;
	tD = y_size + tB;
	tE = SHR16((X1 - XS0)*tg_alpha*x_direction);
	tF = (YS1 - YS0)*y_direction - SHR16((XS1 - X1)*tg_alpha*x_direction);
	
	// A:
	x0 = SHL16(X1) - x_bmp_corner_16;
	y0 = (Y0 << 16) - y_bmp_corner_16;
	dx0 = 0;
	dy0 = dy <= 0 ? 65536 : -65536;
	bmp_len = 0;
	d_bmp_len = d_1_y;

	if(tA < tE){
		screen_len = (Y0 - YS0)*d_1_y_s;
		d_screen_len = d_1_y;
		}
	else{
		screen_len = (X1 - XS0)*d_1_x_s;
		d_screen_len = 0;
		}

	if(tA < tF){
		screen_skip = (X1 - XS1)*d_1_x_s;
		d_screen_skip = 0;
		}
	else{
		screen_skip = (Y0 - YS1)*d_1_y_s;
		d_screen_skip = d_1_y;
		}
	
	if(xy_swap){
		yy = X1 - ScreenCY;
		xx = Y0 - ScreenCX;
		}
	else{
		xx = X1 - ScreenCX;
		yy = Y0 - ScreenCY;
		}
	xg = (ViewX << 16) + (xx*cosTurnInv + yy*sinTurnInv) + (1 << 15);
	yg = (ViewY << 16) + (-xx*sinTurnInv + yy*cosTurnInv) + (1 << 15);
	dxg = k_yscr_x;
	dyg = k_yscr_y;
	dCxg = round(((double)draw_dx*cosTurnInv + (double)draw_dy*sinTurnInv)/65536.);
	dCyg = round(((double)draw_dx*(-sinTurnInv) + (double)draw_dy*cosTurnInv)/65536.);

	t = tA;
	tX = tB;
	for(s = 1;s < 4;s++){
		tmax = tX;
		if(t < tE && tmax > tE)
			tmax = tE;
		if(t < tF && tmax > tF)
			tmax = tF;
		
		for(;t < tmax;t++){
			draw_mbuf = draw_lt[(yg >> 16) & clip_mask_y];
			if(draw_mbuf) {
				if(xy_swap) {
					cast_arbitrary_shadow_line(y0,x0,xg >> 16,zg,bmp_len  >> 16,screen_len  >> 16,screen_skip >> 16);
                } else {
					cast_arbitrary_shadow_line(x0,y0,xg >> 16,zg,bmp_len  >> 16,screen_len  >> 16,screen_skip >> 16);
                }
            }
			x0 += dx0;
			y0 += dy0;
			xg += dxg;
			yg += dyg;
			bmp_len += d_bmp_len;
			screen_len += d_screen_len;
			screen_skip += d_screen_skip;
			if(s == 3){
				if(y_direction > 0)
					while((y0 >> 16) >= size){
						x0 += dx;
						y0 += dy;
						bmp_len -= 1 << 16;
						screen_len -= 1 << 16;
						screen_skip -= 1 << 16;
						xg += dCxg;
						yg += dCyg;
						}
				else
					while((y0 >> 16) < 0){
						x0 += dx;
						y0 += dy;
						bmp_len -= 1 << 16;
						screen_len -= 1 << 16;
						screen_skip -= 1 << 16;
						xg += dCxg;
						yg += dCyg;
						}
				}
			}

		if(t == tE){
			screen_len = round((double)(x_bmp_corner_16 + x0 - (XS0 << 16))*(double)d_1_x_s/65536.);
			d_screen_len = 0;
			tE = -INT_INF;
			s--;
			if(t == tF){
				screen_skip = round((double)(y_bmp_corner_16 + y0 - (YS1 << 16))*(double)d_1_y_s/65536.);
				d_screen_skip = d_1_y;
				tF = -INT_INF;
				}
			continue;
			}
		if(t == tF){
			screen_skip = round((double)(y_bmp_corner_16 + y0 - (YS1 << 16))*(double)d_1_y_s/65536.);
			d_screen_skip = d_1_y;
			tF = -INT_INF;
			s--;
			continue;
			}

		switch(s){
			case 1: // B - point
				bmp_len = SHL16(size);
				d_bmp_len = 0;
				tX = tC;
				break;
			case 2: // C - point
				tX = tD;
				break;
			case 3: // D - point
				return;
			}
		}
}
#endif
