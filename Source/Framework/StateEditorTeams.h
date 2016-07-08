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
		teamMenu.Register(*this);

		teamMenu.SetMetrics({ 500.0f, 500.0f });
		teamMenu.Center();

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

		teamMenu.Draw(draw);

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