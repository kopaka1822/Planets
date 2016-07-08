#pragma once
#include "GameState.h"
#include "UIButtonText.h"
#include "StateSingle.h"
#include "../Game/LocalMapClassicMode.h"
#include "../Game/LocalMapLazyMode.h"

class StateTutorial : public GameState
{
public:
	StateTutorial()
		:
		lblTitle("Tutorial", FONT_BIG),
		btnMenu("Menu", FONT_MED),
		btn1("Basics", FONT_MED),
		btn2("Units", FONT_MED)
	{
		Database::ClearGameStruct();

		AddObject(&btnMenu);
		AddObject(&btn1);
		AddObject(&btn2);

		lblTitle.CenterX(50.0f);

		float bwidth = 400.0f;
		btn1.SetWidth(bwidth);
		btn2.SetWidth(bwidth);

		btn1.CenterX(200.0f);
		btn2.CenterX(300.0f);

		btnMenu.CenterX(Framework::STD_DRAW_Y - btnMenu.GetMetrics().y - 10.0f);
	}
	virtual ~StateTutorial()
	{}

	virtual void ExecuteCode(float dt) override
	{
		if (btnMenu.Pushed())
			SetNextState(states::Menu);

		if (btn1.Pushed())
			LoadTut1();

		if (btn2.Pushed())
			LoadTut2();
	}
	virtual void ComposeFrame(Drawing& draw) override
	{
		lblTitle.Draw(draw);
		btn1.Draw(draw);
		btn2.Draw(draw);
		btnMenu.Draw(draw);
	}
private:
	void LoadTut1()
	{
		GameStruct& gs = Database::GetGameStruct();

		gs.myTeam = 1;
		gs.nTeams = 2;

		MapLoader::MapPlanet p;
		MapLoader::MapSpawn e;
		std::vector< MapLoader::MapPlanet > plans;
		std::vector< MapLoader::MapSpawn > ents;
		
		// own
		p.radius = 50.0f;
		p.team = 1;
		p.x = 150.0f;
		p.y = 250.0f;
		MapLoader::AdjustMapPlanet(&p);
		
		plans.push_back(p);

		// enemy
		p.x = 850.0f;
		p.team = 2;
		plans.push_back(p);

		// neutral
		p.x = 250.0f;
		p.y = 150.0f;
		p.team = 0;
		p.radius = 30.0f;
		MapLoader::AdjustMapPlanet(&p);
		plans.push_back(p);

		p.y = 350.0f;
		plans.push_back(p);

		// units
		e.team = 1;
		e.type = MapLoader::EntityType::E_NORMAL;
		e.x = 250.0f;
		e.y = 250.0f;
		e.nUnits = 30;
		ents.push_back(e);

		// easy ai
		gs.pMap = std::unique_ptr< LocalMapClassicMode >(new LocalMapClassicMode(2, plans, ents, 1000.0f, 500.0f, Map::GameType::AllvAll, std::vector<byte>()));
		
		StateSingle::LoadDefaultNames(gs);

		SetNextState(states::TutoialGame);
	}
	void LoadTut2()
	{
		GameStruct& gs = Database::GetGameStruct();

		gs.myTeam = 1;
		gs.nTeams = 2;

		MapLoader::MapPlanet p;
		MapLoader::MapSpawn e;
		std::vector< MapLoader::MapPlanet > plans;
		std::vector< MapLoader::MapSpawn > ents;

		// wihite planet
		p.radius = 50.0f;
		p.team = 0;
		p.x = 350.0f;
		p.y = 750.0f;
		MapLoader::AdjustMapPlanet(&p);

		plans.push_back(p);

		// units
		e.team = 2;
		e.nUnits = 10;
		e.type = MapLoader::E_NORMAL;
		e.x = 350.0f;
		e.y = 150.0f;
		ents.push_back(e);

		e.y = 350.0f;
		ents.push_back(e);

		e.nUnits = 40;
		e.y = 550.0f;
		ents.push_back(e);

		// my entities
		e.team = 1;
		e.nUnits = 1;
		e.type = MapLoader::E_NORMAL;
		e.x = 150.0f;
		e.y = 150.0f;
		ents.push_back(e);

		e.type = MapLoader::E_SPEEDER;
		e.y = 350.0f;
		ents.push_back(e);

		e.type = MapLoader::E_BOMBER;
		e.y = 550.0f;
		ents.push_back(e);

		e.type = MapLoader::E_SABOTEUR;
		e.y = 750.0f;
		ents.push_back(e);

		// lazy ai
		gs.pMap = std::unique_ptr< LocalMapLazyMode >(new LocalMapLazyMode(2, plans, ents, 500.0f, 900.0f, Map::GameType::AllvAll, std::vector<byte>()));

		StateSingle::LoadDefaultNames(gs);

		SetNextState(states::TutorialGame2);
	}

private:
	UILabel lblTitle;
	UIButtonText btnMenu;

	UIButtonText btn1;
	UIButtonText btn2;
};