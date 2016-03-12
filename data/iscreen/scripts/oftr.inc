/* iScript include file */

#define MOVE_TIME	401
#define MOVE_TIME1	200

#define VOLUME_CHANGE_TIME	5
#define CHANGE_OBJ_TIME 	7
#define PRESS_OBJ_TIME		7
#define SMOOTH_OBJ_TIME 	5
#define HIDE_OBJ_TIME		8
#define MOVE2SCREEN_TIME	10
#define TRIGGER_TIME		10

#define EV_CHANGE_OBJ	time { $CHANGE_OBJ_TIME + 1 }	\
			EvComm {			\
				jump_obj	$OBJ2	\
				start_time	$CHANGE_OBJ_TIME \
				time		1	\
			}				\
			EvComm {			\
				low_obj 	$OBJ1	\
				start_time	0	\
				time		$CHANGE_OBJ_TIME \
			}				\
			EvComm {			\
				rise_obj	$OBJ2	\
				start_time	0	\
				time		$CHANGE_OBJ_TIME \
			}

#define LOGO_EVENTS	Event { 					\
				key LEFT_MOUSE				\
				time 30 				\
				EvComm {				\
					scale_obj	$LOGO0	 128	\
					start_time	0		\
					time		10		\
				}					\
				EvComm {				\
					scale_obj	$LOGO0	255	\
					start_time	10		\
					time		10		\
				}					\
				EvComm {				\
					scale_obj	$LOGO1	 128	\
					start_time	10		\
					time		10		\
				}					\
				EvComm {				\
					scale_obj	$LOGO1	255	\
					start_time	20		\
					time		10		\
				}					\
			}						\
			Event { 					\
				key RIGHT_MOUSE 			\
				time 30 				\
				EvComm {				\
					scale_obj	$LOGO0	 128	\
					start_time	0		\
					time		10		\
				}					\
				EvComm {				\
					scale_obj	$LOGO0	255	\
					start_time	10		\
					time		10		\
				}					\
				EvComm {				\
					scale_obj	$LOGO1	 128	\
					start_time	10		\
					time		10		\
				}					\
				EvComm {				\
					scale_obj	$LOGO1	255	\
					start_time	20		\
					time		10		\
				}					\
				EvComm {				\
					smooth_obj	$LOGO0	 4	\
					start_time	0		\
					time		10		\
				}					\
				EvComm {				\
					smooth_obj	$LOGO0	 0	\
					start_time	10		\
					time		10		\
				}					\
				EvComm {				\
					smooth_obj	$LOGO1	 4	\
					start_time	10		\
					time		10		\
				}					\
				EvComm {				\
					smooth_obj	$LOGO1	 0	\
					start_time	20		\
					time		10		\
				}					\
			}



// Shop trigger states...
#define TRG_MECHOS	2
#define TRG_ITEMS	1
#define TRG_WEAPONS	0

/* EOF */
