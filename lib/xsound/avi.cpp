/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include <stdlib.h>
#include <stdio.h>

#include "_xsound.h"
#include "xglobal.h"
#include "xcritical.h"
#include "xgraph.h"
#include "avi.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */

/* --------------------------- PROTOTYPE SECTION ---------------------------- */


void FinitAVI(void);
void xtRegisterSysFinitFnc(void (*fPtr)(void),int id);
void xtUnRegisterSysFinitFnc(int id);


/* --------------------------- DEFINITION SECTION --------------------------- */

// compatability with newer libavcodec
#if LIBAVCODEC_VERSION_MAJOR < 57
  #define AV_FRAME_ALLOC avcodec_alloc_frame
  #define AV_PACKET_UNREF av_free_packet
#else
  #define AV_FRAME_ALLOC av_frame_alloc
  #define AV_PACKET_UNREF av_packet_unref
#endif

#define AV_CODEC_PAR (LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(57, 33, 100))

static XList aviXList;

AVIFile::AVIFile(void)
{
}

AVIFile::AVIFile(char* aviname,int initFlags,int channel)
{
	open(aviname, initFlags, channel);
}

int AVIFile::open(char* aviname, int initFlags, int channel)
{
	int i;
	filename = aviname;
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
		av_register_all();
#endif
	// Open video file
	pFormatCtx = NULL;

	int ret = avformat_open_input(&pFormatCtx, aviname, NULL, NULL);
	if(ret != 0) {
		char error_message[256];
		av_strerror(ret, error_message, 256);
		std::cout<<"Couldn't open video file:"<<aviname<<" "<<error_message<<std::endl;
		return 0; // Couldn't open file
	}
	// Retrieve stream information
	if(avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		std::cout<<"Couldn't find stream information file:"<<aviname<<std::endl;
		return 0; // Couldn't find stream information
	}
	// Find the first video stream
	videoStream=-1;
	for(i=0; i<static_cast<int>(pFormatCtx->nb_streams); i++)
#if AV_CODEC_PAR
		if(pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO) {
#else
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
#endif
			videoStream=i;
			break;
		}
	if(videoStream==-1) {
		std::cout<<"Didn't find a video stream file:"<<aviname<<std::endl;
		return 0; // Didn't find a video stream
	}

	// Get a pointer to the codec context for the video stream
#if AV_CODEC_PAR
	pCodecCtx=avcodec_alloc_context3(NULL);
	if (!pCodecCtx) {
		std::cout<<"Unabled to allocate codec context";
		return 0;
	}
	ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar);
	if (ret != 0) {
		std::cout<<"Couldn't get the codec context"<<std::endl;
		return 0;
	}
#else
	pCodecCtx=pFormatCtx->streams[videoStream]->codec;
#endif

	// Find the decoder for the video stream
	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL) {
		std::cout<<"Unsupported codec! file:"<<aviname<<std::endl;
		return 0; // Codec not found
	}
	// Open codec
#if (LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 8, 0))
	if(avcodec_open2(pCodecCtx, pCodec, NULL)<0) {
#else
	if(avcodec_open(pCodecCtx, pCodec)<0) {
#endif
		std::cout<<"Could not open codec file:"<<aviname<<std::endl;
		return 0; // Could not open codec
	}

	// Allocate video frame
	pFrame=AV_FRAME_ALLOC();

	width = pCodecCtx->width;
	height = pCodecCtx->height;
	flags = initFlags;
	if (flags & AVI_INTERPOLATE)
		return 0; // XXX: add support for palettes

	for (i = 0; i < 256; i++){
		//gray
		palette[i*3] = i >> 2;
		palette[i*3 + 1] = i >> 2;
		palette[i*3 + 2] = i >> 2;
	}

	released = redraw = x = y = 0;

	if(flags & AVI_DRAW){
		draw();
	}
	avCriticalSection = SDL_CreateMutex();
	pause = 1;
	return 1;
}

void AVIFile::draw(void) {
	if(pause) 
		return;
	int i, frameFinished = 0;
	if(!released) {
		redraw = 1;
		int frame=-1;
		while((frame=av_read_frame(pFormatCtx, &packet))>=0) {
			// Is this a packet from the video stream?
			if(packet.stream_index==videoStream) {
				// Decode video frame
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(57, 106, 102)
				int ret = avcodec_receive_frame(pCodecCtx, pFrame);
				if (ret == 0) {
					frameFinished = 1;
				}
				if (ret != AVERROR(EAGAIN)) {
					std::cout<<"Can't receive the frame"<<std::endl;
					return;
				}
				ret = avcodec_send_packet(pCodecCtx, &packet);
				if (ret != 0) {
					std::cout<<"Can't send packet"<<std::endl;
					return;
				}
#elif LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(52, 23, 0)
				avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
#else
				avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, packet.data, packet.size);
#endif
				 // Did we get a video frame?
				if(frameFinished) {
					AV_PACKET_UNREF(&packet);
					break;
				}
			}
	        AV_PACKET_UNREF(&packet);
		}
		if(frame<0 && (flags & AVI_LOOPING)) {
				// Close the codec
				avcodec_close(pCodecCtx);
				// Close the video file
				//av_close_input_file(pFormatCtx);
				avformat_close_input(&pFormatCtx);
				// Open video file
				int ret = avformat_open_input(&pFormatCtx, filename.c_str(), NULL, NULL);
				if(ret != 0) {
					std::cout<<"Couldn't open video file"<<std::endl;
					return; // Couldn't open file
				}
				// Get a pointer to the codec context for the video stream
#if AV_CODEC_PAR
				pCodecCtx=avcodec_alloc_context3(NULL);
				if (!pCodecCtx) {
					std::cout<<"Unabled to allocate codec context";
					return;
				}
				ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar);
				if (ret != 0) {
					std::cout<<"Couldn't get the codec context"<<std::endl;
					return;
				}
#else
				pCodecCtx=pFormatCtx->streams[videoStream]->codec;
#endif
				// Find the decoder for the video stream
				pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
				if(pCodec==NULL) {
					std::cout<<"Unsupported codec!"<<std::endl;
					return; // Codec not found
				}
				// Open codec
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(53, 8, 0)
				if(avcodec_open2(pCodecCtx, pCodec, NULL)<0) {
#else
				if(avcodec_open(pCodecCtx, pCodec)<0) {
#endif
					std::cout<<"Could not open codec"<<std::endl;
					return; // Could not open codec
				}
				draw();
			}else{
			for (i = 0; i < 256; i++) {
				palette[i*3] = pFrame->data[1][i*4+2] >> 2;
				palette[i*3+1] = pFrame->data[1][i*4+1] >> 2;
				palette[i*3+2] = pFrame->data[1][i*4] >> 2;
				}
			}
	}
}


AVIFile::~AVIFile()
{
	close();
}

void AVIFile::close(void)
{
	released = 1;
	SDL_mutexP(avCriticalSection);
	// Free the YUV frame
	av_free(pFrame);
	// Close the codec
	avcodec_close(pCodecCtx);
	// Close the video file
	//av_close_input_file(pFormatCtx);
	//avformat_free_context(pFormatCtx);
	avformat_close_input(&pFormatCtx);

	SDL_mutexV(avCriticalSection);
	SDL_DestroyMutex(avCriticalSection);
	
}

int AVIopen(char *filename,int flags,int channel,void **avi)
{
	AVIFile *newavi = new AVIFile;

	if(!newavi -> open(filename,flags,channel))
		return 0;

	*avi = newavi;
	aviXList.AddElement((XListElement*)newavi);

	xtRegisterSysFinitFnc(FinitAVI,XAVI_SYSOBJ_ID);
	return 1;
}

void AVIplay(void *avi,int xx,int yy)
{
	((AVIFile *)avi) -> pause = 0;
	((AVIFile *)avi) -> x = xx;
	((AVIFile *)avi) -> y = yy;
}

void AVIstop(void *avi)
{
	((AVIFile *)avi) -> pause = 1;
}

void AVIclose(void *avi)
{
	AVIFile* p = (AVIFile*)avi;
	aviXList.RemoveElement((XListElement*)p);
	delete p;
}


int AVIwidth(void *avi)
{
	return ((AVIFile *)avi) -> width;
}

int AVIheight(void *avi)
{
	return ((AVIFile *)avi) -> height;
}

int AVIredraw(void *avi)
{
	return ((AVIFile *)avi) -> redraw;
}

void AVIredraw(void *avi,int state)
{
	((AVIFile *)avi) -> redraw = state;
}

void AVIdraw(void *avi)
{
	((AVIFile *)avi) -> draw();
}

void AVIGetData(void *avi,void *data)
{
	XCriticalSection critical(((AVIFile *)avi) -> avCriticalSection);
	int xs = AVIwidth(avi);
	int ys = AVIheight(avi);
	int i, i2;

	if (((AVIFile *)avi)->pFrame)
		for(i=0; i<ys; ++i){
			for (i2=0;i2<xs;++i2){
				
				((unsigned char *)data)[i*xs+i2]=
				((unsigned char *)((AVIFile *)avi)->pFrame->data[0])[i*((AVIFile *)avi)->pFrame->linesize[0]+i2];
			}
		}
}

void *AVIGetPalette(void *avi)
{
	XCriticalSection critical(((AVIFile *)avi) -> avCriticalSection);
	return ((AVIFile *)avi) -> palette;
}

void FinitAVI(void)
{
	AVIFile* p,*p1;
	p = (AVIFile*)aviXList.fPtr;
	while(p){
		p1 = (AVIFile*)p -> next;
		AVIclose(p);
		p = p1;
	}
}
