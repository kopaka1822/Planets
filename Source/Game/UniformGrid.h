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
	void addEntity(MapEntity* itm, const PointI& pos);
	FastVector<MapEntity*>& getEntities(PointI pos);
	FastVector<MapEntity*>& getEntitiesRaw(const PointI& box);
	int getBoxSize() const;

	// AI access

	// own m_team
	size_t countUnits(byte team, PointF pos, float radius);

	// own m_team + ally
	size_t countAllyUnits(byte team, PointF pos, float radius);

	// enemies (no allies)
	size_t countEnemyUnits(byte team, PointF pos, float radius);

	// own units (pos in map coord-system)
	size_t countUnitsInBox(byte team, PointF pos);
	// own units (pos in map grid-system)
	size_t countUnitsInBoxRaw(byte team, PointI posi);

	// return point of accumulation in map coordinates on success
	// returns (-1,-1) if no nearby match was found
	PointI getEnemyAccumulationPoint(byte team, const PointF& pos, float radius, size_t lowerLimit);
private:
	const int m_width, m_height,
		m_boxsz,// boxsize
		m_toleran; // tolerance
	Element** m_field;
	const int m_nTeams;
	const class Map& m_map;
};
