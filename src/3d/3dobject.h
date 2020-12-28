#ifndef __3D__3DOBJECT_H__
#define __3D__3DOBJECT_H__

#include "3d_math.h"
#include "3dgraph.h"

const char TG_NO_TOUCH = 1;
const char TG_POINT_TOUCH = 2;
const char TG_SPHERICAL_TOUCH = 3;
const char TG_BOX_TOUCH = 4;

const int ID_BULLET = 1;
const int ID_EXPLOSION = 2;
const int ID_MOBILE_LOCATION = 4;

const int ID_INSECT = 8;
const int ID_VANGER = 64;

const int ID_DEBRIS = 1 << 9;
const int ID_DEFORM = 1 << 10;
const int ID_STUFF = 1 << 12;
const int ID_JUMPBALL = 1 << 14;

const int ID_STATIC = 1 << 15;
const int ID_FIELD_SOURCE = 1 << 16;
const int ID_SKYFARMER = 1 << 17;
const int ID_FISH = 1 << 18;

const int ID_HORDE = 1 << 19;
const int ID_HORDE_SOURCE = 1 << 20;
const int ID_GLORY_PLACE = 1 << 22;

// Types of visibility
#define UNVISIBLE		0
#define VISIBLE 		1
#define NEAR_VISIBLE		2

struct StorageType;
struct DangerDataType;
struct StorageClusterType;
struct LightPoint;
struct MemoryStorageType;

struct GeneralObject
{
	int ID;
	int NetID;
	int Status;
	Vector R_curr;

	StorageType* Storage;
	StorageClusterType* Cluster;
	MemoryStorageType* ObjectHeap;

	GeneralObject* NextTypeList;
	GeneralObject* PrevTypeList;

#ifdef _ROAD_
	virtual void Init(StorageType* s);
	virtual void Init(MemoryStorageType* s);
	virtual void Init(void);

	virtual void Free(void);
	virtual void Open(void);
	virtual void Close(void);
	virtual void Quant(void);
	virtual void TimeOutQuant(void);

	virtual void Touch(GeneralObject* p);

	virtual void NetEvent(int type,int id,int creator,int time,int x,int y,int radius);
	virtual void Send(void);
	void GetUser(void);	
#else
	virtual void Touch(GeneralObject* p){}
#endif
};

struct BaseObject : GeneralObject {
	Vector R_prev;

	int x_of_last_update;
	int y_of_last_update;

	int dynamic_state;
	int Visibility;
	int MapLevel;

	int ez;
	int radius;

	BaseObject(){ dynamic_state = Visibility = MapLevel = ez = radius = 0; }

	BaseObject* NextBaseList;
	BaseObject* PrevBaseList;

	BaseObject* NextViewList;
	BaseObject* PrevViewList;

#ifdef _ROAD_
	virtual void DrawQuant(void);
	void GetVisible(void);
#endif
};

extern char WorldBorderEnable;

/*******************************************************************************
		Dynamic structs and constants
*******************************************************************************/
//   Wheel's sides and Rudder's directions
#define LEFT_SIDE	1
#define RIGHT_SIDE	2
#define FRONT_SIDE	4
#define BACK_SIDE	8

// Motor control
#define ADD_POWER	1
#define DEL_POWER	2

// Dynamic states
#define GROUND_COLLISION		1
#define TRACTION_WHEEL_TOUCH		2
#define STEER_WHEEL_TOUCH		4
#define OBJECT_TO_OBJECT_COLLISION	8
#define TOUCH_OF_WATER			16
#define TOUCH_OF_AIR			32
#define ITS_MOVING			64
#define VERTICAL_WALL_COLLISION		128
#define STRONG_GROUND_COLLISION		256

#define WHEELS_TOUCH			(TRACTION_WHEEL_TOUCH | STEER_WHEEL_TOUCH)
#define TOUCH_OF_GROUND		(GROUND_COLLISION | WHEELS_TOUCH)

// Set_3D modes
enum {
SET_3D_DIRECT_PLACE = 1,
SET_3D_CONST_Z = 2,
SET_3D_TO_THE_LOWER_LEVEL = 4,
SET_3D_TO_THE_UPPER_LEVEL = 8,
SET_3D_CHOOSE_LEVEL = 16
};

// Mechous control modes
struct CONTROLS {
	enum {
		TRACTION_INCREASE,
		TRACTION_DECREASE,
		STEER_LEFT,
		STEER_RIGHT,
		TURBO_QUANT,
		BRAKE_QUANT,
		HAND_BRAKE_QUANT,
		LEFT_SIDE_IMPULSE,
		RIGHT_SIDE_IMPULSE,
		VIRTUAL_UP,
		VIRTUAL_DOWN,

		NUMBER_OF_TRANSFERABLE_CONTROLS,

		STEER_BY_ANGLE,

		JUMP_POWER_ACCUMULATION_ON,
		JUMP_USING_ACCUMULATED_POWER,

		HELICOPTER_UP,
		HELICOPTER_DOWN,
		FLOTATION_UP,
		FLOTATION_DOWN,
		MOLE_UP,
		MOLE_DOWN
		};
};

// Slots
#define LEFT_SLOT	0
#define RIGHT_SLOT	1
#define MIDDLE_SLOT    2
#define MAX_SLOTS	3


// Spring's directions
#define SPRING_UNTOUCHED	0
#define SPRING_UP		1
#define SPRING_DOWN		2
#define SPRING_HORIZONTAL	4

// Object's draw modes
#define NORMAL_DRAW_MODE	1
#define TRANSPARENCY_DRAW_MODE	2

struct Wheel {
	int steer;
	int side;
	int width;
	int radius;
	int bound_index;
	Model model;
	DBV r,n;
	double dZ;
};

/*******************************************************************************
		3D object for building Mechouses, Raffas, Static Objects.
*******************************************************************************/
struct Object : BaseObject {

	// Part of 3D data
	int n_models;
	Model* models;
	unsigned int i_model;
	Model* model;
	Model* bound;
	int xmax,ymax,zmax,zmin;
	int rmax;
	double scale_size;
	double original_scale_size;
	int body_color_offset;
	int body_color_shift;

	int n_wheels;
	Wheel* wheels;

	int n_debris;
	Model* debris;
	Model* bound_debris;

	int slots_existence;
	Object* data_in_slots[MAX_SLOTS];
	Vector R_slots[MAX_SLOTS];
	int location_angle_of_slots[MAX_SLOTS];
	
	char prm_name[128];
	
	Object* old_appearance_storage;


	// Part of coordinates
	Vector R_scr;
	DBM A_scl;
	int psi,tetta;
	double scale;
	double scale_max;
	double scale_real;
	int xmax_real,ymax_real,zmax_real;
	int rmax_screen;
	int z_below;

	// Part of dynamic variables
	int speed;
	int in_water;
	double device_modulation;
	double terrain_immersion;
	double water_immersion;

	DBV R,V,W;
	DBM A_l2g;
	DBM A_g2l;
	Quaternion Q_l2g;
	double V_drag;
	double W_drag;
	Object* collision_object;
	int after_db_coll;

	int interpolation_on;
	DBV dR_corr;
	Quaternion dQ_corr;

	// Part of control variables
	int analysis_off;
	int disable_control;
	int active;
	int rudder;
	int traction;
	int wheel_analyze;
	int brake;
	int hand_brake;
	int nitro;
	int turbo;
	int archimedean;
	int jump_power;
	int stuff_phase;
	int side_impulse_enable;
	int mole_on;
	int k_track;
	int draw_mode;
	unsigned int device_switch_latency;
	int prev_controls;
	int current_controls;
	unsigned int last_send_time;

	int helicopter;
	int helicopter_time;
	int helicopter_strife;
	int helicopter_thrust_phase;
	int helicopter_circle_phase;

	int skyfarmer_fly_direction;
	DBV skyfarmer_direction;

	double k_elastic_modulation;
	DangerDataType* closest_field_object;

	// Part of dynamic constants

	// Base params
	double m;
	DBM J_inv;
	double scale_bound;
	double scale_box;
	int z_offset_of_mass_center;

	// Car's params
	double speed_factor;
	double mobility_factor;
	
	// Devices params
	double water_speed_factor;
	double air_speed_factor;
	double underground_speed_factor;

	// Ship's params
	double k_archimedean;
	double k_water_traction;
	double k_water_rudder;

	// Grader's params
	double TerraMoverSx;
	double TerraMoverSy;
	double TerraMoverSz;

	// Defense
	int FrontDefense;
	int BackDefense;
	int SideDefense;
	int UpperDefense;
	int LowerDefense;

	// Ram Power
	int FrontRamPower;
	int BackRamPower;
	int SideRamPower;
	int UpperRamPower;
	int LowerRamPower;

	int max_jump_distance;

	int end_of_object_data;


	Object();
	Object& operator = (Object& obj);
	virtual ~Object() {
		// stalkerg: memory leak
		// free();
	}
	void free();

	// Part of 3D graphical functions
	void load(char* name,int scale = 256);
	void save(char* name);

	void loadLST(char* name_lst,char* name_asc = 0);
	void loadM3D(char* name);
	void saveM3D(char* name);

	void loadLSA(char* name);
	void loadA3D(char* name);
	void saveA3D(char* name);

	void draw();
	void pixel_draw();
	void non_sorted_draw();

	void set_body_color(unsigned int color_id);
	void lay_to_slot(int slot,Object* weapon);

	// Part of dynamic function
	void dynamics_init(char* prm_name);
	void calc_springs(int NumParts,int Threshold);
	void calc_bound();
	void load_parameters(const char* name);
	void save_parameters(char* name);
	void global_save_parameter(int i_entry);
	void entries_control();

	void set_active(int on);
	void controls(int mode,int param = 0);
	void set_3D(int mode,int x,int y,int z,int dz,int angle,int speed);
	void impulse(int angle,int distance,int slope = Pi/4,int lever_arm = 0);
	void impulse(const DBV& direct,int distance,int lever_arm = 0);
	void jump();
	void direct_keyboard_control();
	void direct_joystick_control();
	void import_controls();
	void analysis();
	void switch_analysis(int off);
	void mechous_analysis(double dt);
	void basic_mechous_analysis(double dt,int last);
	void debris_analysis(double dt);
	void basic_debris_analysis(double dt);
	void insect_analysis();
	void skyfarmer_analysis(double dt);
	void fish_analysis(double dt);
	void update_coord(int camera_correction = 0);
	void convert_to_beeb(Object* beeb = 0); // Zero means converting back
	void set_draw_mode(int mode){ draw_mode = mode; }
	void set_ground_elastic(double k);  // [0..1], properly works for stuffs, debris (non vangers).

	void steer(int dir);
	void motor_control(int dir);
	void impulse(int side);
	void brake_on();

#ifndef _SURMAP_
	virtual int test_objects_collision();
#ifdef _ROAD_
	virtual void DrawMechosParticle(int x,int y,int speed,int level,int n);
	virtual void DestroyCollision(int l,Object* p);
#endif
#else
	virtual int test_objects_collision(){ return 0; }
#endif
	int test_object_to_object(Object* obj);
	int test_object_to_baseobject(BaseObject* obj);
	int test_box_to_box(Object* obj);
	int test_wheels_to_sphere(Object* sphere);

	void DrawQuant(){ draw(); }

	void Explosion(int m_TNT,int m_id);
	void destroy_double_level();
	void ground_pressing();
	int test_whether_there_is_not_something_above();
	int check_terrain_type(int terrain_type);
	int check_mole_submerging();
	int get_max_jump_distance();

	void NetEvent(int type,int id,int creator,int time,int x,int y,int radius);
	void Send(void);

	void skyfarmer_start(int x,int y,int angle = 0);
	void skyfarmer_set_direction(const DBV& direction);
	void skyfarmer_end();
	void precise_impulse(Vector source_point,int x_dest,int y_dest);

	virtual void StartMoleProcess(void);
	int MoleInProcess;

	virtual int UsingCopterig(int decr_8);
	virtual int UsingCrotrig(int decr_8);
	virtual int UsingCutterig(int decr_8);
	};


extern double density;

// Impulses magnitude
extern int impulse_of_tuns_explosion;
extern int impulse_of_jumpballs_creation;
extern int impulse_of_stuffs_throwing;

// Stuff visualisation params
extern double stuff_modulation;
extern int stuff_delta_phase;

// Speed correction params
extern int STANDART_FRAME_RATE;
extern double speed_correction_factor;
extern double speed_correction_tau;

#endif