#pragma once
#include "GameState.h"
#include "Database.h"
#include "UIStatsLister.h"

class StateGameOver2 : public GameState
{
public:
	StateGameOver2()
		:
		res(Database::GetResults()),
		lblTitle(GetTitle(),FONT_BIG),
		btnMenu("Menu",FONT_MED),
		gs(Database::GetGameStruct()),
		btnLobby("Lobby",FONT_MED)
	{
		lblTitle.CenterX(50.0f);

		Database::GetMainServer(&reServer, &rePort);
		if (reServer.length())
			bReconnect = true;

		if (bReconnect)
		{
			btnLobby.CenterX(Framework::STD_DRAW_Y - 10.0f - btnLobby.GetMetrics().y);
			AddObject(&btnLobby);
		}
		else
		{
			btnMenu.CenterX(625.0f);
			AddObject(&btnMenu);
		}
		

		list.SetMetrics(PointF(1000, 450));
		list.CenterX(150.0f);

		list.LoadGameResults(res);

		AddObject(&list);
	}
	virtual ~StateGameOver2()
	{
		Database::ClearGameStruct();
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.Draw(draw);

		if (bReconnect)
			btnLobby.Draw(draw);
		else
			btnMenu.Draw(draw);

		list.Draw(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		if (btnMenu.Pushed())
			SetNextState(states::Single);

		if (btnLobby.Pushed())
		{
			Database::SetServerAdress(reServer, rePort);
			SetNextState(states::Connect);
		}
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
	UIButtonText btnMenu;
	UIButtonText btnLobby;
	UIStatsLister list;
	GameStruct& gs;

	std::string reServer;
	unsigned short rePort;

	bool bReconnect = false;
};