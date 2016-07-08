#include "UIButtonTexture.h"

class UIButtonTextureBorder : public UIButtonTexture
{
public:
	UIButtonTextureBorder(const Texture& tex)
		:
		UIButtonTexture(tex)
	{}

	virtual void Draw(Drawing& draw) override
	{
		draw.DrawBox(GetRect(), (float)border, Color::White(), Color::Black());
		draw.DrawSprite(RectF(pos + PointF(border,border),pos + dim - PointF(border,border)), tex);
	}
	virtual void DrawColored(Drawing& draw, Color col) override
	{
		draw.DrawBox(GetRect(), (float)border, Color::White(), Color::Black());
		draw.DrawSpriteColored(RectF(pos + PointF(border, border), pos + dim - PointF(border, border)), tex, col);
	}
};