	#define BUTTON_NAME "Button09"
	Object "Button09" {

		$BT09_COORDS

		Element BITMAP {
			bmp_null_level	$BUTTON_BMP_NULL
			null_level	180

			bmp_file	$BT09_BMP
		}
		Event {
			key	$VK_LBUTTON
			key	$VK_RBUTTON
			key	$VK_ESCAPE
			key	$VK_TAB
			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 10 + 2 }

			multi_event

			#define BUTTON_SCALE_STRENGTH	-50
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0

			EvComm {
				send2actint	$EV_RESUME_AML
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 10 + 1 }
			}
			EvComm {
				hide_obj       "Avi00"
				start_time	0
				time		$AVI_HIDE_SPEED

				event_line	$MECHOS_MODE
			}
			EvComm {
				hide_obj       "Avi00"
				start_time	0
				time		$AVI_HIDE_SPEED

				event_line	$ITEMS_MODE
			}
			EvComm {
				hide_obj       "Avi00"
				start_time	0
				time		$AVI_HIDE_SPEED

				event_line	$WEAPONS_MODE
			}
			EvComm {
				send2actint	$EV_DEACTIVATE_IINV
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME - 1 }
			}
			EvComm {
				move2screen	$LSCREEN_ID
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME }
				time		10
			}
		}
		prompt_text	$BT09_TEXT
	}

