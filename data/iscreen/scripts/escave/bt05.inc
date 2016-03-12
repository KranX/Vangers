	#define BUTTON_NAME	"Button05"
	Object "Button05" {

		no_fon

		$BT05_COORDS

		Element BITMAP {
			bmp_null_level	$BUTTON_BMP_NULL
			null_level	$BUTTON_NULL

			bmp_file	$BT05_BMP
		}
		Event {
			key	$VK_LBUTTON
			key	$VK_RBUTTON
			key	$VK_RETURN
			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }

			EvComm {
				send2actint	$EV_ASK_QUEST
				start_time	$BUTTON_UNPRESS_START
			}
			EvComm {
				send2actint	$EV_START_SPEECH
				start_time	1
			}

			#define BUTTON_SCALE_STRENGTH	-180
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0
		}
		prompt_text	$BT05_TEXT
	}

