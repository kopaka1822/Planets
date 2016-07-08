#pragma once
#include "LocalMapSingleplayer.h"

// class for tutorial 1 (only default units)
class WeakAI : public LocalMapSingleplayer::AI
{
public:
	WeakAI(int team, Map& map);
	virtual void DoTurn(float dt) override;
private:
	PointF GetTarget();
	PointF GetNearestPlan(PointF pos);
	PointF GetNearestEnt();
private:
	const int myTeam;
	Map& map;

	float sumTime = 0.0f;
};

class LocalMapClassicMode : public LocalMapSingleplayer
{
public:
	LocalMapClassicMode(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns,
		float width, float height, Map::GameType ty, std::vector<byte> clns);
	virtual ~LocalMapClassicMode()
	{}

	virtual std::unique_ptr<AI> SpawnAI(int team) override;
};