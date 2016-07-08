#pragma once
#include "GameState.h"
#include "UIAchievLister.h"
#include "UIButtonText.h"

class StateAchiev : public GameState
{
public:
	StateAchiev()
		:
		lblTitle("Achievements",FONT_BIG),
		btnBack("Back",FONT_MED)
	{
		lblTitle.CenterX(50.0f);
		btnBack.SetOrigin({ 10, Framework::STD_DRAW_Y - btnBack.GetMetrics().y - 10 });

		list.SetMetrics({ 800, 500 });
		list.CenterX(150.0f);

		list.LoadData();

		AddObject(&btnBack);
		AddObject(&list);
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.Draw(draw);
		btnBack.Draw(draw);
		list.Draw(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		if (btnBack.Pushed())
			SetNextState(states::Extras);
	}
private:
	UILabel lblTitle;
	UIButtonText btnBack;
	UIAchievLister list;
};