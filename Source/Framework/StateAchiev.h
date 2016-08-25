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
		lblTitle.centerX(50.0f);
		btnBack.setOrigin({ 10, Framework::STD_DRAW_Y - btnBack.getMetrics().y - 10 });

		list.setMetrics({ 800, 500 });
		list.centerX(150.0f);

		list.LoadData();

		AddObject(&btnBack);
		AddObject(&list);
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.draw(draw);
		btnBack.draw(draw);
		list.draw(draw);
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