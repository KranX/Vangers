	Object "PricePanel"{

		$PANELPRICE_COORDS

		Element BITMAP {
			id	"PricePanelBMP"
			null_level	10

			$PANELPRICE_SIZE

			picture
			bmp_file	NONE
		}
	}

	Object "InfoPanel"{

		$PANELINFO_COORDS

		Element BITMAP {
			id		"InfoPanelBMP"
			null_level	2

			$PANELINFO_SIZE

			picture
			bmp_file	NONE
		}
	}

	Trigger "TRIG00" {
		align_x 	left
		align_y 	up

		align_x_offs	180
		align_y_offs	100

		num_state	3

		null_level	20

		State 2 "S_STRING"{
			align_x center
			align_y center

			string "MECHOS"
		}
		State 1 "S_STRING"{
			align_x center
			align_y center

			string "ITEMS"
		}
		State 0 "S_STRING"{
			align_x center
			align_y center

			string "WEAPONS"
		}
	}

	Object "Matrix00" {
		x	458
		y	52

		Element BITMAP {
			id	"MatrixBorder"
			align_x center
			align_y center

			terrain_num	$MATRIX_TERRAIN

			null_level	200

			bmp_file "resource/actint/iscreen/matrix/mt00.bmp"
		}
	}

	Object "Matrix01" {
		x	858
		y	52

		Element BITMAP {
			id	"sMatrixBorder"
			align_x center
			align_y center

			terrain_num	$MATRIX_TERRAIN

			null_level	200

			bmp_file "resource/actint/iscreen/matrix/mt00.bmp"
		}
	}

	Trigger "Avi00" {
		x	$AVI_OBJ_X
		y	$AVI_OBJ_Y

		num_state	2

		prompt_text	$AVI_TEXT
		prompt_id	$ACI_AVI_PROMPT1

		State 0 AVI "AVI Element" {
			null_level 83
			avi_file "mech00.avi"
			ibs_file	$LBORDER_ID
		}
		State 1 BITMAP {
			null_level	11

			$PANELITEMS_SIZE

			id		"ItemsMenuBMP"

			picture
//			  no_scale

			bmp_file	NONE
		}
		Event {
			key $VK_RBUTTON

			time	{ $AVI_HIDE_SPEED * 2 + 2 }
			EvComm {
				hide_obj       "Avi00"
				start_time	0
				time		$AVI_HIDE_SPEED
			}
			EvComm {
				scale_obj	"Avi00"         256
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED
			}
			EvComm {
				send2actint	$EV_CHANGE_AVI_INDEX
				start_time	$AVI_HIDE_SPEED
			}
		}
		Event {
			key $VK_LBUTTON

			multi_event

			time 30

			// Change modes...
			EvComm {
				set_trigger	"TRIG00"        $TRG_WEAPONS
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$CHANGE_2_WEAPONS_MODE
			}
			EvComm {
				scale_obj	"Avi00"         256
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$CHANGE_2_WEAPONS_MODE
			}
			EvComm {
				exit_event
				start_time	{ $AVI_HIDE_SPEED * 2 + 1 }

				event_line	$CHANGE_2_WEAPONS_MODE
			}
			EvComm {
				send2actint	$EV_INIT_BUTTONS
				start_time	{ $AVI_HIDE_SPEED * 2 }

				event_line	$CHANGE_2_WEAPONS_MODE
			}

			EvComm {
				set_trigger	"TRIG00"        $TRG_MECHOS
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$CHANGE_2_MECHOS_MODE
			}
			EvComm {
				scale_obj	"Avi00"         256
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$CHANGE_2_MECHOS_MODE
			}
			EvComm {
				exit_event
				start_time	{ $AVI_HIDE_SPEED * 2 + 1 }

				event_line	$CHANGE_2_MECHOS_MODE
			}
			EvComm {
				send2actint	$EV_INIT_BUTTONS
				start_time	{ $AVI_HIDE_SPEED * 2 }

				event_line	$CHANGE_2_MECHOS_MODE
			}

			EvComm {
				set_trigger	"TRIG00"        $TRG_ITEMS
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$CHANGE_2_ITEMS_MODE
			}
			EvComm {
				scale_obj	"Avi00"         256
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$CHANGE_2_ITEMS_MODE
			}
			EvComm {
				exit_event
				start_time	{ $AVI_HIDE_SPEED + $AVI_HIDE_SPEED + 1 }

				event_line	$CHANGE_2_ITEMS_MODE
			}
			EvComm {
				send2actint	$EV_INIT_BUTTONS
				start_time	{ $AVI_HIDE_SPEED * 2 }

				event_line	$CHANGE_2_ITEMS_MODE
			}

			// MECHOS_LIST_MODE event...
			EvComm {
				send2actint	$EV_CHOOSE_SHOP_ITEM
				start_time	0

				event_line	$MECHOS_LIST_MODE
			}
			EvComm {
				change_trigger	"Avi00"
				start_time	1

				time		$AVI_HIDE_SPEED

				event_line	$MECHOS_LIST_MODE
			}
			EvComm {
				exit_event
				start_time	{ $AVI_HIDE_SPEED + 2 }
				event_line	$MECHOS_LIST_MODE
			}
			EvComm {
				send2actint	$EV_INIT_BUTTONS
				start_time	{ $AVI_HIDE_SPEED + 1 }

				event_line	$MECHOS_LIST_MODE
			}

			// ITEMS_LIST_MODE event...
			EvComm {
				send2actint	$EV_CHOOSE_SHOP_ITEM
				start_time	0

				event_line	$ITEMS_LIST_MODE
			}
			EvComm {
				change_trigger	"Avi00"
				start_time	1

				time		$AVI_HIDE_SPEED

				event_line	$ITEMS_LIST_MODE
			}
			EvComm {
				exit_event
				start_time	{ $AVI_HIDE_SPEED + 2 }
				event_line	$ITEMS_LIST_MODE
			}
			EvComm {
				send2actint	$EV_INIT_BUTTONS
				start_time	{ $AVI_HIDE_SPEED + 1 }

				event_line	$ITEMS_LIST_MODE
			}

			// WEAPONS_LIST_MODE event...
			EvComm {
				send2actint	$EV_CHOOSE_SHOP_ITEM
				start_time	0

				event_line	$WEAPONS_LIST_MODE
			}
			EvComm {
				change_trigger	"Avi00"
				start_time	1

				time		$AVI_HIDE_SPEED

				event_line	$WEAPONS_LIST_MODE
			}
			EvComm {
				exit_event
				start_time	{ $AVI_HIDE_SPEED + 2 }
				event_line	$WEAPONS_LIST_MODE
			}
			EvComm {
				send2actint	$EV_INIT_BUTTONS
				start_time	{ $AVI_HIDE_SPEED + 1 }

				event_line	$WEAPONS_LIST_MODE
			}

			EvComm {
				send2actint	$EV_INIT_BUTTONS
				start_time	{ $AVI_HIDE_SPEED * 2 }
			}

			// EMPTY_MODE event...
			EvComm {
				scale_obj	"Avi00"         256
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$EMPTY_MODE
			}
			EvComm {
				send2actint	$EV_BUY_ITEM
				start_time	$AVI_HIDE_SPEED

				event_line	$EMPTY_MODE
			}
			EvComm {
				end_event
				start_time	{ $AVI_HIDE_SPEED + $AVI_HIDE_SPEED + 2 }

				event_line	$EMPTY_MODE
			}

			// ITEMS_MODE event...
			EvComm {
				hide_obj       "Avi00"
				start_time	0
				time		5

				event_line	$ITEMS_MODE
			}
			EvComm {
				scale_obj	"Avi00"         256
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		$AVI_HIDE_SPEED

				event_line	$ITEMS_MODE
			}
			EvComm {
				send2actint	$EV_BUY_ITEM
				start_time	$AVI_HIDE_SPEED

				event_line	$ITEMS_MODE
			}
			EvComm {
				end_event
				start_time	{ $AVI_HIDE_SPEED + $AVI_HIDE_SPEED + 2 }

				event_line	$ITEMS_MODE
			}

			// WEAPONS_MODE event...
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

				event_line	$WEAPONS_MODE
			}
			EvComm {
				send2actint	$EV_BUY_ITEM
				start_time	$AVI_HIDE_SPEED

				event_line	$WEAPONS_MODE
			}
			EvComm {
				end_event
				start_time	{ $AVI_HIDE_SPEED + $AVI_HIDE_SPEED + 2 }

				event_line	$WEAPONS_MODE
			}

			// MEHOS_MODE event...
			EvComm {
				hide_obj       "Avi00"
				start_time	0
				time		$AVI_HIDE_SPEED

				event_line	$MECHOS_MODE
			}
			EvComm {
				send2actint	$EV_SELL_MOVE_ITEM
				start_time	$AVI_HIDE_SPEED

				event_line	$MECHOS_MODE
			}
			EvComm {
				send2actint	$EV_SET_MECH_NAME
				start_time	{ $AVI_HIDE_SPEED + $AVI_HIDE_SPEED + 2 }

				event_line	$MECHOS_MODE
			}
			EvComm {
				scale_obj	 "MechNameObj"  256
				start_time	{ $AVI_HIDE_SPEED + $AVI_HIDE_SPEED + 3 }
				time		5

				event_line	$MECHOS_MODE
			}
			EvComm {
				hide_obj	"MechNameObj"
				start_time	1
				time		1

				event_line	$MECHOS_MODE
			}
			EvComm {
				move_screen	400
				start_time	{ $AVI_HIDE_SPEED + 1 }
				time		5

				event_line	$MECHOS_MODE
			}

			EvComm {
				send2actint	$EV_ACTIVATE_MATRIX
				start_time	{ $AVI_HIDE_SPEED + 1 + 5 + 1 }

				event_line	$MECHOS_MODE
			}
			EvComm {
				send2actint	$EV_REDRAW
				start_time	{ $AVI_HIDE_SPEED + 1 + 5 + 2 }

				event_line	$MECHOS_MODE
			}
			EvComm {
				end_event
				start_time	{ $AVI_HIDE_SPEED + $AVI_HIDE_SPEED + 9 }

				event_line	$MECHOS_MODE
			}

//			  EvComm {
//				  check_credits
//				  start_time	  0
//			  }
		}
	}

	Object "MechNameObj" {
		align_x 	center
		align_x_offs	630
		y		10

		Element "STRING"{
			id		"MechNameStr"
			null_level	200

			terrain_num	3

			align_x center
			align_y center

			font		3
			space		1

			string "XXXXXXXXXXXXXXXXXXXXXXXXX"
		}
	}

	Object "CreditsObj" {

		$PANELCREDITS_COORDS

		Element "BITMAP"{
			id		"CreditsBMP"
			null_level	200

			$PANELCREDITS_SIZE

			picture

			bmp_file	NONE
		}
	}
}

/* ------------------------ END OF LOCATION SECTION ------------------------- */

