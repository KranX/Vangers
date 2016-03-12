	#define BUTTON_NAME	"Button00"
	Object "Button00" {

		no_fon

		$BT00_COORDS

		Element BITMAP {
			bmp_null_level	$BUTTON_BMP_NULL
			null_level	$BT00_NULL

			bmp_file	$BT00_BMP
		}
		Event {
			key	$VK_LBUTTON
			key	$VK_RBUTTON
//			  key	  $VK_S

			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }

			EvComm {
				ext_event	$iEXT_SAVE_GAME 	0
				start_time	$BUTTON_UNPRESS_START
			}

			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0
		}
		prompt_text	$BT00_TEXT
	}

