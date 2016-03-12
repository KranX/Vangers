
#define GAMESCR_BACK_OFFS	70
#define GAMESCR_BACK_Y		410


	screen_offs	800
	default_obj "Back Option"

	Object "TitleStr"
	{
		align_x center
		y	$TITLE_STR_Y

		Element STRING {
			align_x center

			terrain_num	$TERRAIN4

			font		0
			null_level	$DEFAULT_NULL
			string		$iSTR_GAME_PARAMETERS
		}
	}

	Object "Back Option"
	{
		x	$Game_Params_0_Back_Option_PosX
		y	$GAMESCR_BACK_Y

		Element STRING {
			align_x center

			terrain_num	$TERRAIN2

			font		0
			null_level	$DEFAULT_NULL
			string		$iSTR_CANCEL
		}
		Event {
			key $VK_LBUTTON
			key $VK_ESCAPE

			multi_event

			time { $PRESS_OBJ_TIME + $MOVE2SCREEN_TIME + 1 }
			EvComm {
				low_obj 	"Back Option"
				time		$PRESS_OBJ_TIME
				start_time	0
			}
			EvComm {
				move2screen	"Select Game"
				start_time	$PRESS_OBJ_TIME
				time		$MOVE2SCREEN_TIME

				event_line	4
			}
			EvComm {
				move2screen	"Server Info screen"
				start_time	$PRESS_OBJ_TIME
				time		$MOVE2SCREEN_TIME

				event_line	3
			}
		}
		Event {
			key $VK_RBUTTON

			multi_event

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
				move2screen	"Select Game"
				start_time	{ $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME }
				time		$MOVE2SCREEN_TIME

				event_line	4
			}
			EvComm {
				move2screen	"Server Info screen"
				start_time	{ $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME }
				time		$MOVE2SCREEN_TIME

				event_line	3
			}
		}
	}
	Object "Ok Option"
	{
		x	$Game_Params_0_Ok_Option_PosX
		y	$GAMESCR_BACK_Y

		Element STRING {
			align_x center

			terrain_num	$TERRAIN2

			font		0
			null_level	$DEFAULT_NULL
			string		$iSTR_OK
		}
		Event {
			key $VK_LBUTTON
			key $VK_RETURN

			multi_event

			time { $PRESS_OBJ_TIME + $MOVE2SCREEN_TIME + 1 }
			EvComm {
				low_obj 	"Ok Option"
				time		$PRESS_OBJ_TIME
				start_time	0
			}
			EvComm {
				move2screen	"Server Config screen"
				start_time	$PRESS_OBJ_TIME
				time		$MOVE2SCREEN_TIME

				event_line	4
			}
			EvComm {
				move2screen	"Server Info screen"
				start_time	$PRESS_OBJ_TIME
				time		$MOVE2SCREEN_TIME

				event_line	3
			}
		}
		Event {
			key $VK_RBUTTON

			multi_event

			time { $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME + $MOVE2SCREEN_TIME + 1 }
			EvComm {
				smooth_obj	"Ok Option" 2
				start_time	0
				time		$SMOOTH_OBJ_TIME
			}
			EvComm {
				hide_obj	"Ok Option"
				start_time	$SMOOTH_OBJ_TIME
				time		$HIDE_OBJ_TIME
			}
			EvComm {
				move2screen	"Server Config screen"
				start_time	{ $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME }
				time		$MOVE2SCREEN_TIME

				event_line	4
			}
			EvComm {
				move2screen	"Server Info screen"
				start_time	{ $SMOOTH_OBJ_TIME + $HIDE_OBJ_TIME }
				time		$MOVE2SCREEN_TIME

				event_line	3
			}
		}
	}
	Object "CashStr"
	{
		x	$Game_Params_0_CashStr_PosX
		y	{ $PARAMS_START_Y + $PARAMS_DELTA_Y * 0 }

		Element STRING {
			align_x center

			terrain_num	$TERRAIN2

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL
			string		$iSTR_Initial_Cash
		}
	}
	Object "BeebosStr"
	{
		x	$Game_Params_0_BeebosStr_PosX
		y	{ $PARAMS_START_Y + $PARAMS_DELTA_Y * 0 }

		Element STRING {
			align_x center

			terrain_num	$TERRAIN2

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL
			string		"$"
		}
	}
	Object "BeebosNumberStr"
	{
		x	$Game_Params_0_BeebosNumberStr_PosX
		y	{ $PARAMS_START_Y + $PARAMS_DELTA_Y * 0 }

		not_unlock

		Element STRING {
			id	"BeebosNumberID"
			align_x center

			terrain_num	$TERRAIN4

			number_text

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL
			string		"100000"
		}
		Event {
			key $VK_LBUTTON
			key $VK_RBUTTON

			time 2
			EvComm {
				input_string	"BeebosNumberID"        7
				time		2
			}
		}
	}
	Object "ArtefactsStr"
	{
		x	$Game_Params_0_ArtefactsStr_PosX
		y	{ $PARAMS_START_Y + $PARAMS_DELTA_Y * 1 }

		Element STRING {
			align_x center

			terrain_num	$TERRAIN2

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL
			string		$iSTR_Artefacts_Using
		}
	}
	Trigger "ArtefactsTrig"
	{
		x	$Game_Params_0_ArtefactsTrig_PosX
		y	{ $PARAMS_START_Y + $PARAMS_DELTA_Y * 1 }

		not_unlock

		num_state	2

		State 1 "STRING"{
			align_x center
			align_y center

			terrain_num	$TERRAIN4

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL

			string		$iSTR_YES
		}
		State 0 "STRING"{
			align_x center
			align_y center

			terrain_num	$TERRAIN4

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL

			string		$iSTR_NO
		}
		Event {
			key $VK_LBUTTON
			key $VK_RBUTTON

			time { $TRIGGER_TIME + 1 }
			EvComm {
				change_trigger	"ArtefactsTrig"
				time $TRIGGER_TIME
			}
		}
	}

	Trigger "TimeTrig"
	{
		x	$Game_Params_0_TimeTrig_PosX
		y	{ $PARAMS_START_Y + $PARAMS_DELTA_Y * 2 }

		num_state	10

		not_unlock

		State 0 "STRING"{
			align_x center
			align_y center

			terrain_num	$TERRAIN4

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL

			string "1"
		}
		State 1 "STRING"{
			align_x center
			align_y center

			terrain_num	$TERRAIN4

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL

			string "2"
		}
		State 2 "STRING"{
			align_x center
			align_y center

			terrain_num	$TERRAIN4

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL

			string "3"
		}
		State 3 "STRING"{
			align_x center
			align_y center

			terrain_num	$TERRAIN4

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL

			string "4"
		}
		State 4 "STRING"{
			align_x center
			align_y center

			terrain_num	$TERRAIN4

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL

			string "5"
		}
		State 5 "STRING"{
			align_x center
			align_y center

			terrain_num	$TERRAIN4

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL

			string "6"
		}
		State 6 "STRING"{
			align_x center
			align_y center

			terrain_num	$TERRAIN4

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL

			string "7"
		}
		State 7 "STRING"{
			align_x center
			align_y center

			terrain_num	$TERRAIN4

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL

			string "8"
		}
		State 8 "STRING"{
			align_x center
			align_y center

			terrain_num	$TERRAIN4

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL

			string "9"
		}
		State 9 "STRING"{
			align_x center
			align_y center

			terrain_num	$TERRAIN4

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL

			string "10"
		}
		Event {
			key $VK_LBUTTON

			time { $TRIGGER_TIME + 1 }
			EvComm {
				change_trigger	"TimeTrig"
				time $TRIGGER_TIME
			}
		}
		Event {
			key $VK_RBUTTON

			time { $TRIGGER_TIME + 1 }
			EvComm {
				prev_trigger  "TimeTrig"
				time $TRIGGER_TIME
			}
		}
	}
	Object "EscaveTimeStr"
	{
		x	$Game_Params_0_EscaveTimeStr_PosX
		y	{ $PARAMS_START_Y + $PARAMS_DELTA_Y * 2 }

		Element STRING {
			align_x center

			terrain_num	$TERRAIN2

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL
			string		$iSTR_In_Escave_Time
		}
	}
	Object "EscaveMinStr"
	{
		x	$Game_Params_0_EscaveMinStr_PosX
		y	{ $PARAMS_START_Y + $PARAMS_DELTA_Y * 2 }

		Element STRING {
			align_x center

			terrain_num	$TERRAIN2

			space		$PARAMS_SPACE
			font		$PARAM_FNT
			null_level	$DEFAULT_NULL
			string		$iSTR_min
		}
	}

