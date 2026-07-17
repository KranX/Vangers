#ifndef VANGERS_SETTINGS_INPUT_BINDING_H
#define VANGERS_SETTINGS_INPUT_BINDING_H

#include <optional>
#include <string>
#include <string_view>

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

const char *legacy_control_action_name(int control_id);
std::optional<int> legacy_control_action_id(std::string_view action);

DecodedLegacyBinding decode_legacy_control_code(int code);
std::optional<int> keyboard_binding_code(std::string_view name);
std::optional<int> gamepad_binding_code(std::string_view name);

} // namespace vangers::settings

#endif
