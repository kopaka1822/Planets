#pragma once
#include "UIButton.h"

class UIButtonDisc : public UIButton
{
public:
	UIButtonDisc(Color col)
		:
		c(col)
	{}
	virtual void Draw(Drawing& draw) override
	{
		UIButton::Draw(draw);

		//Draw geometry
		const PointF center = GetRect().Midpoint();
		float r = std::min(dim.x, dim.y);
		r = r / 2.0f;
		r = r -2.0f * border - 4.0f; // some padding + border

		draw.DrawDisc(center, r, c);
	}

private:
	Color c;
};