#ifndef LIB_RENDERER_SRC_RENDERER_COMMON
#define LIB_RENDERER_SRC_RENDERER_COMMON

#include <cstdint>
#include <iostream>

namespace renderer {
//	const int32_t RID_Invalid = -1;
	
	struct Rect {
		int32_t x;
		int32_t y;
		int32_t width;
		int32_t height;

		void expand(const Rect& other);
		bool contains(const Rect& other) const;
		bool is_empty() const;
	};


	struct Color {
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};
}
std::ostream &operator<<(std::ostream &os, const renderer::Color &color);
std::ostream &operator<<(std::ostream &os, const renderer::Rect &rect);

#endif /* LIB_RENDERER_SRC_RENDERER_COMMON */
