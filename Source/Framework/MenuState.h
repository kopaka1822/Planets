#pragma once
#include "GameState.h"
#include "UIButtonText.h"
#include "Database.h"
#include "UIPlanet.h"
#include "Settings.h"
#include "../System/System.h"
#include "UILabelTexture.h"
#include "UINumUpDownInt.h"

class MenuState : public GameState
{
public:
	MenuState()
		:
		btnSingle("Singleplayer", FONT_MED),
		btnMulti("Multiplayer", FONT_MED),
		btnOption("Options", FONT_MED),
		btnQuit("Quit", FONT_MED),
		btnExtra("Extras", FONT_MED),
		lblWelcome("Welcome to Planets", Drawing::GetDraw().GetFont(Drawing::F_BIG)),

		planRed(Color::GetTeamColor(1),100.0f, score),
		planBlue(Color::GetTeamColor(2),100.0f, score),
		logo(Database::GetTexture(Database::GameTex::Logo)),
		bExtraUnlocked(true)
	{
		Sound::Theme(Sound::ThemeMax());

		AddObject(&btnSingle);
		AddObject(&btnMulti);
		AddObject(&btnOption);
		AddObject(&btnQuit);
		if(bExtraUnlocked)
			AddObject(&btnExtra);

		PointF btnSize = btnSingle.getMetrics();
		btnSize.x += 100.0f;

		btnSingle.setMetrics(btnSize);
		btnMulti.setMetrics(btnSize);
		btnOption.setMetrics(btnSize);
		btnQuit.setMetrics(btnSize);

		//positioning
		//lblWelcome.centerX(50);
		logo.CenterX(20.0f, 700);
		btnSingle.centerX(200);
		btnMulti.centerX(300);
		btnOption.centerX(400);
		btnQuit.centerX(500);
		btnExtra.setOrigin({ 965, 640 });

		planRed.setOrigin({ 1050.0f, 250.0f });
		AddObject(&planRed);
		planBlue.setOrigin({ 200.0f, 600.0f });
		AddObject(&planBlue);

		score = Settings::GetMenuScore();
	}
	virtual ~MenuState()
	{
		Settings::SetMenuStore(score);
	}
	virtual void ComposeFrame(Drawing& draw) override
	{	
		
		//draw label
		logo.draw(draw);

		//draw buttons
		btnSingle.draw(draw);
		btnMulti.draw(draw);
		btnOption.draw(draw);
		btnQuit.draw(draw);
		if(bExtraUnlocked)
			btnExtra.draw(draw);
		

		//cool visual effects

		planRed.draw(draw);
		planBlue.draw(draw);

		planRed.DrawEntities(draw);
		planBlue.DrawEntities(draw);

		if (score)
		{
			Font& f = draw.GetFont(Drawing::F_SMALL);
			f.SetColor(Color::White());
			f.Text("Score: ", PointI(10.0f, 10.0f));
			f.Text(std::to_string(score), PointI(110, 10));
		}
		
	}
	virtual void ExecuteCode(float dt) override
	{
		planRed.Update(dt);
		planBlue.Update(dt);

		if (btnSingle.Pushed())
			SetNextState(states::Single);

		if (btnMulti.Pushed())
		{
			if (Settings::GetName().length() > 0)
			{
				SetNextState(states::ServerSelect);
			}
			else
			{
				SetNextState(states::MultiName);
			}
		}
			

		if (btnOption.Pushed())
			SetNextState(states::Options);

		if (btnQuit.Pushed())
			SetNextState(states::Exit);

		if (btnExtra.Pushed())
			SetNextState(states::Extras);

		if (score > 0)
		{
			Settings::UnlockAchievement(Settings::A_MenuEaster);
			if (score > 9000)
			{
				Settings::UnlockAchievement(Settings::A_9000);
				if (score > 100000)
				{
					Settings::UnlockAchievement(Settings::A_Cookie);
					if (score > 1000000)
					{
						Settings::UnlockAchievement(Settings::A_Click1000000);
					}
				}
			}
		}
	}
private:

	UILabelTexture logo;

	UIButtonText btnSingle,
		btnMulti,
		btnOption,
		btnExtra,
		btnQuit;

	UILabel lblWelcome;

	UIPlanet planRed;
	UIPlanet planBlue;

	unsigned long long score = 0;
	bool bExtraUnlocked;
};