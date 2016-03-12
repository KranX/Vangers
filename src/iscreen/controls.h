
#define iKEY_OBJECT_SIZE	2

enum iControlID {
	iKEY_TURN_WHEELS_LEFT	= 1,
	iKEY_TURN_WHEELS_RIGHT, 	// 2
	iKEY_MOVE_FORWARD,		// 3
	iKEY_MOVE_BACKWARD,		// 4
	iKEY_TURN_OVER_LEFT,		// 5
	iKEY_TURN_OVER_RIGHT,		// 6
	iKEY_DEVICE_ON, 		// 7
	iKEY_DEVICE_OFF,		// 8
	iKEY_ACTIVATE_KID,		// 9
	iKEY_ACCELERATION,		// 10
	iKEY_VERTICAL_THRUST,		// 11
	iKEY_INVENTORY, 		// 12
	iKEY_OPEN,			// 13
	iKEY_FIRE_WEAPON1,		// 14
	iKEY_FIRE_WEAPON2,		// 15
	iKEY_FIRE_WEAPON3,		// 16
	iKEY_FIRE_WEAPON4,		// 17
	iKEY_FIRE_ALL_WEAPONS,		// 18
	iKEY_CHANGE_TARGET,		// 19

	iKEY_FULLSCREEN,		// 20
	iKEY_REDUCE_VIEW,		// 21
	iKEY_ENLARGE_VIEW,		// 22
	iKEY_ROTATION,			// 23
	iKEY_AUTO_ZOOM, 		// 24
	iKEY_AUTO_TILT, 		// 25
	iKEY_ZOOM_IN,			// 26
	iKEY_ZOOM_OUT,			// 27
	iKEY_ZOOM_STANDART,		// 28

	iKEY_HANDBRAKE, 		// 29

	iKEY_USE_GLUEK, 		// 30
	iKEY_JOYSTICK_SWITCH,		// 31
	iKEY_SKIP_TEXT, 		// 32

	iKEY_USE_VECTOR,		// 33
	iKEY_FRAG_INFO, 		// 34
	iKEY_CHAT,			// 35

	iKEY_EXIT,			// 36

	iKEY_MAX_ID
};

int iGetKeyID(int key);
int iKeyPressed(int id);
int iCheckKeyID(int id,int key);

void iInitControls(void);
void iResetControls(void);
void iInitControlObjects(void);

void iSetControlCode(int id,int key,int num = 0);
void iResetControlCode(int id,int num = 0);
int iGetControlCode(int id,int num = 0);

char* iGetJoystickButtonName(int vkey);
char* iGetMouseButtonName(int vkey);
