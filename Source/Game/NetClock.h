#pragma once
#include "../Utility/Timer.h"

class NetClock
{
public:
	NetClock()
	{}
	NetClock(float startsec)
	{
		setStartTime(startsec);
	}
	void setStartTime(float startsec)
	{
		m_t.StartWatch();
		m_startOffset = startsec;
	}
	float getTimeSecond() const
	{
		return m_t.GetTimeSecond() + m_startOffset;
	}
	float makeLapSecond()
	{
		float total = getTimeSecond();
		float dt = total - m_lastLap;
		m_lastLap = total;
		return dt;
	}
	// curTime in seconds - ping in ms
	void synchClock(float curTime, float ping)
	{
		float updTime = curTime + ping / 1000.0f;
		if (updTime > m_t.GetTimeSecond())
		{
			setStartTime(updTime);
		}
	}
	float getTimeDelta(float serverTime) const
	{
		// client time  - server time
		return m_lastLap - serverTime;
	}
private:
	Timer m_t;
	float m_lastLap = 0.0f;
	float m_startOffset = 0.0f;
};