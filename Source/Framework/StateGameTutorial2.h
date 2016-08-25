#pragma once
#include "StateGameSingle.h"
#include "UITutorialBox.h"
#include "UIButtonDetect.h"

class StateGameTutorial2 : public StateGameSingle
{
	enum class Stage
	{
		normalIntro,
		normalMove,
		speedIntro,
		speedMove,
		bomberIntro,
		bomberMove,
		saboIntro,
		saboMove,
		diskExplain,
		diskTry,
		end
	};
	class MapEvents : public Map::EventReciever
	{
	public:
		MapEvents()
		{}
		virtual void Event_EntitySpawn(PlanetID pID, MapEntity& e) override{};
		virtual void Event_EntityKilled(MapEntity& e) override
		{
			bDied = true;
		};
		virtual void Event_PlanetAttacked(PlanetID pID, const MapEntity& e) override{};

		// culprit may be null if it was not captured by an entity
		virtual void Event_PlanetCaptured(PlanetID pID, byte newTeam, byte oldTeam, const MapEntity* culprit) override
		{
			if (newTeam != 0)
				bCapture = true;
		}
		bool DieEvent()
		{
			bool t = bDied;
			bDied = false;
			return t;
		}
		bool HasCaptured()
		{
			return bCapture;
		}
	private:
		bool bDied = false;
		bool bCapture = false;
	};
public:
	StateGameTutorial2()
		:
		StateGameSingle(),
		lblMission("", FONT_SMALL),
		lblMission2("", FONT_SMALL)
	{
		box1.AddLine("This is the normal entity.");
		box1.AddLine("He can attack nearby untis");
		box1.AddLine("and other planets.");
		box1.SetTexture(&Database::GetTexture(Database::GameTex::Particle));

		boxSpeed.AddLine("This unit is called the");
		boxSpeed.AddLine("Speeder. It can do everything");
		boxSpeed.AddLine("the normal unit can do but");
		boxSpeed.AddLine("it moves twice as fast");
		boxSpeed.SetTexture(&Database::GetTexture(Database::GameTex::ParticleTank));

		boxBomber.AddLine("This is the Bomber.");
		boxBomber.AddLine("The bomber can't attack");
		boxBomber.AddLine("anything but if he dies");
		boxBomber.AddLine("he will cause a huge");
		boxBomber.AddLine("explosion");
		boxBomber.SetTexture(&Database::GetTexture(Database::GameTex::ParticleExpl));

		boxSabo.AddLine("This is the Saboteur.");
		boxSabo.AddLine("As soon as this unit touches a");
		boxSabo.AddLine("planet the planets belongs");
		boxSabo.AddLine("to you. This unit cannot");
		boxSabo.AddLine("attack enemy Units.");
		boxSabo.SetTexture(&Database::GetTexture(Database::GameTex::ParticleSabo));

		boxPlanet.AddLine("Your Planet can spawn");
		boxPlanet.AddLine("this different unit types.");
		boxPlanet.AddLine("To change the spawntype you");
		boxPlanet.AddLine("have to select the planet.");
		boxPlanet.EnablePlanet(Color::GetTeamColor(1));

		boxSelector.AddLine("This is the entity Selector");
		boxSelector.AddLine("it will pop up after selecting");
		boxSelector.AddLine("the planet. To set the spawntype");
		boxSelector.AddLine("click on the desired unit.");
		boxSelector.SetTexture(&Database::GetTexture(Database::GameTex::selector));

		RegisterBox(box1);
		RegisterBox(boxSpeed);
		RegisterBox(boxBomber);
		RegisterBox(boxSabo);
		RegisterBox(boxSelector);
		RegisterBox(boxPlanet);
	
		boxPlanet.RegisterNext(&boxSelector);

		lblMission.setOrigin(PointF(10, 10));
		lblMission2.setOrigin(PointF(10, 10) + PointF(0, lblMission.getMetrics().y));

		box1.Activate();
		SetMapInput(false);
		gs.pMap->addEventReciever(&mapEvent);
	}
	virtual ~StateGameTutorial2()
	{}
	virtual void ComposeFrame(Drawing& draw) override
	{
		StateGameSingle::ComposeFrame(draw);

		lblMission.draw(draw);
		lblMission2.draw(draw);

		box1.draw(draw);
		boxSpeed.draw(draw);
		boxBomber.draw(draw);
		boxSabo.draw(draw);
		boxPlanet.draw(draw);
		boxSelector.draw(draw);
	}
	virtual void ExecuteCode(float dt) override final
	{
		const float MAX_ANI_TIME = 2.0f;

		UpdateGameButtons();
		bool bDied = mapEvent.DieEvent();

		switch (curStage)
		{
		case StateGameTutorial2::Stage::normalIntro:
			if (box1.isDone())
				PrepareNormal();
			break;
		case StateGameTutorial2::Stage::normalMove:
			UpdateMap(dt);
			if (bDied)
			{
				bAniStart = true;
			}
			if (bAniStart)
			{
				aniTime += dt;
				if (aniTime > MAX_ANI_TIME)
				{
					curStage = Stage::speedIntro;
					boxSpeed.Activate();
				}
			}
			break;
		case StateGameTutorial2::Stage::speedIntro:
			if (boxSpeed.isDone())
				PrepareSpeeder();
			break;
		case StateGameTutorial2::Stage::speedMove:
			UpdateMap(dt);
			if (bDied)
			{
				bAniStart = true;
			}
			if (bAniStart)
			{
				aniTime += dt;
				if (aniTime > MAX_ANI_TIME)
				{
					curStage = Stage::bomberIntro;
					boxBomber.Activate();
				}
			}
			break;
		case StateGameTutorial2::Stage::bomberIntro:
			if (boxBomber.isDone())
				PrepareBomber();
			break;
		case StateGameTutorial2::Stage::bomberMove:
			UpdateMap(dt);
			if (bDied)
			{
				bAniStart = true;
			}
			if (bAniStart)
			{
				aniTime += dt;
				if (aniTime > MAX_ANI_TIME)
				{
					curStage = Stage::saboIntro;
					boxSabo.Activate();
				}
			}
			break;
		case StateGameTutorial2::Stage::saboIntro:
			if (boxSabo.isDone())
				PrepareSaboteur();
			break;
		case StateGameTutorial2::Stage::saboMove:
			UpdateMap(dt);

			if (mapEvent.HasCaptured())
			{
				bAniStart = true;
			}
			if (bAniStart)
			{
				aniTime += dt;
				if (aniTime > MAX_ANI_TIME)
				{
					curStage = Stage::diskExplain;
					boxPlanet.Activate();
				}
			}
			break;
		case Stage::diskExplain:
			if (boxSelector.isDone())
			{
				curStage = Stage::diskTry;
				SetMission("Select a planet and change his spawntype");
			}
			break;
		case Stage::diskTry:
			SetMapInput(true);
			//UpdateMap(dt);
			break;
		case Stage::end:
			UpdateMap(dt);
			break;
		default:
			break;
		}

		if (isFinished())
		{
			SetNextState(states::Tutorial);
		}
	}

private:
	const float scalarFac = 2.0f;

	virtual void Event_EntityTypeChanged() override
	{
		if (curStage == Stage::diskTry)
		{
			curStage = Stage::end;
			SetMission("win the game");
		}
	}
	void PrepareNormal()
	{
		curStage = Stage::normalMove;
		gs.pMap->SelectAllEntityType(myTeam, MapObject::etNormal);
		gs.pMap->Click(PointF(350.0f, 150.0f),myTeam);

		// maybe set camera
		pMapDraw->SetScalar(scalarFac);
		pMapDraw->SetCam(PointF(250.0f, 150.0f));

		PrepareAnimation();
	}
	void PrepareSpeeder()
	{
		curStage = Stage::speedMove;
		gs.pMap->SelectAllEntityType(myTeam, MapObject::etSpeeder);
		gs.pMap->Click(PointF(350.0f, 350.0f), myTeam);

		pMapDraw->SetScalar(scalarFac);
		pMapDraw->SetCam(PointF(250.0f, 350.0f));
		
		PrepareAnimation();
	}
	void PrepareBomber()
	{
		curStage = Stage::bomberMove;
		gs.pMap->SelectAllEntityType(myTeam, MapObject::etBomber);
		gs.pMap->Click(PointF(350.0f, 550.0f), myTeam);

		pMapDraw->SetScalar(scalarFac);
		pMapDraw->SetCam(PointF(250.0f, 550.0f));

		PrepareAnimation();
	}
	void PrepareSaboteur()
	{
		curStage = Stage::saboMove;
		gs.pMap->SelectAllEntityType(myTeam, MapObject::etSaboteur);
		gs.pMap->Click(PointF(350.0f, 750.0f), myTeam);

		pMapDraw->SetScalar(scalarFac);
		pMapDraw->SetCam(PointF(250.0f, 750.0f));

		PrepareAnimation();
	}
	void PrepareAnimation()
	{
		bAniStart = false;
		aniTime = 0.0f;
	}


	void RegisterBox(UITutorialBox& box)
	{
		AddObject(&box);
		PointF met = { 900.0f, 600.0f };
		box.setMetrics(met);
		box.center();
		box.OrderItems();
	}
	void SetMission(std::string mis)
	{
		if (mis.length())
		{
			std::string all = "Mission: " + mis;
			int maxlen = (Framework::STD_DRAW_X - 20) / FONT_SMALL.GetFontWidth();
			if (mis.length() > maxlen)
			{
				lblMission.SetText(all.substr(0, maxlen - 1));
				lblMission2.SetText(all.substr(maxlen));
			}
			else
			{
				lblMission.SetText(all);
				lblMission2.SetText("");
			}
		}
		else
		{
			lblMission.SetText("");
			lblMission2.SetText("");
		}
	}
private:
	Stage curStage = Stage::normalIntro;

	UITutorialBox box1;
	UITutorialBox boxSpeed;
	UITutorialBox boxBomber;
	UITutorialBox boxSabo;

	UITutorialBox boxPlanet;
	UITutorialBox boxSelector;

	MapEvents mapEvent;

	UILabel lblMission;
	UILabel lblMission2;

	float aniTime = 0.0f;
	bool bAniStart = false;
};