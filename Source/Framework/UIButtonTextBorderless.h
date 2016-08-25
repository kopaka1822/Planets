#pragma once
#include "UIButton.h"

class UIButtonTextBorderless : public UIButton
{
public:
	UIButtonTextBorderless(const std::string& txt, Font& fon)
		:
		txt(txt),
		font(fon)
	{
		AdjustToFont();
	}
	virtual void draw(Drawing& draw) override
	{
		if (bDown)
		{
			font.SetColor(Color::Black());
			draw.DrawRect(getRect(), Color::White());
		}
		else
		{
			font.SetColor(Color::White());
			Color col = bActive ? Color::Blue() : Color::Black();
			
			draw.DrawRect(getRect(), col);
		}
		font.Text(txt, m_pos + PointF(padding, padding));
	}
	void SetActive(bool b)
	{
		bActive = b;
	}
	bool isActive() const
	{
		return bActive;
	}
private:
	void AdjustToFont()
	{
		setMetrics(PointF(txt.length() * font.GetFontWidth() + 2 * padding,
			font.GetFontHeight() + 2 * padding));
	}
private:
	std::string txt;
	Font& font;
	bool bActive = false;

	static const int padding = 4;
};