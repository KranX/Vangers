	#define BUTTON_NAME "Button14"
	Object "Button14" {

		$BT14_COORDS

		Element BITMAP {
			bmp_null_level	$BUTTON_BMP_NULL
			null_level	$BUTTON_NULL

			bmp_file	$BT14_BMP
		}
		Event {
			key $VK_RBUTTON
			key $VK_LBUTTON

			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }
			EvComm {
				send2actint	$EV_AUTO_MOVE_ITEMS
				start_time	$BUTTON_UNPRESS_START
			}
			#define BUTTON_SCALE_STRENGTH	-150
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0
		}
		prompt_text	$BT14_TEXT
	}

