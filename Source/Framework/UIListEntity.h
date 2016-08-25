#pragma once
#include "UIItemListerScroll.h"
#include "UILabel.h"
#include "UINumUpDown.h"
#include "UINumUpDownInt.h"
#include "../Game/MapLoader.h"
#include "UINumEntityType.h"

class UIListEntity : public UIItemListerScroll
{
public:
	UIListEntity()
		:
		UIItemListerScroll(false),
		lblX("x:", FONT_SMALL),
		lblY("y:", FONT_SMALL),
		numX(0, 0, 10000, 1, FONT_SMALL),
		numY(0, 0, 10000, 1, FONT_SMALL),
		lblTeam("team:", FONT_SMALL),
		numTeam(1, 1, 25, 1, FONT_SMALL),
		lblUnits("Units:", FONT_SMALL),
		numUnits(10, 1, 200, 5,FONT_SMALL),
		lblType("type",FONT_SMALL),
		numType(FONT_SMALL)
	{
		float wi = FONT_SMALL.GetFontWidth() * 10.0f;
		numX.AdjustToFont(wi);
		numY.AdjustToFont(wi);
		numTeam.AdjustToFont(wi);
		numUnits.AdjustToFont(wi);
		numType.AdjustToFont(wi);

		AddItem(&lblX, &numX);
		AddItem(&lblY, &numY);
		AddItem(&lblTeam, &numTeam);
		AddItem(&lblUnits, &numUnits);
		AddItem(&lblType, &numType);

		UIItemLister::MoveMidpoint(0.4f);
	}
	MapLoader::MapSpawn GetStruct() const
	{
		MapLoader::MapSpawn s;
		s.x = (float)numX.GetValue();
		s.y = (float)numY.GetValue();
		s.team = numTeam.GetValue();
		s.nUnits = numUnits.GetValue();
		s.type = numType.GetValue();

		return s;
	}
	void LoadStruct(const MapLoader::MapSpawn& s)
	{
		numX.SetValueNoChange((int)s.x);
		numY.SetValueNoChange((int)s.y);
		numTeam.SetValueNoChange(s.team);
		numUnits.SetValueNoChange(s.nUnits);
		numType.SetValueNoChange(s.type);
	}
	void SetFixed()
	{
		numX.enable();
		numY.enable();
	}
	void SetFloating()
	{
		numX.disable();
		numY.disable();
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

		if (numUnits.ValueChanged())
			c = true;

		if (numType.ValueChanged())
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
	UILabel lblUnits;
	UINumUpDownInt numUnits;
	UILabel lblType;
	UINumEntityType numType;
};