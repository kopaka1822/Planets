#pragma once
#include "UIObject.h"
#include <vector>
#include "../Game/Levelpack.h"
#include "UIButtonLevel.h"

class UISingleplayerGrid : public UIObject
{
public:
	// this will set the metrics
	UISingleplayerGrid()
	{
		// create buttons
		for (int i = 1; i <= 12; i++)
			btns.push_back(std::unique_ptr< UIButtonLevel >(new UIButtonLevel(i)));

		// calc metrics
		PointF met;
		met.x = 4.0f * UIButtonLevel::GetWdith() + 3.0f * padding;
		met.y = 3.0f * UIButtonLevel::GetHeight() + 2.0f * padding;
		setMetrics(met);
	}
	virtual ~UISingleplayerGrid()
	{}
	virtual void setOrigin(const PointF& o)
	{
		// Set Button Origins
		float curY = o.y;
		for (int y = 0; y < 3; y++)
		{
			float curX = o.x;
			for (int x = 0; x < 4; x++)
			{
				btns[y * 4 + x]->setOrigin({ curX, curY });

				curX += padding + UIButtonLevel::GetWdith();
			}
			curY += padding + UIButtonLevel::GetHeight();
		}
		
	}
	void LoadLevelpack(Levelpack* pLevelpack)
	{
		pPack = pLevelpack;

		// init buttons
		int nStars = pLevelpack->GetSavedata().CountStars();
		const auto& starflags = pLevelpack->GetSavedata().GetStarflags();

		bool prevLvlDone = true;
		for (int i = 0; i < 12; i++)
		{
			btns[i]->SetStarFlag(starflags[i]);
			if (prevLvlDone && nStars >= StarsToUnlock(i))
				btns[i]->enable();
			else
				btns[i]->disable();

			prevLvlDone = starflags[i] != 0;
		}
	}
	// Creates enty in LevelpackStruct
	bool LevelSelected()
	{
		for (int i = 0; i < 12; i++)
			if (btns[i]->Pushed())
			{
				Database::GetLevelpackStuct().curLvl = i;
				return true;
			}

		return false;
	}
	virtual void draw(Drawing& draw) override
	{
		// draw buttons
		for (auto& b : btns)
			b->draw(draw);
	}
	virtual void registerMe(GameState& gs) override
	{
		UIObject::registerMe(gs);
		for (auto& b : btns)
			b->registerMe(gs);
	}
private:
	// lvl 0 - 11
	int StarsToUnlock(int lvl)
	{
		if (lvl <= 0)
			return 0;
		if (lvl >= 11)
			return 33;

		float x = float(lvl);
		float fx = 0.244f * x * x + 0.266f * x + 0.49f;

		return int(fx);
	}
private:
	Levelpack* pPack = nullptr;
	std::vector< std::unique_ptr< UIButtonLevel >> btns;
	static const int padding = 8;
};