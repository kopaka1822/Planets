#pragma once
#include "LocalMapSingleplayer.h"

// class for tutorial 1 (only default units)
class WeakAI : public LocalMapSingleplayer::AI
{
public:
	WeakAI(int team, Map& map);
	virtual void doTurn(float dt) override;
private:
	PointF GetTarget();
	PointF GetNearestPlan(PointF pos);
	PointF GetNearestEnt();
private:
	const int m_myTeam;
	Map& m_map;

	float m_sumTime = 0.0f;
};

class LocalMapClassicMode : public LocalMapSingleplayer
{
public:
	LocalMapClassicMode(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns,
		float width, float height, Map::GameType ty, std::vector<TeamID> clns);
	virtual ~LocalMapClassicMode()
	{}

	virtual std::unique_ptr<AI> SpawnAI(int team) override;
};