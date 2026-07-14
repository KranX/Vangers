#ifndef VANGERS_FILE_UTILS_H
#define VANGERS_FILE_UTILS_H

#include <cstring>

namespace vangers::files {

inline bool is_visible_directory_entry(const char *name) {
	return name && name[0] && name[0] != '.';
}

inline bool has_exact_extension(const char *name, const char *extension) {
	if (!name || !extension)
		return false;

	const std::size_t name_length = std::strlen(name);
	const std::size_t extension_length = std::strlen(extension);
	return extension_length <= name_length &&
		   std::strcmp(name + name_length - extension_length, extension) == 0;
}

inline bool is_resource_file_name(const char *name, const char *extension) {
	return is_visible_directory_entry(name) && has_exact_extension(name, extension);
}

} // namespace vangers::files

#endif
