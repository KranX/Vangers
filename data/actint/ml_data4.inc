
// IDs...
#define AML4_N0 	0
#define AML4_N1 	1
#define AML4_N2 	2

#define AML4_C1 	3
#define AML4_C2 	4

#define AML4_S0 	5
#define AML4_S1 	6
#define AML4_S2 	7
#define AML4_S3 	8

#define AML4_W1 	9
#define AML4_W2 	10
#define AML4_W3 	11

#define AML4_R0 	12
#define AML4_R1 	13
#define AML4_R2 	14
#define AML4_R3 	15
#define AML4_R4 	16

// Channels...
#define AML4_CHANNEL0	1
#define AML4_CHANNEL1	2
#define AML4_CHANNEL2	3
#define AML4_CHANNEL3	4
#define AML4_CHANNEL4	5
#define AML4_CHANNEL5	6
#define AML4_CHANNEL6	7
#define AML4_CHANNEL7	8

// Phases...
#define AML4_W1_END	14
#define AML4_W2_END	14
#define AML4_W3_END	14

#define AML4_C1_END	6
#define AML4_C2_END	6

#define AML4_R1_END	9
#define AML4_R2_END	11
#define AML4_R3_END	13
#define AML4_R4_END	10

#define AML4_S3_END	12

// Events...
#define AML4_R0_EV	4000
#define AML4_R1_EV	4001
#define AML4_R2_EV	4002
#define AML4_R3_EV	4003
#define AML4_R4_EV	4004

#define AML4_FREEZE_EV	ml_event {					\
				startup 	$AML_SHUTDOWN		\
				startup_data	1			\
									\
				active_time	1			\
									\
				ml_ev_comm {				\
					code	$AML_LOCK $AML4_ML_ID 0 \
				}					\
				ml_ev_comm {				\
					code	$AML_DISABLE_ML $AML4_ML_ID 0 \
				}					\
			}

#define AML4_BUG_EVENTS ml_event {								\
				key_code	     $AML4_BUG_KEY				\
												\
				rnd		5						\
				startup_data	50						\
												\
				sequence							\
				if_locked							\
				no_deactivate							\
												\
				active_time	3						\
												\
				channel 	$AML4_CH_ID					\
												\
				ml_ev_comm {							\
					code	$AML_IS_LOCKED $AML4_WND_ID 0			\
					start_time	0					\
				}								\
				ml_ev_comm {							\
					code	$AML_IS_ACTIVE $AML4_WND_ID 0			\
					start_time	0					\
				}								\
				ml_ev_comm {							\
					code	$AML_UNLOCK $AML4_BUG_ID 0			\
					start_time	1					\
				}								\
				ml_ev_comm {							\
					code	$AML_PUT_IN_CH_QUEUE $AML4_CH_ID $AML4_BUG_ID	\
					start_time	2					\
				}								\
				ml_ev_comm {							\
					code	$AML_START_SOUND $ACI_L4_STEP_SOUND 0		\
					start_time	3					\
				}								\
				ml_ev_comm {							\
					code	$AML_GO_PHASE $AML4_BUG_ID $AML4_BUG_PHASE	\
					start_time	3					\
				}								\
			}									\
			ml_event {								\
				key	$VK_LBUTTON						\
				key	$VK_RBUTTON						\
												\
				sequence							\
				if_not_locked							\
				no_deactivate							\
												\
				active_time	3						\
				priority	-1						\
												\
				channel 	$AML4_CH_ID					\
												\
				ml_ev_comm {							\
					code	$AML_IS_LOCKED $AML4_WND_ID 0			\
					start_time	0					\
				}								\
				ml_ev_comm {							\
					code	$AML_IS_ACTIVE $AML4_WND_ID 0			\
					start_time	0					\
				}								\
				ml_ev_comm {							\
					code	$AML_PUT_IN_CH_QUEUE $AML4_CH_ID $AML4_BUG_ID	\
					start_time	1					\
				}								\
				ml_ev_comm {							\
					code	$AML_GO_PHASE $AML4_BUG_ID 0			\
					start_time	2					\
				}								\
				ml_ev_comm {							\
					code	$AML_START_SOUND $ACI_L4_STEP_SOUND 0		\
					start_time	2					\
				}								\
				ml_ev_comm {							\
					code	$AML_LOCK $AML4_BUG_ID 0			\
					start_time	3					\
				}								\
			}									\
			ml_event {								\
				startup 	$AML_STARTUP					\
												\
				ml_ev_comm {							\
					code	$AML_LOCK $AML4_BUG_ID 0			\
				}								\
				ml_ev_comm {							\
					code	$AML_DISABLE_ML $AML4_BUG_ID 0			\
				}								\
			}

ml_data {
	id	$AML4_W2
	name	"resource/iscreen/ldata/l4/data.vot/w2.vot"

	channel $AML4_CHANNEL0

	ml_event {
		startup 	$AML_SHUTDOWN
		startup_data	2

		channel 	$AML4_CHANNEL0
		priority	10

		sequence
		active_time	1

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL0 $AML4_W2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_W2 $AML4_W2_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	1
		}
	}

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		sequence
		no_deactivate
		channel 	$AML4_CHANNEL0

		active_time	2

		ml_ev_comm {
			code	$AML_SET_PHASE $AML4_W2 $AML4_W2_END
			start_time	0
		}
		ml_ev_comm {
			code	$AML_LOCK $AML4_W2 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_SEND2ACTINT $EV_NEXT_PHRASE 0
			start_time	2
		}
	}
}

ml_data {
	id	$AML4_W1
	name	"resource/iscreen/ldata/l4/data.vot/w1.vot"

	channel $AML4_CHANNEL1

	ml_event {
		startup 	$AML_SHUTDOWN
		startup_data	2

		channel 	$AML4_CHANNEL1
		priority	10

		sequence
		active_time	2

		ml_ev_comm {
			code	$AML_DISABLE_ML $AML4_R0 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL1 $AML4_W1
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_W1 $AML4_W1_END
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	2
		}
	}

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		sequence
		no_deactivate
		channel 	$AML4_CHANNEL1

		active_time	1

		ml_ev_comm {
			code	$AML_SET_PHASE $AML4_W1 $AML4_W1_END
			start_time	0
		}
		ml_ev_comm {
			code	$AML_LOCK $AML4_W1 0
			start_time	1
		}
	}

	ml_event {
		key	$VK_LBUTTON
		key	$VK_RBUTTON

		if_locked

		sequence
		no_deactivate
		channel 	$AML4_CHANNEL0
		priority	-1

		active_time	4

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML4_W2 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_IS_ACTIVE $AML4_W2 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML4_W1 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_W1 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL1 $AML4_N1
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML4_N1 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_ENABLE_ML $AML4_R0 0
			start_time	4
		}
	}
}

ml_data {
	id	$AML4_W3
	name	"resource/iscreen/ldata/l4/data.vot/w3.vot"

	channel $AML4_CHANNEL2

	ml_event {
		startup 	$AML_SHUTDOWN
		startup_data	2

		channel 	$AML4_CHANNEL2
		priority	10

		sequence
		active_time	1

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL2 $AML4_W3
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_W3 $AML4_W3_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	1
		}
	}

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		sequence
		no_deactivate
		channel 	$AML4_CHANNEL2

		active_time	1

		ml_ev_comm {
			code	$AML_SET_PHASE $AML4_W3 $AML4_W3_END
			start_time	0
		}
		ml_ev_comm {
			code	$AML_LOCK $AML4_W3 0
			start_time	1
		}
	}

	ml_event {
		key	$VK_LBUTTON
		key	$VK_RBUTTON

		if_locked

		sequence
		no_deactivate
		channel 	$AML4_CHANNEL0
		priority	-1

		active_time	4

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML4_W2 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_IS_ACTIVE $AML4_W2 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML4_W3 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_W3 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL2 $AML4_N2
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML4_N2 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_ENABLE_ML $AML4_R1 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_ENABLE_ML $AML4_R2 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_ENABLE_ML $AML4_R3 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_ENABLE_ML $AML4_R4 0
			start_time	4
		}
	}
}

ml_data {
	id	$AML4_N0
	name	"resource/iscreen/ldata/l4/data.vot/n0.vot"

	channel $AML4_CHANNEL0

	#define AML4_ML_ID	$AML4_N0
	$AML4_FREEZE_EV
}

ml_data {
	id	$AML4_N1
	name	"resource/iscreen/ldata/l4/data.vot/n1.vot"

	channel $AML4_CHANNEL1

	#define AML4_ML_ID	$AML4_N1
	$AML4_FREEZE_EV
}

ml_data {
	id	$AML4_N2
	name	"resource/iscreen/ldata/l4/data.vot/n2.vot"

	channel $AML4_CHANNEL2

	#define AML4_ML_ID	$AML4_N2
	$AML4_FREEZE_EV
}

ml_data {
	id	$AML4_R0
	name	"resource/iscreen/ldata/l4/data.vot/r0.vot"

	channel $AML4_CHANNEL3

	#define AML4_ML_ID	$AML4_R0
	$AML4_FREEZE_EV

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		active_time	1

		ml_ev_comm {
			code	$AML_DISABLE_ML $AML4_R0 0
		}
	}
	ml_event {
		key_code	$AML4_R0_EV

		rnd		2
		startup_data	100

		channel $AML4_CHANNEL3

		sequence

		active_time	2

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML4_W1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_IS_ACTIVE $AML4_W1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL3 $AML4_R0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L4_HAND_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_R0 0
			start_time	2
		}
	}
}

ml_data {
	id	$AML4_R1
	name	"resource/iscreen/ldata/l4/data.vot/r1.vot"

	channel $AML4_CHANNEL4

	frame_check

	#define AML4_ML_ID	$AML4_R1
	$AML4_FREEZE_EV

	#define AML4_WND_ID	$AML4_W3
	#define AML4_CH_ID	$AML4_CHANNEL4
	#define AML4_BUG_ID	$AML4_R1
	#define AML4_BUG_PHASE	$AML4_R1_END
	#define AML4_BUG_KEY	$AML4_R1_EV

	$AML4_BUG_EVENTS
}

ml_data {
	id	$AML4_R2
	name	"resource/iscreen/ldata/l4/data.vot/r2.vot"

	channel $AML4_CHANNEL5

	frame_check

	#define AML4_ML_ID	$AML4_R2
	$AML4_FREEZE_EV

	#define AML4_WND_ID	$AML4_W3
	#define AML4_CH_ID	$AML4_CHANNEL5
	#define AML4_BUG_ID	$AML4_R2
	#define AML4_BUG_PHASE	$AML4_R2_END
	#define AML4_BUG_KEY	$AML4_R2_EV

	$AML4_BUG_EVENTS
}

ml_data {
	id	$AML4_R3
	name	"resource/iscreen/ldata/l4/data.vot/r3.vot"

	channel $AML4_CHANNEL6

	frame_check

	#define AML4_ML_ID	$AML4_R3
	$AML4_FREEZE_EV

	#define AML4_WND_ID	$AML4_W3
	#define AML4_CH_ID	$AML4_CHANNEL6
	#define AML4_BUG_ID	$AML4_R3
	#define AML4_BUG_PHASE	$AML4_R3_END
	#define AML4_BUG_KEY	$AML4_R3_EV

	$AML4_BUG_EVENTS
}

ml_data {
	id	$AML4_R4
	name	"resource/iscreen/ldata/l4/data.vot/r4.vot"

	channel $AML4_CHANNEL7

	#define AML4_ML_ID	$AML4_R4
	$AML4_FREEZE_EV

	frame_check

	#define AML4_WND_ID	$AML4_W3
	#define AML4_CH_ID	$AML4_CHANNEL7
	#define AML4_BUG_ID	$AML4_R4
	#define AML4_BUG_PHASE	$AML4_R4_END
	#define AML4_BUG_KEY	$AML4_R4_EV

	$AML4_BUG_EVENTS
}

ml_data {
	id	$AML4_S1
	name	"resource/iscreen/ldata/l4/data.vot/s1.vot"

	channel $AML4_CHANNEL0

	#define AML4_ML_ID	$AML4_S1
	$AML4_FREEZE_EV
}

ml_data {
	id	$AML4_S2
	name	"resource/iscreen/ldata/l4/data.vot/s2.vot"

	channel $AML4_CHANNEL0

	#define AML4_ML_ID	$AML4_S2
	$AML4_FREEZE_EV
}

ml_data {
	id	$AML4_S3
	name	"resource/iscreen/ldata/l4/data.vot/s3.vot"

	channel $AML4_CHANNEL0

	#define AML4_ML_ID	$AML4_S3
	$AML4_FREEZE_EV
}

ml_data {
	id	$AML4_C1
	name	"resource/iscreen/ldata/l4/data.vot/c1.vot"

	channel $AML4_CHANNEL0

	#define AML4_ML_ID	$AML4_C1
	$AML4_FREEZE_EV
}

ml_data {
	id	$AML4_C2
	name	"resource/iscreen/ldata/l4/data.vot/c2.vot"

	channel $AML4_CHANNEL0

	#define AML4_ML_ID	$AML4_C2
	$AML4_FREEZE_EV
}

ml_data {
	id	$AML4_S0
	name	"resource/iscreen/ldata/l4/data.vot/s0.vot"

	channel $AML4_CHANNEL0


	#define AML4_ML_ID	$AML4_S0
	$AML4_FREEZE_EV

	ml_event {
		key_code	$AML_BEG_SPEECH_EV

		channel $AML4_CHANNEL0

		sequence
		active_time	1
		priority	2

		ml_ev_comm {
			code	$AML_UNLOCK $AML4_W2 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_W2 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_BREATH_ON 0 0
			start_time	1
		}
	}
	ml_event {
		key_code	$AML_END_SPEECH_EV

		channel $AML4_CHANNEL0

		sequence
		active_time	5
		priority	2

		ml_ev_comm {
			code	$AML_LOCK $AML4_R0 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_LOCK $AML4_R1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_LOCK $AML4_R2 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_LOCK $AML4_R3 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_LOCK $AML4_R4 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_DISABLE_ML $AML4_R0 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_DISABLE_ML $AML4_R1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_DISABLE_ML $AML4_R2 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_DISABLE_ML $AML4_R3 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_DISABLE_ML $AML4_R4 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_LOCK $AML4_N1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_LOCK $AML4_N2 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_W1 $AML4_W1_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_W3 $AML4_W1_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_LOCK $AML4_W1 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_LOCK $AML4_W3 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL0 $AML4_W2
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_W2 $AML4_W2_END
			start_time	4
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_LOCK $AML4_W2 0
			start_time	5
		}
		ml_ev_comm {
			code	$AML_CLOSE_SPEECH 0 0
			start_time	5
		}
		ml_ev_comm {
			code	$AML_BREATH_OFF 0 0
			start_time	5
		}
	}
	ml_event {
		key_code	$AML_SPEECH1_CH0
		key_code	$AML_SPEECH2_CH0
		key_code	$AML_SPEECH3_CH0

		sequence
		no_deactivate

		channel 	$AML4_CHANNEL0

		priority	1
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL0 $AML4_C1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_C1 $AML4_C1_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML4_C1 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH1_CH1
		key_code	$AML_SPEECH2_CH1
		key_code	$AML_SPEECH3_CH1

		sequence
		no_deactivate

		channel 	$AML4_CHANNEL0

		priority	1
		active_time	4

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL0 $AML4_C2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_C2 $AML4_C2_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML4_C2 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL0 $AML4_N0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML4_N0 0
			start_time	4
		}
	}
	ml_event {
		key_code	$AML_SPEECH0_CH0

		sequence
		no_deactivate

		channel 	$AML4_CHANNEL0

		priority	3
		active_time	1

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML4_S0 0
			start_time	0
		}
	}
	ml_event {
		key_code	$AML_SPEECH0_CH1

		sequence
		no_deactivate

		channel 	$AML4_CHANNEL0

		priority	3
		active_time	1

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL0 $AML4_N0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML4_N0 0
			start_time	1
		}
	}
	ml_event {
		key_code	$AML_SPEECH3_EV

		sequence
		no_deactivate

		channel 	$AML4_CHANNEL0

		priority	5
		active_time	4

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL0 $AML4_S2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_S2 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH3_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL0 $AML4_S3
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_S3 $AML4_S3_END
			start_time	3
		}
		ml_ev_comm {
			code	$AML_EXIT 0 0
			start_time	4
		}
	}
	ml_event {
		key_code	$AML_SPEECH0_EV

		sequence
		no_deactivate

		channel 	$AML4_CHANNEL0

		priority	5
		active_time	1

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL0 $AML4_S0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_S0 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH0_SOUND 0
			start_time	1
		}
	}
	ml_event {
		key_code	$AML_SPEECH1_EV

		sequence
		no_deactivate

		channel 	$AML4_CHANNEL0

		priority	5
		active_time	1

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL0 $AML4_S1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_S1 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH1_SOUND 0
			start_time	1
		}
	}
	ml_event {
		key_code	$AML_SPEECH2_EV

		sequence
		no_deactivate

		channel 	$AML4_CHANNEL0

		priority	5
		active_time	1

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML4_CHANNEL0 $AML4_S2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML4_S2 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH2_SOUND 0
			start_time	1
		}
	}
}

speech_channel		$AML4_CHANNEL0

ml_event_seq $AML_UNLOCK0_SEQ 1 {

	id	$AML_UNLOCK1_SEQ
	id	$AML_UNLOCK2_SEQ
	id	$AML_UNLOCK3_SEQ

	priority	2
	sequence	$AML_BEG_SPEECH_EV	$AML_KEY
}

ml_event_seq $AML_PREPARE0_SEQ 1 {
	priority	3
	sequence	$AML_SPEECH0_CH0	$AML_KEY
}

ml_event_seq $AML_PREPARE1_SEQ 1 {

	id	$AML_PREPARE2_SEQ2
	id	$AML_PREPARE3_SEQ2

	priority	1
	sequence	$AML_SPEECH1_CH0	$AML_KEY
}

ml_event_seq $AML_SPEECH0_SEQ 1 {
	sequence	$AML_SPEECH0_EV 	$AML_KEY
}
ml_event_seq $AML_SPEECH1_SEQ 1 {
	sequence	$AML_SPEECH1_EV 	$AML_KEY
}
ml_event_seq $AML_SPEECH2_SEQ 1 {
	sequence	$AML_SPEECH2_EV 	$AML_KEY
}
ml_event_seq $AML_SPEECH3_SEQ 1 {
	sequence	$AML_SPEECH3_EV 	$AML_KEY
}

ml_event_seq $AML_END0_SEQ 1 {
	priority	3
	sequence	$AML_SPEECH0_CH1	$AML_KEY
}

ml_event_seq $AML_END1_SEQ 1 {

	id	$AML_END2_SEQ

	priority	1
	sequence	$AML_SPEECH1_CH1	$AML_KEY
}

ml_event_seq $AML_LOCK0_SEQ 1 {

	id	$AML_LOCK1_SEQ
	id	$AML_LOCK2_SEQ

	priority	2
	sequence	$AML_END_SPEECH_EV	$AML_KEY
}


ml_item 	$ACI_RUBOX	 5	"resource/iscreen/ldata/l4/items/itm00.bmo"
ml_item 	$ACI_RUBOX	 5	"resource/iscreen/ldata/l4/items/itm01.bmo"
ml_item 	$ACI_RUBOX	 5	"resource/iscreen/ldata/l4/items/itm02.bmo"
ml_item 	$ACI_RUBOX	 5	"resource/iscreen/ldata/l4/items/itm03.bmo"
ml_item 	$ACI_RUBOX	 5	"resource/iscreen/ldata/l4/items/itm04.bmo"
ml_item 	$ACI_RUBOX	 5	"resource/iscreen/ldata/l4/items/itm05.bmo"
ml_item 	$ACI_RUBOX	 5	"resource/iscreen/ldata/l4/items/itm06.bmo"

ml_item 	$ACI_PIPKA	 5	"resource/iscreen/ldata/l4/items/itm07.bmo"
ml_item 	$ACI_PIPKA	 5	"resource/iscreen/ldata/l4/items/itm08.bmo"
ml_item 	$ACI_PIPKA	 5	"resource/iscreen/ldata/l4/items/itm09.bmo"
ml_item 	$ACI_PIPKA	 5	"resource/iscreen/ldata/l4/items/itm10.bmo"
ml_item 	$ACI_PIPKA	 5	"resource/iscreen/ldata/l4/items/itm11.bmo"
ml_item 	$ACI_PIPKA	 5	"resource/iscreen/ldata/l4/items/itm12.bmo"
ml_item 	$ACI_PIPKA	 5	"resource/iscreen/ldata/l4/items/itm13.bmo"

