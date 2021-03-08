//#include "PlayOgg.h"

#include <sstream>

#include "../global.h"

#include "hsound.h"
#include "../../lib/xsound/ogg_stream.h"

extern clunk::Context context;
//static MpegSound* music = 0;
//extern LPDIRECTSOUND lpDS;

static int curTrack = 2;
static int volume = 128;

const char* getTrackPathName(int track) {
	switch (track) {
		case ST_INTRO:			//2
			return "resource/music/track02.ogg";
		case ST_FOSTRAL:		//3
			return "resource/music/track03.ogg";
		case ST_GLORX:			//4
			return "resource/music/track04.ogg";
		case ST_NECROSS:		//5
			return "resource/music/track05.ogg";
		case ST_XPLO:			//6
			return "resource/music/track06.ogg";
		case ST_SECRETS:		//7
			return "resource/music/track07.ogg";
		case ST_GAMEOVER:		//8
			return "resource/music/track08.ogg";
		case ST_THEEND:			//9
			return "resource/music/track09.ogg";
		case ST_THEEND_DOUBLE:	//10
			return "resource/music/track10.ogg";
		case ST_DOUBLE:			//11
			return "resource/music/track01.ogg";
		default:
			return "resource/music/track01.ogg";
	}
}

void xsInitMusic(void) {
	//MpegInitLibrary(lpDS);
//	music = new MpegSound();
//	music->SetVolume(255);
}

void xsDeInitMusic(void) {
//	music->Stop();
//	delete music;
	//if (music)
	//	{
		//Mix_FreeMusic(music);
	//	music = NULL;
	//	}
	//MpegDeinitLibrary();
}

void xsPlayMusic(int track,int min = 0,int sec = 0) {
	curTrack = track;
	//music->OpenToPlay(getTrackPathName(track));
	//music=Mix_LoadMUS(getTrackPathName(track));
	//Mix_PlayMusic(music, 31);
	//VNG_DEBUG()<<"xsPlayMusic:"<<curTrack<<std::endl;
	context.play(-1, new OggStream(getTrackPathName(track)), true);
	context.set_volume(-1, volume/256.0);
}

void xsPlayOneTrackMusic(int track,int min = 0,int sec = 0) {
	curTrack = track;
	//if (music)
	//	{
		//Mix_HaltMusic();
		//Mix_RewindMusic();
		//Mix_FreeMusic(music);
		//music = NULL;
		//}
	//music=Mix_LoadMUS(getTrackPathName(track));
	//Mix_RewindMusic();
	//Mix_PlayMusic(music, 31);
	//music->OpenToPlay(getTrackPathName(track));
	//VNG_DEBUG()<<"xsPlayOneTrackMusic:"<<curTrack<<std::endl;
	if (context.playing(-1))
		context.stop(-1);
	context.play(-1, new OggStream(getTrackPathName(track)), true);
	context.set_volume(-1, volume/256.0);
}

void xsStopMusic(void) {
	//music->Stop();
	//Mix_HaltMusic();
	if (context.playing(-1))
		context.stop(-1);

}

void xsStopMusic(int &track) {
	track = curTrack;
	xsStopMusic();
}

void xsPauseMusic(void) {
	//music->Pause();
	//Mix_PausedMusic();
	context.pause(-1);
}

void xsResumeMusic(void) {
	//music->Resume();
	//Mix_ResumeMusic();
	context.pause(-1);
}

int xsGetStatusMusic(void) {
	/*switch (music->IsPlay()) {
		case MPEG_STOP:
			return XCD_STOPPED;
		case MPEG_PAUSE:
			return XCD_PAUSED;
		case MPEG_PLAY:
			return XCD_PLAYING;
		default:
			return XCD_OTHER;
	}*/
	if (context.playing(-1)) {
		return XCD_PLAYING;
	} else {
		return XCD_STOPPED;
	}
	return XCD_OTHER;
}

int xsGetCurTrackMusic(void) {
	return curTrack;
}

int xsGetNumTracksMusic(void) {
	return 0;
}

int xsGetTrackLenMusic(int track) {
	return 0;
}

/* ------------------- Volume Management ------------------- */

void xsMixerOpenMusic(void) {
}

int xsGetVolumeMusic(void) {
	//return music->GetVolume();
	return volume;
}

void xsSetVolumeMusic(int val) {
	//VNG_DEBUG()<<"xsSetVolumeMusic:"<<val<<std::endl;
	//music->SetVolume(val);
	float f_val = val;
	f_val/=256;
	//Mix_VolumeMusic(val);
	context.set_volume(-1, f_val);
	volume=val;
}


