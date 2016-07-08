#pragma once
#include "Map.h"
#include <list>
#include <functional>
#include <float.h>
#include "../Utility/Mutex.h"
#include <vector>
#include "PlanetTask.h"
#include "PlanetAttackTask.h"
#include "HuntTask.h"
#include "BomberHuntTask.h"
#include "LocalMapSingleplayer.h"

class PlayerAI : public LocalMapSingleplayer::AI
{
public:
	PlayerAI(byte team, Map& map);
	virtual ~PlayerAI();
	virtual void DoTurn(float dt) override;

	size_t AttackPlanet(int group, size_t num, PlanetID target);
	size_t HelpPlanet(int group, size_t num, PlanetID target);
	// returns own if no planet is in trouble
	PlanetID GetPlanetInTrouble(PlanetID own);
	size_t CountUnits() const
	{
		return map.ents[team - 1].length();
	}
	size_t CountPlanets() const
	{
		return nPlanets;
	}
	PlanetID GetWhitePlan(PlanetID own);
	PlanetID GetWellDefendedNear(PlanetID pID);

	// return point of accumulation in map coordinates on success
	// returns (-1,-1) if no nearby match was found
	PointI GetEnemyAccumulation(const PointF& pos, float r, size_t lowerLimit);
	size_t GetAlliEntsInDefR(PlanetID pID, byte team) const;
	size_t GetEnemyEntsInDefR(PlanetID pID, byte team) const;
	size_t GetOwnEntsInDefR(PlanetID pID, byte team) const;
	size_t GetAlliEntsInR(PointF pos, byte team, float r) const;
	size_t GetEnemyEntsInR(PointF pos, byte team, float r) const;
	size_t GetOwnEntsInR(PointF pos, byte team, float r) const;
	const FastVector<MapEntity*>& GetEntitiesGrid(PointF pos) const;
	PointF GetMidPlanetPos(byte team);
	PlanetID GetAttackPlan(PointF entPos) const;
	float CalcNearByDistance(int average);

	// nullptr if nothing was found
	MapEntity* GetBomberTarget(const MapEntity& bomber);
	MapEntity* GetHuntTarget(const PointF& center, float radius);
protected:
	virtual void Event_EntitySpawn(PlanetID pID, MapEntity& e)override;
	virtual void Event_EntityKilled(MapEntity& e)override;
	virtual void Event_PlanetAttacked(PlanetID pID, const MapEntity& e)override;
	virtual void Event_PlanetCaptured(PlanetID pID, byte newTeam, byte oldTeam, const MapEntity*)override;
private:
	size_t GroupTransfer(int from, int to, size_t num);
	void AddHunter(MapEntity& e);
public:
	Map& map;
	const byte team;
	static const size_t BOMBER_SEARCH_RADIUS = 350;
private:
	std::vector< PlanetTask* > planTask;
	std::vector< PlanetAttackTask* > planAtkTask;

	std::list< std::unique_ptr< HuntTask >> huntTask;
	std::list< std::unique_ptr< BomberHuntTask >> bombHunters;
	
	Mutex muEvent;

	std::vector< std::vector< PlanetID >> nearbyPlanets; // nearbyPlanets[ 0 ] all planets near planet 0
	size_t nPlanets;

	// constants
	static const size_t BOMBER_SEND_SIZE = 25;
};