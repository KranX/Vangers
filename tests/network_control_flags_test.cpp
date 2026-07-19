#include <iostream>

#include <SDL3/SDL.h>

#include "3d/3dobject.h"

namespace {

bool check(bool condition, const char *message) {
	if (!condition)
		std::cerr << message << '\n';
	return condition;
}

} // namespace

int main() {
	const int transferable_mask = (1 << CONTROLS::NUMBER_OF_TRANSFERABLE_CONTROLS) - 1;
	const int all_metadata_flags = NETWORK_CONTROL_MOLE_FLAG |
								   NETWORK_CONTROL_ANALOG_STEERING_FLAG |
								   NETWORK_CONTROL_ANALOG_THROTTLE_FLAG;

	if (!check((transferable_mask & all_metadata_flags) == 0, "metadata overlaps controls") ||
		!check(
			(NETWORK_CONTROL_MOLE_FLAG & NETWORK_CONTROL_ANALOG_STEERING_FLAG) == 0,
			"analog steering overlaps mole state"
		) ||
		!check(
			(NETWORK_CONTROL_MOLE_FLAG & NETWORK_CONTROL_ANALOG_THROTTLE_FLAG) == 0,
			"analog throttle overlaps mole state"
		))
		return 1;

	const int analog_steering = NETWORK_CONTROL_ANALOG_STEERING_FLAG | (1 << CONTROLS::STEER_LEFT) |
								(1 << CONTROLS::TURBO_QUANT);
	if (!check(
			network_control_uses_analog_value(analog_steering, CONTROLS::STEER_LEFT),
			"analog steering did not suppress digital left steering"
		) ||
		!check(
			network_control_uses_analog_value(analog_steering, CONTROLS::STEER_RIGHT),
			"analog steering did not suppress digital right steering"
		) ||
		!check(
			!network_control_uses_analog_value(analog_steering, CONTROLS::TRACTION_INCREASE),
			"analog steering suppressed digital traction"
		) ||
		!check(
			!network_control_uses_analog_value(analog_steering, CONTROLS::TURBO_QUANT),
			"analog steering suppressed an unrelated control"
		))
		return 1;

	const int analog_throttle =
		NETWORK_CONTROL_ANALOG_THROTTLE_FLAG | (1 << CONTROLS::TRACTION_DECREASE);
	if (!check(
			network_control_uses_analog_value(analog_throttle, CONTROLS::TRACTION_INCREASE),
			"analog throttle did not suppress digital forward traction"
		) ||
		!check(
			network_control_uses_analog_value(analog_throttle, CONTROLS::TRACTION_DECREASE),
			"analog throttle did not suppress digital reverse traction"
		) ||
		!check(
			!network_control_uses_analog_value(analog_throttle, CONTROLS::STEER_LEFT),
			"analog throttle suppressed digital steering"
		))
		return 1;

	const int encoded = NETWORK_CONTROL_MOLE_FLAG | NETWORK_CONTROL_ANALOG_STEERING_FLAG |
						NETWORK_CONTROL_ANALOG_THROTTLE_FLAG | (1 << CONTROLS::STEER_RIGHT);
	const short wire_value = static_cast<short>(encoded);
	return check(static_cast<int>(wire_value) == encoded, "control flags do not fit the wire field")
			   ? 0
			   : 1;
}
