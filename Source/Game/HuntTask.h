#pragma once
#include "Map.h"

// hunting with speeder

class HuntTask
{
public:
	HuntTask(class PlayerAI& ai, MapEntity& startUnit);
	void DoTask(float dt);
	void SetTarget(const MapEntity* target);
	void AddToGroup(int s)
	{
		groupSize += s;
	}
	void IncGroupSize()
	{
		groupSize++;
	}
	void DecGroupSize()
	{
		groupSize--;
	}
	int GetGroupSize() const
	{
		return groupSize;
	}
	const PointF& GetPos() const
	{
		return curPos;
	}
	int GetID() const
	{
		return id;
	}
	const MapEntity* GetTarget() const
	{
		return pTarget;
	}
public:
	static const int ID_START = 512;
private:
	class PlayerAI& ai;
	const size_t id;
	size_t groupSize;
	const MapEntity* pTarget;

	float timeout = 0.0f;

	PointF curPos; // probably the current center of all units
	const float RAD_MIN = 200.0f;
	const float RAD_STEP = 100.0f;
	float curRadius = RAD_MIN;
};
