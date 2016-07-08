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

		lblTitle.CenterX(50.0f);
		btnBack.SetOrigin({ 10, Framework::STD_DRAW_Y - btnBack.GetMetrics().y - 10 });
		btnApply.SetOrigin({ Framework::STD_DRAW_X - btnApply.GetMetrics().x - 10, btnBack.GetOrigin().y });
		btnDefault.SetOrigin(btnBack.GetOrigin() - PointF(0.0f, btnBack.GetMetrics().y + 10.0f));

		grid.CenterX(150.0f);

		picker.SetMetrics({ 400.0f, 400.0f });
		picker.Center();
		picker.Disable();

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
		btnDefault.Draw(draw);
		btnBack.Draw(draw);
		btnApply.Draw(draw);
		lblTitle.Draw(draw);
		grid.Draw(draw);

		if (picker.isEnabled())
			picker.Draw(draw);
	}
private:
	void EnableButtons()
	{
		for (size_t i = 0; i < grid.GetSize(); i++)
		{
			grid.GetButtons()[i]->Enable();
		}
		picker.Disable();
	}
	void DisableButtons()
	{
		for (unsigned int i = 0; i < grid.GetSize(); i++)
		{
			grid.GetButtons()[i]->Disable();
		}
		picker.Enable();
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