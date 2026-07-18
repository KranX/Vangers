/* ---------------------------- INCLUDE SECTION ----------------------------- */

#include <algorithm>
#include <cstring>
#include <iostream>
#include <limits>
#include <vector>

extern "C" {
#include <libavutil/mem.h>
#include <libswscale/swscale.h>
}

#include "_xsound.h"
#include "avi.h"
#include "xcritical.h"
#include "xglobal.h"
#include "xgraph.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */

/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void FinitAVI(void);
void xtRegisterSysFinitFnc(void (*fPtr)(void), int id);
void xtUnRegisterSysFinitFnc(int id);

/* --------------------------- DEFINITION SECTION --------------------------- */

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(60, 3, 100) ||  \
	LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(60, 3, 100) || \
	LIBAVUTIL_VERSION_INT < AV_VERSION_INT(58, 2, 100) ||   \
	LIBSWSCALE_VERSION_INT < AV_VERSION_INT(7, 1, 100)
#	error "FFmpeg 6.0 or newer is required"
#endif

static XList aviXList;

AVIFile::AVIFile(void)
	: pFormatCtx(NULL), pCodecCtx(NULL), pCodec(NULL), pFrame(NULL), pNextFrame(NULL), packet{},
	  videoStream(-1), swsContext(NULL), rgbaFrame(NULL), rgbaWidth(0), rgbaHeight(0),
	  rgbaLineSize(0), avCriticalSection(SDL_CreateMutex()), pause(1), width(0), height(0), x(0),
	  y(0), redraw(0), released(0), flags(0), frameReady(0), pendingFrameReady(0), inputEof(0),
	  flushSent(0), decodeFinished(0), converted(0), decodedFrameCount(0),
	  firstVideoPts(AV_NOPTS_VALUE), currentFrameTime(0), pendingFrameTime(0),
	  lastDecodedFrameTime(0), frameDuration(1000.0 / 15.0), playbackStart(0), audioSample(NULL),
	  audioChannel(0), audioPlaying(0) {}

AVIFile::AVIFile(char *aviname, int initFlags, int channel): AVIFile() {
	open(aviname, initFlags, channel);
}

int AVIFile::open(char *aviname, int initFlags, int channel) {
	filename = aviname;
	flags = initFlags;
	audioChannel = channel;

	if (!avCriticalSection) {
		std::cout << "Couldn't create video mutex" << std::endl;
		return 0;
	}
	if (flags & AVI_INTERPOLATE)
		return 0;

	int ret = avformat_open_input(&pFormatCtx, aviname, NULL, NULL);
	if (ret < 0) {
		char error_message[AV_ERROR_MAX_STRING_SIZE];
		av_strerror(ret, error_message, sizeof(error_message));
		std::cout << "Couldn't open video file: " << aviname << " " << error_message << std::endl;
		return 0;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		std::cout << "Couldn't find stream information file: " << aviname << std::endl;
		return 0;
	}

	for (unsigned i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
			break;
		}
	}
	if (videoStream == -1) {
		std::cout << "Didn't find a video stream file: " << aviname << std::endl;
		return 0;
	}
	pCodecCtx = avcodec_alloc_context3(NULL);
	if (!pCodecCtx) {
		std::cout << "Unable to allocate codec context" << std::endl;
		return 0;
	}
	ret = avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoStream]->codecpar);
	if (ret < 0) {
		std::cout << "Couldn't get the codec context" << std::endl;
		return 0;
	}

	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (!pCodec) {
		std::cout << "Unsupported codec! file: " << aviname << std::endl;
		return 0;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		std::cout << "Could not open codec file: " << aviname << std::endl;
		return 0;
	}

	pFrame = av_frame_alloc();
	pNextFrame = av_frame_alloc();
	if (!pFrame || !pNextFrame) {
		std::cout << "Unable to allocate video frames" << std::endl;
		return 0;
	}

	width = pCodecCtx->width;
	height = pCodecCtx->height;
	if (width <= 0 || height <= 0 || width > std::numeric_limits<int>::max() / 4) {
		std::cout << "Invalid video dimensions file: " << aviname << std::endl;
		return 0;
	}
	rgbaLineSize = width * 4;
	if ((size_t)height > std::numeric_limits<size_t>::max() / rgbaLineSize) {
		std::cout << "Video frame is too large file: " << aviname << std::endl;
		return 0;
	}
	rgbaFrame = (uint8_t *)av_malloc((size_t)rgbaLineSize * height);
	if (!rgbaFrame) {
		std::cout << "Unable to allocate converted video frame" << std::endl;
		return 0;
	}
	rgbaWidth = width;
	rgbaHeight = height;

	AVRational frame_rate = av_guess_frame_rate(pFormatCtx, pFormatCtx->streams[videoStream], NULL);
	if (frame_rate.num > 0 && frame_rate.den > 0)
		frameDuration = 1000.0 * frame_rate.den / frame_rate.num;

	if (!(flags & AVI_NOTIMER))
		loadAudio(aviname);

	if (flags & AVI_DRAW)
		draw();
	return 1;
}

void AVIFile::loadAudio(const char *aviname) {
	AVFormatContext *audioFormat = NULL;
	if (avformat_open_input(&audioFormat, aviname, NULL, NULL) < 0)
		return;
	if (avformat_find_stream_info(audioFormat, NULL) < 0) {
		avformat_close_input(&audioFormat);
		return;
	}

	int audio_stream = -1;
	int sample_rate = 0;
	int channels = 0;
	AVCodecID codec_id = AV_CODEC_ID_NONE;
	for (unsigned i = 0; i < audioFormat->nb_streams; i++) {
		AVCodecParameters *parameters = audioFormat->streams[i]->codecpar;
		if (parameters->codec_type != AVMEDIA_TYPE_AUDIO)
			continue;
		codec_id = parameters->codec_id;
		sample_rate = parameters->sample_rate;
		channels = parameters->ch_layout.nb_channels;
		audio_stream = i;
		break;
	}

	if (audio_stream == -1 || codec_id != AV_CODEC_ID_PCM_S16LE || sample_rate <= 0 ||
		(channels != 1 && channels != 2)) {
		avformat_close_input(&audioFormat);
		return;
	}

	std::vector<uint8_t> samples;
	AVPacket audio_packet{};
	while (av_read_frame(audioFormat, &audio_packet) >= 0) {
		if (audio_packet.stream_index == audio_stream && audio_packet.size > 0) {
			samples.insert(samples.end(), audio_packet.data, audio_packet.data + audio_packet.size);
		}
		av_packet_unref(&audio_packet);
	}
	av_packet_unref(&audio_packet);
	avformat_close_input(&audioFormat);

	const size_t sample_size = sizeof(int16_t) * channels;
	samples.resize(samples.size() - samples.size() % sample_size);
	if (samples.empty())
		return;

	std::string sample_name = "avi:" + filename;
	SoundLoadRaw(
		sample_name.c_str(),
		samples.data(),
		samples.size(),
		sample_rate,
		SDL_AUDIO_S16LE,
		channels,
		&audioSample
	);
}

int AVIFile::decodeNextFrame(AVFrame *frame) {
	av_frame_unref(frame);
	while (true) {
		int ret = avcodec_receive_frame(pCodecCtx, frame);
		if (ret == 0)
			return 1;
		if (ret == AVERROR_EOF)
			return 0;
		if (ret != AVERROR(EAGAIN)) {
			std::cout << "Can't receive video frame: " << filename << std::endl;
			return 0;
		}

		if (inputEof) {
			if (flushSent)
				return 0;
			ret = avcodec_send_packet(pCodecCtx, NULL);
			flushSent = 1;
			if (ret < 0 && ret != AVERROR_EOF) {
				std::cout << "Can't flush video decoder: " << filename << std::endl;
				return 0;
			}
			continue;
		}

		while (true) {
			ret = av_read_frame(pFormatCtx, &packet);
			if (ret < 0) {
				inputEof = 1;
				break;
			}
			if (packet.stream_index != videoStream) {
				av_packet_unref(&packet);
				continue;
			}

			ret = avcodec_send_packet(pCodecCtx, &packet);
			av_packet_unref(&packet);
			if (ret < 0) {
				std::cout << "Can't send video packet: " << filename << std::endl;
				return 0;
			}
			break;
		}
	}
}

double AVIFile::frameTime(const AVFrame *frame) {
	double result = decodedFrameCount * frameDuration;
	if (frame->best_effort_timestamp != AV_NOPTS_VALUE) {
		if (firstVideoPts == AV_NOPTS_VALUE)
			firstVideoPts = frame->best_effort_timestamp;
		result = av_q2d(pFormatCtx->streams[videoStream]->time_base) *
				 (frame->best_effort_timestamp - firstVideoPts) * 1000.0;
	}
	if (decodedFrameCount && result <= lastDecodedFrameTime)
		result = lastDecodedFrameTime + frameDuration;
	lastDecodedFrameTime = result;
	decodedFrameCount++;
	return result;
}

int AVIFile::rewindVideo(void) {
	if (av_seek_frame(pFormatCtx, videoStream, 0, AVSEEK_FLAG_BACKWARD) < 0)
		return 0;
	avcodec_flush_buffers(pCodecCtx);
	av_frame_unref(pFrame);
	av_frame_unref(pNextFrame);
	frameReady = 0;
	pendingFrameReady = 0;
	inputEof = 0;
	flushSent = 0;
	decodeFinished = 0;
	converted = 0;
	decodedFrameCount = 0;
	firstVideoPts = AV_NOPTS_VALUE;
	currentFrameTime = 0;
	pendingFrameTime = 0;
	lastDecodedFrameTime = 0;
	playbackStart = SDL_GetTicks();
	return 1;
}

void AVIFile::draw(void) {
	if (pause || released)
		return;

	XCriticalSection critical(avCriticalSection);
	if (flags & AVI_NOTIMER) {
		if (!decodeNextFrame(pFrame)) {
			if (!(flags & AVI_LOOPING) || !rewindVideo() || !decodeNextFrame(pFrame)) {
				decodeFinished = 1;
				return;
			}
		}
		frameReady = 1;
		converted = 0;
		redraw = 1;
		return;
	}

	if (!frameReady) {
		if (!decodeNextFrame(pFrame)) {
			decodeFinished = 1;
			return;
		}
		currentFrameTime = frameTime(pFrame);
		frameReady = 1;
		converted = 0;
		redraw = 1;
	}

	const double elapsed = SDL_GetTicks() - playbackStart;
	while (true) {
		if (!pendingFrameReady && !decodeFinished) {
			if (decodeNextFrame(pNextFrame)) {
				pendingFrameTime = frameTime(pNextFrame);
				pendingFrameReady = 1;
			} else {
				decodeFinished = 1;
			}
		}
		if (!pendingFrameReady || pendingFrameTime > elapsed)
			break;

		std::swap(pFrame, pNextFrame);
		currentFrameTime = pendingFrameTime;
		pendingFrameReady = 0;
		converted = 0;
		redraw = 1;
	}
}

AVIFile::~AVIFile() {
	close();
}

void AVIFile::close(void) {
	if (released)
		return;
	released = 1;

	if (audioPlaying) {
		SoundStop(audioChannel);
		audioPlaying = 0;
	}
	if (audioSample) {
		SoundRelease(audioSample);
		audioSample = NULL;
	}

	if (avCriticalSection)
		SDL_LockMutex(avCriticalSection);
	av_packet_unref(&packet);
	if (swsContext) {
		sws_freeContext(swsContext);
		swsContext = NULL;
	}
	av_freep(&rgbaFrame);
	av_frame_free(&pNextFrame);
	av_frame_free(&pFrame);
	if (pCodecCtx) {
		avcodec_free_context(&pCodecCtx);
	}
	if (pFormatCtx)
		avformat_close_input(&pFormatCtx);

	if (avCriticalSection) {
		SDL_UnlockMutex(avCriticalSection);
		SDL_DestroyMutex(avCriticalSection);
		avCriticalSection = NULL;
	}
}

int AVIFile::isFinished(void) const {
	if (!decodeFinished)
		return 0;
	if (!frameReady || (flags & AVI_NOTIMER))
		return 1;
	const double elapsed = SDL_GetTicks() - playbackStart;
	return elapsed >= currentFrameTime + frameDuration;
}

int AVIopen(char *filename, int flags, int channel, void **avi) {
	*avi = NULL;
	AVIFile *newavi = new AVIFile;
	if (!newavi->open(filename, flags, channel)) {
		delete newavi;
		return 0;
	}

	*avi = newavi;
	aviXList.AddElement((XListElement *)newavi);
	xtRegisterSysFinitFnc(FinitAVI, XAVI_SYSOBJ_ID);
	return 1;
}

void AVIplay(void *avi, int xx, int yy) {
	AVIFile *file = (AVIFile *)avi;
	if (!file || file->released)
		return;
	file->pause = 0;
	file->x = xx;
	file->y = yy;
	file->playbackStart = SDL_GetTicks();
	if (file->audioSample && !file->audioPlaying) {
		SoundPlay(file->audioSample, file->audioChannel, 0, 0, 0);
		file->audioPlaying = 1;
	}
}

void AVIstop(void *avi) {
	AVIFile *file = (AVIFile *)avi;
	if (!file)
		return;
	file->pause = 1;
	if (file->audioPlaying) {
		SoundStop(file->audioChannel);
		file->audioPlaying = 0;
	}
}

void AVIclose(void *avi) {
	AVIFile *p = (AVIFile *)avi;
	if (!p)
		return;
	aviXList.RemoveElement((XListElement *)p);
	delete p;
}

int AVIwidth(void *avi) {
	return ((AVIFile *)avi)->width;
}

int AVIheight(void *avi) {
	return ((AVIFile *)avi)->height;
}

int AVIredraw(void *avi) {
	return ((AVIFile *)avi)->redraw;
}

void AVIredraw(void *avi, int state) {
	((AVIFile *)avi)->redraw = state;
}

int AVIisFinished(void *avi) {
	return !avi || ((AVIFile *)avi)->isFinished();
}

void AVIPrepareFrame(void *avi) {
	if (avi)
		((AVIFile *)avi)->draw();
}

void AVIDrawFrame(
	void *avi,
	int offsetX,
	int offsetY,
	int lineWidth,
	uint32_t *rgba,
	float bright,
	int outputWidth,
	int outputHeight
) {
	AVIFile *file = (AVIFile *)avi;
	if (!file || !rgba)
		return;
	XCriticalSection critical(file->avCriticalSection);
	if (!file->frameReady)
		return;

	if (outputWidth <= 0)
		outputWidth = file->width;
	if (outputHeight <= 0)
		outputHeight = file->height;
	if (outputWidth > std::numeric_limits<int>::max() / 4 ||
		(size_t)outputHeight > std::numeric_limits<size_t>::max() / (outputWidth * 4))
		return;

	if (file->rgbaWidth != outputWidth || file->rgbaHeight != outputHeight) {
		uint8_t *resized =
			(uint8_t *)av_realloc(file->rgbaFrame, (size_t)outputWidth * outputHeight * 4);
		if (!resized)
			return;
		file->rgbaFrame = resized;
		file->rgbaWidth = outputWidth;
		file->rgbaHeight = outputHeight;
		file->rgbaLineSize = outputWidth * 4;
		file->converted = 0;
	}

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	const AVPixelFormat output_format = AV_PIX_FMT_BGRA;
#else
	const AVPixelFormat output_format = AV_PIX_FMT_ARGB;
#endif
	if (!file->converted) {
		file->swsContext = sws_getCachedContext(
			file->swsContext,
			file->width,
			file->height,
			(AVPixelFormat)file->pFrame->format,
			file->rgbaWidth,
			file->rgbaHeight,
			output_format,
			SWS_BILINEAR,
			NULL,
			NULL,
			NULL
		);
		if (!file->swsContext)
			return;

		uint8_t *destination[] = {file->rgbaFrame, NULL, NULL, NULL};
		int destination_lines[] = {file->rgbaLineSize, 0, 0, 0};
		sws_scale(
			file->swsContext,
			file->pFrame->data,
			file->pFrame->linesize,
			0,
			file->height,
			destination,
			destination_lines
		);
		for (int y = 0; y < file->rgbaHeight; y++) {
			uint32_t *row = reinterpret_cast<uint32_t *>(file->rgbaFrame + y * file->rgbaLineSize);
			for (int x = 0; x < file->rgbaWidth; x++)
				row[x] |= 0xFF000000u;
		}
		file->converted = 1;
	}

	bright = std::max(0.0f, std::min(1.0f, bright));
	const int source_x = std::max(0, -offsetX);
	const int source_y = std::max(0, -offsetY);
	const int end_x = std::min(file->rgbaWidth, lineWidth - offsetX);
	const int end_y = std::min(file->rgbaHeight, XGR_MAXY - offsetY);
	if (source_x >= end_x || source_y >= end_y)
		return;

	for (int y = source_y; y < end_y; y++) {
		const uint8_t *source = file->rgbaFrame + y * file->rgbaLineSize + source_x * 4;
		uint8_t *destination = (uint8_t *)(rgba + (y + offsetY) * lineWidth + source_x + offsetX);
		if (bright == 1.0f) {
			std::memcpy(destination, source, (end_x - source_x) * 4);
			continue;
		}
		for (int x = source_x; x < end_x; x++) {
			destination[0] = source[0] * bright;
			destination[1] = source[1] * bright;
			destination[2] = source[2] * bright;
			destination[3] = source[3];
			source += 4;
			destination += 4;
		}
	}
}

void FinitAVI(void) {
	AVIFile *p = (AVIFile *)aviXList.fPtr;
	while (p) {
		AVIFile *next = (AVIFile *)p->next;
		AVIclose(p);
		p = next;
	}
}
