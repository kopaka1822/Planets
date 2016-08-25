#pragma once
#include "GameState.h"
#include "UIBoxInput.h"

class StateMultiName : public GameState
{
public:
	StateMultiName()
		:
		title("Multiplayer Name",FONT_BIG),
		name(FONT_MED,20),
		btnOk("Continue",FONT_MED),
		btnBack("Back",FONT_MED)
	{
		title.centerX(50.0f);

		name.setMetrics({ 20.0f * FONT_MED.GetFontWidth()
			, FONT_MED.GetFontHeight() + 2 * UIButton::GetBorder() });
		name.center();
		unsigned int flags = UITextInput::F_NUMBER | UITextInput::F_SPECIAL | UITextInput::F_UPPERCASE | UITextInput::F_LOWERCASE | UITextInput::F_STRICT_NAME;
		name.SetFlags(UITextInput::FLAGS(flags));
		
		btnBack.setOrigin({ 10, Framework::STD_DRAW_Y - btnBack.getMetrics().y - 10 });
		btnOk.setOrigin({ Framework::STD_DRAW_X - btnOk.getMetrics().x - 10.0f, btnBack.getOrigin().y });
		
		AddObject(&name);
		AddObject(&btnBack);
		AddObject(&btnOk);
	}
	virtual void ExecuteCode(float dt) override
	{
		if (btnBack.Pushed())
		{
			SetNextState(states::Menu);
		}
		if (btnOk.Pushed())
		{
			if (name.GetText().length() > 0)
			{
				// save
				Settings::SetName(name.GetText());
				Settings::Save();
				SetNextState(states::ServerSelect);
			}
			else
				name.MarkRed();
		}
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		title.draw(draw);
		name.draw(draw);
		btnBack.draw(draw);
		btnOk.draw(draw);
	}

private:
	UILabel title;
	UIBoxInput name;
	UIButtonText btnOk;
	UIButtonText btnBack;
};