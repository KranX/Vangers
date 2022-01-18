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

void DummyVisualBackend::camera_create(const CameraDescription &camera_description)
{

}

void DummyVisualBackend::camera_destroy()
{

}

void DummyVisualBackend::camera_set_transform(const Transform &transform)
{

}

void DummyVisualBackend::map_update_palette(uint32_t *palette, int32_t palette_size)
{

}

