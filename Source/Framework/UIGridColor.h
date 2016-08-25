#pragma once
#include "UIObject.h"
#include "UIButtonColor.h"

class UIGridColor : public UIObject
{
public:
	UIGridColor()
	{
		memset(btns, 0, sizeof(btns));
		float btnwi = 100.0f;
		for (int i = 0; i < 25; i++)
		{
			btns[i] = new UIButtonColor(Color::GetTeamColor(i + 1));
			btns[i]->setMetrics({ btnwi, btnwi });
		}

		setMetrics({ 5 * btnwi + 4 * dist, 5 * btnwi + 4 * dist });
	}
	virtual ~UIGridColor()
	{

	}
	void LoadColors()
	{
		for (int i = 0; i < 25; i++)
		{
			btns[i]->SetColor(Color::GetTeamColor(i + 1));
		}
	}
	virtual void draw(Drawing& draw) override
	{
		for (int i = 0; i < 25; i++)
		{
			btns[i]->draw(draw);
		}
	}
	virtual void setOrigin(const PointF& p) override
	{
		UIObject::setOrigin(p);
		float btnwi = btns[0]->getMetrics().x;

		float curX = m_pos.x;
		for (int x = 0; x < 5; x++)
		{
			float curY = m_pos.y;
			for (int y = 0; y < 5; y++)
			{
				btns[y * 5 + x]->setOrigin({ curX, curY });
				curY += dist + btnwi;
			}
			curX += dist + btnwi;
		}
	}
	UIButtonColor** GetButtons()
	{
		return btns;
	}
	unsigned int GetSize() const
	{
		return 25;
	}
private:
	UIButtonColor* btns[25];
	static const int dist = 10;
};