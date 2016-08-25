#pragma once
#include "UIButton.h"

class UIButtonRect : public UIButton
{
public:
	UIButtonRect(Color col)
		:
		c(col)
	{}
	virtual void draw(Drawing& draw) override
	{
		UIButton::draw(draw);

		const PointF d(m_dim.x * 0.25f, m_dim.y * 0.25f);
		draw.DrawRect(RectF(m_pos + d, m_pos + m_dim - d), c);
	}

private:
	Color c;
};