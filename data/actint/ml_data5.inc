
// IDs...
#define AML5_N0 	0
#define AML5_N1 	1
#define AML5_N2 	2
#define AML5_N3 	3

#define AML5_H1 	4
#define AML5_H2 	5
#define AML5_H3 	6

#define AML5_W1 	7
#define AML5_W2 	8
#define AML5_W3 	9

#define AML5_R1 	10
#define AML5_R2 	11
#define AML5_R3 	12
#define AML5_R4 	13

#define AML5_C0 	14
#define AML5_C1 	15
#define AML5_C2 	16
#define AML5_C3 	17

#define AML5_S0 	18
#define AML5_S2 	19
#define AML5_S3 	20

// Channels...
#define AML5_CHANNEL0	1
#define AML5_CHANNEL1	2
#define AML5_CHANNEL2	3
#define AML5_CHANNEL3	4
#define AML5_CHANNEL4	5

// Phases...
#define AML5_C0_END	3
#define AML5_C1_END	3
#define AML5_C2_END	10
#define AML5_C3_END	4
#define AML5_W1_END	7
#define AML5_W2_END	5
#define AML5_W3_END	5

#define AML5_H1_END	17
#define AML5_H2_END	7
#define AML5_H3_END	2

#define AML5_S3_END	15

// Events...
#define AML5_W1_EVENT		5000

#define AML5_W1_RND_EVENT	5001
#define AML5_W2_RND_EVENT0	5002
#define AML5_W3_RND_EVENT1	5004
#define AML5_W3_RND_EVENT2	5005
#define AML5_W3_RND_EVENT3	5006
#define AML5_W3_RND_EVENT4	5007

#define AML5_CLOSE_ELEECH	5008

#define AML5_FREEZE_EV_CODE	5009
#define AML5_UNFREEZE_EV_CODE	5010

#define AML5_CLOSE_WND_EVENT	5011

#define AML5_FREEZE_EV	ml_event {					\
				startup 	$AML_SHUTDOWN		\
				startup_data	1			\
									\
				active_time	1			\
									\
				ml_ev_comm {				\
					code	$AML_LOCK $AML5_ML_ID 0 \
				}					\
			}						\
			ml_event {					\
				key_code	$AML5_FREEZE_EV_CODE	\
				sequence				\
				ml_ev_comm {				\
					code	$AML_LOCK $AML5_ML_ID 0 \
				}					\
				ml_ev_comm {				\
					code	$AML_REWIND $AML5_ML_ID 0 \
				}					\
			}

#define AML5_UNFREEZE_EV	ml_event {						\
					sequence					\
					key_code	$AML5_UNFREEZE_EV_CODE		\
					ml_ev_comm {					\
						code	$AML_UNLOCK $AML5_ML_ID 0	\
					}						\
				}

#define AML5_WND_EVENTS 	ml_event {									\
					key		$VK_LBUTTON						\
					key		$VK_RBUTTON						\
					sequence								\
														\
					channel 	$AML5_CH_ID						\
					if_not_locked								\
					no_deactivate								\
														\
					priority	-1							\
														\
					active_time	5							\
														\
					ml_ev_comm {								\
						code	$AML_IS_LOCKED $AML5_W1 0				\
						start_time	0						\
					}									\
					ml_ev_comm {								\
						code	$AML_IS_ACTIVE $AML5_W1 0				\
						start_time	0						\
					}									\
					ml_ev_comm {								\
						code	$AML_PUT_IN_CH_QUEUE $AML5_CH_ID $AML5_WND_ID		\
						start_time	1						\
					}									\
					ml_ev_comm {								\
						code	$AML_START_SOUND $ACI_WND_SOUND 0			\
						start_time	2						\
					}									\
					ml_ev_comm {								\
						code	$AML_GO_PHASE $AML5_WND_ID 0				\
						start_time	2						\
					}									\
					ml_ev_comm {								\
						code	$AML_LOCK $AML5_WND_ID 0				\
						start_time	3						\
					}									\
					ml_ev_comm {								\
						code	$AML_PUT_IN_CH_QUEUE $AML5_CH_ID $AML5_BREATHE_ID	\
						start_time	4						\
					}									\
					ml_ev_comm {								\
						code	$AML_GO_INFINITE $AML5_BREATHE_ID 0			\
						start_time	5						\
					}									\
				}										\
				ml_event {									\
					key		$VK_LBUTTON						\
					key		$VK_RBUTTON						\
					key_code	$AML5_CLOSE_WND_EVENT					\
					sequence								\
														\
					channel 	$AML5_CH_ID						\
					if_locked								\
					no_deactivate								\
														\
					active_time	3							\
														\
					ml_ev_comm {								\
						code	$AML_UNLOCK $AML5_WND_ID 0				\
						start_time	0						\
					}									\
					ml_ev_comm {								\
						code	$AML_PUT_IN_CH_QUEUE $AML5_CH_ID $AML5_WND_ID		\
						start_time	1						\
					}									\
					ml_ev_comm {								\
						code	$AML_START_SOUND $ACI_WND_SOUND 0			\
						start_time	2						\
					}									\
					ml_ev_comm {								\
						code	$AML_GO_PHASE $AML5_WND_ID $AML5_WND_END_ID		\
						start_time	3						\
					}									\
				}										\
				ml_event {									\
					startup 	$AML_SHUTDOWN						\
					startup_data	2							\
					sequence								\
														\
					channel 	$AML5_CH_ID						\
					if_locked								\
														\
					active_time	3							\
														\
					ml_ev_comm {								\
						code	$AML_UNLOCK $AML5_WND_ID 0				\
						start_time	0						\
					}									\
					ml_ev_comm {								\
						code	$AML_PUT_IN_CH_QUEUE $AML5_CH_ID $AML5_WND_ID		\
						start_time	1						\
					}									\
					ml_ev_comm {								\
						code	$AML_START_SOUND $ACI_WND_SOUND 0			\
						start_time	2						\
					}									\
					ml_ev_comm {								\
						code	$AML_GO_PHASE $AML5_WND_ID $AML5_WND_END_ID		\
						start_time	3						\
					}									\
				}

ml_data {
	id	$AML5_N0
	name	"resource/iscreen/ldata/l5/data.vot/n0.vot"

	channel $AML5_CHANNEL1

	#define AML5_ML_ID	$AML5_N0
	$AML5_FREEZE_EV
	$AML5_UNFREEZE_EV
}

ml_data {
	id	$AML5_N1
	name	"resource/iscreen/ldata/l5/data.vot/n1.vot"

	channel $AML5_CHANNEL4

	#define AML5_ML_ID     $AML5_N1
	$AML5_FREEZE_EV
}

ml_data {
	id	$AML5_N2
	name	"resource/iscreen/ldata/l5/data.vot/n2.vot"

	channel $AML5_CHANNEL3
}

ml_data {
	id	$AML5_N3
	name	"resource/iscreen/ldata/l5/data.vot/n3.vot"

	channel $AML5_CHANNEL2
}

ml_data {
	id	$AML5_C0
	name	"resource/iscreen/ldata/l5/data.vot/c0.vot"

	channel $AML5_CHANNEL1

	#define AML5_ML_ID	$AML5_C0
	$AML5_FREEZE_EV
	$AML5_UNFREEZE_EV
}

ml_data {
	id	$AML5_C1
	name	"resource/iscreen/ldata/l5/data.vot/c1.vot"

	channel $AML5_CHANNEL1

	#define AML5_ML_ID	$AML5_C1
	$AML5_FREEZE_EV
	$AML5_UNFREEZE_EV
}

ml_data {
	id	$AML5_C2
	name	"resource/iscreen/ldata/l5/data.vot/c2.vot"

	channel $AML5_CHANNEL1

	#define AML5_ML_ID	$AML5_C2
	$AML5_FREEZE_EV
	$AML5_UNFREEZE_EV
}

ml_data {
	id	$AML5_C3
	name	"resource/iscreen/ldata/l5/data.vot/c3.vot"

	channel $AML5_CHANNEL3

	#define AML5_ML_ID	$AML5_C3
	$AML5_FREEZE_EV
	$AML5_UNFREEZE_EV
}

ml_data {
	id	$AML5_R1
	name	"resource/iscreen/ldata/l5/data.vot/r1.vot"

	channel $AML5_CHANNEL2
}

ml_data {
	id	$AML5_R2
	name	"resource/iscreen/ldata/l5/data.vot/r2.vot"

	channel $AML5_CHANNEL3
}

ml_data {
	id	$AML5_R3
	name	"resource/iscreen/ldata/l5/data.vot/r3.vot"

	channel $AML5_CHANNEL1
}

ml_data {
	id	$AML5_R4
	name	"resource/iscreen/ldata/l5/data.vot/r4.vot"

	channel $AML5_CHANNEL2
}

ml_data {
	id	$AML5_H1
	name	"resource/iscreen/ldata/l5/data.vot/h1.vot"

	channel $AML5_CHANNEL2

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_LOCK $AML5_H1 0
		}
	}
	ml_event {
		key_code	$AML_ELEECH_CODE
//		  key		  $VK_A

		no_deactivate

		channel $AML5_CHANNEL2

		sequence
		active_time	5
		priority	3

		ml_ev_comm {
			code	$AML_UNLOCK $AML5_W2 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL2 $AML5_W2
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_W2 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_LOCK $AML5_W2 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML5_H1 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL2 $AML5_H1
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_H1 $AML5_H1_END
			start_time	4
		}
		ml_ev_comm {
			code	$AML_REWIND $AML5_H1 0
			start_time	5
		}
	}
	ml_event {
		key_code	$AML_KILL_ELEECH_CODE

		no_deactivate
		if_not_locked

		channel $AML5_CHANNEL2

		sequence
		active_time	6
		priority	3

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML5_H1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL2 $AML5_H2
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_H2 $AML5_H2_END
			start_time	2
		}
		ml_ev_comm {
			code	$AML_REWIND $AML5_H2 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_LOCK $AML5_H1 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML5_W2 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_W2 $AML5_W2_END
			start_time	5
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	5
		}
	}
	ml_event {
		startup 	$AML_SHUTDOWN
		startup_data	1

		no_deactivate
		if_not_locked

		channel $AML5_CHANNEL2

		sequence
		active_time	6
		priority	3

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML5_H1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL2 $AML5_H2
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_H2 $AML5_H2_END
			start_time	2
		}
		ml_ev_comm {
			code	$AML_REWIND $AML5_H2 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_LOCK $AML5_H1 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML5_W2 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_W2 $AML5_W2_END
			start_time	5
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	5
		}
	}
	ml_event {
		key	$VK_LBUTTON

		no_deactivate
		if_not_locked

		channel $AML5_CHANNEL2

		sequence
		active_time	8
		priority	2

		ml_ev_comm {
			code	$AML_IS_LOCKED $AML5_H1 0
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
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL2 $AML5_H3
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_H3 $AML5_H3_END
			start_time	2
		}
		ml_ev_comm {
			code	$AML_REWIND $AML5_H3 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_LOCK $AML5_H1 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML5_W2 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_W2 $AML5_W2_END
			start_time	5
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
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
		key_code	$AML5_CLOSE_ELEECH
		startup_data	200

		no_deactivate
		if_not_locked

		channel $AML5_CHANNEL2

		rnd	1

		sequence
		active_time	5
		priority	1

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL2 $AML5_H2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_H2 $AML5_H2_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML5_H2 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_LOCK $AML5_H1 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML5_W2 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_W2 $AML5_W2_END
			start_time	5
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	5
		}
	}
}

ml_data {
	id	$AML5_H2
	name	"resource/iscreen/ldata/l5/data.vot/h2.vot"

	channel $AML5_CHANNEL2
}

ml_data {
	id	$AML5_H3
	name	"resource/iscreen/ldata/l5/data.vot/h3.vot"

	channel $AML5_CHANNEL2
}

ml_data {
	id	$AML5_W2
	name	"resource/iscreen/ldata/l5/data.vot/w2.vot"

	channel $AML5_CHANNEL2

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML5_W2 $AML5_W2_END
		}
	}
	#define AML5_CH_ID		$AML5_CHANNEL2
	#define AML5_WND_ID		$AML5_W2
	#define AML5_WND_END_ID 	$AML5_W2_END
	#define AML5_BREATHE_ID 	$AML5_N3
	$AML5_WND_EVENTS

	ml_event {
		key_code	$AML5_W2_RND_EVENT0

		rnd		3
		startup_data	50

		channel $AML5_CHANNEL2

		if_locked
		sequence

		active_time	4

		ml_ev_comm {
			code	$AML_IS_NOT_LOCKED $AML5_H1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL2 $AML5_R1
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L5_SCRATCH_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_R1 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL2 $AML5_N3
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML5_N3 0
			start_time	4
		}
	}
}

ml_data {
	id	$AML5_W3
	name	"resource/iscreen/ldata/l5/data.vot/w3.vot"

	channel $AML5_CHANNEL3

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML5_W3 $AML5_W3_END
		}
	}
	#define AML5_CH_ID		$AML5_CHANNEL3
	#define AML5_WND_ID		$AML5_W3
	#define AML5_WND_END_ID 	$AML5_W3_END
	#define AML5_BREATHE_ID 	$AML5_N2
	$AML5_WND_EVENTS

	ml_event {
		key_code	$AML5_W3_RND_EVENT1

		rnd		3
		startup_data	150

		channel $AML5_CHANNEL3

		if_locked
		sequence

		active_time	3

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_R2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L5_CLAP_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_R2 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_N2
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML5_N2 0
			start_time	3
		}
	}
	ml_event {
		key_code	$AML5_W3_RND_EVENT2

		rnd		3
		startup_data	150

		channel $AML5_CHANNEL3

		if_locked
		sequence

		active_time	4

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_R2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L5_CLAP_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_R2
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_R2 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L5_CLAP_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_N2
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML5_N2 0
			start_time	4
		}
	}
	ml_event {
		key_code	$AML5_W3_RND_EVENT3

		rnd		3
		startup_data	150

		channel $AML5_CHANNEL3

		if_locked
		sequence

		active_time	5

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_R2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L5_CLAP_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_R2
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_R2
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L5_CLAP_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_R2 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_N2
			start_time	4
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML5_N2 0
			start_time	5
		}
	}
	ml_event {
		key_code	$AML5_W3_RND_EVENT4

		rnd		3
		startup_data	150

		channel $AML5_CHANNEL3

		if_locked
		sequence

		active_time	6

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_R2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L5_CLAP_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_R2
			start_time	1
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_R2
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L5_CLAP_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_R2
			start_time	3
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L5_CLAP_SOUND 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_R2 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL3 $AML5_N2
			start_time	5
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML5_N2 0
			start_time	6
		}
	}

}

ml_data {
	id	$AML5_W1
	name	"resource/iscreen/ldata/l5/data.vot/w1.vot"

	channel $AML5_CHANNEL1

	ml_event {
		startup 	$AML_STARTUP
		startup_data	0

		ml_ev_comm {
			code	$AML_SET_PHASE $AML5_W1 $AML5_W1_END
		}
	}
	ml_event {
		startup 	$AML_STARTUP
		startup_data	1

		ml_ev_comm {
			code	$AML_KEY_TRAP $AML_KEY $AML5_W1_EVENT
		}
	}
	ml_event {
		key_code	$AML5_W1_EVENT
		sequence

		active_time	1

		channel $AML5_CHANNEL1

		ml_ev_comm {
			code	$AML_LOCK $AML5_W1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_SEND2ACTINT $EV_NEXT_PHRASE 0
			start_time	1
		}
	}
	ml_event {
		key_code	$AML_BEG_SPEECH_EV
		sequence

		active_time	4
		priority	3

		channel $AML5_CHANNEL1

		ml_ev_comm {
			code	$AML_UNLOCK $AML5_W1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_KEY_TRAP $AML_KEY $AML5_UNFREEZE_EV_CODE
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL1 $AML5_W1
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_W1 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_BREATH_ON 0 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_UNLOCK $AML5_N1 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML5_N1 0
			start_time	4
		}
	}
	ml_event {
		key_code	$AML_END_SPEECH_EV
		sequence

		active_time	4
		priority	3

		channel $AML5_CHANNEL1

		ml_ev_comm {
			code	$AML_KEY_TRAP $AML_KEY $AML5_FREEZE_EV_CODE
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL1 $AML5_W1
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_W1 $AML5_W1_END
			start_time	3
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_WND_SOUND 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_CLOSE_SPEECH 0 0
			start_time	3
		}
		ml_ev_comm {
			code	$AML_KEY_TRAP $AML_KEY $AML5_CLOSE_WND_EVENT
			start_time	3
		}
		ml_ev_comm {
			code	$AML_LOCK $AML5_W1 0
			start_time	4
		}
		ml_ev_comm {
			code	$AML_BREATH_OFF 0 0
			start_time	4
		}
	}
}

ml_data {
	id	$AML5_S3
	name	"resource/iscreen/ldata/l5/data.vot/s3.vot"

	channel $AML5_CHANNEL0
}

ml_data {
	id	$AML5_S2
	name	"resource/iscreen/ldata/l5/data.vot/s2.vot"

	channel $AML5_CHANNEL1

	#define AML5_ML_ID	$AML5_S2
	$AML5_FREEZE_EV
	$AML5_UNFREEZE_EV
}

ml_data {
	id	$AML5_S0
	name	"resource/iscreen/ldata/l5/data.vot/s0.vot"

	channel $AML5_CHANNEL0

	ml_event {
		key_code	$AML_SPEECH0_CH0
		key_code	$AML_SPEECH1_CH0

		sequence
		no_deactivate

		channel 	$AML5_CHANNEL1

		priority	1
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL1 $AML5_C1
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_C1 $AML5_C1_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML5_C1 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH2_CH0

		sequence
		no_deactivate

		channel 	$AML5_CHANNEL1

		priority	2
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL1 $AML5_C2
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_C2 $AML5_C2_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L5_ANGRY_SOUND 0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML5_C2 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH2_CH1

		sequence
		no_deactivate

		channel 	$AML5_CHANNEL1

		priority	2
		active_time	2

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL1 $AML5_C3
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_C3 $AML5_C3_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML5_C3 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH3_EV

		sequence
		no_deactivate

		channel 	$AML5_CHANNEL1

		priority	5
		active_time	3

		ml_ev_comm {
			code	$AML_LOCK $AML5_N0 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_LOCK $AML5_N1 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL0 $AML5_S3
			start_time	1
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_L5_GETOUT_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_S3 $AML5_S3_END
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH3_SOUND 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_EXIT 0 0
			start_time	3
		}
	}
	ml_event {
		key_code	$AML_SPEECH0_EV

		sequence
		no_deactivate

		channel 	$AML5_CHANNEL1

		priority	5
		active_time	2

		ml_ev_comm {
			code	$AML_UNLOCK $AML5_S0 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL0 $AML5_S0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_S0 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH0_SOUND 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH1_EV

		sequence
		no_deactivate

		channel 	$AML5_CHANNEL1

		priority	5
		active_time	2

		ml_ev_comm {
			code	$AML_UNLOCK $AML5_S0 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL0 $AML5_S0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_S0 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH1_SOUND 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH2_EV

		sequence
		no_deactivate

		channel 	$AML5_CHANNEL1

		priority	5
		active_time	2

		ml_ev_comm {
			code	$AML_UNLOCK $AML5_S0 0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL0 $AML5_S0
			start_time	1
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_S0 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_START_SOUND $ACI_SPEECH2_SOUND 0
			start_time	2
		}
	}
	ml_event {
		key_code	$AML_SPEECH0_CH1
		key_code	$AML_SPEECH1_CH1

		sequence
		no_deactivate

		channel 	$AML5_CHANNEL1

		priority	1
		active_time	3

		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL1 $AML5_C0
			start_time	0
		}
		ml_ev_comm {
			code	$AML_GO_PHASE $AML5_C0 $AML5_C0_END
			start_time	1
		}
		ml_ev_comm {
			code	$AML_REWIND $AML5_C0 0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_PUT_IN_CH_QUEUE $AML5_CHANNEL1 $AML5_N0
			start_time	2
		}
		ml_ev_comm {
			code	$AML_GO_INFINITE $AML5_N0 0
			start_time	3
		}
	}

	#define AML5_ML_ID	$AML5_S0
	$AML5_FREEZE_EV
	$AML5_UNFREEZE_EV
}

speech_channel		$AML5_CHANNEL1

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

ml_item 	$ACI_PEREPONKA	     1	   "resource/iscreen/ldata/l5/items/itm00.bmo"
ml_item 	$ACI_PEREPONKA	     1	   "resource/iscreen/ldata/l5/items/itm01.bmo"
ml_item 	$ACI_PEREPONKA	     1	   "resource/iscreen/ldata/l5/items/itm02.bmo"
ml_item 	$ACI_PEREPONKA	     1	   "resource/iscreen/ldata/l5/items/itm03.bmo"
ml_item 	$ACI_PEREPONKA	     1	   "resource/iscreen/ldata/l5/items/itm04.bmo"
ml_item 	$ACI_PEREPONKA	     1	   "resource/iscreen/ldata/l5/items/itm05.bmo"
ml_item 	$ACI_PEREPONKA	     1	   "resource/iscreen/ldata/l5/items/itm06.bmo"

