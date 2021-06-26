
#define Credits0_More_Option_PosX	330
#define Credits0_More_Option_PosY	270
#define Credits0_Back_Option_PosX	-330
#define Credits0_Back_Option_PosY	270

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
		s_text	  $iSTR_CreditsPathA
	}

	Object "Back Option"
	{
		align_x	center
		align_x_offs $Credits0_Back_Option_PosX
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
		align_x	center
		align_x_offs $Credits0_More_Option_PosX
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
				move2screen	"Credits4"
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
				move2screen	"Credits4"
				start_time	{ $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME }
				time		$MOVE2SCREEN_TIME
			}
		}
	}
}


#define Credits4_More_Option_PosX	330
#define Credits4_More_Option_PosY	270
#define Credits4_Back_Option_PosX	-330
#define Credits4_Back_Option_PosY	270

Screen "Credits4"
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
		s_text	  $iSTR_CreditsPathB
	}

	Object "Back Option"
	{
		align_x	center
		align_x_offs $Credits4_Back_Option_PosX
		y	$Credits4_Back_Option_PosY

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
	Object "More Option"
	{
		align_x	center
		align_x_offs $Credits4_More_Option_PosX
		y	$Credits4_More_Option_PosY

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

#define Credits3_Logo0_PosY	177
#define Credits3_Back_Option_PosX	616
#define Credits3_Back_Option_PosY	324

Screen "Credits3"
{
	screen_offs	0

	block_global_obj
	default_obj "Back Option"

	Object "Credits Text A"{
		null_level	200
		align_x center
		align_x_offs -200
		y	30
		space	2

		font	0
		s_text	  $iSTR_CreditsPathCA
	}

	Object "Credits Text B"{
		null_level	200
		align_x center
		align_x_offs 200
		y	30
		space	2

		font	0
		s_text	  $iSTR_CreditsPathCB
	}

	Object "Back Option"
	{
		align_x center
		align_x_offs 200
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
				move2screen	"Credits4"
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
				move2screen	"Credits4"
				start_time	{ $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME }
				time		$MOVE2SCREEN_TIME
			}
		}
	}
	#define LOGO_OBJ_ID	"Logo0"
	Object "Logo0" {
		align_x center
		align_x_offs 200
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


