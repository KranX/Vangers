#ifndef VANGERS_GLUEK_ARMOR_H
#define VANGERS_GLUEK_ARMOR_H

#include <cstdint>

namespace vangers::items {

inline int restore_gluek_armor(int armor, int max_armor) {
	// Armor is 16.16 fixed-point. Widen before adding so the cap cannot overflow.
	const std::int64_t healed = static_cast<std::int64_t>(armor) + (10 << 16);
	return healed > max_armor ? max_armor : static_cast<int>(healed);
}

} // namespace vangers::items

#endif
