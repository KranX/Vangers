#include <cassert>

#include "common.h"

std::ostream &operator<<(std::ostream &os, const renderer::Rect &rect){
	return os << "Rect{" 
	 << ".x=" << rect.x
	 << ", .y=" << rect.y
	 << ", .width=" << rect.width
	 << ", .height=" << rect.height
	 << "}";
}

std::ostream &operator<<(std::ostream &os, const renderer::Color &color){
	return os << "Color{" 
	 << ".r=" << color.r
	 << ", .g=" << color.g
	 << ", .b=" << color.b
	 << ", .a=" << color.a
	 << "}";
}

void renderer::Rect::expand(const renderer::Rect &rect)
{
	if(is_empty()){
		x = rect.x;
		y = rect.y;
		width = rect.width;
		height = rect.height;
		return;
	}

	int rect_right = rect.x + rect.width;
	int rect_bottom = rect.y + rect.height;
	int right = x + width;
	int bottom = y + height;

	if(rect_right > right) {
		right = rect_right;
	}

	if(rect_bottom > bottom) {
		bottom = rect_bottom;
	}

	if(rect.x < x) {
		x = rect.x;
	}

	if(rect.y < y) {
		y = rect.y;
	}

	int new_width = right - x;
	int new_height = bottom - y;
	assert(new_width >= 0);
	assert(new_height >= 0);

	width = new_width;
	height = new_height;
}

bool renderer::Rect::contains(const renderer::Rect &other) const
{
	return x <= other.x &&
			y <= other.y &&
			x + width >= other.x + other.width &&
			y + height >= other.y + other.height;
}

bool renderer::Rect::is_empty() const
{
	return width == 0 || height == 0;
}
