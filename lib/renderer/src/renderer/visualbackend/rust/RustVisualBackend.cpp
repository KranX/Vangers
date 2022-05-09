#include <iostream>

#include <SDL2/SDL.h>
#include <algorithm>

#include "RustVisualBackend.h"
#include "../../exception.h"
#include "vange_rs.h"

using namespace renderer;
using namespace renderer::visualbackend;
using namespace renderer::visualbackend::rust;

RustVisualBackend::RustVisualBackend(int32_t width, int32_t height)
	: _map_created(false)
{
	std::cout << "RustVisualBackend::RustVisualBackend" << std::endl;

	if(rv_api_2 != 1){
		throw RendererException("Invalid libvangers_ffi version");
	}

	rv_init_descriptor desc {
		.width = (uint32_t) width,
		.height = (uint32_t) height,
		.gl_functor = SDL_GL_GetProcAddress,
	};

	std::cout << "rv_init(context=" << _context << ", {" << std::endl
				<< "\t.width="<<desc.width << std::endl
				<< "\t.height="<<desc.height << std::endl
				<< "\t.gl_functor="<< (void*)desc.gl_functor << std::endl
				<< "}" << std::endl;
	_context = rv_init(desc);
}

RustVisualBackend::~RustVisualBackend()
{

}

void RustVisualBackend::camera_set_projection(const CameraProjection &camera_projection)
{
//	std::cout << "RustVisualBackend::camera_set_projection" << std::endl;

	rv_camera_description v_desc {
		.fov = camera_projection.fov,
		.aspect = camera_projection.aspect,
		.near= camera_projection.near_plane,
		.far = camera_projection.far_plane,
	};

//	std::cout << "rv_camera_init(context=" << _context << ", {" << std::endl
//				<< "\t.fov="<<v_desc.fov << std::endl
//				<< "\t.aspect="<<v_desc.aspect << std::endl
//				<< "\t.near="<<v_desc.near << std::endl
//				<< "\t.far="<<v_desc.far << std::endl
//				<< "})" << std::endl;

	rv_camera_init(_context, v_desc);
}

void RustVisualBackend::camera_set_transform(const vectormath::Transform& transform)
{
//	std::cout << "RustVisualBackend::camera_set_transform" << std::endl;
	rv_transform v_transform {
		.position = rv_vector3 {
			.x = transform.position.x,
			.y = transform.position.y,
			.z = transform.position.z * 2.0f + 64,
		},
		.scale = 1.0f,
		.rotation = rv_quaternion {
			.x = transform.rotation.x,
			.y = transform.rotation.y,
			.z = transform.rotation.z,
			.w = transform.rotation.w,
		}
	};
//	std::cout << "rv_camera_set_transform(context=" << _context << ", {" << std::endl
//				<< "\t.position={" << v_transform.position.x << " " << v_transform.position.y << " " << v_transform.position.z << "}"<< std::endl
//				<< "\t.rotation={" << v_transform.rotation.x << " " << v_transform.rotation.y << " " << v_transform.rotation.z << " " << v_transform.rotation.w << "}"<< std::endl
//				<< "})" << std::endl;
	rv_camera_set_transform(_context, v_transform);
}

void RustVisualBackend::map_create(const MapDescription& map_description)
{
	std::cout << "RustVisualBackend::map_create" << std::endl;

	if(_map_created){
		std::cout << "RustVisualBackend::map_create: map has been already created" << std::endl;
		return;
	}
	_map_created = true;
	rv_map_description v_desc {
		.width = map_description.width,
		.height = map_description.height,
		.lineT = map_description.lineT,
		.material_begin_offsets = map_description.material_begin_offsets,
		.material_end_offsets = map_description.material_end_offsets,
		.material_count = map_description.material_count,
	};

	std::cout << "rv_map_init(context=" << _context << ", {" << std::endl
				<< "\t.width="<<v_desc.width << std::endl
				<< "\t.height="<<v_desc.height << std::endl
				<< "\t.lineT="<<(void*)v_desc.lineT << std::endl
				<< "\t.material_begin_offsets="<< (void*)v_desc.material_begin_offsets << std::endl
				<< "\t.material_end_offsets="<< (void*)v_desc.material_end_offsets << std::endl
				<< "\t.material_count="<<v_desc.material_count << std::endl
				<< "})" << std::endl;
	rv_map_init(_context, v_desc);
}

void RustVisualBackend::map_destroy()
{
//	std::cout << "RustVisualBackend::map_destroy" << std::endl;
	if(!_map_created){
		std::cout << "RustVisualBackend::map_destroy: map has been already destroyed" <<std::endl;
		return;
	}
	_map_created = false;
	std::cout << "rv_map_exit(context=" << _context << ", )" << std::endl;
	rv_map_exit(_context);
}

void RustVisualBackend::map_request_update(const Rect& region)
{
//	std::cout << "RustVisualBackend::map_request_update. region=" << region << std::endl;
	rv_rect v_rect {
		.x = region.x,
		.y = region.y,
		.width = region.width,
		.height = region.height,
	};

	if(v_rect.width == 0 || v_rect.height == 0){
		return;
	}
//	std::cout << "rv_map_update_data(context=" << _context << ", {" << std::endl
//				<< "\t{" << v_rect.x << " " << v_rect.y << " " << v_rect.width << " " << v_rect.height << "}" << std::endl
//				<< "})" << std::endl;
	rv_map_update_data(_context, v_rect);
}

void RustVisualBackend::map_update_palette(uint32_t* palette, int32_t palette_size)
{
//	std::cout << "RustVisualBackend::map_update_palette" << std::endl;
	uint8_t* rv_pal = new uint8_t[3 * palette_size];

	for(int i = 0; i < palette_size; i++){
		uint8_t* color = (uint8_t*)&palette[i];
		rv_pal[i * 3 + 0] = color[0];
		rv_pal[i * 3 + 1] = color[1];
		rv_pal[i * 3 + 2] = color[2];
	}
	rv_map_update_palette(_context, 0, palette_size, rv_pal);
}

void RustVisualBackend::set_screen_resolution(int32_t width, int32_t height)
{
	std::cout << "RustVisualBackend::set_screen_resolution" <<
	             " width: " << width <<
	             " , height: "<< height <<
	             std::endl;

	uint32_t rv_width = width;
	uint32_t rv_height = height;
	std::cout << "rv_resize(context=" << _context <<
	             ", width="<<rv_width <<
	             ", height="<<rv_height <<
	             ")" << std::endl;
	rv_resize(_context, rv_width, rv_height);
}

ModelHandle RustVisualBackend::model_create(const char* name, void* model)
{
	std::cout << "RustVisualBackend::model_create"
				<< " name: " << name
				<< ", model: " << (void*)model
				<< std::endl;

	uint64_t handle = rv_model_create(_context, name, model);
	return {handle};
}

void RustVisualBackend::model_destroy(ModelHandle model_handle)
{
	std::cout << "RustVisualBackend::model_destroy"
				<< " model_handle: " << model_handle.handle
				<< std::endl;
	rv_model_destroy(_context, model_handle.handle);
}

ModelInstanceHandle RustVisualBackend::model_instance_create(ModelHandle model_handle, uint8_t color_id)
{
//	std::cout << "RustVisualBackend::model_instance_create"
//				<< " model_handle: " << model_handle.handle
//				<< ", color_id: " << color_id
//				<< std::endl;

	uint64_t model_instance_handle = rv_model_instance_create(_context, model_handle.handle, color_id);
	return {model_instance_handle};
}

void RustVisualBackend::model_instance_destroy(ModelInstanceHandle model_instance_handle)
{
	std::cout << "RustVisualBackend::model_instance_destroy"
				<< " model_instance_handle: " << model_instance_handle.handle
				<< std::endl;
	rv_model_instance_destroy(_context, model_instance_handle.handle);
}

void RustVisualBackend::model_instance_set_transform(ModelInstanceHandle model_instance_handle, const vectormath::Transform& transform)
{
//	std::cout << "RustVisualBackend::model_instance_set_transform"
//				<< " model_instance_handle: " << model_instance_handle.handle
//				<< "\t.position={" << transform.position.x << " " << transform.position.y << " " << transform.position.z << "}"<< std::endl
//				<< "\t.rotation={" << transform.rotation.x << " " << transform.rotation.y << " " << transform.rotation.z << " " << transform.rotation.w << "}"<< std::endl
//				<< "\t.scale="<<transform.scale << std::endl
//				<< "})"
//				<< std::endl;

	rv_transform v_transform {
		.position = rv_vector3 {
			.x = transform.position.x,
			.y = transform.position.y,
			.z = transform.position.z,
		},
		.scale = transform.scale,
		.rotation = rv_quaternion {
			.x = transform.rotation.x,
			.y = transform.rotation.y,
			.z = transform.rotation.z,
			.w = transform.rotation.w,
		}
	};

	rv_model_instance_set_transform(_context, model_instance_handle.handle, v_transform);
}

void RustVisualBackend::model_instance_set_visible(ModelInstanceHandle model_instance_handle, bool visible)
{
	rv_model_instance_set_visibile(_context, model_instance_handle.handle, visible);
}

void RustVisualBackend::render(const Rect& viewport)
{
//	std::cout << "RustVisualBackend::render. viewport=" << viewport<< std::endl;

	rv_rect v_rect {
		.x = viewport.x,
		.y = viewport.y,
		.width = viewport.width,
		.height = viewport.height,
	};
//	std::cout << "rv_render(context=" << _context << ", {" << std::endl
//				<< "\t{" << v_rect.x << " " << v_rect.y << " " << v_rect.width << " " << v_rect.height << "}" << std::endl
//				<< "})" << std::endl;

	rv_render(_context, v_rect);
}

void RustVisualBackend::destroy()
{
	std::cout << "rv_exit(context=" << _context << ", )" << std::endl;
	rv_exit(_context);
}

