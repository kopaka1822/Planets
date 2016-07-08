#pragma once
#include "UIObject.h"
#include "Texture.h"

class UILabelTexture : public UIObject
{
public:
	UILabelTexture(const Texture& tex)
		:
		tex(tex)
	{}

	virtual void Draw(Drawing& draw) override
	{
		draw.DrawSprite(GetRect(), tex);
	}
	
	void CenterX(float y, float width)
	{
		float height = width / float(tex.GetWidth()) * tex.GetHeight();
		SetMetrics(PointF(width, height));
		
		UIObject::CenterX(y);
	}
private:
	const Texture& tex;
};