#ifndef VANGERS_TRACTION_CONTROL_H
#define VANGERS_TRACTION_CONTROL_H

namespace vangers::physics {

constexpr bool traction_control_input_step(int frame, int step_ticks) {
	return step_ticks <= 1 || frame % step_ticks == 0;
}

constexpr bool traction_control_decay_step(int frame, int step_ticks) {
	return step_ticks <= 1 || frame % step_ticks == step_ticks - 1;
}

} // namespace vangers::physics

#endif
