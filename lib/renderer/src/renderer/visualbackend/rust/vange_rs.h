#ifndef VANGE_RS_H
#define VANGE_RS_H

#include <cstdint>

/**
  This is a stub file for the upcoming vange-rs library bindings
**/

// TODO: agree on how we deal with structs
struct rv_camera_description {
	float fov;
	float aspect;
	float near;
	float far;
};

struct rv_vector3 {
	float x;
	float y;
	float z;
};

struct rv_quaternion {
	float x;
	float y;
	float z;
	float w;
};

struct rv_transform {
	rv_vector3 position;
	rv_quaternion rotation;
};

typedef void* (*rv_gl_functor)(const char*) ;

struct rv_init_descriptor {
	uint32_t width;
	uint32_t height;
	rv_gl_functor gl_functor;
};

struct rv_map_description {
	int32_t width; // H_SIZE
	int32_t height; // V_SIZE

	// data layout: first `width` bytes are height, the rest `width` bytes are meta data
	uint8_t** lineT;

	// Material offsets in the palette
	uint8_t* material_begin_offsets;
	uint8_t* material_end_offsets;

	// 8 for world, 16 for escave
	int32_t material_count;
};

// TODO: or it could be left, bottom, right, top
struct rv_rect {
	int32_t x;
	int32_t y;
	int32_t width;
	int32_t height;
};

typedef void* rv_context;



#ifdef __cplusplus
extern "C" {
#endif
#ifndef EMSCRIPTEN
	extern int32_t rv_api_1;
#endif

	rv_context rv_init(rv_init_descriptor desc);

	void rv_exit(rv_context context);

	void rv_camera_init(rv_context context, rv_camera_description desc);
	// TODO:
	//void vange_rs_camera_destroy();

	void rv_camera_set_transform(rv_context context, rv_transform transform);

	void rv_map_init(rv_context context, rv_map_description map_description);

	void rv_map_exit(rv_context context);

	void rv_map_update_data(rv_context context, rv_rect region);

	void rv_map_update_palette(rv_context context, int32_t first_entry, int32_t entry_count, uint8_t* palette);

	void rv_resize(rv_context context, uint32_t width, uint32_t height);
	
	void rv_render(rv_context context, rv_rect viewport);

#ifdef __cplusplus
}
#endif


#endif // VANGE_RS_H
