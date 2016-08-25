#pragma once
#include "UIButton.h"

class UIButtonTexture : public UIButton
{
public:
	UIButtonTexture(const Texture& tex)
		:
		tex(tex)
	{
		
	}
	virtual void draw(Drawing& draw) override
	{
		draw.DrawSprite(getRect(), tex);
	}
	virtual void DrawColored(Drawing& draw, Color col)
	{
		draw.DrawSpriteColored(getRect(), tex, col);
	}
	// draw without stretching image
	virtual void DrawColoredBox(Drawing& draw, Color col)
	{
		RectF rect = getRect();
		if (m_dim.x > m_dim.y)
		{
			float s = (m_dim.x - m_dim.y) / 2.0f;
			rect.p1 += PointF(s, 0.0f);
			rect.p2 -= PointF(s, 0.0f);
		}
		else if (m_dim.y > m_dim.x)
		{
			float s = (m_dim.y - m_dim.x) / 2.0f;
			rect.p1 += PointF(0.0f, s);
			rect.p2 -= PointF(0.0f, s);
		}
		draw.DrawSpriteColored(rect, tex, col);
	}

protected:
	const Texture& tex;
};