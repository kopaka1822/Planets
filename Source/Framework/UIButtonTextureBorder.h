#include "UIButtonTexture.h"

class UIButtonTextureBorder : public UIButtonTexture
{
public:
	UIButtonTextureBorder(const Texture& tex)
		:
		UIButtonTexture(tex)
	{}

	virtual void draw(Drawing& draw) override
	{
		draw.DrawBox(getRect(), (float)border, Color::White(), Color::Black());
		draw.DrawSprite(RectF(m_pos + PointF(border,border),m_pos + m_dim - PointF(border,border)), tex);
	}
	virtual void DrawColored(Drawing& draw, Color col) override
	{
		draw.DrawBox(getRect(), (float)border, Color::White(), Color::Black());
		draw.DrawSpriteColored(RectF(m_pos + PointF(border, border), m_pos + m_dim - PointF(border, border)), tex, col);
	}
};