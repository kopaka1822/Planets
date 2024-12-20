#pragma once
#include "GameState.h"
#include "../Game/MapLoader.h"
#include "../Game/LocalMap.h"
#include "../Game/LocalMapSingleplayer.h"
#include "../Utility/DialogOpenFile.h"
#include "UIGrid.h"
#include "UISingleplayerGrid.h"

class StateSingle : public GameState
{
public:
	StateSingle()
		:
		grid(),
		test("load level", Drawing::GetDraw().GetFont(Drawing::F_MEDIUM)),
		btnBack("Back", FONT_MED),
		lblSingleplayer("Singleplayer", FONT_BIG),
		gs(Database::ClearGameStruct()),
		packs(Database::GetLevelpacks()),
		btnLeft("<",FONT_MED),
		btnRight(">",FONT_MED),
		sPack(Database::GetLevelpackStuct()),
		btnTutorial("Tutorial",FONT_MED)
	{
		if (packs.size() == 0)
			throw GameError("no levelpacks found");

		grid.registerMe(*this);
		AddObject(&test);
		AddObject(&btnBack);
		AddObject(&btnLeft);
		AddObject(&btnRight);
		AddObject(&btnTutorial);

		grid.center();

		test.setOrigin({ Framework::STD_DRAW_X - test.getMetrics().x - 10, Framework::STD_DRAW_Y - test.getMetrics().y - 10 });

		btnBack.setOrigin({ 10, Framework::STD_DRAW_Y - btnBack.getMetrics().y - 10 });
		lblSingleplayer.centerX(50);

		float hei = lblSingleplayer.getMetrics().y;
		btnLeft.setHeight(hei);
		btnRight.setHeight(hei);

		btnLeft.setOrigin(PointF(100.0f, 50.0f));
		btnRight.setOrigin(PointF(Framework::STD_DRAW_X - 100.0f - btnRight.getMetrics().x, 50.0f));

		btnTutorial.setOrigin({ 450, Framework::STD_DRAW_Y - btnBack.getMetrics().y - 10 });

		LoadLevelpack(sPack.curLevelpack);
	}
	virtual ~StateSingle()
	{}
	virtual void ComposeFrame(Drawing& draw) override
	{
		test.draw(draw);

		grid.draw(draw);

		btnBack.draw(draw);

		lblSingleplayer.draw(draw);

		btnTutorial.draw(draw);
		btnLeft.draw(draw);
		btnRight.draw(draw);
	}
	virtual void ExecuteCode(float dt) override
	{
		if (btnBack.Pushed())
			SetNextState(states::Back);

		if (btnTutorial.Pushed())
			SetNextState(states::Tutorial);

		if (test.Pushed())
		{
			DialogOpenFile dialog("map");
			//dialog.SetFiletype(L"MAP", L"*.map");
			dialog.Show();

			if (dialog.IsSuccess())
			{
				auto te = dialog.GetName();
				if (LoadLevel(te))
				{
					SetNextState(states::GameSingle);
				}
				else throw GameError("map invalid");
			}
		}

		if (btnLeft.Pushed())
		{
			LoadLevelpack((sPack.curLevelpack - 1) % packs.size());
		}

		if (btnRight.Pushed())
		{
			LoadLevelpack((sPack.curLevelpack + 1) % packs.size());
		}

		if (grid.LevelSelected())
		{
			// entry is in levelpackStruct
			LoadLevelFromPack();

			SetNextState(states::GameSingle);
		}
	}
	static void LoadDefaultNames(GameStruct& gs)
	{
		// load default names
		gs.playernames.clear();
		if (Settings::GetName().length())
			gs.playernames.push_back(Settings::GetName());
		else
			gs.playernames.push_back("Player");
		for (int i = 1; i < gs.nTeams; i++)
		{
			gs.playernames.push_back(std::string("Computer ") + std::to_string(i));
		}
	}
private:
	bool LoadLevel(std::string name)
	{
		MapLoader loader(name);

		if (!loader.isValid()) return false;

		LoadLevel(loader);

		return true;
	}
	void LoadLevelFromPack()
	{
		gs.lvlPackLevel = true;

		LoadLevel(packs[sPack.curLevelpack].GetLevel(sPack.curLvl));
	}
	// loader has to be valid
	void LoadLevel(const MapLoader& loader)
	{
		assert(loader.isValid());

		//Load Map into Database
		gs.pMap = std::unique_ptr<LocalMapSingleplayer>(new LocalMapSingleplayer(loader.GetPlayers(), loader.GetPlanets(), loader.GetSpawns(), loader.GetWidth(), loader.GetHeight(),
			loader.HasTeamSupport() ? Map::GameType::Allicance : Map::GameType::AllvAll, loader.GetTeams()));
		gs.myTeam = 1;
		gs.nTeams = loader.GetPlayers();

		LoadDefaultNames(gs);
	}
	void LoadLevelpack(int id)
	{
		sPack.curLevelpack = id;
		// Set Headline
		std::string cleaname = tool::fileRemovePath(packs[id].GetName());

		// determine max size of cleanname
		float maxwidth = btnRight.getRect().TopLeft().x - btnLeft.getRect().TopRight().x - 10.0f;
		while (FONT_BIG.GetMetrics(cleaname).x > maxwidth)
		{
			cleaname.pop_back();
		}

		lblSingleplayer.SetText(cleaname);
		lblSingleplayer.AdjustToFont();
		lblSingleplayer.centerX(50.0f);

		grid.LoadLevelpack(&(packs[id]));
	}

private:
	UIButtonText test;
	GameStruct& gs;
	LevelpackStruct& sPack;
	std::vector< Levelpack >& packs;

	UISingleplayerGrid grid;
	UIButtonText btnBack;
	UILabel lblSingleplayer;

	UIButtonText btnLeft;
	UIButtonText btnRight;

	UIButtonText btnTutorial;
};
