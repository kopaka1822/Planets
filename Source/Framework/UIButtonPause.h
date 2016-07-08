#pragma once
#include "UIButton.h"

class UIButtonPause : public UIButton
{
public:
	UIButtonPause()
		:
		UIButton()
	{}
	virtual void Draw(Drawing& draw) override
	{
		UIButton::Draw(draw);
		float dx = (dim.x - 2.0f * border) / 5.0f;
		
		Color c = !bDown ? Color::White() : Color::Black();

		//draw boxes
		draw.DrawRect(RectF(pos + PointF(border + dx, border + dx),
			pos + PointF(border + 2.0f * dx, dim.y - border - dx)), c);

		draw.DrawRect(RectF(pos + PointF(border + 3 * dx, border + dx),
			pos + dim + -PointF(border + dx, border + dx)), c);
	}
};