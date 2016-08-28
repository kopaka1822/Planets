#pragma once
#include "LocalMapSingleplayer.h"

class LazyAI : public LocalMapSingleplayer::AI
{
public:
	LazyAI()
	{}
	virtual ~LazyAI()
	{}
	virtual void doTurn(float dt) override
	{}
};
// ai will do nothing
class LocalMapLazyMode : public LocalMapSingleplayer
{
public:
	LocalMapLazyMode(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns,
		float width, float height, Map::GameType ty, std::vector<TeamID> clns);
	virtual ~LocalMapLazyMode()
	{}

	virtual std::unique_ptr<AI> SpawnAI(int team) override;
};