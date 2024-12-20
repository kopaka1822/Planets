#pragma once
#include "GameState.h"
#include "UIButtonText.h"
#include "UITrackbar.h"
#include "UIItemLister.h"
#include "UINumLowMedHigh.h"
#include "UIUnitFall.h"

class StateFirstStart : public GameState
{
public:
	StateFirstStart()
		:
		score(Settings::GetMenuScore()),
		lblTitle("Welcome", FONT_BIG),
		btnContinue("let's go", FONT_MED),

		lblGfx("Graphics Profile:", FONT_MED),
		lblMusic("Music Volume:", FONT_MED),
		lblSound("Sound Volume:", FONT_MED),
		list(false),
		numGfx(UINumLowMedHigh::MEDIUM, FONT_MED),
		plan(Drawing::PickHSV(0.0f),100.0f,score),
		fall(Drawing::PickHSV(0.0f))
	{
		AddObject(&btnContinue);
		AddObject(&plan);
		AddObject(&fall);

		list.registerMe(*this);

		PointF tdim = { 300.0f, lblMusic.getMetrics().y };
		trackMusic.setMetrics(tdim);
		trackSound.setMetrics(tdim);

		numGfx.AdjustToFont(300.0f);

		list.AddItem(&lblGfx, &numGfx);
		list.AddItem(&lblMusic, &trackMusic);
		list.AddItem(&lblSound, &trackSound);

		list.MoveMidpoint(0.7f);
		list.setMetrics({ 900, 800 });
		list.centerX(300.0f);

		list.OrderItems();

		lblTitle.centerX(100.0f);
		
		btnContinue.centerX(Framework::STD_DRAW_Y - btnContinue.getMetrics().y - 10.0f);

		trackMusic.SetValueNoChange(Settings::GetVolMusic());
		trackSound.SetValueNoChange(Settings::GetVolSound());

		plan.setOrigin({ 1100.0f, 150.0f });
		fall.setMetrics({ 200.0f, 120.0f });
		fall.setOrigin({ 420.0f, 190.0f });

		UpdateGfx();
	}
	virtual ~StateFirstStart()
	{
		Settings::SetShader(Drawing::GetDraw().GetShaderConfig());
	}
	virtual void ExecuteCode(float dt) override
	{
		if (trackSound.Changed())
			Settings::SetVolSound(trackSound.GetValue());

		if (trackSound.Released())
			Sound::Effect(Sound::S_DIE);

		if (trackMusic.Changed())
			Settings::SetVolMusic(trackMusic.GetValue());

		if (numGfx.ValueChanged())
		{
			UpdateGfx();
		}

		curWheel += dt * 0.1f;
		curWheel = fmod(curWheel, 2.0f * float(M_PI));
		Color c = Drawing::PickHSV(curWheel);
		plan.SetColor(c);
		fall.SetColor(c);

		plan.Update(dt);
		fall.Update(dt);

		if (btnContinue.Pushed())
			SetNextState(states::TutorialProposal);
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.draw(draw);
		list.draw(draw);
		plan.draw(draw);
		btnContinue.draw(draw);
		plan.DrawEntities(draw);
		fall.draw(draw);
	}
private:
	void UpdateGfx()
	{
		switch (numGfx.GetValue())
		{
		case UINumLowMedHigh::LOW:
			SetLowGfx();
			break;
		case UINumLowMedHigh::MEDIUM:
			SetMedGfx();
			break;
		case UINumLowMedHigh::HIGH:
			SetHighGfx();
			break;
		}
	}
	void SetLowGfx()
	{
		Settings::SetDetailLevel(0.0f);
		Drawing::GetDraw().SetShaderConfig(0);
		Settings::SetTargetLines(false);
	}
	void SetMedGfx()
	{
		Settings::SetDetailLevel(0.5f);
		Drawing::GetDraw().SetShaderConfig(0);
		Settings::SetTargetLines(true);
	}
	void SetHighGfx()
	{
		Settings::SetDetailLevel(0.5f);
		Drawing::GetDraw().SetShaderConfig(Drawing::RadialBlur | Drawing::FXAA);
		Settings::SetTargetLines(true);
	}
private:
	unsigned long long score;

	float curWheel = 0.0f;

	UILabel lblTitle;
	UILabel lblGfx,
		lblMusic,
		lblSound;

	UITrackbar trackMusic,
		trackSound;
	UIButtonText btnContinue;
	UINumLowMedHigh numGfx;

	UIItemLister list;

	UIPlanet plan;
	UIUnitFall fall;
};