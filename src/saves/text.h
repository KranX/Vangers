#ifndef VANGERS_SAVE_TEXT_H
#define VANGERS_SAVE_TEXT_H
#include <optional>
#include <string>
#include <string_view>

namespace vangers::saves {
// High byte already contains compression flags. This bit explicitly selects UTF-8
// for both display strings; old files without it remain CP866.
constexpr int utf8_text = 2 << 24;
struct Text {
	std::string name, description;
};
// Length fields retain their existing one-byte limits. Reject oversized/invalid
// metadata before opening the destination, never truncate a UTF-8 string.
int text_header(std::string_view name, std::string_view description);
std::optional<Text> read_text(const char *filename);
} // namespace vangers::saves
#endif
