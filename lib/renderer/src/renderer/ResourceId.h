#ifndef RESOURCEID_H
#define RESOURCEID_H
#include <cstdint>

namespace renderer {
	template<typename T>
	struct ResourceId {
		int32_t id;
		typedef T type;

		static const ResourceId<T> Invalid;
		bool operator==(const ResourceId<T>& other) const noexcept {
			return other.id == id;
		}

		bool operator!=(const ResourceId<T>& other) const noexcept {
			return other.id != id;
		}

		bool is_valid() const noexcept {
			return id >= 0;
		}

	};

	template<typename T> const ResourceId<T> ResourceId<T>::Invalid = {-1};
}
#endif // RESOURCEID_H
