#pragma once
#include "GameState.h"

// Menu
/*
	Achievments
	Theme Customization
	Color Customization
	Editor
*/

class StateExtras : public GameState
{
public:
	StateExtras()
		:
		btnMenu("Menu",FONT_MED),
		btnAchv("Achievements", FONT_MED),
		btnColor("Colors", FONT_MED),
		btnEditor("Editor", FONT_MED),
		lblTitle("Extras",FONT_BIG)
	{
		btnAchv.AdjustToFont();
		PointF dim = btnAchv.getMetrics();

		btnMenu.setMetrics(dim);
		btnColor.setMetrics(dim);
		btnEditor.setMetrics(dim);

		lblTitle.AdjustToFont();

		lblTitle.centerX(50);

		btnAchv.centerX(200);
		btnColor.centerX(300);
		btnEditor.centerX(400);
		btnMenu.centerX(500);

		AddObject(&btnMenu);
		AddObject(&btnAchv);
		AddObject(&btnColor);
		AddObject(&btnEditor);
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.draw(draw);

		btnAchv.draw(draw);
		btnColor.draw(draw);
		btnEditor.draw(draw);
		btnMenu.draw(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		if (btnMenu.Pushed())
			SetNextState(states::Menu);

		if (btnEditor.Pushed())
			SetNextState(states::Editor);

		if (btnAchv.Pushed())
			SetNextState(states::Achiev);

		if (btnColor.Pushed())
			SetNextState(states::Color);
	}
private:
	UIButtonText btnMenu;
	UIButtonText btnAchv;
	UIButtonText btnColor;
	UIButtonText btnEditor;

	UILabel lblTitle;
};