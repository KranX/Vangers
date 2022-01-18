//
// Created by nikita on 05.12.2021.
//

#ifndef VANGERS_ABSTRACTCORERENDERER_H
#define VANGERS_ABSTRACTCORERENDERER_H

#include <cstdint>

#include "../common.h"
#include "../ResourceId.h"

namespace renderer::compositor {
	enum class TextureType {
		RGBA32,
	};

	enum class BlendMode {
		None,
		Alpha
	};


	enum class _TextureT: int32_t;
	typedef ResourceId<_TextureT> Texture;

	static_assert (sizeof (Texture) == sizeof (int32_t), "invalid Texture RID size");
	
	class AbstractCompositor{
	public:
		virtual ~AbstractCompositor() = default;
		// TODO: filtering settings
		virtual Texture texture_create(int32_t width, int32_t height, TextureType texture_type, BlendMode blend_mode) = 0;
		virtual void texture_set_data(Texture texture, uint8_t* data) = 0;
		virtual void texture_render(Texture texture, const Rect &src_rect, const Rect &dst_rect) = 0;
		virtual void texture_destroy(Texture texture) = 0;
		virtual void texture_query(Texture texture, int32_t *width, int32_t *height, TextureType *texture_type, BlendMode *blend_mode) = 0;
		virtual void texture_set_color(Texture texture, const Color& color) = 0;
		virtual void render_begin() = 0;
		virtual void render_present() = 0;
		virtual void initialize() = 0;
		virtual void dispose() = 0;
		virtual void query_output_size(int32_t* width, int32_t* height) = 0;
		virtual void set_physical_screen_size(int32_t width, int32_t height) = 0;
		virtual void set_logical_screen_size(int32_t width, int32_t height) = 0;
		virtual void read_pixels(uint8_t* output) = 0;
	};
}
#endif //VANGERS_ABSTRACTCORERENDERER_H
