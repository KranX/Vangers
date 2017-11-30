	#define COLOR0		{ 48 + 1 * 256 }
	#define COLOR1		{ 64 + 1 * 256 }
	#define COLOR2		{ 96 + 1 * 256 }

	#define SAVE_SLOT_SIZE_X	360
	#define SAVE_SLOT_SIZE_Y	40

	#define SAVE_STRING_SIZE_X	300
	#define SAVE_STRING_SIZE_Y	50

	resource $ACS_BMO {
		id	$ACS_SAVE_STRING

		build
		offs_x	$SAVE_STRING_POS_X
		offs_y	$SAVE_STRING_POS_Y

		size_x	$SAVE_STRING_SIZE_X
		size_y	$SAVE_STRING_SIZE_Y
	}

	resource $ACS_BMO {
		id	$ACS_SAVE_SLOT0

		build
		offs_x	$SAVE_SLOT_OFFS_X
		offs_y	{ $SAVE_SLOT_START_Y + $SAVE_SLOT_DELTA_Y * 0 }

		size_x	$SAVE_SLOT_SIZE_X
		size_y	$SAVE_SLOT_SIZE_Y
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_SLOT1

		build
		offs_x	$SAVE_SLOT_OFFS_X
		offs_y	{ $SAVE_SLOT_START_Y + $SAVE_SLOT_DELTA_Y * 1 }

		size_x	$SAVE_SLOT_SIZE_X
		size_y	$SAVE_SLOT_SIZE_Y
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_SLOT2

		build
		offs_x	$SAVE_SLOT_OFFS_X
		offs_y	{ $SAVE_SLOT_START_Y + $SAVE_SLOT_DELTA_Y * 2 }

		size_x	$SAVE_SLOT_SIZE_X
		size_y	$SAVE_SLOT_SIZE_Y
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_SLOT3

		build
		offs_x	$SAVE_SLOT_OFFS_X
		offs_y	{ $SAVE_SLOT_START_Y + $SAVE_SLOT_DELTA_Y * 3 }

		size_x	$SAVE_SLOT_SIZE_X
		size_y	$SAVE_SLOT_SIZE_Y
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_SLOT4

		build
		offs_x	$SAVE_SLOT_OFFS_X
		offs_y	{ $SAVE_SLOT_START_Y + $SAVE_SLOT_DELTA_Y * 4 }

		size_x	$SAVE_SLOT_SIZE_X
		size_y	$SAVE_SLOT_SIZE_Y
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_SLOT5

		build
		offs_x	$SAVE_SLOT_OFFS_X
		offs_y	{ $SAVE_SLOT_START_Y + $SAVE_SLOT_DELTA_Y * 5 }

		size_x	$SAVE_SLOT_SIZE_X
		size_y	$SAVE_SLOT_SIZE_Y
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_SLOT6

		build
		offs_x	$SAVE_SLOT_OFFS_X
		offs_y	{ $SAVE_SLOT_START_Y + $SAVE_SLOT_DELTA_Y * 6 }

		size_x	$SAVE_SLOT_SIZE_X
		size_y	$SAVE_SLOT_SIZE_Y
	}
	resource $ACS_BMO {
		id	$ACS_SAVE_SLOT7

		build
		offs_x	$SAVE_SLOT_OFFS_X
		offs_y	{ $SAVE_SLOT_START_Y + $SAVE_SLOT_DELTA_Y * 7 }

		size_x	$SAVE_SLOT_SIZE_X
		size_y	$SAVE_SLOT_SIZE_Y
	}

	object $ACS_INPUT_FIELD_OBJ {
		id	$ACS_SAVE_STRING
		string_len	20
		ifont		0
		color		0	$COLOR0s
		space		1
		string		$acsSTR_SAVE_GAME

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SAVE_STRING $ACS_SAVE_STRING 0 0
			}
		}
	}

	object $ACS_BASE_OBJ {
		id	$ACS_SAVE_OK

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SAVE_OK $ACS_SAVE_OK 0 0
			}
		}
		event {
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$iMOUSE_RIGHT_PRESS_CODE
			key	$VK_RETURN

			time	3
			ev_comm {
				command $ACS_EXEC_SEQUENCE $ACS_SAVE_OK 1 0 0
				start_time	0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_SAVE_GAME 0 0 0
				start_time	1
			}
			ev_comm {
				command $ACS_EXIT 1 0 0 0
				start_time	2
			}
		}
		frame_seq {
			id	1
			resource_id	$ACS_SAVE_OK
			seq	0	$ACS_MAX_FRAME		1
		}
	}
	object $ACS_BASE_OBJ {
		id	$ACS_SAVE_CANCEL

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SAVE_CANCEL $ACS_SAVE_CANCEL 0 0
			}
		}
		event {
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$iMOUSE_RIGHT_PRESS_CODE
			key	$VK_ESCAPE

			time	3
			ev_comm {
				command $ACS_EXEC_SEQUENCE $ACS_SAVE_CANCEL 1 0 0
				start_time	0
			}
			ev_comm {
				command $ACS_EXIT 1 0 0 0
				start_time	1
			}
		}
		frame_seq {
			id	1
			resource_id	$ACS_SAVE_CANCEL
			seq	0	$ACS_MAX_FRAME		1
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id		$ACS_SAVE_SLOT0
		string_len	$ACS_SLOT_LEN
		ifont		0
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	2

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SAVE_SLOT0 $ACS_SAVE_SLOT0 0 0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME $ACS_SAVE_SLOT0 0 0
			}
		}
		event {
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$iMOUSE_RIGHT_PRESS_CODE

			time	2
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME_INPUT $ACS_SAVE_SLOT0 0 0
				start_time	0
			}
			ev_comm {
				command $ACS_INPUT_STRING $ACS_SAVE_SLOT0 0 0 0
				start_time	1
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id		$ACS_SAVE_SLOT1
		string_len	$ACS_SLOT_LEN
		ifont		0
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	2

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SAVE_SLOT1 $ACS_SAVE_SLOT1 0 0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME $ACS_SAVE_SLOT1 1 0
			}
		}
		event {
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$iMOUSE_RIGHT_PRESS_CODE

			time	2
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME_INPUT $ACS_SAVE_SLOT1 1 0
				start_time	0
			}
			ev_comm {
				command $ACS_INPUT_STRING $ACS_SAVE_SLOT1 0 0 0
				start_time	1
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id		$ACS_SAVE_SLOT2
		string_len	$ACS_SLOT_LEN
		ifont		0
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	2

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SAVE_SLOT2 $ACS_SAVE_SLOT2 0 0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME $ACS_SAVE_SLOT2 2 0
			}
		}
		event {
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$iMOUSE_RIGHT_PRESS_CODE

			time	2
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME_INPUT $ACS_SAVE_SLOT2 2 0
				start_time	0
			}
			ev_comm {
				command $ACS_INPUT_STRING $ACS_SAVE_SLOT2 0 0 0
				start_time	1
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id		$ACS_SAVE_SLOT3
		string_len	$ACS_SLOT_LEN
		ifont		0
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	2

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SAVE_SLOT3 $ACS_SAVE_SLOT3 0 0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME $ACS_SAVE_SLOT3 3 0
			}
		}
		event {
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$iMOUSE_RIGHT_PRESS_CODE

			time	2
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME_INPUT $ACS_SAVE_SLOT3 3 0
				start_time	0
			}
			ev_comm {
				command $ACS_INPUT_STRING $ACS_SAVE_SLOT3 0 0 0
				start_time	1
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id		$ACS_SAVE_SLOT4
		string_len	$ACS_SLOT_LEN
		ifont		0
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	2

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SAVE_SLOT4 $ACS_SAVE_SLOT4 0 0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME $ACS_SAVE_SLOT4 4 0
			}
		}
		event {
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$iMOUSE_RIGHT_PRESS_CODE

			time	2
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME_INPUT $ACS_SAVE_SLOT4 4 0
				start_time	0
			}
			ev_comm {
				command $ACS_INPUT_STRING $ACS_SAVE_SLOT4 0 0 0
				start_time	1
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id		$ACS_SAVE_SLOT5
		string_len	$ACS_SLOT_LEN
		ifont		0
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	2

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SAVE_SLOT5 $ACS_SAVE_SLOT5 0 0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME $ACS_SAVE_SLOT5 5 0
			}
		}
		event {
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$iMOUSE_RIGHT_PRESS_CODE

			time	2
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME_INPUT $ACS_SAVE_SLOT5 5 0
				start_time	0
			}
			ev_comm {
				command $ACS_INPUT_STRING $ACS_SAVE_SLOT5 0 0 0
				start_time	1
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id		$ACS_SAVE_SLOT6
		string_len	$ACS_SLOT_LEN
		ifont		0
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	2

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SAVE_SLOT6 $ACS_SAVE_SLOT6 0 0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME $ACS_SAVE_SLOT6 6 0
			}
		}
		event {
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$iMOUSE_RIGHT_PRESS_CODE

			time	2
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME_INPUT $ACS_SAVE_SLOT6 6 0
				start_time	0
			}
			ev_comm {
				command $ACS_INPUT_STRING $ACS_SAVE_SLOT6 0 0 0
				start_time	1
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id		$ACS_SAVE_SLOT7
		string_len	$ACS_SLOT_LEN
		ifont		0
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	2

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SAVE_SLOT7 $ACS_SAVE_SLOT7 0 0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME $ACS_SAVE_SLOT7 7 0
			}
		}
		event {
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$iMOUSE_RIGHT_PRESS_CODE

			time	2
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SLOT_NAME_INPUT $ACS_SAVE_SLOT7 7 0
				start_time	0
			}
			ev_comm {
				command $ACS_INPUT_STRING $ACS_SAVE_SLOT7 0 0 0
				start_time	1
			}
		}
	}

