#pragma once

#include "UIButton.h"

class UIYNButton : public UIButton
{
public:
	UIYNButton(bool State)
		:
		bClicked(State)
	{
		setMetrics({ 50, 50 });
	}
	virtual void draw(Drawing& draw) override
	{
		UIButton::draw(draw);

		if (bClicked)
			draw.DrawCircle(m_pos + m_dim / 2.0f, m_dim.x / 3.3f, Color::Green(), 6.0f);
		else
		{
			float b = GetBorder() * 2;
			draw.DrawLine(m_pos + PointF(b, b), m_pos + m_dim - PointF(b, b), Color::Red(), 3);
			draw.DrawLine(m_pos + PointF(m_dim.x - b, b), m_pos + PointF(b, m_dim.y - b), Color::Red(), 3);
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