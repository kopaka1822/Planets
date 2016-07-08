#pragma once
#include "../Utility/Timer.h"

class NetClock
{
public:
	NetClock()
	{}
	NetClock(float startsec)
	{
		SetStartTime(startsec);
	}
	void SetStartTime(float startsec)
	{
		t.StartWatch();
		startOffset = startsec;
	}
	float GetTimeSecond() const
	{
		return t.GetTimeSecond() + startOffset;
	}
	float MakeLapSecond()
	{
		float total = GetTimeSecond();
		float dt = total - lastLap;
		lastLap = total;
		return dt;
	}
	// curTime in seconds - ping in ms
	void SynchClock(float curTime, float ping)
	{
		float updTime = curTime + ping / 1000.0f;
		if (updTime > t.GetTimeSecond())
		{
			SetStartTime(updTime);
		}
	}
	float GetTimeDelta(float serverTime) const
	{
		// client time  - server time
		return lastLap - serverTime;
	}
private:
	Timer t;
	float lastLap = 0.0f;
	float startOffset = 0.0f;
};