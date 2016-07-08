#pragma once
#include "StateGame.h"

class StateGameSingle : public StateGame
{
public:
	StateGameSingle()
		:
		StateGame(1, Database::GetGameStruct().nTeams)
	{
		Event_Resize(Framework::DrawStart(), Framework::DrawWidth());
	}
	virtual ~StateGameSingle()
	{}
	virtual void ComposeFrame(Drawing& draw) override
	{
		DrawMap(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		if (bTurbo)
			dt *= turboFac;

		UpdateMap(dt);
		UpdateGameButtons();
	}
	virtual void Event_KeyDown(SDL_Scancode code)override
	{
		if (code == SDL_SCANCODE_T)
		{
			bTurbo = true;
			turboFac = 10.0f;
		}

		if (bTurbo && !gs.lvlPackLevel)
		{
			if (code == SDL_SCANCODE_RIGHTBRACKET)
				turboFac += 10.0f;
		}
	}
	virtual void Event_KeyUp(SDL_Scancode code) override
	{
		if (code == SDL_SCANCODE_T)
			bTurbo = false;
	}
private:
	bool bTurbo = false;
	float turboFac = 10.0f;
};