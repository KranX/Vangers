	#define BUTTON_NAME "Button15"
	Object "Button15" {

		$BT15_COORDS

		Element BITMAP {
			bmp_null_level	$BUTTON_BMP_NULL
			null_level	$BUTTON_NULL

			bmp_file	$BT15_BMP
		}
		Event {
			key $VK_RBUTTON
			key $VK_LBUTTON

			#define BUTTON_PRESS_START	1
			#define BUTTON_UNPRESS_START	8

			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + $AVI_HIDE_SPEED + 7 }

			EvComm {
				check_credits
				start_time	0
			}

			EvComm {
				hide_obj	"MechNameObj"
				start_time	1
				time		5
			}

			EvComm {
				send2actint	$EV_DEACTIVATE_MATRIX
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME }
			}

			EvComm {
				move_screen	-400
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }
				time		5
			}
			EvComm {
				scale_obj	"Avi00"         256
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 6 }
				time		$AVI_HIDE_SPEED
			}

			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2

			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV

			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0

			#define BUTTON_PRESS_START	0
			#define BUTTON_UNPRESS_START	7
		}
		prompt_text	$BT15_TEXT
		prompt_id	$ACI_BT15_PROMPT
	}

