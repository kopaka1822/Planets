#pragma once
#include "Map.h"

class PlanetTask
{
public:
	PlanetTask(class PlayerAI& ai, PlanetID pid);
	PlanetTask& operator=(const PlanetTask&) = delete;
	void doTask(float dt);
	void incGroupSize(MapObject::EntityType et)
	{
		if (et != MapObject::etNormal)
		{
			m_plan.setEntityType(MapObject::etNormal);
			PointF tar;
			tar.x = (float)m_plan.getID();
			m_plan.setTarget(tar, MapObject::tgPlanetDefend);
		}
		m_groupSize++;
		m_bEvent = true;
	}
	void decGroupSize()
	{
		m_groupSize--;
		m_bEvent = true;
	}
	void addToGroup(int s)
	{
		m_groupSize += s;
		m_bEvent = true;
	}
	void destroy();
	// reselect untis and assign standart task
	void update();
	bool isInTrouble() const
	{
		return m_bInTrouble;
	}
	size_t getGroupSize()
	{
		return m_groupSize;
	}
public:
	static const int ID_START = 0;
	static const int ID_END = 255;
private:
	static const int MAX_ATTACK_SIZE = 20;
	static const int MAX_DEF_SIZE = 40;
private:
	PlanetID getAttackPlan() const;
	PlanetID getNearestFreePlan() const;
	size_t getSplitSize() const;
	int getAttackSize(PlanetID pid) const;
private:
	class PlayerAI& m_ai;
	MapPlanet& m_plan;
	size_t m_groupSize = 0;
	bool m_bInTrouble = false;

	float m_sumTime = 0.0f;
	bool m_bEvent = false;
};