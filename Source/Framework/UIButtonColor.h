#pragma once
#include "UIButton.h"

class UIButtonColor : public UIButton
{
public:
	UIButtonColor(Color c)
		:
		col(c)
	{}
	virtual void draw(Drawing& draw) override
	{
		if (bDown)
		{
			//draw inverted
			draw.DrawRect(getRect(), Color::White());
		}
		else
		{
			//dont invert
			draw.DrawBox(getRect(), (float)border, Color::White(),bHover? col.Brightness(0.9f) : col);
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