#ifndef VANGERS_ISCREEN_RELIEF_FONT_H
#define VANGERS_ISCREEN_RELIEF_FONT_H

#include "actint/ttf_font.h"
#include "hfont.h"

#include <map>

namespace vangers::iscreen {

// The outline defines the footprint, not the lighting. Retain the source's
// signed height profile; scale/smooth/terrain illumination remain separate.
struct ReliefGlyph {
	int advance = 0;
	SDL_Rect bounds{};
	std::shared_ptr<const xgr::text::Image> outline;
	int source_width = 0, source_height = 0, source_left = 0;
	std::vector<uint8_t> source;
	std::vector<int> profile;
	bool bitmap = false;
	std::vector<uint8_t> heights(int width, int height) const;
};

class ReliefFont {
  public:
	ReliefFont(const HFont &source, actint::FontFamily family, bool icons_only = false);
	const ReliefGlyph &glyph(char32_t codepoint);
	int width(std::string_view utf8, int spacing);

  private:
	const HFont &source_;
	actint::TtfFont font_;
	bool icons_only_;
	std::map<char32_t, ReliefGlyph> glyphs_;
};

// Runtime faces follow the lifetime of HFntTable; no global reinterpretation.
ReliefFont *relief_font(int index);
void free_relief_fonts();

} // namespace vangers::iscreen

#endif
