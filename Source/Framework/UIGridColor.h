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
			btns[i]->SetMetrics({ btnwi, btnwi });
		}

		SetMetrics({ 5 * btnwi + 4 * dist, 5 * btnwi + 4 * dist });
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
	virtual void Draw(Drawing& draw) override
	{
		for (int i = 0; i < 25; i++)
		{
			btns[i]->Draw(draw);
		}
	}
	virtual void SetOrigin(const PointF& p) override
	{
		UIObject::SetOrigin(p);
		float btnwi = btns[0]->GetMetrics().x;

		float curX = pos.x;
		for (int x = 0; x < 5; x++)
		{
			float curY = pos.y;
			for (int y = 0; y < 5; y++)
			{
				btns[y * 5 + x]->SetOrigin({ curX, curY });
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