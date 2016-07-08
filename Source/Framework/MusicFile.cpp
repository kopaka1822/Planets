#include "MusicFile.h"
#include "../Utility/MIX_Exception.h"
#include <assert.h>

MusicFile::MusicFile(const std::string& filename, float vol)
{
	pMusic = Mix_LoadWAV(filename.c_str());
	if (!pMusic)
		throw MIX_Exception("music file error - " + filename);

	SetVolume(vol);
}
MusicFile::MusicFile(const WaveFile& wave, float vol)
{
	/*pMusic = (Mix_Chunk*)malloc(sizeof(Mix_Chunk));
	pMusic->*/
	pMusic = Mix_QuickLoad_RAW(wave.GetData().get(), wave.GetHeader().dataSize);

	SetVolume(vol);
}
MusicFile::~MusicFile()
{
	if (pMusic)
	{
		Stop();

		Mix_FreeChunk(pMusic);
		pMusic = nullptr;
	}
}

void MusicFile::Play(int chan, bool repeat)
{
	if (repeat)
	{
		channel = Mix_PlayChannel(chan, pMusic, -1);
	}
	else
	{
		channel = Mix_PlayChannel(chan, pMusic, 0);
	}
	assert(channel == chan);
}
//void MusicFile::Pause()
//{
//	if (channel != -1)
//	{
//		Mix_Pause(channel);
//	}
//}
void MusicFile::Stop()
{
	if (channel != -1)
	{
		Mix_HaltChannel(channel);
		channel = -1;
	}
}
int MusicFile::GetChannel() const
{
	assert(channel != -1);
	return channel;
}
bool MusicFile::isPlaying() const
{
	if (channel == -1)
		return false;
	else
	{
		int playing = Mix_Playing(channel);
		int paused = Mix_Paused(channel);

		if (!playing || paused)
			return false;
		else
			return true;
	}
}
void MusicFile::SetVolume(float vol)
{
	curVol = int(vol * float(MIX_MAX_VOLUME));
	SetVolumeNoSave(curVol);
}
void MusicFile::SetVolumeNoSave(int vol)
{
	Mix_VolumeChunk(pMusic, vol);

	if (channel != -1)
	{
		Mix_Volume(channel, vol);
	}
}
void MusicFile::SetMute(bool mute)
{
	bMuted = mute;
	if (bMuted)
	{
		SetVolumeNoSave(0);
	}
	else
	{
		SetVolumeNoSave(curVol);
	}
}