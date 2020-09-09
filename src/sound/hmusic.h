#ifndef __HMUSIC_H
#define __HMUSIC_H

void xsInitMusic(void);
void xsDeInitMusic(void);
void xsPlayMusic(int track,int min = 0,int sec = 0);
void xsPlayOneTrackMusic(int track,int min = 0,int sec = 0);
void xsStopMusic(void);
void xsStopMusic(int &track);
void xsPauseMusic(void);
void xsResumeMusic(void);
int xsGetStatusMusic(void);
int xsGetCurTrackMusic(void);
int xsGetNumTracksMusic(void);
int xsGetTrackLenMusic(int track);

/* ------------------- Volume Management ------------------- */

void xsMixerOpenMusic(void);
int xsGetVolumeMusic(void);
void xsSetVolumeMusic(int val);


#endif