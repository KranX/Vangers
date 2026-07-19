#ifndef VANGERS_SETTINGS_INPUT_BINDING_H
#define VANGERS_SETTINGS_INPUT_BINDING_H

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace vangers::settings {

enum class LegacyBindingKind {
	Unassigned,
	Keyboard,
	Gamepad,
	Unsupported,
};

struct DecodedLegacyBinding {
	LegacyBindingKind kind = LegacyBindingKind::Unsupported;
	std::string name;
};

constexpr std::size_t LEGACY_CONTROL_BINDING_SLOTS = 2;
using LegacyControlCodes = std::array<int, LEGACY_CONTROL_BINDING_SLOTS>;

const char *legacy_control_action_name(int control_id);
std::optional<int> legacy_control_action_id(std::string_view action);

DecodedLegacyBinding decode_legacy_control_code(int code);
std::optional<int> keyboard_binding_code(std::string_view name);
std::optional<int> gamepad_binding_code(std::string_view name);

LegacyControlCodes encode_legacy_control_bindings(
	const std::vector<std::string> &keyboard,
	const std::vector<std::string> &gamepad
);
void merge_legacy_control_bindings(
	const LegacyControlCodes &codes,
	std::vector<std::string> &keyboard,
	std::vector<std::string> &gamepad
);

} // namespace vangers::settings

#endif
