#pragma once
#include "UIObject.h"
#include "UIButtonText.h"

class UIColorSelect : public UIObject
{
public:
	UIColorSelect()
		:
		curCol(Color::Red()),
		btnOkay("OK",FONT_SMALL)
	{
		lowerPart = btnOkay.getMetrics().y + 2 * padding;
	}
	virtual ~UIColorSelect()
	{}
	virtual void draw(Drawing& draw) override
	{
		draw.DrawBox(getRect(), (float)border, Color::White(), Color::Black());

		// midpoint ?

		draw.DrawDiscHSV(m_pos + cCenter, radius);
		draw.DrawDisc(m_pos + cCenter, radius - padding, Color::Black());
		draw.DrawTriangleHSV(m_pos + cCenter, radius - padding, curCol);

		// draw Line
		PointF start(1.0f, 0.0f);
		start = start.rot(curAngle);

		draw.DrawDisc(trianglePos, 4.0f, Color::Black());

		draw.DrawLine(m_pos + cCenter + (start * (radius - padding)),
			m_pos + cCenter + (start * radius), Color::Black(), 3.0f);
		// draw a box with the selected color
		//draw.DrawBox(RectF(padding,padding,lowerPart - border - padding, lowerPart - border - padding) + 
			//PointF(border , dim.y - lowerPart) + pos, border, Color::White(), GetColor());
		PointF pp = PointF(1.5f * padding + border, 1.5f * padding + border + m_dim.y - lowerPart) + m_pos;
		draw.DrawParticle(pp, GetColor(),2.0f * padding);
		draw.DrawPlanet(pp + PointF(2.0f * padding, 0.0f), GetColor(), (float)padding);

		btnOkay.draw(draw);
	}
	virtual void Event_MouseDown(Input::MouseKey k,const PointF& p) override
	{
		if (!getRect().PointInside(p))
			this->disable();
		else if (k == Input::Left)
		{
			float dist = ((m_pos + cCenter) - p).length();
			if (dist <= radius && dist >= radius - padding)
			{
				bChangeWheel = true;
			}
			else if (inTri(p,false))
			{
				bChangeTri = true;
			}
			Event_MouseMove(p);
		}
	}
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& p) override
	{
		if (k == Input::Left)
		{
			bChangeWheel = false;
			bChangeTri = false;
		}
	}
	virtual void Event_MouseMove(const PointF& p) override
	{
		if (bChangeWheel)
		{
			if (p == cCenter + m_pos)
				return;
			// calc color
			PointF d = p - (m_pos + cCenter);
			float angle = atan2f(d.y, d.x);
			if (angle < 0.0f)
				angle += 2.0f * float(M_PI);

			curAngle = angle;

			curCol = Drawing::PickHSV(angle);
		}
		if (bChangeTri)
		{
			inTri(p, true); //recalculate a b c
		}
	}
	virtual void setMetrics(const PointF& m) override
	{
		UIObject::setMetrics(m);

		cCenter = PointF(m_dim.x / 2, m_dim.x / 2);
		radius = cCenter.x - border - padding;

		m_dim.y = m_dim.y + lowerPart;

		float r = radius - padding;
		triRig = (PointF(1.0f, 0.0f) * r) + cCenter;
		triTop = (PointF(-0.707f, -0.707f) * r) + cCenter;
		triBot = (PointF(-0.707f, 0.707f) * r) + cCenter;

		trianglePos = triRig + m_pos;
	}
	virtual void setOrigin(const PointF& p)
	{
		UIObject::setOrigin(p);

		btnOkay.setOrigin(m_pos + m_dim - btnOkay.getMetrics() - PointF(border + padding, border + padding));
		trianglePos = triRig + m_pos;
	}
	UIButtonText& GetButton()
	{
		return btnOkay;
	}
	virtual void disable() override
	{
		UIObject::disable();
		btnOkay.disable();
	}
	virtual void enable() override
	{
		UIObject::enable();
		btnOkay.enable();
	}
	Color GetColor() const
	{
		float r = curCol.r * curA + curB;
		float g = curCol.g * curA + curB;
		float b = curCol.b * curA + curB;
		Color c1(r, g, b, 1.0f);

		return c1;
	}

	void SetColor(Color c)
	{
		float r = c.r;
		float g = c.g;
		float b = c.b;

		float cmax = std::max(r, std::max(g, b));
		float cmin = std::min(r, std::min(g, b));

		float delta = cmax - cmin;
		float deg = float(M_PI) / 3.0f; // 60 degrees

		float h = 0.0f;
		if (cmax == r)
		{
			h = fmod((g - b) / delta, 6.0f) * deg;
		}
		else if (cmax == g)
		{
			h = ((b - r) / delta + 2) * deg;
		}
		else if (cmax == b)
		{
			h = ((r - g) / delta + 4) * deg;
		}
		curAngle = fmod(h, 2.0f * float(M_PI));
		if (curAngle < 0.0f)
			curAngle += 2.0f * float(M_PI);
		curCol = Drawing::PickHSV(curAngle);
		
		float s = 0.0f;
		if (cmax != 0.0f)
			s = delta / cmax;
		s = tool::clamp(s,0.0f, 1.0f);

		float v = cmax;

		// calc point
		PointF vStart = (triTop + (triBot - triTop) * v);
		PointF click = vStart + PointF((triRig.x - triTop.x) * s, 0.0f);
		inTri(click + m_pos, true);
		//curA = s;
		//curB = v;
		//curC = 1.0f - v;
	}
private:
	bool inTri(const PointF& p, bool setpos)
	{
		PointF p1 = triRig + m_pos;
		PointF p2 = triBot + m_pos;
		PointF p3 = triTop + m_pos;

		float denom = ((p2.y - p3.y)*(p1.x - p3.x) + (p3.x - p2.x)*(p1.y - p3.y));

		float a = ((p2.y - p3.y)*(p.x - p3.x) + (p3.x - p2.x)*(p.y - p3.y)) / denom;
		float b = ((p3.y - p1.y)*(p.x - p3.x) + (p1.x - p3.x)*(p.y - p3.y)) / denom;
		float c = 1 - a - b;

		if (a >= 0 && a <= 1 && b >= 0 && b <= 1 && c >= 0 && c <= 1)
		{
			curA = a;
			curB = b;
			curC = c;
			trianglePos = ((triRig + m_pos) * curA + (triBot + m_pos) * curB + (triTop + m_pos) * curC);
			return true;
		}
		else if (setpos)
		{
			curA = tool::clamp(a, 0.0f, 1.0f);
			curB = tool::clamp(b, 0.0f, 1.0f);
			//curC = clamp(c, 0.0f, 1.0f);
			//if (c < 0.0f)
				//curA = 0.0f;
			curC = tool::clamp(1.0f - curA - curB, 0.0f, 1.0f);
			//curC = c;
			if (c < 0.0f)
			{
				curC = 0.0f;
				curB = 1.0f - curA;
			}
			trianglePos = ((triRig + m_pos) * curA + (triBot + m_pos) * curB + (triTop + m_pos) * curC);
		}
		return false;
	}
private:
	static const int border = 6;
	static const int padding = 30;

	PointF cCenter;
	float radius;

	bool bChangeWheel = false;
	bool bChangeTri = false;
	Color curCol;

	UIButtonText btnOkay;

	float lowerPart;

	float curAngle = 0.0f;

	PointF trianglePos;
	PointF triTop;
	PointF triBot;
	PointF triRig;

	float curA = 1.0f;
	float curB = 0.0f;
	float curC = 0.0f;
};