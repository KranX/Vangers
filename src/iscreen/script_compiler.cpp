#include "script_compiler.h"

#include "../global.h"
#include "iscreen.h"

#include <SDL3/SDL_filesystem.h>

#include <iostream>
#include <string>

void InitScreenFonts(void);

void CompileIScreenScript(const char *source_path, const char *output_path) {
	const std::string temporary_path = std::string(output_path) + ".tmp";
	SDL_RemovePath(temporary_path.c_str());

	InitScreenFonts();
	ParseScript(source_path, temporary_path.c_str(), false);

	if (!SDL_RenamePath(temporary_path.c_str(), output_path)) {
		XBuffer message(1024);
		message < "Unable to replace interface binary script '" < output_path < "': " <
			SDL_GetError();
		ErrH.Abort(message.GetBuf(), XERR_USER);
	}

	std::cout << "Compiled interface script: " << source_path << " -> " << output_path << std::endl;
}
