#ifndef __AVI_H__
#define __AVI_H__

#include <SDL.h>
#include <string>

#include "xtcore.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifndef INT64_C
#	define INT64_C(c) (c##LL)
#	define UINT64_C(c) (c##ULL)
#endif

#include <avcodec.h>
#include <avformat.h>
#ifdef __cplusplus
}
#endif

struct SwsContext;

#define AVI_END_VIDEO 0x00010000
#define AVI_END_SOUND 0x00020000

struct AVIFile: XListElement {
	AVFormatContext *pFormatCtx;
	AVCodecContext *pCodecCtx;
	const AVCodec *pCodec;
	AVFrame *pFrame;
	AVFrame *pNextFrame;
	AVPacket packet;
	int videoStream;
	SwsContext *swsContext;
	uint8_t *rgbaFrame;
	int rgbaWidth;
	int rgbaHeight;
	int rgbaLineSize;

	SDL_mutex *avCriticalSection;
	int pause;

	int width;
	int height;
	int x, y;

	int redraw;
	int released;
	int flags;
	int frameReady;
	int pendingFrameReady;
	int inputEof;
	int flushSent;
	int decodeFinished;
	int converted;
	int decodedFrameCount;
	int64_t firstVideoPts;
	double currentFrameTime;
	double pendingFrameTime;
	double lastDecodedFrameTime;
	double frameDuration;
	Uint32 playbackStart;

	void *audioSample;
	int audioChannel;
	int audioPlaying;
	std::string filename;

	~AVIFile();
	AVIFile(void);
	AVIFile(char *aviname, int flags, int channel);
	int open(char *aviname, int flags, int channel);
	void draw(void);
	void close(void);
	int isFinished(void) const;

	int decodeNextFrame(AVFrame *frame);
	double frameTime(const AVFrame *frame);
	int rewindVideo(void);
	void loadAudio(const char *aviname);
};

int AVIopen(char *filename, int flags, int channel, void **avi);
void AVIplay(void *avi, int x, int y);
void AVIstop(void *avi);
void AVIclose(void *avi);
int AVIwidth(void *avi);
int AVIheight(void *avi);
int AVIredraw(void *avi);
void AVIredraw(void *avi, int state);
int AVIisFinished(void *avi);

void AVIPrepareFrame(void *avi);
void AVIDrawFrame(
	void *avi,
	int offsetX,
	int offsetY,
	int lineWidth,
	uint32_t *rgba,
	float bright = 1.0,
	int outputWidth = 0,
	int outputHeight = 0
);

#endif //__AVI_H__
