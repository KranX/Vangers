	#define BUTTON_NAME	"Button02"
	Object "Button02" {

		no_fon

		$BT02_COORDS

		Element BITMAP {
			bmp_null_level	$BUTTON_BMP_NULL
			null_level	$BT02_NULL

			bmp_file	$BT02_BMP
		}
		Event {
			key	$VK_LBUTTON
			key	$VK_RBUTTON
			key	$VK_DOWN

			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }

			EvComm {
				send2actint	$EV_NEXT_PHRASE
				start_time	$BUTTON_UNPRESS_START
			}
			EvComm {
				send2actint	$EV_START_SPEECH
				start_time	1
			}

			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0
		}
		prompt_text	$BT02_TEXT
	}

