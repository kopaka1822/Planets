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
		PointF dim = btnAchv.GetMetrics();

		btnMenu.SetMetrics(dim);
		btnColor.SetMetrics(dim);
		btnEditor.SetMetrics(dim);

		lblTitle.AdjustToFont();

		lblTitle.CenterX(50);

		btnAchv.CenterX(200);
		btnColor.CenterX(300);
		btnEditor.CenterX(400);
		btnMenu.CenterX(500);

		AddObject(&btnMenu);
		AddObject(&btnAchv);
		AddObject(&btnColor);
		AddObject(&btnEditor);
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.Draw(draw);

		btnAchv.Draw(draw);
		btnColor.Draw(draw);
		btnEditor.Draw(draw);
		btnMenu.Draw(draw);
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