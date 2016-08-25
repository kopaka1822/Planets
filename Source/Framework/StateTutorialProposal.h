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

		lblTitle.centerX(150.0f);

		float wi = btnYes.getMetrics().x + 20.0f;
		btnNo.setWidth(wi);
		btnYes.setWidth(wi);

		PointF mid = PointF(Framework::STD_DRAW_X / 2, 350.0f);
		btnNo.setOrigin(mid + PointF(10.0f, 0.0f));

		btnYes.setOrigin(mid - PointF(wi + 10.0f, 0.0f));
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
		lblTitle.draw(draw);
		btnYes.draw(draw);
		btnNo.draw(draw);
	}

private:
	UILabel lblTitle;
	UIButtonText btnYes;
	UIButtonText btnNo;
};