#include "ogg_stream.h"
#include <clunk/sample.h>
#include <clunk/locker.h>
#include <assert.h>
#include <stdexcept>

static size_t stream_read_func  (void *ptr, size_t size, size_t nmemb, void *datasource) {
	//LOG_DEBUG(("read(%p, %u, %u)", ptr, (unsigned)size, (unsigned)nmemb));
	assert(datasource != NULL);
	
	FILE *file = (FILE *)datasource;
	int r = fread(ptr, size, nmemb, file);
	if (r <= 0)
		return r;
	
	return r / size;
}

static int    stream_seek_func  (void *datasource, ogg_int64_t offset, int whence) {
	//LOG_DEBUG(("seek(%u, %d)", (unsigned)offset, whence));
	assert(datasource != NULL);
	FILE *file = (FILE *)datasource;
	return fseek(file, offset, whence);
}

static int    stream_close_func (void *datasource) {
	//LOG_DEBUG(("close()"));
	assert(datasource != NULL);
	FILE *file = (FILE *)datasource;
	return fclose(file);
}

static long   stream_tell_func  (void *datasource) {
	//LOG_DEBUG(("tell"));
	assert(datasource != NULL);
	FILE *file = (FILE *)datasource;
	return ftell(file);
}

OggStream::OggStream(const std::string &fname) {
	_file = fopen(fname.c_str(), "rb");
	if (_file == NULL) {
		perror("fopen");
		throw std::runtime_error("cannot open file");
	}

	ov_callbacks ov_cb;
	memset(&ov_cb, 0, sizeof(ov_cb));

	ov_cb.read_func = stream_read_func;
	ov_cb.seek_func = stream_seek_func;
	ov_cb.tell_func = stream_tell_func;
	ov_cb.close_func = stream_close_func;

	int r = ov_open_callbacks(_file, &_ogg_stream, NULL, 0, ov_cb);
	if (r < 0) {
		throw std::runtime_error("ov_open_callbacks failed");
	}
	
	_vorbis_info = ov_info(&_ogg_stream, -1);

	sample_rate = _vorbis_info->rate;
	//LOG_DEBUG(("open(%s) : %d", fname.c_str(), sample_rate));
	format = AUDIO_S16LSB;
	channels = _vorbis_info->channels;

	//_vorbis_comment = ov_comment(&_ogg_stream, -1);
	assert(_vorbis_info != NULL);
}

void OggStream::rewind() {
	int r = ov_raw_seek(&_ogg_stream, 0);
	if (r != 0)
		throw std::runtime_error("ov_raw_seek");
}

bool OggStream::read(clunk::Buffer &data, unsigned hint) {
	clunk::AudioLocker l;
	if (hint == 0) 
		hint = 44100;
	
	data.set_size(hint);

	int section = 0;
	int r = ov_read(&_ogg_stream, (char *)data.get_ptr(), hint, 0, 2, 1, & section);
	//LOG_DEBUG(("ov_read(%d) = %d (section: %d)", hint, r, section));
	
	if(r >= 0) {
		data.set_size(r);
		
		return r != 0;
	}

	return false; //:(
}

OggStream::~OggStream() {}
