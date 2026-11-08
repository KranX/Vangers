#ifndef VANGERS_LOCALIZATION_DESCRIPTION_H
#define VANGERS_LOCALIZATION_DESCRIPTION_H

#include <cstdint>
#include <string_view>
#include <vector>

namespace vangers::localization {
enum class DescriptionKind { Item, Mechos };
enum class DescriptionStyle { RegularTop, RegularCentered, CompactCentered };

// Game IDs, not translated labels or AVI filenames. Nonempty keys are static,
// NUL-terminated strings; an unmapped ID returns a null data() for AVI fallback.
std::string_view description_key(DescriptionKind kind, int id);

// Same fixed canvas as the original description frames. Built only on selection;
// the existing AVI compositor supplies position, visibility and fade each frame.
class Description {
  public:
	static constexpr int width = 340, height = 255;
	explicit Description(
		std::string_view text,
		DescriptionStyle style = DescriptionStyle::RegularTop
	);
	void draw(uint32_t *target, int w, int h, int x, int y, float brightness) const;
	const std::vector<uint32_t> &pixels() const {
		return pixels_;
	}

  private:
	std::vector<uint32_t> pixels_;
};
} // namespace vangers::localization
#endif
