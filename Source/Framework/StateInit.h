#pragma once
#include "GameState.h"
#include "UIWaiting.h"
#include "../Utility/Thread.h"
#include "UIPlanet.h"
#include "UIProgressBar.h"

class StateInit : public GameState
{
	class Initializer : public Thread
	{
	public:
		Initializer()
		{}
		virtual ~Initializer()
		{}
		virtual int ThreadProc() override
		{
			try
			{
				Sound::LoadFiles(Settings::GetVolMusic(), Settings::GetVolSound());
				// Loading levelpacks
				Database::LoadLevelpacks();

				bReady = true;
			}
			catch (const std::exception& e)
			{
				SetThreadError(e.what());
			}
			return 0;
		}
		bool isReady()
		{
			return bReady;
		}
		float GetPercent() const
		{
			return Sound::GetFileLoadPercent();
		}
	private:
		bool bReady = false;
	};
public:
	StateInit()
		:
		waitPlan(Color::White()),
		lblTitle("Initializing", FONT_BIG),
		planLeft(Color::GetTeamColor(4), 100.0f, score),
		planRight(Color::GetTeamColor(3),100.0f,score),
		lblError("",FONT_SMALL),
		btnQuit("Quit",FONT_MED)
	{
		waitPlan.setMetrics(PointF(400, 400));
		waitPlan.centerX(150.0f);

		lblTitle.centerX(50.0f);

		score = Settings::GetMenuScore();
		
		AddObject(&planLeft);
		AddObject(&planRight);

		planLeft.setOrigin({ 200, 300 });
		planRight.setOrigin({ 1100, 400 });

		bar.setMetrics({ 800.0f, 50.0f });
		bar.centerX(650.0f);

		AddObject(&btnQuit);
		btnQuit.disable();
		btnQuit.AdjustToFont();
		btnQuit.setOrigin({ 1100, 600.0f });

		bFirstStart = Settings::UnlockAchievement(Settings::A_FirstStart);
	}
	virtual ~StateInit()
	{
		Settings::SetMenuStore(score);
		initer.Join();
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.draw(draw);
		waitPlan.draw(draw);

		planLeft.draw(draw);
		planRight.draw(draw);

		bar.draw(draw);

		if (bError)
		{
			lblError.draw(draw);
			btnQuit.draw(draw);
		}

		planLeft.DrawEntities(draw);
		planRight.DrawEntities(draw);
/*
		if (initer.HasThreadError())
		{
			FONT_MED.SetColor(Color::Red());
			FONT_MED.Text(initer.GetThreadError(), PointF(10, 650));
		}*/
	}
	virtual void ExecuteCode(float dt) override
	{
		if (!bError)
			waitPlan.Update(dt);
		else
			waitPlan.Update(-0.01f * dt);
		// init sound
		
		if (!bSoundInit)
		{
			bSoundInit = Sound::InitStep();
		}
		else
		{
			if (!bInitStart)
			{
				bInitStart = true;
				initer.Begin();
			}
			else
			{
				if (initer.isReady())
				{
					if (bFirstStart)
						SetNextState(states::StateFirstStart);
					else
						SetNextState(states::Menu);
				}
			}
		}

		planLeft.Update(dt);
		planRight.Update(dt);

		bar.SetProgress(initer.GetPercent());

		if (initer.HasThreadError())
		{
			if (lblError.GetText().length() == 0)
			{
				lblError.SetText(initer.GetThreadError());
				lblError.AdjustToFont();
				lblError.centerX(600.0f);
				btnQuit.enable();
				waitPlan.SetColor(Color::Red());
				bError = true;
			}
		}

		if (btnQuit.Pushed())
		{
			SetNextState(states::Exit);
		}
	}
private:
	UIWaiting waitPlan;
	Initializer initer;

	bool bSoundInit = false;
	bool bInitStart = false;

	UILabel lblTitle;
	UIPlanet planLeft;
	UIPlanet planRight;
	UIProgressBar bar;

	unsigned long long score;
	UILabel lblError;

	bool bError = false;

	UIButtonText btnQuit;
	bool bFirstStart = false;
};