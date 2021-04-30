
#ifndef __3D__3DGRAPH_H__
#define __3D__3DGRAPH_H__

/*******************************************************************************
		3D graphical structs and constants
*******************************************************************************/

const int  SCREEN_LEN_X = 4;
const int  SCREEN_LEN_Y = 3;

#define NORMAL_LEN   7
#define NORMAL	((1 << NORMAL_LEN) - 5)

#define SIZE_OF_MODEL	116
#define SCALE_THRESHOLD 240

#define NEXT_VERTEX(curr)	(curr != end ? curr + 1 : beg)
#define PREV_VERTEX(curr)	(curr != beg ? curr - 1 : end)

#define C3D_VERSION_1	3
#define C3D_VERSION_3	8

#define DRAW_SX 	256
#define DRAW_SY 	256

#ifdef _ROAD_
#define COMPACT_3D
#endif


enum {
	DRAW_DEFAULT,
	DRAW_LINE,
	DRAW_FLAT,
	DRAW_GOURAUD
};

struct COLORS_IDS {
	enum {
		ZERO_RESERVED,
		
		BODY,

		WINDOW,
		WHEEL,
		DEFENSE,
		WEAPON,
		TUBE,
		
		BODY_RED,
		BODY_BLUE,
		BODY_YELLOW,
		BODY_GRAY,
		YELLOW_CHARGED,
		
		MATERIAL_0,
		MATERIAL_1,
		MATERIAL_2,
		MATERIAL_3,
		MATERIAL_4,
		MATERIAL_5,
		MATERIAL_6,
		MATERIAL_7,
		
		BLACK,
		BODY_GREEN,
		
		SKYFARMER_KERNOBOO,
		SKYFARMER_PIPETKA,
		ROTTEN_ITEM,

		BODY_CRIMSON,

		MAX_COLORS_IDS
		};
	};

/* ---------------------------- EXTERN SECTION ----------------------------- */

extern int draw_mode;
extern int draw_color;

extern DBM A_g2s;
extern DBM A_s2g;
extern DBM A_g2sZ;
extern Vector Refl, View, Light;
extern DBV CurrShadowDirection;
extern double ModulatedNORMAL;
extern int ViewX,ViewY,ViewZ;
extern int focus;
extern double focus_flt;
extern int TurnAngle;
extern int sinTurn,cosTurn;
extern int sinTurnInv,cosTurnInv;
extern double sinTurnFlt,cosTurnFlt;
extern double sinTurnInvFlt,cosTurnInvFlt;
extern double Ha,Va;
extern double Hb,Vb;
extern double Oc,Hc,Vc;
extern int SlopeAngle;
extern int TurnSecX;
extern int TurnSecY;
extern int ScaleMap;
extern int ScaleMapInv;
extern double ScaleMapFlt;
extern double ScaleMapInvFlt;
extern int DepthShow;
extern int light_modulation;


extern int ScreenCX;
extern int ScreenCY;
extern int Xmax2;
extern int Ymax2;
extern int UcutLeft;
extern int UcutRight;
extern int VcutUp;
extern int VcutDown;

extern int page;
extern int counter;
extern DBM A_convert;
extern Matrix A_convert_8;
extern char convert_offset;
extern char z_global_offset;
extern unsigned char* draw_buffer;

extern int CentrX;
extern int CentrY;
extern int CentrZ;

extern int u_cut_left;
extern int u_cut_right;
extern int v_cut_up;
extern int v_cut_down;

extern int flush_x;
extern int flush_y;
extern int flush_sx;
extern int flush_sy;

extern int poly_cnt;
extern int poly_number;
extern int poly_save;

extern int poly_count;

extern int PRECISION_8;

extern int n_power;
extern int x_prec;

extern int z_buffer_log;


extern unsigned char COLORS_VALUE_TABLE[2*COLORS_IDS::MAX_COLORS_IDS];
void register_color(unsigned int id,int first,int last);

/*******************************************************************************
			Vertex -> Polygon link
	Vertex:    <─────────┐	   ┌──────────> Polygon:
	num_poly	         └─────┼─────┐	num_vert
	polygons[0]  ──────────────┘	 └────	vertices[indices_in_poly[0]]
	indices_in_poly[0]

			Polygon -> Vertex link
	Vertex:   <──────────────────────┐  ┌─> Polygon:
	num_poly		              ┌──┼──┘	num_vert
	polygons[indices_in_vert[0]] ─┘  └────	vertices[0]
			 indices_in_vert[0]
*******************************************************************************/

#define Polygon SimplePolygonType
#ifndef COMPACT_3D
#define VariablePolygon SimplePolygonType
#endif
struct Polygon;

struct Vertex {
	char x_8,y_8,z_8;
	unsigned char yscr_8;
	unsigned char xscr_8;
	unsigned char zscr_8;

#ifndef COMPACT_3D
	float x,y,z;
	Vector turned;
	int xscr,yscr;

	int flag;
	int index;
	int wheel_belonging;

	int num_poly;
	Polygon** polygons;
	char* indices_in_poly;
	Vertex* reference;


		Vertex(double vx,double vy,double vz){ x_8 = round(vx); x = (float)vx; y_8 = round(vy); y = (float)vy; z_8 = round(vz); z = (float)vz; }
		Vertex(const DBV& v){ x_8 = round(v.x); x = (float)v.x; y_8 = round(v.y); y = (float)v.y; z_8 = round(v.z); z = (float)v.z; }
		Vertex& operator = (const DBV& v){ x_8 = round(v.x); x = (float)v.x; y_8 = round(v.y); y = (float)v.y; z_8 = round(v.z); z = (float)v.z; return *this; }
		operator DBV (){ return DBV(x,y,z); }
#else
		operator Vector (){ return Vector(x_8,y_8,z_8); }
#endif

		Vertex(){}

	void convert();
	inline void fast_convert()
		{
		xscr_8 = ((unsigned int)(MUL_CHAR(A_convert_8.a[0],x_8) + MUL_CHAR(A_convert_8.a[1],y_8) +
		MUL_CHAR(A_convert_8.a[2],z_8) + convert_offset));
		yscr_8 = ((unsigned char)(MUL_CHAR(A_convert_8.a[3],x_8) + MUL_CHAR(A_convert_8.a[4],y_8) +
		MUL_CHAR(A_convert_8.a[5],z_8) + convert_offset));
		}
	void fast_convert_z();
	void convert_big();
};

struct Normal {
	char x,y,z;
	unsigned char I_8,n_power;

#ifndef COMPACT_3D
	int I;
	int flag;
	int index;
	Normal* reference;

		Normal(int xx,int yy,int zz){ x = xx; y = yy; z = zz; }
		Normal(const Vector& v){ x = v.x; y = v.y; z = v.z; }
		Normal& operator = (const Vector& v){ x = v.x; y = v.y; z = v.z; return *this; }
#endif
		operator Vector (){ return Vector(x,y,z); }
		Normal(){}

	void calc_intensity();
	inline void fast_calc_intensity()
		{
		if(n_power){ // Metal
			char nx = x;
			char ny = y;
			char nz = z;
			char Ln = (MUL_CHAR(nx,Refl.x) + MUL_CHAR(ny,Refl.y) + MUL_CHAR(nz,Refl.z));
			if(Ln < 0){
				I_8 = 0;
				return;
			}
			nx = (MUL_CHAR(nx,Ln) << 1) + Refl.x;
			ny = (MUL_CHAR(ny,Ln) << 1) + Refl.y;
			nz = (MUL_CHAR(nz,Ln) << 1) + Refl.z;
			char RV = (MUL_CHAR(nx,View.x) + MUL_CHAR(ny,View.y) + MUL_CHAR(nz,View.z));
			if(RV < 0){
				I_8 = 0;
				return;
			}
			unsigned char Intens = (power_table[((int(n_power) - 2) << 8) + RV] << 2) + (Ln >> 1);
			if(Intens > NORMAL)
				I_8 = NORMAL;
			else
				I_8 = Intens;
			}
		else{	// Diffuse
			char nx = x;
			char ny = y;
			char nz = z;
			char Ln = (MUL_CHAR(nx,Refl.x) + MUL_CHAR(ny,Refl.y) + MUL_CHAR(nz,Refl.z));
			if(Ln < 0){
				I_8 = 0;
				return;
				}
			if(Ln > NORMAL)
				I_8 = NORMAL;
			else
				I_8 = Ln;
			}
	}
};

struct VariablePolygon {
	int num_vert;
	Vertex** vertices;
	Normal** normals;
	unsigned char color_id;
	char middle_x,middle_y,middle_z;
	Normal flat_normal;

#ifndef COMPACT_3D
	int flag;
	int index;

	char* indices_in_vert;

	float z_min,z_max;
	Polygon *prev;
	Polygon *next;
#endif

	void put_in_list();
	void draw_line();
	void draw_flat();
	void draw_gouraud();
	void draw_gouraud_old();
	void draw_gouraud_decomposed_true_z();
	void draw_pixel();
	void draw_gouraud_big();
	void height_project();
	int lower_average(int xx,int yy,int zz,double& X0,double& Y0,double& dZ0,double& X1,double& Y1,double& dZ1);
	int upper_average(int xx,int yy,int zz,double& X0,double& Y0,double& dZ0,double& X1,double& Y1,double& dZ1);
	int check_terrain_type(int xx,int yy,int zz,int terrain_type);
	int check_mole_submerging(int xx,int yy,int zz);
	int lower_average_z(int x_offset,int y_offset,int z_offset,double& dZ,DBV& r,DBV& n);

	void compProjectionIntegrals();
	void compFaceIntegrals();
};

#ifdef COMPACT_3D
struct Polygon {
	Vertex* vertices[3];
	Normal* normals[3];
	unsigned char color_id;
	char middle_x,middle_y,middle_z;

	void draw_gouraud_decomposed_true_z();
	void draw_pixel();
	void draw_gouraud_big();
	void height_project();
};
#endif


struct Model {
	int num_vert;
	Vertex* vertices;
	int num_norm;
	Normal* normals;
	int num_poly;
	union {
		Polygon* polygons;
		VariablePolygon* variable_polygons;
		};
#ifndef COMPACT_3D
	union {
		Polygon** sorted_polygons[3];
		VariablePolygon** sorted_variable_polygons[3];
		};
#endif

	int xmax,ymax,zmax;
	int xmin,ymin,zmin;
	int x_off,y_off,z_off;
	int rmax;
	int memory_allocation_method;
	
	double volume;
	DBV rcm;
	DBM J;


		Model();

	void loadASC(char* name);
	void loadASCinverted(char* name);
	void loadC3D(XBuffer& in);
	void loadC3Dvariable(XBuffer& in);
	void saveC3D(XStream& out);
	Vertex* add_vertices(int num_add);
	Polygon* add_polygons(int num_add);
	void make_polygon_vertex_links();
	void triangle_to_quadrilateral();
	void calc_box(int xmin,int xmax,int ymin,int ymax,int zmin,int zmax,int delta);
	void join(Model& m);
	void free();

	void prepare(int central = 0);
	void move(Vector offset);
	void rotate(DBM R);
	void scale(double k,int dont_move = 0);
	void calc_polygons_middle();
	void set_colors(int color_id,int n_power,int wheel_belonging);
	void height_project();

	void make_normals();
	void make_normals_smoothing(int grad_threshould = 60);
	void weld(double grid_size);
	void split(double threshold);
	void morph(Model& shape,int z_low);
	DBV r_attract(DBV r0);
	void calc_bound(Model& shape);

	void draw(Vector R, const DBM& A_scl, const DBM& A_inv);
	void draw_child(const Vector& R,const DBM& A_c2p,const DBM& A_p2c);
	void pixel_draw();
	void z_buffering_draw();
	void draw_big();

	void pre_sorting();
	void sample(int i_sample);

	void VolumeIntegral(int only_upper_level = 0);
	double compVolumeIntegrals();
	};


extern Polygon* z_list_min;
extern Polygon* z_list_max;
extern Polygon* z_list_curr;
extern Polygon* max_z_poly;
extern Polygon* min_z_poly;

void graph3d_init();
void graph3d_restart();
void graph3d_flush();
void graph3d_correct_ratio(DBM& A);
void calc_screen(int zoom_percent);
void calc_border(int& x1,int& x2,int& xc,int& sx2,int& flush_x,int& flush_sx,int xmax);

void clip_pixel(int x,int y,int col);
void clip_line(int x1,int y1,int x2,int y2,int color);
void z_line(int x1,int y1,int x2,int y2,int c1,int c2);

inline void Vertex::fast_convert_z()
{
	xscr_8 = ((unsigned int)(MUL_CHAR(A_convert_8.a[0],x_8) + MUL_CHAR(A_convert_8.a[1],y_8) +
		  MUL_CHAR(A_convert_8.a[2],z_8) + convert_offset));
	yscr_8 = ((unsigned char)(MUL_CHAR(A_convert_8.a[3],x_8) + MUL_CHAR(A_convert_8.a[4],y_8) +
		  MUL_CHAR(A_convert_8.a[5],z_8) + convert_offset));
	zscr_8 = ((unsigned char)(MUL_CHAR(A_convert_8.a[6],x_8) + MUL_CHAR(A_convert_8.a[7],y_8) +
		  MUL_CHAR(A_convert_8.a[8],z_8) + z_global_offset));
}
#endif
