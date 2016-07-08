#pragma once

class Sound
{
	friend class ThemeMixer;
public:
	enum SoundID
	{
		S_WIN,
		S_LOSE,
		S_CAPTURE,
		S_DIE,
		S_EXPLOSION,
		S_SPAWN,
		S_HOVER,
		S_CLICK,
		S_HORNS,
		S_SUDDEN,
		S_UPGRADE,

		S_SIZE
	};

	static const int SAMPLERATE = 44100;
	static const int BITRATE = 16;
	static const int CHANNELS = 2;
public:
	//static void Init(float volMusic, float volSound);
	static bool InitStep(); // Stepwise initialization
	static void LoadFiles(float mVol, float sVol);
	static float GetFileLoadPercent();
	static void Shutdown();

	static void Effect(SoundID id);
	static void SetSoundVolume(float vol);
	static void SetMusicVolume(float vol);

	static void Theme(unsigned int lvl);
	static unsigned int ThemeMax();
	static unsigned int ThemeGetMask();
	static void ThemeSetMask(unsigned int m);
	static void ThemeSetFilter(unsigned int f);
	static unsigned int ThemeGetFilter();
private:
	static int GetNextFreeChannel();
};