#pragma once
#include "GameState.h"
#include "Input.h"
#include "Sound.h"
#include "UITrackbar.h"
#include "Sound.h"
//#include "UITextInput.h"
#include "UIBoxInput.h"
#include "UIYNButton.h"
#include "UIItemListerScroll.h"
#include "UIButtonDetect.h"
#include "UISettings.h"
#include "UINumUpDownInt.h"
#include "UINumPowerTwo.h"
#include "UIButtonEnableDisable.h"

class StateOptions : public GameState
{
	static const int M_MELODY = 1;
	static const int M_GLOCK = 2;
	static const int M_DRUMS = 4;
	static const int M_ARPS = 8;
	static const int M_JUMPER = 16;
	static const int M_PULSER = 32;
	static const int M_BASS = 64;
public:
	StateOptions(std::unique_ptr<GameState> previ)  //change options ingame
		:
		prev(std::move(previ)),

		tInpName(FONT_MED,20),
		tInpClan(FONT_MED,20),

		btnBack("Back", FONT_MED),
		lblOptions("Options", FONT_BIG),
		lblName("Name", FONT_MED),
		lblClan("Clan", FONT_MED),
		lblMusic("Music", FONT_MED),
		lblSound("Sound", FONT_MED),

		btnFullscreen(Settings::GetFullscreen()),
		itmScroll(false),
		scrollSound(false),
		scrollControls(false),
		scrollVideo(false),
		lblFull("Fullscreen", FONT_MED),
		lblControls("Controls:", FONT_MED),
		btnStarfield(Settings::GetStarfield()),
		lblStarfield("Starfield",FONT_MED),

		lblLeft("Set Target", FONT_MED),
		lblSel("Select Units", FONT_MED),
		lblDesel("Deselect", FONT_MED),
		lblSelAll("Select All", FONT_MED),
		lblGroupadd("Add to Group", FONT_MED),
		lblGroupmake("Assign Group", FONT_MED),
		lblDeselectOnTarget("Auto Deselect", FONT_MED),

		btnDisableOnTarget(Settings::DeselectOnTargetSelect(), FONT_MED),
		ctrlLeft(FONT_MED, Settings::GetGamekey(Input::GK_CLICK)),
		ctrlSel(FONT_MED, Settings::GetGamekey(Input::GK_SELECT)),
		ctrlDesel(FONT_MED, Settings::GetGamekey(Input::GK_DESELECT)),
		ctrlSelAll(FONT_MED, Settings::GetGamekey(Input::GK_SELECTALL)),
		ctrlGroupadd(FONT_MED, Settings::GetGamekey(Input::GK_ADDGROUP)),
		ctrlGroupmake(FONT_MED, Settings::GetGamekey(Input::GK_MAKEGROUP)),

		lblRestart("Please restart the game to apply changes",FONT_SMALL),
		btnLines(Settings::TargetLinesEnabled()),
		lblLines("Target Lines",FONT_MED),
		lblShaderFXAA("FXAA Shader",FONT_MED),
		btnShaderFXAA((Drawing::GetDraw().GetShaderConfig() & Drawing::FXAA) != 0),
		lblShaderStory("Storybook Shader", FONT_MED),
		btnShaderStory((Drawing::GetDraw().GetShaderConfig() & Drawing::Storybook) != 0),
		lblShaderRadial("Radial Shader", FONT_MED),
		btnShaderRadial((Drawing::GetDraw().GetShaderConfig() & Drawing::RadialBlur) != 0),
		lblFXAASpan("FXAA span",FONT_MED),
		lblFXAAMul("FXAA reduce mul", FONT_MED),
		lblFXAAShift("FXAA pixel shift", FONT_MED),
		lblPlanetGlow("Planet Glow", FONT_MED),

		lblTheme("Theme",FONT_BIG),
		lblThMelody("Melody:",FONT_MED),
		lblThGlock("Glock:", FONT_MED),
		lblThDrums("Drums:", FONT_MED),
		lblThArps("Arps:", FONT_MED),
		lblThJumper("Jumper:", FONT_MED),
		lblThPulser("Pulser:", FONT_MED),
		lblThBass("Bass:", FONT_MED),

		btnThMelody((Sound::ThemeGetFilter() & M_MELODY) != 0),
		btnThGlock((Sound::ThemeGetFilter() & M_GLOCK) != 0 ),
		btnThDrums((Sound::ThemeGetFilter() & M_DRUMS) != 0 ),
		btnThArps((Sound::ThemeGetFilter() & M_ARPS) != 0 ),
		btnThJumper((Sound::ThemeGetFilter() & M_JUMPER) != 0 ),
		btnThPulser((Sound::ThemeGetFilter() & M_PULSER) != 0 ),
		btnThBass((Sound::ThemeGetFilter() & M_BASS) != 0),

		lblChat("Chat",FONT_MED),
		lblPlayer("Playerlist",FONT_MED),

		ctrlChat(FONT_MED, Settings::GetGamekey(Input::GK_CHAT)),
		ctrlPlayer(FONT_MED, Settings::GetGamekey(Input::GK_PLAYERLIST)),

		lblDetail("Details",FONT_MED),
		lblPerfInfo("Show FPS",FONT_MED),
		btnPerfInfo(Settings::GetPerformanceInfo()),

		ctrlFilterEnt(FONT_MED, Settings::GetGamekey(Input::GK_FILTERENT)),
		ctrlFilterBomb(FONT_MED, Settings::GetGamekey(Input::GK_FILTERBOMB)),
		ctrlFilterSabo(FONT_MED, Settings::GetGamekey(Input::GK_FILTERSABO)),
		ctrlFilterSpeed(FONT_MED, Settings::GetGamekey(Input::GK_FILTERSPEED)),
		ctrlAllPlanDefense(FONT_MED, Settings::GetGamekey(Input::GK_PLANDEFENSE)),

		lblFilterEnt("Filter Entity", FONT_MED),
		lblFilterBomb("Filter Bomber", FONT_MED),
		lblFilterSabo("Filter Saboteur", FONT_MED),
		lblFilterSpeed("Filter Speeder", FONT_MED),
		lblAllPlanDefense("Defend Planets", FONT_MED)
	{

		// add grid elements
		grid.AddSection("General", &itmScroll);
		grid.AddSection("Video", &scrollVideo);
		grid.AddSection("Sound", &scrollSound);
		grid.AddSection("Controls", &scrollControls);

		grid.SetOrigin(PointF(10.0f, 100.0f));
		grid.SetMetrics(PointF(Framework::STD_DRAW_X - 20, Framework::STD_DRAW_Y - 200.0f));
		//grid.CenterX(100.0f);

		// add scroll items
		float rw = 400.0f;

		tInpName.SetMetrics({ rw, mFontHeight + 2 * UIButton::GetBorder() });
		tInpClan.SetMetrics({ rw, mFontHeight + 2 * UIButton::GetBorder() });

		trackM.SetMetrics({ rw, mFontHeight });
		trackS.SetMetrics({ rw, mFontHeight });
		trackDetail.SetMetrics({ rw, mFontHeight });

		trackFXAAMul.SetMetrics({ rw, mFontHeight });
		trackFXAAShift.SetMetrics({ rw, mFontHeight });
		trackFXAASpan.SetMetrics({ rw, mFontHeight });
		trackPlanetGlow.SetMetrics({ rw, mFontHeight });

		btnDisableOnTarget.SetWidth(rw);
		ctrlLeft.AdjustToFont(rw);
		ctrlSel.AdjustToFont(rw);
		ctrlDesel.AdjustToFont(rw);
		ctrlSelAll.AdjustToFont(rw);
		ctrlGroupadd.AdjustToFont(rw);
		ctrlGroupmake.AdjustToFont(rw);
		ctrlChat.AdjustToFont(rw);
		ctrlPlayer.AdjustToFont(rw);
		ctrlFilterEnt.AdjustToFont(rw);
		ctrlFilterBomb.AdjustToFont(rw);
		ctrlFilterSpeed.AdjustToFont(rw);
		ctrlFilterSabo.AdjustToFont(rw);
		ctrlAllPlanDefense.AdjustToFont(rw);

		// General
		itmScroll.AddItem(&lblName, &tInpName);
		itmScroll.AddItem(&lblClan, &tInpClan);

		// Video
		scrollVideo.AddItem(&lblFull, &btnFullscreen);
		scrollVideo.AddItem(&lblLines, &btnLines);
		scrollVideo.AddItem(&lblDetail, &trackDetail);
		scrollVideo.AddItem(&lblPlanetGlow, &trackPlanetGlow);
		scrollVideo.AddItem(&lblStarfield, &btnStarfield);
		scrollVideo.AddItem(&lblPerfInfo, &btnPerfInfo);

		if (Drawing::GetDraw().HasFramebufferSupport())
		{
			scrollVideo.AddItem(&lblShaderRadial, &btnShaderRadial);
			scrollVideo.AddItem(&lblShaderStory, &btnShaderStory);

			// FXAA settings
			scrollVideo.AddItem(&lblShaderFXAA, &btnShaderFXAA);
			scrollVideo.AddItem(&lblFXAASpan, &trackFXAASpan);
			//scrollVideo.AddItem(&lblFXAAMul, &trackFXAAMul); 
			//scrollVideo.AddItem(&lblFXAAShift, &trackFXAAShift);
		}

		// Sound
		scrollSound.AddItem(&lblMusic, &trackM);
		scrollSound.AddItem(&lblSound, &trackS);

		scrollSound.AddItem(&lblTheme, nullptr);
		scrollSound.AddItem(&lblThMelody, &btnThMelody);
		scrollSound.AddItem(&lblThGlock, &btnThGlock);
		scrollSound.AddItem(&lblThDrums, &btnThDrums);
		scrollSound.AddItem(&lblThArps, &btnThArps);
		scrollSound.AddItem(&lblThJumper, &btnThJumper);
		scrollSound.AddItem(&lblThPulser, &btnThPulser);
		scrollSound.AddItem(&lblThBass, &btnThBass);
		
		// Controls
		scrollControls.AddItem(&lblDeselectOnTarget, &btnDisableOnTarget);
		scrollControls.AddItem(&lblLeft, &ctrlLeft);
		scrollControls.AddItem(&lblSel, &ctrlSel);
		scrollControls.AddItem(&lblDesel, &ctrlDesel);
		scrollControls.AddItem(&lblSelAll, &ctrlSelAll);
		scrollControls.AddItem(&lblGroupadd, &ctrlGroupadd);
		scrollControls.AddItem(&lblGroupmake, &ctrlGroupmake);
		scrollControls.AddItem(&lblChat, &ctrlChat);
		scrollControls.AddItem(&lblPlayer, &ctrlPlayer);
		scrollControls.AddItem(&lblFilterEnt, &ctrlFilterEnt);
		scrollControls.AddItem(&lblFilterBomb, &ctrlFilterBomb);
		scrollControls.AddItem(&lblFilterSpeed, &ctrlFilterSpeed);
		scrollControls.AddItem(&lblFilterSabo, &ctrlFilterSabo);
		scrollControls.AddItem(&lblAllPlanDefense, &ctrlAllPlanDefense);

		//itmScroll.OrderItems();
		grid.OrderItems();

		btnBack.SetOrigin({ 10, Framework::STD_DRAW_Y - btnBack.GetMetrics().y - 10 });

	
		AddObject(&btnBack);
		grid.Register(*this);

		lblOptions.CenterX(10);

		trackM.SetValue(Settings::GetVolMusic());
		trackS.SetValue(Settings::GetVolSound());
		trackDetail.SetValue(Settings::GetDetailLevel());

		// span 1 - 16
		trackFXAASpan.SetValue((Settings::GetFXAASpan() - 1) / 15.0f);
		// reduce mul 0 - 1
		trackFXAAMul.SetValue(Settings::GetFXAAMul());
		// subpix shift 0.1 - 1.0
		trackFXAAShift.SetValue((Settings::GetFXAAShift() - 0.1f) / 0.9f);

		tInpName.SetText(Settings::GetName());
		tInpClan.SetText(Settings::GetClan());

		trackPlanetGlow.SetValue(Settings::GetPlanetGlowFactor());

		unsigned int flags = UITextInput::F_NUMBER | UITextInput::F_SPECIAL | UITextInput::F_UPPERCASE | UITextInput::F_LOWERCASE | UITextInput::F_STRICT_NAME;
		tInpName.SetFlags(UITextInput::FLAGS(flags));
		tInpClan.SetFlags(UITextInput::FLAGS(flags));

		lblRestart.SetColor(Color::Red());
		lblRestart.CenterX(btnBack.GetOrigin().y);
		lblRestart.Disable();

		if (!btnShaderFXAA.isEnabled())
		{
			trackFXAASpan.Disable();
			trackFXAAMul.Disable();
			trackFXAAShift.Disable();
		}
	}
	virtual ~StateOptions()
	{
		Settings::SetDeselectOnTargetSelect(btnDisableOnTarget.GetState());
		Settings::SetPlanetGlowFactor(trackPlanetGlow.GetValue());

		Settings::SetGamekey(Input::GK_CLICK, ctrlLeft.GetKey());
		Settings::SetGamekey(Input::GK_SELECT, ctrlSel.GetKey());
		Settings::SetGamekey(Input::GK_DESELECT, ctrlDesel.GetKey());
		Settings::SetGamekey(Input::GK_SELECTALL, ctrlSelAll.GetKey());
		Settings::SetGamekey(Input::GK_ADDGROUP, ctrlGroupadd.GetKey());
		Settings::SetGamekey(Input::GK_MAKEGROUP, ctrlGroupmake.GetKey());
		Settings::SetGamekey(Input::GK_CHAT, ctrlChat.GetKey());
		Settings::SetGamekey(Input::GK_PLAYERLIST, ctrlPlayer.GetKey());

		Settings::SetGamekey(Input::GK_FILTERENT, ctrlFilterEnt.GetKey());
		Settings::SetGamekey(Input::GK_FILTERBOMB, ctrlFilterBomb.GetKey());
		Settings::SetGamekey(Input::GK_FILTERSPEED, ctrlFilterSpeed.GetKey());
		Settings::SetGamekey(Input::GK_FILTERSABO, ctrlFilterSabo.GetKey());
		Settings::SetGamekey(Input::GK_PLANDEFENSE, ctrlAllPlanDefense.GetKey());

		if (tInpName.GetText().length() > 0)
			Settings::SetName(tInpName.GetText());

		if (tInpClan.GetText().length() > 0)
			Settings::SetClan(tInpClan.GetText());

		Settings::SetShader(Drawing::GetDraw().GetShaderConfig());

		Settings::Save();
	}
	virtual void Event_Resize(const PointF& ul, const PointF& dim)
	{
		btnFullscreen.SetState(Settings::GetFullscreen());
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblOptions.Draw(draw);
		
		//itmScroll.Draw(draw);
		grid.Draw(draw);

		btnBack.Draw(draw);

		if (lblRestart.isEnabled())
			lblRestart.Draw(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		if (prev)
			if (prev->BackgroundUpdate())
				prev->ExecuteCode(dt);

		if (btnBack.Pushed())
			SetNextState(states::Back);

		if (trackM.Changed())
			Settings::SetVolMusic(trackM.GetValue());

		if (trackS.Changed())
			Settings::SetVolSound(trackS.GetValue());

		if (trackS.Released())
			Sound::Effect(Sound::S_DIE);

		if (btnFullscreen.Pushed())
			Settings::SetFullscreen(btnFullscreen.GetState());

		if (btnLines.Pushed())
			Settings::SetTargetLines(btnLines.GetState());

		if (trackDetail.Changed())
			Settings::SetDetailLevel(trackDetail.GetValue());

		if (btnPerfInfo.Pushed())
			Settings::SetPerformanceInfo(btnPerfInfo.GetState());

		if (btnThMelody.Pushed() ||
			btnThGlock.Pushed() ||
			btnThDrums.Pushed() ||
			btnThArps.Pushed() ||
			btnThJumper.Pushed() ||
			btnThPulser.Pushed() ||
			btnThBass.Pushed())
			UpdateFilter();

		if (btnStarfield.Pushed())
			Settings::SetStarfield(btnStarfield.GetState());

		//if (btnShader.Pushed())
			//Settings::SetShader(btnShader.GetState());
		Drawing& draw = Drawing::GetDraw();

		if (btnShaderFXAA.Pushed())
		{
			UpdateShaders();

			if (btnShaderFXAA.isEnabled())
			{
				trackFXAASpan.Enable();
				trackFXAAMul.Enable();
				trackFXAAShift.Enable();
			}
			else
			{
				trackFXAASpan.Disable();
				trackFXAAMul.Disable();
				trackFXAAShift.Disable();
			}
		}
		if (btnShaderStory.Pushed())
		{
			UpdateShaders();
		}
		if (btnShaderRadial.Pushed())
		{
			UpdateShaders();
		}

		
		if (trackFXAAMul.Changed())
		{
			// reduce mul 0 - 1
			Settings::SetFXAAMul(trackFXAAMul.GetValue());
		}
		if (trackFXAAShift.Changed())
		{
			// subpix shift 0.1 - 1.0
			Settings::SetFXAAShift(trackFXAAShift.GetValue() * 0.9f + 0.1f);
		}
		if (trackFXAASpan.Changed())
		{
			// span 1 - 16
			Settings::SetFXAASpan(trackFXAASpan.GetValue() * 15.0f + 1.0f);
		}

		btnDisableOnTarget.Pushed(); // check to remove clicked flag

		grid.Update();
	}
	virtual std::unique_ptr<GameState> GetPrev() override
	{
		return std::move(prev);
	}
private:
	void UpdateFilter()
	{
		unsigned int filter = (btnThMelody.GetState() * M_MELODY) |
			(btnThGlock.GetState() * M_GLOCK) |
			(btnThDrums.GetState() * M_DRUMS) |
			(btnThArps.GetState() * M_ARPS) |
			(btnThJumper.GetState() * M_JUMPER) |
			(btnThPulser.GetState() * M_PULSER) |
			(btnThBass.GetState() * M_BASS);
		Sound::ThemeSetFilter(filter);
		static int djCount = 0;
		djCount++;
		if (djCount > 20)
			Settings::UnlockAchievement(Settings::A_DJ);
	}
	void UpdateShaders()
	{
		unsigned int mask = 0;
		if (btnShaderFXAA.GetState())
			mask |= Drawing::FXAA;
		if (btnShaderRadial.GetState())
			mask |= Drawing::RadialBlur;
		if (btnShaderStory.GetState())
			mask |= Drawing::Storybook;

		Drawing::GetDraw().SetShaderConfig(mask);
	}
private:
	std::unique_ptr<GameState> prev;

	UITrackbar trackM;
	UITrackbar trackS;
	UIButtonText btnBack;

	UIBoxInput tInpName;
	UIBoxInput tInpClan;

	UILabel lblOptions;
	UILabel lblName;
	UILabel lblClan;
	UILabel lblMusic;
	UILabel lblSound;
	UILabel lblFull;
	
	// controls
	UILabel lblControls;
	UILabel lblDeselectOnTarget;
	UILabel lblLeft;
	UILabel lblSel;
	UILabel lblDesel;
	UILabel lblSelAll;
	UILabel lblGroupmake;
	UILabel lblGroupadd;
	UILabel lblChat;
	UILabel lblPlayer;

	UILabel lblFilterEnt;
	UILabel lblFilterBomb;
	UILabel lblFilterSabo;
	UILabel lblFilterSpeed;
	UILabel lblAllPlanDefense;

	//UIYNButton btnDisableOnTarget;
	UIButtonEnableDisable btnDisableOnTarget;

	UIButtonDetect ctrlLeft;
	UIButtonDetect ctrlSel;
	UIButtonDetect ctrlDesel;
	UIButtonDetect ctrlSelAll;
	UIButtonDetect ctrlGroupmake;
	UIButtonDetect ctrlGroupadd;

	UIButtonDetect ctrlFilterEnt;
	UIButtonDetect ctrlFilterBomb;
	UIButtonDetect ctrlFilterSabo;
	UIButtonDetect ctrlFilterSpeed;
	UIButtonDetect ctrlAllPlanDefense;

	UIButtonDetect ctrlChat;
	UIButtonDetect ctrlPlayer;

	// Shader
	UILabel lblShaderStory;
	UIYNButton btnShaderStory;
	UILabel lblShaderFXAA;
	UIYNButton btnShaderFXAA;
	UILabel lblShaderRadial;
	UIYNButton btnShaderRadial;
	UILabel lblPlanetGlow;
	UITrackbar trackPlanetGlow;
	// FXAA Settings
	UILabel lblFXAAShift;
	UILabel lblFXAAMul;
	UILabel lblFXAASpan;
	UITrackbar trackFXAAShift;
	UITrackbar trackFXAAMul;
	UITrackbar trackFXAASpan;


	UIYNButton btnFullscreen;
	UIYNButton btnStarfield;
	UIYNButton btnLines;
	UILabel lblLines;
	UILabel lblDetail;
	UILabel lblStarfield;
	UITrackbar trackDetail;
	UILabel lblPerfInfo;
	UIYNButton btnPerfInfo;

	UILabel lblRestart;

	const float mFontHeight = Drawing::GetDraw().GetFont(Drawing::F_MEDIUM).GetFontHeight();
	const float bFontHeight = Drawing::GetDraw().GetFont(Drawing::F_BIG).GetFontHeight();

	UIItemListerScroll itmScroll; // General
	UIItemListerScroll scrollSound;
	UIItemListerScroll scrollVideo;
	UIItemListerScroll scrollControls;

	UISettings grid;

	// music
	UILabel lblTheme;
	UILabel lblThMelody;
	UILabel lblThGlock;
	UILabel lblThDrums;
	UILabel lblThArps;
	UILabel lblThJumper;
	UILabel lblThPulser;
	UILabel lblThBass;

	UIYNButton btnThMelody;
	UIYNButton btnThGlock;
	UIYNButton btnThDrums;
	UIYNButton btnThArps;
	UIYNButton btnThJumper;
	UIYNButton btnThPulser;
	UIYNButton btnThBass;
};
