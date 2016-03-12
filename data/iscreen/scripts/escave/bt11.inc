	#define BUTTON_NAME "Button11"
	Object "Button11" {

		$BT11_COORDS

		Element BITMAP {
			id		"Button11el"

			bmp_null_level	$BUTTON_BMP_NULL
			null_level	$BUTTON_NULL

			scale_null
			bmp_file	$BT11_BMP
		}
		Event {
			key $VK_RBUTTON
			key $VK_LBUTTON
			key $VK_RETURN

			time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }

			multi_event

			EvComm {
				end_event
				start_time	0

				event_line	$EMPTY_MODE
			}

			EvComm {
				change_trigger	"Avi00"
				start_time	1

				time		{ $AVI_HIDE_SPEED - 1 }
			}
			EvComm {
				send2actint	$EV_ACTIVATE_SHOP_MENU
				start_time	1
			}
			EvComm {
				send2actint	$EV_INIT_BUTTONS
				start_time	$BUTTON_UNPRESS_START
			}

			#define BUTTON_PRESS_START	1
			#define BUTTON_PRESS_TIME	6
			#define BUTTON_SCALE_STRENGTH	128
			#define BUTTON_SMOOTH_INDEX	1
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0
			#define BUTTON_PRESS_START	0
			#define BUTTON_PRESS_TIME	7
		}
		prompt_text	$BT11_TEXT
	}


