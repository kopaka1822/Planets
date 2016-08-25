#pragma once
#include "UIObject.h"
#include "../Utility/Tools.h"

class UIProgressBar : public UIObject
{
public:
	UIProgressBar()
	{}
	void SetProgress(float percent)
	{
		perc = tool::clamp(percent, 0.0f, 1.0f);
	}
	virtual void draw(Drawing& draw) override
	{
		drawPerc = perc * 0.2f + drawPerc * 0.8f;

		// white box
		draw.DrawRect(getRect(), Color::White());

		// black free area
		const float boxWi = (m_dim.x - float(2 * border)) * (1.0f - drawPerc);
		const float boxHi = m_dim.y - float(2 * border);

		RectF rect;
		rect.p1 = m_pos + m_dim - PointF(border + boxWi, border + boxHi);
		rect.p2 = m_pos + m_dim - PointF(border, border);

		draw.DrawRect(rect, Color::Black());
	}
private:
	float perc = 0.0f;
	float drawPerc = 0.0f;
	static const int border = 8;
};