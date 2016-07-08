#pragma once
#include "UIButton.h"
#include "../Utility/Mutex.h"
#include "../Utility/LockGuard.h"

class UIButtonText : public UIButton
{
public:
	UIButtonText(const std::string& text, Font& font)
		:
		UIButton(),
		font(font),
		txt(text),
		col(Color::White())
	{
		AdjustToFont();
	}
	virtual void Draw(Drawing& draw) override
	{
		UIButton::Draw(draw);

		LockGuard g(muTxt);

		if (bDown)
		{
			font.SetColor(Color::Black());
			font.Text(txt, CalculateFontPos());
		}
		else
		{
			font.SetColor(col);
			font.Text(txt, CalculateFontPos());
		}
		g.Unlock();
	}
	void SetColor(Color c)
	{
		col = c;
	}
	void AdjustToFont()
	{
		PointF fmet = font.GetMetrics(txt);
		dim.x =	fmet.x + 2 * border + 2 * padding;
		dim.y = fmet.y + 2 * border + 2 * padding;
	}
	virtual void SetText(const std::string& t)
	{
		LockGuard g(muTxt);
		txt = t;
		g.Unlock();
	}
protected:
	PointF CalculateFontPos() const
	{
		PointF fmet = font.GetMetrics(txt);
		PointF p;
		p.x = dim.x - 2 * border - 2 * padding - fmet.x;
		p.x *= 0.5f;
		p.x += border + padding;
		p.y = dim.y - 2 * border - 2 * padding - fmet.y;
		p.y *= 0.5f;
		p.y += border + padding;

		return pos + p;
	}
protected:
	std::string txt;
	Mutex muTxt;
	Font& font;
	static const int padding = 4;
	Color col;
};