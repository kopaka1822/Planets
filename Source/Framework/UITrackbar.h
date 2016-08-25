#pragma once

#include "UIObject.h"
#include "../Utility/Tools.h"

class UITrackbar : public UIObject
{
public:
	UITrackbar()
	{

	}

	virtual void draw(Drawing& draw) override
	{
		draw.DrawRect(RectF(m_pos + PointF(0, m_dim.y / 3.0f), m_pos + m_dim + PointF(0, -m_dim.y / 3)), Color::White());
		draw.DrawRect(RectF(m_pos + PointF(value*(m_dim.x - width), 0), m_pos + PointF(value*(m_dim.x-width) + width, m_dim.y)), Color::White());
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
		if (getRect().PointInside(mpos))
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
			float temp = (mpos.x - width / 2 - m_pos.x) / (m_dim.x - width);
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
	virtual void setMetrics(const PointF& d) override
	{
		UIObject::setMetrics(d);
		width = m_dim.y / 4.0f;
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