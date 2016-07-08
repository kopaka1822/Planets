#pragma once
#include "UIButton.h"

class UIButtonRect : public UIButton
{
public:
	UIButtonRect(Color col)
		:
		c(col)
	{}
	virtual void Draw(Drawing& draw) override
	{
		UIButton::Draw(draw);

		const PointF d(dim.x * 0.25f, dim.y * 0.25f);
		draw.DrawRect(RectF(pos + d, pos + dim - d), c);
	}

private:
	Color c;
};