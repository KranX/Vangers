
// IDs...
#define AML1_N0 		0
#define AML1_N1 		1
#define AML1_N2 		2
#define AML1_N3 		3
#define AML1_N4 		4

#define AML1_W1 		5
#define AML1_W2 		6
#define AML1_W3 		7
#define AML1_W4 		8

#define AML1_S0 		9
#define AML1_S1 		10
#define AML1_S2 		11
#define AML1_S3 		12

#define AML1_C1 		13
#define AML1_C2 		14
#define AML1_C3 		15
#define AML1_C4 		16

#define AML1_R1 		17
#define AML1_R2 		18
#define AML1_R3 		19
#define AML1_R4 		20

#define AML1_H0 		21
#define AML1_H1 		22
#define AML1_H2 		23

// Channels...
#define AML1_CHANNEL0		1
#define AML1_CHANNEL1		2
#define AML1_CHANNEL2		3
#define AML1_CHANNEL3		4

// Phases...
#define AML1_W1_END		6
#define AML1_W2_END		6
#define AML1_W3_END		6
#define AML1_W4_END		6

#define AML1_C1_END		8
#define AML1_C2_END		5
#define AML1_C3_END		9
#define AML1_C4_END		6

#define AML1_S3_END		17

#define AML1_H0_END		9
#define AML1_H1_END		10
#define AML1_H2_END		10

// Events..
#define AML1_W1_EVENT		1010
#define AML1_CLOSE_ELEECH	1011

// RND Events...
#define AML1_W1_RND_EVENT	1012
#define AML1_W2_RND_EVENT	1013
#define AML1_W3_RND_EVENT	1014
#define AML1_W4_RND_EVENT	1015

#define AML1_CLOSE_WND_EVENT	1016

#define AML1_CH_RND_EVENT	1017

#define AML1_WND_EVENTS 	ml_event {									\
					key		$VK_LBUTTON						\
					key		$VK_RBUTTON						\
					sequence								\
														\
					channel 	$AML1_CH_ID						\
					if_not_locked								\
					no_deactivate								\
														\
					priority	-1							\
														\
					active_time	5							\
														\
					ml_ev_comm {								\
						code	$AML_IS_ACTIVE $AML1_W1 0				\
						start_time	0						\
					}									\
					ml_ev_comm {								\
						code	$AML_IS_LOCKED $AML1_W1 0				\
						start_time	0						\
					}									\
					ml_ev_comm {								\
						code	$AML_PUT_IN_CH_QUEUE $AML1_CH_ID $AML1_WND_ID		\
						start_time	1						\
					}									\
					ml_ev_comm {								\
						code	$AML_START_SOUND $ACI_WND_SOUND 0			\
						start_time	2						\
					}									\
					ml_ev_comm {								\
						code	$AML_GO_PHASE $AML1_WND_ID 0				\
						start_time	2						\
					}									\
					ml_ev_comm {								\
						code	$AML_LOCK $AML1_WND_ID 0				\
						start_time	3						\
					}									\
					ml_ev_comm {								\
						code	$AML_PUT_IN_CH_QUEUE $AML1_CH_ID $AML1_BREATHE_ID	\
						start_time	4						\
					}									\
					ml_ev_comm {								\
						code	$AML_GO_INFINITE $AML1_BREATHE_ID 0			\
						start_time	5						\
					}									\
				}										\
				ml_event {									\
					key		$VK_LBUTTON						\
					key		$VK_RBUTTON						\
					key_code	$AML1_CLOSE_WND_EVENT					\
					sequence								\
														\
					channel 	$AML1_CH_ID						\
					if_locked								\
					no_deactivate								\
														\
					active_time	3							\
														\
					ml_ev_comm {								\
						code	$AML_UNLOCK $AML1_WND_ID 0				\
						start_time	0						\
					}									\
					ml_ev_comm {								\
						code	$AML_PUT_IN_CH_QUEUE $AML1_CH_ID $AML1_WND_ID		\
						start_time	1						\
					}									\
					ml_ev_comm {								\
						code	$AML_START_SOUND $ACI_WND_SOUND 0			\
						start_time	2						\
					}									\
					ml_ev_comm {								\
						code	$AML_GO_PHASE $AML1_WND_ID $AML1_WND_END_ID		\
						start_time	3						\
					}									\
				}										\
				ml_event {									\
					startup 	$AML_SHUTDOWN						\
					startup_data	2							\
					sequence								\
														\
					channel 	$AML1_CH_ID						\
					if_locked								\
														\
					active_time	3							\
														\
					ml_ev_comm {								\
						code	$AML_UNLOCK $AML1_WND_ID 0				\
						start_time	0						\
					}									\
					ml_ev_comm {								\
						code	$AML_PUT_IN_CH_QUEUE $AML1_CH_ID $AML1_WND_ID		\
						start_time	1						\
					}									\
					ml_ev_comm {								\
						code	$AML_START_SOUND $ACI_WND_SOUND 0				     \
						start_time	2						\
					}									\
					ml_ev_comm {								\
						code	$AML_GO_PHASE $AML1_WND_ID $AML1_WND_END_ID		\
						start_time	3						\
					}									\
				}

speech_channel		$AML1_CHANNEL0
speech_level	0	1
speech_level	1	1
speech_level	2	3

ml_data {
	id	$AML1_C1
	name	"resource/iscreen/ldata/l1/data.vot/c1.vot"

	channel $AML1_CHANNEL0
}

ml_data {
	id	$AML1_C2
	name	"resource/iscreen/ldata/l1/data.vot/c2.vot"

	channel $AML1_CHANNEL0
}

ml_data {
	id	$AML1_C3
	name	"resource/iscreen/ldata/l1/data.vot/c3.vot"

	channel $AML1_CHANNEL0
}

ml_data {
	id	$AML1_C4
	name	"resource/iscreen/ldata/l1/data.vot/c4.vot"

	channel $AML1_CHANNEL0
}

ml_data {
	id	$AML1_N0
	name	"resource/iscreen/ldata/l1/data.vot/n0.vot"

	channel $AML1_CHANNEL0
}

ml_data {
	id	$AML1_N1
	name	"resource/iscreen/ldata/l1/data.vot/n1.vot"

	channel $AML1_CHANNEL1
}

ml_data {
	id	$AML1_N2
	name	"resource/iscreen/ldata/l1/data.vot/n2.vot"

	channel $AML1_CHANNEL2
}

ml_data {
	id	$AML1_N3
	name	"resource/iscreen/ldata/l1/data.vot/n3.vot"

	channel $AML1_CHANNEL3
}

ml_data {
	id	$AML1_H1
	name	"resource/iscreen/ldata/l1/data.vot/h1.vot"

	channel $AML1_CHANNEL2
}

ml_data {
	id	$AML1_H2
	name	"resource/iscreen/ldata/l1/data.vot/h2.vot"

	channel $AML1_CHANNEL2
}

ml_data {
	id	$AML1_R1
	name	"resource/iscreen/ldata/l1/data.vot/r1.vot"

	channel $AML1_CHANNEL1
}

ml_data {
	id	$AML1_R2
	name	"resource/iscreen/ldata/l1/data.vot/r2.vot"

	channel $AML1_CHANNEL2
}

ml_data {
	id	$AML1_R3
	name	"resource/iscreen/ldata/l1/data.vot/r3.vot"

	channel $AML1_CHANNEL0
}

ml_data {
	id	$AML1_R4
	name	"resource/iscreen/ldata/l1/data.vot/r4.vot"

	channel $AML1_CHANNEL3
}

ml_data {
	id	$AML1_W2
	name	"resource/iscreen/ldata/l1/data.vot/w2.vot"

	channel $AML1_CHANNEL1

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML1_W2 $AML1_W2_END
		}
	}

	ml_event {
		key_code	$AML1_W2_RND_EVENT

		rnd		3
		startup_data	130

		channel $AML1_CHANNEL1

		if_locked
		sequence

		active_time	3

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL1 $AML1_R1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L1_SWARM_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_R1 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL1 $AML1_N1
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML1_N1 0
			start_time	3
		}
	}

	#define AML1_CH_ID		$AML1_CHANNEL1
	#define AML1_WND_ID		$AML1_W2
	#define AML1_WND_END_ID 	$AML1_W2_END
	#define AML1_BREATHE_ID 	$AML1_N1
	$AML1_WND_EVENTS
}

ml_data {
	id	$AML1_W3
	name	"resource/iscreen/ldata/l1/data.vot/w3.vot"

	channel $AML1_CHANNEL2

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML1_W3 $AML1_W3_END
		}
	}

	ml_event {
		key_code	$AML1_W3_RND_EVENT

		rnd		2
		startup_data	180

		channel $AML1_CHANNEL2

		if_locked
		sequence

		active_time	4

		ml_ev_comm {
			code	$AML_IS_NOT_LOCKED  $AML1_H0 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL2 $AML1_R2
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L1_SWARM_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_R2 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL2 $AML1_N2
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML1_N2 0
			start_time	4
		}
	}

	#define AML1_CH_ID		$AML1_CHANNEL2
	#define AML1_WND_ID		$AML1_W3
	#define AML1_WND_END_ID 	$AML1_W3_END
	#define AML1_BREATHE_ID 	$AML1_N2
	$AML1_WND_EVENTS
}


ml_data {
	id	$AML1_W4
	name	"resource/iscreen/ldata/l1/data.vot/w4.vot"

	channel $AML1_CHANNEL3

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML1_W4 $AML1_W4_END
		}
	}

	ml_event {
		key_code	$AML1_W4_RND_EVENT

		rnd		3
		startup_data	110

		channel $AML1_CHANNEL3

		if_locked
		sequence

		active_time	3

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL3 $AML1_R4
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L1_STEP0_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_R4 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL3 $AML1_N3
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML1_N3 0
			start_time	3
		}
	}

	#define AML1_CH_ID		$AML1_CHANNEL3
	#define AML1_WND_ID		$AML1_W4
	#define AML1_WND_END_ID 	$AML1_W4_END
	#define AML1_BREATHE_ID 	$AML1_N3
	$AML1_WND_EVENTS
}

#define AML1_W1_SHUTDOWN_EV	ml_event {								\
					startup 	$AML_SHUTDOWN					\
					startup_data	$AML1_W1_TIME					\
													\
					sequence							\
													\
					channel $AML1_CHANNEL0						\
					priority	$AML1_W1_PRIORITY				\
													\
					active_time	2						\
													\
					ml_ev_comm {							\
						code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_W1	\
						start_time	0					\
					}								\
					ml_ev_comm {							\
						code	$AML_GO_PHASE $AML1_W1 $AML1_W1_END		\
						start_time	1					\
					}								\
				}

ml_data {
	id	$AML1_W1
	name	"resource/iscreen/ldata/l1/data.vot/w1.vot"

	channel $AML1_CHANNEL0

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML1_W1 $AML1_W1_END
		}
	}
	ml_event {
		startup 	$AML_STARTUP
		startup_data	1

		ml_ev_comm {
			code	$AML_KEY_TRAP $AML_KEY $AML1_W1_EVENT
		}
	}
	ml_event {
		key_code	$AML1_W1_RND_EVENT

		rnd		5
		startup_data	200

		channel $AML1_CHANNEL0

		if_not_locked
		sequence

		active_time	3

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_R3
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L1_SWARM_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_R3 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_N0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML1_N0 0
			start_time	3
		}
	}

	#define AML1_W1_TIME	1
	#define AML1_W1_PRIORITY	-1
	$AML1_W1_SHUTDOWN_EV

	#define AML1_W1_TIME	10
	#define AML1_W1_PRIORITY	-2
	$AML1_W1_SHUTDOWN_EV

	#define AML1_W1_TIME	20
	#define AML1_W1_PRIORITY	-3
	$AML1_W1_SHUTDOWN_EV

	#define AML1_W1_TIME	30
	#define AML1_W1_PRIORITY	-4
	$AML1_W1_SHUTDOWN_EV

	#define AML1_W1_TIME	40
	#define AML1_W1_PRIORITY	-5
	$AML1_W1_SHUTDOWN_EV

	#define AML1_W1_TIME	50
	#define AML1_W1_PRIORITY	-6
	$AML1_W1_SHUTDOWN_EV

	#define AML1_W1_TIME	60
	#define AML1_W1_PRIORITY	-7
	$AML1_W1_SHUTDOWN_EV

	#define AML1_W1_TIME	70
	#define AML1_W1_PRIORITY	-8
	$AML1_W1_SHUTDOWN_EV

	#define AML1_W1_TIME	80
	#define AML1_W1_PRIORITY	-9
	$AML1_W1_SHUTDOWN_EV

	#define AML1_W1_TIME	90
	#define AML1_W1_PRIORITY	-10
	$AML1_W1_SHUTDOWN_EV

	#define AML1_W1_TIME	100
	#define AML1_W1_PRIORITY	-11
	$AML1_W1_SHUTDOWN_EV

	ml_event {
		key_code	$AML1_W1_EVENT
		sequence

		active_time	1

		ml_ev_comm {
			code	$AML_LOCK $AML1_W1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_SEND2ACTINT $EV_NEXT_PHRASE 0
			start_time	1
		}
	}
	ml_event {
		key	$VK_RBUTTON
		key	$VK_LBUTTON

		sequence

		active_time	20

		channel $AML1_CHANNEL0

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML1_W1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_W1
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_W1 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_N0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML1_N0 0
			start_time	4
		}
	}
}

ml_data {
	id	$AML1_S1
	name	"resource/iscreen/ldata/l1/data.vot/s1.vot"

	channel $AML1_CHANNEL0
}

ml_data {
	id	$AML1_S2
	name	"resource/iscreen/ldata/l1/data.vot/s2.vot"

	channel $AML1_CHANNEL0
}

ml_data {
	id	$AML1_S3
	name	"resource/iscreen/ldata/l1/data.vot/s3.vot"

	channel $AML1_CHANNEL0
}

ml_data {
	id	$AML1_S0
	name	"resource/iscreen/ldata/l1/data.vot/s0.vot"

	channel $AML1_CHANNEL0

	ml_event {
		key_code	$AML_SPEECH0_CH0
		key_code	$AML_SPEECH1_CH0

		sequence
		no_deactivate

		channel 	$AML1_CHANNEL0

		priority	1
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_C1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_C1 $AML1_C1_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML1_C1 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH2_CH0
		key_code	$AML_SPEECH3_CH0

		sequence
		no_deactivate

		channel 	$AML1_CHANNEL0

		priority	2
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_C3
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L1_ANGRY_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_C3 $AML1_C3_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML1_C3 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH3_EV
		sequence

		no_deactivate

		channel 	$AML1_CHANNEL0

		priority	5
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_S3
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH3_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_S3 $AML1_S3_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_EXIT 0 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH0_EV
		sequence

		no_deactivate

		channel 	$AML1_CHANNEL0

		priority	5

		active_time	1
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_S0
			start_time	0
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

		channel 	$AML1_CHANNEL0

		priority	5

		active_time	1
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_S1
			start_time	0
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

		channel 	$AML1_CHANNEL0

		priority	5

		active_time	1
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_S2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH2_SOUND 0
			start_time	1
		}
	}
	ml_event {
		key_code	$AML_SPEECH0_CH1
		key_code	$AML_SPEECH1_CH1

		sequence
		no_deactivate

		channel 	$AML1_CHANNEL0

		priority	1
		active_time	4

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_C2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_C2 $AML1_C2_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML1_C2 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_N0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML1_N0 0
			start_time	4
		}
	}
	ml_event {
		key_code	$AML_SPEECH2_CH1

		sequence
		no_deactivate

		channel 	$AML1_CHANNEL0

		priority	2
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_C4
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_C4 $AML1_C4_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML1_C4 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_END_SPEECH_EV

		channel $AML1_CHANNEL0

		sequence
		active_time	2
		priority	3

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_W1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_W1 $AML1_W1_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_LOCK $AML1_W1 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_CLOSE_SPEECH 0 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_BREATH_OFF 0 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_KEY_TRAP $AML_KEY $AML1_CLOSE_WND_EVENT
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_BEG_SPEECH_EV

		channel $AML1_CHANNEL0

		sequence
		active_time	2
		priority	3

		ml_ev_comm {
			code	$AML_UNLOCK $AML1_W1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL0 $AML1_W1
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_W1 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_BREATH_ON 0 0
			start_time	2
		}
	}
}

ml_data {
	id	$AML1_H0
	name	"resource/iscreen/ldata/l1/data.vot/h0.vot"

	channel $AML1_CHANNEL2

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_LOCK $AML1_H0 0
		}
	}
	ml_event {
		key_code	$AML_ELEECH_CODE
//		  key		  $VK_A

		no_deactivate

		channel $AML1_CHANNEL2

		sequence
		active_time	5
		priority	3

		ml_ev_comm {
			code	$AML_UNLOCK $AML1_W3 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL2 $AML1_W3
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_W3 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_LOCK $AML1_W3 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML1_H0 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL2 $AML1_H0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_H0 $AML1_H0_END
			start_time	4
		}
		ml_ev_comm {
			code	$AML_REWIND $AML1_H0 0
			start_time	5
		}
	}
	ml_event {
		key_code	$AML_KILL_ELEECH_CODE

		no_deactivate
		if_not_locked

		channel $AML1_CHANNEL2

		sequence
		active_time	6
		priority	3

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML1_H0 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL2 $AML1_H1
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_H1 $AML1_H1_END
			start_time	2
		}
		ml_ev_comm {
			code	$AML_REWIND $AML1_H1 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_LOCK $AML1_H0 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML1_W3 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_W3 $AML1_W3_END
			start_time	5
		}
	}
	ml_event {
		startup 	$AML_SHUTDOWN
		startup_data	1

		no_deactivate
		if_not_locked

		channel $AML1_CHANNEL2

		sequence
		active_time	6
		priority	3

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML1_H0 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL2 $AML1_H1
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_H1 $AML1_H1_END
			start_time	2
		}
		ml_ev_comm {
			code	$AML_REWIND $AML1_H1 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_LOCK $AML1_H0 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML1_W3 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_W3 $AML1_W3_END
			start_time	5
		}
	}
	ml_event {
		key	$VK_LBUTTON

		no_deactivate
		if_not_locked

		channel $AML1_CHANNEL2

		sequence
		active_time	8
		priority	2

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML1_H0 0
			start_time	0
		}
		ml_ev_comm {
			code		$AML_SEND2ACTINT $EV_GET_ELEECH 0
			start_time	1
		}
		ml_ev_comm {
			code		$AML_SEND2ACTINT $EV_LOCK_ISCREEN 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL2 $AML1_H2
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_H2 $AML1_H2_END
			start_time	2
		}
		ml_ev_comm {
			code	$AML_REWIND $AML1_H2 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_LOCK $AML1_H0 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML1_W3 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_W3 $AML1_W3_END
			start_time	5
		}
		ml_ev_comm {
			code	$AML_SEND2ACTINT $EV_ISCR_KEYTRAP $AML_ELEECH_CODE
			start_time	6
		}
		ml_ev_comm {
			code	$AML_SEND2ACTINT $EV_UNLOCK_ISCREEN 0
			start_time	7
		}
	}

	ml_event {
		key_code	$AML1_CLOSE_ELEECH
		startup_data	200

		no_deactivate
		if_not_locked

		channel $AML1_CHANNEL2

		rnd	1

		sequence
		active_time	5
		priority	1

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML1_CHANNEL2 $AML1_H1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_H1 $AML1_H1_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML1_H1 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_LOCK $AML1_H0 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML1_W3 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML1_W3 $AML1_W3_END
			start_time	5
		}
	}
}

ml_event_seq $AML_UNLOCK0_SEQ 1 {

	id	$AML_UNLOCK1_SEQ
	id	$AML_UNLOCK2_SEQ
	id	$AML_UNLOCK3_SEQ

	priority	3
	sequence	$AML_BEG_SPEECH_EV	$AML_KEY
}

ml_event_seq $AML_PREPARE0_SEQ 1 {

	id	$AML_PREPARE1_SEQ
	id	$AML_PREPARE2_SEQ2
	id	$AML_PREPARE3_SEQ2

	priority	1
	sequence	$AML_SPEECH0_CH0	$AML_KEY
}

ml_event_seq $AML_PREPARE2_SEQ 1 {

	id	$AML_PREPARE3_SEQ

	priority	2
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

	id	$AML_END1_SEQ
	id	$AML_END2_SEQ2

	priority	1
	sequence	$AML_SPEECH0_CH1	$AML_KEY
}

ml_event_seq $AML_END2_SEQ 1 {
	priority	2
	sequence	$AML_SPEECH2_CH1	$AML_KEY
}

ml_event_seq $AML_LOCK0_SEQ 1 {

	id	$AML_LOCK1_SEQ
	id	$AML_LOCK2_SEQ

	priority	3
	sequence	$AML_END_SPEECH_EV	$AML_KEY
}

ml_item 	$ACI_PALOCHKA	    38	   "resource/iscreen/ldata/l1/items/itm00.bmo"
ml_item 	$ACI_PALOCHKA	    38	   "resource/iscreen/ldata/l1/items/itm01.bmo"
ml_item 	$ACI_PALOCHKA	    38	   "resource/iscreen/ldata/l1/items/itm02.bmo"
ml_item 	$ACI_PALOCHKA	    38	   "resource/iscreen/ldata/l1/items/itm03.bmo"

