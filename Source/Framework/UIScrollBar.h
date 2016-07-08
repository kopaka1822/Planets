#pragma once
#include "UIButton.h"

class UIScrollBar : public UIObject
{
public:
	UIScrollBar()
	{}
	virtual ~UIScrollBar()
	{}
	void Init(float campos, float displaywidth, float height)
	{
		curTop = campos;
		curWidth = displaywidth;
		maxCam = height;
	}
	virtual void Draw(Drawing& draw) override
	{
		draw.DrawBox(GetRect(), (float)border, Color::White(), Color::Black());

		float totalwidth = dim.y - 2.0f * border;
		float yStart = curTop / maxCam * totalwidth;
		float yEnd = (curTop + curWidth) / maxCam * totalwidth;

		PointF start = pos + PointF(border, border);
		draw.DrawRect(RectF(start + PointF(0.0f, yStart), start + PointF(dim.x - 2.0f * border, yEnd)), Color::Gray());
	}
	void SetCam(float cam)
	{
		curTop = cam;
	}

	virtual void Event_MouseMove(const PointF& pos) override
	{
		bHover = GetRect().PointInside(pos);
		if (bDown)
		{
			dy = -(pos.y - lastPos.y);
			lastPos = pos;
		}
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& pos) override
	{
		if (k == Input::Left && bHover)
		{
			bDown = true;
			lastPos = pos;
		}
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override
	{
		bDown = false;
	}
	float GetDelta()
	{
		float d = dy;
		dy = 0.0f;
		return d;
	}
private:
	float curTop;
	//float curBottm;
	float curWidth;
	float maxCam;
	static const int border = 2;

	bool bDown = false;
	bool bHover = false;
	PointF lastPos;

	float dy = 0.0f;
};