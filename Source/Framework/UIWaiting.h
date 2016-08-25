#pragma once
#include "UIObject.h"

class UIWaiting : public UIObject
{
public:
	UIWaiting(Color col)
		:
		col(col)
	{}
	virtual ~UIWaiting()
	{}
	virtual void draw(Drawing& draw) override
	{
		float wi = m_dim.y;
		const PointF mid = getMidpoint();
		draw.DrawPlanet(mid, col, wi / 3.0f);

		float rPart = wi / 6.0f;

		PointF part((wi / 3.0f) + rPart / 2.0f, 0.0f);
		part = part.rot(angle);
		DrawPart(part, draw, rPart);
		part = part.CCW90();
		part = part.CCW90();
		DrawPart(part, draw, rPart);
	}
	void Update(float dt)
	{
		angle += dt *  5.0f;
		angle = fmodf(angle, 2.0f * float(M_PI));
	}
	void SetColor(Color c)
	{
		col = c;
	}
private:

	void DrawPart(PointF rad, Drawing& draw, float radius)
	{
		const PointF mid = getMidpoint();
		Color col = this->col;
		for (int i = 0; i < 5; i++)
		{
			draw.DrawParticle(mid + rad, col, radius);
			rad = rad.rot(-0.1f);
			col = col.Brightness(50.0f);
			radius = radius / 2.0f;
		}
	}
private:
	Color col;
	float angle = 0.0f;
};