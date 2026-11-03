#ifndef VANGERS_TEXT_LAYER_H
#define VANGERS_TEXT_LAYER_H

#include <SDL3/SDL.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace xgr::text {

// Coverage is independent of palette colour: fades must not require glyph reloads.
struct Pixel {
	uint8_t colour = 0;
	uint8_t alpha = 0;
	uint8_t border = 0;
	uint8_t border_alpha = 0;
};

struct Raster {
	int width = 0, height = 0;
	std::vector<Pixel> pixels;
};

class Image {
  public:
	virtual ~Image() = default;
	virtual Raster rasterize(int width, int height) const = 0;
};

// A native-resolution companion to a logical, indexed surface. Bitmap writes
// erase coverage; copies carry coverage with them. It is not a second UI tree.
// Masks use logical pixels only for occlusion, never for the glyph contours.
class Layer {
  public:
	Layer(int width = 0, int height = 0);
	void clear();
	bool empty() const;
	void paint(SDL_Rect bounds, std::shared_ptr<const Image> image, const SDL_Rect *clip = nullptr);
	// Remove this caller-owned image without erasing neighbouring text.
	void remove(const Image *image);
	void erase(SDL_Rect bounds, const uint8_t *mask = nullptr, int pitch = 0);
	void copy(
		const Layer &source,
		SDL_Rect from,
		int x,
		int y,
		const uint8_t *mask = nullptr,
		int pitch = 0
	);
	// Premultiplied ARGB8888 output; the caller supplies the current palette.
	std::vector<uint32_t> render(
		int width,
		int height,
		const SDL_Color *palette,
		const uint8_t *occlusion = nullptr,
		int pitch = 0
	) const;
	int width() const {
		return width_;
	}
	int height() const {
		return height_;
	}

  private:
	struct Draw {
		SDL_Rect bounds;
		std::shared_ptr<const Image> image;
		std::vector<uint8_t> visible;
	};
	int width_, height_;
	std::vector<Draw> draws_;
	void prune();
};

// Only explicitly registered surfaces participate. The bitmap path keeps this
// disabled, and unrelated render buffers never acquire text state implicitly.
extern bool enabled;
Layer *find(const void *buffer);
Layer *prepare(const void *buffer, int width, int height, bool clear = true);
void forget(const void *buffer);
void erase(const void *buffer, SDL_Rect bounds, const uint8_t *mask = nullptr, int pitch = 0);
void clear(const void *buffer);

} // namespace xgr::text

#endif
