#include "text_layer.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <unordered_map>

namespace xgr::text {
bool enabled = false;
namespace {
std::unordered_map<const void *, Layer> layers;
SDL_Rect intersection(SDL_Rect a, SDL_Rect b) {
	SDL_Rect result{};
	SDL_GetRectIntersection(&a, &b, &result);
	return result;
}

uint32_t over(uint32_t dst, Pixel src, const SDL_Color *palette) {
	const auto colour = palette[src.colour];
	const unsigned a = src.alpha, inverse = 255 - a;
	const unsigned r = (colour.r * a + ((dst >> 16) & 255) * inverse + 127) / 255;
	const unsigned g = (colour.g * a + ((dst >> 8) & 255) * inverse + 127) / 255;
	const unsigned b = (colour.b * a + (dst & 255) * inverse + 127) / 255;
	const unsigned alpha = a + ((dst >> 24) * inverse + 127) / 255;
	return (alpha << 24) | (r << 16) | (g << 8) | b;
}
} // namespace

Layer *find(const void *buffer) {
	if (!enabled || !buffer)
		return nullptr;
	const auto found = layers.find(buffer);
	return found == layers.end() ? nullptr : &found->second;
}

Layer *prepare(const void *buffer, int width, int height, bool reset) {
	if (!enabled || !buffer)
		return nullptr;
	auto [it, inserted] = layers.try_emplace(buffer, width, height);
	if (!inserted && (it->second.width() != width || it->second.height() != height))
		it->second = Layer(width, height);
	else if (reset)
		it->second.clear();
	return &it->second;
}

void forget(const void *buffer) {
	layers.erase(buffer);
}

void erase(const void *buffer, SDL_Rect bounds, const uint8_t *mask, int pitch) {
	if (auto *layer = find(buffer))
		layer->erase(bounds, mask, pitch);
}

void clear(const void *buffer) {
	if (auto *layer = find(buffer))
		layer->clear();
}

Layer::Layer(int width, int height): width_(width), height_(height) {
	if (width < 0 || height < 0)
		throw std::invalid_argument("Negative text layer dimensions");
}

void Layer::clear() {
	draws_.clear();
}
bool Layer::empty() const {
	return draws_.empty();
}

void Layer::prune() {
	draws_.erase(
		std::remove_if(
			draws_.begin(),
			draws_.end(),
			[](const Draw &draw) {
				return std::none_of(draw.visible.begin(), draw.visible.end(), [](uint8_t v) {
					return v;
				});
			}
		),
		draws_.end()
	);
}

void Layer::paint(SDL_Rect bounds, std::shared_ptr<const Image> image, const SDL_Rect *limit) {
	if (!image || bounds.w <= 0 || bounds.h <= 0)
		return;
	auto clip = intersection(bounds, {0, 0, width_, height_});
	if (limit)
		clip = intersection(clip, *limit);
	if (clip.w <= 0 || clip.h <= 0)
		return;
	Draw draw{bounds, std::move(image), std::vector<uint8_t>(size_t(bounds.w) * bounds.h)};
	for (int y = clip.y; y < clip.y + clip.h; ++y)
		std::fill_n(
			draw.visible.begin() + size_t(y - bounds.y) * bounds.w + clip.x - bounds.x, clip.w, 255
		);
	draws_.push_back(std::move(draw));
}

void Layer::remove(const Image *image) {
	draws_.erase(
		std::remove_if(
			draws_.begin(),
			draws_.end(),
			[image](const Draw &draw) { return draw.image.get() == image; }
		),
		draws_.end()
	);
}

void Layer::erase(SDL_Rect bounds, const uint8_t *mask, int pitch) {
	if (bounds.w <= 0 || bounds.h <= 0)
		return;
	for (auto &draw : draws_) {
		const auto clip = intersection(bounds, draw.bounds);
		for (int y = clip.y; y < clip.y + clip.h; ++y) {
			for (int x = clip.x; x < clip.x + clip.w; ++x) {
				if (!mask || mask[size_t(y - bounds.y) * pitch + x - bounds.x])
					draw.visible[size_t(y - draw.bounds.y) * draw.bounds.w + x - draw.bounds.x] = 0;
			}
		}
	}
	prune();
}

void Layer::copy(const Layer &source, SDL_Rect from, int x, int y, const uint8_t *mask, int pitch) {
	// Snapshot first: scrolling within one layer must behave like memmove.
	std::vector<Draw> copies;
	const SDL_Rect dest_bounds{0, 0, width_, height_};
	for (const auto &src : source.draws_) {
		const auto clip = intersection(from, src.bounds);
		if (clip.w <= 0 || clip.h <= 0)
			continue;
		Draw dst = src;
		dst.bounds.x += x - from.x;
		dst.bounds.y += y - from.y;
		std::fill(dst.visible.begin(), dst.visible.end(), 0);
		for (int sy = clip.y; sy < clip.y + clip.h; ++sy) {
			for (int sx = clip.x; sx < clip.x + clip.w; ++sx) {
				const int dx = sx + x - from.x, dy = sy + y - from.y;
				if (dx < 0 || dy < 0 || dx >= dest_bounds.w || dy >= dest_bounds.h)
					continue;
				if (mask && !mask[size_t(sy - from.y) * pitch + sx - from.x])
					continue;
				const auto offset = size_t(sy - src.bounds.y) * src.bounds.w + sx - src.bounds.x;
				dst.visible[offset] = src.visible[offset];
			}
		}
		copies.push_back(std::move(dst));
	}
	draws_.insert(draws_.end(), copies.begin(), copies.end());
	prune();
}

std::vector<uint32_t> Layer::render(
	int width,
	int height,
	const SDL_Color *palette,
	const uint8_t *occlusion,
	int pitch
) const {
	if (width <= 0 || height <= 0 || width_ <= 0 || height_ <= 0 || !palette)
		return {};
	std::vector<uint32_t> output(size_t(width) * height);
	const double scale_x = double(width) / width_, scale_y = double(height) / height_;
	for (const auto &draw : draws_) {
		const int left = int(std::lround(draw.bounds.x * scale_x));
		const int top = int(std::lround(draw.bounds.y * scale_y));
		const int right = int(std::lround((draw.bounds.x + draw.bounds.w) * scale_x));
		const int bottom = int(std::lround((draw.bounds.y + draw.bounds.h) * scale_y));
		if (right <= left || bottom <= top)
			continue;
		const auto raster = draw.image->rasterize(right - left, bottom - top);
		if (raster.width != right - left || raster.height != bottom - top ||
			raster.pixels.size() != size_t(raster.width) * raster.height)
			throw std::runtime_error("Invalid native text raster dimensions");
		for (int y = std::max(0, top); y < std::min(height, bottom); ++y) {
			const int local_y =
				std::min(draw.bounds.h - 1, int((y - top + 0.5) * draw.bounds.h / raster.height));
			for (int x = std::max(0, left); x < std::min(width, right); ++x) {
				const int local_x = std::min(
					draw.bounds.w - 1, int((x - left + 0.5) * draw.bounds.w / raster.width)
				);
				if (!draw.visible[size_t(local_y) * draw.bounds.w + local_x])
					continue;
				const int logical_x = local_x + draw.bounds.x, logical_y = local_y + draw.bounds.y;
				if (occlusion && occlusion[size_t(logical_y) * pitch + logical_x])
					continue;
				const auto pixel = raster.pixels[size_t(y - top) * raster.width + x - left];
				if (pixel.border_alpha)
					output[size_t(y) * width + x] = over(
						output[size_t(y) * width + x], {pixel.border, pixel.border_alpha}, palette
					);
				if (pixel.alpha)
					output[size_t(y) * width + x] =
						over(output[size_t(y) * width + x], pixel, palette);
			}
		}
	}
	return output;
}

} // namespace xgr::text
