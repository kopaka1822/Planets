#pragma once
#include "GameState.h"
#include "UITextBox.h"

class StateError : public GameState
{
public:
	StateError(std::string errmsg)
		:
		lblTitle("Error",FONT_BIG),
		btnMenu("Menu",FONT_MED),
		boxError(errmsg,FONT_MED)
	{
		lblTitle.CenterX(50.0f);
		btnMenu.CenterX(600.0f);

		boxError.AdjustToFont(1000.0f);
		boxError.Center();

		AddObject(&btnMenu);

		Sound::Effect(Sound::S_LOSE);
	}
	virtual ~StateError()
	{}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.Draw(draw);
		boxError.Draw(draw);
		btnMenu.Draw(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		if (btnMenu.Pushed())
			SetNextState(states::Menu);
	}
private:
	UILabel lblTitle;
	UITextBox boxError;
	UIButtonText btnMenu;
};