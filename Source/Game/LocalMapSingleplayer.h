#pragma once
#include "LocalMap.h"
#include <memory>

class LocalMapSingleplayer : public LocalMap
{
public:
	class AI : public EventReciever
	{
	public:
		virtual ~AI(){}
		virtual void doTurn(float dt) = 0;
	};
public:
	LocalMapSingleplayer(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns,
		float width, float height, Map::GameType ty, std::vector<TeamID> clns, bool bInitAI = true);
	virtual ~LocalMapSingleplayer()
	{}

	virtual void update(float dt) override;
	virtual std::unique_ptr<AI> SpawnAI(int team);
protected:
	void InitAI();
private:
	std::vector< std::unique_ptr<AI> > pAI;
};