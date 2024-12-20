#pragma once
#include "StateGame.h"

class StateGameSingle : public StateGame
{
	const float TURBO_VEL = 5.0f;
public:
	StateGameSingle()
		:
		StateGame(1, Database::GetGameStruct().nTeams)
	{
		StateGame::Event_Resize(Framework::DrawStart(), Framework::DrawWidth());
		btnFastForward.enable();
	}
	virtual ~StateGameSingle()
	{}
	virtual void ComposeFrame(Drawing& draw) override
	{
		DrawMap(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		if(btnFastForward.GetState())
		{
			bTurbo = true;
			turboFac = TURBO_VEL;
		}
		else
		{
			bTurbo = false;
		}

		if (bTurbo)
			dt *= turboFac;

		UpdateMap(dt);
		UpdateGameButtons();
	}
	virtual void Event_KeyDown(SDL_Scancode code)override
	{
		if (bTurbo && !gs.lvlPackLevel)
		{
			if (code == SDL_SCANCODE_RIGHTBRACKET)
				turboFac += TURBO_VEL;
		}
	}

	virtual void Event_GameKeyUp(Input::GameKey k, const PointF& p) override
	{
		StateGame::Event_GameKeyUp(k, p);

		if(k == Input::GK_TURBO)
		{
			btnFastForward.SetState(!btnFastForward.GetState());
		}
	}
private:
	bool bTurbo = false;
	float turboFac = TURBO_VEL;
};