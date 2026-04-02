#include "ttf_runtime.h"

#include <SDL_ttf.h>

namespace text
{

namespace
{

bool g_ttf_runtime_owned = false;

}

bool init_ttf_runtime(void)
{
	if(TTF_WasInit()){
		g_ttf_runtime_owned = false;
		return true;
	}

	if(TTF_Init() == 0){
		g_ttf_runtime_owned = true;
		return true;
	}

	return false;
}

void shutdown_ttf_runtime(void)
{
	if(g_ttf_runtime_owned && TTF_WasInit())
		TTF_Quit();

	g_ttf_runtime_owned = false;
}

bool ttf_runtime_ready(void)
{
	return TTF_WasInit() != 0;
}

const char* ttf_runtime_error(void)
{
	return TTF_GetError();
}

}
