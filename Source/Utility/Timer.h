#pragma once
#include "../SDL/SDL.h"
#include "../SDL/SDL_timer.h"
#include "SDL_Exception.h"

class Timer
{
public:
	Timer()
	{
		// static init
		Init();
	}
	void StartWatch()
	{
		running = true;
		startCount = SDL_GetPerformanceCounter();
	}
	void StopWatch()
	{
		if (running)
		{
			curCount = SDL_GetPerformanceCounter();
			running = false;
		}
	}
	bool WatchRunning() const
	{
		return running;
	}
	float GetTimeMilli() const
	{
		if (running)
		{
			Uint64 cur = SDL_GetPerformanceCounter();
			return (float)(cur - startCount) * invFreqMilli;
		}
		else
		{
			return (float)(curCount - startCount) * invFreqMilli;
		}
	}
	float GetTimeSecond() const
	{
		return GetTimeMilli() / 1000.0f;
	}
	static Uint64 GetTimestamp()
	{
		return SDL_GetPerformanceCounter();
	}
	static float ConvertMilli(const Uint64& val1, const Uint64& val2)
	{
		return float(val2 - val1) * invFreqMilli;
	}
	static float ConvertMilli(const Uint64& val)
	{
		return ConvertMilli(val, SDL_GetPerformanceCounter());
	}
private:
	static void Init()
	{
		static bool bInit = false;
		if (!bInit)
		{
			if (SDL_Init(SDL_INIT_TIMER) != 0)
				throw SDL_Exception("SDL_INIT_TIMER failed");

			Uint64 freq = SDL_GetPerformanceFrequency();
			invFreqMilli = 1.0f / float(double(freq) / 1000.0);

			bInit = true;
		}
	}
private:
	static float invFreqMilli;

	Uint64 startCount;
	Uint64 curCount;
	bool running = false;
};
