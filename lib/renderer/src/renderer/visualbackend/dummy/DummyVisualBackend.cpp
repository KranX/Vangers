//
// Created by nikita on 30.11.2021.
//
#include <iostream>

#include "DummyVisualBackend.h"

using namespace renderer::visualbackend;
using namespace renderer::visualbackend::dummy;

void DummyVisualBackend::map_create(const MapDescription& map_description) {
	std::cout << "DummyVisualBackend::map_create"
	          << " => " << 0
	          << std::endl;
}

void DummyVisualBackend::map_destroy() {
	std::cout << "DummyVisualBackend::map_destroy"
	          << std::endl;
}

void DummyVisualBackend::map_request_update(const Rect& rect) {
	std::cout << "DummyVisualBackend::map_request_update"
	          << ", rect: " << rect
			  << std::endl;
}

void DummyVisualBackend::render(const renderer::Rect &viewport)
{

}

void DummyVisualBackend::camera_set_projection(const CameraProjection &camera_projection)
{

}

void DummyVisualBackend::camera_set_transform(const vectormath::Transform &transform)
{

}

void DummyVisualBackend::map_update_palette(uint32_t *palette, int32_t palette_size)
{

}

void DummyVisualBackend::set_screen_resolution(int32_t width, int32_t height)
{

}

ModelHandle DummyVisualBackend::model_create(const char* name, void* model)
{

}

void DummyVisualBackend::model_destroy(ModelHandle model_handle)
{

}

ModelInstanceHandle DummyVisualBackend::model_instance_create(ModelHandle model_handle, uint8_t color_id)
{

}

void DummyVisualBackend::model_instance_destroy(ModelInstanceHandle model_instance_handle)
{

}

void DummyVisualBackend::model_instance_set_transform(ModelInstanceHandle model_instance_handle, const vectormath::Transform& transform)
{

}

void DummyVisualBackend::destroy()
{

}

