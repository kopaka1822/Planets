#pragma once
#include "GameState.h"
#include "../Game/Map.h"
#include "Database.h"
#include "MapDrawer.h"
#include "UIButtonPause.h"
#include "MiniMap.h"
#include "UIYNButton.h"
#include "UIButtonTexture.h"
#include "Settings.h"
#include "UIIngamePlayerList.h"
#include "UIButtonTextureBorder.h"
#include "UIButtonText.h"


extern float MAP_AI;
extern float MAPU_PLANS;
extern float MAPU_ENTS;
extern float MAPU_PRIM_VEL;
extern float MAPU_CROWD;
extern float MAPU_POS;
extern float MAPU_DIE;
extern float MAPU_ATTK;

extern unsigned int MAP_UNITS;

class StateGame : public GameState
{
	class SoundBox : public Map::EventReciever
	{
	public:
		SoundBox(int maxPlans, int myPlans, int Team);
		void Init(MapDrawer* pDrawer);
		virtual void Event_EntitySpawn(PlanetID pID, MapEntity& e) override;
		virtual void Event_EntityKilled(MapEntity& e) override;
		//virtual void Event_PlanetAttacked(PlanetID pID, const EntityID& eID){};

		virtual void Event_PlanetCaptured(PlanetID pID, byte newTeam, byte oldTeam, const MapEntity* culprit) override;
		void Update(int nPlans);
		virtual ~SoundBox(){}
		void Reset();
	private:
		unsigned int CalcSoundLvl();
	private:
		bool bDied = false;
		bool bSpawn = false;

		int maxPlans;
		int myPlans;
		byte myTeam;

		int lvl = 0;

		bool firstPlanet = false;

		MapDrawer* pMapDrawer = nullptr;
	};

	class Scoreboard : public Map::EventReciever
	{
	public:
		Scoreboard(int maxTeams);
		virtual void Event_EntitySpawn(PlanetID pID, MapEntity& e) override;
		virtual void Event_EntityKilled(MapEntity& e) override;
		//virtual void Event_PlanetAttacked(PlanetID pID, const EntityID& eID){};
		virtual void Event_PlanetCaptured(PlanetID pID, byte newTeam, byte oldTeam, const MapEntity* culprit) override;
		void SaveInDatabase(bool isWinner);
		bool IsWinner() const;
		void Update(float dt, const Map& map);
		void AddBranch(const Map& map);
		float GetGametime();
	private:
		void initVec(std::vector<unsigned int>& v);
	private:
		std::vector<unsigned int> entSpawned;
		std::vector<unsigned int> entKilled;
		std::vector<unsigned int> entLost;
		std::vector<unsigned int> planCaptured;
		std::vector<unsigned int> planLost;
		bool isWinner = false;
		const int maxTeams;

		std::vector< std::vector< unsigned int >> unitCounts;
		float recordTime = 0.0f;
		double sumTime = 0.0f; // double for better precicion when adding float
	};
public:
	StateGame(byte myTeam, byte maxTeams);
	virtual ~StateGame(){}

	virtual void Event_MouseDown(Input::MouseKey k, const PointF& pos) override;
	virtual void Event_MouseUp(Input::MouseKey k, const PointF& pos) override final;
	virtual void Event_GameKeyDown(Input::GameKey k, const PointF& p) override;
	virtual void Event_GameKeyUp(Input::GameKey k, const PointF& p) override;
	virtual void Event_MouseMove(const PointF& pos) override final;

	virtual void Event_Resize(const PointF& ul, const PointF& dim) override;
	virtual void Event_KeyDown(SDL_Scancode code) override;
	//positive = forwards | negative = backwards
	virtual void Event_MouseWheel(float amount, const PointF& pos) override;

protected:
	void DrawMap(Drawing& draw);
	void UpdateMap(float dt);
	void UpdateGameButtons();
	virtual void Event_UnitsSelected()
	{}
	virtual void Event_EntityTypeChanged()
	{}
	void SetMapInput(bool b)
	{
		bMapInput = b;
	}
	void SetEntitySelector(bool b)
	{
		bEntitySelector = b;
		if (!bEntitySelector)
			pMapDraw->DisableEntSelect();
	}
private:
	void HandleMouseClick(bool bInside);
	void SetTarget(const PointF& p);
	void AlignButtons(PointF start, const PointF& dim, float dist, const std::initializer_list< UIObject* >& btns);
	void SetMiniMap(bool active);
	float GetCurGametime();
protected:
	std::unique_ptr<MapDrawer> pMapDraw;
	GameStruct& gs;

	UIButtonText
		btnSA,
		btnSN,
		btnChat,
		btnPlayer;
	UITooltip
		tipSA,
		tipSN,
		tipChat,
		tipPlayer,
		tipPause,
		tipMap,
		tipEnt,
		tipBomb,
		tipSpeed,
		tipSabo,
		tipDefPlan;

	UIButtonTextureBorder
		btnFilterEnt,
		btnFilterBomb,
		btnFilterSpeed,
		btnFilterSabo,
		btnDefPlanets;


	RectF ctrlBar;
	const byte myTeam;
	UIIngamePlayerList tablist;
private:
	std::unique_ptr<MiniMap> pMiniMap;

	//Input handling
	bool bScolling = false; // mouse wheel
	bool bMouseDown = false;
	bool bAddGroupDown = false;
	bool bMakeGroupDown = false;

	PointF msDown;  //model space
	PointF msUp; //model space
	PointF msLast;  //model space

	PointF scollLast; //client space

	SoundBox sBox;
	Scoreboard score;

	UIButtonPause btnPause;
	UIYNButton btnMiniOnOff;

	bool bMapInput = true;
	bool bEntitySelector = true;
};
