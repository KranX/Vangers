#include "../global.h"

#include "general.h"

#include "../xjoystick.h"

#include "../common.h"

#include "../network.h"

#include "../sound/hsound.h"

#include "../terra/vmap.h"
#include "../terra/world.h"
#include "../terra/render.h"

#include "../actint/item_api.h"
#include "../units/uvsapi.h"

#include "../uvs/univang.h"

#include "../units/hobj.h"
struct ParticleProcess;
#include "../units/moveland.h"
#include "../units/track.h"
#include "../units/items.h"
#include "../units/sensor.h"
#include "../dast/poly3d.h"

#include "../iscreen/controls.h"

#undef random
#define random(num) ((int)(((long)_rand()*(num)) >> 15))

#ifdef SICHER_DEBUG
#define NO_BORDER_FIELD
#define UsingCopterig(t)	1
#define UsingCrotrig(t)		1
#define UsingCutterig(t)	1
#endif

#ifdef _DEBUG
#define ENTRIES_CONTROL
#define MSG_OUT
#endif

#define SPHERICAL_TEST (ID_INSECT | ID_JUMPBALL | ID_BULLET | ID_STATIC | ID_FIELD_SOURCE | ID_HORDE)
#define BOX_TEST (ID_VANGER | ID_DEBRIS | ID_HORDE_SOURCE  | ID_STUFF)

#define PRM_VERSION	2
#define DESIRED_FPS	15
#define DESIRED_TICKS	(256/DESIRED_FPS)


// terrain_analysis_flags
#define BREAKE_DBLEVEL_ENABLE	1
#define BREAKE_DBLEVEL 			   2
#define UNBREAKABLE_TERRAIN	    4
#define MIDDLE_LEVEL_PREFER 	       8


#define LOW_LEVEL(p)	((unsigned char*)((uintptr_t)p & ~1))
#define HIGH_LEVEL(p)	((unsigned char*)((uintptr_t)p | 1))
#define GET_THICKNESS(p) ((GET_DELTA(*HIGH_LEVEL(p + H_SIZE)) + (GET_DELTA(*LOW_LEVEL(p + H_SIZE)) << 2) + 0) << DELTA_SHIFT)
#define GET_THICKNESS_ATTR(la,ha) (GET_DELTA(ha) + (GET_DELTA(la) << 2) + 1 << DELTA_SHIFT)
#define BREAKABLE_TERRAIN(prop)		(GET_DESTROY_TERRAIN(GET_TERRAIN(prop)) > 10)
#define GET_MIDDLE_HIGHT(lh,uh)		(uh - lh > 130 ? lh + 110 : lh + 48)


#define HEIGHTS(x,y)	get_three_heights(x,y)
#define THREE_HEIGHTS(z3)	((z3) >> 24)
#define LOWER_HEIGHT(z3)	 ((z3) & 0x000000ff)
#define MIDDLE_HEIGHT(z3)	  (((z3) >> 8) & 0x000000ff)
#define UPPER_HEIGHT(z3)	 (((z3) >> 16) & 0x000000ff)

#define DUST_BY_BODY(x,y,speed,level) DrawMechosBody(x,y,speed,level)
#define WHEEL_TRACK(x_curr,y_curr,speed,level,n) DrawMechosParticle(x_curr,y_curr,speed,level,n)

#define GET_DEVICE_LATENCY() (SDL_GetTicks() + 4000)

#ifdef MSG_OUT
#define RESTRICT(v,a)	{ if(v.vabs() > a){ if(active) msg_buf < "Restriction of " #v "\n"; v.norm(a); } else if(active) msg_buf < "\n"; }
#else
#define RESTRICT(v,a)	{ if(v.vabs() > a){ v.norm(a); } }
#endif

#define aci_CONTROLS(key)
void FIRE_ALL_WEAPONS();
extern int aciCurJumpImpulse,aciMaxJumpImpulse;
extern int aciKeyboardLocked;
extern int aciAutoRun;


int obb_disjoint(const DBM&  B, const DBV& T, double ax, double ay, double az, double bx, double by, double bz);
DBV box_to_box_intersection(DBV RR,DBV X,DBV Y,DBV Z,double sx,double sy,double sz);
DBV interpolation(double t,double x[3],DBV y[3]);
DBV interpolation_factors(double t,double x[3]);


/*******************************************************************************
			Global Parameters
*******************************************************************************/

// Base properties of nature params
double g;
double density;
double dt_impulse = 1;
double dt0;
double scale_general = 1.0;
int num_calls_analysis;
int movement_detection_threshould;
int num_skip_updates;

// Impulse upproach params
double elastic_restriction;
double elastic_time_scale_factor;
double rolling_scale;
double normal_threshould;
double k_wheel;
double horizontal_impulse_factor;
double vertical_impulse_factor;
double k_friction_impulse;

// Car's params
int rudder_step;
int rudder_max;
double rudder_k_decr;
int traction_increment;
int traction_decrement;

double global_speed_factor;
double global_mobility_factor;
double global_water_speed_factor;
double global_air_speed_factor;
double global_underground_speed_factor;
double k_traction_turbo;
double f_brake_max;

// Helicopter's params
int max_helicopter_height;
int helicopter_height_incr;
int helicopter_height_decr;
double k_helicopter_thrust;
double k_helicopter_rotate;
double k_helicopter_strife;
int max_helicopter_time;

double heli_x_convert;
double heli_y_convert;
double heli_rudder_decr;
double heli_traction_decr;
double heli_z_offset;
double helicopter_ampl;
int helicopter_dphi;
double helicopter_circle_radius_x;
double helicopter_circle_radius_y;
int helicopter_circle_dphi;

// Drag and restriction params
double V_drag_speed;
double W_drag_speed;
double V_drag_wheel_speed;
double V_drag_z;
double V_drag_free;
double W_drag_free;
double V_drag_wheel;
double W_drag_wheel;
double V_drag_spring;
double W_drag_spring;
double V_drag_coll;
double W_drag_coll;
double V_drag_helicopter;
double W_drag_helicopter;
double V_drag_float;
double W_drag_float;
double V_drag_friction;
double W_drag_friction;
double V_abs_stop;
double W_abs_stop;
double V_drag_stuff;
double V_drag_swamp;
double V_drag_mole;

double V_abs_min;
double W_abs_min;

// Terra Analysis params
double dZ_max;
int MIN_WALL_DELTA;

// Mole params
double k_elastic_mole;
double K_mole;
double k_mole_rudder;
double mole_emerging_fz;
double mole_submerging_fz;

// Contact properties params
double k_elastic_wheel;
double k_elastic_spring;
double k_elastic_xy;
double k_elastic_db_coll;
double k_destroy_level;
double strong_ground_collision_threshould;
double strong_double_collision_threshould;

double k_friction_wheel_x;
double k_friction_wheel_x_back;
double k_friction_wheel_y;
double k_friction_wheel_z;
double k_friction_spring;

// Some common force params
double f_spring_impulse;
double K_spring_impulse;
double f_traction_impulse;
double k_distance_to_force;
double V_explosion;
double W_explosion;
int max_jump_power;
int side_impulse_delay;
int side_impulse_duration;

// Insect's params
double k_elastic_insect;
double k_traction_insect;
double V_drag_insect;
double W_drag_insect;
double K_insect;

// Debris' params
double dt_debris;
int num_calls_analysis_debris;
double f_archimedean_debris;
double k_elastic_spring_debris;
double k_friction_spring_debris;
double V_drag_spring_debris;
double W_drag_spring_debris;

// Jumpball params
double k_elastic_spring_jumpball;
double V_drag_spring_jumpball;

// Stuff params
double stuff_modulation;
int stuff_delta_phase;
int stuff_z_offset;

// Visualization adjust params used in optimize.cpp
extern int dz_between_water_and_mask_levels;
extern int light_modulation;
extern int dz_shadow;
extern int SHADOWDEEP_3D;
extern int ground_pressing_z_offset;

// Camera impulse
extern double camera_mi;
extern double camera_miz;
extern double camera_mit;
extern double camera_mii;
extern double camera_mis;

extern double camera_drag;
extern double camera_dragz;
extern double camera_dragt;
extern double camera_dragi;
extern double camera_drags;

extern double camera_vz_min;
extern double camera_vt_min;
extern double camera_vs_min;

extern int camera_turn_impulse;

extern int max_time_vibration;
extern double A_vibration;
extern double alpha_vibration;
extern double oscillar_vibration;


// Test fields params
double field_max_distance;
double field_rotator;
double field_rotator_moment;
double field_tractor_xy;
double field_tractor_z;
double train_field_rotator;
double train_field_rotator_moment;
double train_field_tractor_xy;
double train_field_tractor_z;
int modulation_power;
int y_border_field;
double k_border_field;

// Impulses magnitude
int impulse_of_tuns_explosion;
int impulse_of_jumpballs_creation;
int impulse_of_stuffs_throwing;

// SkyFarmer
double skyfarmer_scale_incr;
double skyfarmer_scale_decr;
double skyfarmer_z;
double skyfarmer_Fy;
double skyfarmer_kFz;
double skyfarmer_kKy;
double skyfarmer_kKz;
double V_drag_skyfarmer;
double W_drag_skyfarmer;

// Fish
double k_archimedean_fish;
double k_traction_fish;
double k_elastic_fish;
double k_elastic_xy_fish;
double k_rudder_fish;

// Interpolaton
double correction_tau;
double correction_V0;
int number_of_passed_events;

// Speed correction params
int STANDART_FRAME_RATE = 14;
double speed_correction_factor = 1;
double speed_correction_tau = 0.01;


unsigned int last_keyboard_touch;
int terrain_analysis_flag;
int non_loaded_space;

DBM A_g2l_old;
Vector R_old;

dastPoly3D terra_moving_tool(Vector(0,0,0),Vector(0,0,0),Vector(0,0,0));

extern dastPoly3D MolePoint1;
extern int MoleInProcess;
void StartMoleProcess(void);

int stop_flag = 0;
int fish_test = 0;
int skyfarmer_test = 0;

int g_x_sum = 0,g_y_sum = 0,g_z_sum = 0,g_dz_sum = 0,g_cnt = 0;
int gN=0;
int gSx=0,gSx2=0;
int gSy=0,gSy2=0;
int gSz=0;
int gSxz=0,gSyz=0;

int check_dynamics_locals()
{
	if(stop_flag | fish_test | skyfarmer_test)
		return 1;
	if(rudder_step != 40 ||
		rudder_max != 512 ||
		traction_increment != 44 ||
		traction_decrement != 12)
			return 1;
	return 0;
}


/*******************************************************************************
			Main init function
*******************************************************************************/
DBM calc_collision_matrix(const DBV& r,const DBM& J_inv)
{
	double t3 = -r.z*J_inv(2,2)+r.y*J_inv(3,2);
	double t7 = -r.z*J_inv(2,3)+r.y*J_inv(3,3);
	double t12 = -r.z*J_inv(2,1)+r.y*J_inv(3,1);
	double t21 = r.z*J_inv(1,2)-r.x*J_inv(3,2);
	double t25 = r.z*J_inv(1,3)-r.x*J_inv(3,3);
	double t30 = r.z*J_inv(1,1)-r.x*J_inv(3,1);
	double t39 = -r.y*J_inv(1,2)+r.x*J_inv(2,2);
	double t43 = -r.y*J_inv(1,3)+r.x*J_inv(2,3);
	double t48 = -r.y*J_inv(1,1)+r.x*J_inv(2,1);
	return DBM(
		1 + -t3*r.z+t7*r.y,  t12*r.z-t7*r.x, -t12*r.y+t3*r.x,
		-t21*r.z+t25*r.y, 1 + t30*r.z-t25*r.x, -t30*r.y+t21*r.x,
		-t39*r.z+t43*r.y, t48*r.z-t43*r.x, 1 + -t48*r.y+t39*r.x);
} 

/*******************************************************************************
			Entries control functions
*******************************************************************************/
int entry_scan_code;

#ifndef ENTRIES_CONTROL

#define ENTRY(val)	{ f.search_name(#val":"); f >= val; }
#define COMMON_ENTRY(val) { fc.search_name(#val":"); fc >= val; }
#define entries_control()

#else											    

#define ENTRY(val)	{ int t = f.is_next_name("//"); f.search_name(#val":"); f >= val; if(active) add_entry(val,#val,t); }
#define COMMON_ENTRY(val)      { int t = fc.is_next_name("//"); fc.search_name(#val":"); fc >= val; if(active) add_entry(val,#val,t); }

const int  MAX_ENTRIES = 256;
const int  INT_ENTRY = 1;
const int  DOUBLE_ENTRY = 2;
const int  FIRST_IN_GROUP = 4;
int num_entries = 0;
int first_common_entry = 0;
int curr_entry = 0;
int max_indices;
int num_indices;
int indices_entry[MAX_ENTRIES];
char* name_entries[MAX_ENTRIES];
void* pval_entries[MAX_ENTRIES];
char type_entries[MAX_ENTRIES];
struct entry_init {
       entry_init();
       ~entry_init();
} useless;
entry_init::entry_init()
{
	int i;
	XStream fin(0);
	if(fin.open("entry.cfg",XS_IN)){
		fin >= max_indices >= num_indices >= curr_entry;
		if(max_indices == MAX_ENTRIES){
			for(i = 0; i < max_indices;i++)
				fin >= indices_entry[i];
			return;
			}
		}

	max_indices = MAX_ENTRIES;
	num_indices = 6;
	curr_entry = 0;
	for(i = 0; i < max_indices;i++)
		indices_entry[i] = 0;
}
entry_init::~entry_init()
{
	XStream fout("entry.cfg",XS_OUT);
	fout <= max_indices < "\n" <= num_indices < "\n" <= curr_entry < "\n";
	for(int i = 0; i < max_indices;i++)
		fout <= indices_entry[i] < "\n";
}
void add_entry(int& val,char* name,int first_in_group)
{
	pval_entries[num_entries] = &val;
	type_entries[num_entries] = INT_ENTRY | (first_in_group ? FIRST_IN_GROUP : 0);
	name_entries[num_entries++] = name;
}
void add_entry(double& val,char* name,int first_in_group)
{
	pval_entries[num_entries] = &val;
	type_entries[num_entries] = DOUBLE_ENTRY | (first_in_group ? FIRST_IN_GROUP : 0);
	name_entries[num_entries++] = name;
}
void Object::entries_control()
{
	#ifdef MSG_OUT
	double val = 0;
	switch(ID){
		case ID_VANGER:
			msg_buf < "ID_VANGER: ";
			break;
		case ID_INSECT:
			msg_buf < "ID_INSECT: ";
			break;
		case ID_STUFF:
		case ID_DEBRIS:
			msg_buf < "ID_STUFF: ";
			break;
		default:
			msg_buf < "ID_UNKNOWN: ";
		}
	msg_buf < prm_name < "\n";
	for(int j,i = 0;i < num_indices;i++){
		j = indices_entry[i];
		if(j >= num_entries)
			j = indices_entry[i] = 0;
		if(type_entries[j] & INT_ENTRY)
			val = *(int*)pval_entries[j];
		else
			val = *(double*)pval_entries[j];
		msg_buf < (i == curr_entry ? "> ENTRY: ":"  ENTRY: ")
			< name_entries[j] < " = " <= val < "\n";
		}
	#endif

	switch(entry_scan_code){
		case VK_NEXT:
			if(XKey.Pressed(VK_CONTROL)){
				do{
					if(++indices_entry[curr_entry] >= num_entries)
						indices_entry[curr_entry] = 0;
					} while(!(type_entries[indices_entry[curr_entry]] & FIRST_IN_GROUP));
				break;
				}
			if(XKey.Pressed(VK_SHIFT)){
				if(++curr_entry >= num_indices)
					curr_entry = 0;
				break;
				}
			if(++indices_entry[curr_entry] >= num_entries)
				indices_entry[curr_entry] = 0;
			break;
		case VK_PRIOR:
			if(XKey.Pressed(VK_CONTROL)){
				do{
					if(--indices_entry[curr_entry] < 0)
						indices_entry[curr_entry] = num_entries - 1;
					} while(!(type_entries[indices_entry[curr_entry]] & FIRST_IN_GROUP));
				break;
				}
			if(XKey.Pressed(VK_SHIFT)){
				if(--curr_entry < 0)
					curr_entry = num_indices - 1;
				break;
				}
			if(--indices_entry[curr_entry] < 0)
				indices_entry[curr_entry] = num_entries - 1;
			break;
		case VK_ADD:
			if(XKey.Pressed(VK_CONTROL)){
				if(++num_indices >= max_indices)
					num_indices = max_indices;
				break;
				}
			if(type_entries[indices_entry[curr_entry]] & INT_ENTRY)
				if(XKey.Pressed(VK_SHIFT))
					(*(int*)pval_entries[indices_entry[curr_entry]])++;
				else
					(*(int*)pval_entries[indices_entry[curr_entry]]) += abs((*(int*)pval_entries[indices_entry[curr_entry]]))/16 + 1;
			else
				if(XKey.Pressed(VK_SHIFT))
					*(double*)pval_entries[indices_entry[curr_entry]] *= 1.0002;
				else
					*(double*)pval_entries[indices_entry[curr_entry]] *= 1.05;
			break;
		case VK_SUBTRACT:
			if(XKey.Pressed(VK_CONTROL)){
				if(--num_indices < 0)
					num_indices = 0;
				break;
				}
			if(type_entries[indices_entry[curr_entry]] & INT_ENTRY)
				if(XKey.Pressed(VK_SHIFT))
					(*(int*)pval_entries[indices_entry[curr_entry]])--;
				else
					(*(int*)pval_entries[indices_entry[curr_entry]]) -= abs((*(int*)pval_entries[indices_entry[curr_entry]]))/16 + 1;
			else
				if(XKey.Pressed(VK_SHIFT))
					*(double*)pval_entries[indices_entry[curr_entry]] /= 1.0002;
				else
					*(double*)pval_entries[indices_entry[curr_entry]] /= 1.05;
			break;
		case 'S':
			if(XKey.Pressed(VK_CONTROL))
				global_save_parameter(indices_entry[curr_entry]);
			else{
				save_parameters(prm_name);
				save_parameters("common.prm");
				}
			break;
		}
	entry_scan_code = 0;

	m = density*model -> volume*pow(scale_size,2);
	J_inv = (model -> J*(pow(scale_size,2)/model -> volume)).inverse();
}
void Object::save_parameters(char* name)
{
	int i;
	int i_beg,i_end;
	if(!strcmp(name,"common.prm")){
		i_beg = first_common_entry;
		i_end = num_entries;
		}
	else{
		i_beg = 0;
		i_end = first_common_entry;
		}
	Parser f(name);
	XStream out(name,XS_OUT);
	out.SetDigits(6);
	char* str;

	for(i = i_beg;i < i_end;i++){
		str = f.GetBuf() + f.tell();
		f.search_name(name_entries[i]);
		*(f.GetBuf() + f.tell()) = 0;
		++f;
		out < str < ":" < "\t\t";
		if(type_entries[i] & INT_ENTRY){
			f.get_int();
			out <= *(int*)pval_entries[i];
			}
		else{
			f.get_double();
			out <= *(double*)pval_entries[i];
			}
		}
	out < f.GetBuf() + f.tell();
}

void Object::global_save_parameter(int i_entry)
{
	if(i_entry >= first_common_entry)
		return;
	char str_buf[80];
	char* log = win32_findfirst("resource/m3d/mechous/*.prm");
	int main_prm = 1;
	while(log || main_prm){
		if(!log && main_prm){
			strcpy(str_buf,"resource/m3d/mechous/default.prm");
			main_prm = 0;
			}
		else{
			strcpy(str_buf,"resource/m3d/mechous/");
			strcat(str_buf,log);
			}
		Parser f(str_buf);
		XStream out(str_buf,XS_OUT);
		out.SetDigits(6);
		char* str = f.GetBuf() + f.tell();
		do
			f.search_name(name_entries[i_entry]);
			while(*(f.GetBuf() + f.tell()) != ':');
		*(f.GetBuf() + f.tell()) = 0;
		++f;
		out < str < ":" < "\t\t";
		if(type_entries[i_entry] & INT_ENTRY){
			f.get_int();
			out <= *(int*)pval_entries[i_entry];
			}
		else{
			f.get_double();
			out <= *(double*)pval_entries[i_entry];
			}
		out < f.GetBuf() + f.tell();
		if(log)
			log = win32_findnext();
		}
}
#endif

void Object::load_parameters(const char* name)
{
	#ifdef ENTRIES_CONTROL
	if(active)
		num_entries = 0;
	#endif

	Parser f(name);

	// Base params
	double old_scale = scale_size;
	ENTRY(scale_size);
	if(!strcmp(name,"resource/m3d/mechous/default.prm"))
		scale_size = old_scale;
	original_scale_size = scale_size;
	ENTRY(scale_bound);
	ENTRY(scale_box);
	ENTRY(z_offset_of_mass_center);

	// Car's params
	ENTRY(speed_factor);
	ENTRY(mobility_factor);
	
	// Devices params
	ENTRY(water_speed_factor); 
	ENTRY(air_speed_factor);
	ENTRY(underground_speed_factor);

	// Ship's params
	ENTRY(k_archimedean);
	ENTRY(k_water_traction);
	ENTRY(k_water_rudder);

	// Grader's params
	ENTRY(TerraMoverSx);
	ENTRY(TerraMoverSy);
	ENTRY(TerraMoverSz);

	// Defense
	ENTRY(FrontDefense);
	ENTRY(BackDefense);
	ENTRY(SideDefense);
	ENTRY(UpperDefense);
	ENTRY(LowerDefense);

	// Ram Power
	ENTRY(FrontRamPower);
	ENTRY(BackRamPower);
	ENTRY(SideRamPower);
	ENTRY(UpperRamPower);
	ENTRY(LowerRamPower);


#ifdef ENTRIES_CONTROL
	first_common_entry = num_entries;
#endif

	Parser fc("common.prm");
	static int COMMON;
	COMMON_ENTRY(COMMON);

	// Base properties of nature params
	COMMON_ENTRY(g);
	COMMON_ENTRY(density);
	COMMON_ENTRY(dt0);
//	COMMON_ENTRY(scale_general);

	COMMON_ENTRY(num_calls_analysis);
	COMMON_ENTRY(movement_detection_threshould);
	COMMON_ENTRY(num_skip_updates);
	COMMON_ENTRY(wheel_analyze);
	COMMON_ENTRY(analysis_off);
	

	// Impulse upproach params
	COMMON_ENTRY(elastic_restriction);
	COMMON_ENTRY(elastic_time_scale_factor);
	COMMON_ENTRY(rolling_scale);
	COMMON_ENTRY(normal_threshould);
	COMMON_ENTRY(k_wheel);
	COMMON_ENTRY(horizontal_impulse_factor);
	COMMON_ENTRY(vertical_impulse_factor);
	COMMON_ENTRY(k_friction_impulse);

	// Car's params
	COMMON_ENTRY(rudder_step);
	COMMON_ENTRY(rudder_max);
	COMMON_ENTRY(rudder_k_decr);
	COMMON_ENTRY(traction_increment);
	COMMON_ENTRY(traction_decrement);

	// Global modulating params
	COMMON_ENTRY(global_speed_factor);
	COMMON_ENTRY(global_mobility_factor);
	COMMON_ENTRY(global_water_speed_factor);
	COMMON_ENTRY(global_air_speed_factor);
	COMMON_ENTRY(global_underground_speed_factor);
	COMMON_ENTRY(k_traction_turbo);
	COMMON_ENTRY(f_brake_max);

	// Helicopter's params
	COMMON_ENTRY(max_helicopter_height);
	COMMON_ENTRY(helicopter_height_incr);
	COMMON_ENTRY(helicopter_height_decr);
	COMMON_ENTRY(k_helicopter_thrust);
	COMMON_ENTRY(k_helicopter_rotate);
	COMMON_ENTRY(k_helicopter_strife);
	COMMON_ENTRY(max_helicopter_time);

	COMMON_ENTRY(heli_x_convert);
	COMMON_ENTRY(heli_y_convert);
	COMMON_ENTRY(heli_rudder_decr);
	COMMON_ENTRY(heli_traction_decr);
	COMMON_ENTRY(heli_z_offset);
	COMMON_ENTRY(helicopter_ampl);
	COMMON_ENTRY(helicopter_dphi);
	COMMON_ENTRY(helicopter_circle_radius_x);
	COMMON_ENTRY(helicopter_circle_radius_y);
	COMMON_ENTRY(helicopter_circle_dphi);


	// Drag and restriction params
	COMMON_ENTRY(V_drag_speed);
	COMMON_ENTRY(W_drag_speed);
	COMMON_ENTRY(V_drag_wheel_speed);
	COMMON_ENTRY(V_drag_z);
	COMMON_ENTRY(V_drag_free);
	COMMON_ENTRY(W_drag_free);
	COMMON_ENTRY(V_drag_wheel);
	COMMON_ENTRY(W_drag_wheel);
	COMMON_ENTRY(V_drag_spring);
	COMMON_ENTRY(W_drag_spring);
	COMMON_ENTRY(V_drag_coll);
	COMMON_ENTRY(W_drag_coll);
	COMMON_ENTRY(V_drag_helicopter);
	COMMON_ENTRY(W_drag_helicopter);
	COMMON_ENTRY(V_drag_float);
	COMMON_ENTRY(W_drag_float);
	COMMON_ENTRY(V_drag_friction);
	COMMON_ENTRY(W_drag_friction);
	COMMON_ENTRY(V_abs_stop);
	COMMON_ENTRY(W_abs_stop);
	COMMON_ENTRY(V_drag_stuff);
	COMMON_ENTRY(V_drag_swamp);
	COMMON_ENTRY(V_drag_mole);

	COMMON_ENTRY(V_abs_min);
	COMMON_ENTRY(W_abs_min);

	// Terra Analysis params
	COMMON_ENTRY(dZ_max);
	COMMON_ENTRY(MIN_WALL_DELTA);

	// Mole params
	COMMON_ENTRY(k_elastic_mole);
	COMMON_ENTRY(K_mole);
	COMMON_ENTRY(k_mole_rudder);
	COMMON_ENTRY(mole_emerging_fz);
	COMMON_ENTRY(mole_submerging_fz);


	// Contact properties params
	COMMON_ENTRY(k_elastic_wheel);
	COMMON_ENTRY(k_elastic_spring);
	COMMON_ENTRY(k_elastic_xy);
	COMMON_ENTRY(k_elastic_db_coll);
	COMMON_ENTRY(k_destroy_level);
	COMMON_ENTRY(strong_ground_collision_threshould);
	COMMON_ENTRY(strong_double_collision_threshould);

	//
	COMMON_ENTRY(k_friction_wheel_x);
	COMMON_ENTRY(k_friction_wheel_x_back);
	COMMON_ENTRY(k_friction_wheel_y);
	COMMON_ENTRY(k_friction_wheel_z);
	COMMON_ENTRY(k_friction_spring);

	// Some common force params
	COMMON_ENTRY(f_spring_impulse);
	COMMON_ENTRY(K_spring_impulse);
	COMMON_ENTRY(f_traction_impulse);
	COMMON_ENTRY(k_distance_to_force);
	COMMON_ENTRY(V_explosion);
	COMMON_ENTRY(W_explosion);
	COMMON_ENTRY(max_jump_power);
	COMMON_ENTRY(side_impulse_delay);
	COMMON_ENTRY(side_impulse_duration);

	// Insect's params
	COMMON_ENTRY(k_elastic_insect);
	COMMON_ENTRY(k_traction_insect);
	COMMON_ENTRY(V_drag_insect);
	COMMON_ENTRY(W_drag_insect);
	COMMON_ENTRY(K_insect);

	// Debris' params
	COMMON_ENTRY(dt_debris);
	COMMON_ENTRY(num_calls_analysis_debris);
	COMMON_ENTRY(f_archimedean_debris);
	COMMON_ENTRY(k_elastic_spring_debris);
	COMMON_ENTRY(k_friction_spring_debris);
	COMMON_ENTRY(V_drag_spring_debris);
	COMMON_ENTRY(W_drag_spring_debris);

	// Jumpball params
	COMMON_ENTRY(k_elastic_spring_jumpball);
	COMMON_ENTRY(V_drag_spring_jumpball);

	// Stuff params
	COMMON_ENTRY(stuff_modulation);
	COMMON_ENTRY(stuff_delta_phase);
	COMMON_ENTRY(stuff_z_offset);

	// Visualization adjust params
	COMMON_ENTRY(dz_between_water_and_mask_levels);
	COMMON_ENTRY(dz_shadow);
	COMMON_ENTRY(SHADOWDEEP_3D);
	COMMON_ENTRY(ground_pressing_z_offset);

	// Camera impulse
	COMMON_ENTRY(camera_mi);
	COMMON_ENTRY(camera_miz);
	COMMON_ENTRY(camera_mit);
	COMMON_ENTRY(camera_mii);
	COMMON_ENTRY(camera_mis);

	COMMON_ENTRY(camera_drag);
	COMMON_ENTRY(camera_dragz);
	COMMON_ENTRY(camera_dragt);
	COMMON_ENTRY(camera_dragi);
	COMMON_ENTRY(camera_drags);

	COMMON_ENTRY(camera_vz_min);
	COMMON_ENTRY(camera_vt_min);
	COMMON_ENTRY(camera_vs_min);

	COMMON_ENTRY(camera_turn_impulse);

	COMMON_ENTRY(max_time_vibration);
	COMMON_ENTRY(A_vibration);
	COMMON_ENTRY(alpha_vibration);
	COMMON_ENTRY(oscillar_vibration);


	// Test fields params
	COMMON_ENTRY(field_max_distance);
	COMMON_ENTRY(field_rotator);
	COMMON_ENTRY(field_rotator_moment);
	COMMON_ENTRY(field_tractor_xy);
	COMMON_ENTRY(field_tractor_z);
	COMMON_ENTRY(train_field_rotator);
	COMMON_ENTRY(train_field_rotator_moment);
	COMMON_ENTRY(train_field_tractor_xy);
	COMMON_ENTRY(train_field_tractor_z);
	COMMON_ENTRY(modulation_power);
	COMMON_ENTRY(y_border_field);
	COMMON_ENTRY(k_border_field);

	// Impulses magnitude
	COMMON_ENTRY(impulse_of_tuns_explosion);
	COMMON_ENTRY(impulse_of_jumpballs_creation);
	COMMON_ENTRY(impulse_of_stuffs_throwing);

	// SkyFarmer
	COMMON_ENTRY(skyfarmer_scale_incr);
	COMMON_ENTRY(skyfarmer_scale_decr);
	COMMON_ENTRY(skyfarmer_z);
	COMMON_ENTRY(skyfarmer_Fy);
	COMMON_ENTRY(skyfarmer_kFz);
	COMMON_ENTRY(skyfarmer_kKy);
	COMMON_ENTRY(skyfarmer_kKz);
	COMMON_ENTRY(V_drag_skyfarmer);
	COMMON_ENTRY(W_drag_skyfarmer);

	// Fish
	COMMON_ENTRY(k_archimedean_fish);
	COMMON_ENTRY(k_traction_fish);
	COMMON_ENTRY(k_elastic_fish);
	COMMON_ENTRY(k_elastic_xy_fish);
	COMMON_ENTRY(k_rudder_fish);

	// Interpolaton
	COMMON_ENTRY(correction_tau);
	COMMON_ENTRY(correction_V0);
	COMMON_ENTRY(number_of_passed_events);

	// Speed correction params
	COMMON_ENTRY(STANDART_FRAME_RATE);
//	COMMON_ENTRY(speed_correction_factor);
	COMMON_ENTRY(speed_correction_tau);


	m = density*model -> volume*pow(scale_size,2);
	J_inv = (model -> J*(pow(scale_size,2)/model -> volume)).inverse();
	max_jump_distance = get_max_jump_distance();
}

/*******************************************************************************
			Terrain analysis functions
*******************************************************************************/
inline int get_three_heights(int x,int y)
{
	unsigned char* p = vMap -> lineT[y & clip_mask_y];
	if(!p)
		return 255;
	p += x & clip_mask_x;
	unsigned char attr = *(p + H_SIZE);
	if(!(attr & DOUBLE_LEVEL))
		return *p;
	//64 bit problem
	uintptr_t ll = *LOW_LEVEL(p);
	return ll | (ll + (GET_THICKNESS(p) << 8)) | ((uintptr_t)(*HIGH_LEVEL(p)) << 16) | 0xff000000;
}
inline int get_upper_height(int x,int y)
{
	unsigned char* p = vMap -> lineT[y & clip_mask_y];
	if(!p)
		return 255;
	p += (x | 1) & clip_mask_x;
	return *p;
}
inline int check_double_level(int x,int y)
{
	unsigned char* p = vMap -> lineT[y & clip_mask_y];
	if(!p)
		return 0;
	return p[(x & clip_mask_x) + H_SIZE] & DOUBLE_LEVEL ? 1 : 0;
}
int trace_shadow(int x,int y,int z)
{
	int x0 = x;
	z <<= 8;	
	while((z >> 8) > get_upper_height(x,y)){
		z -= SHADOWDEEP;
		x--;
		}
	return x0 - x;
}
int set_3D_adjust(int mode,int xx,int yy,int zz,int D)
{
	unsigned char** LineT = vMap -> lineT;
	unsigned char *p0,*p;
	int x,y;
	unsigned int z_sum = 0,u_sum=0,l_sum=0,cnt = 0,db_cnt=0,lh,uh;
	xx -= (xx - D) & 1;
	for(y = -D;y <= D;y++){
		p0 = LineT[(y + yy) & clip_mask_y];
		if(!p0)
			continue;
		for(x = -D;x < D;x += 2){
			p = (unsigned char *)((uintptr_t)p0 + ((x + xx) & clip_mask_x));
			if(!(*(p + H_SIZE) & DOUBLE_LEVEL)){
				z_sum += *p;
				z_sum += *(p + 1);
				cnt += 2;
				}
			else{
				lh = *p;
				uh = *(p + 1);
				u_sum += uh << 1;
				l_sum += lh << 1;
				db_cnt += 2;
				}
			}
		}
	if(mode == SET_3D_TO_THE_LOWER_LEVEL)
		return db_cnt ? l_sum/db_cnt : (cnt ? z_sum/cnt : 255);
	if(mode == SET_3D_TO_THE_UPPER_LEVEL)
		return db_cnt ? u_sum/db_cnt : (cnt ? z_sum/cnt : 255);
	if(mode == SET_3D_CHOOSE_LEVEL)
		if(db_cnt){
			uh = u_sum/db_cnt;
			return zz > (int)uh - 16 ? uh : l_sum/db_cnt;
			}
	return cnt ? z_sum/cnt : 255;
}

double get_height(int xx,int yy,double z_dbl,int D,DBV& n,int& z_below)
{
	unsigned char** LineT = vMap -> lineT;
	int double_level_log = 0;
	unsigned char *p0,*p;
	int zz = round(z_dbl);
	int x,y,z = 0,upper_log = 0,z_sum = 0,cnt = 0,lh,mh,uh;
	int N0 = 0,N1 = 0;
	int Sx0 = 0,Sy0 = 0;
	int Sx1 = 0,Sy1 = 0;
	int N=0;
	int Sx=0,Sx2=0;
	int Sy=0,Sy2=0;
	int Sz=0;
	int Sxz=0,Syz=0;
	int zmax = 0;

	int D2 = D*D;
	int D3 = D2*D;
	int D4 = D3*D;
	N = 2*D2 + D;
	Sx = -N;
	Sx2 = (D4 + D3 + 2*(D + D2))/3;
	Sy2 = (2*D4 + D2)/3 + D3;

	xx -= (xx - D) & 1;
	for(y = -D;y <= D;y++){
		p0 = LineT[(y + yy) & clip_mask_y];
		if(!p0)
			continue;
		for(x = -D;x < D;x += 2){
			p = p0 + ((x + xx) & clip_mask_x);
			if(!(*(p + H_SIZE) & DOUBLE_LEVEL)){
				z = *p;
				if((uh = *(p + 1)) > z)
					z = uh;
				if(zmax < z)
					zmax = z;

				if(zz < z){
					z_sum += z;
					cnt++;
					upper_log++;
					}
				}
			else{
				lh = *p;
//				  mh = lh + GET_THICKNESS_ATTR(*(p + H_SIZE),*(p + 1 + H_SIZE));
				uh = *(p + 1);
				mh = GET_MIDDLE_HIGHT(lh,uh);
				if(zz >= mh && zz <= uh){
					double_level_log = 1;
					if(zz - mh < uh - zz){
						z_sum += mh;
						cnt++;
						upper_log--;
						}
					else{
						z_sum += uh;
						cnt++;
						upper_log++;
						}
					}
				else{
					if(zz < lh){
						z_sum += lh;
						cnt++;
						upper_log++;
						}
					}

				z = zz >= mh ? uh : lh;
				if(zmax < z)
					zmax = z;
				}

			if(z - zz > MIN_WALL_DELTA){
				Sx1 += x; Sy1 += y; N1++;
				}
			else{
				Sx0 += x; Sy0 += y; N0++;
				}

			Sz += z;
			Sxz += x*z;
			Syz += y*z;
			}
		}

	z_below = zmax;

	if(!cnt)
		return -1;

	if(upper_log < 0){
		n = DBV(0,0,-1);
		return (z_dbl - (double)z_sum/cnt);
		}

//	if(N1 > 0 && N0 > 0){
//		n = DBV((double)Sx0/(double)N0 - (double)Sx1/(double)N1,(double)Sy0/(double)N0 - (double)Sy1/(double)N1,0);
//		n.norm(1);
//		return k_elastic_xy*(double)N1;
//		}

	if(double_level_log){
		n = DBV(0,0,1);
		return ((double)zmax - z_dbl);
		}

	double Det_x = (double)N*Sx2 - (double)Sx*Sx;
	if(fabs(Det_x) < DBL_EPS)
		return -1;
	double Det_y = (double)N*Sy2 - (double)Sy*Sy;
	if(fabs(Det_y) < DBL_EPS)
		return -1;
	double A = ((double)N*Sxz - (double)Sx*Sz)/Det_x;
	double B = ((double)N*Syz - (double)Sy*Sz)/Det_y;
	n = DBV(-A,-B,1);
	n.norm(1);
	return (double)zmax - z_dbl;
	//return ((double)zmax - z_dbl)*n.z;
}

int VariablePolygon::lower_average(int xx,int yy,int zz,double& X0,double& Y0,double& dZ0,double& X1,double& Y1,double& dZ1)
{
	unsigned char** LineT = vMap -> lineT;
	int xl,xr,al,ar,bl,br,d,where,Y;
	int x1,x2;
	Vertex *lfv,*rfv,*ltv,*rtv;
	Vertex **lto,**rto;
	Vertex **curr,**beg,**up,**end;
	unsigned char *p0,*p;

	beg = up = curr = vertices;
	end = vertices + num_vert - 1;
	curr++;
	for(;curr <= end;curr++)
		if((*up) -> yscr_8 > (*curr) -> yscr_8)
			up = curr;

	rfv = lfv = *up;
	lto = PREV_VERTEX(up);
	ltv = *lto;
	rto = NEXT_VERTEX(up);
	rtv = *rto;

	Y = lfv -> yscr_8; xl = lfv -> xscr_8;
	al = ltv -> xscr_8 - xl; bl = ltv -> yscr_8 - Y;
	ar = rtv -> xscr_8 - xl; br = rtv -> yscr_8 - Y;
	xr = xl = (xl << 16) + (1 << 15);

	DIV16T(al,bl);
	DIV16T(ar,br);

	int z,lh,mh,uh,dz,dm,du;
	int N = 0;
	int N0 = 0,N1 = 0;
	int Sx0 = 0,Sy0 = 0;
	int Sx1 = 0,Sy1 = 0;
	int Sz0 = 0,Sz1 = 0;
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
			p0 = LineT[(Y + yy) & clip_mask_y];
			if(!p0){
				non_loaded_space = 1;
				goto cont;
				}

			x1 = xl >> 16;
			x2 = xr >> 16;

			if(x1 > x2)
				SWAP(x1,x2);
			x1 &= ~1;
			x2 |= 1;
			N += (x2 - x1) >> 1;
			for(;x1 < x2;x1 += 2){
				p = p0 + ((x1 + xx) & clip_mask_x);
				if(!(*(p + H_SIZE) & DOUBLE_LEVEL)){
					z = *p;
					if((uh = *(p + 1)) > z)
						z = uh;
					if((dz = z - zz) > 0){
						if(dz < MIN_WALL_DELTA){
							Sx0 += x1; Sy0 += Y; N0++;
							Sz0 += dz;
							}
						else{
							Sx1 += x1; Sy1 += Y; N1++;
							Sz1 += dz;
							}
						}
					}
				else{
					lh = *p;
					uh = *(p + 1);
					if((du = uh - zz) > 0){
						mh = GET_MIDDLE_HIGHT(lh,uh);
						if((dm = zz - mh) > 0){
							if(dm > du){
								if(du > MIN_WALL_DELTA){
									Sx1 += x1; Sy1 += Y; N1++;
									Sz1 += du;
									}
								else{
									Sx0 += x1; Sy0 += Y; N0++;
									Sz0 += du;
									}
								}
							}
						else{
							if((dz = lh - zz) > 0){
								if(dz > MIN_WALL_DELTA){
									Sx1 += x1; Sy1 += Y; N1++;
									Sz1 += dz;
									}
								else{
									Sx0 += x1; Sy0 += Y; N0++;
									Sz0 += dz;
									}
								}
							}
						}
					}
				}

		cont:
			Y++;
			xl += al;
			xr += ar;
			}

		if(where){
			if(ltv == rtv)
				goto ret;
			lfv = ltv;
			lto = PREV_VERTEX(lto);
			ltv = *lto;

			br -= bl;

			xl = lfv -> xscr_8;
			al = ltv -> xscr_8 - xl;
			bl = ltv -> yscr_8 - Y;
			xl = (xl << 16) + (1 << 15);

			DIV16T(al,bl);
			}
		else{
			if(rtv == ltv)
				goto ret;
			rfv = rtv;
			rto = NEXT_VERTEX(rto);
			rtv = *rto;

			bl -= br;

			xr = rfv -> xscr_8;
			ar = rtv -> xscr_8 - xr;
			br = rtv -> yscr_8 - Y;
			xr = (xr << 16) + (1 << 15);

			DIV16T(ar,br);
			}
		}
ret:
	int ret_val = 0;
	if(N0){
		double t = 1/(double)N0;
		X0 = t*Sx0;
		Y0 = t*Sy0;
		dZ0 = t*Sz0;
		ret_val |= 1;
		}
	else
		dZ0 = 0;
	if(N1 > 4){
		double t = 1/(double)N1;
		X1 = t*Sx1;
		Y1 = t*Sy1;
		dZ1 = t*Sz1;
		ret_val |= 2;
		if(N1 > 4 && N1 > (N >> 1))
			ret_val |= 4;
		}
	else
		dZ1 = 0;
	return ret_val;
}

int VariablePolygon::upper_average(int xx,int yy,int zz,double& X0,double& Y0,double& dZ0,double& X1,double& Y1,double& dZ1)
{
	unsigned char** LineT = vMap -> lineT;
	int xl,xr,al,ar,bl,br,d,where,Y;
	int x1,x2;
	Vertex *lfv,*rfv,*ltv,*rtv;
	Vertex **lto,**rto;
	Vertex **curr,**beg,**up,**end;
	unsigned char *p0,*p;

	beg = up = curr = vertices;
	end = vertices + num_vert - 1;
	curr++;
	for(;curr <= end;curr++)
		if((*up) -> yscr_8 > (*curr) -> yscr_8)
			up = curr;

	rfv = lfv = *up;
	lto = PREV_VERTEX(up);
	ltv = *lto;
	rto = NEXT_VERTEX(up);
	rtv = *rto;

	Y = lfv -> yscr_8; xl = lfv -> xscr_8;
	al = ltv -> xscr_8 - xl; bl = ltv -> yscr_8 - Y;
	ar = rtv -> xscr_8 - xl; br = rtv -> yscr_8 - Y;
	xr = xl = (xl << 16) + (1 << 15);

	DIV16T(al,bl);
	DIV16T(ar,br);

	int lh,mh,uh,dm,du;
	int N = 0;
	int N0 = 0,N1 = 0;
	int Sx0 = 0,Sy0 = 0;
	int Sx1 = 0,Sy1 = 0;
	int Sz0 = 0,Sz1 = 0;
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
			p0 = LineT[(Y + yy) & clip_mask_y];
			if(!p0){
				non_loaded_space = 1;
				goto cont;
				}

			x1 = xl >> 16;
			x2 = xr >> 16;

			if(x1 > x2)
				SWAP(x1,x2);
			x1 &= ~1;
			x2 |= 1;
			N += (x2 - x1) >> 1;
			for(;x1 < x2;x1 += 2){
				p = p0 + ((x1 + xx) & clip_mask_x);
				if((*(p + H_SIZE) & DOUBLE_LEVEL)){
					lh = *p;
					uh = *(p + 1);
					if((du = uh - zz) > 0){
						mh = GET_MIDDLE_HIGHT(lh,uh);
						if((dm = zz - mh) > 0 && dm < du){
								if(terrain_analysis_flag & BREAKE_DBLEVEL_ENABLE){
									if(BREAKABLE_TERRAIN(*(p + 1 + H_SIZE))){
										terrain_analysis_flag |= BREAKE_DBLEVEL;
										continue;
										}
									else
										terrain_analysis_flag |= UNBREAKABLE_TERRAIN;
									}

								if(dm > MIN_WALL_DELTA){
									Sx1 += x1; Sy1 += Y; N1++;
									Sz1 += dm;
									}
								else{
									Sx0 += x1; Sy0 += Y; N0++;
									Sz0 += dm;
									}
								}
							}
						}
					}

		cont:
			Y++;
			xl += al;
			xr += ar;
			}

		if(where){
			if(ltv == rtv)
				goto ret;
			lfv = ltv;
			lto = PREV_VERTEX(lto);
			ltv = *lto;

			br -= bl;

			xl = lfv -> xscr_8;
			al = ltv -> xscr_8 - xl;
			bl = ltv -> yscr_8 - Y;
			xl = (xl << 16) + (1 << 15);

			DIV16T(al,bl);
			}
		else{
			if(rtv == ltv)
				goto ret;
			rfv = rtv;
			rto = NEXT_VERTEX(rto);
			rtv = *rto;

			bl -= br;

			xr = rfv -> xscr_8;
			ar = rtv -> xscr_8 - xr;
			br = rtv -> yscr_8 - Y;
			xr = (xr << 16) + (1 << 15);

			DIV16T(ar,br);
			}
		}
ret:
	if(terrain_analysis_flag & UNBREAKABLE_TERRAIN)
		terrain_analysis_flag &= ~BREAKE_DBLEVEL;
	if(terrain_analysis_flag & BREAKE_DBLEVEL)
		return 0;
	int ret_val = 0;
	if(N0){
		double t = 1/(double)N0;
		X0 = t*Sx0;
		Y0 = t*Sy0;
		dZ0 = t*Sz0;
		ret_val |= 1;
		}
	else
		dZ0 = 0;
	if(N1 > 4){
		double t = 1/(double)N1;
		X1 = t*Sx1;
		Y1 = t*Sy1;
		dZ1 = t*Sz1;
		ret_val |= 2;
		if(N1 > 4 && N1 > (N >> 1))
			ret_val |= 4;
		}
	else
		dZ1 = 0;
	return ret_val;
}

int VariablePolygon::check_terrain_type(int xx,int yy,int zz,int terrain_type)
{
	unsigned char** LineT = vMap -> lineT;
	int xl,xr,al,ar,bl,br,d,where,Y;
	int x1,x2;
	Vertex *lfv,*rfv,*ltv,*rtv;
	Vertex **lto,**rto;
	Vertex **curr,**beg,**up,**end;
	unsigned char *p0,*p,type;
	int lh,mh,uh;

	beg = up = curr = vertices;
	end = vertices + num_vert - 1;
	curr++;
	for(;curr <= end;curr++)
		if((*up) -> yscr_8 > (*curr) -> yscr_8)
			up = curr;

	rfv = lfv = *up;
	lto = PREV_VERTEX(up);
	ltv = *lto;
	rto = NEXT_VERTEX(up);
	rtv = *rto;

	Y = lfv -> yscr_8; xl = lfv -> xscr_8;
	al = ltv -> xscr_8 - xl; bl = ltv -> yscr_8 - Y;
	ar = rtv -> xscr_8 - xl; br = rtv -> yscr_8 - Y;
	xr = xl = (xl << 16) + (1 << 15);

	//if(bl)
		DIV16T(al,bl);
	  
	//if(br)
		DIV16T(ar,br);

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
			p0 = LineT[(Y + yy) & clip_mask_y];
			if(!p0)
				goto cont;

			x1 = xl >> 16;
			x2 = xr >> 16;

			if(x1 > x2)
				SWAP(x1,x2);
			x1 &= ~1;
			x2 |= 1;
					
			for(;x1 < x2;x1 += 2){
				// Считаю, что "грибы" не размещаются под двухуровневостью.
				p = p0 + ((x1 + xx) & clip_mask_x) + 1;
				if(GET_TERRAIN((type = *(p + H_SIZE))) != terrain_type)
					continue;
				if(!(type & DOUBLE_LEVEL)){
					if(((int)(*p) - zz) > 0)
						return 1;
					}
				else{
					lh = *(p - 1);
					uh = *p;
					mh = GET_MIDDLE_HIGHT(lh,uh);
					if(zz > mh && uh > zz)
						return 1;
					}
				}
		cont:
			Y++;
			xl += al;
			xr += ar;
			}

		if(where){
			if(ltv == rtv)
				return 0;
			lfv = ltv;
			lto = PREV_VERTEX(lto);
			ltv = *lto;

			br -= bl;

			xl = lfv -> xscr_8;
			al = ltv -> xscr_8 - xl;
			bl = ltv -> yscr_8 - Y;
			xl = (xl << 16) + (1 << 15);

			//if(bl)
				DIV16T(al,bl);
			}
		else{
			if(rtv == ltv)
				return 0;
			rfv = rtv;
			rto = NEXT_VERTEX(rto);
			rtv = *rto;

			bl -= br;

			xr = rfv -> xscr_8;
			ar = rtv -> xscr_8 - xr;
			br = rtv -> yscr_8 - Y;
			xr = (xr << 16) + (1 << 15);

			//if(br)
				DIV16T(ar,br);
			}
		}
}

int VariablePolygon::check_mole_submerging(int xx,int yy,int zz)
{
	unsigned char** LineT = vMap -> lineT;
	int xl,xr,al,ar,bl,br,d,where,Y;
	int x1,x2;
	Vertex *lfv,*rfv,*ltv,*rtv;
	Vertex **lto,**rto;
	Vertex **curr,**beg,**up,**end;
	unsigned char *p0,*p,type;
	int lh,mh,uh;

	beg = up = curr = vertices;
	end = vertices + num_vert - 1;
	curr++;
	for(;curr <= end;curr++)
		if((*up) -> yscr_8 > (*curr) -> yscr_8)
			up = curr;

	rfv = lfv = *up;
	lto = PREV_VERTEX(up);
	ltv = *lto;
	rto = NEXT_VERTEX(up);
	rtv = *rto;

	Y = lfv -> yscr_8; xl = lfv -> xscr_8;
	al = ltv -> xscr_8 - xl; bl = ltv -> yscr_8 - Y;
	ar = rtv -> xscr_8 - xl; br = rtv -> yscr_8 - Y;
	xr = xl = (xl << 16) + (1 << 15);

	//if(bl)
		DIV16T(al,bl);
	  
	//if(br)
		DIV16T(ar,br);

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
			p0 = LineT[(Y + yy) & clip_mask_y];
			if(!p0)
				goto cont;

			x1 = xl >> 16;
			x2 = xr >> 16;

			if(x1 > x2)
				SWAP(x1,x2);
			x1 &= ~1;
			x2 |= 1;
					
			for(;x1 < x2;x1 += 2){
				p = p0 + ((x1 + xx) & clip_mask_x) + 1;
				type = *(p + H_SIZE);
				if(!(type & DOUBLE_LEVEL)){
					if(((int)(*p) - zz) > 0 && !BREAKABLE_TERRAIN(type))
						return 0;
					}
				else{
					lh = *(p - 1);
					uh = *p;
					mh = GET_MIDDLE_HIGHT(lh,uh);
					if(zz > mh && uh > zz)
						return 0;
					}
				}
		cont:
			Y++;
			xl += al;
			xr += ar;
			}

		if(where){
			if(ltv == rtv)
				return 1;
			lfv = ltv;
			lto = PREV_VERTEX(lto);
			ltv = *lto;

			br -= bl;

			xl = lfv -> xscr_8;
			al = ltv -> xscr_8 - xl;
			bl = ltv -> yscr_8 - Y;
			xl = (xl << 16) + (1 << 15);

			//if(bl)
				DIV16T(al,bl);
			}
		else{
			if(rtv == ltv)
				return 1;
			rfv = rtv;
			rto = NEXT_VERTEX(rto);
			rtv = *rto;

			bl -= br;

			xr = rfv -> xscr_8;
			ar = rtv -> xscr_8 - xr;
			br = rtv -> yscr_8 - Y;
			xr = (xr << 16) + (1 << 15);

			//if(br)
				DIV16T(ar,br);
			}
		}
}

int height_analysis_fish(int xx,int yy,double z_dbl,int D,double& dZ,DBV& h_factor)
{
	unsigned char** LineT = vMap -> lineT;
	unsigned char *p0,*p;
	int zz = round(z_dbl);
	int x,y,z,lh,mh,uh;
	int upper_log = 0;
	int N=0;
	int Sx=0, Sy=0, Sz=0;
	int zmin = 255;
	int zmax = 0;

	xx -= (xx - D) & 1;
	for(y = -D;y <= D;y++){
		p0 = LineT[(y + yy) & clip_mask_y];
		if(!p0){
			non_loaded_space = 1;
			return 0;
			}
		for(x = -D;x < D;x += 2){
			p = p0 + ((x + xx) & clip_mask_x);
			if(!(*(p + H_SIZE) & DOUBLE_LEVEL)){
				z = *p;
				if(zmax < z)
					zmax = z;
				if(zmin > z)
					zmin = z;
				if(zz < z){
					upper_log++;
					Sx += x; Sy += y; Sz += z; N++;
					}
				}
			else{
				lh = *p;
				uh = *(p + 1);
				mh = GET_MIDDLE_HIGHT(lh,uh);
				if(zz >= mh && zz <= uh){
					if(zz - mh < uh - zz){
						upper_log--;
						Sx += x; Sy += y; Sz += mh; N++;
						}
					else{
						upper_log++;
						Sx += x; Sy += y; Sz += mh; N++;
						}
					}
				else{
					if(zz < lh){
						upper_log++;
						Sx += x; Sy += y; Sz += mh; N++;
						}
					}

				if(zmax < uh)
					zmax = uh;
				if(zmin > lh)
					zmin = lh;
				}
			}
		}

	if(!N)
		return 0;

	h_factor = DBV((double)Sx/N,(double)Sx/N,zmax - zmin);
	
	dZ = (double)Sz/N - z_dbl;
	return upper_log > 0 ? (dZ > 0 ? 1 : 0) :  (dZ < 0 ? 1 : 0);
}
int square_analysis(int xx,int yy,double z_dbl,int D,double& dZ,DBV& r,DBV& n)
{
	unsigned char** LineT = vMap -> lineT;
	unsigned char *p0,*p;
	int zz = round(z_dbl);
	int x,y,z,dz,upper_log = 0,lh,mh,uh;
	int x_sum = 0,y_sum = 0,z_sum = 0,dz_sum = 0,cnt = 0;
	int N=0;
	int Sx=0,Sx2=0;
	int Sy=0,Sy2=0;
	int Sz=0;
	int Sxz=0,Syz=0;

	int D2 = D*D;
	int D3 = D2*D;
	int D4 = D3*D;
	N = 2*D2 + D;
	Sx = -N;
	Sx2 = (D4 + D3 + 2*(D + D2))/3;
	Sy2 = (2*D4 + D2)/3 + D3;

	xx -= (xx - D) & 1;
	for(y = -D;y <= D;y++){
		p0 = LineT[(y + yy) & clip_mask_y];
		if(!p0)
			continue;
		for(x = -D;x < D;x += 2){
			p = p0 + ((x + xx) & clip_mask_x);
			if(!(*(p + H_SIZE) & DOUBLE_LEVEL)){
				z = *p;
				if((uh = *(p + 1)) > z)
					z = uh;
				if((dz = z - zz) > 0){
					x_sum += x;
					y_sum += y;
					z_sum += z;
					dz_sum += dz;
					cnt++;
					upper_log++;
					}
				}
			else{
				lh = *p;
				uh = *(p + 1);
				if(zz <= uh){
					mh = GET_MIDDLE_HIGHT(lh,uh);
					if(zz >= mh){
						if(zz - mh < uh - zz){
							z = lh;
							x_sum += x;
							y_sum += y;
							z_sum += mh;
							dz_sum += mh - zz;
							cnt++;
							upper_log--;
							}
						else{
							z = uh;
							x_sum += x;
							y_sum += y;
							z_sum += uh;
							dz_sum += uh - zz;
							cnt++;
							upper_log++;
							}
						}
					else{
						z = lh;
						if((dz = lh - zz) > 0){
							x_sum += x;
							y_sum += y;
							z_sum += lh;
							dz_sum += dz;
							cnt++;
							upper_log++;
							}
						}
					}
				else
					z = uh;
				}

			Sz += z;
			Sxz += x*z;
			Syz += y*z;
			}
		}

	if(!cnt)
		return 0;

	double cnt_inv = 1/double(cnt);
	r.x = cnt_inv*x_sum;
	r.y = cnt_inv*y_sum;
	r.z = cnt_inv*z_sum;
	dZ = cnt_inv*dz_sum;

	double Det_x = (double)N*Sx2 - (double)Sx*Sx;
	if(fabs(Det_x) < DBL_EPS){
		n = DBV(0,0,1);
		return 1;
		}
	double Det_y = (double)N*Sy2 - (double)Sy*Sy;
	if(fabs(Det_y) < DBL_EPS){
		n = DBV(0,0,1);
		return 1;
		}
	double A = ((double)N*Sxz - (double)Sx*Sz)/Det_x;
	double B = ((double)N*Syz - (double)Sy*Sz)/Det_y;
	n = DBV(-A,-B,1);
	n.norm(1);
	//dZ *= n.z;
	return 1;
}

int VariablePolygon::lower_average_z(int x_offset,int y_offset,int z_offset,double& dZ,DBV& r,DBV& n)
{
	unsigned char** LineT = vMap -> lineT;
	int xl,xr,al,ar,bl,br,d,where,Y;
	int x1,x2;
	Vertex *lfv,*rfv,*ltv,*rtv;
	Vertex **lto,**rto;
	Vertex **curr,**beg,**up,**end;
	unsigned char *p0,*p;
	int zl,zr,zkl,zkr,zz,z1,z2,zf,zkf;
	int z,dz,lh,mh,uh;

	beg = up = curr = vertices;
	end = vertices + num_vert - 1;
	curr++;
	for(;curr <= end;curr++)
		if((*up) -> yscr_8 > (*curr) -> yscr_8)
			up = curr;

	rfv = lfv = *up;
	lto = PREV_VERTEX(up);
	ltv = *lto;
	rto = NEXT_VERTEX(up);
	rtv = *rto;

	Y = lfv -> yscr_8; xl = lfv -> xscr_8;
	al = ltv -> xscr_8 - xl; bl = ltv -> yscr_8 - Y;
	ar = rtv -> xscr_8 - xl; br = rtv -> yscr_8 - Y;
	xr = xl = (xl << 16) + (1 << 15);
	
	zl = lfv -> zscr_8;
	zkl = ltv -> zscr_8 - zl; 
	zkr = rtv -> zscr_8 - zl;
	zr = zl = (zl << 16) + (1 << 15);

	DIV16T(al,bl);
	DIV16T(ar,br);
	DIV16T(zkl,bl);
	DIV16T(zkr,br);

	int upper_log = 0;
	int x_sum = 0,y_sum = 0,z_sum = 0,dz_sum = 0,cnt = 0;
	int N=0;
	int Sx=0,Sx2=0;
	int Sy=0,Sy2=0;
	int Sz=0;
	int Sxz=0,Syz=0;
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
			p0 = LineT[(Y + y_offset) & clip_mask_y];
			if(!p0){
				non_loaded_space = 1;
				goto cont;
				}

			x1 = xl >> 16;
			x2 = xr >> 16;
			z1 = zl;
			z2 = zr;

			if(x1 == x2)
				goto cont;

			if(x1 > x2){
				SWAP(x1,x2);
				SWAP(z1,z2);
				}
			zkf = (z2 - z1)/(x2 - x1);
			//x1 &= ~1;
			if(x1 & 1){
				z1 -= zkf;
				x1--;
				}
			x2 |= 1;
			zkf >>= 1;
			zf = z1;
			N += (x2 - x1) >> 1;
			for(;x1 < x2;x1 += 2){
				p = p0 + ((x1 + x_offset) & clip_mask_x);
				zz = (zf >> 16) + z_offset;
				if(!(*(p + H_SIZE) & DOUBLE_LEVEL)){
					z = *p;
					if((uh = *(p + 1)) > z)
						z = uh;
					if((dz = z - zz) > 0){
						x_sum += dz*x1;
						y_sum += dz*Y;
						z_sum += z;
						dz_sum += dz;
						cnt++;
						upper_log++;
						}
					}
				else{
					lh = *p;
					uh = *(p + 1);
					if(zz <= uh){
						mh = GET_MIDDLE_HIGHT(lh,uh);
						if(zz >= mh){
							//double_level_log = 1;
							if(zz - mh < uh - zz){
								z = lh;
								dz = mh - zz;
								x_sum += dz*x1;
								y_sum += dz*Y;
								z_sum += mh;
								dz_sum += dz;
								cnt++;
								upper_log--;
								}
							else{
								z = uh;
								dz = uh - zz;
								x_sum += dz*x1;
								y_sum += dz*Y;
								z_sum += uh;
								dz_sum += dz;
								cnt++;
								upper_log++;
								}
							}
						else{
							z = lh;
							if((dz = lh - zz) > 0){
								x_sum += dz*x1;
								y_sum += dz*Y;
								z_sum += lh;
								dz_sum += dz;
								cnt++;
								upper_log++;
								}
							}
						}
					else
						z = uh;
					}

				Sx += x1;
				Sx2 += x1*x1;
				Sy += Y;
				Sy2 += Y*Y;
				Sxz += x1*z;
				Syz += Y*z;
				Sz += z;

				zf += zkf;
				}

		cont:
			Y++;
			xl += al;
			xr += ar;
			zl += zkl;
			zr += zkr;
			}

		if(where){
			if(ltv == rtv)
				goto ret;
			lfv = ltv;
			lto = PREV_VERTEX(lto);
			ltv = *lto;

			br -= bl;

			xl = lfv -> xscr_8;
			al = ltv -> xscr_8 - xl;
			bl = ltv -> yscr_8 - Y;
			xl = (xl << 16) + (1 << 15);
			
			zl = lfv -> zscr_8;
			zkl = ltv -> zscr_8 - zl;
			zl = (zl << 16) + (1 << 15);

			DIV16T(al,bl);
			DIV16T(zkl,bl);
			}
		else{
			if(rtv == ltv)
				goto ret;
			rfv = rtv;
			rto = NEXT_VERTEX(rto);
			rtv = *rto;

			bl -= br;

			xr = rfv -> xscr_8;
			ar = rtv -> xscr_8 - xr;
			br = rtv -> yscr_8 - Y;
			xr = (xr << 16) + (1 << 15);
			
			zr = rfv -> zscr_8;
			zkr = rtv -> zscr_8 - zr;
			zr = (zr << 16) + (1 << 15);

			DIV16T(ar,br);
			DIV16T(zkr,br);
			}
		}
ret:
	if(!cnt)
		return 0;
	
	g_x_sum += x_sum;
	g_y_sum += y_sum;
	g_z_sum += z_sum;
	g_dz_sum += dz_sum;
	g_cnt += cnt;
	gN += N;
	gSx += Sx;
	gSx2 += Sx2;
	gSy += Sy;
	gSy2 += Sy2;
	gSz += Sz;
	gSxz += Sxz;
	gSyz += Syz;

	double cnt_inv = 1/double(cnt);
	r.x = cnt_inv*x_sum;
	r.y = cnt_inv*y_sum;
	r.z = cnt_inv*z_sum;
	dZ = cnt_inv*dz_sum;

	double Det_x = (double)N*Sx2 - (double)Sx*Sx;
	if(fabs(Det_x) < DBL_EPS){
		n = DBV(0,0,1);
		return 1;
		}
	double Det_y = (double)N*Sy2 - (double)Sy*Sy;
	if(fabs(Det_y) < DBL_EPS){
		n = DBV(0,0,1);
		return 1;
		}
	double A = ((double)N*Sxz - (double)Sx*Sz)/Det_x;
	double B = ((double)N*Syz - (double)Sy*Sz)/Det_y;
	n = DBV(-A,-B,1);
	n.norm(1);
	//dZ *= n.z;
	return 1;
}

/*******************************************************************************
			Object's function
*******************************************************************************/
void Object::dynamics_init(char* name)
{
	strcpy(prm_name,name);
	strcpy(prm_name + strlen(prm_name) - 3,"prm");
	XStream test(0);
	if(test.open(prm_name,XS_IN))
		test.close();
	else
		strcpy(prm_name,"resource/m3d/mechous/default.prm");

	load_parameters(prm_name);

	R = DBV(0,0,0);
	V = DBV(0,0,0);
	W = DBV(0,0,0);
	A_l2g = A_g2l = DBM(1,-1,1,DIAGONAL);
	Q_l2g = Quaternion(1,0,0,0);

	interpolation_on = 0;
	dR_corr = DBV(0,0,0);
	dQ_corr = Quaternion(1,0,0,0);

	zmin = n_wheels ? (-int(wheels[0].r.z) + wheels[0].radius) : zmax;
	xmax_real = round(xmax*scale_size);
	ymax_real = round(ymax*scale_size);
	zmax_real = round(zmax*scale_size);
	rmax_screen = radius = round(rmax*scale_size);
	scale_real = scale = 0.5;
	int scl_max = (126 << 8)/rmax - 2;
	if(scl_max > 255)
		scl_max = 255;
	scale_max = scl_max/256.;

	disable_control = 0;
	rudder = 0;
	traction = 0;
	brake = 0;
	hand_brake = 0;
	turbo = 0;
	speed = 0;
	archimedean = 0;
	in_water = 0;
	closest_field_object = 0;
	jump_power = 0;
	side_impulse_enable = 0;
	mole_on = 0;
	draw_mode = NORMAL_DRAW_MODE;
	//draw_mode = TRANSPARENCY_DRAW_MODE;
	device_switch_latency = 0;
	prev_controls = current_controls = 0;
	last_send_time = 0;

	helicopter = 0;
	helicopter_strife = 0;
	helicopter_time = 0;
	helicopter_thrust_phase = 0;
	helicopter_circle_phase = 0;

	after_db_coll = 0;
	stuff_phase = 0;
	
	k_elastic_modulation = 1;
	device_modulation = 1;
	water_immersion = 0;
	terrain_immersion = 0;
	
	skyfarmer_fly_direction = 0;
	skyfarmer_direction = DBV(1,0,0);
	
	z_below = 0;

	V_drag = V_drag_free;
	W_drag = W_drag_free;

	update_coord();
}

//stalkerg Самове важное для физики тут
void Object::update_coord(int camera_correction)
{
	double xx,yy;
	if(R.z > 450)
		R.z = 450;
	if(!camera_correction)
		R_prev = R_curr;
	if(!interpolation_on) {
		xx = R.x = fmod(R.x + map_size_x, map_size_x);
		yy = R.y = fmod(R.y + map_size_y, map_size_y);
		R_curr = R;
		A_scl = A_g2sZ*A_l2g;
	} else {
		DBV R_corr = R + dR_corr;
		xx = R_corr.x = fmod(R_corr.x + map_size_x, map_size_x);
		yy = R_corr.y = fmod(R_corr.y + map_size_y, map_size_y);
		R_curr = R_corr;
		A_scl = A_g2sZ*DBM(1,-1,1,DIAGONAL)*DBM(dQ_corr)*DBM(1,-1,1,DIAGONAL)*A_l2g;
		dR_corr *= correction_tau;
		dQ_corr = Slerp(Quaternion(1,0,0,0), dQ_corr, correction_tau);
	}

	if(abs(R_curr.x - x_of_last_update) + abs(R_curr.y - y_of_last_update) > movement_detection_threshould) {
		dynamic_state |= ITS_MOVING;
		x_of_last_update = R_curr.x;
		y_of_last_update = R_curr.y;
	}

	
	xx = getDistX(xx,(double)ViewX);
	yy = getDistY(yy,(double)ViewY);
	

	if(DepthShow) {
		double offset = zmin*scale_real;
		double x1 = A_g2s.a[0]*xx + A_g2s.a[1]*yy - A_g2s.a[2]*offset;
		double y1 = A_g2s.a[3]*xx + A_g2s.a[4]*yy - A_g2s.a[5]*offset;
		double z0 = A_g2s.a[6]*xx + A_g2s.a[7]*yy;
		double z1 = focus_flt + z0 - (R_curr.z >> 1);
		scale_real = z1 > 0 ? focus_flt/z1 : 1;
		z1 = z0 + ViewZ;
		z1 = z1 > 0 ? focus_flt/z1 : 1;
		R_scr.x = round(x1*z1) + ScreenCX;
		R_scr.y = round(y1*z1) + ScreenCY;
	} else {
		R_scr.x = round((xx*cosTurnFlt - yy*sinTurnFlt)) + ScreenCX;
		R_scr.y = round((xx*sinTurnFlt + yy*cosTurnFlt)) + ScreenCY;
		double z1 = focus_flt - (R_curr.z >> 1);
		scale_real = z1 > 0 ? focus_flt/z1 : 1;
	}
	
	scale_real *= scale_general;
	scale_real *= scale_size;
	if(scale_real >1)
		scale_real = 1;
	scale = scale_real*ScaleMapInvFlt;
	if(scale > scale_max)
		scale = scale_max;
	//A_scl = A_g2sZ*A_l2g*scale;
	A_scl *= scale;
	rmax_screen = round(rmax*scale);

	xmax_real = round(xmax*scale_real);
	ymax_real = round(ymax*scale_real);
	zmax_real = round(zmax*scale_real);
	radius = round(rmax*scale_real);
	if(radius < 4)
		radius = 4;

	psi = (A_l2g[8] < 0 ?
		(-RTOG(atan2(A_l2g[0],A_l2g[1]))) :
		(RTOG(atan2(A_l2g[0],A_l2g[1])))) - Pi/2;
	psi &= ANGLE_MASK;
	tetta = RTOG(DBV(A_l2g[6],A_l2g[7],-A_l2g[8]).tetta());
}
void Object::set_3D(int mode,int x,int y,int z,int dz,int angle,int speed)
{
	radius = round(rmax*scale_size);
	if(mode == SET_3D_CONST_Z)
			z = (int)R.z;
	else
		if(mode & (SET_3D_TO_THE_LOWER_LEVEL | SET_3D_TO_THE_UPPER_LEVEL | SET_3D_CHOOSE_LEVEL)){
			z = set_3D_adjust(mode,x,y,z,radius*2/3);
			z += dz + zmax_real;
			}

	R = DBV(x,y,z);
	V = DBV(0,speed,0);
	W = DBV(0,0,0);
	interpolation_on = 0;
	dR_corr = DBV(0,0,0);
	dQ_corr = Quaternion(1,0,0,0);
	closest_field_object = 0;
	double dangle = GTOR(angle) - M_PI/2;
	A_l2g = DBM(1,-1,1,DIAGONAL)*DBM(dangle,Z_AXIS);
	A_g2l = DBM(-dangle,Z_AXIS)*DBM(1,-1,1,DIAGONAL);
	Q_l2g = Quaternion(DBM(dangle,Z_AXIS));
	update_coord();
}
void Object::set_active(int on)
{
	active = on;
	if(!on)
		return;
	if(!strcmp(prm_name,"resource/m3d/mechous/default.prm"))
		ID = n_models > 1 ? ID_INSECT : ID_STUFF;
	else
		ID = ID_VANGER;
	load_parameters(prm_name);
}

/*******************************************************************************
		Object movement control
*******************************************************************************/
void Object::motor_control(int dir)
{
	int sign = SIGN(traction);
	if(dir == ADD_POWER)
		if((traction += traction_increment) > 256)
			traction = 256;
	if(dir == DEL_POWER)
		if((traction -= traction_increment) < -256)
			traction = -256;
	if(!(sign + SIGN(traction)))
		traction = 0;
}
void Object::brake_on()
{
	traction = traction/2;
	brake = 1;
}
void Object::steer(int dir)
{
	int delta = rudder_step << 1;
	//zmod 1.17
	//if (!hand_brake && traction>0 && abs(rudder)<delta) delta = delta/2;
	if(dir == LEFT_SIDE)
		if((rudder += delta) > rudder_max)
			rudder = rudder_max;

	if(dir == RIGHT_SIDE)
		if((rudder -= delta) < -rudder_max)
			rudder = -rudder_max;
}
void Object::impulse(int side)
{
	if(mole_on)
		return;
	if(side_impulse_enable > side_impulse_delay)
		side_impulse_enable = 0;
	if(side_impulse_enable > side_impulse_duration)
		return;

	if(side == LEFT_SIDE || side == RIGHT_SIDE){
		if(!(dynamic_state & (WHEELS_TOUCH | GROUND_COLLISION | TOUCH_OF_WATER)))
			return;
		//double df = f_spring_impulse*dt_impulse/pow(m,0.3);
		double df = f_spring_impulse*dt_impulse;
		V += A_g2l*DBV(0,0,df);
		W += J_inv*DBV(0,side == LEFT_SIDE ? -df*xmax_real : df*xmax_real,0);
		}
	else{
		DBV F_nitro(0,side == FRONT_SIDE ? 1 : -1,0);
		F_nitro *= A_l2g;
		F_nitro.z = sqrt(sqr(F_nitro.x) + sqr(F_nitro.y))/4;
		F_nitro *= A_g2l;
		F_nitro.norm(f_traction_impulse*dt_impulse);
		V += F_nitro;
		}
}
void Object::impulse(int angle,int distance,int slope,int lever_arm)
{
	impulse(DBV(-Sin(slope)*Sin(angle),-Sin(slope)*Cos(angle),Cos(slope)),distance,lever_arm);
}
void Object::impulse(const DBV& direct,int distance,int lever_arm)
{
	if(R.z > 450)
		return;
	DBV F = A_g2l*direct;
	F.norm((double)distance*k_distance_to_force*dt_impulse/sqrt(m));
	V += F;
	if(lever_arm){
		DBV r = DBV(random(2*lever_arm) - lever_arm,
			    random(2*lever_arm) - lever_arm,
			    random(2*lever_arm) - lever_arm);
		W += J_inv*(r % F);
		}
}
void Object::jump()
{
	if(R.z > 450)
		return;
	double f = device_modulation*5*jump_power*k_distance_to_force*dt_impulse/pow(m,0.3);
	DBV dV = A_g2l*DBV(-Sin(Pi/10)*Sin(psi),-Sin(Pi/10)*Cos(psi),Cos(Pi/10));
	V = V + dV*f;
	jump_power = 0;
}
int Object::get_max_jump_distance()
{
	DBV V = DBV(Sin(Pi/10),0,Cos(Pi/10))*(5*max_jump_power*k_distance_to_force*dt_impulse/pow(m,0.3));
	DBV R(0,0,0);
	while(R.z >= 0){
		V.z -= g*dt0;
		R += V*dt0;
		V *= V_drag_free*pow(V_drag_speed,V.vabs());
		 }
	return round(R.x);
}

void Object::controls(int mode,int param)
{
	switch(mode){
		case CONTROLS::STEER_BY_ANGLE:
			rudder = param;
			break;
		case CONTROLS::STEER_LEFT:
			current_controls |= 1 << CONTROLS::STEER_LEFT;
			steer(LEFT_SIDE);
			helicopter_strife = 0;
			break;
		case CONTROLS::STEER_RIGHT:
			current_controls |= 1 << CONTROLS::STEER_RIGHT;
			steer(RIGHT_SIDE);
			helicopter_strife = 0;
			break;

		case CONTROLS::TRACTION_INCREASE:
			current_controls |= 1 << CONTROLS::TRACTION_INCREASE;
			motor_control(ADD_POWER);
			break;
		case CONTROLS::TRACTION_DECREASE:
			current_controls |= 1 << CONTROLS::TRACTION_DECREASE;
			motor_control(DEL_POWER);
			break;
		case CONTROLS::TURBO_QUANT:
			current_controls |= 1 << CONTROLS::TURBO_QUANT;
			turbo = 1;
			break;
		case CONTROLS::BRAKE_QUANT:
			current_controls |= 1 << CONTROLS::BRAKE_QUANT;
			brake_on();
			break;
		case CONTROLS::HAND_BRAKE_QUANT:
			current_controls |= 1 << CONTROLS::HAND_BRAKE_QUANT;
			hand_brake = 1;
			break;

		case CONTROLS::JUMP_POWER_ACCUMULATION_ON:
			//std::cout<<"JUMP_POWER_ACCUMULATION_ON"<<std::endl;
			if(!jump_power && dynamic_state & (WHEELS_TOUCH | GROUND_COLLISION | TOUCH_OF_WATER) && !mole_on){
				if(active)
					SOUND_KIDPULL();
				jump_power = 1;
				}
			break;
		case CONTROLS::JUMP_USING_ACCUMULATED_POWER:
			if(jump_power){
				jump();
				if(active)
					SOUND_KIDPUSH();
				}
			break;

		case CONTROLS::LEFT_SIDE_IMPULSE:
			current_controls |= 1 << CONTROLS::LEFT_SIDE_IMPULSE;
			if(!helicopter)
				impulse(LEFT_SIDE);
			else{
				steer(LEFT_SIDE);
				helicopter_strife = 1;
				}
			break;
		case CONTROLS::RIGHT_SIDE_IMPULSE:
			current_controls |= 1 << CONTROLS::RIGHT_SIDE_IMPULSE;
			if(!helicopter)
				impulse(RIGHT_SIDE);
			else{
				steer(RIGHT_SIDE);
				helicopter_strife = 1;
				}
			break;

		case CONTROLS::HELICOPTER_UP:
			if(UsingCopterig(0) && !mole_on){
				if(!helicopter)
					helicopter = round(R.z);
				if((helicopter += helicopter_height_incr) > max_helicopter_height)
					helicopter = max_helicopter_height;
				helicopter_time = max_helicopter_time;
				rudder = 0;
				}
			break;																					
		case CONTROLS::HELICOPTER_DOWN:
			if(helicopter && (helicopter -= helicopter_height_decr) < 0){
				helicopter = 0;
				device_switch_latency = GET_DEVICE_LATENCY();
				}
			break;

		case CONTROLS::FLOTATION_UP:
			if(dynamic_state & TOUCH_OF_WATER && archimedean < 256 && (archimedean += 4) >= 256){
				archimedean = 256;
				device_switch_latency = GET_DEVICE_LATENCY();
				 }
			break;
		case CONTROLS::FLOTATION_DOWN:
			if(dynamic_state & TOUCH_OF_WATER && archimedean && (archimedean -= 16) < 0)
				archimedean = 0;
			break;

		case CONTROLS::MOLE_DOWN:
			if(UsingCrotrig(0) && !(dynamic_state & TOUCH_OF_WATER) && (dynamic_state & TOUCH_OF_GROUND) && !helicopter && check_mole_submerging()){
				mole_on = 256;
				rudder = 0;
				StartMoleProcess();
				device_switch_latency = GET_DEVICE_LATENCY();
				}
			break;
		case CONTROLS::MOLE_UP:
			mole_on--;
			device_switch_latency = GET_DEVICE_LATENCY();
			break;

		case CONTROLS::VIRTUAL_UP:
			current_controls |= 1 << CONTROLS::VIRTUAL_UP;
			if(mole_on){
				controls(CONTROLS::MOLE_UP);
				return;
				}
			if((dynamic_state & TOUCH_OF_WATER) && UsingCutterig(0) && archimedean < 256){
				controls(CONTROLS::FLOTATION_UP);
				return;
				}
			if(device_switch_latency > SDL_GetTicks())
				return;
			controls(CONTROLS::HELICOPTER_UP);
			break;

		case CONTROLS::VIRTUAL_DOWN:
			current_controls |= 1 << CONTROLS::VIRTUAL_DOWN;
			if(helicopter){
				controls(CONTROLS::HELICOPTER_DOWN);
				return;
				}
			if(device_switch_latency > SDL_GetTicks())
				return;
			if(dynamic_state & TOUCH_OF_WATER)
				controls(CONTROLS::FLOTATION_DOWN);
			else 
				controls(CONTROLS::MOLE_DOWN);
			break;

		}
}
#ifdef SICHER_DEBUG
void Object::direct_keyboard_control()
{
	if(XKey.Pressed(VK_UP)){
		static int delay;
		if(traction < 0)
			delay = 5;
		if(delay-- > 0)
			controls(CONTROLS::BRAKE_QUANT);
		else
			controls(CONTROLS::TRACTION_INCREASE);
		}
	if(XKey.Pressed(VK_DOWN) || XKey.Pressed(VK_OEM_5) || XKey.Pressed('5')){
		static int delay;
		if(traction > 0)
			delay = 5;
		if(delay-- > 0)
			controls(CONTROLS::BRAKE_QUANT);
		else
			controls(CONTROLS::TRACTION_DECREASE);
		}

	if(!aciAutoRun){
		if(XKey.Pressed(VK_SHIFT))
			controls(CONTROLS::TURBO_QUANT);
		}
	else{
		if(!XKey.Pressed(VK_SHIFT))
			controls(CONTROLS::TURBO_QUANT);
		}
	if(XKey.Pressed('X'))
		controls(CONTROLS::HAND_BRAKE_QUANT);

	if(XKey.Pressed(VK_LEFT))
		controls(CONTROLS::STEER_LEFT);
	if(XKey.Pressed(VK_RIGHT))
		controls(CONTROLS::STEER_RIGHT);

	if(XKey.Pressed(VK_END))
		controls(CONTROLS::LEFT_SIDE_IMPULSE);
	if(XKey.Pressed(VK_NEXT))
		controls(CONTROLS::RIGHT_SIDE_IMPULSE);

	if(XKey.Pressed(VK_INSERT) | XKey.Pressed('A'))
		controls(CONTROLS::JUMP_POWER_ACCUMULATION_ON);
	else
		if(jump_power)
			controls(CONTROLS::JUMP_USING_ACCUMULATED_POWER);

	if(XKey.Pressed('Z'))
		controls(CONTROLS::VIRTUAL_UP);

	if(XKey.Pressed(VK_PRIOR))
		controls(CONTROLS::VIRTUAL_UP);

	if(XKey.Pressed(VK_HOME))
		controls(CONTROLS::VIRTUAL_DOWN);

#ifdef SICHER_DEBUG	
	static int fish_switch = 1;
	if(XKey.Pressed('G') && fish_switch){
		fish_test = 1 - fish_test;
		fish_switch = 0;
		}
	else
		fish_switch = 1;

	/*
	static int skyfarmer_switch = 1;
	if(XKey.Pressed('H') && skyfarmer_switch){
		skyfarmer_test = 1 - skyfarmer_test;
		if(skyfarmer_test){
			skyfarmer_start(round(R.x),round(R.y),0);
			skyfarmer_set_direction(DBV(1,1,0));
			}
		skyfarmer_switch = 0;
		}
	else
		skyfarmer_switch = 1;
	 */
	static int stop_switch = 1;
	if(XKey.Pressed('B') && stop_switch){
		stop_flag = 1 - stop_flag;
		stop_switch = 0;
		}
	else
		stop_switch = 1;
#endif

    	aciCurJumpImpulse = jump_power;
	aciMaxJumpImpulse = max_jump_power;
}
#else
void Object::direct_keyboard_control()
{
	if(iKeyPressed(iKEY_MOVE_FORWARD)){
		static int delay;
		if(traction < 0)
			delay = 5;
		if(delay-- > 0)
			controls(CONTROLS::BRAKE_QUANT);
		else
			controls(CONTROLS::TRACTION_INCREASE);
		last_keyboard_touch = SDL_GetTicks();
		}
	if(iKeyPressed(iKEY_MOVE_BACKWARD)){
		static int delay;
		if(traction > 0)
			delay = 5;
		if(delay-- > 0)
			controls(CONTROLS::BRAKE_QUANT);
		else
			controls(CONTROLS::TRACTION_DECREASE);
		last_keyboard_touch = SDL_GetTicks();
		}

	if(!aciAutoRun){
		if(iKeyPressed(iKEY_ACCELERATION))
			controls(CONTROLS::TURBO_QUANT);
		}
	else{
		if(!iKeyPressed(iKEY_ACCELERATION))
			controls(CONTROLS::TURBO_QUANT);
		}
	if(iKeyPressed(iKEY_HANDBRAKE))
		controls(CONTROLS::HAND_BRAKE_QUANT);

	if(iKeyPressed(iKEY_TURN_WHEELS_LEFT)){
		controls(CONTROLS::STEER_LEFT);
		last_keyboard_touch = SDL_GetTicks();
		}
	if(iKeyPressed(iKEY_TURN_WHEELS_RIGHT)){
		controls(CONTROLS::STEER_RIGHT);
		last_keyboard_touch = SDL_GetTicks();
		}

	if(iKeyPressed(iKEY_TURN_OVER_LEFT))
		controls(CONTROLS::LEFT_SIDE_IMPULSE);
	if(iKeyPressed(iKEY_TURN_OVER_RIGHT))
		controls(CONTROLS::RIGHT_SIDE_IMPULSE);

	if(iKeyPressed(iKEY_ACTIVATE_KID))
		controls(CONTROLS::JUMP_POWER_ACCUMULATION_ON);
	else
		if(jump_power)
			controls(CONTROLS::JUMP_USING_ACCUMULATED_POWER);

	//if(iKeyPressed(iKEY_VERTICAL_THRUST))
	//	controls(CONTROLS::HELICOPTER_UP);


	if(iKeyPressed(iKEY_DEVICE_ON))	     
		controls(CONTROLS::VIRTUAL_UP);

	if(iKeyPressed(iKEY_DEVICE_OFF))
		controls(CONTROLS::VIRTUAL_DOWN);

    	aciCurJumpImpulse = jump_power;
	aciMaxJumpImpulse = max_jump_power;
}
#endif

void Object::direct_joystick_control()
{
	if(!XJoystickInput())
		return;

	if((int)(SDL_GetTicks() - last_keyboard_touch) < 2000)
		return;

	int dx = XJoystickState.lX;
	if(abs(dx) < RANGE_MAX/16)
		dx = 0;
	int dy = XJoystickState.lY;
	if(abs(dy) < RANGE_MAX/16)
		dy = 0;

	if(!XJoystickState.rgbButtons[JoystickStickSwitchButton - VK_BUTTON_1])
		switch(JoystickMode){
			case JOYSTICK_GamePad:
				if(dy < 0){
					static int delay;
					if(traction < 0)
						delay = 5;
					if(delay-- > 0)
						controls(CONTROLS::BRAKE_QUANT);
					else
						controls(CONTROLS::TRACTION_INCREASE);
					}
				if(dy > 0){
					static int delay;
					if(traction > 0)
						delay = 5;
					if(delay-- > 0)
						controls(CONTROLS::BRAKE_QUANT);
					else
						controls(CONTROLS::TRACTION_DECREASE);
					}
				
				if(dx < 0)
					controls(CONTROLS::STEER_LEFT);
				if(dx > 0)
					controls(CONTROLS::STEER_RIGHT);
				break;

			case JOYSTICK_Joystick:
				traction = round(sqrt((double)(sqr(XJoystickState.lX) + sqr(XJoystickState.lY))));
				if(dy > 0)
					traction = -traction;
				rudder = -XJoystickState.lX*rudder_max >> 8;
				if(abs(traction) > 200)
					controls(CONTROLS::TURBO_QUANT);
				//if(abs(dx) > 200)
				//	controls(CONTROLS::HAND_BRAKE_QUANT);
				break;

			case JOYSTICK_SteeringWheel:
				traction = -XJoystickState.lY;
				rudder = -XJoystickState.lX*rudder_max >> 8;
				if(abs(dy) > 200)
					controls(CONTROLS::TURBO_QUANT);
				if(abs(dx) > 200)
					controls(CONTROLS::HAND_BRAKE_QUANT);
				break;
			}
}

void Object::import_controls()
{
	for(int i = 0;i < CONTROLS::NUMBER_OF_TRANSFERABLE_CONTROLS;i++)
		if(current_controls & (1 << i))
			controls(i);
}
/*******************************************************************************
			Main dynamic analysis
*******************************************************************************/
void Object::analysis()
{
	//dt0 = XTCORE_FRAME_DELTA/0.1286;
	if(analysis_off){
		test_objects_collision();
		update_coord();
		return;
		}
#ifdef SICHER_DEBUG	
	if(stop_flag){
		int aaa = 0;
		}
	if(fish_test){
		fish_analysis(1);
		update_coord();
		return;
		}
	if(skyfarmer_test){
		skyfarmer_analysis(1);
		update_coord();
		return;
		}
#endif

	if(active){
		prev_controls = current_controls;
		current_controls = 0;
		if(!aciKeyboardLocked){
			entries_control();
			if(!disable_control){
				direct_keyboard_control();
				/*if(JoystickMode)
					direct_joystick_control();*/
				}
			}
		}
	if(interpolation_on){
		import_controls();
		}

	if(old_appearance_storage){
		helicopter = 0;
		insect_analysis();
		update_coord();
		return;
		}

	switch(ID){
		case ID_VANGER:
			mechous_analysis(dt0);
			break;

		case ID_JUMPBALL:
		case ID_DEBRIS:
		case ID_STUFF:
		case ID_HORDE_SOURCE:
			debris_analysis(dt_debris);
			break;
		case ID_INSECT:
			insect_analysis();
			break;
		case ID_SKYFARMER:
			skyfarmer_analysis(1);
			break;

		case ID_FISH:
			fish_analysis(1);
			break;
		}

	update_coord();

#ifdef MSG_OUT
	if(active){
		msg_buf < "R: " <= R;
		msg_buf < "V: " <= V.vabs() < ";  " <= V;
		msg_buf < "W: " <= W.vabs() < ";  " <= W;
		//msg_buf < "BOX collision counter: " <= spring_touch < "\n";
		msg_buf < "terrain_immersion: " <= terrain_immersion < "\n";
		msg_buf < "water_immersion: " <= water_immersion < "\n";
		msg_buf < "Drags: " <= V_drag < ";  " <= W_drag < "\n";
		msg_buf < "Psi: " <= psi < "\n";
		msg_buf < "Scale_real: " <= scale_real < "\n";
		msg_buf < "Speed: " <= speed < "\n";
		//msg_buf < "Z: " <= z_axis;
		msg_buf.SetRadix(2);
		msg_buf < "State: " <= dynamic_state < "\n";
		msg_buf.SetRadix(10);
		msg_buf < "Speed_correction_factor: " <= speed_correction_factor < "\n";

		int z3 = HEIGHTS((int)R.x,(int)R.y);
		if(THREE_HEIGHTS(z3))
			msg_buf <= LOWER_HEIGHT(z3) < "  " <=  MIDDLE_HEIGHT(z3) < "  " <= UPPER_HEIGHT(z3) < "\n";
		else
			msg_buf <= LOWER_HEIGHT(z3) < "\n";
		msg_buf <= R.z + zmax_real < "\n";
		msg_buf <= radius < "; \t";
		msg_buf <= xmax_real < ", \t";
		msg_buf <= ymax_real < ", \t";
		msg_buf <= zmax_real < ";;\t";
		msg_buf <= m < "\n";
		msg_buf < "SoundFlag: " <= SoundFlag < "\n";
		}
#endif
}
  void Object::switch_analysis(int off)
{
	analysis_off = off;
	SoundFlag = 0;
	rudder = traction = 0;
	V = W = DBV();
}

/*******************************************************************************
		Mechous Analysis
*******************************************************************************/
//stalkerg Именно тут физика мехоса
DBV F_global,K_global;
void Object::mechous_analysis(double dt)
{
	int i;
	dt *= speed_correction_factor;
	if(Status & SOBJ_AUTOMAT){
		if(jump_power && ++jump_power > max_jump_power)
			jump();
	} else {
		if(jump_power && CheckStartJump(this)){
			jump();
			if(active)
				SOUND_KIDPUSH();
		}
	}

	// Global force calculation
	F_global = K_global = DBV();

	F_global.z -= g;

#ifndef NO_BORDER_FIELD
	if(WorldBorderEnable){
		int y = round(R.y);
		if(y < y_border_field)
			F_global.y += k_border_field/(y + 1);
		if(y > (int)map_size_y - y_border_field)
			F_global.y -= k_border_field/(map_size_y - y);
	}
#endif

	if(helicopter) {
		if(!UsingCopterig((helicopter << 8)/max_helicopter_height))
			controls(CONTROLS::HELICOPTER_DOWN);
		//if(air_speed_factor < 1 && --helicopter_time < 0)
		//	helicopter -= helicopter_height_decr;
		if(helicopter <= 0)
			 helicopter = 0;

		double heli_x_offset = rudder*heli_x_convert;
		double heli_y_offset = -traction*heli_y_convert;
		rudder = round(rudder*heli_rudder_decr); //heli
		traction = round(traction*heli_traction_decr);

		if(R.z - helicopter > 2*helicopter_height_decr && dynamic_state & TOUCH_OF_GROUND) {
			helicopter = 0;
			device_switch_latency = GET_DEVICE_LATENCY();
		}

		double dz = R.z;
		if(dz < 1)
			dz = 1;

		double fz = g*k_helicopter_thrust*pow(.5,sqr(dz/(double)helicopter))*(1 + helicopter_ampl*Sin(helicopter_thrust_phase += helicopter_dphi));
		double fy = -heli_y_offset*air_speed_factor*global_air_speed_factor;
		F_global.x += A_l2g[1]*fy;
		F_global.y += A_l2g[4]*fy;
		F_global.z += fz;

		double dx = heli_x_offset + helicopter_circle_radius_x*Cos(helicopter_circle_phase += helicopter_circle_dphi);
		double dy = heli_y_offset + helicopter_circle_radius_y*Sin(helicopter_circle_phase);
		K_global.x -= (A_l2g[3]*dx + A_l2g[4]*dy + A_l2g[5]*heli_z_offset)*fz;
		K_global.y += (A_l2g[0]*dx + A_l2g[1]*dy + A_l2g[2]*heli_z_offset)*fz;
		if(!helicopter_strife)
			K_global.z += heli_x_offset*k_helicopter_rotate;
		else{
			double fx = -heli_x_offset*air_speed_factor*k_helicopter_strife;
			F_global.x += A_l2g[0]*fx;
			F_global.y += A_l2g[3]*fx;
			}
	}

	A_g2l_old = A_g2l;
	R_old = R;
	for(i = 0;i < num_calls_analysis - 1;i++)
		basic_mechous_analysis(dt,0);
	basic_mechous_analysis(dt,1);
	if(test_objects_collision()){
//		A_g2l = A_g2l_old;
//		A_l2g = transpose(A_g2l);
//		R = R_old;
	}

	if(mole_on && !UsingCrotrig(mole_on) && !interpolation_on)
		controls(CONTROLS::MOLE_UP);

	if(in_water > 32)
		if(UsingCutterig(dynamic_state & TOUCH_OF_AIR))
			controls(CONTROLS::FLOTATION_UP);
		else
			controls(CONTROLS::FLOTATION_DOWN);
	else 
		archimedean = 0;

	if(abs(traction) > traction_decrement)
		if(traction > 0)
			traction -= traction_decrement;
		else
			traction += traction_decrement;
	else
		traction = 0;

	MoleInProcess = (mole_on && traction && dynamic_state & (GROUND_COLLISION | WHEELS_TOUCH)) ? 1 : 0;

	if(active) {

		// TerraMover
//		if(dynamic_state & WHEELS_TOUCH && traction){
//			Vector left_border,right_border;
//			if(traction > 0){
//				left_border = A_l2g*(DBV(-TerraMoverSx*xmax_real,TerraMoverSy*ymax_real,-TerraMoverSz*zmax_real)) + R;
//				right_border = A_l2g*(DBV(TerraMoverSx*xmax_real,TerraMoverSy*ymax_real,-TerraMoverSz*zmax_real)) + R;
//				}
//			else{
//				left_border = A_l2g*(DBV(TerraMoverSx*xmax_real,-TerraMoverSy*ymax_real,-TerraMoverSz*zmax_real)) + R;
//				right_border = A_l2g*(DBV(-TerraMoverSx*xmax_real,-TerraMoverSy*ymax_real,-TerraMoverSz*zmax_real)) + R;
//				}
//
//			terra_moving_tool.set(left_border, right_border,(A_l2g*V)*(dt*num_calls_analysis));
//			terra_moving_tool.make_dast();
//			}
		
		SoundFlag = 0;
		if(helicopter && air_speed_factor < 1)
			SoundFlag |= SoundCopterig;
		if(mole_on)
			SoundFlag |= SoundCrotrig;
		//TODO: if else hell, need test
		if(dynamic_state & TOUCH_OF_WATER) {
			if(dynamic_state & TOUCH_OF_AIR){
				if(traction && archimedean) {
					SoundFlag |= SoundCutterig;
				}
			} else {
				SoundFlag |= SoundUnderWater;
			}
		}

		if(!(SoundFlag & (~SoundUnderWater)) && traction)
			SoundFlag |= SoundMotor;
	}

	after_db_coll--;
	side_impulse_enable++;
	hand_brake = turbo = brake = 0;
	if(rudder && dynamic_state & WHEELS_TOUCH)
		rudder -= SIGN(rudder)*(abs(round(rudder*V.y*dt*num_calls_analysis*rudder_k_decr) + 1));
	speed = round(V.vabs()*dt*num_calls_analysis);
}

//stalkerg: а ещё важнее физика тут
void Object::basic_mechous_analysis(double dt,int last)
{
	int i;
	double f_traction = mobility_factor*global_mobility_factor;
	if(turbo)
		f_traction *= k_traction_turbo;
	f_traction *= double(traction)/64./(double)n_wheels;
	int num_bounds = bound -> num_poly;
	double f_archimedean = k_archimedean*double(archimedean)/256./double(num_bounds);
	//double f_brake = brake ? f_brake_max : 0;

	V_drag = V_drag_free*pow(V_drag_speed,V.vabs());
	W_drag = W_drag_free*pow(W_drag_speed,W.abs2());
	if(dynamic_state & WHEELS_TOUCH)
		V.y *= pow(1 + log(V_drag_wheel_speed)*mobility_factor*global_speed_factor/speed_factor,speed_correction_factor);

	DBV F,K;
	
	// Analysis of springs
	double dZ;
	DBV r,rg,rg0,vr,dF,n,vw;
	DBV z_axis = DBV(A_g2l[2],A_g2l[5],A_g2l[8]);
	double Vz = z_axis*V;
	DBV F_friction,K_friction;
	DBV F_springs,K_springs;
	int spring_touch = 0;
	int wheel_touch = 0;
	int float_cnt = 0;
	int xy_offset;
	unsigned char* p;
	DBV r_sum;
	int r_cnt = 0;
	int r_diff = 0;
	DBV rgf;

	Vector R_field;
	DBV F_field,K_field;
	unsigned char** LineT = vMap -> lineT;
	double modulation = 1;
	device_modulation = 1;
	if(closest_field_object){
		R_field = closest_field_object -> R_curr;
		double rf_x = getDistX((int)R.x,R_field.x);
		double rf_y = getDistY((int)R.y,R_field.y);
		switch(closest_field_object -> Type){
			case DangerTypeList::WHIRLPOOL:
				if(dynamic_state & TOUCH_OF_WATER){
					device_modulation = water_immersion < 20 ? 1 : (water_immersion < 200 ? (200 - water_immersion)/200 : 0);
					double kr2 = 1/(sqr(rf_x) + sqr(rf_y) + 1);
					double kr = sqrt(kr2);
					F_field += DBV((field_rotator*rf_y - field_tractor_xy*rf_x)*kr2,
							         (-field_rotator*rf_x - field_tractor_xy*rf_y)*kr2,
								    -field_tractor_z*kr);
					K_field.z += -field_rotator_moment*radius*kr;
					F += A_g2l*F_field;
					K += A_g2l*K_field;
					modulation = 0.5;
					}
				break;
			case DangerTypeList::TRAIN:
				if(dynamic_state & TOUCH_OF_WATER){
					device_modulation = water_immersion < 20 ? 1 : (water_immersion < 200 ? (200 - water_immersion)/200 : 0);
					double kr2 = 1/(sqr(rf_x) + sqr(rf_y) + 1);
					double kr = sqrt(kr2);
					F_field += DBV((train_field_rotator*rf_y - train_field_tractor_xy*rf_x)*kr2,
							         (-train_field_rotator*rf_x - train_field_tractor_xy*rf_y)*kr2,
								    -train_field_tractor_z*kr);
					K_field.z += -train_field_rotator_moment*radius*kr;
					F += A_g2l*F_field;
					K += A_g2l*K_field;
					modulation = 0.5;
					}
				break;
			case DangerTypeList::SWAMP:
			case DangerTypeList::FASTSAND:
				if(!mole_on){
					modulation = (sqr(rf_x) + sqr(rf_y))/sqr(closest_field_object -> radius);
					modulation = pow(modulation,modulation_power);
					if(modulation > 1)
						modulation = 1;
					device_modulation = terrain_immersion < 20 ? 1 : (terrain_immersion < 200 ? (200 - terrain_immersion)/200 : 0);
					if(modulation > device_modulation)
						modulation = device_modulation;
					double t = V_drag_swamp + (1 - V_drag_swamp)*device_modulation;
					V_drag *= t;
					//W_drag *= t;
					f_traction *= device_modulation;
					f_archimedean = 0;
					}
				break;
			}
		}

	if(mole_on){
		if(mole_on != 256){
			k_elastic_modulation = modulation = 1;
			if(terrain_immersion < 50){
				mole_on = 0;
				W = V = DBV();
				}
			else
				F_global.z += mole_emerging_fz;
			}
		else{
			k_elastic_modulation = modulation = k_elastic_mole;
			if(terrain_immersion < 900)
				F_global.z -= mole_submerging_fz;
			}

		V_drag *= V_drag_mole;
		W_drag *= V_drag_mole;
		F.y += underground_speed_factor*global_underground_speed_factor*(double)traction;
		K.z += (double)rudder*(double)radius*k_mole_rudder*underground_speed_factor; //krot
		K.x -= z_axis.y*(double)radius*K_mole;
		K.y += z_axis.x*(double)radius*K_mole;
		rudder = 0;
		}
	else
		if(modulation > 1 - DBL_EPS){ // for swamp only
			double t = 0.93 + (1 - 0.93)*(terrain_immersion < 50 ? 1 : (terrain_immersion < 200 ? (200 - terrain_immersion)/200 : 0));
			V_drag *= t;
			}

#ifdef MSG_OUT
	if(msg_log)
		msg_buf < "modulation: " <= modulation < "\n";
#endif

	int stand_on_wheels = z_axis.z > 0 && fabs(A_l2g.a[6]) < .70;
	dynamic_state &= TOUCH_OF_WATER	| TOUCH_OF_AIR;
	non_loaded_space = 0;
	terrain_immersion = 0;
	water_immersion = 0;

	A_convert_8 = Matrix(A_l2g*(scale_real*127));
	convert_offset = xy_offset = 126;
	for(i = 0;i < bound -> num_vert;i++)
		bound -> vertices[i].fast_convert();
	int x_offset = ((int)round(R.x) - xy_offset) & ~1;
	int y_offset = round(R.y) - xy_offset;

	terrain_analysis_flag = (Vz > 30 ? BREAKE_DBLEVEL_ENABLE : 0) | (Vz > 4 ? MIDDLE_LEVEL_PREFER : 0);
	//double scale_bound_real = scale_real;
	double scale_bound_real = scale_real*scale_bound;
	for(i = 0;i < num_bounds;i++){
		r = DBV(bound->variable_polygons[i].middle_x,bound -> variable_polygons[i].middle_y,bound -> variable_polygons[i].middle_z)
		    *scale_bound_real;
		rg0 = A_l2g*r;
		rg = rg0 + R;
		if(!mole_on){
			p = LineT[(int)round(rg.y) & clip_mask_y];
			if(p){
				if(GET_TERRAIN(*(p + (((int)rg.x & ~1) & clip_mask_x) + H_SIZE)) == WATER_TERRAIN){
					dZ = (double)FloodLEVEL - rg.z;
					if(dZ > 0){
						if(bound -> variable_polygons[i].color_id && archimedean){
							dF = z_axis*(f_archimedean*dZ);
							F += dF;
							K += r % dF;
							}
						float_cnt++;
						water_immersion += dZ;
						}
					}
				}
			}

		double X0,Y0,dZ0,X1,Y1,dZ1,df;
		int code, xy_vs_z;
		DBV n = DBV(Vector(bound -> variable_polygons[i].flat_normal)).get_norm(1.);
		double nz = DBV(Vector(bound -> variable_polygons[i].flat_normal)).get_norm(1.)*z_axis;
		if(nz < 0){ // lower average
			if((code = bound -> variable_polygons[i].lower_average(x_offset,y_offset,round(rg.z),X0,Y0,dZ0,X1,Y1,dZ1)) != 0){
				terrain_immersion += fabs(dZ0) + fabs(dZ1);
				if(!(code & 1)){
					X0 = X1;
					Y0 = Y1;
					dZ0 = dZ1;
					}
				vr = (V + W % r);
				xy_vs_z = sqr(vr.z) < sqr(vr.x) + sqr(vr.y);
				if((code & 2) && xy_vs_z){ // horisontal mirror
					DBV r1 = A_g2l*DBV(X1 - xy_offset,Y1 - xy_offset,0);
					DBV normal = (r1/DBV(xmax,ymax,zmax)).get_norm(1);
					DBV u0 = V + W % r1;
					if(u0 * normal > 0){
						dynamic_state |= VERTICAL_WALL_COLLISION;
						u0 = normal*(u0*normal);
						DBV P = (calc_collision_matrix(r1,J_inv).inverse()*u0)*(-horizontal_impulse_factor*modulation);
						V += P;
						W += J_inv*(r1 % P);
						if(P.vabs() > strong_ground_collision_threshould)
							dynamic_state |= STRONG_GROUND_COLLISION;
						}
					}
				else{ // vertical mirror
					DBV r1 = A_g2l*DBV(X0 - xy_offset,Y0 - xy_offset,(A_l2g*r).z);
					DBV u0 = V + W % r1;
					if(u0 * z_axis < 0){
						//if(!bound -> variable_polygons[i].color_id || nz > -normal_threshould)
						//	u0 = z_axis*(u0*z_axis); // wheel
						//else
							//u0 = u0*k_friction_impulse + z_axis*((u0*z_axis)*(1 - k_friction_impulse));
						//u0 = u0*k_friction_impulse + n*((u0*n)*(1 - k_friction_impulse));
						//if(bound -> variable_polygons[i].color_id < 2)
						//	u0.y = 0;
						//if(stand_on_wheels || bound -> variable_polygons[i].color_id < 2)
						if(stand_on_wheels)
							u0.x = u0.y = 0; // wheel
							//u0 = n*(u0*n); // wheel
						else
							u0 = u0*k_friction_impulse + n*((u0*n)*(1 - k_friction_impulse));
						DBV P = (calc_collision_matrix(r,J_inv).inverse()*u0)*(-vertical_impulse_factor*modulation);
						V += P;
						W += J_inv*(r % P);
						if(P.vabs() > strong_ground_collision_threshould)
							dynamic_state |= STRONG_GROUND_COLLISION;
						}
					}

				if(code & 5){
					if((df = k_elastic_spring*dZ0*modulation) > elastic_restriction)
						df = elastic_restriction;
					F_springs.z += df;
					K_springs.x -= rg0.y*df;
					K_springs.y += rg0.x*df;
					}

				r_sum += r;
				r_cnt++;
				r_diff++;
				//if(stand_on_wheels || bound -> variable_polygons[i].color_id > 2)
				if(stand_on_wheels)
					wheel_touch++;
				else
					spring_touch++;
				}
			}
		else{ // upper average
			if((code = bound -> variable_polygons[i].upper_average(x_offset,y_offset,round(rg.z),X0,Y0,dZ0,X1,Y1,dZ1)) != 0){
				terrain_immersion += fabs(dZ0) + fabs(dZ1);
				if(!(code & 1)){
					X0 = X1;
					Y0 = Y1;
					dZ0 = dZ1;
					}
				vr = (V + W % r);
				xy_vs_z = sqr(vr.z) < sqr(vr.x) + sqr(vr.y);
				if((code & 2) && xy_vs_z){ // horisontal mirror
					DBV r1 = A_g2l*DBV(X1 - xy_offset,Y1 - xy_offset,0);
					DBV normal = (r1/DBV(xmax,ymax,zmax)).get_norm(1);
					DBV u0 = V + W % r1;
					if(u0 * normal > 0){
						dynamic_state |= VERTICAL_WALL_COLLISION;
						u0 = normal*(u0*normal);
						DBV P = (calc_collision_matrix(r1,J_inv).inverse()*u0)*(-horizontal_impulse_factor*modulation);
						V += P;
						W += J_inv*(r1 % P);
						if(P.vabs() > strong_ground_collision_threshould)
							dynamic_state |= STRONG_GROUND_COLLISION;
						}
					}
				else{ // vertical mirror
					//DBV r1 = A_g2l*DBV(X0 - xy_offset,Y0 - xy_offset,0);
					DBV u0 = V + W % r;
					if(u0 * z_axis > 0){
						//u0 = z_axis*(u0*z_axis); 
						u0 = n*(u0*n);
						DBV P = (calc_collision_matrix(r,J_inv).inverse()*u0)*(-vertical_impulse_factor*modulation);
						V += P;
						W += J_inv*(r % P);
						if(P.vabs() > strong_ground_collision_threshould)
							dynamic_state |= STRONG_GROUND_COLLISION;
						}
					}

				if(code & 5){
					if((df = -k_elastic_spring*dZ0*modulation) < -2)
						df = -2;
					F_springs.z += df;
					K_springs.x -= rg0.y*df;
					K_springs.y += rg0.x*df;
					}

				r_sum -= r;
				r_cnt++;
				r_diff--;
				spring_touch++;
				}
			}
		}

	if(terrain_analysis_flag & BREAKE_DBLEVEL){
		destroy_double_level();
		F_springs = K_springs = DBV();
		}
	if(spring_touch | wheel_touch){
		if(active && dynamic_state & STRONG_GROUND_COLLISION)
			SOUND_SHOCK();
		r_sum /= r_cnt;
		if(last && !mole_on){
			Vector r = R + A_l2g*r_sum;
			if(get_upper_height(r.x,r.y) < r.z + 15)
				DUST_BY_BODY(r.x,r.y,speed/3,check_double_level(r.x,r.y));
			}
		dynamic_state |= GROUND_COLLISION;

		F_friction += A_g2l*F_springs;
		K_friction += A_g2l*K_springs;
		//dt *= 1./(1. + elastic_time_scale_factor*sqr(F_springs.abs2()));
		}
	in_water = (float_cnt << 8)/num_bounds;
	if(archimedean && traction){
		double dFy = device_modulation*(double)traction;
		double dFx = (double)(traction > 0 ? -rudder : rudder)*dFy*k_water_rudder; 
		dFy *= water_speed_factor*global_water_speed_factor;
		rudder = rudder*15/16;
		F.y += dFy;
		F.x += dFx;
		K.z -= traction > 0 ? (double)ymax_real*dFx : -(double)ymax_real*dFx;
		K.x += (double)zmax_real*dFy*(1./16.);
		}

	// Analysis of wheels
	k_track = 0;
	double df;
	if(wheel_touch && stand_on_wheels && !mole_on){
		for(i = 0;i < n_wheels;i++){
			r = wheels[i].r*scale_real;
			//r.z = -zmax_real;
			rg = A_l2g*r + R;
			dZ = 1;
			if(1){
				r.x = r.x > 0 ? xmax_real : -xmax_real;
				//r.z = 0;
				vw = V + W % r;
				//F.y += df = f_traction - vw.y*(brake || (!wheels[i].steer && hand_brake) ? f_brake_max : 0);
				F.y += df = f_traction;
				F -= vw*(brake || (!wheels[i].steer && hand_brake) ? f_brake_max : 0);
				//K += r % DBV(0,df,0);
				k_track = speed/3 + (abs(traction) >> 7) + turbo;

				if(wheels[i].steer){
					dynamic_state |= STEER_WHEEL_TOUCH;
					if(hand_brake && ((r.x > 0 && rudder > 0) ||  (r.x < 0 && rudder < 0))){
						k_track *= 2;
						goto wheel_continue;
						}
					}
				else{
					dynamic_state |= TRACTION_WHEEL_TOUCH;
					if(hand_brake){
						k_track *= 2;
						goto wheel_continue;
						}
					}
				if(after_db_coll <= 0){
					DBV normal = wheels[i].steer ? DBV(Cos(rudder),Sin(rudder),0) : DBV(1,0,0);
					DBV u0 = vw;
					u0 = normal*(u0*normal);
					DBV P = -(calc_collision_matrix(r,J_inv).inverse()*u0)*k_wheel;

					V += P				;// * (c_world == WORLD_WEEXOW ? 0.02:1);//ZMOD ICE. world now is unknown
					W += J_inv*(r % P)	;// * (c_world == WORLD_WEEXOW ? 0.02:1);//ZMOD ICE
					}

wheel_continue :
				if(dZ > 0 && wheels[i].dZ > 0 && last && get_upper_height(round(rg.x),round(rg.y)) < round(rg.z) + 15)
					WHEEL_TRACK((int)round(rg.x),(int)round(rg.y),k_track,check_double_level((int)round(rg.x),(int)round(rg.y)),i);
				wheels[i].dZ = dZ;
				}
			else
				wheels[i].dZ = 0;
			}
		}

	if(last && active)
		SOUND_MOTOR_PARAMETER(k_track);

	if(spring_touch || wheel_touch){
		F += F_friction;//ZNFO looks like fast sand
		K += K_friction;
		if(Vz < -10)
			V_drag *= pow(V_drag_z,-Vz);
		}

	if(spring_touch || wheel_touch || in_water)
		K -= DBV(0,0,(double)z_offset_of_mass_center*scale_real) % z_axis*g;


	V += (F + A_g2l*F_global)*dt;
	W += (J_inv*(K + A_g2l*K_global))*dt;

	if(non_loaded_space){
		V *= A_l2g;
		V.z = 0;
		V *= A_g2l;
		W *= A_l2g;
		W.x = W.y = 0;
		W *= A_g2l;
		}

	//  Drag parameters calculation
	if(spring_touch){
		  //V_drag *= pow(V_drag_spring,terrain_immersion);
		  V_drag *= V_drag_spring;
		  W_drag *= W_drag_spring;
		}

	if(helicopter){
		V_drag *= V_drag_helicopter;
		W_drag *= W_drag_helicopter;
		}

	if((wheel_touch || spring_touch) && in_water < 32 && V.vabs() < V_abs_min && W.vabs() < W_abs_min){
		#ifdef MSG_OUT
		if(msg_log)
			msg_buf < "Static friction\n";
		#endif
		V_drag *= pow(V_drag_coll,V_abs_min/(V.vabs() + 1e-10));
		W_drag *= pow(W_drag_coll,W_abs_min/(W.vabs() + 1e-10));
		}
#ifdef MSG_OUT
	else
		if(msg_log)
			msg_buf < "\n";
#endif

	if(in_water > 64){
		V_drag *= V_drag_float;
		W_drag *= W_drag_float;
		}

	if(!(V.vabs()*V_drag < V_abs_stop && W.vabs()*W_drag < W_abs_stop)){
		DBV Vs = V;
		if(r_diff > 0)
			Vs -= (z_axis*(radius*rolling_scale)) % W;
		else
			if(r_diff < 0)
				Vs += (z_axis*(radius*rolling_scale)) % W;
		R += (A_l2g*Vs)*dt;

		/*
		if(!r_diff)
			R += (A_l2g*V)*dt;
		else{
			DBV dV= A_l2g*(DBV(0,0,-radius*rolling_scale) % W);
			dV.z = 0;
			R += (A_l2g*V + (r_diff > 0 ? dV : -dV))*dt;
			}

		Q_l2g.rotate(W*dt); 
				
		A_l2g = DBM(Q_l2g);
		A_l2g[3] = -A_l2g[3];
		A_l2g[4] = -A_l2g[4];
		A_l2g[5] = -A_l2g[5];
		
		A_g2l = DBM(~Q_l2g);
		A_g2l[1] = -A_g2l[1];
		A_g2l[4] = -A_g2l[4];
		A_g2l[7] = -A_g2l[7];
		*/

		DBM A_rot_inv = DBM(W,W.vabs()*(-dt));
		A_g2l = A_rot_inv*A_g2l;
		A_l2g = transpose(A_g2l);
		V *= A_rot_inv;
		W *= A_rot_inv;
		}
/*
	if(interpolation_on){
		double t = correction_tau*(V.vabs() + correction_V0);
		if(t > 1)
			t = 1;
		DBV dR = dR_corr*t;
		R += dR;
		dR_corr -= dR;

		Quaternion dQ = Slerp(Quaternion(1,0,0,0), dQ_corr, t);
		dQ_corr %= dQ.inverse();
		DBM dM = DBM(1,-1,1,DIAGONAL)*DBM(dQ)*DBM(1,-1,1,DIAGONAL);
		A_l2g = dM*A_l2g;
		A_g2l = transpose(A_l2g);
		}
*/
	V *= pow(V_drag,speed_correction_factor);
	W *= pow(W_drag,speed_correction_factor);
}

/*******************************************************************************
		Debris and Stuff Analysis
*******************************************************************************/
void Object::debris_analysis(double dt)
{
	A_g2l_old = A_g2l;
	R_old = R;
	for(int i = 0;i < num_calls_analysis_debris - 1;i++)
		basic_debris_analysis(dt);
	basic_debris_analysis(dt);
	if(test_objects_collision()){
//		A_g2l = A_g2l_old;
//		A_l2g = transpose(A_g2l);
//		R = R_old;
		}
	after_db_coll--;
	speed = round(V.vabs()*dt*num_calls_analysis);
}

void Object::basic_debris_analysis(double dt)
{
	int i;
	V_drag = V_drag_stuff;
	W_drag = W_drag_free;
	double f_archimedean = f_archimedean_debris;

	DBV F,K;

	Vector R_field;
	DBV F_field,K_field;
	unsigned char** LineT = vMap -> lineT;
	double modulation = 1;
	if(closest_field_object){
		R_field = closest_field_object -> R_curr;
		double rf_x = getDistX((int)R.x,R_field.x);
		double rf_y = getDistY((int)R.y,R_field.y);
		switch(closest_field_object -> Type){
			case DangerTypeList::WHIRLPOOL:
			case DangerTypeList::TRAIN:
				if(dynamic_state & TOUCH_OF_WATER){
					device_modulation = water_immersion < 20 ? 1 : (water_immersion < 200 ? (200 - water_immersion)/200 : 0);
					double kr2 = 1/(sqr(rf_x) + sqr(rf_y) + 1);
					double kr = sqrt(kr2);
					F_field += DBV((field_rotator*rf_y - field_tractor_xy*rf_x)*kr2,
							         (-field_rotator*rf_x - field_tractor_xy*rf_y)*kr2,
								    -field_tractor_z*kr);
					K_field.z += -field_rotator_moment*radius*kr;
					F += A_g2l*F_field;
					K += A_g2l*K_field;
					modulation = 0.5;
					}
				break;
			case DangerTypeList::SWAMP:
			case DangerTypeList::FASTSAND:
				modulation = (sqr(rf_x) + sqr(rf_y))/sqr(closest_field_object -> radius);
				modulation = pow(modulation,modulation_power);
				if(modulation > 1)
					modulation = 1;
				device_modulation = terrain_immersion < 20 ? 1 : (terrain_immersion < 200 ? (200 - terrain_immersion)/200 : 0);
				if(modulation > device_modulation)
					modulation = device_modulation;
				double t = V_drag_swamp + (1 - V_drag_swamp)*device_modulation;
				V_drag *= t;
				//W_drag *= t;
				//f_traction *= device_modulation;
				f_archimedean = 0;
				break;
			}
		}

	if(modulation > 1 - DBL_EPS){
		double t = 0.93 + (1 - 0.93)*(terrain_immersion < 50 ? 1 : (terrain_immersion < 200 ? (200 - terrain_immersion)/200 : 0));
		V_drag *= t;
		}
	if(modulation > device_modulation)
		modulation = device_modulation;

	dynamic_state &= TOUCH_OF_WATER	| TOUCH_OF_AIR;
	non_loaded_space = 0;
	terrain_immersion = 0;
	water_immersion = 0;

	// Analysis of springs
	double dZ;
	DBV z_axis = DBV(A_g2l[2],A_g2l[5],A_g2l[8]);
	double Vz = z_axis*V;
	int spring_touch = 0;
	DBV r,rg,rg0,vr,dF,n;
	DBV F_springs,K_springs;
	in_water = 0;
	unsigned char* p = LineT[(int)round(R.y) & clip_mask_y];
	if(p)
		if(GET_TERRAIN(*(p + (((int)round(R.x) & ~1) & clip_mask_x) + H_SIZE))
		   == WATER_TERRAIN && (dZ = (double)FloodLEVEL - R.z) > 0){
			in_water = 256;
			if(archimedean)
				F += z_axis*(f_archimedean*dZ);
			}

	A_convert_8 = Matrix(A_l2g*(scale_real*127));
	int xy_offset = convert_offset = 126;
	for(i = 0;i < bound -> num_vert;i++)
		bound -> vertices[i].fast_convert();
	int x_offset = ((int)round(R.x) - xy_offset) & ~1;
	int y_offset = (int)round(R.y) - xy_offset;

	terrain_analysis_flag = (Vz > 30 ? BREAKE_DBLEVEL_ENABLE : 0) | (Vz > 4 ? MIDDLE_LEVEL_PREFER : 0);
	int num_bounds = bound -> num_poly;
	double scale_bound_real = scale_real*scale_bound;
	for(i = 0;i < num_bounds;i++){
		r = DBV(bound -> variable_polygons[i].middle_x,bound -> variable_polygons[i].middle_y,bound -> variable_polygons[i].middle_z)
		    *scale_bound_real;
		rg0 = A_l2g*r;
		rg = rg0 + R;

		double X0,Y0,dZ0,X1,Y1,dZ1,df;
		int code, xy_vs_z;
		DBV n = DBV(Vector(bound -> variable_polygons[i].flat_normal)).get_norm(1.);
		double nz = DBV(Vector(bound -> variable_polygons[i].flat_normal)).get_norm(1.)*z_axis;
		if(nz < 0){ // lower average
			if((code = bound -> variable_polygons[i].lower_average(x_offset,y_offset,round(rg.z),X0,Y0,dZ0,X1,Y1,dZ1)) != 0){
				terrain_immersion += fabs(dZ0) + fabs(dZ1);
				if(!(code & 1)){
					X0 = X1;
					Y0 = Y1;
					dZ0 = dZ1;
					}
				vr = (V + W % r);
				xy_vs_z = sqr(vr.z) < sqr(vr.x) + sqr(vr.y);
				if((code & 2) && xy_vs_z){ // horisontal mirror
					DBV r1 = A_g2l*DBV(X1 - xy_offset,Y1 - xy_offset,0);
					DBV normal = (r1/DBV(xmax,ymax,zmax)).get_norm(1);
					DBV u0 = V + W % r1;
					if(u0 * normal > 0){
						u0 = normal*(u0*normal);
						DBV P = (calc_collision_matrix(r1,J_inv).inverse()*u0)*(-horizontal_impulse_factor*modulation);
						V += P;
						W += J_inv*(r1 % P);
						}
					}
				else{ // vertical mirror
					DBV r1 = A_g2l*DBV(X0 - xy_offset,Y0 - xy_offset,(A_l2g*r).z);
					DBV u0 = V + W % r1;
					if(u0 * z_axis < 0){
						u0 = u0*k_friction_impulse + n*((u0*n)*(1 - k_friction_impulse));
						DBV P = (calc_collision_matrix(r,J_inv).inverse()*u0)*(-vertical_impulse_factor*modulation);
						V += P;
						W += J_inv*(r % P);
						}
					}

				if(code & 5){
					if((df = k_elastic_spring_debris*dZ0*modulation) > elastic_restriction)
						df = elastic_restriction;
					F_springs.z += df;
					K_springs.x -= rg0.y*df;
					K_springs.y += rg0.x*df;
					}

				spring_touch++;
				}
			}
		else{ // upper average
			if((code = bound -> variable_polygons[i].upper_average(x_offset,y_offset,round(rg.z),X0,Y0,dZ0,X1,Y1,dZ1)) != 0){
				terrain_immersion += fabs(dZ0) + fabs(dZ1);
				if(!(code & 1)){
					X0 = X1;
					Y0 = Y1;
					dZ0 = dZ1;
					}
				vr = (V + W % r);
				xy_vs_z = sqr(vr.z) < sqr(vr.x) + sqr(vr.y);
				if((code & 2) && xy_vs_z){ // horisontal mirror
					DBV r1 = A_g2l*DBV(X1 - xy_offset,Y1 - xy_offset,0);
					DBV normal = (r1/DBV(xmax,ymax,zmax)).get_norm(1);
					DBV u0 = V + W % r1;
					if(u0 * normal > 0){
						u0 = normal*(u0*normal);
						DBV P = (calc_collision_matrix(r1,J_inv).inverse()*u0)*(-horizontal_impulse_factor*modulation);
						V += P;
						W += J_inv*(r1 % P);
						}
					}
				else{ // vertical mirror
					//DBV r1 = A_g2l*DBV(X0 - xy_offset,Y0 - xy_offset,0);
					DBV u0 = V + W % r;
					if(u0 * z_axis > 0){
						//u0 = z_axis*(u0*z_axis); 
						u0 = n*(u0*n);
						DBV P = (calc_collision_matrix(r,J_inv).inverse()*u0)*(-vertical_impulse_factor*modulation);
						V += P;
						W += J_inv*(r % P);
						}
					}

				if(code & 5){
					if((df = -k_elastic_spring_debris*dZ0*modulation) < -2)
						df = -2;
					F_springs.z += df;
					K_springs.x -= rg0.y*df;
					K_springs.y += rg0.x*df;
					}

				spring_touch++;
				}
			}
		}

	if(spring_touch){
		dynamic_state |= GROUND_COLLISION;

		F += A_g2l*F_springs;
		K += A_g2l*K_springs;

		if(Vz < -10)
			V_drag *= pow(V_drag_z,-Vz);

		if(ID != ID_JUMPBALL){
			V_drag *= V_drag_spring_debris;
			W_drag *= W_drag_spring_debris;
			}
		else{
			V_drag *= V_drag_spring_jumpball;
			W_drag *= V_drag_spring_jumpball;
			}
		dt *= 1./(1. + elastic_time_scale_factor*sqr(F_springs.abs2()));
		}

#ifndef NO_BORDER_FIELD
	if(WorldBorderEnable){
		int y = round(R.y);
		if(y < y_border_field)
			F += A_g2l*DBV(0,k_border_field/(y + 1),0);
		if(y > (int)map_size_y - y_border_field)
			F += A_g2l*DBV(0,-k_border_field/(map_size_y - y),0);
		}
#endif

	F -= z_axis*g;

	V += F*dt;
	W += (J_inv*K)*dt;

	if(non_loaded_space){											
		V *= A_l2g;
		V.z = 0;
		V *= A_g2l;
		W *= A_l2g;
		W.x = W.y = 0;
		W *= A_g2l;
		}


	if(ID != ID_JUMPBALL && spring_touch && in_water < 32 && V.vabs() < V_abs_min && W.vabs() < W_abs_min){
		V_drag *= pow(V_drag_coll,V_abs_min/(V.vabs() + 1e-10));
		W_drag *= pow(W_drag_coll,W_abs_min/(W.vabs() + 1e-10));
		}

	if(in_water > 64){
		V_drag *= V_drag_float;
		W_drag *= W_drag_float;
		}

	if(!(ID != ID_JUMPBALL && V.vabs()*V_drag < V_abs_stop && W.vabs()*W_drag < W_abs_stop)){
		DBV Vs = V;
		if(spring_touch)
			Vs -= (z_axis*(radius*rolling_scale)) % W;
		R += (A_l2g*Vs)*dt;

		DBM A_rot_inv = DBM(W,W.vabs()*(-dt));
		A_g2l = A_rot_inv*A_g2l;
		A_l2g = transpose(A_g2l);
		V *= A_rot_inv;
		W *= A_rot_inv;
		}

	V *= V_drag;
	W *= W_drag;
}
void Object::set_ground_elastic(double k)
{
	device_modulation = k;
}

void Object::fish_analysis(double dt)
{
	V_drag = V_drag_float;
	W_drag = 0.85;

	double dZ_f,dZ_b,df;
	DBV F,K,h_factor_f,h_factor_b,dF;
	DBV z_axis = DBV(A_g2l[2],A_g2l[5],A_g2l[8]);
	dynamic_state &= TOUCH_OF_WATER	| TOUCH_OF_AIR;
	non_loaded_space = 0;
	DBV r = DBV(0,ymax_real/2,0);
	DBV rf = R + A_l2g*r;
	DBV rb = R - A_l2g*r;
	int D = xmax_real/2;
	int log_f = height_analysis_fish(round(rf.x),round(rf.y),rf.z - D,D,dZ_f,h_factor_f);
	int log_b = height_analysis_fish(round(rb.x),round(rb.y),rb.z - D,D,dZ_b,h_factor_b);
	if(log_f | log_b){
		dynamic_state |= GROUND_COLLISION;
		V_drag *= V_drag_spring_debris;
		W_drag *= W_drag_spring_debris;
		if(log_f & log_b){
			if((df = k_elastic_fish*dZ_f) > 2)
				df = 2;
			dF = z_axis*df;
			F += dF;
			K += r % dF;

			if((df = k_elastic_fish*dZ_b) > 2)
				df = 2;
			dF = z_axis*df;
			F += dF;
			K -= r % dF;
			}
		else{
			DBV R1 = log_f ? A_g2l*DBV(h_factor_f.x,h_factor_f.y,0) + r : A_g2l*DBV(h_factor_b.x,h_factor_b.y,0) - r;
			DBV normal = (R1/DBV(xmax,ymax,zmax)).get_norm(1);
			DBV u0 = V + W % R1;
			if(u0 * normal > 0){
				u0 = normal*(u0*normal);
				DBV P = (calc_collision_matrix(R1,J_inv).inverse()*u0)*(-2);
				V += P;
				W += J_inv*(R1 % P);
				}
			//F -= R1.get_norm(k_elastic_xy_fish);
			//K += DBV(0,1,0) % R1;
			}
		}

	if(non_loaded_space)
		return;

	F -= z_axis*g;
	if(dynamic_state & TOUCH_OF_WATER){
		int dz = FloodLEVEL - round(rf.z);
		if(dz > 0){
			F += dF = z_axis*(dz*k_archimedean_fish);
			K += (DBV(0,0,zmax_real/2) + r) % dF;
			}
		dz = FloodLEVEL - round(rb.z);
		if(dz > 0){
			F += dF = z_axis*(dz*k_archimedean_fish);
			K += (DBV(0,0,zmax_real/2) - r) % dF;
			}
		if(traction){
			K += A_g2l*DBV(0,0,(double)rudder*traction*k_rudder_fish); //fish
			rudder = rudder*15/16;
			F.y += traction*k_traction_fish;
			}
		}

	V += F*dt;
	W += (J_inv*K)*dt;
	
	A_g2l_old = A_g2l;
	R_old = R;

	R += A_l2g*V*dt;
	DBM A_rot_inv = DBM(W,W.vabs()*(-dt));
	A_g2l = A_rot_inv*A_g2l;
	A_l2g = transpose(A_g2l);
	V *= A_rot_inv;
	W *= A_rot_inv;

	if(test_objects_collision()){
		A_g2l = A_g2l_old;
		A_l2g = transpose(A_g2l);
		R = R_old;
		}

	V.y *= V_drag;
	V.x *= 0.7;
	V.z *= 0.2;
	W *= W_drag;
	speed = round(V.vabs());
}
void Object::insect_analysis()
{
	double f_traction = k_traction_insect*double(traction)/256.;

	dynamic_state = 0;
	in_water = 0;
	int yg;
	unsigned char* p = vMap -> lineT[yg = int(R.y) & clip_mask_y];
	if(p)
		if(GET_TERRAIN(*(p + (((int)R.x & ~1) & clip_mask_x) + H_SIZE))
		   == WATER_TERRAIN && FloodLEVEL > round(R.z)){
			in_water = 256;
			dynamic_state |= TOUCH_OF_WATER;
			}
	if(in_water != 256)
		dynamic_state |= TOUCH_OF_AIR;

	DBV n;
	double dZ = get_height(round(R.x),round(R.y),round(R.z) - radius,radius/3,n,z_below);
	double a;
	if(n.z < (a = sqrt(sqr(n.x) + sqr(n.y))))
		n.z = a;

	n.norm(1);

	DBV F = A_g2l*DBV(0,0,dZ*k_elastic_insect - g);
	F.y += f_traction;

	n *= A_g2l;
	DBV K = -n % DBV(0,0,K_insect);

	V += F;
	W += J_inv*K;

	V *= V_drag_insect;
	W *= W_drag_insect;

	R += A_l2g*V;
	double psi = V.y*sin(GTOR(rudder))/((double)ymax_real); //insect
	rudder -= SIGN(rudder)*RTOG(fabs(psi))/2;

	DBM A_rot_inv = DBM(-psi,Z_AXIS)*DBM(W,-(W.vabs()));
	A_g2l = A_rot_inv*A_g2l;
	A_l2g = transpose(A_g2l);
	V *= A_rot_inv;
	W *= A_rot_inv;

	test_objects_collision();
	speed = round(V.vabs());
}
/*******************************************************************************
		SKY FARMER's functions
*******************************************************************************/
void Object::skyfarmer_start(int x,int y,int angle)
{
	skyfarmer_fly_direction = 1;
	scale_size = 0;
	R = DBV(x,y,0);
	double dangle = GTOR(angle) - M_PI/2;
	A_l2g = DBM(1,-1,1,DIAGONAL)*DBM(dangle,Z_AXIS);
	A_g2l = DBM(-dangle,Z_AXIS)*DBM(1,-1,1,DIAGONAL);
}
void Object::skyfarmer_set_direction(const DBV& direction)
{
	skyfarmer_direction = direction.get_norm(1);
	skyfarmer_direction.z = 0;
}
void Object::skyfarmer_end()
{
	skyfarmer_fly_direction = -1;
}
void Object::skyfarmer_analysis(double dt)
{
	DBV F,K;
	switch(skyfarmer_fly_direction){
		case 1:
			if(scale_size < original_scale_size)
				scale_size += skyfarmer_scale_incr;
			else
				skyfarmer_fly_direction = 0;
			return;

		case 0:{
			double fz = skyfarmer_kFz*(skyfarmer_z - R.z);
			if(fz > 1.5)
				fz = 1.5;
			F += DBV(A_g2l[2],A_g2l[5],A_g2l[8])*(g*(fz - 1));

			if(R.z > skyfarmer_z - radius){
				DBV dir = A_g2l*skyfarmer_direction;
				F.y += skyfarmer_Fy;
				K += (DBV(0,skyfarmer_kKz*2,0) % dir) - (DBV(0,-skyfarmer_kKz,0) % dir);
				K.y += skyfarmer_kKy;
				}
			}
			break;

		case -1:
			F += DBV(A_g2l[2],A_g2l[5],A_g2l[8])*(-g);
			if((scale_size -= skyfarmer_scale_decr) < 0)
				scale_size = 0;
			break;
		}

	V += F*dt;
	W += (J_inv*K)*dt;
	R += (A_l2g*V)*dt;
	DBM A_rot_inv = DBM(W,W.vabs()*(-dt));
	A_g2l = A_rot_inv*A_g2l;
	A_l2g = transpose(A_g2l);
	V *= A_rot_inv;
	W *= A_rot_inv;
	V *= V_drag_skyfarmer;
	W *= W_drag_skyfarmer;
	speed = round(V.vabs());
}

int Object::check_terrain_type(int terrain_type)
{
	int i;
	if(!bound)
		return 0;
	A_convert_8 = Matrix(A_l2g*(scale_real*127));
	int xy_offset = convert_offset = 126;
	for(i = 0;i < bound -> num_vert;i++)
		bound -> vertices[i].fast_convert();
	int x_offset = ((int)round(R.x) - xy_offset) & ~1;
	int y_offset = (int)round(R.y) - xy_offset;

	double scale_bound_real = scale_real*scale_bound;
	int num_bounds = bound -> num_poly;
	DBV r;
	for(i = 0;i < num_bounds;i++){
		r = (DBV(bound -> variable_polygons[i].middle_x,bound -> variable_polygons[i].middle_y,bound -> variable_polygons[i].middle_z)
		    *scale_bound_real)*A_l2g + R;
		if(bound -> variable_polygons[i].check_terrain_type(x_offset,y_offset,round(r.z),terrain_type))
			return 1;
		}	
	return 0;
}

int Object::check_mole_submerging()
{
	int i;
	if(!bound)
		return 0;
	A_convert_8 = Matrix(A_l2g*(scale_real*127));
	int xy_offset = convert_offset = 126;
	for(i = 0;i < bound -> num_vert;i++)
		bound -> vertices[i].fast_convert();
	int x_offset = ((int)round(R.x) - xy_offset) & ~1;
	int y_offset = (int)round(R.y) - xy_offset;

	double scale_bound_real = scale_real*scale_bound;
	int num_bounds = bound -> num_poly;
	DBV r;
	for(i = 0;i < num_bounds;i++){
		r = (DBV(bound -> variable_polygons[i].middle_x,bound -> variable_polygons[i].middle_y,bound -> variable_polygons[i].middle_z)
		    *scale_bound_real)*A_l2g + R;
		if(!bound -> variable_polygons[i].check_mole_submerging(x_offset,y_offset,round(r.z)))
			return 0;
		}	
	return 1;
}


/*******************************************************************************
			Precise Impulse
*******************************************************************************/
double Lambert_W(double x)
{
	double y = -1;
	for(int i = 0;i < 20;i++)
		y = x*exp(-y);
	return y;
}

void Object::precise_impulse(Vector source_point,int x_dest,int y_dest)
{
	double k = V_drag_stuff;
	double dt = ID & ID_VANGER ? dt0 : dt_debris;
	double gdt2 = g*dt*dt;
	double v0dt = 0;
	double V0x,V0y,N;

	double dx = getDistX(x_dest,source_point.x);
	double dy = getDistY(y_dest,source_point.y);
	int z_dest;
	unsigned char* p = vMap -> lineT[y_dest & clip_mask_y];
	if(!p)
		z_dest = 127;
	else
		z_dest = p[(x_dest | 1) & clip_mask_x];
	double dz = source_point.z - z_dest;
	if(dz < 0){
		source_point.z = z_dest + 80;
		dz = 80;
		}

	{
	double t1 = log(k);
        double t2 = k*k;
        double t4 = v0dt*k;
        double t8 = 1/gdt2;
        double t10 = 1/(k-1.0);
        double t23 = t1*t8;
        double t24 = t10*dz;
        double t27 = t10*v0dt;
        N = -(t1+Lambert_W(-exp(-(dz*t2+t4-2.0*dz*k+gdt2+dz-v0dt)*t1*t8*t10)*t1*(gdt2*k+t4-v0dt)*t8/k*t10)+t23*t24*t2+t23*t27*k-2.0*t23*t24*k+t1*t10+t23*t24-t23*t27)/t1;
	}

	{
	double t3 = pow(k,N+1.0);
	double t6 = 1/(k-1.0);
	V0x = -dx/(-1/k*t3*t6+t6)/dt;
	}

	{
	double t3 = pow(k,N+1.0);
	double t6 = 1/(k-1.0);
	V0y = -dy/(-1/k*t3*t6+t6)/dt;
	}

	R = source_point;
	V = A_g2l*DBV(V0x,V0y,0);
}

/*******************************************************************************
		BOX TO BOX COLLISION TEST
*******************************************************************************/
DBV box_to_box_intersection(DBV RR,DBV X,DBV Y,DBV Z,double sx,double sy,double sz)
{
	int i;
	DBV R1(0,0,0),R2 = RR;
	double x,y,z;
	for(i = 0;i < 12;i++){
		x = sx*RR.x;
		y = sy*RR.y;
		z = sz*RR.z;
		if(fabs(x) > fabs(y))
			if(fabs(x) > fabs(z)){
				sx /= 2;
				R1.x += x > 0 ? sx : -sx;
				}
			else{
				sz /= 2;
				R1.z += z > 0 ? sz : -sz;
				}
		else
			if(fabs(y) > fabs(z)){
				sy /= 2;
				R1.y += y > 0 ? sy : -sy;
				}
			else{
				sz /= 2;
				R1.z += z > 0 ? sz : -sz;
				}

		x = X*RR;
		y = Y*RR;
		z = Z*RR;
		if(fabs(x) > fabs(y))
			if(fabs(x) > fabs(z)){
				X /= 2;
				R2 -= x > 0 ? X : -X;
				}
			else{
				Z /= 2;
				R2 -= z > 0 ? Z : -Z;
				}
		else
			if(fabs(y) > fabs(z)){
				Y /= 2;
				R2 -= y > 0 ? Y : -Y;
				}
			else{
				Z /= 2;
				R2 -= z > 0 ? Z : -Z;
				}
		RR = R2 - R1;
		}
	return R1;
}
/*
int obb_disjoint(double B[3][3], double T[3], double a[3], double b[3]);

This is a test between two boxes, box A and box B.  It is assumed that
the coordinate system is aligned and centered on box A.  The 3x3
matrix B specifies box B's orientation with respect to box A.
Specifically, the columns of B are the basis vectors (axis vectors) of
box B.	The center of box B is located at the vector T.  The
dimensions of box B are given in the array b.  The orientation and
placement of box A, in this coordinate system, are the identity matrix
and zero vector, respectively, so they need not be specified.  The
dimensions of box A are given in array a.

This test operates in two modes, depending on how the library is
compiled.  It indicates whether the two boxes are overlapping, by
returning a boolean.

The second version of the routine will return a conservative bounds on
the distance between the polygon sets which the boxes enclose.	It is
used when RAPID is being used to estimate the distance between two
models.
*/
int obb_disjoint(const DBM&  B, const DBV& T, double ax, double ay, double az, double bx, double by, double bz)
{
  double t, s;
  int r;
  double Bf[9];
  const double reps = 1e-6;

  // Bf = fabs(B.a)
  Bf[0] = fabs(B.a[0]);  Bf[0] += reps;
  Bf[1] = fabs(B.a[1]);  Bf[1] += reps;
  Bf[2] = fabs(B.a[2]);  Bf[2] += reps;
  Bf[3] = fabs(B.a[3]);  Bf[3] += reps;
  Bf[4] = fabs(B.a[4]);  Bf[4] += reps;
  Bf[5] = fabs(B.a[5]);  Bf[5] += reps;
  Bf[6] = fabs(B.a[6]);  Bf[6] += reps;
  Bf[7] = fabs(B.a[7]);  Bf[7] += reps;
  Bf[8] = fabs(B.a[8]);  Bf[8] += reps;


  // if any of these tests are one-sided, then the polyhedra are disjoint
  r = 1;

  // A1 x A2 = A0
  t = fabs(T.x);

  r &= (t <=
	  (ax + bx * Bf[0] + by * Bf[1] + bz * Bf[2]));
  if (!r) return 1;

  // B1 x B2 = B0
  s = T.x*B.a[0] + T.y*B.a[3] + T.z*B.a[6];
  t = fabs(s);

  r &= ( t <=
	  (bx + ax * Bf[0] + ay * Bf[3] + az * Bf[6]));
  if (!r) return 2;

  // A2 x A0 = A1
  t = fabs(T.y);

  r &= ( t <=
	  (ay + bx * Bf[3] + by * Bf[4] + bz * Bf[5]));
  if (!r) return 3;

  // A0 x A1 = A2
  t = fabs(T.z);

  r &= ( t <=
	  (az + bx * Bf[6] + by * Bf[7] + bz * Bf[8]));
  if (!r) return 4;

  // B2 x B0 = B1
  s = T.x*B.a[1] + T.y*B.a[4] + T.z*B.a[7];
  t = fabs(s);

  r &= ( t <=
	  (by + ax * Bf[1] + ay * Bf[4] + az * Bf[7]));
  if (!r) return 5;

  // B0 x B1 = B2
  s = T.x*B.a[2] + T.y*B.a[5] + T.z*B.a[8];
  t = fabs(s);

  r &= ( t <=
	  (bz + ax * Bf[2] + ay * Bf[5] + az * Bf[8]));
  if (!r) return 6;

  // A0 x B0
  s = T.z * B.a[3] - T.y * B.a[6];
  t = fabs(s);

  r &= ( t <=
	(ay * Bf[6] + az * Bf[3] +
	 by * Bf[2] + bz * Bf[1]));
  if (!r) return 7;

  // A0 x B1
  s = T.z * B.a[4] - T.y * B.a[7];
  t = fabs(s);

  r &= ( t <=
	(ay * Bf[7] + az * Bf[4] +
	 bx * Bf[2] + bz * Bf[0]));
  if (!r) return 8;

  // A0 x B2
  s = T.z * B.a[5] - T.y * B.a[8];
  t = fabs(s);

  r &= ( t <=
	  (ay * Bf[8] + az * Bf[5] +
	   bx * Bf[1] + by * Bf[0]));
  if (!r) return 9;

  // A1 x B0
  s = T.x * B.a[6] - T.z * B.a[0];
  t = fabs(s);

  r &= ( t <=
	  (ax * Bf[6] + az * Bf[0] +
	   by * Bf[5] + bz * Bf[4]));
  if (!r) return 10;

  // A1 x B1
  s = T.x * B.a[7] - T.z * B.a[1];
  t = fabs(s);

  r &= ( t <=
	  (ax * Bf[7] + az * Bf[1] +
	   bx * Bf[5] + bz * Bf[3]));
  if (!r) return 11;

  // A1 x B2
  s = T.x * B.a[8] - T.z * B.a[2];
  t = fabs(s);

  r &= (t <=
	  (ax * Bf[8] + az * Bf[2] +
	   bx * Bf[4] + by * Bf[3]));
  if (!r) return 12;

  // A2 x B0
  s = T.y * B.a[0] - T.x * B.a[3];
  t = fabs(s);

  r &= (t <=
	  (ax * Bf[3] + ay * Bf[0] +
	   by * Bf[8] + bz * Bf[7]));
  if (!r) return 13;

  // A2 x B1
  s = T.y * B.a[1] - T.x * B.a[4];
  t = fabs(s);

  r &= ( t <=
	  (ax * Bf[4] + ay * Bf[1] +
	   bx * Bf[8] + bz * Bf[6]));
  if (!r) return 14;

  // A2 x B2
  s = T.y * B.a[2] - T.x * B.a[5];
  t = fabs(s);

  r &= ( t <=
	  (ax * Bf[5] + ay * Bf[2] +
	   bx * Bf[7] + by * Bf[6]));
  if (!r) return 15;

  return 0;  // should equal 0
}

/*******************************************************************************
		OBJECT TO OBJECT COLLISION TEST
*******************************************************************************/
int Object::test_object_to_baseobject(BaseObject* bobj)
{
	if(sqr(getDistX(bobj -> R_curr.x,R_curr.x)) + sqr(getDistY(bobj -> R_curr.y,R_curr.y)) + sqr(bobj -> R_curr.z - R_curr.z) < sqr(radius + bobj -> radius)){
		Object* obj = (Object*)bobj;
		if((ID | obj -> ID) & SPHERICAL_TEST){
			if((ID == ID_INSECT || (ID == ID_VANGER && old_appearance_storage)) && obj -> ID == ID_VANGER){
				if(obj -> test_wheels_to_sphere(this)){
					Touch(obj);
					}
				return 0;
				}
			if((obj -> ID == ID_INSECT || (obj -> ID == ID_VANGER && obj -> old_appearance_storage)) && ID == ID_VANGER){
				if(test_wheels_to_sphere(obj)){
					obj -> Touch(this);
					}
				return 0;
				}
			dynamic_state |= OBJECT_TO_OBJECT_COLLISION;
			obj -> dynamic_state |= OBJECT_TO_OBJECT_COLLISION;
			Touch(obj);
			obj -> Touch(this);
			return 0;
			}

		DBV RR = A_g2l*DBV(getDistX(obj -> R.x,R.x),getDistY(obj -> R.y,R.y),obj -> R.z - R.z);
		DBM A_t2o = obj -> A_g2l*A_l2g;
		double x1 = xmax_real*scale_box;
		double y1 = ymax_real*scale_box;
		double z1 = zmax_real*scale_box;
		double x2 = obj -> xmax_real*obj -> scale_box;
		double y2 = obj -> ymax_real*obj -> scale_box;
		double z2 = obj -> zmax_real*obj -> scale_box;
		if(obb_disjoint(A_t2o,RR,x1,y1,z1,x2,y2,z2)){
			return 0;
			}

		dynamic_state |= OBJECT_TO_OBJECT_COLLISION;
		obj -> dynamic_state |= OBJECT_TO_OBJECT_COLLISION;

		Touch(obj);
		obj -> Touch(this);

		//if(analysis_off || obj -> analysis_off || (after_db_coll > 0 && obj -> after_db_coll > 0))
		if(analysis_off || obj -> analysis_off)
			return 1;

		collision_object = obj;
		obj -> collision_object = this;
		int collision_again_log = (after_db_coll > 0 || obj -> after_db_coll > 0);
		if(ID == ID_VANGER && obj -> ID == ID_VANGER)
			after_db_coll = obj -> after_db_coll = 2;

		DBM A_o2t = A_g2l*obj -> A_l2g;

		DBV R1 = box_to_box_intersection(RR,A_t2o*DBV(x2,0,0),A_t2o*DBV(0,y2,0),A_t2o*DBV(0,0,z2),x1,y1,z1);
		DBV R2 = R1 - RR;
		double m1 = m;
		double m2 = obj -> m;
		DBV u01 = V + W % R1;
		DBV u02 = A_o2t*obj -> V + (A_o2t*obj -> W) % R2;
		DBV u0 = (u01 - u02);
		if(u0 * RR <= 0)
			return 0;
		DBV normal = RR.get_norm(1);
		u0 = normal*(u0*normal);
		DBM K = calc_collision_matrix(R1,J_inv)/m1 + calc_collision_matrix(R2,A_o2t*obj -> J_inv*A_t2o)/m2;
		DBV P = (K.inverse()*u0)*(-2);

		//double df = fmina(fmina(R1.x - x1,R1.y - y1,R1.z - z1),fmina(R1.x + x1,R1.y + y1,R1.z + z1))*k_elastic_db_coll;
		//P -= RR.get_norm(df);

		V += P/m1;
		W += (J_inv/m1)*(R1 % P);

		obj -> V -= A_t2o*(P/m2);
		obj -> W -= (obj -> J_inv/m2)*A_t2o*(R2 % P);

		if(ID != ID_VANGER || obj -> ID != ID_VANGER || collision_again_log)
			return 1;

		double Pabs = P.vabs();
		if(Pabs < strong_double_collision_threshould)
			return 1; 
		//fout <= k_destroy_level*Pabs/m2 < "\t";
		//fout <= k_destroy_level*Pabs/m1 < "\n";
		if(active || obj -> active)
			SOUND_OBJECT_TO_OBJECT_COLLISION();

		int defense1,defense2;
		int ram_power1,ram_power2;
		double n1x  = R1.x/x1;
		double n1y  = R1.y/y1;
		double n1z  = R1.z/z1;
		if(fabs(n1y) > fabs(n1x))
			if(fabs(n1y) > fabs(n1z))
				if(n1y > 0){
					defense1 = FrontDefense;
					ram_power1 = FrontRamPower;
					}
				else{
					defense1 = BackDefense;
					ram_power1 = BackRamPower;
					}
			else
				if(n1z > 0){
					defense1 = UpperDefense;
					ram_power1 = UpperRamPower;
					}
				else{
					defense1 = LowerDefense;
					ram_power1 = LowerRamPower;
					}
		else
			if(fabs(n1x) > fabs(n1z)){
				defense1 = SideDefense;
				ram_power1 = SideRamPower;
				}
			else
				if(n1z > 0){
					defense1 = UpperDefense;
					ram_power1 = UpperRamPower;
					}
				else{
					defense1 = LowerDefense;
					ram_power1 = LowerRamPower;
					}

		R2 *= A_t2o;
		double n2x  = R2.x/x2;
		double n2y  = R2.y/y2;
		double n2z  = R2.z/z2;
		if(fabs(n2y) > fabs(n2x))
			if(fabs(n2y) > fabs(n2z))
				if(n2y > 0){
					defense2 = obj -> FrontDefense;
					ram_power2 = obj -> FrontRamPower;
					}
				else{
					defense2 = obj -> BackDefense;
					ram_power2 = obj -> BackRamPower;
					}
			else
				if(n2z > 0){
					defense2 = obj -> UpperDefense;
					ram_power2 = obj -> UpperRamPower;
					}
				else{
					defense2 = obj -> LowerDefense;
					ram_power2 = obj -> LowerRamPower;
					}
		else
			if(fabs(n2x) > fabs(n2z)){
				defense2 = obj -> SideDefense;
				ram_power2 = obj -> SideRamPower;
				}
			else
				if(n2z > 0){
					defense2 = obj -> UpperDefense;
					ram_power2 = obj -> UpperRamPower;
					}
				else{
					defense2 = obj -> LowerDefense;
					ram_power2 = obj -> LowerRamPower;
					}

		Pabs *= k_destroy_level;
		double k_size = sqr(obj -> scale_size*original_scale_size/(obj -> original_scale_size*scale_size));
		int level1 = round(Pabs*(ram_power2 << 16)*k_size/(double)defense1);
		int level2 = round(Pabs*(ram_power1 << 16)/((double)defense2*k_size));
#ifdef _FOUT_
		extern int frame;
		fout <= frame < "\t";
		fout <= Pabs < "\t";
		fout <= ram_power1 < "\t";
		fout <= ram_power2 < "\t";
		fout <= defense1 < "\t";
		fout <= defense2 < "\t";
		fout <= (double)level1/65536 < "\t";
		fout <= (double)level2/65536 < "\n";
#endif
		DestroyCollision(level1,obj);
		obj -> DestroyCollision(level2,this);
		return 1;
		}
	return 0;
}

int Object::test_wheels_to_sphere(Object* obj)
{
	if(fabs(V.y) < 2)
		return 0;
	DBV r = A_g2l*(obj -> R - R);
	r /= scale_real;
	int obj_rmax = round(obj -> radius*.25/scale_real);
	for(int i = 0;i < n_wheels;i++)
		if((wheels[i].r - r).vabs() < wheels[i].width + obj_rmax)
			return 1;
	return 0;
}

void Object::Explosion(int m_TNT,int m_id)
{
	// location in global coord system, m_TNT - 0..256

	int i;
	Object* obj;
	DBV r;
	double m_debris = m/(n_debris*5);
	for(i = 0;i < n_debris;i++){
		if((obj = DebrisD.CreateDebris()) == 0) return;
		*obj = *this;
		obj -> active = 0;
		obj -> m = density*debris[i].volume*pow(scale_size,2);
		obj -> J_inv = (debris[i].J*(pow(scale_size,2)/debris[i].volume)).inverse();
		obj -> J_inv = obj -> J_inv*(obj -> m/m_debris);
		obj -> m = m_debris;
		obj -> k_archimedean = 0;
		obj -> body_color_shift++;
		obj -> xmax = debris[i].xmax;
		obj -> ymax = debris[i].ymax;
		obj -> zmax = debris[i].zmax;
		obj -> rmax = debris[i].rmax;
		obj -> radius = radius;
		obj -> n_models = 1;
		obj -> models = obj -> model = &debris[i];
		obj -> bound = &bound_debris[i];
		obj -> n_wheels = 0;
		obj -> n_debris = 0;
		r = DBV(debris[i].x_off,debris[i].y_off,debris[i].z_off);
		obj -> R = R + A_l2g*r;
		obj -> V = DBV(random(2*m_TNT) - m_TNT,random(2*m_TNT) - m_TNT,random(2*m_TNT) - m_TNT)*V_explosion;
		obj -> W = DBV((double)random(256)*W_explosion,(double)random(256)*W_explosion,(double)random(256)*W_explosion);
		obj -> update_coord();
		((DebrisObject*)(obj))->CreateDebris(m_id,i);
	}
}

/*******************************************************************************
			Network IN and OUT  functions
*******************************************************************************/
const double Rz_pack_min = 0.5;
const double Rz_pack_max = 550;
const double V_pack_min = 0.0001;
const double V_pack_max = 100;
const double W_pack_min = 0.0001;
const double W_pack_max = 50;
const double Q_pack_min = 0.0001;
const double Q_pack_max = 2;
unsigned char pack(double x,double min,double max)
{
	int sign = 0;
	if(x < 0){
		x = -x;
		sign = 128;
		}
	if(x < min)
		return 0;
	if(x > max)
		x = max;
	double t1 = 1/min;
	double t2 = t1*max;
	double t5 = pow(t2,1.0/126.0);
	int a = round(126.0*log(t5*t1*x)/log(t2));
	return a | sign;
}
double unpack(unsigned char a,double min,double max)
{
	if(!a)
		return 0;
	double x = (int)(a & 127);
	double t4 = pow(max/min ,(x - 1.)/126.);
	x = t4*min;
	if(a & 128)
		return -x;
	else
		return x;
}

unsigned short pack_short(double x,double min,double max)
{
	int sign = 0;
	if(x < 0){
		x = -x;
		sign = 1 << 15;
		}
	if(x < min)
		return 0;
	if(x > max)
		x = max;
	double t1 = 1/min;
	double t2 = t1*max;
	double t5 = pow(t2,1.0/(double)((1 << 15) - 2));
	int a = round((double)((1 << 15) - 2)*log(t5*t1*x)/log(t2));
	return a | sign;
}
double unpack_short(unsigned short a,double min,double max)
{
	if(!a)
		return 0;
	double x = (int)(a & ((1 << 15) - 1));
	double t4 = pow(max/min ,(x - 1.)/(double)((1 << 15) - 2));
	x = t4*min;
	if(a & (1 << 15))
		return -x;
	else
		return x;
}

void Object::NetEvent(int type,int id,int creator,int time,int x,int y,int radius)
{
	if(analysis_off)
		analysis_off = 0;
	current_controls = events_in.get_short();
	mole_on = current_controls & (1 << CONTROLS::NUMBER_OF_TRANSFERABLE_CONTROLS) ? 256 : 0;
	traction = round(unpack(events_in.get_byte(),Rz_pack_min, Rz_pack_max));
	rudder = round(unpack(events_in.get_byte(),Rz_pack_min, Rz_pack_max));
	helicopter = round(unpack(events_in.get_byte(),Rz_pack_min, Rz_pack_max));

	DBV R_real;
	R_real.x = x;
	R_real.y = y;
	R_real.z = events_in.get_short();

	V.x = unpack_short(events_in.get_word(),V_pack_min,V_pack_max);
	V.y = unpack_short(events_in.get_word(),V_pack_min,V_pack_max);
	V.z = unpack_short(events_in.get_word(),V_pack_min,V_pack_max);

	W.x = unpack_short(events_in.get_word(),W_pack_min,W_pack_max);
	W.y = unpack_short(events_in.get_word(),W_pack_min,W_pack_max);
	W.z = unpack_short(events_in.get_word(),W_pack_min,W_pack_max);
	 
	Quaternion Q_real;

	Q_real.x = unpack_short(events_in.get_word(),Q_pack_min,Q_pack_max);
	Q_real.y = unpack_short(events_in.get_word(),Q_pack_min,Q_pack_max);
	Q_real.z = unpack_short(events_in.get_word(),Q_pack_min,Q_pack_max);
	Q_real.w = sqrt(1 - sqr(Q_real.x) - sqr(Q_real.y) - sqr(Q_real.z));

	//GetVisible();
	if(!interpolation_on || Visibility == UNVISIBLE){
		interpolation_on = 1;
		dR_corr = DBV(0,0,0);
		dQ_corr = Quaternion(1,0,0,0);
		R = R_real;
		A_l2g = DBM(1,-1,1,DIAGONAL)*DBM(Q_real);
		A_g2l = transpose(A_l2g);
		update_coord();
		GetVisible();
		return;
		}
	DBV R_t = R + dR_corr;
	dR_corr = -DBV(getDistX(R_real.x,R_t.x),getDistY(R_real.y,R_t.y),R_real.z - R_t.z);
	Quaternion Q_t(DBM(dQ_corr)*DBM(1,-1,1,DIAGONAL)*A_l2g);
	if(Q_real*Q_t < 0)
		Q_t = -Q_t;
	dQ_corr = Q_t % Q_real.inverse();
	
	R = R_real;
	A_l2g = DBM(1,-1,1,DIAGONAL)*DBM(Q_real);
	A_g2l = transpose(A_l2g);

	//update_coord();
	//speed = round(V.vabs());
}
void Object::Send(void)
{
	events_out < (short)(current_controls | (mole_on ? 1 << CONTROLS::NUMBER_OF_TRANSFERABLE_CONTROLS : 0));
	events_out < pack(traction,Rz_pack_min,Rz_pack_max);
	events_out < pack(rudder,Rz_pack_min,Rz_pack_max);
	events_out < pack(helicopter,Rz_pack_min,Rz_pack_max);
	
	events_out < (short)round(R.z);
	events_out < pack_short(V.x,V_pack_min,V_pack_max) < pack_short(V.y,V_pack_min,V_pack_max) < pack_short(V.z,V_pack_min,V_pack_max);
	events_out < pack_short(W.x,W_pack_min,W_pack_max) < pack_short(W.y,W_pack_min,W_pack_max) < pack_short(W.z,W_pack_min,W_pack_max);

	Quaternion q(DBM(1,-1,1,DIAGONAL)*A_l2g);
	events_out < pack_short(q.x,Q_pack_min,Q_pack_max) < pack_short(q.y,Q_pack_min,Q_pack_max) < pack_short(q.z,Q_pack_min,Q_pack_max);

/*
	fout < "V: " <= V.y < "\t" <= unpack_short(pack_short(V.y,V_pack_min,V_pack_max),V_pack_min,V_pack_max) < "\n";
	fout < "W: " <= W.y < "\t" <= unpack_short(pack_short(W.y,W_pack_min,W_pack_max),W_pack_min,W_pack_max) < "\n";
	fout < "Q: " <= q.y < "\t" <= unpack_short(pack_short(q.y,Q_pack_min,Q_pack_max),Q_pack_min,Q_pack_max) < "\n";
*/
}

DBV interpolation(double t,double x[3],DBV y[3])
{
	if(fabs(x[1] - x[2]) < DBL_EPS){
		if(fabs(x[0] - x[1]) < DBL_EPS)
			return y[0];
		return (y[0] - y[1])*((t - x[1])/(x[0] - x[1]));
		}
	if(fabs(x[0] - x[1]) < DBL_EPS)
		return (y[1] - y[2])*((t - x[2])/(x[1] - x[2]));

      DBV t3 = x[1]*y[2];
      double t8 = x[1]*x[1];
      double t11 = x[2]*x[2];
      double t12 = x[1]*t11;
      double t13 = x[0]*x[0];
      DBV t20 = t13*y[1];
      DBV t22 = t11*y[1];
      DBV t23 = y[2]*t8;
      DBV t24 = y[0]*t8;
      double t28 = 1/(-x[2]+x[0]);
      double t34 = 1/(x[0]*x[1]-x[0]*x[2]-t8+x[2]*x[1]);
      DBV A = (y[1]*x[2]-y[1]*x[0]-t3+x[1]*y[0]-y[0]*x[2]+x[0]*y[2])/(t8*x[2]-t8*x[0]-t12+x[1]*t13-t13*x[2]+x[0]*t11);
      DBV B = -(-t20+t13*y[2]+t22-t23+t24-y[0]*t11)*t28*t34;
      DBV C = (t3*t13-t20*x[2]-t23*x[0]+t22*x[0]+t24*x[2]-t12*y[0])*t28*t34;
      return (A*t + B)*t + C;
}

DBV interpolation_factors(double t,double x[3])
{
	if(fabs(x[1] - x[2]) < DBL_EPS){
		if(fabs(x[0] - x[1]) < DBL_EPS)
			return DBV(1,0,0);
		double t1 = t - x[1]/(x[0] - x[1]);
		return DBV(t1, 1-t1, 0);
		}
	if(fabs(x[0] - x[1]) < DBL_EPS){
		double t1 = t - x[2]/(x[1] - x[2]);
		return DBV(0,t1, 1-t1);
		}

	double t1 = x[0]*x[0];
	double t2 = x[1]*t1;
	double t3 = x[1]*x[1];
	double t4 = t3*x[0];
	double t7 = 1/(-x[2]+x[0]);
	double t13 = 1/(x[0]*x[1]-x[0]*x[2]-t3+x[2]*x[1]);
	double t19 = t3*x[2];
	double t20 = x[2]*x[2];
	double t21 = x[1]*t20;
	double t22 = t1*x[2];
	double t23 = x[0]*t20;
	double t25 = 1/(t19-t4-t21+t2-t22+t23);
	return DBV((-t21+t19)*t7*t13+(-(t3-t20)*t7*t13+(x[1]-x[2])*t25*t)*t,
			  (-t22+t23)*t7*t13+(-(t20-t1)*t7*t13+(x[2]-x[0])*t25*t)*t,
			(t2-t4)*t7*t13+(-(-t3+t1)*t7*t13+(-x[1]+x[0])*t25*t)*t);
}

