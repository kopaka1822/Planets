#pragma once
#include "Map.h"
#include "MapLoader.h"
#include <vector>

class LocalMap : public Map
{
public:
	LocalMap(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns,
		float width, float height, Map::GameType ty, std::vector<TeamID> clns);
	LocalMap(int nPlayers, int nPlanets, float mWi, float mHei, Map::GameType ty, std::vector<TeamID> clns);
	virtual ~LocalMap();

	virtual bool select(PointF center, float r2, TeamID team);
	virtual bool setTarget(PointF pt, TeamID team) override;
	virtual TeamID gameEnd() const override;
	virtual bool filterEntityType(TeamID team, MapObject::EntityType et) override;
protected:
	bool tryEntitySpawn(const PointF& c, const byte team, float r, MapObject::TargetType ttype,
		const PointF& target, int group, bool isSelected, float maxR, MapObject::EntityType entType);
	MapEntity* getEnemyEnt(const PointF& pt, const byte team);
	//initialization
	void loadMapComponents(const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns);
	void killEnts(const PointF& center, float radius, int damage, byte team);

protected:

	void updateEnts(const float dt);
	void attackNearby(MapEntity& curEnt);
};
