#ifndef __AVI_H__
#define __AVI_H__

#include <SDL.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
#ifndef INT64_C
#	define INT64_C(c) (c ## LL)
#	define UINT64_C(c) (c ## ULL)
#endif

#include <avformat.h>
#ifdef __cplusplus
}
#endif

#define AVI_END_VIDEO		0x00010000
#define AVI_END_SOUND		0x00020000

struct AVIFile : XListElement
{
	AVFormatContext *pFormatCtx;
	AVCodecContext *pCodecCtx;
	AVCodec *pCodec;
	AVFrame *pFrame;
	AVPacket packet;
	int videoStream;

	SDL_mutex *avCriticalSection;
	int pause;

	int width;
	int height;
	int x,y;

	int redraw;
	int released;
	int flags;
	unsigned char* color_table;
	unsigned char palette[768];
	std::string filename;

	~AVIFile();
	AVIFile(void);
	AVIFile(char *aviname, int flags, int channel);
	int open(char *aviname, int flags, int channel);
	void draw(void);
	void close(void);
};

#endif //__AVI_H__
