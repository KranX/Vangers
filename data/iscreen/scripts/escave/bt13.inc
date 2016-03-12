	#define BUTTON_NAME "Button13"
	Object "Button13" {

		$BT13_COORDS

		Element BITMAP {
			bmp_null_level	$BUTTON_BMP_NULL
			null_level	$BUTTON_NULL

			bmp_file	$BT13_BMP
		}
		Event {
			key $VK_RBUTTON
			key $VK_LBUTTON

			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + $AVI_HIDE_SPEED + 8 }
			EvComm {
				hide_obj	"MechNameObj"
				start_time	0
				time		10
			}

			EvComm {
				send2actint	$EV_CANCEL_MATRIX
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME }
			}

			EvComm {
				move_screen	-400
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }
				time		5
			}
			EvComm {
				scale_obj	"Avi00"         256
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 7 }
				time		$AVI_HIDE_SPEED
			}

			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2

			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV

			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0
		}
		prompt_text	$BT13_TEXT
	}

