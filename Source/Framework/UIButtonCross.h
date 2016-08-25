#pragma once
#include "UIButton.h"

class UIButtonCross : public UIButton
{
public:
	UIButtonCross()
	{}
	virtual void draw(Drawing& draw) override
	{
		UIButton::draw(draw);

		const PointF off(2.0f, 2.0f);
		draw.DrawLine(m_pos + off, m_pos + m_dim - off, Color::White(), 2.0f);
		draw.DrawLine(m_pos + PointF(m_dim.x, 0.0f) + PointF(-2.0f, 2.0f), m_pos + PointF(0.0f, m_dim.y) + PointF(2.0f, -2.0f), Color::White(), 2.0f);
	}

};