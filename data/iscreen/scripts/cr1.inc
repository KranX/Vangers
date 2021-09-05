
Screen "Credits1"
{
	screen_offs	0

	block_global_obj
	default_obj "Back Option"

	Object "Credits Text A"{
		null_level	200
		align_x center
		align_x_offs 130
		y	30
		space	2

		font	0
		s_text	  $iSTR_CreditsPathDA
	}

	Object "Credits Text B"{
		null_level	200
		align_x center
		y	90
		space	2

		font	0
		s_text	  $iSTR_CreditsPathDB
	}

	Object "Back Option"
	{
		align_x 	right
		align_x_offs	20
		align_y 	center
		align_y_offs	50

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
	#define LOGO_OBJ_ID	"Logo1"
	Object "Logo1" {
		align_x 	center
		align_x_offs -250
		y		10

		Element BITMAP {
			align_x 	center
			align_y 	center

			terrain_num	$TERRAIN4

			bmp_null_level	148
			null_level	128

			bmp_file	"resource/iscreen/bitmap/logo1.bmp"
		}
		$LOGO_EV
	}
}

