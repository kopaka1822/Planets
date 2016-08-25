#pragma once
#include "GameState.h"
#include "UITeamMenu.h"

class StateEditorTeams : public GameState
{
public:
	StateEditorTeams(std::unique_ptr<GameState> previ)
		:
		prev(std::move(previ)),
		teamMenu(Database::GetEditorTeams())
	{
		teamMenu.registerMe(*this);

		teamMenu.setMetrics({ 500.0f, 500.0f });
		teamMenu.center();

	}
	virtual ~StateEditorTeams()
	{
	
	}
	virtual void ExecuteCode(float dt) override
	{

	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		if (prev)
			prev->ComposeFrame(draw);

		teamMenu.draw(draw);

		if (teamMenu.OkayPressed())
		{
			Database::SetEditorTeams(teamMenu.GetTeams());
			SetNextState(states::Back);
			return;
		}

		if (teamMenu.Cancel())
		{
			SetNextState(states::Back);
		}
	}
	virtual std::unique_ptr<GameState> GetPrev() override
	{
		return std::move(prev);
	}
private:
	std::unique_ptr<GameState> prev;
	UITeamMenu teamMenu;
};