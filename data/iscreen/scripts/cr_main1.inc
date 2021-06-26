
#define LOGO_EV     Event {							\
				key $VK_LBUTTON 				\
										\
				time { $PRESS_OBJ_TIME * 2 + $MOVE2SCREEN_TIME + 1 }	\
				EvComm {					\
					scale_obj	$LOGO_OBJ_ID	    -200 \
					start_time	0			\
					time		$PRESS_OBJ_TIME 	\
				}						\
				EvComm {					\
					smooth_obj	$LOGO_OBJ_ID	    2	\
					start_time	0			\
					time		$PRESS_OBJ_TIME 	\
				}						\
				EvComm {					\
					rise_obj	$LOGO_OBJ_ID		\
					start_time	$PRESS_OBJ_TIME 	\
					time		$PRESS_OBJ_TIME 	\
				}						\
				EvComm {					\
					smooth_obj	$LOGO_OBJ_ID	 0	\
					start_time	$PRESS_OBJ_TIME 	\
					time		$PRESS_OBJ_TIME 	\
				}						\
				EvComm {					\
					move2screen	$CREDITS_SCR_ID 	\
					start_time	{ $PRESS_OBJ_TIME * 2 } \
					time	$MOVE2SCREEN_TIME		\
				}						\
			}							\
			Event { 						\
				key $VK_RBUTTON 				\
										\
				time { $SMOOTH_OBJ_TIME * 2 + $MOVE2SCREEN_TIME + 1 }	\
				EvComm {					\
					smooth_obj	$LOGO_OBJ_ID	    3	\
					start_time	0			\
					time		$SMOOTH_OBJ_TIME	\
				}						\
				EvComm {					\
					smooth_obj	$LOGO_OBJ_ID	    0	\
					start_time	$SMOOTH_OBJ_TIME	\
					time		$SMOOTH_OBJ_TIME	\
				}						\
				EvComm {					\
					move2screen	$CREDITS_SCR_ID 	\
					start_time { $SMOOTH_OBJ_TIME * 2 }	\
					time	$MOVE2SCREEN_TIME		\
				}						\
			}

Screen "Credits screen"
{
	screen_offs	800

	default_obj "Logo0"

	Object "TitleStr"
	{
		align_x center
		y	$TITLE_STR_Y

		Element STRING {
			align_x center

			terrain_num	$TERRAIN4

			font		0
			null_level	$DEFAULT_NULL
			string		$iSTR_Credits
		}
	}

	Object "SubtitleStrA"
	{
		align_x	center
		align_x_offs -200
		y	90
		Element STRING {
			align_x	center
			terrain_num	$TERRAIN3
			font		4
			null_level	$DEFAULT_NULL
			string		$iSTR_CreditsSubA
		}
	}

	Object "SubtitleStrBA"
	{
		align_x	center
		align_x_offs 200
		y	90
		Element STRING {
			align_x	center
			terrain_num	$TERRAIN3
			font		4
			null_level	$DEFAULT_NULL
			string		$iSTR_CreditsSubBA
		}
	}
	Object "SubtitleStrBB"
	{
		align_x	center
		align_x_offs 200
		y	122
		Element STRING {
			align_x	center
			terrain_num	$TERRAIN3
			font		4
			null_level	$DEFAULT_NULL
			string		$iSTR_CreditsSubBB
		}
	}

	Object "SubtitleStrCA"
	{
		align_x	center
		align_x_offs 200
		y	250
		Element STRING {
			align_x	center
			terrain_num	$TERRAIN3
			font		4
			null_level	$DEFAULT_NULL
			string		$iSTR_CreditsSubCA
		}
	}
	Object "SubtitleStrCB"
	{
		align_x	center
		align_x_offs 200
		y	282
		Element STRING {
			align_x	center
			terrain_num	$TERRAIN3
			font		4
			null_level	$DEFAULT_NULL
			string		$iSTR_CreditsSubCB
		}
	}
	
	Object "Main Menu Option"
	{
		align_x center
		y	450

		Element STRING {
			align_x center

			terrain_num	$TERRAIN2

			font		0
			null_level	$DEFAULT_NULL
			string		$iSTR_MAIN_MENU
		}

		Event {
			key $VK_LBUTTON
			key $VK_ESCAPE

			time { $PRESS_OBJ_TIME + $MOVE2SCREEN_TIME + 1 }
			EvComm {
				low_obj 	"Main Menu Option"
				time		$PRESS_OBJ_TIME
				start_time	0
			}
			EvComm {
				move2screen	"Main Screen"
				start_time	$PRESS_OBJ_TIME
				time		$MOVE2SCREEN_TIME
			}
		}
		Event {
			key $VK_RBUTTON

			time 32
			EvComm {
				smooth_obj	"Main Menu Option" 2
				start_time	0
				time		$SMOOTH_OBJ_TIME
			}
			EvComm {
				hide_obj	"Main Menu Option"
				start_time	$SMOOTH_OBJ_TIME
				time		$HIDE_OBJ_TIME
			}
			EvComm {
				move2screen	"Main Screen"
				start_time	{ $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME }
				time		$MOVE2SCREEN_TIME
			}
		}
	}

	#define LOGO_OBJ_ID	"Logo0"
	#define CREDITS_SCR_ID	"Credits0"
	Object "Logo0" {
		align_x	center
		align_x_offs -200
		y	140

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
	#define LOGO_OBJ_ID	"Logo1"
	#define CREDITS_SCR_ID	"Credits1"
	Object "Logo1" {
		align_x	center
		align_x_offs -200
		y	290

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
	#define LOGO_OBJ_ID	"Logo2"
	#define CREDITS_SCR_ID	"Credits2"
	Object "Logo2" {
		align_x	center
		align_x_offs -200
		y	390

		Element BITMAP {
			align_x 	center
			align_y 	center

			terrain_num	$TERRAIN4

			bmp_null_level	148
			null_level	128

			bmp_file	"resource/iscreen/bitmap/logo2.bmp"
		}
		$LOGO_EV
	}
	#define LOGO_OBJ_ID	"Logo3"
	#define CREDITS_SCR_ID	"Credits5"
	Object "Logo3" {
		align_x	center
		align_x_offs 200
		y	160

		Element BITMAP {
			align_x 	center
			align_y 	center

			terrain_num	$TERRAIN4

			bmp_null_level	148
			null_level	128

			bmp_file	"resource/iscreen/bitmap/community.bmp"
		}
		$LOGO_EV
	}
	#define LOGO_OBJ_ID	"Logo4"
	#define CREDITS_SCR_ID	"Credits6"
	Object "Logo4" {
		align_x	center
		align_x_offs 200
		y	340

		Element BITMAP {
			align_x 	center
			align_y 	center

			terrain_num	$TERRAIN4

			bmp_null_level	148
			null_level	128

			bmp_file	"resource/iscreen/bitmap/kranx_logo.bmp"
		}
		$LOGO_EV
	}
}

#define LOGO_EV     Event {							\
				key $VK_LBUTTON 				\
										\
				time { $PRESS_OBJ_TIME * 2 + 1 }		\
				EvComm {					\
					scale_obj	$LOGO_OBJ_ID	    -200 \
					start_time	0			\
					time		$PRESS_OBJ_TIME 	\
				}						\
				EvComm {					\
					smooth_obj	$LOGO_OBJ_ID	    2	\
					start_time	0			\
					time		$PRESS_OBJ_TIME 	\
				}						\
				EvComm {					\
					rise_obj	$LOGO_OBJ_ID		\
					start_time	$PRESS_OBJ_TIME 	\
					time		$PRESS_OBJ_TIME 	\
				}						\
				EvComm {					\
					smooth_obj	$LOGO_OBJ_ID	 0	\
					start_time	$PRESS_OBJ_TIME 	\
					time		$PRESS_OBJ_TIME 	\
				}						\
			}							\
			Event { 						\
				key $VK_RBUTTON 				\
										\
				time { $SMOOTH_OBJ_TIME * 2 + 1 }		\
				EvComm {					\
					smooth_obj	$LOGO_OBJ_ID	    3	\
					start_time	0			\
					time		$SMOOTH_OBJ_TIME	\
				}						\
				EvComm {					\
					smooth_obj	$LOGO_OBJ_ID	    0	\
					start_time	$SMOOTH_OBJ_TIME	\
					time		$SMOOTH_OBJ_TIME	\
				}						\
			}

#include "iscreen/scripts/cr4.inc"
#include "iscreen/scripts/cr3.inc"
#include "iscreen/scripts/cr2.inc"
#include "iscreen/scripts/cr1.inc"
#include "iscreen/scripts/cr0.inc"
