#pragma once

#include "UIObject.h"
#include "../Utility/Tools.h"

class UITrackbar : public UIObject
{
public:
	UITrackbar()
	{

	}

	virtual void Draw(Drawing& draw) override
	{
		draw.DrawRect(RectF(pos + PointF(0, dim.y / 3.0f), pos + dim + PointF(0, -dim.y / 3)), Color::White());
		draw.DrawRect(RectF(pos + PointF(value*(dim.x - width), 0), pos + PointF(value*(dim.x-width) + width, dim.y)), Color::White());
	}

	float GetValue() const
	{
		return value;
	}

	void SetValue(float NewValue)
	{
		SetValueNoChange(NewValue);
		bChanged = true;
	}
	void SetValueNoChange(float val)
	{
		value = tool::clamp(val, 0.0f, 1.0f);
	}
	virtual void Event_MouseMove(const PointF& mpos) override
	{
		if (GetRect().PointInside(mpos))
		{
			if (!bHover)
			{
				//Sound::Effect(Sound::S_HOVER);
				bHover = true;
			}
		}
		else
		{
			bHover = false;
		}
		if (bDown)
		{
			float temp = (mpos.x - width / 2 - pos.x) / (dim.x - width);
			SetValue(temp);
		}
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		if (bHover && k == Input::Left)
		{
			bDown = true;
		}
		Event_MouseMove(pos);
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
	{
		if (bDown)		
			bReleased = true;

		bDown = false;
	}
	virtual void SetMetrics(const PointF& d) override
	{
		UIObject::SetMetrics(d);
		width = dim.y / 4.0f;
	}
	bool Changed()
	{
		bool temp = bChanged;
		bChanged = false;
		return temp;
	}
	bool Released()
	{
		bool temp = bReleased;
		bReleased = false;
		return temp;
	}
private:
	float value = 1.0f;
	float width;
	bool bHover = false;
	bool bDown = false;
	bool bChanged = false;
	bool bReleased = false;
};