#include "LocalMapLazyMode.h"

LocalMapLazyMode::LocalMapLazyMode(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns,
	float width, float height, Map::GameType ty, std::vector<TeamID> clns)
	:
	LocalMapSingleplayer(nPlayers,planets,spawns,width,height,ty,clns,false)
{
	InitAI();
}

std::unique_ptr<LocalMapSingleplayer::AI> LocalMapLazyMode::SpawnAI(int team)
{
	return std::unique_ptr<AI>(new LazyAI());
}