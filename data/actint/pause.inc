
	resource $ACS_BMO {
		id	$ACS_PAUSE_TEXT

		build
		offs_x	{ ( $PAUSE_RES_X - $STR_SX ) / 2 }
		offs_y	{ $STR_START_Y - 10 }

		size_x	$STR_SX
		size_y	$STR_SY
	}
	resource $ACS_BMO {
		id	$ACS_RESUME_GAME

		build
		offs_x	{ ( $PAUSE_RES_X - $STR_SX ) / 2 }
		offs_y	{ $STR_START_Y + $STR_DY * 1 }

		size_x	$STR_SX
		size_y	$STR_SY
	}
	resource $ACS_BMO {
		id	$ACS_SOUND_TEXT

		build
		offs_x	{ $STR_SOUND_X + $STR_DELTA_X }
		offs_y	{ $STR_START_Y + $STR_DY * 2 }

		size_x	$STR_SOUND_SX
		size_y	$STR_SY
	}
	resource $ACS_BMO {
		id	$ACS_SOUND_MODE

		build
		offs_x	{ $STR_SOUND_X + $STR_SOUND_SX + $STR_DX + $STR_DELTA_X }
		offs_y	{ $STR_START_Y + $STR_DY * 2 }

		size_x	$STR_SOUND_SX2
		size_y	$STR_SY
	}
	resource $ACS_BMO {
		id	$ACS_MUSIC_TEXT

		build
		offs_x	{ $STR_MUSIC_X + $STR_DELTA_X }
		offs_y	{ $STR_START_Y + $STR_DY * 3 }

		size_x	$STR_MUSIC_SX
		size_y	$STR_SY
	}
	resource $ACS_BMO {
		id	$ACS_MUSIC_MODE

		build
		offs_x	{ $STR_MUSIC_X + $STR_MUSIC_SX + $STR_DX + $STR_DELTA_X }
		offs_y	{ $STR_START_Y + $STR_DY * 3 }

		size_x	$STR_MUSIC_SX2
		size_y	$STR_SY
	}
	resource $ACS_BMO {
		id	$ACS_TUTORIAL_TEXT

		build
		offs_x	{ $STR_TUTORIAL_X + $STR_DELTA_X }
		offs_y	{ $STR_START_Y + $STR_DY * 4 }

		size_x	$STR_TUTORIAL_SX
		size_y	$STR_SY
	}
	resource $ACS_BMO {
		id	$ACS_TUTORIAL_MODE

		build
		offs_x	{ $STR_TUTORIAL_X + $STR_TUTORIAL_SX + $STR_DX + $STR_DELTA_X }
		offs_y	{ $STR_START_Y + $STR_DY * 4 }

		size_x	$STR_TUTORIAL_SX2
		size_y	$STR_SY
	}
	resource $ACS_BMO {
		id	$ACS_AUTORUN_TEXT

		build
		offs_x	{ $STR_AUTORUN_X + $STR_DELTA_X }
		offs_y	{ $STR_START_Y + $STR_DY * 5 }

		size_x	$STR_AUTORUN_SX
		size_y	$STR_SY
	}
	resource $ACS_BMO {
		id	$ACS_AUTORUN_MODE

		build
		offs_x	{ $STR_AUTORUN_X + $STR_AUTORUN_SX + $STR_DX + $STR_DELTA_X }
		offs_y	{ $STR_START_Y + $STR_DY * 5 }

		size_x	$STR_AUTORUN_SX2
		size_y	$STR_SY
	}
	resource $ACS_BMO {
		id	$ACS_ABORT_GAME

		build
		offs_x	{ ( $PAUSE_RES_X - $STR_SX ) / 2 }
		offs_y	{ $STR_START_Y + $STR_DY * 6 }

		size_x	$STR_SX
		size_y	$STR_SY
	}
	resource $ACS_BMO {
		id	$ACS_SOUND_VOLUME

		build
		offs_x	{ $STR_SOUND_X + $STR_SOUND_SX + $STR_DX + $STR_SOUND_SX2 + $VOLUME_DX + $STR_DELTA_X }
		offs_y	{ $STR_START_Y + $STR_DY * 2 }

		size_x	100
		size_y	30
	}
	resource $ACS_BMO {
		id	$ACS_MUSIC_VOLUME

		build
		offs_x	{ $STR_MUSIC_X + $STR_MUSIC_SX + $STR_DX + $STR_MUSIC_SX2 + $VOLUME_DX + $STR_DELTA_X }
		offs_y	{ $STR_START_Y + $STR_DY * 3 }

		size_x	100
		size_y	30
	}
	object $ACS_INPUT_FIELD_OBJ {
		id	$ACS_SOUND_TEXT
		string_len	20
		ifont		1
		color		0	$COLOR0s
		space		1
		string		$acsSTR_SOUND

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SOUND_TEXT $ACS_SOUND_TEXT 0 0
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id	$ACS_PAUSE_TEXT
		string_len	20
		ifont		0
		color		0	$COLOR0s
		space		1
		string		$acsSTR_GAME_PAUSED

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_PAUSE_TEXT $ACS_PAUSE_TEXT 0 0
			}
		}
	}
	object $ACS_SCROLLER_OBJ {
		id		$ACS_SOUND_VOLUME
		max_value	256
		colors		103	 80
		resource_id	$ACS_VOLUME_SCROLLER
		s_resource_id	$ACS_VOLUME_S_RES
		background

		active_sx	90
		active_sy	20

		event {
			key	$ACS_STARTUP_KEY
			time	2

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SOUND_VOLUME $ACS_SOUND_VOLUME 0 0
				start_time	0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SOUND_VOLUME 0 0 0
				start_time	0
			}
		}
		event {
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$iMOUSE_RIGHT_PRESS_CODE
			key	$iMOUSE_LEFT_MOVE
			key	$iMOUSE_RIGHT_MOVE

			time	2
			ev_comm {
				command $ACS_CHANGE_SCROLLER $ACS_SOUND_VOLUME 0 0 0
				start_time	0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_SET_SOUND_VOLUME 0 0 0
				start_time	0
			}
		}
	}
	object $ACS_SCROLLER_OBJ {
		id		$ACS_MUSIC_VOLUME
		max_value	256
		colors		103	 80
		resource_id	$ACS_VOLUME_SCROLLER
		s_resource_id	$ACS_VOLUME_S_RES
		background

		active_sx	90
		active_sy	20

		event {
			key	$ACS_STARTUP_KEY
			time	2

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_MUSIC_VOLUME $ACS_MUSIC_VOLUME 0 0
				start_time	0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_MUSIC_VOLUME 0 0 0
				start_time	1
			}
		}
		event {
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$iMOUSE_RIGHT_PRESS_CODE
			key	$iMOUSE_LEFT_MOVE
			key	$iMOUSE_RIGHT_MOVE

			time	2
			ev_comm {
				command $ACS_CHANGE_SCROLLER $ACS_MUSIC_VOLUME 0 0 0
				start_time	0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_SET_MUSIC_VOLUME 0 0 0
				start_time	1
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id	$ACS_SOUND_MODE
		string_len	20
		ifont		1
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1
		string		"OFF"

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_SOUND_MODE $ACS_SOUND_MODE 0 0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_SOUND_MODE 0 0 0
			}
		}
		event {
			key	$iMOUSE_RIGHT_PRESS_CODE
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$VK_S

			time	2
			ev_comm {
				command $ACS_CHANGE_STR_STATE $ACS_SOUND_MODE 0 0 0
				start_time	0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_SET_SOUND_MODE 0 0 0
				start_time	1
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id	$ACS_MUSIC_TEXT
		string_len	20
		ifont		1
		color		0	$COLOR0s
		space		1
		string		$acsSTR_MUSIC

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_MUSIC_TEXT $ACS_MUSIC_TEXT 0 0
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id	$ACS_MUSIC_MODE
		string_len	20
		ifont		1
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1
		string		"OFF"

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_MUSIC_MODE $ACS_MUSIC_MODE 0 0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_MUSIC_MODE 0 0 0
			}
		}
		event {
			key	$iMOUSE_RIGHT_PRESS_CODE
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$VK_S

			time	2
			ev_comm {
				command $ACS_CHANGE_STR_STATE $ACS_MUSIC_MODE 0 0 0
				start_time	0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_SET_MUSIC_MODE 0 0 0
				start_time	1
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id	$ACS_TUTORIAL_TEXT
		string_len	20
		ifont		1
		color		0	$COLOR0s
		space		1
		string		$acsSTR_TIPS

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_TUTORIAL_TEXT $ACS_TUTORIAL_TEXT 0 0
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id	$ACS_TUTORIAL_MODE
		string_len	20
		ifont		1
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1
		string		"OFF"

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_TUTORIAL_MODE $ACS_TUTORIAL_MODE 0 0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_TUTORIAL_MODE 0 0 0
			}
		}
		event {
			key	$iMOUSE_RIGHT_PRESS_CODE
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$VK_S

			time	2
			ev_comm {
				command $ACS_CHANGE_STR_STATE $ACS_TUTORIAL_MODE 0 0 0
				start_time	0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_SET_TUTORIAL_MODE 0 0 0
				start_time	1
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id	$ACS_AUTORUN_TEXT
		string_len	20
		ifont		1
		color		0	$COLOR0s
		space		1
		string		$acsSTR_AUTORUN

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_AUTORUN_TEXT $ACS_AUTORUN_TEXT 0 0
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id	$ACS_AUTORUN_MODE
		string_len	20
		ifont		1
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1
		string		"OFF"

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_AUTORUN_MODE $ACS_AUTORUN_MODE 0 0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_INIT_AUTORUN_MODE 0 0 0
			}
		}
		event {
			key	$iMOUSE_RIGHT_PRESS_CODE
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$VK_S

			time	2
			ev_comm {
				command $ACS_CHANGE_STR_STATE $ACS_AUTORUN_MODE 0 0 0
				start_time	0
			}
			ev_comm {
				command $ACS_EXT_EVENT $ACS_SET_AUTORUN_MODE 0 0 0
				start_time	1
			}
		}
	}
	object $ACS_INPUT_FIELD_OBJ {
		id	$ACS_RESUME_GAME
		string_len	20
		ifont		1
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1
		string		$acsSTR_RESUME_GAME

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_RESUME_GAME $ACS_RESUME_GAME 0 0
			}
		}
		event {
			key	$iMOUSE_RIGHT_PRESS_CODE
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$VK_R
			key	$VK_ESCAPE

			time	2
			ev_comm {
				command $ACS_EXIT 1 0 0 0
				start_time	0
			}
		}
	}

	object $ACS_INPUT_FIELD_OBJ {
		id	$ACS_ABORT_GAME
		string_len	20
		ifont		1
		color		0	$COLOR0
		color		1	$COLOR1
		color		2	$COLOR2
		space		1
		string		$acsSTR_ABORT_GAME

		align_center

		event {
			key	$ACS_STARTUP_KEY
			time	1

			ev_comm {
				command $ACS_SET_RESOURCE $ACS_ABORT_GAME $ACS_ABORT_GAME 0 0
			}
		}
		event {
			key	$iMOUSE_RIGHT_PRESS_CODE
			key	$iMOUSE_LEFT_PRESS_CODE
			key	$VK_A

			time	2
			ev_comm {
				command $ACS_EXT_EVENT $ACS_GLOBAL_EXIT 0 0 0
				start_time	0
			}
		}
	}

