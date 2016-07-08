#pragma once
#include "UIButton.h"

class UIButtonColor : public UIButton
{
public:
	UIButtonColor(Color c)
		:
		col(c)
	{}
	virtual void Draw(Drawing& draw) override
	{
		if (bDown)
		{
			//draw inverted
			draw.DrawRect(GetRect(), Color::White());
		}
		else
		{
			//dont invert
			draw.DrawBox(GetRect(), (float)border, Color::White(),bHover? col.Brightness(0.9f) : col);
		}
	}
	void SetColor(Color c)
	{
		col = c;
	}
	Color GetColor() const
	{
		return col;
	}
private:
	Color col;
};