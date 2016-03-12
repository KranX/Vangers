
/* ------------------------- Options Screen Defines ------------------------- */

#define LANG_Y		150
#define LANG_STR_X	100
#define LANG_TRG_X	500

#define MEM_Y		220
#define MEM_STR_X	100
#define MEM_TRG_X	400

#define SND_Y		290
#define SND_STR_X	50
#define SND_TRG_X	550

#define SND2_Y		360
#define SND2_STR_X	70
#define SND_SCR_X	470

#define OK_X		310
#define OK_Y		490

/* -------------------------------------------------------------------------- */

// Terrains...
#define TERRAIN2	2
#define TERRAIN3	3
#define TERRAIN4	4

#define DEFAULT_NULL	150

#define TEXT_BT_NULL		90
#define TEXT_BT_NULL_LEV	128

#define TEXT_BT_Y	380

#define TEXT_BT_DELTA	100
#define TEXT01_PLAY_X	250

num_fonts	5
default_scr	"Main Screen"

#define BUG_OBJ 	Object $BUG_OBJ_ID {						\
				$BUG_COORDS						\
				shadow_size	15					\
											\
				Element BITMAP {					\
					bmp_null_level	0				\
					null_level	128				\
											\
					bmp_file	$BUG_OBJ_BMP			\
				}							\
				Event { 						\
					key $VK_LBUTTON 				\
											\
					time { $PRESS_OBJ_TIME * 2 + 1 }		\
					EvComm {					\
						scale_obj	$BUG_OBJ_ID	   -200 \
						start_time	0			\
						time		$PRESS_OBJ_TIME 	\
					}						\
					EvComm {					\
						smooth_obj	$BUG_OBJ_ID	   2	\
						start_time	0			\
						time		$PRESS_OBJ_TIME 	\
					}						\
					EvComm {					\
						rise_obj	$BUG_OBJ_ID		\
						start_time	$PRESS_OBJ_TIME 	\
						time		$PRESS_OBJ_TIME 	\
					}						\
					EvComm {					\
						smooth_obj	$BUG_OBJ_ID	0	\
						start_time	$PRESS_OBJ_TIME 	\
						time		$PRESS_OBJ_TIME 	\
					}						\
				}							\
				Event { 						\
					key $VK_RBUTTON 				\
											\
					time { $SMOOTH_OBJ_TIME * 2 + 1 }		\
					EvComm {					\
						smooth_obj	$BUG_OBJ_ID	   3	\
						start_time	0			\
						time		$SMOOTH_OBJ_TIME	\
					}						\
					EvComm {					\
						smooth_obj	$BUG_OBJ_ID	   0	\
						start_time	$SMOOTH_OBJ_TIME	\
						time		$SMOOTH_OBJ_TIME	\
					}						\
				}							\
			}

#define BUG_OBJ_ID	"Bug00"
#define BUG_OBJ_BMP	"resource/iscreen/bitmap/bug00.bmp"
#define BUG_COORDS	$BUG00_COORDS
$BUG_OBJ

#define BUG_OBJ_ID	"Bug01"
#define BUG_OBJ_BMP	"resource/iscreen/bitmap/bug01.bmp"
#define BUG_COORDS	$BUG01_COORDS
$BUG_OBJ

#define BUG_OBJ_ID	"Bug02"
#define BUG_OBJ_BMP	"resource/iscreen/bitmap/bug02.bmp"
#define BUG_COORDS	$BUG02_COORDS
$BUG_OBJ

#define BUG_OBJ_ID	"Bug03"
#define BUG_OBJ_BMP	"resource/iscreen/bitmap/bug03.bmp"
#define BUG_COORDS	$BUG03_COORDS
$BUG_OBJ

#define BUG_OBJ_ID	"Bug04"
#define BUG_OBJ_BMP	"resource/iscreen/bitmap/bug04.bmp"
#define BUG_COORDS	$BUG04_COORDS
$BUG_OBJ

#define BUG_OBJ_ID	"Bug05"
#define BUG_OBJ_BMP	"resource/iscreen/bitmap/bug05.bmp"
#define BUG_COORDS	$BUG05_COORDS
$BUG_OBJ

#define BUG_OBJ_ID	"Bug06"
#define BUG_OBJ_BMP	"resource/iscreen/bitmap/bug06.bmp"
#define BUG_COORDS	$BUG06_COORDS
$BUG_OBJ

#define BUG_OBJ_ID	"Bug07"
#define BUG_OBJ_BMP	"resource/iscreen/bitmap/bug07.bmp"
#define BUG_COORDS	$BUG07_COORDS
$BUG_OBJ

#define BUG_OBJ_ID	"Bug08"
#define BUG_OBJ_BMP	"resource/iscreen/bitmap/bug08.bmp"
#define BUG_COORDS	$BUG08_COORDS
$BUG_OBJ

#define BUG_OBJ_ID	"Bug09"
#define BUG_OBJ_BMP	"resource/iscreen/bitmap/bug09.bmp"
#define BUG_COORDS	$BUG09_COORDS
$BUG_OBJ

#define BUG_OBJ_ID	"Bug10"
#define BUG_OBJ_BMP	"resource/iscreen/bitmap/bug10.bmp"
#define BUG_COORDS	$BUG10_COORDS
$BUG_OBJ

Screen "TextScreen00"
{
	screen_offs		800
	default_obj		"Text00"

	Event {
		key	$VK_ESCAPE

		time 11

		EvComm {
			move2screen	"Main Screen"
			start_time	0
			time		10
		}
	}

	Event {
		scancode $iTEXT_NEXT_EVENT_CODE0

		time 13

		EvComm {
			next_text	"TextScreen01"  0
			start_time	0
		}
		EvComm {
			move2screen	"TextScreen01"
			start_time	1
			time		10
		}
	}
	Event {
		scancode $iTEXT_PREV_EVENT_CODE0

		time 13

		EvComm {
			prev_text	"TextScreen01"  0
			start_time	0
		}
		EvComm {
			move2screen	"TextScreen01"
			start_time	1
			time		10
		}
	}
	Event {
		scancode $iTEXT_END_EVENT_CODE0

		time 13

		EvComm {
			move2screen	"LOADING"
			start_time	1
			time		10
		}
		EvComm {
			exit		1
			start_time	12
		}
	}

	Object "TitleObj" {
		align_x 	center
		y		20

		Element "STRING" {
			id		"TextTitle00"

			null_level	$DEFAULT_NULL

			string		"XXXXXXXXXXXXXX"

			terrain_num	$TERRAIN4

			align_x 	center
			align_y 	center
		}
	}

	#define BUTTON_NAME	"NextText"
	Object "NextText" {
		align_x center
		align_x_offs	$TEXT_BT_DELTA
		y	$TEXT_BT_Y

		Element BITMAP {
			bmp_null_level	$TEXT_BT_NULL
			null_level	$TEXT_BT_NULL_LEV

			terrain_num	$TERRAIN2

			bmp_file	"resource/iscreen/bitmap/next_txt.bmp"
		}
		Event {
			key $VK_LBUTTON
			key $VK_RBUTTON
			key $VK_RIGHT
			key $VK_RETURN
			key $VK_TAB

			time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 12 }

			EvComm {
				next_text	"TextScreen00"  0
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME }
			}
			EvComm {
				move2screen	"TextScreen00"
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }
				time		10
			}

			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SCALE_STRENGTH	0
			#define BUTTON_SMOOTH_INDEX	4
		}
	}
	#define BUTTON_NAME	"PrevText"
	Object "PrevText" {
		align_x center
		align_x_offs	{ 0 - $TEXT_BT_DELTA }
		y		$TEXT_BT_Y

		Element BITMAP {
			bmp_null_level	$TEXT_BT_NULL
			null_level	$TEXT_BT_NULL_LEV

			terrain_num	$TERRAIN2

			bmp_file	"resource/iscreen/bitmap/prev_txt.bmp"
		}
		Event {
			key $VK_LBUTTON
			key $VK_RBUTTON
			key $VK_LEFT

			time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 12 }

			EvComm {
				prev_text	"TextScreen00"  0
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME }
			}
			EvComm {
				move2screen	"TextScreen00"
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }
				time		10
			}

			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SCALE_STRENGTH	0
			#define BUTTON_SMOOTH_INDEX	4
		}
	}
	#define BUTTON_NAME	"PlayBT"
	Object "PlayBT" {
		align_x center
		y		$TEXT_BT_Y

		Element BITMAP {
			bmp_null_level	$TEXT_BT_NULL
			null_level	$TEXT_BT_NULL_LEV

			terrain_num	$TERRAIN4

			bmp_file	"resource/iscreen/bitmap/play_bt.bmp"
		}
		Event {
			key $VK_LBUTTON
			key $VK_RBUTTON
			key $VK_SPACE

			time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 12 }

			EvComm {
				move2screen	"LOADING"
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME }
				time		10
			}
			EvComm {
				exit		1
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 11 }
			}

			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SCALE_STRENGTH	0
			#define BUTTON_SMOOTH_INDEX	4
		}
	}

	Object "Text00"{
		null_level	200
		align_x center

		y	70

		space	2

		font	0
		s_text	  "resource/iscreen/text/empty.txt"
	}
}

Screen "TextScreen01"
{
	screen_offs		0
	default_obj		"Text00"

	Event {
		key	$VK_ESCAPE

		time 11

		EvComm {
			move2screen	"Main Screen"
			start_time	0
			time		10
		}
	}

	Event {
		scancode $iTEXT_NEXT_EVENT_CODE1

		time 13

		EvComm {
			next_text	"TextScreen00"  1
			start_time	0
		}
		EvComm {
			move2screen	"TextScreen00"
			start_time	1
			time		10
		}
	}
	Event {
		scancode $iTEXT_PREV_EVENT_CODE1

		time 13

		EvComm {
			prev_text	"TextScreen00"  1
			start_time	0
		}
		EvComm {
			move2screen	"TextScreen00"
			start_time	1
			time		10
		}
	}

	Event {
		scancode $iTEXT_END_EVENT_CODE1

		time 13

		EvComm {
			move2screen	"LOADING"
			start_time	1
			time		10
		}
		EvComm {
			exit		1
			start_time	12
		}
	}

	Object "TitleObj" {
		align_x 	center
		y		20

		Element "STRING" {
			id		"TextTitle00"

			null_level	$DEFAULT_NULL

			string		"XXXXXXXXXXXXXX"

			terrain_num	$TERRAIN4

			align_x 	center
			align_y 	center
		}
	}

	#define BUTTON_NAME	"NextText"
	Object "NextText" {
		align_x center
		align_x_offs	 $TEXT_BT_DELTA 
		y		$TEXT_BT_Y

		Element BITMAP {
			bmp_null_level	$TEXT_BT_NULL
			null_level	$TEXT_BT_NULL_LEV

			terrain_num	$TERRAIN2

			bmp_file	"resource/iscreen/bitmap/next_txt.bmp"
		}
		Event {
			key $VK_LBUTTON
			key $VK_RBUTTON
			key $VK_RIGHT
			key $VK_RETURN
			key $VK_TAB

			time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 12 }

			EvComm {
				next_text	"TextScreen01"  1
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME }
			}
			EvComm {
				move2screen	"TextScreen01"
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }
				time		10
			}

			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SCALE_STRENGTH	0
			#define BUTTON_SMOOTH_INDEX	4
		}
	}
	#define BUTTON_NAME	"PrevText"
	Object "PrevText" {
		align_x center
		align_x_offs	{ 0 - $TEXT_BT_DELTA }
		y		$TEXT_BT_Y

		Element BITMAP {
			bmp_null_level	$TEXT_BT_NULL
			null_level	$TEXT_BT_NULL_LEV

			terrain_num	$TERRAIN2

			bmp_file	"resource/iscreen/bitmap/prev_txt.bmp"
		}
		Event {
			key $VK_LBUTTON
			key $VK_RBUTTON
			key $VK_LEFT

			time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 12 }

			EvComm {
				prev_text	"TextScreen01"  1
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME }
			}
			EvComm {
				move2screen	"TextScreen01"
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }
				time		10
			}

			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SCALE_STRENGTH	0
			#define BUTTON_SMOOTH_INDEX	4
		}
	}
	#define BUTTON_NAME	"PlayBT"
	Object "PlayBT" {
		align_x 	center
		y		$TEXT_BT_Y

		Element BITMAP {
			bmp_null_level	$TEXT_BT_NULL
			null_level	$TEXT_BT_NULL_LEV

			terrain_num	$TERRAIN4

			bmp_file	"resource/iscreen/bitmap/play_bt.bmp"
		}
		Event {
			key $VK_LBUTTON
			key $VK_RBUTTON
			key $VK_SPACE

			time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 12 }

			EvComm {
				move2screen	"LOADING"
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME }
				time		10
			}
			EvComm {
				exit		1
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 11 }
			}

			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SCALE_STRENGTH	0
			#define BUTTON_SMOOTH_INDEX	4
		}
	}

	Object "Text00"{
		null_level	200
		align_x center

		y	70

		space	2

		font	0
		s_text	  "resource/iscreen/text/empty.txt"
	}
}

#include "iscreen/scripts/mainmenu.scr"

Screen "LOADING"
{
	default_obj "Loading0"
	Event {
		key		$VK_ESCAPE
		EvComm {
			exit	1
		}
	}

	Object "Loading0"
	{
		y	190
		align_x center
		Element STRING {
			font		1

			terrain_num	$TERRAIN2

			null_level	$DEFAULT_NULL
			string		$iSTR_ACHTUNG
		}
	}
	Object "Loading1"
	{
		y	250
		align_x center
		Element STRING {

			terrain_num	$TERRAIN2

			font		0
			null_level	$DEFAULT_NULL
			string		$iSTR_VANGERS_APPROACHING
		}
	}
}

Screen "LOADING2"
{
	screen_offs	800
	default_obj "Loading0"
	Event {
		key		$VK_ESCAPE
		EvComm {
			exit	1
		}
	}

	Object "Loading0"
	{
		y	190
		align_x center
		Element STRING {
			font		1

			terrain_num	$TERRAIN2

			null_level	$DEFAULT_NULL
			string		$iSTR_ACHTUNG
		}
	}
	Object "Loading1"
	{
		y	250
		align_x center
		Element STRING {
			font		0

			terrain_num	$TERRAIN2

			null_level	$DEFAULT_NULL
			string		$iSTR_VANGERS_APPROACHING
		}
	}
}

/* ------------------------------- LOCATIONS -------------------------------- */

// Podish
#include "iscreen/scripts/l0.inc"

// Incubator
#include "iscreen/scripts/l1.inc"

// VigBoo
#include "iscreen/scripts/l2.inc"

// Lampasy
#include "iscreen/scripts/l3.inc"

// Ogorod
#include "iscreen/scripts/l4.inc"

// ZeePa
#include "iscreen/scripts/l5.inc"

// B-Zone
#include "iscreen/scripts/l6.inc"

// Spobs
#include "iscreen/scripts/l7.inc"

/* -------------------------------------------------------------------------- */
