#pragma once
#include "UILabel.h"
#include "../Utility/Mutex.h"
#include <algorithm>

class UICountdownClock : public UILabel
{
public:
	UICountdownClock(Font& font)
		:
		UILabel("00",font)
	{}
	virtual void draw(Drawing& draw)
	{
		float time = std::max(0.0f, tmax - timer.GetTimeSecond());
		time = std::min(time, 98.9f);
		int itime = int(time + 0.5f);

		char buffer[32];
		sprintf(buffer, "%02d", itime);
		txt = buffer;

		UILabel::draw(draw);
	}
	void SetTimer(float maxTime)
	{
		timer.StartWatch();
		tmax = maxTime;
	}
protected:
	//cMutex muTxt;
	Timer timer;
	float tmax = 0.0f;
};
