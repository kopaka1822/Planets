#pragma once
#include "UIObject.h"

class UIParticle : public UIObject
{
public:
	UIParticle(Color c)
		:
		col(c)
	{}
	virtual ~UIParticle()
	{}
	virtual void Draw(Drawing& draw) override
	{
		draw.DrawParticle(pos + (dim / 2.0f), col, dim.x * 2.0f / 3.0f);
	}
	void SetColor(Color c)
	{
		col = c;
	}
private:
	Color col;
};