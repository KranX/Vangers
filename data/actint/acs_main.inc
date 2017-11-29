
#define ACS_SLOT_LEN		15
#define SAVE_SLOT_DELTA_Y	48

#define ACS_NONE	0
#define ACS_BMP 	1
#define ACS_BML 	2
#define ACS_BMO 	3
#define ACS_XBM 	4

// aciScreenObject:type values...
#define ACS_BASE_OBJ		1
#define ACS_INPUT_FIELD_OBJ	2
#define ACS_SCROLLER_OBJ	3

#define COLOR0		{  88  + 1 * 256 }
#define COLOR0s 	{  160 + 1 * 256 }
#define COLOR1		{  128 + 1 * 256 }
#define COLOR2		{   0 + 0 * 256 }

#define PAUSE_RES_X	640
#define STR_DELTA_X	25
#define STR_START_Y	120

screen {
	id		$ACS_PAUSE_SCREEN1
	background	$ACS_PAUSE_BACK0

	resource $ACS_BMP {
		id	$ACS_VOLUME_SCROLLER
		name	"resource/actint/640x480/pause/scr.bmp"
	}
	resource $ACS_BMP {
		id	$ACS_VOLUME_S_RES
		name	"resource/actint/640x480/pause/volume.bmp"
	}
	resource $ACS_BMO {
		id	$ACS_PAUSE_BACK0
/*
		build
		offs_x	0
		offs_y	0

		size_x	10
		size_y	10
*/
		name	"resource/actint/640x480/pause/back.bmo"
	}
	#include "actint/pause.inc"
}

#define COLOR0		{ 96 + 1 * 256 }
#define COLOR0s 	{ 48 + 1 * 256 }
#define COLOR1		{ 16 + 1 * 256 }
#define COLOR2		{ 64 + 1 * 256 }

#define PAUSE_RES_X	800
#define STR_DELTA_X	100
#define STR_START_Y	180

screen {
	id		$ACS_PAUSE_SCREEN2
	background	$ACS_PAUSE_BACK1

	resource $ACS_BMP {
		id	$ACS_VOLUME_SCROLLER
		name	"resource/actint/iscreen/pause/scr.bmp"
	}
	resource $ACS_BMP {
		id	$ACS_VOLUME_S_RES
		name	"resource/actint/iscreen/pause/volume.bmp"
	}
	resource $ACS_BMO {
		id	$ACS_PAUSE_BACK1
/*
		build
		offs_x	0
		offs_y	0

		size_x	10
		size_y	10
*/
		name	"resource/actint/iscreen/pause/back.bmo"
	}
	#include "actint/pause.inc"
}

screen {
	id		$ACS_SAVE_SCREEN0
	background	$ACS_SAVE_BACK

	resource $ACS_BMP {
		id	$ACS_SAVE_BACK
		name	"resource/actint/iscreen/save/scr00/back.bmp"
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_OK
		name	"resource/actint/iscreen/save/scr00/ok.bmo"
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_CANCEL
		name	"resource/actint/iscreen/save/scr00/cancel.bmo"
	}
	#define SAVE_STRING_POS_X	250
	#define SAVE_STRING_POS_Y	40

	#define SAVE_SLOT_OFFS_X	225
	#define SAVE_SLOT_START_Y	140
	#include "actint/save_scr.inc"
}

screen {
	id		$ACS_SAVE_SCREEN1
	background	$ACS_SAVE_BACK

	resource $ACS_BMP {
		id	$ACS_SAVE_BACK
		name	"resource/actint/iscreen/save/scr01/back.bmp"
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_OK
		name	"resource/actint/iscreen/save/scr01/ok.bmo"
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_CANCEL
		name	"resource/actint/iscreen/save/scr01/cancel.bmo"
	}
	#define SAVE_STRING_POS_X	250
	#define SAVE_STRING_POS_Y	40

	#define SAVE_SLOT_OFFS_X	215
	#define SAVE_SLOT_START_Y	155
	#include "actint/save_scr.inc"
}

screen {
	id		$ACS_SAVE_SCREEN2
	background	$ACS_SAVE_BACK

	resource $ACS_BMP {
		id	$ACS_SAVE_BACK
		name	"resource/actint/iscreen/save/scr02/back.bmp"
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_OK
		name	"resource/actint/iscreen/save/scr02/ok.bmo"
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_CANCEL
		name	"resource/actint/iscreen/save/scr02/cancel.bmo"
	}
	#define SAVE_STRING_POS_X	220
	#define SAVE_STRING_POS_Y	40

	#define SAVE_SLOT_OFFS_X	200
	#define SAVE_SLOT_START_Y	145
	#include "actint/save_scr.inc"
}



