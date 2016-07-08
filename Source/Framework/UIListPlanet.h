#pragma once
#include "UIItemListerScroll.h"
#include "UILabel.h"
#include "UINumUpDown.h"
#include "UINumUpDownInt.h"
#include "../Game/MapLoader.h"

class UIListPlanet : public UIItemListerScroll
{
public:
	UIListPlanet()
		:
		UIItemListerScroll(false),
		lblX("x:",FONT_SMALL),
		lblY("y:",FONT_SMALL),
		numX(0,0,10000,1,FONT_SMALL),
		numY(0, 0, 10000, 1, FONT_SMALL),
		lblTeam("team:",FONT_SMALL),
		numTeam(0,0,25,1,FONT_SMALL),
		lblRadius("radius:",FONT_SMALL),
		numRadius(50,25,200,5,FONT_SMALL),
		lblSPU("s/Unit:",FONT_SMALL),
		numSPU(2.0f, 0.05f, 800.0f, 0.25f,FONT_SMALL),
		lblHP("HP:",FONT_SMALL),
		numHP(10, 0, 999999, 10,FONT_SMALL)
	{
		float wi = FONT_SMALL.GetFontWidth() * 10.0f;
		numX.AdjustToFont(wi);
		numY.AdjustToFont(wi);
		numTeam.AdjustToFont(wi);
		numRadius.AdjustToFont(wi);
		numSPU.AdjustToFont(wi);
		numHP.AdjustToFont(wi);

		AddItem(&lblX, &numX);
		AddItem(&lblY, &numY);
		AddItem(&lblTeam, &numTeam);
		AddItem(&lblRadius, &numRadius);
		AddItem(&lblSPU, &numSPU);
		AddItem(&lblHP, &numHP);

		auto p = GetStruct();
		MapLoader::AdjustMapPlanet(&p);
		numSPU.SetValueNoChange(p.sUnit);
		numHP.SetValueNoChange(p.HP);

		UIItemLister::MoveMidpoint(0.4f);
	}
	MapLoader::MapPlanet GetStruct() const
	{
		MapLoader::MapPlanet p;
		p.x = (float)numX.GetValue();
		p.y = (float)numY.GetValue();
		p.radius = (float)numRadius.GetValue();
		p.team = numTeam.GetValue();
		p.HP = numHP.GetValue();
		p.sUnit = numSPU.GetValue();

		return p;
	}
	void LoadStruct(const MapLoader::MapPlanet& p)
	{
		numX.SetValueNoChange((int)p.x);
		numY.SetValueNoChange((int)p.y);
		numRadius.SetValueNoChange((int)p.radius);
		numTeam.SetValueNoChange(p.team);
		numHP.SetValueNoChange(p.HP);
		numSPU.SetValueNoChange(p.sUnit);
	}
	void SetFixed()
	{
		numX.Enable();
		numY.Enable();
	}
	void SetFloating()
	{
		numX.Disable();
		numY.Disable();
	}
	bool Changed()
	{
		bool c = false;
		if (numX.ValueChanged())
			c = true;

		if (numY.ValueChanged())
			c = true;

		if (numTeam.ValueChanged())
			c = true;

		if (numRadius.ValueChanged())
		{
			auto p = GetStruct();
			MapLoader::AdjustMapPlanet(&p);
			numSPU.SetValueNoChange(p.sUnit);
			numHP.SetValueNoChange(p.HP);
			c = true;
		}

		if (numSPU.ValueChanged())
			c = true;

		if (numHP.ValueChanged())
			c = true;

		return c;
	}
	void SetXY(const PointF& p)
	{
		numX.SetValueNoChange((int)p.x);
		numY.SetValueNoChange((int)p.y);
	}
private:
	UILabel lblX;
	UINumUpDownInt numX;
	UILabel lblY;
	UINumUpDownInt numY;
	UILabel lblTeam;
	UINumUpDownInt numTeam;
	UILabel lblRadius;
	UINumUpDownInt numRadius;
	UILabel lblSPU;
	UINumUpDownFloat numSPU;
	UILabel lblHP;
	UINumUpDownInt numHP;
};