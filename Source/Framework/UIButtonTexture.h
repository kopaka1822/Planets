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
	virtual void Draw(Drawing& draw) override
	{
		draw.DrawSprite(GetRect(), tex);
	}
	virtual void DrawColored(Drawing& draw, Color col)
	{
		draw.DrawSpriteColored(GetRect(), tex, col);
	}
	// draw without stretching image
	virtual void DrawColoredBox(Drawing& draw, Color col)
	{
		RectF rect = GetRect();
		if (dim.x > dim.y)
		{
			float s = (dim.x - dim.y) / 2.0f;
			rect.p1 += PointF(s, 0.0f);
			rect.p2 -= PointF(s, 0.0f);
		}
		else if (dim.y > dim.x)
		{
			float s = (dim.y - dim.x) / 2.0f;
			rect.p1 += PointF(0.0f, s);
			rect.p2 -= PointF(0.0f, s);
		}
		draw.DrawSpriteColored(rect, tex, col);
	}

protected:
	const Texture& tex;
};