	#define BUTTON_NAME	"Button03"
	Object "Button03" {

		no_fon

		$BT03_COORDS

		Element BITMAP {
			bmp_null_level	$BUTTON_BMP_NULL
			null_level	$BUTTON_NULL

			bmp_file	$BT03_BMP
		}
		Event {
			key	$VK_LBUTTON
			key	$VK_RBUTTON
			key	$VK_LEFT
			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }

			EvComm {
				send2actint	$EV_SHOW_QUESTS
				start_time	$BUTTON_UNPRESS_START
			}

			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0
		}
		prompt_text	$BT03_TEXT
	}

