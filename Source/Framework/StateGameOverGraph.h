#pragma once
#include "UIButtonText.h"
#include "GameState.h"
#include "Database.h"
#include "UIStatsGraph.h"

class StateGameOverGraph : public GameState
{
public:
	StateGameOverGraph()
		:
		res(Database::GetResults()),
		lblTitle(GetTitle(),FONT_MED),
		btnContinue("Continue",FONT_MED),
		graph(res.unitCounts, Database::GetGameStruct().myTeam, FONT_SMALL)
	{
		lblTitle.centerX(50.0f);
		btnContinue.centerX(625.0f);
		
		graph.setMetrics(PointF(1100.0f, 500.0f));
		graph.center();

		AddObject(&btnContinue);
	}
	virtual ~StateGameOverGraph()
	{}


	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.draw(draw);
		graph.draw(draw);
		btnContinue.draw(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		graph.Update(dt);

		if (btnContinue.Pushed())
			SetNextState(states::GameOver2);
	}
private:
	std::string GetTitle() const
	{
		if (res.isWinner)
		{
			Sound::Effect(Sound::S_WIN);
			Settings::UnlockAchievement(Settings::A_WinGame);
			return "Victory";
		}
		else
		{
			Sound::Effect(Sound::S_LOSE);
			Settings::UnlockAchievement(Settings::A_LoseGame);
			return "Defeat";
		}
	}
private:
	const GameResults& res;
	UILabel lblTitle;
	UIButtonText btnContinue;
	UIStatsGraph graph;
};