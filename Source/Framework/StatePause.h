#pragma once
#include "GameState.h"

class StatePause : public GameState
{
public:
	StatePause(std::unique_ptr<GameState> previ)
		:
		prev(std::move(previ)),
		btnMenu("Exit",FONT_MED),
		btnBack("Continue",FONT_MED),
		btnOptions("Options",FONT_MED)
	{
		if (prev)
		{
			bUpdate = prev->BackgroundUpdate();
		}

		float wi = btnBack.getMetrics().x + 100.0f;
 
		btnBack.setWidth(wi);
		btnMenu.setWidth(wi);
		btnOptions.setWidth(wi);

		btnBack.centerX(200.0f);
		btnOptions.centerX(300.0f);
		btnMenu.centerX(400.0f);

		AddObject(&btnBack);
		AddObject(&btnMenu);
		AddObject(&btnOptions);
	}
	virtual ~StatePause()
	{}
	virtual void ComposeFrame(Drawing& draw) override
	{
		if (prev)
			prev->ComposeFrame(draw);

		btnMenu.draw(draw);
		btnOptions.draw(draw);
		btnBack.draw(draw);
	}
	virtual void Event_Resize(const PointF& ul, const PointF& dim) override
	{
		if (prev)
			prev->Event_Resize(ul, dim);
	}
	virtual void ExecuteCode(float dt) override
	{
		if (bUpdate)
			prev->ExecuteCode(dt);

		if (btnBack.Pushed())
			SetNextState(states::Back);

		if (btnMenu.Pushed())
			SetNextState(states::Menu);

		if (btnOptions.Pushed())
			SetNextState(states::Options);
	}
	virtual bool BackgroundUpdate() override
	{
		return true;
	}
	virtual std::unique_ptr<GameState> GetPrev() override
	{
		return std::move(prev);
	}
private:
	bool bUpdate = false;
	std::unique_ptr<GameState> prev;

	UIButtonText btnMenu;
	UIButtonText btnBack;
	UIButtonText btnOptions;
};