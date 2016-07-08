#pragma once
#include "UIButton.h"

class UIButtonCross : public UIButton
{
public:
	UIButtonCross()
	{}
	virtual void Draw(Drawing& draw) override
	{
		UIButton::Draw(draw);

		const PointF off(2.0f, 2.0f);
		draw.DrawLine(pos + off, pos + dim - off, Color::White(), 2.0f);
		draw.DrawLine(pos + PointF(dim.x, 0.0f) + PointF(-2.0f, 2.0f), pos + PointF(0.0f, dim.y) + PointF(2.0f, -2.0f), Color::White(), 2.0f);
	}

};