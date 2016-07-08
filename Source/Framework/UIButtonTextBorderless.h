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
	virtual void Draw(Drawing& draw) override
	{
		if (bDown)
		{
			font.SetColor(Color::Black());
			draw.DrawRect(GetRect(), Color::White());
		}
		else
		{
			font.SetColor(Color::White());
			Color col = bActive ? Color::Blue() : Color::Black();
			
			draw.DrawRect(GetRect(), col);
		}
		font.Text(txt, pos + PointF(padding, padding));
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
		SetMetrics(PointF(txt.length() * font.GetFontWidth() + 2 * padding,
			font.GetFontHeight() + 2 * padding));
	}
private:
	std::string txt;
	Font& font;
	bool bActive = false;

	static const int padding = 4;
};