#pragma once
#include "GameState.h"
#include "UIButtonText.h"

class StateTutorialProposal : public GameState
{
public:
	StateTutorialProposal()
		:
		lblTitle("Do you want to play the Tutorial?",FONT_MED),
		btnYes("yes",FONT_MED),
		btnNo("no",FONT_MED)
	{
		AddObject(&btnYes);
		AddObject(&btnNo);

		lblTitle.CenterX(150.0f);

		float wi = btnYes.GetMetrics().x + 20.0f;
		btnNo.SetWidth(wi);
		btnYes.SetWidth(wi);

		PointF mid = PointF(Framework::STD_DRAW_X / 2, 350.0f);
		btnNo.SetOrigin(mid + PointF(10.0f, 0.0f));

		btnYes.SetOrigin(mid - PointF(wi + 10.0f, 0.0f));
	}
	virtual ~StateTutorialProposal()
	{}
	virtual void ExecuteCode(float dt) override
	{
		if (btnYes.Pushed())
			SetNextState(states::Tutorial);

		if (btnNo.Pushed())
			SetNextState(states::Menu);
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.Draw(draw);
		btnYes.Draw(draw);
		btnNo.Draw(draw);
	}

private:
	UILabel lblTitle;
	UIButtonText btnYes;
	UIButtonText btnNo;
};