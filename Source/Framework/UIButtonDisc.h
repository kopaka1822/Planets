#pragma once
#include "UIButton.h"

class UIButtonDisc : public UIButton
{
public:
	UIButtonDisc(Color col)
		:
		c(col)
	{}
	virtual void draw(Drawing& draw) override
	{
		UIButton::draw(draw);

		//draw geometry
		const PointF center = getRect().Midpoint();
		float r = std::min(m_dim.x, m_dim.y);
		r = r / 2.0f;
		r = r -2.0f * border - 4.0f; // some padding + border

		draw.DrawDisc(center, r, c);
	}

private:
	Color c;
};