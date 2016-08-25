#pragma once
#include "UIYNButton.h"

class UIYNButtonPlay : public UIYNButton
{
public:
	UIYNButtonPlay(bool State)
		: UIYNButton(State)
	{
	}
	virtual ~UIYNButtonPlay()
	{}

	virtual void Draw(Drawing& draw) override
	{
		draw.DrawRect(GetRect(), Color::White());
		Texture& tex = GetState() ? Database::GetTexture(Database::GameTex::FastForward) :
			Database::GetTexture(Database::GameTex::Play);
		
		draw.DrawSprite(RectF(pos + PointF(border, border), pos + dim - PointF(border, border)), tex);
	}

private:
	const float border = 6.0f;
};