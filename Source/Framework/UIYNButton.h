#pragma once

#include "UIButton.h"

class UIYNButton : public UIButton
{
public:
	UIYNButton(bool State)
		:
		bClicked(State)
	{
		SetMetrics({ 50, 50 });
	}
	virtual void Draw(Drawing& draw) override
	{
		UIButton::Draw(draw);

		if (bClicked)
			draw.DrawCircle(pos + dim / 2.0f, dim.x / 3.3f, Color::Green(), 6.0f);
		else
		{
			float b = GetBorder() * 2;
			draw.DrawLine(pos + PointF(b, b), pos + dim - PointF(b, b), Color::Red(), 3);
			draw.DrawLine(pos + PointF(dim.x - b, b), pos + PointF(b, dim.y - b), Color::Red(), 3);
		}
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
	{
		if (bHover && bDown && k == Input::Left)
		{
			bClick = true;
			Sound::Effect(Sound::S_CLICK);
			bClicked = !bClicked;
		}
		bDown = false;
	}
	bool GetState() const
	{
		return bClicked;
	}
	void SetState(bool s)
	{
		bClicked = s;
	}
private:
	bool bClicked;
};