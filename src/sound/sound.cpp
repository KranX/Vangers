#include "../global.h"
#include "../lang.h"

#include "hmusic.h"
#include "hsound.h"

#define EFFECT_KHZ	44100
//#define EFFECT_KHZ	22050
#define SPEECH_KHZ	22050
#define DELTA_KHZ	2000

#define DISTANCE_TO_TOWN 2000
#define DISTANCE_TO_SICRET 512

#define DB_MIN		-10000
#define DB_MAX		0
#define DB_SIZE 	10000

/* ---------------------------- EXTERN SECTION ----------------------------- */
extern int MuteLog;
extern int ViewZ;
extern int frame;
extern int CurrentWorld;
/* --------------------------- PROTOTYPE SECTION --------------------------- */
int GetEscaveDist(void);
int getWID(void);
/* --------------------------- DEFINITION SECTION -------------------------- */
const int LOCAL_CHANNEL = 9;
const int LOCAL_SOUND = 1;
const int ESCAVE_SOUND = 2;

int SoundFlag = 0;
int lastSoundFlag = 0;
int SoundVolumeCD = -1;
int SoundVolumePanning = 1;
int MusicON = 1;
static int LastTrack = 2;

struct SndParameters {
	int channel;
	int priority;
	int cropos;
	int flags;
	int status;
	const char* fname;
	void* lpDSB;
	};

static int *TrackCDTime = 0;

static const char* SndMotorFileName[7*2] = {
	"raffa",  "raffa2",
	"light",   "light2", 
	"microbus", "microbus2",
	"atw", "atw2",
	"truck",  "truck2",
	"special",  "special2",
	"copter",  "copter2"
};

static SndParameters SndData[EFF_MAX] = {
	{ 0,1,255,0,LOCAL_SOUND,"",NULL },
	{ 0,1,255,0, LOCAL_SOUND,"",NULL },
	{ 0,1,0,0,LOCAL_SOUND,"",NULL },
	{  5,1,255,0,0,"underwater",NULL },
	{  4,2,255,0,0,"copterig",NULL },
	{  4,2,255,0,0,"cutterig",NULL },
	{  4,2,255,0,0,"crotrig",NULL },
	{  1,1,255,0,0,"explode",NULL },
	{  1,1,128,DS_QUEUE,0,"explode2",NULL },
	{  1,3,255,0,0,"crustest",NULL },
	{  1,2,0,0,0,"collis",NULL },
	{  2,1,20,0,0,"shot0",NULL },
	{  2,1,20,0,0,"shot1",NULL },
	{  3,1,20,0,0,"shot2",NULL },
	{ 14,2,20,0,0,"shot3",NULL },
	{ 14,1,20,0,0,"shot4",NULL },
	{ 6,1,20,0,0,"shot5",NULL },
	{ 3,2,20,0,0,"shot6",NULL },
	{  9,1,0,0,0,"select0",NULL },
	{  9,1,0,0,0,"select1",NULL },
	{  9,1,0,0,0,"select2",NULL },
	{  6,2,255,0,LOCAL_SOUND,"event0",NULL },
	{  6,2,255,0,LOCAL_SOUND,"event1",NULL },
	{  6,2,255,0,LOCAL_SOUND,"event2",NULL },
	{  6,2,255,0,LOCAL_SOUND,"event3",NULL },
	{  6,2,255,0,LOCAL_SOUND,"event4",NULL },
	{  6,2,255,0,LOCAL_SOUND,"event5",NULL },
	{  8,2,255,0,LOCAL_SOUND,"door",NULL },
	{  8,1,255,0,0,"passage",NULL },
	{  9,1,0,0,0,"pick",NULL },
	{  9,1,0,0,0,"drop",NULL },
	{ 10,2,0,0,0,"crack",NULL },
	{ 11,1,255,0,LOCAL_SOUND,"global",NULL },
	{ 12,1,128,0,LOCAL_SOUND,"mushroom",NULL },
	{ 13,1,0,0,0,"item",NULL },
	{ 1,3,0,0,0,"hit",NULL },
	{ 15,3,0,0,0,"kidpull",NULL },
	{ 15,2,255,0,0,"kidpush",NULL },
	{ 15,1,255,0,0,"ghorb",NULL },
	{ 15,1,255,0,0,"machotine",NULL },
	{ 15,1,255,0,0,"repair",NULL },
	{ 15,1,255,0,0,"shield",NULL },
	{ 15,1,255,0,0,"spiral",NULL },
	{ 15,1,255,0,0,"rigs",NULL },
	{ 15,1,0,0,LOCAL_SOUND,"crash",NULL },
	{ 13,2,255,0,0,"horde",NULL },

	{ 10,0,255,DS_QUEUE,0,"cycle",NULL },
	{ 8,1,255,0,0,"failedpass",NULL },
	{ 1,4,255,0,0,"shock0",NULL },
	{ 1,4,255,0,0,"shock1",NULL },
	{ 1,4,255,0,0,"shock2",NULL },
	{ 1,4,255,0,0,"shock3",NULL },
	{ 1,4,255,0,0,"shock4",NULL },
	{ 1,4,255,0,0,"shock5",NULL },
	{ 10,1,255,DS_QUEUE,0,"success",NULL },
	{ 10,1,255,DS_QUEUE,0,"failed",NULL },

	{ 12,1,64,0,LOCAL_SOUND,"thunder",NULL },
	{ 10,1,255,0,LOCAL_SOUND,"toggle",NULL },
	{ 7,1,128,0,LOCAL_SOUND,"bolt",NULL },
	{ 7,2,255,0,LOCAL_SOUND,"acid",NULL },
	{ 10,1,255,0,LOCAL_SOUND,"checkpoint",NULL },
	{ 10,2,255,0,LOCAL_SOUND,"swamp",NULL },
	{  7,2,255,0,LOCAL_SOUND,"tear",NULL },
	{  10,1,255,0,LOCAL_SOUND,"hideout",NULL },
	{  7,3,255,0,LOCAL_SOUND,"grow",NULL },

	{ 0,1,255,0,ESCAVE_SOUND,"noise",NULL },
	{ 2,1,0,0,ESCAVE_SOUND,"talk0",NULL },
	{ 3,1,0,0,ESCAVE_SOUND,"talk01",NULL },
	{ 4,1,0,0,ESCAVE_SOUND,"talk02",NULL },
	{ 2,1,0,0,ESCAVE_SOUND,"talk1",NULL },
	{ 3,1,0,0,ESCAVE_SOUND,"talk11",NULL },
	{ 4,1,0,0,ESCAVE_SOUND,"talk12",NULL },
	{ 2,1,0,0,ESCAVE_SOUND,"talk2",NULL },
	{ 3,1,0,0,ESCAVE_SOUND,"talk21",NULL },
	{ 4,1,0,0,ESCAVE_SOUND,"talk22",NULL },
	{ 11,1,255,0,ESCAVE_SOUND,"window",NULL },

	{ 5,1,255,0,ESCAVE_SOUND,"outin",NULL },
	{ 6,1,255,0,ESCAVE_SOUND,"swarm",NULL },
	{ 13,1,255,0,ESCAVE_SOUND,"angry",NULL },
	{ 14,1,255,0,ESCAVE_SOUND,"getout",NULL },
	{ 1,1,255,0,ESCAVE_SOUND,"gate",NULL },
	{ 6,1,255,0,ESCAVE_SOUND,"step0",NULL },
	{ 7,1,255,0,ESCAVE_SOUND,"step1",NULL },
	{ 8,1,255,0,ESCAVE_SOUND,"step2",NULL },
	{ 12,1,255,0,ESCAVE_SOUND,"hand",NULL },
	{ 6,1,255,0,ESCAVE_SOUND,"scratch0",NULL },
	{ 7,1,255,0,ESCAVE_SOUND,"scratch1",NULL },
	{ 8,1,255,0,ESCAVE_SOUND,"clap",NULL },

	{ 1,2,128,0,0,"messiah",NULL },
	{ 1,2,255,0,0,"sign",NULL },
	{ 1,2,255,0,0,"boot",NULL },
	{ 1,1,255,0,0,"larva",NULL },
	{ 10,0,255,0,0,"endcycle",NULL },
	{ 11,1,255,0,0,"revive",NULL },
	{ 11,1,255,0,0,"revive2",NULL },
	{ 1,3,255,0,0,"thru",NULL },
	{ 15,1,255,0,0,"discharge",NULL },
	{ 1,2,255,0,0,"function",NULL },
	{ 1,2,255,0,0,"resize",NULL },
	{ 1,2,255,0,0,"barrier",NULL },
	{ 1,2,255,0,0,"cloak",NULL },
//	{ 1,2,255,0,0,"luck",NULL },
	{ 1,2,255,0,0,"beebsospy",NULL },
	{ 1,2,0,0,0,"fire",NULL },
	{8,1,255,0,LOCAL_SOUND,"spobs",NULL},
	{9,1,0,0,0,"beep",NULL},
	{1,4,255,0,0,"damage",NULL},
	{2,1,20,0,0,"shot",NULL}
};

static int SpeechForce[2*SPEECH_MAX] = {
	2,0	  //	  SPEECH_MUFLER,			  //	  0
	};

void LoadMotorSound(void);

static int soundVolume,soundVolumecurrent;
int EffectInUse,EffectInUsePriory;
static int preViewZ;
int EffectInFrequence = 1;
int NumBackgroundsEff;
int sndBackgroundMax,sndStepMax,sndScratchMax;
int EngineNoise = 1,BackgroundSound = 1;

int activeWTRACK = 0;
static long TimeCD = 0;
static time_t lastTimeCD = 0;

void InstallSOUND(void)
{
    if (lang() == RUSSIAN) {
        TrackCDTime = new int[11] { 0, 1, 174, 108, 128, 238, 130, 181, 120, 151, 200 };
    } else {
        TrackCDTime = new int[11] { 0, 1, 174, 108, 128, 238, 130, 181, 120, 151, 60 };
    }

	if(!SoundInit(EFFECT_KHZ, 16)){
		std::cout<<"\nInitialization of sound failed, mute mode accepted...\n";
		MuteLog = 1;
		return;
		}
	EffectInUsePriory = EffectInUse = 1;

//	xsInitCD();
	xsInitMusic();
}

void RestoreSOUND(void)
{
    delete[] TrackCDTime;
#ifndef _NO_CDAUDIO_
#ifndef _DEMO_
	if(!MusicON) return;

//	xsStopCD();
	xsStopMusic();
	xsDeInitMusic();

#endif
#endif
}

void SetMotorFile( int type ){
	SndData[EFF_DRIVING].fname = SndMotorFileName[type*2];
//	SndData[EFF_START].fname = SndMotorFileName[type*3+1];
	SndData[EFF_STOP].fname = SndMotorFileName[type*2+1];

	LoadMotorSound();
}

void SetMotorSound(int speed){
	if ((speed > 0) && (speed < 11)){
		if(SndData[EffectInFrequence].lpDSB)
			SetSoundFrequency(SndData[EffectInFrequence].lpDSB, EFFECT_KHZ + DELTA_KHZ*speed);
		if(SndData[EffectInFrequence + 1].lpDSB)
			SetSoundFrequency(SndData[EffectInFrequence + 1].lpDSB, EFFECT_KHZ + DELTA_KHZ*speed);
		}
}

void LoadMotorSound(void){
	XBuffer buf;
	XStream ff(0);
	for(int i = 0;i < 3;i++){
		buf.init();

		if(EngineNoise)
			buf < "resource/sound/effects/" < "/" < SndData[i].fname < ".wav";

		if(ff.open(buf.GetBuf(),XS_IN)){
			ff.close();
			if (SndData[i].lpDSB){
				SoundStop(SndData[i].channel);
				SoundRelease(SndData[i].lpDSB);
				SndData[i].lpDSB = NULL;
			}
			SoundLoad(buf.GetBuf(),&SndData[i].lpDSB);
		}
	}	//  end for i
}

void LoadResourceSOUND(const char *path_name, int surface)
{
	int i;
	//if(MuteLog) return;

	EffectsOff();

	XBuffer buf;
	XStream ff(0);
	for(i = 0;i < EFF_MAX;i++) {

		if ((SndData[i].status & LOCAL_SOUND) && SndData[i].lpDSB && surface) {
			SoundRelease(SndData[i].lpDSB);
			SndData[i].lpDSB = NULL;
		}

		buf.init();
		if((i == EFF_DRIVING) || (i == EFF_START) || (i == EFF_STOP)) {
			if(EngineNoise) {
				buf < "resource/sound/effects" < "/" < SndData[i].fname < ".wav";
			} else {
				continue;
			}
		} else {
			buf < path_name < "/" < SndData[i].fname < ".wav";
		}

		if(ff.open(buf.GetBuf(),XS_IN)){
			ff.close();
			if (SndData[i].lpDSB){
				SoundRelease(SndData[i].lpDSB);
				SndData[i].lpDSB = NULL;
				}
			SoundLoad(buf.GetBuf(),&SndData[i].lpDSB);
			
		}
		//std::cout<<"file:"<<buf.GetBuf()<<" addr:"<<SndData[i].lpDSB<<std::endl;
	}
	EffectsOn(surface);

	sndBackgroundMax = sndStepMax = sndScratchMax = 0;
	for(i = EFF_BACKGROUND_0;i <= EFF_BACKGROUND_5;i++)
		if(SndData[i].lpDSB) sndBackgroundMax++;
	for(i = EFF_STEP0;i <= EFF_STEP2;i++)
		if(SndData[i].lpDSB) sndStepMax++;
	for(i = EFF_SCRATCH0;i <= EFF_SCRATCH1;i++)
		if(SndData[i].lpDSB) sndScratchMax++;
}

void LocalEffectFree(void){
	EffectsOff();
	for(int i = 0;i < EFF_MAX;i++){
		if ((SndData[i].status & LOCAL_SOUND) && SndData[i].lpDSB){
			SoundRelease(SndData[i].lpDSB);
			SndData[i].lpDSB = NULL;
		}
	}
	EffectsOn(1);
}

void SoundEscaveOff(void){

	EffectsOff();
	for(int i = 0;i < EFF_MAX;i++){

		if ((SndData[i].status & ESCAVE_SOUND) && SndData[i].lpDSB){
			SoundRelease(SndData[i].lpDSB);
			SndData[i].lpDSB = NULL;
		}
	}
	EffectsOn(0);
}

void StartEFFECT(EFFECT_VALUE EffectValue,int loop,int pan)
{
	if(!EffectInUse || !SndData[EffectValue].lpDSB) return;
	int flag = SndData[EffectValue].flags;
	if(loop) flag |= DS_LOOPING;
	if(SndData[EffectValue].channel != LOCAL_CHANNEL)
		SetVolume(SndData[EffectValue].lpDSB,soundVolumecurrent);
	else
		SetVolume(SndData[EffectValue].lpDSB,soundVolume);

	if(EngineNoise && EffectValue == EFF_DRIVING) SoundStop(SndData[EffectValue].channel);
	if (SoundVolumePanning)
		SoundPan(SndData[EffectValue].channel, -pan*8);
	//if (SndData[EffectValue].status & ESCAVE_SOUND&&EffectValue!=EFF_NOISE)
	//	std::cout<<"StartEFFECT:"<<SndData[EffectValue].fname<<" int:"<<(int)EffectValue<<std::endl;
	SoundPlay(SndData[EffectValue].lpDSB,SndData[EffectValue].channel,SndData[EffectValue].priority,SndData[EffectValue].cropos,flag);
}

void StopEFFECT(EFFECT_VALUE EffectValue)
{
	if(!EffectInUse) return;
	SoundStop(SndData[EffectValue].channel);
	if(EngineNoise && EffectValue == EFF_DRIVING && SndData[EFF_STOP].lpDSB) StartEFFECT(EFF_STOP);
}

void StartSPEECH(SPEECH_VALUE SpeechValue)
{
}

void EffectsOnOff(void)
{
	if(EffectInUsePriory){
		EffectInUse = 1 - EffectInUse;
		if(!EffectInUse) {
			for(int i = 0;i < 16;i++) SoundStop(i);
			lastSoundFlag = 0;
		}
	}
}

void EffectsOff(void)
{
	if(EffectInUsePriory && EffectInUse){
		for(int i = 0;i < 16;i++) SoundStop(i);
		EffectInUse = 0;
		lastSoundFlag = 0;

		//SoundVolumeCD = -1;
	}
}

void EffectsOn(int surface)
{
	if(EffectInUsePriory && !EffectInUse) EffectInUse = 1;
}

void SetSoundVolume(int value)
{
	//std::cout<<"SetSoundVolume:"<<value<<std::endl;
	if(EffectInUsePriory){
//		xsSetVolumeWAVE(value);
		soundVolumecurrent = soundVolume = value;
		//soundVolumecurrent = soundVolume = DB_MIN + round(log10(9.0*log(double(value))/(log(2.0)*8) + 1.0)*DB_SIZE);
	}
}

void SoundQuant(void)
{
	if(!EffectInUse) return;

	if(MusicON){
		int _world_ = CurrentWorld;

		int Distance = DISTANCE_TO_TOWN;
			
		if (_world_ > 3) {
			_world_ = 4;
			Distance = DISTANCE_TO_SICRET;
		}

//		int len = GetEscaveDist();
			
		if (SoundVolumeCD == -1)
//			SoundVolumeCD = xsGetVolumeCD();
			SoundVolumeCD = xsGetVolumeMusic();
			
/*		if ( len == -1 )
			xsSetVolumeCD(0);
		else*/ {
			time_t l_time;
			time(&l_time);

			if (l_time - lastTimeCD > TimeCD){
//				int status = xsGetStatusCD();
				int status = xsGetStatusMusic();
				if (status & XCD_PAUSED)
					StartWTRACK();
			}

//			if(len > Distance) len = 0; else len = Distance - abs(len);
		}

//		xsSetVolumeCD(len*SoundVolumeCD/Distance);
	}

	int DiffSound = SoundFlag ^ lastSoundFlag;
	
	if (DiffSound){
		if (DiffSound & SoundCopterig) {
			if (SoundFlag & SoundCopterig) {
				SOUND_COPTERIG_START()
			} else {
				SOUND_COPTERIG_STOP()
			}
		}

		if (DiffSound & SoundCrotrig) {
			if (SoundFlag & SoundCrotrig) {
				SOUND_CROTRIG_START()
			} else {
				SOUND_CROTRIG_STOP()
			}
		}

		if (DiffSound & SoundCutterig) {
			if (SoundFlag & SoundCutterig) {
				SOUND_CUTTERIG_START()
			} else {
				SOUND_CUTTERIG_STOP()
			}
		}

		if (DiffSound & SoundUnderWater) {
			if (SoundFlag & SoundUnderWater) {
				SOUND_UNDERWATER_START()
			} else {
				SOUND_UNDERWATER_STOP()
			}
		}

		if (DiffSound & SoundMotor) {
			if (SoundFlag & SoundMotor) {
				SOUND_START_MOTOR()
			} else {
				SOUND_STOP_MOTOR()
			}
		}
	}
		
	lastSoundFlag = SoundFlag;

	/* Громкость расчитывается с учётом размера мира? или растояния от карты.
	int d = ViewZ - preViewZ;
	if(!d) {
		return;
	}
	std::cout<<"AAAAAAAAAAAAA"<<std::cout;
	if(ViewZ >= 512) {
		d = soundVolume - DB_SIZE/32 - (ViewZ - 512)*DB_SIZE/4/(1024 - 512);
	} else {
		d = soundVolume - DB_SIZE/32 + (512 - ViewZ)*DB_SIZE/4/(512 - 384);
	}
	preViewZ = ViewZ;

	soundVolumecurrent = d;
	if(soundVolumecurrent < DB_MIN) {
		soundVolumecurrent = DB_MIN;
	} else {
		if(soundVolumecurrent > DB_MAX) {
			soundVolumecurrent = DB_MAX;
		}
	}*/

	for(int i = 0;i < 16;i++) {
		if(i != LOCAL_CHANNEL) {
			SoundVolume(i,soundVolumecurrent);
		}
	}
}

void StartCDTRACK(void)
{
#ifndef _DEMO_
	if(!MusicON) return;
//	xsPlayOneTrackCD(LastTrack);
	xsPlayOneTrackMusic(LastTrack);
#endif
}

void PauseCDTRACK(void)
{
#ifndef _DEMO_
	if(!MusicON) return;
//	xsPauseCD();
	xsPauseMusic();
#endif
}

void ResumeCDTRACK(void)
{
#ifndef _DEMO_
	if(!MusicON) return;
//	xsResumeCD();
	xsResumeMusic();
#endif
}
static int SlowOn = 1;
void StopCDTRACK(void)
{
#ifndef _DEMO_
	if(!MusicON) return;
//	LastTrack = xsGetCurTrackCD();
//	xsStopCD();
	xsStopMusic(LastTrack);
	activeWTRACK = 0;
	SlowOn = 1;
#endif
}

static int LocalVolume;
static int DeltaVolume = 0;

void SlowCDTRACK(void)
{
#ifndef _DEMO_
	if(!MusicON) return;
	if (SlowOn){
//		LocalVolume = xsGetVolumeCD();
		LocalVolume = xsGetVolumeMusic();
		DeltaVolume = (LocalVolume/40) + 1;
	}
		
	SlowOn = 0;
//	LocalVolume = xsGetVolumeCD() - DeltaVolume;
	LocalVolume = xsGetVolumeMusic() - DeltaVolume;
	if (LocalVolume > 0 )
//		xsSetVolumeCD(LocalVolume);
		xsSetVolumeMusic(LocalVolume);
	else{
//		xsStopCD();
		xsStopMusic();
		activeWTRACK = 0;
		SlowOn = 1;
	}
#endif
}

void StartWTRACK(void)
{
#ifndef _DEMO_
	if(!MusicON) return;
	int w_id = CurrentWorld;

	if (CurrentWorld == -1)
		w_id = getWID();

	if(w_id >= 0 && w_id <= 3){
//		xsPlayOneTrackCD(ST_FOSTRAL + w_id);
		xsPlayOneTrackMusic(ST_FOSTRAL + w_id);
		TimeCD = TrackCDTime[ST_FOSTRAL + w_id];
	} else {
		if(w_id > 3){
//			xsPlayOneTrackCD(ST_SECRETS);
			xsPlayOneTrackMusic(ST_SECRETS);
			TimeCD = TrackCDTime[ST_SECRETS];
		}
	}
	time(&lastTimeCD);
	activeWTRACK = 1;
#endif
}

void MainMenuSoundQuant(int TRACK){
#ifndef _DEMO_
	if(!MusicON) return;

//	int track = xsGetCurTrackCD();
//	int status = xsGetStatusCD();
	int status = xsGetStatusMusic();

	if (SoundVolumeCD == -1)
//			SoundVolumeCD = xsGetVolumeCD();
			SoundVolumeCD = xsGetVolumeMusic();

//	if ( TRACK != track || !activeWTRACK )
	time_t l_time;
	time(&l_time);

	if (l_time - lastTimeCD > TimeCD){
//		int status = xsGetStatusCD();
		int status = xsGetStatusMusic();
		if (status & XCD_PAUSED){
//			xsPlayOneTrackCD(TRACK);
			xsPlayOneTrackMusic(TRACK);
			TimeCD = TrackCDTime[TRACK];
			time(&lastTimeCD);
		}
	} else 	if ( !activeWTRACK ){
//		xsPlayOneTrackCD(TRACK);
		xsPlayOneTrackMusic(TRACK);
		TimeCD = TrackCDTime[TRACK];
		time(&lastTimeCD);
	}
	activeWTRACK = 1;
#endif
}

void LastStartWTRACK(int TRACK)
{
	if(!MusicON) return;
	LastTrack = TRACK;

	if (TRACK == ST_DOUBLE){
//		xsPlayCD(ST_THEEND);
		xsPlayMusic(ST_THEEND);
		TimeCD = TrackCDTime[ST_THEEND] + TrackCDTime[ST_THEEND_DOUBLE];
	}else{
		if (TRACK == ST_THEEND_DOUBLE)
//			xsPlayCD(TRACK);
			xsPlayMusic(TRACK);
		else
//			xsPlayOneTrackCD(TRACK);
			xsPlayOneTrackMusic(TRACK);
		TimeCD = TrackCDTime[TRACK];
	}

	time(&lastTimeCD);
	activeWTRACK = 1;
}

void LastSoundQuant(void){
	if(!MusicON) return;

	time_t l_time;
	time(&l_time);

	if (l_time - lastTimeCD > TimeCD){
//		int status = xsGetStatusCD();
		int status = xsGetStatusMusic();
		if (status & XCD_PAUSED){
			if (LastTrack == ST_DOUBLE){
//				xsPlayCD(ST_THEEND);
				xsPlayMusic(ST_THEEND);
				TimeCD = TrackCDTime[ST_THEEND] + TrackCDTime[ST_THEEND_DOUBLE];
			}else{
				if (LastTrack == ST_THEEND_DOUBLE)
//					xsPlayCD(LastTrack);
					xsPlayMusic(LastTrack);
				else
//					xsPlayOneTrackCD(LastTrack);
					xsPlayOneTrackMusic(LastTrack);
				TimeCD = TrackCDTime[LastTrack];
			}
			time(&lastTimeCD);
		}
	} 
}

