	#define BUTTON_NAME "Button08"
	Object "Button08" {

		$BT08_COORDS

		shadow_size	5

		Element BITMAP {
			id		"Button08el"

			bmp_null_level	$BUTTON_BMP_NULL
			null_level	$BUTTON_NULL

			scale_null

			bmp_file	$BT08_BMP
		}
		Event {
			key	$VK_LBUTTON
			key	$VK_RBUTTON
			key	$VK_3
			key	$VK_F3
			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }

			multi_event

			EvComm {
				set_event_line	$ITEMS_MODE
				start_time	0

				event_line	$MECHOS_MODE
			}
			EvComm {
				set_event_line	$ITEMS_MODE
				start_time	0

				event_line	$WEAPONS_MODE
			}
			EvComm {
				set_event_line	$ITEMS_MODE
				start_time	0

				event_line	$EMPTY_MODE
			}
			EvComm {
				send2actint	$EV_CHANGE_AVI_LIST
				start_time	$AVI_HIDE_SPEED

				event_line	$ITEMS_MODE
			}
			EvComm {
				send2actint	$EV_INIT_BUTTONS
				start_time	{ $AVI_HIDE_SPEED + 1 }

				event_line	$ITEMS_MODE
			}
			EvComm {
				send2actint	$EV_INIT_BUTTONS
				start_time	{ $AVI_HIDE_SPEED + 1 }

				event_line	$ITEMS_LIST_MODE
			}
			EvComm {
				set_event_line	$ITEMS_LIST_MODE
				start_time	0

				event_line	$MECHOS_LIST_MODE
			}
			EvComm {
				set_event_line	$ITEMS_LIST_MODE
				start_time	0

				event_line	$WEAPONS_LIST_MODE
			}
			EvComm {
				send2actint	$EV_CHANGE_AVI_LIST
				start_time	1

				event_line	$ITEMS_LIST_MODE
			}

			EvComm {
				hide_obj	"Avi00"
				start_time	1
				time		$AVI_HIDE_SPEED

				event_line	$ITEMS_MODE
			}
			EvComm {
				scale_obj	"Avi00"        256
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$ITEMS_MODE
			}

			EvComm {
				set_trigger	"TRIG00"        $TRG_ITEMS
				time		1
				start_time	1
			}

			EvComm {
				scale_obj	"Avi00"        256
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$EMPTY_MODE
			}

			#define BUTTON_SCALE_STRENGTH	150
			#define BUTTON_SMOOTH_INDEX	2
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0
		}
		prompt_text	$BT08_TEXT
	}

