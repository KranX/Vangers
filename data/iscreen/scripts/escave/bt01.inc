	#define BUTTON_NAME	"Button01"
	Object "Button01" {

		no_fon

		$BT01_COORDS

		Element BITMAP {
			bmp_null_level	$BUTTON_BMP_NULL
			null_level	185

			bmp_file	$BT01_BMP
		}
		Event {
			key		$VK_LBUTTON
			key		$VK_RBUTTON
			key		$VK_UP

			scancode	$AML_FORCED_EXIT_CODE

			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }

			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0

			EvComm {
				exit		1
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME }
			}
		}
		prompt_text	$BT01_TEXT
	}

