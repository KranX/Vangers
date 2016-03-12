
#define Credits0_More_Option_PosX	650
#define Credits0_More_Option_PosY	310
#define Credits0_Back_Option_PosX	10
#define Credits0_Back_Option_PosY	280

Screen "Credits0"
{
	screen_offs	0

	block_global_obj
	default_obj "Back Option"

	Object "Credits Text"{
		null_level	200
		align_x center
		y	20

		space	2

		font	0
		s_text	  "resource/iscreen/credits.txt"
	}

	Object "Back Option"
	{
		x	$Credits0_Back_Option_PosX
		y	$Credits0_Back_Option_PosY

		Element STRING {
			align_x center

			terrain_num	$TERRAIN2

			font		3
			space		2
			null_level	$DEFAULT_NULL
			string		$iSTR_BACK
		}
		Event {
			key $VK_LBUTTON
			key $VK_ESCAPE

			time { $PRESS_OBJ_TIME + $MOVE2SCREEN_TIME + 1 }
			EvComm {
				low_obj 	"Back Option"
				time		$PRESS_OBJ_TIME
				start_time	0
			}
			EvComm {
				move2screen	"Credits screen"
				start_time	$PRESS_OBJ_TIME
				time		$MOVE2SCREEN_TIME
			}
		}
		Event {
			key $VK_RBUTTON

			time { $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME + $MOVE2SCREEN_TIME + 1 }
			EvComm {
				smooth_obj	"Back Option" 2
				start_time	0
				time		$SMOOTH_OBJ_TIME
			}
			EvComm {
				hide_obj	"Back Option"
				start_time	$SMOOTH_OBJ_TIME
				time		$HIDE_OBJ_TIME
			}
			EvComm {
				move2screen	"Credits screen"
				start_time	{ $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME }
				time		$MOVE2SCREEN_TIME
			}
		}
	}
	Object "More Option"
	{
		x	$Credits0_More_Option_PosX
		y	$Credits0_More_Option_PosY

		Element STRING {
			align_x center

			terrain_num	$TERRAIN2

			font		3
			space		2
			null_level	$DEFAULT_NULL
			string		$iSTR_MORE
		}
		Event {
			key $VK_LBUTTON

			time { $PRESS_OBJ_TIME + $MOVE2SCREEN_TIME + 1 }
			EvComm {
				low_obj 	"More Option"
				time		$PRESS_OBJ_TIME
				start_time	0
			}
			EvComm {
				move2screen	"Credits3"
				start_time	$PRESS_OBJ_TIME
				time		$MOVE2SCREEN_TIME
			}
		}
		Event {
			key $VK_RBUTTON

			time { $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME + $MOVE2SCREEN_TIME + 1 }
			EvComm {
				smooth_obj	"More Option" 2
				start_time	0
				time		$SMOOTH_OBJ_TIME
			}
			EvComm {
				hide_obj	"More Option"
				start_time	$SMOOTH_OBJ_TIME
				time		$HIDE_OBJ_TIME
			}
			EvComm {
				move2screen	"Credits3"
				start_time	{ $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME }
				time		$MOVE2SCREEN_TIME
			}
		}
	}
}

#define Credits3_Logo0_PosX	71
#define Credits3_Logo0_PosY	207
#define Credits3_Back_Option_PosX	616
#define Credits3_Back_Option_PosY	254

Screen "Credits3"
{
	screen_offs	800

	block_global_obj
	default_obj "Back Option"

	Object "Credits Text"{
		null_level	200
		align_x center
		y	20
		space	2

		font	0
		s_text	  "resource/iscreen/credits3.txt"
	}

	Object "Back Option"
	{
		x	$Credits3_Back_Option_PosX
		y	$Credits3_Back_Option_PosY

		Element STRING {
			align_x center

			terrain_num	$TERRAIN2

			font		3
			space		2
			null_level	$DEFAULT_NULL
			string		$iSTR_BACK
		}
		Event {
			key $VK_LBUTTON
			key $VK_ESCAPE

			time { $PRESS_OBJ_TIME + $MOVE2SCREEN_TIME + 1 }
			EvComm {
				low_obj 	"Back Option"
				time		$PRESS_OBJ_TIME
				start_time	0
			}
			EvComm {
				move2screen	"Credits0"
				start_time	$PRESS_OBJ_TIME
				time		$MOVE2SCREEN_TIME
			}
		}
		Event {
			key $VK_RBUTTON

			time { $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME + $MOVE2SCREEN_TIME + 1 }
			EvComm {
				smooth_obj	"Back Option" 2
				start_time	0
				time		$SMOOTH_OBJ_TIME
			}
			EvComm {
				hide_obj	"Back Option"
				start_time	$SMOOTH_OBJ_TIME
				time		$HIDE_OBJ_TIME
			}
			EvComm {
				move2screen	"Credits0"
				start_time	{ $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME }
				time		$MOVE2SCREEN_TIME
			}
		}
	}
	#define LOGO_OBJ_ID	"Logo0"
	Object "Logo0" {
		x	$Credits3_Logo0_PosX
		y	$Credits3_Logo0_PosY

		Element BITMAP {
			align_x 	center
			align_y 	center

			terrain_num	$TERRAIN4

			bmp_null_level	148
			null_level	128

			bmp_file	"resource/iscreen/bitmap/logo0.bmp"
		}
		$LOGO_EV
	}
}


