/* ---------------------------- INCLUDE SECTION ----------------------------- */

//#include "dsound.h"
//#include <SDL_mixer.h>
#include <clunk.h>

#include "xglobal.h"
#include "xgraph.h"
#include "_xsound.h"
//#include "XCritical.h"

/* ----------------------------- STRUCT SECTION ----------------------------- */

struct XSoundChannelQueue
{
	/* SDL_Mixer version
	Mix_Chunk *sound;*/
	clunk::Sample *sound;
	int pan;
	int priority;
	int cropos;
	int flags;
	int volume;
};

struct XSoundChannel : XSoundChannelQueue
{
	XSoundChannelQueue queue;
};

/* ----------------------------- EXTERN SECTION ----------------------------- */
/* --------------------------- PROTOTYPE SECTION ---------------------------- */

void xsMixerOpen(void);

void xtRegisterSysFinitFnc(void (*fPtr)(void),int id);
void xtUnRegisterSysFinitFnc(int id);
void xtDeactivateSysFinitFnc(int id);

/* --------------------------- DEFINITION SECTION --------------------------- */

#define MAX_CHANNELS	32


static XSoundChannel channels[MAX_CHANNELS];

int XSoundError = 0;
int XSoundInitFlag = 0;
clunk::Context context;
clunk::Object * clunk_object;
int g_freq;


void SoundPlay(void *lpDSB, int channel, int priority, int cropos, int flags)
{
	if(XSoundInitFlag){
		clunk::Sample *source = (clunk::Sample *)lpDSB;
		
		//if (channel==2)
		//	std::cout<<"SoundPlay:"<<source->name<<" channel:"<<channel<<" cropos:"<<cropos<<" pr1:"<<channels[channel].priority<<" pr2:"<<priority<<" playing:"<<clunk_object->playing(source->name)<<std::endl;
		if (cropos == 255) {
			if (!clunk_object->playing(source->name)||channels[channel].priority>priority) {
				SoundStop(channel);
				clunk_object->play(source->name, new clunk::Source(source, false, clunk::v3<float>(), 1, 1, channels[channel].pan*0.0001));
				if(flags & DS_LOOPING){
					clunk_object->set_loop(source->name, true);
				}
			} else {
				return;
			}
		} else {
			if (channels[channel].sound) {
				if (clunk_object->playing(channels[channel].sound->name)&&channels[channel].priority<priority) {
					return;
				}
				SoundStop(channel);
			}
			clunk_object->play(source->name, new clunk::Source(source, false, clunk::v3<float>(), 1, 1, channels[channel].pan*0.0001));
			if(flags & DS_LOOPING){
				clunk_object->set_loop(source->name, true);
			}
		}
		if(flags & DS_QUEUE){
			std::cout<<"DS_QUEUE!!!"<<std::endl;
			
		}
		if(flags & DS_STREAM){
			std::cout<<"DS_STREAM!!!"<<std::endl;
			
		}
		channels[channel].priority = priority;
		channels[channel].flags = flags;
		channels[channel].cropos = cropos;
		channels[channel].sound = source;
		/*
		//Other sound logic for FUTURE
		//channels[channel].priority = priority;
		//channels[channel].sound = (Mix_Chunk *)lpDSB;
		//channels[channel].flags = flags;
		//Mix_PlayChannel(-1, channels[channel].sound, 0);
		if(channels[channel].sound)
			{
			if(channels[channel].priority < priority)
				{
				//ErrH.Log("D1");
				SoundStop(channel);
				channels[channel].priority = priority;
				channels[channel].flags = flags;
				//SDL_Mixer version
				//channels[channel].sound = (Mix_Chunk *)lpDSB;
				//Mix_PlayChannel(channel, channels[channel].sound, 0);
				channels[channel].sound = (clunk::Sample *)lpDSB;
				clunk_object->play(channels[channel].sound->name, new clunk::Source(channels[channel].sound, false));
				if(flags & DS_LOOPING){
					clunk_object->set_loop(channels[channel].sound->name, true);
				}
				}
			else
				{
				if(channel<20)
					SoundPlay(lpDSB, channel+10, priority, cropos, flags);
				}
			}
		else
			{
			//ErrH.Log("D2");
			channels[channel].priority = priority;
			channels[channel].flags = flags;
			//SDL_Mixer version
			//channels[channel].sound = (Mix_Chunk *)lpDSB;
			//Mix_PlayChannel(channel, channels[channel].sound, 0);
			channels[channel].sound = (clunk::Sample *)lpDSB;
			clunk_object->play(channels[channel].sound->name, new clunk::Source(channels[channel].sound, false));
			if(flags & DS_LOOPING){
				clunk_object->set_loop(channels[channel].sound->name, true);
			}
			}*/
	}
}

void SoundVolume(int channel, int volume)
{
	/* SDL_Mixer Version
	volume+=10000;
	volume*=0.0128;*/
	float f_volume = volume + 10000;
	f_volume *= 0.0001;
	//Debug
	//std::cout<<"SoundVolume:"<<channel<<" "<<volume<<std::endl;
	
	if(XSoundInitFlag){
		if(channels[channel].sound) //channels[channel].sound -> SetVolume((LONG )volume);
			{
			channels[channel].volume = volume;
			/* SDL_Mixer Version
			Mix_Volume(channel, volume);*/
			context.set_volume(channel, f_volume);
			}	
	}
}

int GetSoundVolume(int channel)
{
	int volume = 0;
	if(XSoundInitFlag){
		std::cout<<"GetSoundVolume:"<<channel<<std::endl;
		if(channels[channel].sound)
			volume = channels[channel].volume;
	}
	return volume;
}

void GlobalVolume(int volume)
{
	int i;
	std::cout<<"GlobalVolume:"<<volume<<std::endl;
	for(i = 0; i < MAX_CHANNELS; i ++)
		SoundVolume(i, volume);
}

void SoundPan(int channel, int panning)
{
	if(XSoundInitFlag){
		//std::cout<<"SoundPan "<<channel<<" "<<panning*0.001<<std::endl;
		channels[channel].pan = panning;
		//if(channels[channel].sound) {
		//	channels[channel].sound->panning = panning*0.01;
		//}
		//if(channels[channel].sound) channels[channel].sound -> SetPan((LONG )panning);
	}
}

int GetSoundFrequency(void *lpDSB)
{
	//std::cout<<"Get SoundFrequency"<<((clunk::Sample *)lpDSB)->name<<std::endl;
	unsigned long ret = 0;
	if(XSoundInitFlag){
		/*int numtimesopened, frequency, channels;
		Uint16 format;
		SDL_Mixer Version
		numtimesopened=Mix_QuerySpec(&frequency, &format, &channels);
		ret = frequency;*/
		//ret = g_freq;
		ret = ((clunk::Sample *)lpDSB)->pitch*22050;
	}
	return ret;
}

void SetSoundFrequency(void *lpDSB,int frq)
{
	//std::cout<<"SetSoundFrequency:"<<((clunk::Sample *)lpDSB)->name<<" frq:"<<frq<<std::endl;
	if(XSoundInitFlag){
		float out_pitch = frq;
		out_pitch/=22050;
		//std::cout<<"pitch:"<<out_pitch<<" freq:"<<((clunk::Sample *)lpDSB)->spec.freq<<std::endl;
		((clunk::Sample *)lpDSB)->pitch = out_pitch;
	}
}

void* GetSound(int channel)
{
	//std::cout<<"GetSound:"<<((clunk::Sample *)channels[channel].sound)->name<<" channel:"<<channel<<std::endl;
	if(XSoundInitFlag)
		return channels[channel].sound;

	return NULL;
}

void SoundStop(int channel)
{
	if(XSoundInitFlag){
		//std::cout<<"SoundStop:"<<channel<<std::endl;
		if(channels[channel].sound){
			/* SDL_Mixer version
			Mix_HaltChannel(channel); */
			//context.stop_all();
			clunk_object->cancel(channels[channel].sound->name, 0);
			channels[channel].sound = NULL;
			channels[channel].priority = channels[channel].flags = channels[channel].cropos = channels[channel].pan = 0;
		}
	}
}

void SoundRelease(void *lpDSB)
{
	if(XSoundInitFlag){
		//std::cout<<"SoundRelease:"<<((clunk::Sample *)lpDSB)->name<<std::endl;
		/* SDL_Mixer version
		Mix_FreeChunk((Mix_Chunk *)lpDSB);*/
		//clunk_object->cancel(((clunk::Sample *)lpDSB)->name, 0);
		delete (clunk::Sample *)lpDSB;
	}
}

void SoundLoad(char *filename, void **lpDSB)
{
	/* SDL_Mixer version
	Mix_Chunk *chunk=Mix_LoadWAV(filename);
	if(chunk==NULL)
		{
		std::string tmp;
		tmp="Error load sound: ";
		tmp+=filename;
		ErrH.Log(tmp.c_str());
		}
	*/
	//std::cout<<"SoundLoad:"<<filename<<std::endl;
	clunk::Sample *chunk;
	try {
		chunk = context.create_sample();
		chunk->load(filename);
	} catch(const std::exception & e)
		{ 
		std::string tmp;
		tmp="Error load sound: ";
		tmp+=filename;
		ErrH.Log(tmp.c_str());
		delete chunk;
		chunk = NULL;
		}
	(*lpDSB) = (void *)chunk;
}

void SetVolume(void *lpDSB, int volume)
{
	/* SDL_Mixer Version
	volume+=10000;
	volume*=0.0128;*/
	/* OLD system
	float f_volume = volume + 10000;
	f_volume *= 0.0001;*/
	//std::cout<<"SetVolume lpDSB f_volume:"<<volume/256.0<<" volume:"<<volume<<std::endl;
	
	((clunk::Sample *)lpDSB)->gain=volume/256.0;
	
	/* SDL_Mixer Version */
	/*if(XSoundInitFlag){
		
		Mix_VolumeChunk((Mix_Chunk *)lpDSB, volume);
	}*/
}

int GetVolume(void *lpDSB)
{
	int ret = 0;
	if(XSoundInitFlag){
		/* SDL_Mixer Version
		ret = Mix_VolumeChunk((Mix_Chunk *)lpDSB, 0);
		Mix_VolumeChunk((Mix_Chunk *)lpDSB, ret);*/
		ret = ((clunk::Sample *)lpDSB)->gain*256;
	}
	return ret;
}

void ChannelFinished(int channel)
{
	channels[channel].sound=NULL;
}

int SoundInit(int maxHZ, int schannels)
{
	//Only one freq, now sound more powerfull.	
	/* SDL_Mixer version
	if (SDL_Init (SDL_INIT_AUDIO) == -1)
	{
		std::cout<<"SDL_init: "<<Mix_GetError()<<std::endl;
		ErrH.Abort("SDL_init:trabl!");
		return false;
	}

	if (Mix_OpenAudio (maxHZ, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		std::cout<<"Mix_OpenAudio: "<<Mix_GetError()<<std::endl;
		ErrH.Abort(Mix_GetError());
		return false;
	}

	Mix_AllocateChannels(MAX_CHANNELS); */
	//std::cout<<"SoundInit maxHZ:"<<maxHZ<<std::endl;
	context.init(maxHZ, 2, 512);
	g_freq = maxHZ;
	context.set_max_sources(MAX_CHANNELS);
	clunk_object = context.create_object();
	int i;
	for(i = 0; i < MAX_CHANNELS; ++i)
	{
		/* SDL_Mixer version
		Mix_Volume(i, MIX_MAX_VOLUME); */
		channels[i].sound = NULL;
		channels[i].priority = channels[i].flags = channels[i].cropos = channels[i].pan = 0;
	}
	/* SDL_Mixer version
	Mix_ChannelFinished(&ChannelFinished);*/
	XSoundInitFlag = 1;
	//ErrH.Abort("SoundInit OK!!!");
	return true;
}

void SoundFinit(void)
{
	int i;
	if(XSoundInitFlag){
		for(i = 0; i < MAX_CHANNELS; ++i)
			SoundStop(i);
		/* SDL_Mixer version
		Mix_CloseAudio();*/
		delete clunk_object;
		context.deinit();
		XSoundInitFlag = 0;
	}
}


