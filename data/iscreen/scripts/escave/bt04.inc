	#define BUTTON_NAME "Button04"
	Object "Button04" {

		no_fon

		$BT04_COORDS

		Element BITMAP {
			bmp_null_level	$BT04_BMP_NULL
			null_level	$BT04_NULL

			bmp_file	$BT04_BMP
		}
		Event {
			key	$VK_LBUTTON
			key	$VK_RBUTTON
			key	$VK_RETURN
			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }

			if_locked

			EvComm {
				send2actint	$EV_GET_BLOCK_PHRASE
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
		Event {
			key		$VK_LBUTTON
			key		$VK_RBUTTON
			key		$VK_RIGHT

			scancode	$AML_ELEECH_CODE
			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 15 + 10 + 1 }

			#define BUTTON_SCALE_STRENGTH	-180
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0

			EvComm {
				send2actint	$EV_PAUSE_AML
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME - 1 }
			}

			EvComm {
				scale_obj	"SELECTED_OBJ" 256
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 10 }
				time		15
			}
			EvComm {
				move2screen	$LSCREEN_SHOP_ID
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME }
				time		10
			}
			EvComm {
				send2actint	$EV_INIT_BUTTONS
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME - 1 }
			}
			EvComm {
				send2actint	$EV_INIT_AVI_OBJECT
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME - 2 }
			}
			EvComm {
				send2actint	$EV_ACTIVATE_IINV
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME - 1 }
			}
			EvComm {
				send2actint	$EV_REDRAW
				start_time	{ $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME }
			}
		}
		prompt_text	$BT04_TEXT
	}

