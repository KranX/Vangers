#include "zmod_client.h"

#include <SDL3/SDL.h>
#include <time.h>

int z_time = 0;
int z_time_tic = 0;
int z_time_cnt = 0;
int z_time_all = 0;
int z_time_avg = 0;
static Uint64 z_time_start = 0;

void z_time_init() {
	z_time_start = SDL_GetTicks();
	z_time_tic = static_cast<int>(z_time_start & 0x7fffffff);
}

void z_time_collect() {
	z_time = static_cast<int>(SDL_GetTicks() - z_time_start);
	z_time_all += z_time;
	++z_time_cnt;
	z_time_avg = z_time_all / z_time_cnt;
}

#ifdef _ZMOD_DEBUG_

FILE *ZLog;

void ZLogClose() {
	if (ZLog != NULL)
		fclose(ZLog);
}

void ZLogFlush() {
	if (ZLog != NULL)
		fflush(ZLog);
}

FILE *ZLogOpen() {
	FILE *F = fopen("zMod.log", "wb");
	return F;
}

#endif
