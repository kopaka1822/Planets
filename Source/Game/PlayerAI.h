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
	virtual void doTurn(float dt) override;

	size_t attackPlanet(int group, size_t num, PlanetID target);
	size_t helpPlanet(int group, size_t num, PlanetID target);
	// returns own if no planet is in trouble
	PlanetID getPlanetInTrouble(PlanetID own);
	size_t countUnits() const
	{
		return m_map.m_ents[m_team - 1].length();
	}
	size_t countPlanets() const
	{
		return m_nPlanets;
	}
	PlanetID getWhitePlan(PlanetID own);
	PlanetID getWellDefendedNear(PlanetID pID);

	// return point of accumulation in map coordinates on success
	// returns (-1,-1) if no nearby match was found
	PointI getEnemyAccumulation(const PointF& pos, float r, size_t lowerLimit);
	size_t getAlliEntsInDefR(PlanetID pID, byte team) const;
	size_t getEnemyEntsInDefR(PlanetID pID, byte team) const;
	size_t getOwnEntsInDefR(PlanetID pID, byte team) const;
	size_t getAlliEntsInR(PointF pos, byte team, float r) const;
	size_t getEnemyEntsInR(PointF pos, byte team, float r) const;
	size_t getOwnEntsInR(PointF pos, byte team, float r) const;
	const FastVector<MapEntity*>& getEntitiesGrid(PointF pos) const;
	PointF getMidPlanetPos(byte team);
	PlanetID getAttackPlan(PointF entPos) const;
	float calcNearByDistance(int average);

	// nullptr if nothing was found
	MapEntity* getBomberTarget(const MapEntity& bomber);
	MapEntity* getHuntTarget(const PointF& center, float radius);
protected:
	virtual void Event_EntitySpawn(PlanetID pID, MapEntity& e)override;
	virtual void Event_EntityKilled(MapEntity& e)override;
	virtual void Event_PlanetAttacked(PlanetID pID, const MapEntity& e)override;
	virtual void Event_PlanetCaptured(PlanetID pID, TeamID newTeam, TeamID oldTeam, const MapEntity*)override;
private:
	size_t groupTransfer(int from, int to, size_t num);
	void addHunter(MapEntity& e);
public:
	Map& m_map;
	const byte m_team;
	static const size_t BOMBER_SEARCH_RADIUS = 350;
private:
	std::vector< PlanetTask* > m_planTask;
	std::vector< PlanetAttackTask* > m_planAtkTask;

	std::list< std::unique_ptr< HuntTask >> m_huntTask;
	std::list< std::unique_ptr< BomberHuntTask >> m_bombHunters;
	
	Mutex m_muEvent;

	std::vector< std::vector< PlanetID >> m_nearbyPlanets; // nearbyPlanets[ 0 ] all planets near planet 0
	size_t m_nPlanets;

	// constants
	static const size_t BOMBER_SEND_SIZE = 25;
};