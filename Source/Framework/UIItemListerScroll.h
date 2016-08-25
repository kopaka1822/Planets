#pragma once
#include "UIItemLister.h"

class UIItemListerScroll : public UIItemLister
{
public:
	UIItemListerScroll(bool drawLines)
		:
		UIItemLister(drawLines)
	{

	}
	virtual void Event_MouseWheel(float amount, const PointF& pos) override
	{
		if (bHover)
			UIItemLister::MoveCam(-(float)amount * 20.0f);
	}
	virtual void Event_MouseMove(const PointF& pos) override
	{
		UIItemLister::Event_MouseMove(pos);
		float dy = sbar.GetDelta();

		bHover = getRect().PointInside(pos);
		if (bDown)
		{
			if (dy == 0.0f)
			{
				dy = pos.y - lastPos.y;
				if (fabs(dy) <= 4.0f * fabs(pos.x - lastPos.x)) // dy <= 4 * dx
					dy = 0.0f;
			}
			lastPos = pos;
			UIItemLister::MoveCam(-dy);
		}
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		UIItemLister::Event_MouseDown(k, pos);
		if (k == Input::Left && bHover)// && RectF(this->pos, this->m_pos + PointF(dim.x * midpoint, dim.y)).PointInside(m_pos))
		{
			bDown = true;
			lastPos = pos;
		}
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
	{
		UIItemLister::Event_MouseUp(k, pos);
		bDown = false;
	}
protected:
	bool bHover = false;
	bool bDown = false;
	PointF lastPos;
};