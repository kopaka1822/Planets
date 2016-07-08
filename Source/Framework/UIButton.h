#pragma once
#include "UIObject.h"

// border + hover + click
class UIButton : public UIObject
{
public:
	UIButton()
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
			draw.DrawBox(GetRect(), (float)border, Color::White(), bHover ? Color::Gray() : Color::Black());
		}
	}
	virtual ~UIButton(){}

	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		if (bHover && k == Input::Left)
		{
			bDown = true;
		}
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
	{
		if (bHover && bDown && k == Input::Left)
		{
			bClick = true;
			Sound::Effect(Sound::S_CLICK);
		}
		bDown = false;
	}
	virtual void Event_MouseMove(const PointF& pos) override
	{
		if (GetRect().PointInside(pos))
		{
			if (!bHover)
			{
				Sound::Effect(Sound::S_HOVER);
				bHover = true;
			}
		}
		else
		{
			bHover = false;
		}
	}
	bool Pushed() //return true when clicked
	{
		bool t = bClick;
		bClick = false;
		return t;
	}
	static float GetBorder()
	{
		return float(border);
	}

protected:
	
	static const int border = 6;
	bool bHover = false;
	bool bDown = false;
	bool bClick = false;
};