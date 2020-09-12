#ifndef __ISCREEN_OPTIONS_H__
#define __ISCREEN_OPTIONS_H__

int iGetOptionValue(int id);
void iSetOptionValue(int id,int val);
char* iGetOptionValueCHR(int id);
void iSetOptionValueCHR(int id, const char* p);
void iUpdateOptionValue(int id);

enum iScreenOptionID
{
	iSOUND_ON,			// 0
	iSOUND_VOLUME_CUR,		// 1
	iSOUND_VOLUME_MAX,		// 2

	iMUSIC_ON,			// 3
	iMUSIC_VOLUME_CUR,		// 4
	iMUSIC_VOLUME_MAX,		// 5

	iTUTORIAL_ON,			// 6
	iDETAIL_SETTING,		// 7

	iPLAYER_COLOR3, 		// 8
	iPLAYER_COLOR,			// 9

	iSERVER_NAME,			// 10
	iSERVER_NAME2,			// 11
	iPLAYER_PASSWORD,		// 12

	iMPGAME0_ID,			// 13
	iMPGAME1_ID,			// 14
	iMPGAME2_ID,			// 15
	iMPGAME3_ID,			// 16
	iMPGAME4_ID,			// 17

	iCUR_MPGAME_ID, 		// 18
	iCUR_MPGAME_ID2,		// 19

	iPLAYER_NAME2,			// 20
	iSCREEN_RESOLUTION,		// 21
	iHOST_NAME,			// 22

	iKEEP_IN_USE,			// 23
	iKEEP_CLEAN_UP, 		// 24

	iKEEP_MODE,			// 25
	iPANNING_ON,			// 26
	iDESTR_MODE,			// 27

	iPLAYER_COLOR2, 		// 28
	iPLAYER_NAME3,			// 29

	iMECH_SOUND,			// 30
	iBACK_SOUND,			// 31

	iJOYSTICK_TYPE, 		// 32

	iPROXY_USAGE,			// 33
	iPROXY_SERVER,			// 34
	iPROXY_PORT,			// 35

	iPROXY_SERVER_STR,		// 36
	iPROXY_PORT_STR,		// 37

	iSERVER_PORT,			// 38

	iPLAYER_NAME_CR,		// 39
	iPLAYER_PASSWORD_CR,		// 40

	iIP_ADDRESS,			// 41
	iCAMERA_TURN,			// 42
	iCAMERA_SLOPE,			// 43
	iCAMERA_SCALE,			// 44
	
	iFULLSCREEN,			// 45

	iAUTO_ACCELERATION,			// 46

	iMAX_OPTION_ID
};

enum iScreenOptionObjType
{
	iSCROLLER,
	iTRIGGER,
	iSTRING,
	iS_STRING,

	iMAX_OPTION_OBJ_TYPE
};

#endif