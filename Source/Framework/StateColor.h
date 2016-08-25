#pragma once
#include "GameState.h"
#include "UIButtonText.h"
#include "UIButtonColor.h"
#include "UIGridColor.h"
#include "UIColorSelect.h"

class StateColor : public GameState
{
public:
	StateColor()
		:
		btnBack("Back", FONT_MED),
		lblTitle("Colors",FONT_BIG),
		btnApply("Apply",FONT_MED),
		btnDefault("Load Default",FONT_MED),
		grid(),
		picker()
	{
		AddObject(&btnBack);
		AddObject(&picker);
		AddObject(&btnApply);
		AddObject(&btnDefault);

		lblTitle.centerX(50.0f);
		btnBack.setOrigin({ 10, Framework::STD_DRAW_Y - btnBack.getMetrics().y - 10 });
		btnApply.setOrigin({ Framework::STD_DRAW_X - btnApply.getMetrics().x - 10, btnBack.getOrigin().y });
		btnDefault.setOrigin(btnBack.getOrigin() - PointF(0.0f, btnBack.getMetrics().y + 10.0f));

		grid.centerX(150.0f);

		picker.setMetrics({ 400.0f, 400.0f });
		picker.center();
		picker.disable();

		// Add Objects
		for (unsigned int i = 0; i < grid.GetSize(); i++)
		{
			AddObject(grid.GetButtons()[i]);
		}

		AddObject(&picker.GetButton());
	}
	virtual ~StateColor()
	{

	}
	virtual void ExecuteCode(float dt) override
	{
		if (btnBack.Pushed())
		{
			SetNextState(states::Extras);
		}

		if (btnApply.Pushed())
		{
			for (unsigned int i = 0; i < grid.GetSize(); i++)
			{
				Settings::SetColor(i + 1, grid.GetButtons()[i]->GetColor());
			}
			Settings::Save();
			SetNextState(states::Extras);
		}

		if (btnDefault.Pushed())
		{
			Settings::LoadDefaultColors();
			grid.LoadColors();
		}

		// test buttons
		for (unsigned int i = 0; i < grid.GetSize(); i++)
		{
			if (grid.GetButtons()[i]->Pushed())
			{
				// edit color
				curBtn = i;
				picker.SetColor(grid.GetButtons()[i]->GetColor());
				DisableButtons();
			}
		}

		if (!picker.isEnabled() && !grid.GetButtons()[0]->isEnabled())
			EnableButtons();

		if (picker.GetButton().Pushed())
		{
			// save Color
			if (curBtn != -1)
			{
				grid.GetButtons()[curBtn]->SetColor(picker.GetColor());
				curBtn = -1;
			}
			EnableButtons();
		}
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		btnDefault.draw(draw);
		btnBack.draw(draw);
		btnApply.draw(draw);
		lblTitle.draw(draw);
		grid.draw(draw);

		if (picker.isEnabled())
			picker.draw(draw);
	}
private:
	void EnableButtons()
	{
		for (size_t i = 0; i < grid.GetSize(); i++)
		{
			grid.GetButtons()[i]->enable();
		}
		picker.disable();
	}
	void DisableButtons()
	{
		for (unsigned int i = 0; i < grid.GetSize(); i++)
		{
			grid.GetButtons()[i]->disable();
		}
		picker.enable();
	}
private:
	UIButtonText btnBack;
	UIButtonText btnApply;
	UIButtonText btnDefault;
	UILabel lblTitle;
	
	UIGridColor grid;
	UIColorSelect picker;

	int curBtn = -1;
};