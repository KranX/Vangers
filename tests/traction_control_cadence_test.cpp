#include <algorithm>
#include <iostream>

#include "3d/traction_control.h"

namespace {

constexpr int TRACTION_MAX = 256;
constexpr int TRACTION_INCREMENT = 44;
constexpr int TRACTION_DECREMENT = 12;

bool check(bool condition, const char *message) {
	if (!condition)
		std::cerr << message << '\n';
	return condition;
}

bool test_cadence_phases() {
	for (int frame = 0; frame < 4; ++frame) {
		if (!check(
				vangers::physics::traction_control_input_step(frame, 1),
				"20 FPS input skipped a frame"
			) ||
			!check(
				vangers::physics::traction_control_decay_step(frame, 1),
				"20 FPS decay skipped a frame"
			))
			return false;
	}

	for (int frame = 0; frame < 6; ++frame) {
		const bool expected_input = frame == 0 || frame == 3;
		const bool expected_decay = frame == 2 || frame == 5;
		if (!check(
				vangers::physics::traction_control_input_step(frame, 3) == expected_input,
				"60 FPS input is not at the start of a legacy quantum"
			) ||
			!check(
				vangers::physics::traction_control_decay_step(frame, 3) == expected_decay,
				"60 FPS decay is not at the end of a legacy quantum"
			))
			return false;
	}

	return true;
}

bool test_held_keyboard_matches_full_gamepad() {
	int keyboard_traction = TRACTION_MAX - TRACTION_DECREMENT;
	int gamepad_traction = keyboard_traction;

	for (int frame = 0; frame < 6; ++frame) {
		if (vangers::physics::traction_control_input_step(frame, 3))
			keyboard_traction = std::min(keyboard_traction + TRACTION_INCREMENT, TRACTION_MAX);

		gamepad_traction = TRACTION_MAX;
		if (!check(
				keyboard_traction == TRACTION_MAX,
				"held keyboard traction dropped during a 60 FPS legacy quantum"
			) ||
			!check(
				gamepad_traction == keyboard_traction,
				"full gamepad traction exceeded held keyboard traction"
			))
			return false;

		if (vangers::physics::traction_control_decay_step(frame, 3)) {
			keyboard_traction -= TRACTION_DECREMENT;
			gamepad_traction -= TRACTION_DECREMENT;
		}
	}

	return true;
}

bool test_released_keyboard_decays_once_per_legacy_quantum() {
	int traction = TRACTION_MAX - TRACTION_DECREMENT;

	for (int frame = 0; frame < 6; ++frame) {
		const int expected =
			frame < 3 ? TRACTION_MAX - TRACTION_DECREMENT : TRACTION_MAX - 2 * TRACTION_DECREMENT;
		if (!check(traction == expected, "released traction changed inside a legacy quantum"))
			return false;

		if (vangers::physics::traction_control_decay_step(frame, 3))
			traction -= TRACTION_DECREMENT;
	}

	return true;
}

} // namespace

int main() {
	return test_cadence_phases() && test_held_keyboard_matches_full_gamepad() &&
				   test_released_keyboard_decays_once_per_legacy_quantum()
			   ? 0
			   : 1;
}
