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

	virtual void draw(Drawing& draw) override
	{
		draw.DrawSprite(getRect(), tex);
	}
	
	void CenterX(float y, float width)
	{
		float height = width / float(tex.GetWidth()) * tex.GetHeight();
		setMetrics(PointF(width, height));
		
		UIObject::centerX(y);
	}
private:
	const Texture& tex;
};