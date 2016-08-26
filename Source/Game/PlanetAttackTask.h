#pragma once
#include "Map.h"

class PlanetAttackTask
{
public:
	PlanetAttackTask(class PlayerAI& ai, PlanetID p);
	PlanetAttackTask& operator=(const PlanetAttackTask&) = delete;
	void destroy();
	int getGroupID()
	{
		return m_gid;
	}
	void update(); // reselect
	void addToGroup(int size)
	{
		assert(-size <= (int)m_groupsize);
		m_groupsize += size;
	}
	size_t getGroupSize() const
	{
		return m_groupsize;
	}
public:
	static const int ID_START = 256;
	static const int ID_END = 511;
private:
	class PlayerAI& m_ai;
	MapPlanet& m_plan;
	const int m_gid; // m_group id
	size_t m_groupsize = 0;
};