
// IDs...
#define AML0_N0 		0
#define AML0_N1 		1
#define AML0_N2 		2
#define AML0_N3 		3
#define AML0_N4 		4
#define AML0_N5 		5

#define AML0_W1 		6
#define AML0_W2 		7
#define AML0_W3 		8
#define AML0_W4 		9
#define AML0_W5 		10
#define AML0_W6 		11

#define AML0_S0 		12
#define AML0_S1 		13
#define AML0_S2 		14
#define AML0_S3 		15

#define AML0_C1 		16
#define AML0_C2 		17
#define AML0_C3 		18
#define AML0_C4 		19

#define AML0_R1 		20
#define AML0_R2 		21
#define AML0_R3 		22
#define AML0_R4 		23

// Channels...
#define AML0_CHANNEL0		1
#define AML0_CHANNEL1		2
#define AML0_CHANNEL2		3
#define AML0_CHANNEL3		4
#define AML0_CHANNEL4		5
#define AML0_CHANNEL5		6

// Phases...
#define AML0_W1_END		6
#define AML0_W2_END		6
#define AML0_W3_END		6
#define AML0_W4_END		6
#define AML0_W5_END		6
#define AML0_W6_END		6

#define AML0_C1_END		7
#define AML0_C2_END		7
#define AML0_C3_END		11
#define AML0_C4_END		5

#define AML0_S3_END		8

// Events..
#define AML0_W1_EVENT		910

// RND Events...
#define AML0_W1_RND_EVENT	911
#define AML0_W2_RND_EVENT	912
#define AML0_W3_RND_EVENT	913
#define AML0_W5_RND_EVENT	914
#define AML0_RND_CH_EVENT	915

// Windows...
#define AML0_CLOSE_WND_EVENT	916

#define AML0_WND_EVENTS 	ml_event {									\
					key		$VK_LBUTTON						\
					key		$VK_RBUTTON						\
					sequence								\
														\
					priority	-1							\
														\
					channel 	$AML0_CH_ID						\
					if_not_locked								\
					no_deactivate								\
														\
					active_time	6							\
														\
					ml_ev_comm {								\
						code	$AML_IS_ACTIVE $AML0_W1 0				\
						start_time	0						\
					}									\
					ml_ev_comm {								\
						code	$AML_IS_LOCKED $AML0_W1 0				\
						start_time	0						\
					}									\
					ml_ev_comm {								\
						code	$AML_PUT_IN_CH_QUEUE $AML0_CH_ID $AML0_WND_ID		\
						start_time	1						\
					}									\
					ml_ev_comm {								\
						code	$AML_START_SOUND $ACI_WND_SOUND 0			\
						start_time	2						\
					}									\
					ml_ev_comm {								\
						code	$AML_GO_PHASE $AML0_WND_ID 0				\
						start_time	3						\
					}									\
					ml_ev_comm {								\
						code	$AML_LOCK $AML0_WND_ID 0				\
						start_time	4						\
					}									\
					ml_ev_comm {								\
						code	$AML_PUT_IN_CH_QUEUE $AML0_CH_ID $AML0_BREATHE_ID	\
						start_time	5						\
					}									\
					ml_ev_comm {								\
						code	$AML_GO_INFINITE $AML0_BREATHE_ID 0			\
						start_time	6						\
					}									\
				}										\
				ml_event {									\
					key		$VK_LBUTTON						\
					key		$VK_RBUTTON						\
					sequence								\
														\
					channel 	$AML0_CH_ID						\
					key_code	$AML0_CLOSE_WND_EVENT					\
					if_locked								\
					no_deactivate								\
														\
					active_time	3							\
														\
					ml_ev_comm {								\
						code	$AML_UNLOCK $AML0_WND_ID 0				\
						start_time	0						\
					}									\
					ml_ev_comm {								\
						code	$AML_PUT_IN_CH_QUEUE $AML0_CH_ID $AML0_WND_ID		\
						start_time	1						\
					}									\
					ml_ev_comm {								\
						code	$AML_START_SOUND $ACI_WND_SOUND 0			\
						start_time	2						\
					}									\
					ml_ev_comm {								\
						code	$AML_GO_PHASE $AML0_WND_ID $AML0_WND_END_ID		\
						start_time	3						\
					}									\
				}										\
				ml_event {									\
					startup 	$AML_SHUTDOWN						\
					startup_data	1							\
					sequence								\
														\
					channel 	$AML0_CH_ID						\
					if_locked								\
														\
					active_time	3							\
														\
					ml_ev_comm {								\
						code	$AML_UNLOCK $AML0_WND_ID 0				\
						start_time	0						\
					}									\
					ml_ev_comm {								\
						code	$AML_PUT_IN_CH_QUEUE $AML0_CH_ID $AML0_WND_ID		\
						start_time	1						\
					}									\
					ml_ev_comm {								\
						code	$AML_START_SOUND $ACI_WND_SOUND 0			\
						start_time	2						\
					}									\
					ml_ev_comm {								\
						code	$AML_GO_PHASE $AML0_WND_ID $AML0_WND_END_ID		\
						start_time	3						\
					}									\
				}

#define AML0_W1_SHUTDOWN_EV	ml_event {								\
					startup 	$AML_SHUTDOWN					\
					startup_data	$AML0_W1_TIME					\
													\
					sequence							\
													\
					channel 	$AML0_CHANNEL0					\
					priority	$AML0_W1_PRIORITY				\
													\
					active_time	2						\
													\
					ml_ev_comm {							\
						code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_W1	\
						start_time	0					\
					}								\
					ml_ev_comm {							\
						code	$AML_GO_PHASE $AML0_W1 $AML0_W1_END		\
						start_time	1					\
					}								\
					ml_ev_comm {							\
						code	$AML_LOCK $AML0_W1 0				\
						start_time	2					\
					}								\
				}

ml_data {
	id	$AML0_W1
	name	"resource/iscreen/ldata/l0/data.vot/w1.vot"

	channel $AML0_CHANNEL0

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML0_W1 $AML0_W1_END
		}
	}
	ml_event {
		startup 	$AML_STARTUP
		startup_data	1

		ml_ev_comm {
			code	$AML_KEY_TRAP $AML_KEY $AML0_W1_EVENT
		}
	}

	ml_event {
		key_code	$AML0_W1_RND_EVENT

		rnd		2
		startup_data	200

		channel $AML0_CHANNEL0

		if_not_locked
		sequence

		active_time	3

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_R1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L0_SWARM_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_R1 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_N0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML0_N0 0
			start_time	3
		}
	}

	#define AML0_W1_TIME	1
	#define AML0_W1_PRIORITY	-1
	$AML0_W1_SHUTDOWN_EV

	#define AML0_W1_TIME	10
	#define AML0_W1_PRIORITY	-2
	$AML0_W1_SHUTDOWN_EV

	#define AML0_W1_TIME	20
	#define AML0_W1_PRIORITY	-3
	$AML0_W1_SHUTDOWN_EV

	#define AML0_W1_TIME	30
	#define AML0_W1_PRIORITY	-4
	$AML0_W1_SHUTDOWN_EV

	#define AML0_W1_TIME	40
	#define AML0_W1_PRIORITY	-5
	$AML0_W1_SHUTDOWN_EV

	#define AML0_W1_TIME	50
	#define AML0_W1_PRIORITY	-6
	$AML0_W1_SHUTDOWN_EV

	#define AML0_W1_TIME	60
	#define AML0_W1_PRIORITY	-7
	$AML0_W1_SHUTDOWN_EV

	#define AML0_W1_TIME	70
	#define AML0_W1_PRIORITY	-8
	$AML0_W1_SHUTDOWN_EV

	#define AML0_W1_TIME	80
	#define AML0_W1_PRIORITY	-9
	$AML0_W1_SHUTDOWN_EV

	#define AML0_W1_TIME	90
	#define AML0_W1_PRIORITY	-10
	$AML0_W1_SHUTDOWN_EV

	#define AML0_W1_TIME	100
	#define AML0_W1_PRIORITY	-11
	$AML0_W1_SHUTDOWN_EV

	ml_event {
		key_code	$AML0_W1_EVENT
		sequence

		active_time	1

		ml_ev_comm {
			code	$AML_LOCK $AML0_W1 0
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

		active_time	4

		channel $AML0_CHANNEL0

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML0_W1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_W1
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_W1 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_N0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML0_N0 0
			start_time	4
		}
	}
}

ml_data {
	id	$AML0_W3
	name	"resource/iscreen/ldata/l0/data.vot/w3.vot"

	channel $AML0_CHANNEL1

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML0_W3 $AML0_W3_END
		}
	}

	ml_event {
		key_code	$AML0_W3_RND_EVENT

		rnd		3
		startup_data	100

		channel $AML0_CHANNEL1

		if_locked
		sequence

		active_time	3

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL1 $AML0_R2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L0_SWARM_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_R2 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL1 $AML0_N3
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML0_N3 0
			start_time	3
		}
	}

	#define AML0_CH_ID		$AML0_CHANNEL1
	#define AML0_WND_ID		$AML0_W3
	#define AML0_WND_END_ID 	$AML0_W3_END
	#define AML0_BREATHE_ID 	$AML0_N3
	$AML0_WND_EVENTS
}

ml_data {
	id	$AML0_W2
	name	"resource/iscreen/ldata/l0/data.vot/w2.vot"

	channel $AML0_CHANNEL2

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML0_W2 $AML0_W2_END
		}
	}

	ml_event {
		key_code	$AML0_W2_RND_EVENT

		rnd		3
		startup_data	100

		channel $AML0_CHANNEL2

		if_locked
		sequence

		active_time	3

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL2 $AML0_R3
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L0_SWARM_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_R3 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL2 $AML0_N4
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML0_N4 0
			start_time	3
		}
	}

	#define AML0_CH_ID		$AML0_CHANNEL2
	#define AML0_WND_ID		$AML0_W2
	#define AML0_WND_END_ID 	$AML0_W2_END
	#define AML0_BREATHE_ID 	$AML0_N4
	$AML0_WND_EVENTS
}

ml_data {
	id	$AML0_W4
	name	"resource/iscreen/ldata/l0/data.vot/w4.vot"

	channel $AML0_CHANNEL3

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML0_W4 $AML0_W4_END
		}
	}
	#define AML0_CH_ID		$AML0_CHANNEL3
	#define AML0_WND_ID		$AML0_W4
	#define AML0_WND_END_ID 	$AML0_W4_END
	#define AML0_BREATHE_ID 	$AML0_N1
	$AML0_WND_EVENTS
}

ml_data {
	id	$AML0_W6
	name	"resource/iscreen/ldata/l0/data.vot/w6.vot"

	channel $AML0_CHANNEL4

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML0_W6 $AML0_W6_END
		}
	}
	#define AML0_CH_ID		$AML0_CHANNEL4
	#define AML0_WND_ID		$AML0_W6
	#define AML0_WND_END_ID 	$AML0_W6_END
	#define AML0_BREATHE_ID 	$AML0_N5
	$AML0_WND_EVENTS
}

ml_data {
	id	$AML0_W5
	name	"resource/iscreen/ldata/l0/data.vot/w5.vot"

	channel $AML0_CHANNEL5

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML0_W5 $AML0_W5_END
		}
	}
	ml_event {
		key_code	$AML0_W5_RND_EVENT

		rnd		3
		startup_data	30

		channel $AML0_CHANNEL5

		if_locked
		sequence

		active_time	5

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL5 $AML0_R4
			start_time	0
		}
//		  ml_ev_comm {
//			  code	  $AML_START_SOUND $ACI_L0_SWARM_SOUND 0
//			  start_time	  1
//		  }
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL5 $AML0_R4
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL5 $AML0_R4
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_R4 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL5 $AML0_N2
			start_time	4
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML0_N2 0
			start_time	5
		}
	}

	#define AML0_CH_ID		$AML0_CHANNEL5
	#define AML0_WND_ID		$AML0_W5
	#define AML0_WND_END_ID 	$AML0_W5_END
	#define AML0_BREATHE_ID 	$AML0_N2
	$AML0_WND_EVENTS
}

ml_data {
	id	$AML0_N0
	name	"resource/iscreen/ldata/l0/data.vot/n0.vot"

	channel $AML0_CHANNEL0
}

ml_data {
	id	$AML0_N3
	name	"resource/iscreen/ldata/l0/data.vot/n3.vot"

	channel $AML0_CHANNEL1
}

ml_data {
	id	$AML0_N4
	name	"resource/iscreen/ldata/l0/data.vot/n4.vot"

	channel $AML0_CHANNEL2
}

ml_data {
	id	$AML0_N1
	name	"resource/iscreen/ldata/l0/data.vot/n1.vot"

	channel $AML0_CHANNEL3
}

ml_data {
	id	$AML0_N5
	name	"resource/iscreen/ldata/l0/data.vot/n5.vot"

	channel $AML0_CHANNEL4
}

ml_data {
	id	$AML0_N2
	name	"resource/iscreen/ldata/l0/data.vot/n2.vot"

	channel $AML0_CHANNEL5
}

ml_data {
	id	$AML0_R1
	name	"resource/iscreen/ldata/l0/data.vot/r1.vot"

	channel $AML0_CHANNEL0
}

ml_data {
	id	$AML0_R2
	name	"resource/iscreen/ldata/l0/data.vot/r2.vot"

	channel $AML0_CHANNEL1
}

ml_data {
	id	$AML0_R3
	name	"resource/iscreen/ldata/l0/data.vot/r3.vot"

	channel $AML0_CHANNEL2
}

ml_data {
	id	$AML0_R4
	name	"resource/iscreen/ldata/l0/data.vot/r4.vot"

	channel $AML0_CHANNEL5
}

ml_data {
	id	$AML0_C1
	name	"resource/iscreen/ldata/l0/data.vot/c1.vot"

	channel $AML0_CHANNEL0
}

ml_data {
	id	$AML0_C2
	name	"resource/iscreen/ldata/l0/data.vot/c2.vot"

	channel $AML0_CHANNEL0
}

ml_data {
	id	$AML0_C3
	name	"resource/iscreen/ldata/l0/data.vot/c3.vot"

	channel $AML0_CHANNEL0
}

ml_data {
	id	$AML0_C4
	name	"resource/iscreen/ldata/l0/data.vot/c4.vot"

	channel $AML0_CHANNEL0
}

ml_data {
	id	$AML0_S1
	name	"resource/iscreen/ldata/l0/data.vot/s1.vot"

	channel $AML0_CHANNEL0
}

ml_data {
	id	$AML0_S2
	name	"resource/iscreen/ldata/l0/data.vot/s2.vot"

	channel $AML0_CHANNEL0
}

ml_data {
	id	$AML0_S3
	name	"resource/iscreen/ldata/l0/data.vot/s3.vot"

	channel $AML0_CHANNEL0
}

ml_data {
	id	$AML0_S0
	name	"resource/iscreen/ldata/l0/data.vot/s0.vot"

	channel $AML0_CHANNEL0

	ml_event {
		key_code	$AML_SPEECH0_CH0
		key_code	$AML_SPEECH1_CH0

		sequence
		no_deactivate

		channel 	$AML0_CHANNEL0

		priority	1
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_C1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L0_OUTIN_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_C1 $AML0_C1_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML0_C1 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH2_CH0
		key_code	$AML_SPEECH3_CH0

		sequence
		no_deactivate

		channel 	$AML0_CHANNEL0

		priority	2
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_C3
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L0_ANGRY_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_C3 $AML0_C3_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML0_C3 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH0_EV
		sequence

		no_deactivate

		channel 	$AML0_CHANNEL0

		priority	5

		active_time	1
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_S0
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

		channel 	$AML0_CHANNEL0

		priority	5

		active_time	1
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_S1
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

		channel 	$AML0_CHANNEL0

		priority	5

		active_time	1
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_S2
			start_time	0
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

		channel 	$AML0_CHANNEL0

		priority	5

		active_time	4
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_S2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_S2 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_S3
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_S3 $AML0_S3_END
			start_time	3
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH3_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_EXIT 0 0
			start_time	4
		}
	}
	ml_event {
		key_code	$AML_SPEECH0_CH1
		key_code	$AML_SPEECH1_CH1

		sequence
		no_deactivate

		channel 	$AML0_CHANNEL0

		priority	1
		active_time	4

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_C2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L0_OUTIN_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_C2 $AML0_C2_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML0_C2 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_N0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML0_N0 0
			start_time	4
		}
	}
	ml_event {
		key_code	$AML_SPEECH2_CH1

		sequence
		no_deactivate

		channel 	$AML0_CHANNEL0

		priority	2
		active_time	4

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_C4
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_C4 $AML0_C4_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML0_C4 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_N0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML0_N0 0
			start_time	4
		}
	}
	ml_event {
		key_code	$AML_END_SPEECH_EV

		channel $AML0_CHANNEL0

		sequence
		active_time	2
		priority	3

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_W1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_W1 $AML0_W1_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_LOCK $AML0_W1 0
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
			code	$AML_KEY_TRAP $AML_KEY $AML0_CLOSE_WND_EVENT
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_BEG_SPEECH_EV

		channel $AML0_CHANNEL0

		sequence
		active_time	2
		priority	3

		ml_ev_comm {
			code	$AML_UNLOCK $AML0_W1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_BREATH_ON 0 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML0_CHANNEL0 $AML0_W1
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML0_W1 0
			start_time	2
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

speech_channel		$AML0_CHANNEL0

ml_item 	$ACI_LEEPURINGA       10     "resource/iscreen/ldata/l0/items/itm00.bmo"
ml_item 	$ACI_LEEPURINGA       10     "resource/iscreen/ldata/l0/items/itm01.bmo"
ml_item 	$ACI_LEEPURINGA       10     "resource/iscreen/ldata/l0/items/itm02.bmo"
ml_item 	$ACI_LEEPURINGA       10     "resource/iscreen/ldata/l0/items/itm03.bmo"
