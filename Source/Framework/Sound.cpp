#include "Sound.h"
#include "../SDL/SDL.h"
#include "../SDL/SDL_mixer.h"
#include "../Utility/SDL_Exception.h"
#include "../System/Log.h"
#include <assert.h>
#include "MusicFile.h"
#include <memory>
#include <vector>
#include "ThemeMixer.h"
#include "SoundEffect.h"
#include "../Utility/Mutex.h"
#include "../Utility/LockGuard.h"
#include "Settings.h"
#include "../Utility/Exception.h"

static bool bInit = false;

static std::unique_ptr< SoundEffect > SoundList[Sound::S_SIZE];
static std::vector< std::unique_ptr< MusicFile > > FileList; // Theme files
static std::unique_ptr< ThemeMixer > pMixer;

static float soundVolume;

static int filesLoad = 0;
static const int nFiles = 8; // 7 = theme + sounds

static std::vector< int > playingChannels; // int because atomic operations
// this will be called when a channel finishes playback
static void SoundCallback(int channel)
{
	if (bInit)
	{
		try
		{
			assert(channel < playingChannels.size());
			assert(playingChannels[channel]);
			// delete this channel
			int count = 0;

			static Mutex muSound; // not sure if this can be called twice at the same time
			LockGuard g(muSound);

			for (auto& s : SoundList)
				if (s)
					count += s->CollectGarbage(channel);

			g.Unlock();
			assert(count == 1);
			playingChannels[channel] = 0;
			// channel freed
		}
		catch (const std::exception& e)
		{
			Log::Warning("Exception in sound callback!");
			Log::Warning(e.what());
#if _DEBUG
			Log::Error(e);
#endif
		}
		catch (...)
		{
			Log::Error("unexpected sound error");
		}
	}
}

// returns -1 if no channel is free
int Sound::GetNextFreeChannel()
{
	static int curChannel = 0;
	int startChan = curChannel;
	do
	{
		if (playingChannels[curChannel] == 0)
		{
			playingChannels[curChannel] = 1; // use this channel
			return curChannel;
		}
		curChannel = (curChannel + 1) % playingChannels.size();
	} while (startChan != curChannel);
	return -1;
}

float Sound::GetFileLoadPercent()
{
	return float(filesLoad) / float(nFiles);
}

void Sound::LoadFiles(float mVol, float sVol)
{
	soundVolume = sVol;
	// Music Files
	FileList.clear();
	Log::Write("loading music files");

	filesLoad = 0;
	FileList.push_back(std::unique_ptr<MusicFile>(new MusicFile("data/msc/theme01.ogg", mVol)));
	filesLoad++;
	FileList.push_back(std::unique_ptr<MusicFile>(new MusicFile("data/msc/theme02.ogg", mVol)));
	filesLoad++;
	FileList.push_back(std::unique_ptr<MusicFile>(new MusicFile("data/msc/theme03.ogg", mVol)));
	filesLoad++;
	FileList.push_back(std::unique_ptr<MusicFile>(new MusicFile("data/msc/theme04.ogg", mVol)));
	filesLoad++;
	FileList.push_back(std::unique_ptr<MusicFile>(new MusicFile("data/msc/theme05.ogg", mVol)));
	filesLoad++;
	FileList.push_back(std::unique_ptr<MusicFile>(new MusicFile("data/msc/theme06.ogg", mVol)));
	filesLoad++;
	FileList.push_back(std::unique_ptr<MusicFile>(new MusicFile("data/msc/theme07.ogg", mVol)));
	filesLoad++;

	Log::Write("creating theme mixer");
	pMixer = std::unique_ptr<ThemeMixer>(new ThemeMixer(FileList));

	//Sound Effects
	Log::Write("loading sound effects");

	SoundList[Sound::S_WIN] = std::unique_ptr<SoundEffect>(new SoundEffect("data/sfx/win.wav", sVol));
	SoundList[Sound::S_LOSE] = std::unique_ptr<SoundEffect>(new SoundEffect("data/sfx/los.wav", sVol));
	SoundList[Sound::S_CAPTURE] = std::unique_ptr<SoundEffect>(new SoundEffect("data/sfx/cap.wav", sVol));
	SoundList[Sound::S_DIE] = std::unique_ptr<SoundEffect>(new SoundEffect("data/sfx/die.wav", sVol));
	SoundList[Sound::S_EXPLOSION] = std::unique_ptr<SoundEffect>(new SoundEffect("data/sfx/expl.wav", sVol));
	SoundList[Sound::S_SPAWN] = std::unique_ptr<SoundEffect>(new SoundEffect("data/sfx/spawn.wav", sVol));
	SoundList[Sound::S_HOVER] = std::unique_ptr<SoundEffect>(new SoundEffect("data/sfx/hover.wav", sVol));
	SoundList[Sound::S_CLICK] = std::unique_ptr<SoundEffect>(new SoundEffect("data/sfx/click.wav", sVol));
	SoundList[Sound::S_HORNS] = std::unique_ptr<SoundEffect>(new SoundEffect("data/sfx/horns.wav", sVol));
	SoundList[Sound::S_SUDDEN] = std::unique_ptr<SoundEffect>(new SoundEffect("data/sfx/sudden.wav", sVol));
	SoundList[Sound::S_UPGRADE] = std::unique_ptr<SoundEffect>(new SoundEffect("data/sfx/upgrade.wav", sVol));
	filesLoad++;

	bInit = true;
}
bool Sound::InitStep()
{
	static int lvl = 0;
	switch (lvl++)
	{
	case 0:
		assert(!bInit);
		Log::Write("initializing sound system");
		if (Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG)
			throw SDL_Exception("init ogg");
		return false;

	case 1:
		Log::Write("creating primary mixer");
		if (Mix_OpenAudio(SAMPLERATE, MIX_DEFAULT_FORMAT, CHANNELS, 4096) != 0)
			throw SDL_Exception("open audio");
		return false;

	case 2:
	{
		int num = Mix_AllocateChannels(40);
		Log::Write(std::to_string(num) + " audio channels allocated");
		playingChannels.assign(num, 0);

		Mix_ChannelFinished(SoundCallback);
		return true;
	}

	/*case 3:
		LoadFiles(Settings::GetVolMusic(), Settings::GetVolSound());
		return true;*/
	}

	throw Exception("invalid sound init state");
	return true;
}
void Sound::Shutdown()
{
	bInit = false;

	Log::Write("deleting music files");
	for (auto& m : FileList)
		m.reset();

	Log::Write("deleting sound files");
	for (auto& s : SoundList)
		s.reset();

	Log::Write("closing audio");
	Mix_CloseAudio();

	Mix_Quit();
}
void Sound::Effect(SoundID id)
{
	if (bInit)
	{
		if (soundVolume > 0.0f)
		{
			assert(id < Sound::S_SIZE);

			SoundList[id]->Play(GetNextFreeChannel());
		}
	}
}
void Sound::SetSoundVolume(float vol)
{
	soundVolume = vol;
	for (auto& s : SoundList)
		if (s)
			s->SetVolume(vol);
}
void Sound::SetMusicVolume(float vol)
{
	for (auto& m : FileList)
		m->SetVolume(vol);
}

void Sound::Theme(unsigned int lvl)
{
	pMixer->Play(lvl);
}
unsigned int Sound::ThemeMax()
{
	return pMixer->MaxLvl();
}
unsigned int Sound::ThemeGetMask()
{
	return pMixer->GetMask();
}
void Sound::ThemeSetMask(unsigned int m)
{
	pMixer->SetMask(m);
}
void Sound::ThemeSetFilter(unsigned int f)
{
	pMixer->SetFilter(f);
}
unsigned int Sound::ThemeGetFilter()
{
	return pMixer->GetFilter();
}
