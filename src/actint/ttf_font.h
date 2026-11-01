#ifndef VANGERS_ACTINT_TTF_FONT_H
#define VANGERS_ACTINT_TTF_FONT_H

#include "text_layer.h"

#include <memory>
#include <string>
#include <string_view>

namespace vangers::actint {

struct FontIcon {
	int left = 0, right = 0;
	std::vector<uint8_t> pixels;
};

struct FontStyle {
	int cell_width = 10;
	int cell_height = 18;
	int spacing = 1;
	int line_spacing = 1;
	uint32_t colour = 255;
	uint32_t colour_size = 0;
	bool range = false;
	bool highlight_numbers = true;
	std::shared_ptr<const std::vector<FontIcon>> icons;
};

struct TextLayout {
	int width = 0, height = 0;
	SDL_Rect ink_bounds{};
	std::shared_ptr<const xgr::text::Image> image;
};

// One face, with the historical fixed vertical placement encoded in its glyphs.
// Layout uses font units; resizing a window must not change wrapping/hit boxes.
class TtfFont {
  public:
	TtfFont();
	explicit TtfFont(const std::string &path);
	~TtfFont();
	TtfFont(const TtfFont &) = delete;
	TtfFont &operator=(const TtfFont &) = delete;
	TextLayout layout(std::string_view text, const FontStyle &style) const;

  private:
	struct Face;
	std::shared_ptr<Face> face_;
};

} // namespace vangers::actint

#endif
