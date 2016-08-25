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

	virtual void draw(Drawing& draw) override
	{
		draw.DrawRect(getRect(), Color::White());
		Texture& tex = GetState() ? Database::GetTexture(Database::GameTex::FastForward) :
			Database::GetTexture(Database::GameTex::Play);
		
		draw.DrawSprite(RectF(m_pos + PointF(border, border), m_pos + m_dim - PointF(border, border)), tex);
	}

private:
	const float border = 6.0f;
};