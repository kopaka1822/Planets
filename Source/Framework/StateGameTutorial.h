#pragma once
#include "StateGameSingle.h"
#include "UITutorialBox.h"
#include "UIButtonDetect.h"

class StateGameTutorial : public StateGameSingle
{
	enum class Stage
	{
		welcome,
		zooming,
		welcome2,
		dragging,
		welcome3,
		firstMove,
		whitePlan,
		secondMove,
		defendPlan,
		thirdMove
	};
	class MapEvents : public Map::EventReciever
	{
	public:
		MapEvents(UITutorialBox& boxCapture)
			: 
			boxCapture(boxCapture)
		{}
		virtual void Event_EntitySpawn(PlanetID pID, MapEntity& e) override{};
		virtual void Event_EntityKilled(MapEntity& e) override{};
		virtual void Event_PlanetAttacked(PlanetID pID, const MapEntity& e) override{};

		// culprit may be null if it was not captured by an entity
		virtual void Event_PlanetCaptured(PlanetID pID, TeamID newTeam, TeamID oldTeam, const MapEntity* culprit) override
		{
			if (newTeam == 1)
			{
				boxCapture.Activate();
			}
		}
	private:
		UITutorialBox& boxCapture;
	};
public:
	StateGameTutorial()
		:
		StateGameSingle(),
		mapEvents(boxDefend),
		lblMission("",FONT_SMALL),
		lblMission2("",FONT_SMALL)
	{
		boxWelcome.AddLine("Welcome to Planets!");
		boxWelcome.AddLine("This tutorial will show");
		boxWelcome.AddLine("you how to play.");
		boxWelcome.AddLine("");
		boxWelcome.AddLine("Tap anywhere to continue.");
		boxWelcome.EnablePlanet(Color::GetTeamColor(1));

		box1.AddLine("You are the red player.");
		box1.AddLine("");
		box1.AddLine("Your goal is to destroy");
		box1.AddLine("the blue player.");
		box1.EnablePlanet(Color::GetTeamColor(2));

		box2.AddLine("You can zoom in and out");
		box2.AddLine("by pinching the screen or");
		box2.AddLine("turning the mouse wheel");
		box2.SetTexture(&Database::GetTexture(Database::zoom));

		box3.AddLine("To look around, drag two fingers");
		box3.AddLine("around the screen or press and");
		box3.AddLine("hold the middle mouse button");
		box3.SetTexture(&Database::GetTexture(Database::drag));
		
		box4.AddLine("This is your planet. It is");
		box4.AddLine("spawning units, your primary");
		box4.AddLine("resource in the game.");
		box4.EnablePlanet(Color::GetTeamColor(1));

		box5.AddLine("Now it's time to move your units!");
		box5.AddLine("To select your units draw");
		box5.AddLine("a circle around them. Use");
		std::string selectKey = UIButtonDetect::KeyToString(Settings::GetGamekey(Input::GameKey::GK_SELECT));
		box5.AddLine("your finger or " + selectKey);
		box5.AddLine("to do this");
		box5.SetTexture(&Database::GetTexture(Database::swipe));

		boxWhitePlan.AddLine("Good Job!");
		boxWhitePlan.AddLine("Now you can tap on any");
		boxWhitePlan.AddLine("location to move them");
		boxWhitePlan.AddLine("");
		std::string targetKey = UIButtonDetect::KeyToString(Settings::GetGamekey(Input::GameKey::GK_CLICK));
		boxWhitePlan.AddLine("Use your finger or");
		boxWhitePlan.AddLine(targetKey + " to do this.");

		boxWhitePlan2.AddLine("Your untis will attack nearby");
		boxWhitePlan2.AddLine("planets and enemy units.");
		boxWhitePlan2.AddLine("White planets will become");
		boxWhitePlan2.AddLine("your planets if you");
		boxWhitePlan2.AddLine("attack them.");
		boxWhitePlan2.EnablePlanet(Color::White());

		boxDefend.AddLine("Well Done!");
		boxDefend.AddLine("You can defend your planet by");
		boxDefend.AddLine("selecting units and then");
		boxDefend.AddLine("tapping on the planet.");
		boxDefend.EnablePlanet(Color::GetTeamColor(1));

		boxWelcome.Activate();
		boxWelcome.RegisterNext(&box1);
		box1.RegisterNext(&box2);
		//box2.RegisterNext(&box3);
		//box3.RegisterNext(&box4);
		box4.RegisterNext(&box5);

		boxWhitePlan.RegisterNext(&boxWhitePlan2);

		RegisterBox(boxDefend);
		RegisterBox(boxWhitePlan2);
		RegisterBox(boxWhitePlan);
		RegisterBox(box5);
		RegisterBox(box4);
		RegisterBox(box3);
		RegisterBox(box2);
		RegisterBox(box1);
		RegisterBox(boxWelcome);

		gs.pMap->addEventReciever(&mapEvents);

		lblMission.setOrigin(PointF(10, 10));
		lblMission2.setOrigin(PointF(10, 10) + PointF(0, lblMission.getMetrics().y));

		SetEntitySelector(false);
	}
	virtual ~StateGameTutorial()
	{}
	virtual void ComposeFrame(Drawing& draw) override
	{
		StateGameSingle::ComposeFrame(draw);

		lblMission.draw(draw);
		lblMission2.draw(draw);

		boxWelcome.draw(draw);
		box1.draw(draw);
		box2.draw(draw);
		box3.draw(draw);
		box4.draw(draw);
		box5.draw(draw);

		boxWhitePlan.draw(draw);
		boxWhitePlan2.draw(draw);

		boxDefend.draw(draw);
	}
	virtual void ExecuteCode(float dt) override final
	{
		switch (curStage)
		{
		case Stage::welcome:
			UpdateGameButtons();
			//if (box5.isDone())
				//curStage = Stage::firstMove;
			if (box2.isDone())
				curStage = Stage::zooming;
			break;

		case Stage::zooming:
			SetMission("zoom by pinching the screen or turning the mouse wheel.   Press any key if you can't do this");

			UpdateGameButtons();
			if (bZoomed)
				zoomTime += dt;

			if (zoomTime > 2.0f)
			{
				box3.Activate();
				curStage = Stage::welcome2;
			}
			break;

		case Stage::welcome2:
			SetMission("");

			UpdateGameButtons();
			if (box3.isDone())
				curStage = Stage::dragging;
			break;

		case Stage::dragging:
			SetMission("look around by dragging two fingers around the screen or holding the middle mouse button. Press any key if you can't do this");

			UpdateGameButtons();
			if (bDragged)
				dragTime += dt;
			if (dragTime > 4.0f)
			{
				box4.Activate();
				curStage = Stage::welcome3;
			}
			break;

		case Stage::welcome3:
			SetMission("");

			UpdateGameButtons();
			if (box5.isDone())
				curStage = Stage::firstMove;
			break;

		case Stage::whitePlan:
			SetMission("");
			UpdateGameButtons();
			if (boxWhitePlan2.isDone())
				curStage = Stage::secondMove;
			break;

		case Stage::defendPlan:
			SetMission("");
			UpdateGameButtons();
			if (boxDefend.isDone())
				curStage = Stage::thirdMove;
			break;

		case Stage::secondMove:
			SetMission("attack a white planet");

			if (boxDefend.isActive())
				curStage = Stage::defendPlan;

			UpdateMap(dt);
			UpdateGameButtons();
			break;

		case Stage::thirdMove:
			SetMission("destroy the blue player");
			UpdateMap(dt);
			UpdateGameButtons();
			break;

		case Stage::firstMove: // select
			SetMission("select the red units by drawing a circle around them");
			UpdateGameButtons();
			break;
		}

		switch (curStage)
		{
		case Stage::firstMove:
		case Stage::secondMove:
		case Stage::thirdMove:
			SetMapInput(true);
			break;
		default:
			SetMapInput(false);
		}
		//UpdateMap(dt);


		// end
		if (isFinished())
		{
			SetNextState(states::Tutorial);
		}
	}
protected:
	virtual void Event_UnitsSelected()
	{
		if (curStage == Stage::firstMove)
		{
			boxWhitePlan.Activate();
			curStage = Stage::whitePlan;
		}
	}
	virtual void Event_MouseWheel(float amount, const PointF& pos) override
	{
		if (curStage == Stage::zooming)
		{
			bZoomed = true;
		}
		StateGame::Event_MouseWheel(amount, pos);
	}
	virtual void Event_MouseDown(Input::MouseKey k, const PointF& pos) override
	{
		if (curStage == Stage::dragging && k == Input::Middle)
		{
			bDragged = true;
		}

		StateGame::Event_MouseDown(k, pos);
	}
	virtual void Event_KeyDown(SDL_Scancode k) override
	{
		if (curStage == Stage::dragging)
		{
			bDragged = true;
			dragTime = 1000.0f;
		}
		else if (curStage == Stage::zooming)
		{
			bZoomed = true;
			zoomTime = 1000.0f;
		}

		StateGame::Event_KeyDown(k);
	}
private:
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
			int maxlen = int((Framework::STD_DRAW_X - 20) / FONT_SMALL.GetFontWidth());
			if (mis.length() > size_t(maxlen))
			{
				lblMission.SetText(all.substr(0, maxlen - 1));
				lblMission2.SetText(all.substr(maxlen - 1));
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
	Stage curStage = Stage::welcome;
	UITutorialBox boxWelcome;
	UITutorialBox box1;
	UITutorialBox box2;
	UITutorialBox box3;
	UITutorialBox box4;
	UITutorialBox box5;

	UITutorialBox boxWhitePlan;
	UITutorialBox boxWhitePlan2;

	UITutorialBox boxDefend;

	MapEvents mapEvents;

	UILabel lblMission;
	UILabel lblMission2;

	bool bZoomed = false;
	float zoomTime = 0.0f;

	bool bDragged = false;
	float dragTime = 0.0f;
};