#pragma once
#include <string>
#include "../SDL/SDL_mixer.h"
#include "WaveFile.h"

class MusicFile
{
public:
	MusicFile(const std::string& filename, float vol);
	MusicFile(const WaveFile& wave, float vol);
	~MusicFile();

	void Play(int channel, bool repeat);
	void Stop();
	bool isPlaying() const;
	void SetVolume(float vol);
	void SetMute(bool mute);
	int GetChannel() const;

private:
	void SetVolumeNoSave(int vol);
private:
	bool bMuted = false;
	Mix_Chunk* pMusic = nullptr;
	int curVol;
	int channel = -1;

	bool bPaused = false;
};
