	#define BUTTON_NAME "Button10"
	Object "Button10" {

		$BT10_COORDS

		shadow_size	6

		Element BITMAP {
			bmp_null_level	$BT10_BMP_NULL
			null_level	$BT10_NULL

			scale_null
			bmp_file	$BT10_BMP
		}
		Event {
			key $VK_RBUTTON
			key $VK_LBUTTON
			key $VK_LEFT

			multi_event

			time { $BUTTON_UNPRESS_START + $BUTTON_UNPRESS_TIME + 1 }
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
				scale_obj	"Avi00"         256
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$MECHOS_MODE
			}
			EvComm {
				scale_obj	"Avi00"         256
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$ITEMS_MODE
			}
			EvComm {
				scale_obj	"Avi00"         256
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$WEAPONS_MODE
			}
			EvComm {
				send2actint	$EV_PREV_SHOP_AVI
				start_time	$AVI_HIDE_SPEED
			}

			#define BUTTON_SCALE_STRENGTH	$BT10_SCALE
			#define BUTTON_SMOOTH_INDEX	$BT10_SMOOTH
			$BUTTON_PRESS_EV
			$BUTTON_UNPRESS_EV
			#define BUTTON_SMOOTH_INDEX	4
			#define BUTTON_SCALE_STRENGTH	0
		}
		prompt_text	$BT10_TEXT
	}


