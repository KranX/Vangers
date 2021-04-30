#include "../global.h"

#include "general.h"

#ifdef _SURMAP_
//@caiiiycuk: should we call dynamics_init or not?
#define dynamics_init(a)
#endif

/*******************************************************************************
			Load Model function
*******************************************************************************/
Model::Model()
{
	memory_allocation_method = 0;
	num_vert = num_norm = num_poly = 0;
	vertices = nullptr;
	normals = nullptr;
	polygons = nullptr;
}
void Model::free()
{
	if(memory_allocation_method) {
		delete[] vertices;
	}
	memory_allocation_method = 0;
	num_vert = num_norm = num_poly = 0;
	vertices = nullptr;
	normals = nullptr;
	polygons = nullptr;
}

void Model::loadC3Dvariable(XBuffer& buf)
{
	int i,j,size;
	int num_vert_total, version;
	int num, vert_ind, norm_ind, sort_info;
	unsigned color_id, color_shift;
	int phi, psi, tetta;

	buf > version;
	if(version != C3D_VERSION_1 && version != C3D_VERSION_3)
		ErrH.Abort("Incorrect C3D version");

	buf > num_vert > num_norm > num_poly > num_vert_total;

	buf > xmax > ymax > zmax;
	buf > xmin > ymin > zmin;
	buf > x_off > y_off > z_off;
	buf > rmax;
	buf > phi > psi > tetta;
	if(version == C3D_VERSION_3)
		buf > volume > rcm > J;
#ifdef _ROAD_
	else
		ErrH.Abort("C3D - old version.You need to update all m3d & a3d");
#endif

#ifdef COMPACT_3D
	size = num_vert*static_cast<int>(sizeof(Vertex)) +
		   num_norm*static_cast<int>(sizeof(Normal)) +
		   num_poly*static_cast<int>(sizeof(VariablePolygon));
#else
	size = num_vert*sizeof(Vertex) + num_norm*sizeof(Normal) +
		   num_poly*(sizeof(VariablePolygon) + 3*sizeof(VariablePolygon*));
#endif
	size += num_vert_total*static_cast<int>(sizeof(Vertex*) + sizeof(Normal*));

	memory_allocation_method = 1;
	HEAP_BEGIN(size);
	vertices = HEAP_ALLOC(num_vert, Vertex);
	normals = HEAP_ALLOC(num_norm, Normal);
	variable_polygons = HEAP_ALLOC(num_poly, VariablePolygon);

	if (phi == 83 && psi == 83 && tetta == 83) {
		for(i = 0;i < num_vert;i++) {
		#ifdef COMPACT_3D
			float tf;
			buf > tf > tf > tf;
		#else
			buf > vertices[i].x > vertices[i].y > vertices[i].z;
		#endif
			buf > vertices[i].x_8 > vertices[i].y_8 > vertices[i].z_8
			    > sort_info;
			}
	} else {
		int ti;
		for(i = 0;i < num_vert;i++) {
		#ifdef COMPACT_3D
			buf > ti > ti > ti;
		#else
			buf > ti;
			vertices[i].x = (float)ti;
			buf > ti;
			vertices[i].y = (float)ti;
			buf > ti;
			vertices[i].z = (float)ti;
		#endif
			buf > vertices[i].x_8 > vertices[i].y_8 > vertices[i].z_8
			    > sort_info;
		}
	}

	for(i = 0;i < num_norm;i++)
		buf > normals[i].x > normals[i].y > normals[i].z
		    > normals[i].n_power > sort_info;

	for(i = 0;i < num_poly;i++) {
		buf > num > sort_info
		    > color_id > color_shift
		    > variable_polygons[i].flat_normal.x > variable_polygons[i].flat_normal.y
		    > variable_polygons[i].flat_normal.z > variable_polygons[i].flat_normal.n_power
		    > variable_polygons[i].middle_x > variable_polygons[i].middle_y > variable_polygons[i].middle_z;
		    
		variable_polygons[i].color_id = static_cast<unsigned char>(
			color_id < static_cast<unsigned int>(COLORS_IDS::MAX_COLORS_IDS) ?
			color_id : static_cast<unsigned int>(COLORS_IDS::BODY)
		);

		variable_polygons[i].num_vert = num;
		variable_polygons[i].vertices = HEAP_ALLOC(num, Vertex*);
		variable_polygons[i].normals = HEAP_ALLOC(num, Normal*);
		for(j = 0;j < num;j++) {
				buf > vert_ind > norm_ind;
				variable_polygons[i].vertices[j] = &vertices[vert_ind];
				variable_polygons[i].normals[j] = &normals[norm_ind];
			}
		}

#ifndef COMPACT_3D
	int poly_ind;
	for(i = 0;i < 3;i++){
		sorted_variable_polygons[i] = HEAP_ALLOC(num_poly,VariablePolygon*);
		for(j = 0;j < num_poly;j++){
			buf > poly_ind;
			sorted_variable_polygons[i][j] = &variable_polygons[poly_ind];
			}
		}
#else
	//buf.set(3*num_poly*sizeof(VariablePolygon*),XB_CUR);
	buf.set(3*num_poly*4,XB_CUR);
#endif
	HEAP_END;
}

#ifdef COMPACT_3D
void Model::loadC3D(XBuffer& buf)
{
	int i,j,size;
	int num_vert_total,version;
	int num,vert_ind,norm_ind,sort_info;
	unsigned color_id,color_shift;
	char skip_char;
	int phi,psi,tetta;
	
	buf > version;
	//std::cout<<"Load C3D. Version:"<<version<<std::endl;
	if(version != C3D_VERSION_1 && version != C3D_VERSION_3)
		ErrH.Abort("Incorrect C3D version", version);

	buf > num_vert > num_norm > num_poly > num_vert_total;

	buf > xmax > ymax > zmax;
	buf > xmin > ymin > zmin;
	buf > x_off > y_off > z_off;
	buf > rmax;
	buf > phi > psi > tetta;
	/*std::cout<<"num_vert:"<<num_vert<<" num_norm:"<<num_norm<<" num_poly:"<<num_poly<<" num_vert_total:"<<num_vert_total<<std::endl;
	std::cout<<"xmax:"<<xmax<<" ymax:"<<ymax<<" zmax:"<<zmax<<std::endl;
	std::cout<<"xmin:"<<xmin<<" ymin:"<<ymin<<" zmin:"<<zmin<<std::endl;
	std::cout<<"x_off:"<<x_off<<" y_off:"<<y_off<<" z_off:"<<z_off<<std::endl;
	std::cout<<"rmax:"<<rmax<<std::endl;
	std::cout<<"phi:"<<phi<<" psi:"<<psi<<" tetta:"<<tetta<<std::endl;*/
	if(version == C3D_VERSION_3)
		buf > volume > rcm > J;
	else
		ErrH.Abort("C3D - old version.You need to update all m3d & a3d");

	size = num_vert*static_cast<int>(sizeof(Vertex)) + num_norm*static_cast<int>(sizeof(Normal)) + num_poly*static_cast<int>(sizeof(Polygon));

	memory_allocation_method = 1;
	HEAP_BEGIN(size);
	vertices = HEAP_ALLOC(num_vert,Vertex);
	normals = HEAP_ALLOC(num_norm,Normal);
	polygons = HEAP_ALLOC(num_poly,Polygon);

	if(phi == 83 && psi == 83 && tetta == 83){
		float tf;
		for(i = 0;i < num_vert;i++){
			buf > tf > tf > tf;
			buf > vertices[i].x_8 > vertices[i].y_8 > vertices[i].z_8
			    > sort_info;
			}
		}
	else{
		int ti;
		for(i = 0;i < num_vert;i++){
			buf > ti > ti > ti;
			buf > vertices[i].x_8 > vertices[i].y_8 > vertices[i].z_8
			    > sort_info;
			}
		}

	for(i = 0;i < num_norm;i++)
		buf > normals[i].x > normals[i].y > normals[i].z
		    > normals[i].n_power > sort_info;

	for(i = 0;i < num_poly;i++){
		buf > num > sort_info
		    > color_id > color_shift
		    > skip_char > skip_char 
		    > skip_char > skip_char 
		    > polygons[i].middle_x > polygons[i].middle_y > polygons[i].middle_z;
		
		polygons[i].color_id = static_cast<unsigned char>(
			color_id < static_cast<unsigned int>(COLORS_IDS::MAX_COLORS_IDS) ?
			color_id : static_cast<unsigned int>(COLORS_IDS::BODY)
		);

		if(num != 3)
			ErrH.Abort("Non triangular 3D model");
		for(j = 0;j < 3;j++){
			buf > vert_ind > norm_ind;
			polygons[i].vertices[j] = &vertices[vert_ind];
			polygons[i].normals[j] = &normals[norm_ind];
			}
		}

	//buf.set(3*num_poly*sizeof(VariablePolygon*),XB_CUR);
	buf.set(3*num_poly*4,XB_CUR);

	HEAP_END;
}
#else
void Model::loadC3D(XBuffer& buf){ loadC3Dvariable(buf); }
#endif

/*******************************************************************************
			Objects function
*******************************************************************************/
Object::Object()
{
	i_model = n_models = 0;
	models = 0;
	model = 0;
	bound = 0;
	n_wheels = 0;
	wheels = 0;
	n_debris = 0;
	debris = 0;
	bound_debris = 0;
	active = 0;
	original_scale_size = scale_size = 1;
	collision_object = 0;
	slots_existence = 0;
	memset(data_in_slots,0,MAX_SLOTS*sizeof(Object*));
	old_appearance_storage = 0;

  	set_body_color(COLORS_IDS::BODY_RED);

	ID = ID_VANGER;

	dynamic_state = 0;
	Visibility = 0;
	MapLevel = 0;
	x_of_last_update = y_of_last_update = 0;
	R_curr = R_prev = Vector(0,0,0);
}

Object& Object::operator = (Object& obj)
{
	memcpy(&n_models,&obj.n_models,(char*)&end_of_object_data - (char*)&n_models);
	return *this;
}

#ifdef _ROAD_
void Object::convert_to_beeb(Object* beeb)
{
	if(beeb) {
		old_appearance_storage = new Object;
		*old_appearance_storage = *this;
		memcpy(&n_models,&beeb -> n_models,(char*)&old_appearance_storage - (char*)&n_models);
		memcpy(&m,&beeb -> m,(char*)&end_of_object_data - (char*)&m);
	} else if(old_appearance_storage) {
		memcpy(&n_models,&old_appearance_storage -> n_models,(char*)&old_appearance_storage - (char*)&n_models);
		memcpy(&m,&old_appearance_storage -> m,(char*)&end_of_object_data - (char*)&m);
		delete old_appearance_storage;
		old_appearance_storage = 0;
	}
	update_coord();
}
#endif

void Object::free()
{
	int i;
	for(i = 0;i < n_models;i++)
		models[i].free();
	if (n_models <= 1) {
		delete models;
	} else {
		delete[] models;
	}
	if(bound) {
		bound -> free();
		delete bound;
	}
	if(n_wheels) {
		for(i = 0;i < n_wheels;i++)
			if(wheels[i].steer)
				wheels[i].model.free();
		delete[] wheels;
	}
	if(n_debris) {
		for(i = 0;i < n_debris;i++) {
			debris[i].free();
			bound_debris[i].free();
		}
		delete[] debris;
		delete[] bound_debris;
	}

	i_model = n_models = 0;
	models = nullptr;
	model = nullptr;
	bound = nullptr;
	n_wheels = 0;
	wheels = nullptr;
	n_debris = 0;
	debris = nullptr;
	bound_debris = nullptr;
	slots_existence = 0;
	memset(data_in_slots, 0, MAX_SLOTS*sizeof(Object*));
}
void Object::loadM3D(char* name)
{
	int i;
	n_models = 1;
	models = model = new Model;
	bound = new Model;

	XStream ff(name,XS_IN);
	XBuffer buf(ff.size());
	ff.read(buf.address(),ff.size());
	ff.close();
	//ErrH.Log(name);
	model -> loadC3D(buf);
	buf > xmax > ymax > zmax > rmax > n_wheels > n_debris;
	buf > body_color_offset > body_color_shift;
	if(n_wheels)
		wheels = new Wheel[n_wheels];
	if(n_debris){
		debris = new Model[n_debris];
		bound_debris = new Model[n_debris];
		}
	for(i = 0;i < n_wheels;i++){
		buf > wheels[i].steer > wheels[i].r > wheels[i].width > wheels[i].radius > wheels[i].bound_index;
		if(wheels[i].steer)
			wheels[i].model.loadC3D(buf);
		wheels[i].dZ = 0;
		}

	for(i = 0;i < n_debris;i++){
		debris[i].loadC3D(buf);
		bound_debris[i].loadC3Dvariable(buf);
		}

	bound -> loadC3Dvariable(buf);

	buf > slots_existence;
	if(slots_existence){
		for(i = 0;i < MAX_SLOTS;i++){
			buf > R_slots[i] > location_angle_of_slots[i];
			data_in_slots[i] = 0;
			}
		}

	dynamics_init(name);
}
void Object::loadA3D(char* name)
{

	int i;
	XStream ff(name,XS_IN);
	XBuffer buf(ff.size());
	ff.read(buf.address(),ff.size());
	ff.close();
	//std::cout<<"Load A3D:"<<name<<std::endl;
	buf > n_models > xmax > ymax > zmax > rmax;
	buf >  body_color_offset > body_color_shift;
	//std::cout<<"n_models:"<<n_models<<" xmax:"<<xmax<<" ymax:"<<ymax<<" zmax:"<<zmax<<" rmax:"<<rmax<<std::endl;
	//std::cout<<"body_color_offset:"<<body_color_offset<<" body_color_shift:"<<body_color_shift<<std::endl;
	//ErrH.Abort(name, n_models);
	models = new Model[n_models];

	for(i = 0;i < n_models;i++)
		{
		//std::cout<<"BBB:"<<n_models<<" "<<xmax<<""<<ymax<<" "<<zmax<<" "<<rmax<<std::endl;
		models[i].loadC3D(buf);
		
		}
	ID = ID_INSECT;
	model = models;
	dynamics_init(name);
}
void Object::load(char* name,int size_for_m3d)
{

#ifndef COMPACT_3D
	if(strstr(name,".c3d")){
		n_models = 1;
		models = model = new Model;
		XStream ff(name,XS_IN);
		XBuffer buf(ff.size());
		ff.read(buf.address(),ff.size());
		ff.close();
		model -> loadC3D(buf);
		xmax = model -> xmax;
		ymax = model -> ymax;
		zmax = model -> zmax;
		rmax = model -> rmax;
		dynamics_init(name);
		return;
		}

#ifndef _SURMAP_
	if(strstr(name,".asc")){
		#ifdef _MT_
			scale_size = .5;
		#else
			scale_size = (double)size_for_m3d/256;
		#endif
		loadLST("common.lst",name);
		return;
		}
	if(strstr(name,".lst")){
		#ifdef _MT_
			scale_size = .5;
		#else
			scale_size = (double)size_for_m3d/256;
		#endif
		loadLST(name);
		return;
		}
	if(strstr(name,".lsa")){
		#ifdef _MT_
			scale_size = 0.5;
		#else
			scale_size = (double)size_for_m3d/256;
		#endif
		loadLSA(name);
		return;
		}
#endif
#endif

	if(strstr(name,".m3d")){
		#ifdef _MT_
			scale_size = 0.5;
		#else
			original_scale_size = scale_size = (double)size_for_m3d/256;
		#endif
		loadM3D(name);
		return;
		}
#ifndef _SURMAP_
	if(strstr(name,".a3d")){
		#ifdef _MT_
			scale_size = 0.5;
		#else
			original_scale_size = scale_size = (double)size_for_m3d/256;
		#endif
		loadA3D(name);
		return;
		}

#endif
	ErrH.Abort("Unable to recognize 3d model file extension");
}

void Object::lay_to_slot(int slot,Object* weapon)
{
	if(!((1 << slot) & slots_existence))
		return;
	data_in_slots[slot] = weapon;
}
