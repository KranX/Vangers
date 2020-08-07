#ifndef _ZMOD_CLIENT_
#define _ZMOD_CLIENT_

#include "zmod_common.h"
#define zSTR_LOADING_800_RU	"(C)1997-2014. Все права защищены. Бета версия. Идет загрузка..."

#define zCHAT_TIMELIMIT	30000
#define zCHAT_ROWLIMIT	5
#define zCHAT_ROWHEIGHT	15.2

//#define zRECORDER_ENABLED

#define zCOLOR_ORANGE	255
#define zCOLOR_GREEN	254
#define zCOLOR_WHITE	223
#define zCOLOR_BLACK	191
#define zCOLOR_YELLOW	175
#define zCOLOR_BLUE		159
#define zCOLOR_RED		199
#define zCOLOR_SILVER	236
#define zCOLOR_GRAY		231
#define zCOLOR_TRANSPARENT -1
#define zCOLOR_CAMOUFLAGE		254
#define zCOLOR_PATROL		159

extern int z_time;
extern int z_time_tic;
extern int z_time_cnt;
extern int z_time_all;
extern int z_time_avg;

void z_time_init();
void z_time_collect();

#endif
