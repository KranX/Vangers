#ifndef __XSOUND_H
#define __XSOUND_H

#include <clunk/clunk.h>


#define DS_LOOPING			0x00000001
#define DS_QUEUE			0x00000100
#define DS_STREAM			0x00000200

#define AVI_LOOPING			DS_LOOPING
#define AVI_DRAW			0x00100000
#define AVI_END 			0x00200000
#define AVI_NOTIMER			0x00400000
#define AVI_NODRAW			0x00800000
#define AVI_INTERPOLATE 		0x01000000
#define AVI_NOPALETTE			0x02000000

#define DIG_F_MONO_8			0
#define DIG_F_MONO_16			1
#define DIG_F_STEREO_8			2
#define DIG_F_STEREO_16 		3

#define PLAYBACK_TIMER_PERIOD		200
#define PLAYBACK_OVERSAMPLE		4
#define PLAYBACK_TIMER_ACCURACY 	10
#define NUM_BUFFER_SEGMENTS		4


int SoundInit(int maxHZ, int channels);
void SoundPlay(void *lpDSB, int channel, int priority, int cropos, int flags);
void SoundRelease(void *lpDSB);
void SoundStop(int channel);
void* GetSound(int channel);
void SoundLoad(char *filename, void **lpDSB);
void SoundFinit(void);
void SoundVolume(int channel, int volume);
void SetVolume(void *lpDSB, int volume);
int GetVolume(void *lpDSB);
int GetSoundVolume(int channel);
void GlobalVolume(int volume);
void SoundPan(int channel, int panning);

int GetSoundFrequency(void *lpDSB);
void SetSoundFrequency(void *lpDSB,int frq);

void SoundStreamOpen(char *filename, void **strptr);
void SoundStreamClose(void *stream);
void SoundStreamRelease(void *stream);

#define SoundStreamPlay(lpDSB, channel, priority, cropos, flags) SoundPlay(lpDSB, channel, priority, cropos, flags | DS_STREAM)

/* --------------------- CD Management --------------------- */

// xsGetStatusCD() return values...
#define XCD_NOT_READY			0x00
#define XCD_PAUSED			0x01
#define XCD_PLAYING			0x02
#define XCD_STOPPED			0x03
#define XCD_OTHER			0x04

void xsInitCD(void);
void xsPlayCD(int track,int min = 0,int sec = 0);
void xsPlayOneTrackCD(int track,int min = 0,int sec = 0);
void xsStopCD(void);
void xsPauseCD(void);
void xsResumeCD(void);
int xsGetStatusCD(void);
int xsGetCurTrackCD(void);
int xsGetNumTracksCD(void);
int xsGetTrackLenCD(int track);

/* ------------------- Volume Management ------------------- */

void xsMixerOpen(void);
int xsGetVolumeCD(void);
void xsSetVolumeCD(int val);
int xsGetVolumeWAVE(void);
void xsSetVolumeWAVE(int val);

#endif 
