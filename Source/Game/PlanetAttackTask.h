#pragma once
#include "Map.h"

class PlanetAttackTask
{
public:
	PlanetAttackTask(class PlayerAI& ai, PlanetID p);
	PlanetAttackTask& operator=(const PlanetAttackTask&) = delete;
	void Destroy();
	int GetGroupID()
	{
		return gid;
	}
	void Update(); // reselect
	void AddToGroup(int size)
	{
		assert(-size <= (int)groupsize);
		groupsize += size;
	}
	size_t GetGroupSize() const
	{
		return groupsize;
	}
public:
	static const int ID_START = 256;
	static const int ID_END = 511;
private:
	class PlayerAI& ai;
	MapPlanet& plan;
	const int gid; // m_group id
	size_t groupsize = 0;
};