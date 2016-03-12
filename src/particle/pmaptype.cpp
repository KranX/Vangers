#include "partmap.h"

ParticleMapProcessType ParticleMapProcessTypeArray[PROCESS_TYPE_TOTAL] =
/*MSSfile,
ProcedureType,
Xmax,Ymax,
HotSpotArea,
NhotSpots,
AttackTime,
LifeTime,
FadeTime,
FadePower,
DevRad,
DevPer,
HotCentersRad*/
{
	{
		"resource/mss/064x064.mss",0,64,64,60,223,8,0,20,20,2,0,0
	},{
		"resource/mss/064x064.mss",1,64,64,60,123,8,0,10,6,1,4,0
	},{
		"resource/mss/64x64st.mss",0,64,64,20,123,8,30,20,20,2,0,0
	},{
//		  "resource\\mss\\128x128.mss",2,128,128,60,223,20,0,10,6,10,10
		  "resource/mss/128x128.mss",0,128,128,10,123,5,8,10,10,2,0,10
//		"resource\\mss\\128x128.mss",0,128,128,10,123,10,10,10,10,2,0,10
	},{
//		  "16x16.mss",0,16,16,1,6,7,0,3,30,2,0,0
//		  "016x016.mss",1,16,16,5,123,8,0,10,6,1,4,0
		"resource/mss/016x016.mss",0,16,16,18,200,2,0,20,20,1,0,0
	}
};
