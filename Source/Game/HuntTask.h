#pragma once
#include "Map.h"

// hunting with speeder

class HuntTask
{
public:
	HuntTask(class PlayerAI& ai, MapEntity& startUnit);
	void doTask(float dt);
	void setTarget(const MapEntity* target);
	void addToGroup(int s)
	{
		m_groupSize += s;
	}
	void incGroupSize()
	{
		m_groupSize++;
	}
	void decGroupSize()
	{
		m_groupSize--;
	}
	int getGroupSize() const
	{
		return m_groupSize;
	}
	const PointF& getPos() const
	{
		return m_curPos;
	}
	int getID() const
	{
		return m_id;
	}
	const MapEntity* getTarget() const
	{
		return m_pTarget;
	}
public:
	static const int ID_START = 512;
private:
	class PlayerAI& m_ai;
	const size_t m_id;
	size_t m_groupSize;
	const MapEntity* m_pTarget;

	float m_timeout = 0.0f;

	PointF m_curPos; // probably the current center of all units
	const float RAD_MIN = 200.0f;
	const float RAD_STEP = 100.0f;
	float m_curRadius = RAD_MIN;
};
