#pragma once
#include "UIButtonTexture.h"
#include "Database.h"
#include "UITooltip.h"

class UIGridEntity : public UIObject
{
public:
	UIGridEntity(byte team)
		:
		teamCol(Color::GetTeamColor(team)),
		active(MapObject::etNormal),
		activeVisible(MapObject::etNormal)
	{
		memset(btns, 0, sizeof(btns));
		memset(btns, 0, sizeof(tips));
		for (int i = 0; i < 4; i++)
		{
			btns[i] = new UIButtonTexture(Database::GetTexture((Database::GameTex)((int)(Database::GameTex::Particle) + i)));
			btns[i]->SetMetrics({ item, item });

			tips[i] = new UITooltip(*btns[i], FONT_SMALL);
		}

		SetMetrics({ 3 * border + 2 * item, 3 * border + 2 * item });

		Disable();

		tips[0]->AddLine("Entity");
		tips[0]->AddLine("HP: 8");
		tips[0]->AddLine("Damage: 4");
		tips[0]->AddLine("Spawntime: x1");

		tips[1]->AddLine("Bomber");
		tips[1]->AddLine("HP: 1");
		tips[1]->AddLine("Damage: 20 (AOE if destroyed)");
		tips[1]->AddLine("Spawntime: x3");
		tips[1]->AddLine("Idle, explodes on death");

		tips[2]->AddLine("Speeder");
		tips[2]->AddLine("HP: 8");
		tips[2]->AddLine("Damage: 4");
		tips[2]->AddLine("Spawntime: x1.5");
		tips[2]->AddLine("Speed: x2");

		tips[3]->AddLine("Saboteur");
		tips[3]->AddLine("HP: 2");
		tips[3]->AddLine("Spawntime: x12");
		tips[3]->AddLine("Changes status of opposing");
		tips[3]->AddLine("or neutral planets");
		tips[3]->AddLine("by touching an dies");
	}
	virtual void SetOrigin(const PointF& p) override
	{
		UIObject::SetOrigin(p);
		btns[0]->SetOrigin(PointF( border, border ) + pos);
		btns[1]->SetOrigin(PointF(2 * border + item, border) + pos);
		
		btns[2]->SetOrigin(PointF(border, 2 * border + item) + pos);
		btns[3]->SetOrigin(PointF(2 * border + item, 2 * border + item) + pos);
	}
	virtual ~UIGridEntity()
	{
		for (int i = 0; i < 4; i++)
		{
			tool::safeDelete(btns[i]);
			tool::safeDelete(tips[i]);
		}
	}
	virtual void Draw(Drawing& draw) override
	{
		draw.DrawRect(GetRect(), Color::White());
		for (int i = 0; i < 4; i++)
		{
			if (i == activeVisible)
			{
				btns[i]->DrawColored(draw, teamCol);
			}
			else
			{
				btns[i]->Draw(draw);
			}
		}
		for (int i = 0; i < 4; i++)
		{
			tips[i]->Draw(draw);
		}
	}
	void SetActiveType(MapObject::entityType et)
	{
		active = et;
		activeVisible = et;
	}
	bool Pushed()
	{
		int nActive = active;
		for (int i = 0; i < 4; i++)
		{
			if (btns[i]->Pushed())
			{
				nActive = i;
			}
		}
		if (nActive != active)
		{
			active = (MapObject::entityType)nActive;
			return true;
		}
		return false;
	}
	MapObject::entityType GetCurType() const
	{
		return active;
	}
	virtual void Register(GameState& gs) override
	{
		for (auto& b : btns)
		{
			b->Register(gs);
		}
		for (auto& b : tips)
		{
			b->Register(gs);
		}
	}
	virtual void Enable() override
	{
		UIObject::Enable();
		for (int i = 0; i < 4; i++)
		{
			btns[i]->Enable();
		}
	}
	virtual void Disable() override
	{
		UIObject::Disable();
		for (int i = 0; i < 4; i++)
		{
			btns[i]->Disable();
		}
	}
private:
	UIButtonTexture* btns[4];
	UITooltip* tips[4];

	static const int border = 6;
	static const int item = 50;

	MapObject::entityType active;
	MapObject::entityType activeVisible;
	const Color teamCol;
};