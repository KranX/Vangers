
// IDs..
#define AML6_N1 	0
#define AML6_N2 	1

#define AML6_R1 	2
#define AML6_R2 	3

#define AML6_C1 	4
#define AML6_C2 	5
#define AML6_C3 	6
#define AML6_C4 	7
#define AML6_C5 	8
#define AML6_C6 	9

#define AML6_S0 	10
#define AML6_S1 	11
#define AML6_S2 	12
#define AML6_S3 	13

#define AML6_W1 	14

#define AML6_R1 	15
#define AML6_R2 	16
#define AML6_R3 	17

// Channels...
#define AML6_CHANNEL0	1
#define AML6_CHANNEL1	2
#define AML6_CHANNEL2	3

// Phases...
#define AML6_W1_END	6

#define AML6_C1_END	3
#define AML6_C2_END	3
#define AML6_C3_END	4
#define AML6_C4_END	3
#define AML6_C5_END	3
#define AML6_C6_END	3

#define AML6_S3_END	16

// Events...
#define AML6_R1_EVENT	6000
#define AML6_R2_EVENT1	6001
#define AML6_R2_EVENT2	6002
#define AML6_R2_EVENT3	6003
#define AML6_R2_EVENT4	6004
#define AML6_R3_EVENT	6005

#define AML6_W1_EVENT		6006
#define AML6_W1_MOUSE_EV	6007

#define AML6_FREEZE_EV	ml_event {					\
				startup 	$AML_SHUTDOWN		\
				sequence				\
				startup_data	1			\
									\
				active_time	1			\
									\
				ml_ev_comm {				\
					code	$AML_LOCK $AML6_ML_ID 0 \
				}					\
			}

ml_data {
	id	$AML6_N1
	name	"resource/iscreen/ldata/l6/data.vot/n1.vot"

	channel $AML6_CHANNEL0

	#define AML6_ML_ID	$AML6_N1
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_N2
	name	"resource/iscreen/ldata/l6/data.vot/n2.vot"

	channel $AML6_CHANNEL1

	#define AML6_ML_ID	$AML6_N2
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_C1
	name	"resource/iscreen/ldata/l6/data.vot/c1.vot"

	channel $AML6_CHANNEL1

	#define AML6_ML_ID	$AML6_C1
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_C2
	name	"resource/iscreen/ldata/l6/data.vot/c2.vot"

	channel $AML6_CHANNEL1

	#define AML6_ML_ID	$AML6_C2
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_C3
	name	"resource/iscreen/ldata/l6/data.vot/c3.vot"

	channel $AML6_CHANNEL1

	#define AML6_ML_ID	$AML6_C3
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_C4
	name	"resource/iscreen/ldata/l6/data.vot/c4.vot"

	channel $AML6_CHANNEL1

	#define AML6_ML_ID	$AML6_C4
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_C5
	name	"resource/iscreen/ldata/l6/data.vot/c5.vot"

	channel $AML6_CHANNEL1

	#define AML6_ML_ID	$AML6_C5
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_C6
	name	"resource/iscreen/ldata/l6/data.vot/c6.vot"

	channel $AML6_CHANNEL1

	#define AML6_ML_ID	$AML6_C6
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_R1
	name	"resource/iscreen/ldata/l6/data.vot/r1.vot"

	channel $AML6_CHANNEL1

	ml_event {
		key_code	$AML6_R1_EVENT

		rnd		3
		startup_data	50

		channel $AML6_CHANNEL1

		sequence

		active_time	4

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML6_W1 0
			start_time 0
		}
		ml_ev_comm {
			code	$AML_IS_ACTIVE $AML6_W1 0
			start_time 0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_R1
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L6_SCRATCH_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_R1 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_N2
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML6_N2 0
			start_time	4
		}
	}

	#define AML6_ML_ID	$AML6_R1
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_R2
	name	"resource/iscreen/ldata/l6/data.vot/r2.vot"

	channel $AML6_CHANNEL0

	ml_event {
		key_code	$AML6_R2_EVENT1

		rnd		3
		startup_data	75

		channel $AML6_CHANNEL0

		sequence

		active_time	4

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML6_W1 0
			start_time 0
		}
		ml_ev_comm {
			code	$AML_IS_ACTIVE $AML6_W1 0
			start_time 0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_R2
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L6_CLAP_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_R2 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_N1
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML6_N1 0
			start_time	4
		}
	}
	ml_event {
		key_code	$AML6_R2_EVENT2

		rnd		3
		startup_data	75

		channel $AML6_CHANNEL0

		sequence

		active_time	5

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML6_W1 0
			start_time 0
		}
		ml_ev_comm {
			code	$AML_IS_ACTIVE $AML6_W1 0
			start_time 0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_R2
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L6_CLAP_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_R2
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L6_CLAP_SOUND 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_R2 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_N1
			start_time	4
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML6_N1 0
			start_time	5
		}
	}
	ml_event {
		key_code	$AML6_R2_EVENT3

		rnd		3
		startup_data	100

		channel $AML6_CHANNEL0

		sequence

		active_time	6

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML6_W1 0
			start_time 0
		}
		ml_ev_comm {
			code	$AML_IS_ACTIVE $AML6_W1 0
			start_time 0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_R2
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_R2
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L6_CLAP_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_R2
			start_time	3
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L6_CLAP_SOUND 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_R2 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_N1
			start_time	5
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML6_N1 0
			start_time	6
		}
	}
	ml_event {
		key_code	$AML6_R2_EVENT4

		rnd		3
		startup_data	150

		channel $AML6_CHANNEL0

		sequence

		active_time	7

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML6_W1 0
			start_time 0
		}
		ml_ev_comm {
			code	$AML_IS_ACTIVE $AML6_W1 0
			start_time 0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_R2
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L6_CLAP_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_R2
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_R2
			start_time	3
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L6_CLAP_SOUND 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_R2
			start_time	4
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L6_CLAP_SOUND 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_R2 0
			start_time	5
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL0 $AML6_N1
			start_time	6
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML6_N1 0
			start_time	7
		}
	}

	#define AML6_ML_ID	$AML6_R2
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_R3
	name	"resource/iscreen/ldata/l6/data.vot/r3.vot"

	channel $AML6_CHANNEL1

	ml_event {
		key_code	$AML6_R3_EVENT

		rnd		3
		startup_data	50

		channel $AML6_CHANNEL1

		sequence

		active_time	4

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML6_W1 0
			start_time 0
		}
		ml_ev_comm {
			code	$AML_IS_ACTIVE $AML6_W1 0
			start_time 0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_R3
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L6_SCRATCH_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_R3 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_N2
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML6_N2 0
			start_time	4
		}
	}

	#define AML6_ML_ID	$AML6_R3
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_S0
	name	"resource/iscreen/ldata/l6/data.vot/s0.vot"

	channel $AML6_CHANNEL1

	ml_event {
		key_code	$AML_BEG_SPEECH_EV

		channel $AML6_CHANNEL1

		sequence
		active_time	3
		priority	4

		ml_ev_comm {
			code	$AML_UNLOCK $AML6_W1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_W1 0
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
		ml_ev_comm {
			code	$AML_UNLOCK $AML6_N1 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML6_N1 0
			start_time	3
		}
	}
	ml_event {
		key_code	$AML_END_SPEECH_EV

		channel $AML6_CHANNEL1

		sequence
		active_time	3
		priority	4

		ml_ev_comm {
			code	$AML_LOCK $AML6_N1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_LOCK $AML6_N2 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_LOCK $AML6_R2 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_W1
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_W1 $AML6_W1_END
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_LOCK $AML6_W1 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_CLOSE_SPEECH 0 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_BREATH_OFF 0 0
			start_time	3
		}
	}
	ml_event {
		key_code	$AML_SPEECH0_CH0

		sequence
		no_deactivate

		channel 	$AML0_CHANNEL1

		priority	1
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_C1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_C1 $AML6_C1_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML6_C1 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH1_CH0

		sequence
		no_deactivate

		channel 	$AML0_CHANNEL1

		priority	2
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_C3
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_C3 $AML6_C3_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML6_C3 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH2_CH0

		sequence
		no_deactivate

		channel 	$AML0_CHANNEL1

		priority	3
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_C4
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_C4 $AML6_C4_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML6_C4 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH0_EV
		sequence

		no_deactivate

		channel 	$AML6_CHANNEL1

		priority	5

		active_time	1
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_S0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_S0 0
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

		channel 	$AML6_CHANNEL1

		priority	5

		active_time	1
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_S1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_S1 0
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

		channel 	$AML6_CHANNEL1

		priority	5

		active_time	1
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_S2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_S2 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH2_SOUND 0
			start_time	1
		}
	}
	ml_event {
		key_code	$AML_SPEECH3_EV
		sequence

		no_deactivate

		channel 	$AML6_CHANNEL1

		priority	5

		active_time	4
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_S2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_S2 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH2_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_S3
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L6_GETOUT_SOUND 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_S3 $AML6_S3_END
			start_time	3
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH3_SOUND 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_EXIT 0 0
			start_time	4
		}
	}

	ml_event {
		key_code	$AML_SPEECH0_CH1

		sequence
		no_deactivate

		channel 	$AML6_CHANNEL1

		priority	1
		active_time	4

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_C2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_C2 $AML6_C2_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML6_C2 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_N2
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML6_N2 0
			start_time	4
		}
	}

	ml_event {
		key_code	$AML_SPEECH1_CH1

		sequence
		no_deactivate

		channel 	$AML6_CHANNEL1

		priority	2
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_C5
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_C5 $AML6_C5_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML6_C5 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH2_CH1

		sequence
		no_deactivate

		channel 	$AML6_CHANNEL1

		priority	3
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_C6
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_C6 $AML6_C6_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML6_C6 0
			start_time	2
		}
	}

	#define AML6_ML_ID	$AML6_S0
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_S1
	name	"resource/iscreen/ldata/l6/data.vot/s1.vot"

	channel $AML6_CHANNEL1

	#define AML6_ML_ID	$AML6_S1
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_S2
	name	"resource/iscreen/ldata/l6/data.vot/s2.vot"

	channel $AML6_CHANNEL1

	#define AML6_ML_ID	$AML6_S2
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_S3
	name	"resource/iscreen/ldata/l6/data.vot/s3.vot"

	channel $AML6_CHANNEL1

	#define AML6_ML_ID	$AML6_S3
	$AML6_FREEZE_EV
}

ml_data {
	id	$AML6_W1
	name	"resource/iscreen/ldata/l6/data.vot/w1.vot"

	channel $AML6_CHANNEL1

	ml_event {
		startup 	$AML_SHUTDOWN
		startup_data	5

		channel 	$AML6_CHANNEL1
		priority	10

		sequence
		active_time	20

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML6_CHANNEL1 $AML6_W1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML6_W1 $AML6_W1_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_LOCK $AML6_W1 0
			start_time	2
		}
	}
	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML6_W1 $AML6_W1_END
		}
	}
	ml_event {
		startup 	$AML_STARTUP
		startup_data	1

		ml_ev_comm {
			code	$AML_KEY_TRAP $AML_KEY $AML6_W1_EVENT
		}
	}
	ml_event {
		key_code	$AML6_W1_EVENT
		sequence

		active_time	1

		channel $AML6_CHANNEL1

		ml_ev_comm {
			code	$AML_LOCK $AML6_W1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_SEND2ACTINT $EV_NEXT_PHRASE 0
			start_time	1
		}
	}
}

speech_channel		$AML6_CHANNEL1

ml_event_seq $AML_UNLOCK0_SEQ 1 {

	id	$AML_UNLOCK1_SEQ
	id	$AML_UNLOCK2_SEQ
	id	$AML_UNLOCK3_SEQ

	priority	4
	sequence	$AML_BEG_SPEECH_EV	$AML_KEY
}

ml_event_seq $AML_PREPARE0_SEQ 1 {

	id	$AML_PREPARE1_SEQ2
	id	$AML_PREPARE2_SEQ2
	id	$AML_PREPARE3_SEQ2

	priority	1
	sequence	$AML_SPEECH0_CH0	$AML_KEY
}

ml_event_seq $AML_PREPARE1_SEQ 1 {
	priority	2
	sequence	$AML_SPEECH1_CH0	$AML_KEY
}

ml_event_seq $AML_PREPARE2_SEQ 1 {

	id	$AML_PREPARE3_SEQ

	priority	3
	sequence	$AML_SPEECH2_CH0	$AML_KEY
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

	id	$AML_END1_SEQ2
	id	$AML_END2_SEQ2

	priority	1
	sequence	$AML_SPEECH0_CH1	$AML_KEY
}

ml_event_seq $AML_END1_SEQ 1 {
	priority	2
	sequence	$AML_SPEECH1_CH1	$AML_KEY
}

ml_event_seq $AML_END2_SEQ 1 {
	priority	3
	sequence	$AML_SPEECH2_CH1	$AML_KEY
}

ml_event_seq $AML_LOCK0_SEQ 1 {

	id	$AML_LOCK1_SEQ
	id	$AML_LOCK2_SEQ

	priority	4
	sequence	$AML_END_SPEECH_EV	$AML_KEY
}

ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm00.bmo"
ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm01.bmo"
ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm02.bmo"
ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm03.bmo"
ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm04.bmo"
ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm05.bmo"
ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm06.bmo"
ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm07.bmo"
ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm08.bmo"
ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm09.bmo"
ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm10.bmo"
ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm11.bmo"
ml_item 	$ACI_ZEEFICK	   1	  "resource/iscreen/ldata/l6/items/itm12.bmo"

