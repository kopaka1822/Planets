#pragma once
#include "../Utility/FastVector.h"
#include "../Utility/Point.h"
#include <algorithm>
#include "MapEntity.h"
#include "../Utility/FastVector.h"
#include "../Utility/Tools.h"

class UniformGrid
{
	class Element
	{
	public:
		FastVector<MapEntity*> ents;
		FastVector<size_t> teamsz;
	};
public:
	UniformGrid(int mapX, int mapY, int boxSize, int boxTolerance, int nTeams, const class Map& map);
	UniformGrid& operator=(UniformGrid&) = delete;
	~UniformGrid();
	void clear();
	void AddEntity(MapEntity* itm, const PointI& pos);
	FastVector<MapEntity*>& GetEntities(PointI pos);
	FastVector<MapEntity*>& GetEntitiesRaw(const PointI& box);
	int GetBoxSize() const;

	// AI access

	// own m_team
	size_t CountUnits(byte team, PointF pos, float radius);

	// own m_team + ally
	size_t CountAllyUnits(byte team, PointF pos, float radius);

	// enemies (no allies)
	size_t CountEnemyUnits(byte team, PointF pos, float radius);

	// own units (pos in map coord-system)
	size_t CountUnitsInBox(byte team, PointF pos);
	// own units (pos in map grid-system)
	size_t CountUnitsInBoxRaw(byte team, PointI posi);

	// return point of accumulation in map coordinates on success
	// returns (-1,-1) if no nearby match was found
	PointI GetEnemyAccumulationPoint(byte team, const PointF& pos, float radius, size_t lowerLimit);
private:
	const int width, height,
		boxsz,// boxsize
		toleran; // tolerance
	Element** field;
	const int nTeams;
	const class Map& map;
};
