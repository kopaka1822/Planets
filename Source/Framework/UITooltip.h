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

		if (obj.GetRect().PointInside(pos))
		{
			float xStart = pos.x;
			float yStart = pos.y - dim.y;

			float xEnd = xStart + dim.x;
			if (xEnd > Framework::DrawStart().x + Framework::DrawWidth().x)
				xStart -= dim.x;

			float yEnd = pos.y;

			if (yStart < Framework::DrawStart().y)
				yStart = pos.y;

			SetOrigin({ xStart, yStart });
			objLastPos = obj.GetOrigin();
			bHover = true;
		}
		else
			bHover = false;
	}
	virtual void Draw(Drawing& draw) override
	{
		if (!isEnabled())
			return;

		if (obj.GetOrigin() != objLastPos || msLastPos != Input::GetMouseXY())
		{
			Event_MouseMove(Input::GetMouseXY());
		}

		if (obj.isEnabled() && bHover)
		{
			draw.DrawBox(GetRect(), (float)border, Color::White(), Color::Black());

			const float x = pos.x + border + padding;
			float y = pos.y + border + padding;

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

		SetMetrics({ wi, hei });
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
