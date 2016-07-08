#pragma once
#include "MusicFile.h"
#include <deque>
#include "../Utility/LockGuard.h"

class SoundEffect
{
public:
	SoundEffect(const std::string& filename, float vol);
	~SoundEffect();
	void Play(int channel);
	void Stop();
	void SetVolume(float vol);
	int CollectGarbage(int channel); // deletes finished sounds
private:
	float curVol;
	//std::queue< std::unique_ptr< MusicFile >> pSounds;
	std::deque< std::unique_ptr< MusicFile >> pSounds;

	bool bStopping = false;
	WaveFile wave;
	Mutex muSounds;
};