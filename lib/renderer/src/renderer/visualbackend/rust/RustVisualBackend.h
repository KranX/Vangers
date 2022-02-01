#ifndef RUSTRENDERER_H
#define RUSTRENDERER_H

#include "../AbstractVisualBackend.h"

typedef void* rv_context;

namespace renderer::visualbackend::rust {
    class RustVisualBackend: public AbstractVisualBackend
	{
	public:
		RustVisualBackend(int32_t width, int32_t height);
		~RustVisualBackend();
		void camera_set_projection(const CameraProjection &camera_projection) override;
		void camera_set_transform(const Transform& transform) override;
		void map_create(const MapDescription& map_description) override;
		void map_destroy() override;
		void map_request_update(const Rect& region) override;
		void map_update_palette(uint32_t* palette, int32_t palette_size) override;
		void set_screen_resolution(int32_t width, int32_t height) override;
		void render(const Rect& viewport) override;
		void destroy() override;
	private:
		rv_context _context;
		bool _map_created;
	};

}

#endif // RUSTRENDERER_H
