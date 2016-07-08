#pragma once
#include "GameState.h"
#include "UIStar.h"

class StateLevelpackWin : public GameState
{
public:
	StateLevelpackWin()
		:
		btnContinue("Continue",FONT_MED),
		lblTitle("Victory",FONT_BIG),
		lblMyTime("",FONT_MED),
		lblPackTime("",FONT_MED),
		sPack(Database::GetLevelpackStuct()),
		gs(Database::GetGameStruct())
	{
		AddObject(&btnContinue);

		lblTitle.CenterX(50.0f);
		
		btnContinue.CenterX(640.0f);

		// Set info for time labels
		packTime = GetPackTime();
		lblPackTime.SetText("Star Time: " + GetTimeString(packTime));
		lblPackTime.AdjustToFont();

		lblPackTime.CenterX(560.0f);

		myTime = gs.time;
		lblMyTime.SetText("Your Time: " + GetTimeString(myTime));
		lblMyTime.AdjustToFont();

		lblMyTime.CenterX(500.0f);

		// Set stars
		float rad = 280.0f;
		float off = 50.0f;
		s1.SetMetrics({ rad, rad });
		s2.SetMetrics({ rad, rad });
		s3.SetMetrics({ rad, rad });

		s2.CenterX(150.0f);

		s1.SetOrigin(s2.GetOrigin() - PointF(off + rad,0.0f));
		s3.SetOrigin(s2.GetOrigin() + PointF(off + rad, 0.0f));

		CalcStarTypes();
	}
	virtual ~StateLevelpackWin()
	{}
	virtual void ExecuteCode(float dt) override
	{
		if (btnContinue.Pushed())
			SetNextState(states::GameOver);

		s1.Update(dt);
		s2.Update(dt);
		s3.Update(dt);
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.Draw(draw);
		s1.Draw(draw);
		s2.Draw(draw);
		s3.Draw(draw);

		lblPackTime.Draw(draw);
		lblMyTime.Draw(draw);
		btnContinue.Draw(draw);
	}
private:
	float GetPackTime()
	{
		const auto& packs = Database::GetLevelpacks();
		return packs[sPack.curLevelpack].GetLevelTime(sPack.curLvl);
	}
	std::string GetTimeString(float seconds)
	{
		int i = int(seconds);
		int mins = i / 60;
		int secs = i % 60;

		std::string res = std::to_string(mins) + std::string(":");
		if (mins < 10)
			res = "0" + res;

		if (secs < 10)
			res += "0";

		res += std::to_string(secs);

		return res;
	}
	void CalcStarTypes()
	{
		int newFlag = 1; // level cleared

		if (myTime <= packTime * 1.3f)
		{
			// mid star
			newFlag |= 2;
			if (myTime <= packTime)
			{
				// all stars
				newFlag |= 4;
			}
		}

		auto& packs = Database::GetLevelpacks();
		LevelSavedata& savedata = packs[sPack.curLevelpack].GetSavedata();
		int oldFlag = savedata.GetStarflags()[sPack.curLvl];

		SetStartype(s1, newFlag, oldFlag, 1);
		SetStartype(s2, newFlag, oldFlag, 2);
		SetStartype(s3, newFlag, oldFlag, 4);

		// save if change
		savedata.SetNewRecord(sPack.curLvl, newFlag, myTime);

		if (sPack.curLvl == 11) // last level
			Settings::UnlockAchievement(Settings::A_AllLevel);
	}
	void SetStartype(UIStar& dst, int newFlag, int oldFlag, int curFlag)
	{
		int mixed = newFlag | oldFlag;

		// Set star types
		if (mixed & curFlag)
		{
			// star
			if (oldFlag & curFlag)
			{
				// previous star
				dst.SetType(UIStar::FILLED);
			}
			else
			{
				// new star
				dst.SetType(UIStar::NEW);
			}
		}
		else
		{
			// no star
			dst.SetType(UIStar::EMPTY);
		}
	}
private:
	UIButtonText btnContinue;
	UILabel lblTitle;

	UILabel lblMyTime;
	UILabel lblPackTime;

	float myTime = 0.0f;
	float packTime = 0.0f;

	LevelpackStruct& sPack;
	GameStruct& gs;

	UIStar s1;
	UIStar s2;
	UIStar s3;
};