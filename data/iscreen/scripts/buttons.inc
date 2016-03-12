
// Button parameters...
#define BUTTON_BMP_NULL 	200
#define BUTTON_NULL		200

// Button Events...
#define BUTTON_PRESS_START	0
#define BUTTON_UNPRESS_START	7
#define BUTTON_PRESS_TIME	7
#define BUTTON_UNPRESS_TIME	7
#define BUTTON_SCALE_STRENGTH	0

#define BUTTON_SMOOTH_INDEX	4

#define BUTTON_PRESS_EV 	EvComm {							\
					smooth_obj	$BUTTON_NAME	$BUTTON_SMOOTH_INDEX	\
					start_time	$BUTTON_PRESS_START			\
					time		$BUTTON_PRESS_TIME			\
				}								\
				EvComm {							\
					scale_obj	$BUTTON_NAME	$BUTTON_SCALE_STRENGTH	\
					start_time	$BUTTON_PRESS_START			\
					time		$BUTTON_PRESS_TIME			\
				}


#define BUTTON_UNPRESS_EV	EvComm {					\
					smooth_obj	$BUTTON_NAME	0	\
					start_time	$BUTTON_UNPRESS_START	\
					time		$BUTTON_UNPRESS_TIME	\
				}						\
				EvComm {					\
					scale_obj	$BUTTON_NAME	256	\
					start_time	$BUTTON_UNPRESS_START	\
					time		$BUTTON_UNPRESS_TIME	\
				}


#define AVI_HIDE_SPEED		5

