#pragma once
#include "UIButton.h"

class UIButtonPause : public UIButton
{
public:
	UIButtonPause()
		:
		UIButton()
	{}
	virtual void draw(Drawing& draw) override
	{
		UIButton::draw(draw);
		float dx = (m_dim.x - 2.0f * border) / 5.0f;
		
		Color c = !bDown ? Color::White() : Color::Black();

		//draw boxes
		draw.DrawRect(RectF(m_pos + PointF(border + dx, border + dx),
			m_pos + PointF(border + 2.0f * dx, m_dim.y - border - dx)), c);

		draw.DrawRect(RectF(m_pos + PointF(border + 3 * dx, border + dx),
			m_pos + m_dim + -PointF(border + dx, border + dx)), c);
	}
};