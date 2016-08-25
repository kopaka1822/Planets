#pragma once
#include "UIObject.h"

class UITooltip : public UIObject
{
public:
	UITooltip(UIObject& obj, Font& font)
		:
		obj(obj),
		font(font)
	{
		CalcMetrics();
	}
	void AddLine(const std::string& str)
	{
		info.push_back(str);
		CalcMetrics();
	}
	virtual void Event_MouseMove(const PointF& pos)
	{
		msLastPos = pos;

		if (obj.getRect().PointInside(pos))
		{
			float xStart = pos.x;
			float yStart = pos.y - m_dim.y;

			float xEnd = xStart + m_dim.x;
			if (xEnd > Framework::DrawStart().x + Framework::DrawWidth().x)
				xStart -= m_dim.x;

			float yEnd = pos.y;

			if (yStart < Framework::DrawStart().y)
				yStart = pos.y;

			setOrigin({ xStart, yStart });
			objLastPos = obj.getOrigin();
			bHover = true;
		}
		else
			bHover = false;
	}
	virtual void draw(Drawing& draw) override
	{
		if (!isEnabled())
			return;

		if (obj.getOrigin() != objLastPos || msLastPos != Input::GetMouseXY())
		{
			Event_MouseMove(Input::GetMouseXY());
		}

		if (obj.isEnabled() && bHover)
		{
			draw.DrawBox(getRect(), (float)border, Color::White(), Color::Black());

			const float x = m_pos.x + border + padding;
			float y = m_pos.y + border + padding;

			font.SetColor(Color::White());
			for (const auto& s : info)
			{
				font.Text(s, { x, y });
				y += font.GetFontHeight() + padding;
			}
		}
	}
private:
	void CalcMetrics()
	{
		float hei = (float)(2 * border + padding);
		unsigned int maxlen = 0;

		for (const auto& s : info)
		{
			hei += font.GetFontHeight() + padding;
			maxlen = std::max((size_t)maxlen, s.length());
		}

		float wi = font.GetFontWidth() * maxlen;
		wi += float(2 * border + 2 * padding);

		setMetrics({ wi, hei });
	}
private:
	UIObject& obj;
	Font& font;
	std::vector< std::string > info;

	static const int border = 6;
	static const int padding = 4;

	bool bHover = false;

	PointF objLastPos;
	PointF msLastPos;
};
