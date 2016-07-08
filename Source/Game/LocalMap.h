#pragma once
#include "Map.h"
#include "MapLoader.h"
#include <vector>

class LocalMap : public Map
{
public:
	LocalMap(int nPlayers, const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns,
		float width, float height, Map::GameType ty, std::vector<byte> clns);
	LocalMap(int nPlayers, int nPlanets, float mWi, float mHei, Map::GameType ty, std::vector<byte> clns);
	virtual ~LocalMap();

	virtual bool Select(PointF center, float r2, byte team);
	virtual bool Click(PointF pt, byte team) override;
	virtual byte GameEnd() const override;
	virtual bool FilterEntityType(byte team, MapObject::entityType et) override;
protected:
	bool TryEntitySpawn(const PointF& c, const byte team, float r, MapObject::targetType ttype,
		const PointF& target, int group, bool isSelected, float maxR, MapObject::entityType entType);
	MapEntity* GetEnemyEnt(const PointF& pt, const byte team);
	//initialization
	void LoadMapComponents(const std::vector<MapLoader::MapPlanet>& planets, const std::vector<MapLoader::MapSpawn>& spawns);
	void KillEnts(const PointF& center, float radius, int damage, byte team);

protected:

	void UpdateEnts(const float dt);
	void AttackNearby(MapEntity& curEnt);
};
